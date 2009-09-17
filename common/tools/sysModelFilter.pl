#!perl -w

use strict;

use XML::Parser;

my $sysModel = shift or die "First arg must be system model xml file";
my @filters = @ARGV or die "Subsequent arg(s) must be filter(s) to apply";

# Sort out the positive and negative filters specified
my @negativeFilters;
my @positiveFilters;
foreach my $filter (@filters)
{
	if ($filter =~ m{^!(.*)})
	{
		push @negativeFilters, $1;
	}
	else
	{
		push @positiveFilters, $filter;
	}
}

# Read input tree
my $parser = new XML::Parser(Style => "Objects") or die;
my $sysTree = eval { $parser->parsefile($sysModel) } or die "Failed to parse $sysModel : $@";

# Apply filter
filterTree($sysTree->[0], \@negativeFilters, \@positiveFilters);

# Output total tree
print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
printTree($sysTree->[0]);
print "\n";

exit;

sub filterTree
{
	my $tree = shift;
	my $negativeFilters = shift;
	my $positiveFilters = shift;

	if (exists $tree->{filter})
	{
		# Work out how this item in the tree is tagged for filtering
		my $itemTags = [split ",", $tree->{filter}];
		my @negativeTags;
		my @positiveTags;
		foreach my $tag (@$itemTags)
		{
			if ($tag =~ m{^!(.*)})
			{
				push @negativeTags, $1;
			}
			else
			{
				push @positiveTags, $tag;
			}
		}

		# Test whether this item should be removed
		if (intersect(\@positiveTags, $negativeFilters) ||
		    intersect(\@negativeTags, $positiveFilters) )
		{
			# It should!
			# Return false, and the removal will be executed by the
			# calling instance
			# (No need to examine children)
			return 0;
		}

		# Tidy up the filter attribute
		# Remove tags from this item that have been "used up"
		# Remove the filter attribute entirely if they have all been
		# used up
		my %filterLookup = map { $_ => 1 } (@$negativeFilters, @$positiveFilters);
		@$itemTags = grep { !exists $filterLookup{$_} } @$itemTags;
		if (scalar @$itemTags)
		{
			$tree->{filter} = join ",", @$itemTags;
		}
		else
		{
			delete $tree->{filter};
		}
	}

	# Now iterate through the children of this item and remove any that
	# should be filtered out
	@{$tree->{Kids}} = grep {
		filterTree($_, $negativeFilters, $positiveFilters)
	} @{$tree->{Kids}};

	# Return true so that the calling instance will preserve this item in
	# the output tree
	return 1;
}

# Test whether two sets (arrays) intersect
sub intersect
{
	my $set1 = shift;
	my $set2 = shift;

	my %set1 = map { $_ => 1 } @$set1;
	foreach (@$set2)
	{
		return 1 if exists $set1{$_};
	}
	return 0;
}

sub printTree
{
	my $tree = shift or die;
	die unless ref $tree;

	my $tagName = ref $tree;
	$tagName =~ s{^main::}{};
	if ($tagName eq "Characters")
	{
		print $tree->{Text};
		return;
	}
	
	print "<$tagName";

	foreach my $attr (
		sort {
			my $order = "unofficial name long-name tech_domain level span schema levels filter introduced deprecated purpose class plugin origin-model bldFile mrp version priority";
			my $ixA = 1 + index $order, $a or die $a;
			my $ixB = 1 + index $order, $b or die $b;
			$ixA - $ixB;
		}
		grep {
			! ref $tree->{$_}
		}
		keys %$tree)
	{
		print " $attr=\"$tree->{$attr}\"";
	}

	my $children = $tree->{Kids};
	if (scalar @$children)
	{
		print ">";
		foreach my $child (@$children)
		{
			printTree($child);
		}
		print "</$tagName";
	}
	else
	{
		print "/"
	}

	print ">";
}

