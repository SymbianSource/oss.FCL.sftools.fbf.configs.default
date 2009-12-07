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

package XML::Printer;

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
			# TODO: This is hard coded for the case of a BRAG XML merge - ought to be passed in as a parameter
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

