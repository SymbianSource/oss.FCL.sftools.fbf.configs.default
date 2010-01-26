#!perl -w
#
# Copyright (c) 2010 Symbian Foundation Ltd
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
# Find and output the drive letter mapped to the physical volume with the
# largest amount of free space
# 

use strict;

# Use Windows command to list physical volumes on the machine
# (No substed drives, or mapped network drives)
my @drives = map {chomp;$_} `echo list volume | diskpart`;

my %drives;
for my $driveLine (@drives)
{
	# If this line of output is actually about a healthy HD volume...
	if ($driveLine =~ m{^\s+Volume \d+\s+([A-Z]).*?(Partition|RAID-5)\s+\d+ [A-Z]+\s+Healthy} )
	{
		my $letter = $1;
		# Ignore the system drive
		next if ($driveLine =~ m{System\s*$});

		# Use dir to get the freespace (bytes)
		my @bytesFree = grep { s{^.*?(\d+) bytes free\s*$}{$1} } map {chomp;$_} `cmd /c dir /-C $letter:\\`;
		# Take the value from the bottom of the report
		my $bytesFree = $bytesFree[-1];

		# Record info for this volume
		$drives{$letter} = $bytesFree;
	}
}

die "Unable to find any suitable drives at all\n" unless %drives;

# Switch keys and values
%drives = reverse %drives;
# Sort by space to find the volume with the largest amount of space and print out the corresponding letter
print "$drives{(reverse sort keys %drives)[0]}:\n";

