#!perl -w

use strict;

use XML::Parser;
use Data::Dumper;
use Text::CSV;

my $sourcesCSV = shift or die "First arg must be source csv file";
shift and die "No more than one argument please";

# Load CSV
open my $csvText, "<", $sourcesCSV or die;
my $csv = Text::CSV->new();
my @keys;
my @packages;
while (my $line = <$csvText>)
{
	chomp $line;
	next unless $line;
	unless ($csv->parse($line))
	{
		my $err = $csv->error_input();
		die "Failed to parse line '$line': $err";
	}

	if (! @keys)
	{
		# First line - note the column names
		@keys =  $csv->fields();
	}
	else
	{
		# Already got the keys, so get the data
		my %package;
		# Read into a hash slice
		@package{@keys} = $csv->fields();
		push @packages, \%package;
	}
}
close $csvText;

my $parser = new XML::Parser(Style => "Objects") or die;
my $outTree;

# For each package in CSV...
foreach my $package (@packages)
{
	warn "Warning: Package $package->{dst} does not appear on the local system\n" unless -d $package->{dst};
	# Look for the pkg defn in the root of the package tree
	my $pkgDef = "$package->{dst}/$package->{sysdef}";
	if (!-f $pkgDef)
	{
		# Not there, so try the "backup" location
		$pkgDef =~ s{^/sf/}{};
		$pkgDef =~ s{/[^/]*$}{};
		# TODO: Where will this be on the build machine?
		$pkgDef = "./packages/3k/$pkgDef/package_definition.xml";
	}
	die unless -f $pkgDef;

	my $pkgTree = $parser->parsefile($pkgDef) or die;
	if (!$outTree)
	{
		# The first file is taken verbatim
		$outTree = $pkgTree;
	}
	else
	{
		# Merge into output Tree
		mergeTrees($outTree->[0], $pkgTree->[0]);
	}
}

#print Data::Dumper->Dump([$outTree->[0]], ["tree"]);

# Output total tree
print "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
printTree($outTree->[0]);
print "\n";

sub mergeTrees
{
	my $baseTree = shift or die;
	my $extrasTree = shift or die;

	die unless ref $baseTree eq ref $extrasTree;
	return if ref $baseTree eq "main::Characters";

	foreach my $extraChild (@{$extrasTree->{Kids}})
	{
		# Work out whether this child should be merged with a namesake, or appended
		my $mergeIt = undef;

		my $extraChildTag = ref $extraChild;
		$extraChildTag =~ s{^main::}{};
		
		if ($extraChildTag =~ m{^(SystemDefinition|systemModel)$})
		{
			# Should be merged if there's already one there
#			warn "Always merge $extraChildTag";
			# Look for a namesake in the base
			$mergeIt = matchTag($baseTree->{Kids}, $extraChild, undef);
		}
		elsif ($extraChildTag =~ m{layer|block|package|collection|component})
		{
			# Should be merged if there is another tag with the same "name" attribute
#			warn "Sometimes merge $extraChildTag";
			# Look for a namesake in the base
			$mergeIt = matchTag($baseTree->{Kids}, $extraChild, "name");
		}

		if ($mergeIt)
		{
			# Merge children
			mergeTrees($mergeIt, $extraChild);
		}
		else
		{
			# Add this child
			push @{$baseTree->{Kids}}, $extraChild;
		}
	}
}

sub matchTag
{
	my $peers = shift;
	my $outsider = shift;
	my $attr = shift;

	foreach my $peer (@$peers)
	{
		if (ref $peer eq ref $outsider && (!defined $attr || $peer->{$attr} eq $outsider->{$attr}))
		{
			return $peer;
		}
	}
	
	return undef;
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

	foreach my $attr (grep { ! ref $tree->{$_} } keys %$tree)
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

