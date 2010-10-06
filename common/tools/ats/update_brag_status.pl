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
#   This is a tool for updating BRAG status with test results.

use strict;
use Getopt::Long;
use XML::Simple;
use Tie::File;

my $brag_file;
my $ats3_report;
my $ats4_report;
my $test_report;
my $help;
my $line;
my $auto_tests_total = 0;
my $auto_tests_passed = 0;
my $auto_tests_failed = 0;
my $auto_tests_skipped = 0;
my $auto_tests_noresult = 0;
my $auto_tests_notrun = 0;
my $auto_tests_disabled = 0;
my $man_tests_total = 0;
my $man_tests_passed = 0;
my $man_tests_failed = 0;
my $man_tests_notrun = 0;
my $temp_path;
my $detailshref;
my $xml_report;
my $test_case;
my $test_brag_xml;
my $existing_results;
my $n;
my $m;
my @lines;
my $man_detailshref;
my $build_id;
my $name;
my $type = "smoke"; # Currently hard-coded.
my $start_time;
my $end_time;
my $time_stamp;
my ($date, $time);
my ($day, $month, $year);
my $cases_total;
my $cases_passed;
my $cases_failed;
my $cases_not_run;
my $cases_not_available;
my $fail_rate;
my $auto_pass_rate;
my $test_brag_status;
my $test_target = "winscw"; # Currently hard-coded.
my $report_url;
my $bit_file = "bitinfo_smoketest.txt";
my $bitops_path = "\.\.\\bitops.pl"; # Location of the bitops script (to update BIT db).

sub usage($);
sub help();
sub usage_error();

my %optmap = (  'brag-file' => \$brag_file,
			    'ats3-report' => \$ats3_report,
				'ats4-report' => \$ats4_report,
			    'test-report' => \$test_report,
				'build-id' => \$build_id,
				'help' => \$help);

GetOptions(\%optmap,
          'brag-file=s',
          'ats3-report=s',
          'ats4-report=s',
          'test-report=s',
          'build-id=i',
		  'help!') 
          or usage_error();

if ($help) {
	help();
}

# --brag-file is mandatory.
usage_error(), unless (defined($brag_file));

# --ats3-report/ats4-report is mandatory.
usage_error(), unless ((defined($ats3_report)) || (defined($ats4_report)));

if (defined($ats3_report)) { # Get ATS3 Smoketest results.
	open (FILE, "<$ats3_report") or die ("Couldn't open $ats3_report for reading: $!\n");
	my $read_flag = -1;
	print "Reading $ats3_report... ";
	while ($line = <FILE>) {
		chomp $line;
		if ($read_flag >= 0) { $read_flag++ }
		if ($read_flag == 1) { # Get Total number of tests.
			$auto_tests_total = $line;
			# Remove td tags.
			$auto_tests_total =~ s/^.*\">//;
			$auto_tests_total =~ s/<.*//;
		}
		if ($read_flag == 3) { # Get number of Disabled tests.
			($auto_tests_disabled) = split /\//,$line;
			$auto_tests_disabled =~ s/^.*\">//; # Remove opening td tag.
		}
		if ($read_flag == 4) { # Get number of Passed tests.
			($auto_tests_passed) = split /\//,$line;
			$auto_tests_passed =~ s/^.*\">//; # Remove opening td tag.
		}
		if ($read_flag == 5) { # Get number of Failed tests.
			($auto_tests_failed) = split /\//,$line;
			$auto_tests_failed =~ s/^.*\">//; # Remove opening td tag.
		}
		if ($read_flag == 6) { # Get number of Skipped tests.
			($auto_tests_skipped) = split /\//,$line;
			$auto_tests_skipped =~ s/^.*\">//; # Remove opening td tag.
		}
		if ($read_flag == 7) { # Get number of No result tests.
			($auto_tests_noresult) = split /\//,$line;
			$auto_tests_noresult =~ s/^.*\">//; # Remove opening td tag.
		}
		if ($line eq "<td class=\"tableData\">Nested tests<\/td>") { # Set flag for reading results.
			$read_flag = 0;
		}
		if (($line eq "<\/tr>") && ($read_flag >= 0)) { # Exit loop.
			last;
		};
	}
	close FILE;
	# Set number of No run tests.
	$auto_tests_notrun = $auto_tests_skipped + $auto_tests_noresult;
	
	# Get additional info
	open (FILE, "<$ats3_report") or die ("Couldn't open $ats3_report for reading: $!\n");
	while ($line = <FILE>) {
		chomp $line;
		if ($line =~ "<b>Test run:") {
			$name = $line;
			$name =~ s/^.*<td>//;
			$name =~ s/<\/td>.*//;
		}
		if ($line =~ "<b>Start time:") {
			$start_time = $line;
			$start_time =~ s/^.*<td>//;
			$start_time =~ s/<\/td>.*//;
		}
		if ($line =~ "<b>End time:") {
			$end_time = $line;
			$end_time =~ s/^.*<td>//;
			$end_time =~ s/<\/td>.*//;
		}
	}
	close FILE;
	
}
if (defined($ats4_report)) { # Get ATS4 Smoketest results.
	open (FILE, "<$ats4_report") or die ("Couldn't open $ats4_report for reading: $!\n");
	my $read_flag = -1;
	print "Reading $ats4_report... ";
	while ($line = <FILE>) {
		chomp $line;
		if ($read_flag >= 0) { $read_flag++ }
		if ($read_flag == 3) { # Get status.
			$auto_tests_total++;
			if ($line =~ "PASSED") { $auto_tests_passed++ }
			if ($line =~ "FAILED") { $auto_tests_failed++ }
			if ($line =~ "SKIPPED") { $auto_tests_skipped++ }
			if ($line =~ "NOT_AVAILABLE") { $auto_tests_notrun++ }
			# Re-set flag.
			$read_flag = -1;
		}
		if ($line eq "<result type=\"TestCaseResult\">") { # Set flag for reading results.
			$read_flag = 0;
		}
	}
	close FILE;
	
	# Get additional info
	$name = "Smoketest"; # Name not available in ATS4 testreport.
	open (FILE, "<$ats4_report") or die ("Couldn't open $ats4_report for reading: $!\n");
	$start_time = "";
	while ($line = <FILE>) {
		chomp $line;
		if ($line =~ "timeStamp=\"") { 
			$time_stamp = $line;
			$time_stamp =~ s/^.*timeStamp=\"//;
			$time_stamp =~ s/\..*//;
			($date,$time) = split /\s/,$time_stamp;
			($day, $month, $year) = split /-/,$date;
			$time_stamp = "$year-$month-$day $time";
			if ($start_time eq "") { $start_time = $time_stamp }
		}
	}
	close FILE;
	$end_time = $time_stamp; # Last entry becomes end_time.
}
print "complete\n";
#print "Total number of tests: $auto_tests_total\n";
#print "Passed tests: $auto_tests_passed\n";
#print "Failed tests: $auto_tests_failed\n";
#print "Not run tests: $auto_tests_notrun\n";

# Define detailshref attribute and report_url.
$temp_path = $brag_file;
$temp_path =~ s/\\/\//g; # Replace \ with /
$temp_path =~ s/\/summary\/.*//;
if (defined($ats3_report)) { $detailshref = $ats3_report };
if (defined($ats4_report)) { $detailshref = $ats4_report };
$detailshref =~ s/\\/\//g; # Replace \ with /
$report_url = lc($detailshref);
$detailshref =~ s/($temp_path)/../;
#Replace "//v800020/publish" with "http://cdn.symbian.org" in report_url
$report_url =~ s/^.*\/publish\//http:\/\/cdn.symbian.org\//;

if (defined($test_report)) {
	$man_detailshref = $test_report;
	$man_detailshref =~ s/\\/\//g; # Replace \ with /
	$man_detailshref =~ s/($temp_path)/../;
}

if (defined($test_report)) { # Get manual Smoketest results.
	print "Parsing " . $test_report . "... ";
	$xml_report = XMLin("$test_report", keeproot => 1, forcearray => [ 'test-set', 'test-case' ], keyattr => [] );
	print "complete \n";
	foreach $test_case (@{$xml_report->{'test-report'}->{'test-set'}->[0]->{'test-case'}}) {
		if (lc($test_case->{'result'}) eq "passed") { $man_tests_passed++ }
		if (lc($test_case->{'result'}) eq "failed") { $man_tests_failed++ }
		if (lc($test_case->{'result'}) eq "not run") { $man_tests_notrun++ }
		$man_tests_total++;
	}
#	print "Total number of tests: $man_tests_total\n";
#	print "Passed tests: $man_tests_passed\n";
#	print "Failed tests: $man_tests_failed\n";
#	print "Not run tests: $man_tests_notrun\n";
}

# Check for Smoketest results in BRAG file.
open (FILE, "<$brag_file") or die ("Couldn't open $brag_file for reading: $!\n");
$existing_results = 0;
$n = 0;
while ($line = <FILE>)
{
	chomp $line;
	if ($line =~ "<phase name=\"Test\">") {
		$existing_results = 1;
		last;
	}
	$n++;
}
close FILE;

tie @lines, 'Tie::File', $brag_file or die ("Couldn't tie file $brag_file: $!\n");
if ($existing_results) { # Delete existing Test Results from BRAG file.
	$m = 0;
	do {
		$m++;
	} while (@lines[$n+$m] !~ "<\/phase>");
	splice @lines, $n, $m+1;
}

# Insert (New) Test Results to BRAG file.
$n = 0;
foreach ( @lines )
{
	if (lc(@lines[$n]) =~ "<\/buildstatus>") {
		last;
	}
	$n++;
}
splice @lines, $n, 0, "<phase name=\"Test\">"; $n++;
splice @lines, $n, 0, "<step detailshref=\"$detailshref\" name=\"Smoketest - automated tests\">"; $n++;
splice @lines, $n, 0, "<nestedtests count=\"$auto_tests_total\" property=\"total\"/>"; $n++;
splice @lines, $n, 0, "<nestedtests count=\"$auto_tests_passed\" property=\"passed\"/>"; $n++;
splice @lines, $n, 0, "<nestedtests count=\"$auto_tests_failed\" property=\"failed\"/>"; $n++;
splice @lines, $n, 0, "<nestedtests count=\"$auto_tests_notrun\" property=\"not run\"/>"; $n++;
splice @lines, $n, 0, "<\/step>"; $n++;
if (defined($test_report)) {
	splice @lines, $n, 0, "<step detailshref=\"$man_detailshref\" name=\"Smoketest - manual tests\">"; $n++;
	splice @lines, $n, 0, "<nestedtests count=\"$man_tests_total\" property=\"total\"/>"; $n++;
	splice @lines, $n, 0, "<nestedtests count=\"$man_tests_passed\" property=\"passed\"/>"; $n++;
	splice @lines, $n, 0, "<nestedtests count=\"$man_tests_failed\" property=\"failed\"/>"; $n++;
	splice @lines, $n, 0, "<nestedtests count=\"$man_tests_notrun\" property=\"not run\"/>"; $n++;
	splice @lines, $n, 0, "<\/step>"; $n++;
}
splice @lines, $n, 0, "<\/phase>";

untie @lines;
print "$brag_file updated\n";

# Create Test_BRAG.xml
$test_brag_xml = $temp_path . "\/summary\/" . "Test_BRAG.xml";
if (-e $test_brag_xml) { # Delete the file if exists.
	unlink($test_brag_xml);
	print "$test_brag_xml deleted\n";
}
open (FILE, "> $test_brag_xml") or die ("Couldn't open $test_brag_xml for writing: $!\n");
print FILE "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print FILE "<?xml-stylesheet type='text/xsl' href='brag.xsl'?>\n";
print FILE "<buildStatus>\n";
print FILE "<phase name=\"Test\">\n";
print FILE "<step detailshref=\"$detailshref\" name=\"Smoketest - automated tests\">\n";
print FILE "<nestedtests count=\"$auto_tests_total\" property=\"total\"/>\n";
print FILE "<nestedtests count=\"$auto_tests_passed\" property=\"passed\"/>\n";
print FILE "<nestedtests count=\"$auto_tests_failed\" property=\"failed\"/>\n";
print FILE "<nestedtests count=\"$auto_tests_notrun\" property=\"not run\"/>\n";
print FILE "<\/step>\n";
if (defined($test_report)) {
	print FILE "<step detailshref=\"$man_detailshref\" name=\"Smoketest - manual tests\">\n";
	print FILE "<nestedtests count=\"$man_tests_total\" property=\"total\"/>\n";
	print FILE "<nestedtests count=\"$man_tests_passed\" property=\"passed\"/>\n";
	print FILE "<nestedtests count=\"$man_tests_failed\" property=\"failed\"/>\n";
	print FILE "<nestedtests count=\"$man_tests_notrun\" property=\"not run\"/>\n";
	print FILE "<\/step>\n";
}
print FILE "<\/phase>\n";
print FILE "</buildStatus>\n";
close FILE;
print "$test_brag_xml saved\n";

# Calculate Test BRAG Status (based on auto + manual)
$cases_total = $man_tests_total + $auto_tests_total;
$cases_passed = $man_tests_passed + $auto_tests_passed;
$cases_failed = $man_tests_failed + $auto_tests_failed;
$cases_not_run = $man_tests_notrun + $auto_tests_notrun;
$cases_not_available = $auto_tests_disabled;
if ($auto_tests_total > 0) {
	$auto_pass_rate = ($auto_tests_passed / $auto_tests_total) * 100;
} else {
	$auto_pass_rate = 0;
}
if ($cases_total > 0) {
	$fail_rate = ($cases_failed / $cases_total) * 100;
} else {
	$fail_rate = 0;
}
if ($cases_total == 0) {
	$test_brag_status = "n/a"; 
} elsif (($fail_rate > 0) && ($fail_rate <= 50)) { 
	$test_brag_status = "amber"; 
} elsif ($fail_rate > 50) { 
	$test_brag_status = "red"; 
} else {
	$test_brag_status = "green";
}
$auto_pass_rate = sprintf("%.1f", $auto_pass_rate);
# Generate bit file
if (-e $bit_file) { # Delete the file if exists.
	unlink($bit_file);
	print "$bit_file deleted\n";
}
open (FILE, "> $bit_file") or die ("Couldn't open $bit_file for writing: $!\n");
print FILE "test	$name,$type,$test_target,$start_time,$end_time,$auto_tests_total,$auto_tests_passed,$auto_tests_failed,$auto_tests_notrun,$auto_tests_disabled,$auto_pass_rate\n";
if ($test_brag_status ne "n/a") { print FILE "test_brag	$test_brag_status\n"; }
print FILE "report	Smoke Test Report,$report_url,report";
close FILE;
print "$bit_file saved\n";

if (defined($build_id)) {
	print "Updating BIT db... ";
	my @args = ("perl", $bitops_path, "-u", $build_id, "-a", "-f", $bit_file);
	system(@args) == 0 or die "system @args failed: $?";
	print "complete\n";
}

exit 0;

sub usage($)
{
    my $error = shift;
    my $fh = $error == 0 ? *STDOUT : *STDERR;
    print $fh "update_brag_status.pl\n" .
            "Specify the BRAG xml file and ATS report\n" .
            "synopsis:\n" .
            "  update_brag_status.pl --help\n" .
            "  update_brag_status.pl [--brag-file=XML_FILE] [--ats3-report=HTML_REPORT] [--ats4-report=SIMPLELOGGER_REPORT] [--test-report=XML_REPORT] [--build-id=ID]\n" .
            "options:\n" .
            "  --help                            Display this help and exit.\n" .
            "  --brag-file=XML_FILE              XML_FILE is the full path to the file containing BRAG summary.\n" .
            "  --ats3-report=HTML_REPORT         HTML_REPORT is the full path to the ATS3 report.\n" .
            "  --ats4-report=SIMPLELOGGER_REPORT SIMPLELOGGER_REPORT is the full path to the ATS4 report.\n" .
            "  --test-report=XML_REPORT          XML_REPORT is the name of the test report for manually executed tests.\n" .
			"                                    If not specified only automated tests results will be taken into account.\n" .
			"  --build-id=ID                     ID is the id of the build which will be updated with the test results.\n";
    exit $error;            
}

sub help()
{
    usage(0);
}

sub usage_error()
{
    usage(1);
}
