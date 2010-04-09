#!perl -w

use strict;

use FindBin;
use lib "$FindBin::Bin/lib";

use XML::Parser;
use Text::CSV;

my $sourcesCSV = shift or die "First arg must be source csv file";
my $backupBaseDir = shift or die "Second arg must be path to tree of package_definitions to use if not found in the source packages";
shift and die "No more than two arguments please";

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
	# If the sources.csv does not include a sys def for this package, it doesn't get built
	next unless $package->{sysdef};
	# If it's in the "backup" location, use that one (ie our copy overrides the package's own copy)
	my $pkgDef = "$package->{dst}/$package->{sysdef}";
	$pkgDef =~ s{^/sf/}{};
	$pkgDef =~ s{/[^/]*$}{};
	$pkgDef = "$backupBaseDir/$pkgDef/package_definition.xml";
	if (!-f $pkgDef)
	{
		# Not there, so look for the pkg defn in the root of the package tree
		warn "Warning: Package $package->{dst} does not appear on the local system\n" unless -d $package->{dst};
		$pkgDef = "$package->{dst}/$package->{sysdef}";
	}
	die "Unable to locate any package_definition at all for $package->{dst}" unless -f $pkgDef;

	warn "Including $pkgDef for $package->{dst}\n";
	my $pkgTree = eval { $parser->parsefile($pkgDef) } or die "Failed to parse $pkgDef : $@";
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

# Output total tree
print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
printTree($outTree->[0]);
print "\n";

exit;

sub mergeTrees
{
	my $baseTree = shift or die;
	my $extrasTree = shift or die;

	die ("Package Definitions do not match: ".(ref $baseTree)." vs ".(ref $extrasTree)) unless ref $baseTree eq ref $extrasTree;
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
			# Look for a namesake in the base
			$mergeIt = matchTag($baseTree->{Kids}, $extraChild, undef);
		}
		elsif ($extraChildTag =~ m{layer|block|package|collection|component})
		{
			# Should be merged if there is another tag with the same "name" attribute
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
		if ($tree->{Text} =~ m{[<>&]})
		{
			print "<![CDATA[$tree->{Text}]]>";
		}
		else
		{
			print $tree->{Text};
		}
		return;
	}
	
	print "<$tagName";

	foreach my $attr (
		sort {
			my $order = "name long-name tech_domain level span schema levels filter introduced deprecated purpose class plugin origin-model bldFile mrp version priority";
			my $ixA = index $order, $a;
			my $ixB = index $order, $b;
			die "$a $b" if $ixA + $ixB == -2;
			$ixA - $ixB;
		}
		grep {
			! ref $tree->{$_}
		}
		keys %$tree)
	{
		my $value_escaped = $tree->{$attr};
		$value_escaped =~ s/&/&amp;/g;
		$value_escaped =~ s/</&lt;/g;
		$value_escaped =~ s/>/&gt;/g;
		print " $attr=\"$value_escaped\"";
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

