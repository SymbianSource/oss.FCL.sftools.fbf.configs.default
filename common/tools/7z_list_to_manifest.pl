#!/usr/bin/perl

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
# Turn a 7z listing into a manifest file that can be re-used by 7z 
# - Assumes that header/footer is fixed, and that file listing starts at column 53
# Also allows use on a zipfile directly by passing -z as an option

use strict;

my $line;
my $between_header_footer = 0;
my $header_footer_pattern = "------------------- ----- ------------ ------------  ------------------------";

my $file   = shift or die "Usage: $0 <7z list output file> [-z to run directly on a zipfile]\n";   #  provided 7z list to process
my $mode   = shift;

# if -z has been specified, then generate intermediate 7z list file for processing
if ($mode eq lc("-z")) {
	system("7z l $file > $file.tmp.lst");
	$file = $file.".tmp.lst";
}

open my $sevenZfile, "<", $file or die;

while ($line =<$sevenZfile>)
{
	if ($line =~ /$header_footer_pattern/) 
	{
        	$between_header_footer= !$between_header_footer;

	}
	if (($line =~ /^(.{53})(.*)/) && ($line !=/$header_footer_pattern/))
	{
		print $2 ."\n" if $between_header_footer;
		next;
	}
}

close $sevenZfile;

# remove temp 7z file if one has been generated
if ($mode eq lc("-z")) {
	system("del $file");
}

exit 0;