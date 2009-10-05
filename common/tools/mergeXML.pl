#!perl -w
# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
# 
# Contributors:
#
# Description:
# Merge a set of XML files

use strict;

use XML::Parser;
use Getopt::Long;

# Read option arguments
my $howtoString;
my $xslLink;
my $help;
GetOptions((
	'xsl=s' => \$xslLink,
	'merge=s' => \$howtoString,
	'help!' => \$help,
));

my $wrongArgs = 0;
unless ($help)
{
	$wrongArgs += warn "No merge string specified to indicate how the files should be merged\n" unless defined $howtoString;
	$wrongArgs += warn "No files to be merged\n" unless scalar @ARGV;
}
if ($help || $wrongArgs)
{
	print <<"EOT";

mergeXML.pl --xsl=brag.xsl --merge=SystemDefinition,systemModel,layer(name),block(name),package(name) sysModel1.xml [model*.xml ...] > output.xml
EOT
	exit(0 + !$help);
}

# Hash of tags that should be merged, with optional attribute consideration
my $mergeTags;
foreach my $term (split m{\s*,\s*}, $howtoString)
{
	my ($tag, $attribute) = $term =~ m{(\w+)\((\w+)\)};
	$tag ||= $term;
	$mergeTags->{$tag} = $attribute;
}

# Expand wildcards
@ARGV = map { glob $_ } @ARGV;

# Merge all the trees together
my $outTree = mergeMultipleTrees($mergeTags, @ARGV);

# Output total tree
print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<?xml-stylesheet type=\"text/xsl\" href=\"$xslLink\"?>\n" if $xslLink;
printTree($outTree->[0]);
print "\n";

exit(0);

sub mergeMultipleTrees
{
	my $mergeTags = shift or die;

	# Create an XML parser
	my $parser = new XML::Parser(Style => "Objects") or die;
	
	my $outTree;
	# For each XML file to merge...
	foreach my $xmlFile (@_)
	{
		my $tree = eval { $parser->parsefile($xmlFile) } or die "Failed to parse $xmlFile : $@";
		if (!$outTree)
		{
			# The first file is taken verbatim
			$outTree = $tree;
		}
		else
		{
			# Merge into output Tree
			mergeTwoTrees($outTree->[0], $tree->[0], $mergeTags);
		}
	}

	return $outTree;
}

sub mergeTwoTrees
{
	my $baseTree = shift or die;
	my $extrasTree = shift or die;
	my $mergeTags = shift or die;

	die ("Trees do not match: ".(ref $baseTree)." vs ".(ref $extrasTree)) unless ref $baseTree eq ref $extrasTree;
	return if ref $baseTree eq "main::Characters";

	foreach my $extraChild (@{$extrasTree->{Kids}})
	{
		# Work out whether this child should be merged with a namesake, or appended
		my $mergeIt;

		my $extraChildTag = ref $extraChild;
		$extraChildTag =~ s{^main::}{};
		
		if (exists $mergeTags->{$extraChildTag})
		{
			# Should be merged if there's already one there
			# Look for a namesake in the base
			$mergeIt = matchTag($baseTree->{Kids}, $extraChild, $mergeTags->{$extraChildTag});
		}

		if ($mergeIt)
		{
			# Merge children
			mergeTwoTrees($mergeIt, $extraChild, $mergeTags);
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

	foreach my $attr (
		sort
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


