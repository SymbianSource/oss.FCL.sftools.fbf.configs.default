#! perl -w
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
# Compares two files

use strict;

my $element;
my @union = ();
my @intersection = ();
my @difference = ();
my %count = ();

my $file1 = shift or die "Usage: $0 file1 file2 | optional -I[ntersection]\n";
my $file2 = shift or die "Usage: $0 file1 file2 | optional -I[ntersection]\n";
my $mode  = shift;
open FILE1, "<$file1" or die "ERROR: Can't read $file1";
open FILE2, "<$file2" or die "ERROR: Can't read $file2";

my @file1_content = <FILE1>;
my @file2_content = <FILE2>;

close FILE1;
close FILE2;

print "* Comparing $file1 and $file2\n";
foreach $element (@file1_content, @file2_content) { $count{$element}++ }
foreach $element (keys %count) {
    push @union, $element;
    push @{ $count{$element} > 1 ? \@intersection : \@difference }, $element;
}

if (!defined $mode) {
	if (@difference > 0) {
		foreach (@difference){
			print $_;
		}
	} else {
		print "* Files are identical\n";
	}
} elsif ($mode eq "-I") {
	if (@intersection > 0) {
		foreach (@intersection){
			print $_;
		}
	}
} else {
	print "Usage: $0 file1 file2 | optional -I[ntersection]\n";
}
