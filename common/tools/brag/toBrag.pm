#!perl -w
#
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
# Functionality common to BRAG file generation

use strict;

package ToBrag;

# A useful constant
our $xmlNewline = bless { Text => "\n" }, "Characters";

sub createDocumentAndRoot
{
	my $rootTag = shift;

	my $root = bless
	{
		Kids =>
		[ $ToBrag::xmlNewline ]
	}, $rootTag;

	return [$root], $root;
}

sub createBuildStatus
{
	return [
		bless
		{
			Kids =>
			[ $ToBrag::xmlNewline ]
		}, "buildStatus"
	];
}

sub ensurePhase
{
	my $buildStatus = shift;
	my $phaseName = shift;

	my ($phase) = grep { ref $_ eq "phase" && $_->{name} eq $phaseName } @{$buildStatus->[-1]->{Kids}};
	unless ($phase)
	{
		$phase = bless
		{
			name => $phaseName,
			Kids => [ $ToBrag::xmlNewline ]
		}, "phase";
		push @{$buildStatus->[-1]->{Kids}}, $phase, $ToBrag::xmlNewline;
	}
	return $phase;
}

sub ensureStep
{
	my $phase = shift;
	my $stepName = shift;

	return ensureChild($phase, "step", "name", $stepName);
}

sub ensureFailureSet
{
	my $step = shift;
	my $level = shift;

	return ensureChild($step, "failures", "level", $level);
}

sub ensureChild
{
	my $parent = shift;
	my $childName = shift;
	my $childAttr = shift;
	my $childAttrValue = shift;

	my ($child) = grep { ref $_ eq $childName && $_->{$childAttr} eq $childAttrValue } @{$parent->{Kids}};
	unless ($child)
	{
		$child = bless
		{
			$childAttr => $childAttrValue,
			Kids => [ $ToBrag::xmlNewline ]
		}, $childName;
		push @{$parent->{Kids}}, $child, $ToBrag::xmlNewline;
	}
	return $child;
}

# Prints out the XML tree to STDOUT
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
			my $order = "name level start stop href package effect";
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
		print " $attr=\"$tree->{$attr}\"";
	}

	my $children = $tree->{Kids} || [];
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

1;

