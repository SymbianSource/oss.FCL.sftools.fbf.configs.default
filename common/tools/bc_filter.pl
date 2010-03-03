#!/usr/bin/perl

# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#	Maciej Seroka, maciejs@symbian.org
#
# Description:
#   This is a tool for filtering static BC header reports.
#

use strict;
use XML::Simple;
use File::Copy;
use Tie::File;
use Data::Dumper;

my $report;
my $header_list;
my $destfile = "stripped_headers_report.xml";
my $del_ok_issues = 1; # This variable determines whether to delete OK issues first.
my $del_comp_issues = 0; # This variable determies whether to delete Compilation errors.
my $del_boost_issues = 1; # This variable determies whether to delete issues for Boost API headers.

my $n;
my $m;
my $file_name;
my $delete_node;

if ($ARGV[1]) {
	$report = $ARGV[0];
	$header_list = $ARGV[1];
} else { 
	die "Missing parameter(s). For example: bc_filter.pl headers_report.xml public_headers.txt"; 
}

# Parse the input XML into hashrefs.
print "Parsing " . $report . "... ";
my $current_report = XMLin("./$report", keeproot => 1,
    forcearray => [ 'header', 'baselineversion', 'currentversion', 'timestamp', 'day', 'month', 'year', 'hour', 'minute', 'second', #
	'haversion', 'formatversion', 'cmdlineparms', 'parm', 'pname', 'pvalue', 'knownissuesversion', 'os', 'version', 'buildweek', 'issuelist',#
	'headerfile', 'filename', 'comparefilename', 'status', 'comment', 'issue', 'checksum', 'shortname', 'issueid', 'typeid', 'identityid', #
	'identitydescription', 'typestring', 'cause', 'documentation', 'ignoreinformation', 'linenumber', 'severity', 'scseverity'], keyattr => [] );
print "complete \n";

# Load Public API definitions
my @lines;
tie @lines, 'Tie::File', $header_list or die ("Cannot tie file \"$header_list\". $!\n");

# Get number of header files
my $header_num = @{$current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}};
print "Number of all header files with issues: $header_num \n";

# Delete known issues.
if ($del_ok_issues) {
	$n = 0;
	while ($n < $header_num) {
		$file_name = $current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}->[$n]->{'shortname'}->[0];
		# Delete the node if known issue.
		if ($current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}->[$n]->{'status'}->[0] eq "OK") {
			print "Known issue: $file_name ...deleted\n";
			splice(@{$current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}},$n, 1);
			$header_num--;
		} else {
#			print "Unknown issue: $file_name \n";
			$n++;
		}
	}
	# Get number of header files again
	$header_num = @{$current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}};
	print "Number of remaining header files with issues: $header_num \n";
}

# Delete compilation issues.
# Assumption: Compilation issue is always the top issue (and probably the only one)
if ($del_comp_issues) {
	$n = 0;
	while ($n < $header_num) {
		$file_name = $current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}->[$n]->{'shortname'}->[0];
		if (($current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}->[$n]->{'issue'}->[0]->{'typestring'}->[0] eq "has compilation errors") && #
			($current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}->[$n]->{'status'}->[0] ne "OK")) { # Delete the node if compilation error.
			print "$file_name has compilation errors \n";
			splice(@{$current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}},$n, 1);
			$header_num--;
		} else {
			$n++;
		}
	}
	# Get number of header files again
	$header_num = @{$current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}};
	print "Number of header files with non-compilation issues: $header_num \n";
}

# Delete Boost API related issues (Boost API headers are not present in any of the Public SDK!).
if ($del_boost_issues) {
	$n = 0;
	while ($n < $header_num) {
		$file_name = $current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}->[$n]->{'shortname'}->[0];
		# Delete the node if Boost API header.
		if ($file_name =~ m/\\boost/) {
			print "Boost API: $file_name \n";
			splice(@{$current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}},$n, 1);
			$header_num--;
		} else {
			$n++;
		}
	}
	# Get number of header files again
	$header_num = @{$current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}};
	print "Number of non-Boost API header files: $header_num \n";
}

# Delete non-public API issues.
$n = 0;
while ($n < $header_num) {
	$file_name = $current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}->[$n]->{'shortname'}->[0];
	$m = 0;
	$delete_node = 1;
	foreach (@lines) { # Check against header list.
		if (lc($file_name) eq lc(@lines[$m])) {	# Mark the node to NOT be deleted.
			$delete_node = 0;
			last;
		}
		$m++;
	}
	# Delete the node if non-public issue.
	if ($delete_node) {
		print "Header file: $file_name not found in Public API definition file... deleted\n";
		splice(@{$current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}},$n, 1);
		$header_num--;
	} else {
		$n++;
	}
}

# Get number of header files again
$header_num = @{$current_report->{'bbcresults'}->{'issuelist'}->[0]->{'headerfile'}};
print "Final number of header files with issues: $header_num \n";

# Close Public API definition file.
untie @lines;

# Write new XML to dest file.
open OUT,">$destfile" or die("Cannot open file \"$destfile\" for writing. $!\n");
print OUT XMLout($current_report, keeproot => 1);
close OUT;

# Insert:	<?xml version="1.0" encoding="ASCII" standalone="no" ?>
#			<?xml-stylesheet type="text/xsl" href="BBCResults.xsl"?>
tie @lines, 'Tie::File', $destfile or die ("Cannot tie file \"$destfile\". $!\n");
unshift @lines, "<?xml-stylesheet type=\"text/xsl\" href=\"BBCResults.xsl\"?>";
unshift @lines, "<?xml version=\"1.0\" encoding=\"ASCII\" standalone=\"no\" ?>";
untie @lines;
