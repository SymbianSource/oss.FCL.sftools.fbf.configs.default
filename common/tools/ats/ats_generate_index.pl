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
#   This is a tool for generating the top-level index for BC test reports.

use strict;
use File::Copy;
use Tie::File;
use File::Find;

my @files;
my @lines;
sub Wanted;

my $path;
if ($ARGV[0]) {
  $path = $ARGV[0];
 }
else { die "Missing parameter \"path\". For example: //v800020/Publish/SF_builds/symbian2/builds/default/symbian2_default.sf-test-bc-check.PDK_2.0.1.51/ats_reports"; }

find(\&Wanted, $path);
my $n = 0;
foreach (@files) { #Replace "//v800020/Publish" with "http://cdn.symbian,org"
	@files[$n] =~ s/\/\/v800020\/Publish/http:\/\/cdn.symbian.org/;
	$n++;
}
#Copy template and insert links
copy("report_template.html","index.html") or die ("Cannot copy file \"report_template.html\". $!\n");
tie @lines, 'Tie::File', "index.html" or die ("Cannot tie file \"index.html\". $!\n");
my $current_line = 0;
for (@lines) {
	if (/<!-- Insert reports here -->/) {
		my $i = 0;
		foreach (@files) {
			splice @lines, $current_line+$i+1, 0, "<tr class=\"tableData\"><td align=\"center\">" . ($i+1) . "</td>" . "<td align=\"left\"><a href=\"" . @files[$i] . "\">" . @files[$i] . "</a></td></tr>"; 
			$i++;
		}
		last;
	}
	$current_line++;
}
untie @lines;
print @{files} . " link(s) added.\n";
#copy index
copy("index.html","$path/index.html") or die ("Cannot copy file \"index.html\" to $path. $!\n");

sub Wanted {
    # only operate on ATS3Report.html files
    /ATS3Report.html/ or return;	
	push (@files, $File::Find::name);
}