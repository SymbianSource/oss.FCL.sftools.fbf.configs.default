#!/usr/bin/perl

# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#   Maciej Seroka, maciej@symbian.org
#
# Description:
#   This is a script for fixing pkg files.

use strict;
use File::Copy;
use Tie::File;
use File::Find;

my @files;
my @lines;
my $file;
my $n;
my $file_fixed;
sub Wanted;

my $package_path;
if ($ARGV[0]) {
  $package_path = $ARGV[0];
 }
else { die "Missing parameter \"package path\". For example: D:\\sf\\app\\musicplayer"; }

find(\&Wanted, $package_path);

#Copy a pkg file and replace \armv5\urel with $(platform)\$(target)
foreach $file (@files) { #Replace "//v800020/Publish" with "http://cdn.symbian,org"
	copy($file,$file . ".orig") or die ("Cannot copy file \"$file\". $!\n");
	tie (@lines, 'Tie::File', $file, recsep => "\n") or die ("Cannot tie file \"$file\". $!\n");
	$n = 0;
	$file_fixed = 0;
	foreach (@lines) {
		if (lc(@lines[$n]) =~ m/epoc32\\release\\armv5\\urel\\/) {
			@lines[$n] = lc(@lines[$n]);
			@lines[$n] =~ s/\\armv5\\urel\\/\\\$(platform)\\\$(target)\\/;
			$file_fixed = 1;
		}
		if (lc(@lines[$n]) =~ m/epoc32\\release\\armv5\\udeb\\/) {
			@lines[$n] = lc(@lines[$n]);
			@lines[$n] =~ s/\\armv5\\udeb\\/\\\$(platform)\\\$(target)\\/;
			$file_fixed = 1;
		}
		$n++;
	}
	if ($file_fixed) { print $file . " fixed.\n"; }
	untie @lines;
}

sub Wanted {
    # only operate on .pkg files
	/.pkg$/ or return;
    push (@files, $File::Find::name);
}