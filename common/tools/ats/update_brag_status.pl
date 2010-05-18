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
my $ats_report;
my $test_report;
my $help;
my $line;
my $auto_tests_total;
my $auto_tests_passed;
my $auto_tests_failed;
my $auto_tests_notrun;
my $man_tests_total;
my $man_tests_passed;
my $man_tests_failed;
my $man_tests_notrun;
my $temp_path;
my $detailshref;
my $xml_report;
my $test_case;
my $test_brag_xml;
my $existing_results;
my $n;
my $m;
my @lines;

sub usage($);
sub help();
sub usage_error();

my %optmap = (  'brag-file' => \$brag_file,
			    'ats-report' => \$ats_report,
			    'test-report' => \$test_report,
				'help' => \$help);

GetOptions(\%optmap,
          'brag-file=s',
          'ats-report=s',
          'test-report=s',
		  'help!') 
          or usage_error();

if ($help) {
	help();
}

# --brag-file is mandatory.
usage_error(), unless (defined($brag_file));

# --ats-report is mandatory.
usage_error(), unless (defined($ats_report));

open (FILE, "<$ats_report") or die ("Couldn't open $ats_report for reading: $!\n");
my $read_flag = -1;
print "Reading $ats_report... ";
while ($line = <FILE>) {
	chomp $line;
	if ($read_flag >= 0) { $read_flag++ }
	if ($read_flag == 1) { # Get Total number of tests.
		$auto_tests_total = $line;
		# Remove td tags.
		$auto_tests_total =~ s/^.*\">//;
		$auto_tests_total =~ s/<.*//;
	}
	if ($read_flag == 4) { # Get number of Passed tests.
		($auto_tests_passed) = split /\//,$line;
		$auto_tests_passed =~ s/^.*\">//; # Remove opening td tag.
	}
	if ($read_flag == 5) { # Get number of Failed tests.
		($auto_tests_failed) = split /\//,$line;
		$auto_tests_failed =~ s/^.*\">//; # Remove opening td tag.

	}
	if ($read_flag == 7) { # Get number of Not run tests.
		($auto_tests_notrun) = split /\//,$line;
		$auto_tests_notrun =~ s/^.*\">//; # Remove opening td tag.
	}
	if ($line eq "<td class=\"tableData\">Nested tests<\/td>") { # Set flag for reading results.
		$read_flag = 0;
	}
	if (($line eq "<\/tr>") && ($read_flag >= 0)) { # Exit loop.
		last;
	};
}
close FILE;
print "complete\n";
print "Total number of tests: $auto_tests_total\n";
print "Passed tests: $auto_tests_passed\n";
print "Failed tests: $auto_tests_failed\n";
print "Not run tests: $auto_tests_notrun\n";

# Define detailshref attribute.
$temp_path = $brag_file;
$temp_path =~ s/\\/\//g; # Replace \ with /
$temp_path =~ s/\/summary\/.*//;
$detailshref = $ats_report;
$detailshref =~ s/\\/\//g; # Replace \ with /
$detailshref =~ s/($temp_path)/../;

if (defined($test_report)) { # Get manual Smoketest results.
	print "Parsing " . $test_report . "... ";
	$xml_report = XMLin("$test_report", keeproot => 1, forcearray => [ 'test-set', 'test-case' ], keyattr => [] );
	print "complete \n";
	$man_tests_total = 0;
	$man_tests_passed = 0;
	$man_tests_failed = 0;
	$man_tests_notrun = 0;
	foreach $test_case (@{$xml_report->{'test-report'}->{'test-set'}->[0]->{'test-case'}}) {
		if (lc($test_case->{'result'}) eq "passed") { $man_tests_passed++ }
		if (lc($test_case->{'result'}) eq "failed") { $man_tests_failed++ }
		if (lc($test_case->{'result'}) eq "not run") { $man_tests_notrun++ }
		$man_tests_total++;
	}
	print "Total number of tests: $man_tests_total\n";
	print "Passed tests: $man_tests_passed\n";
	print "Failed tests: $man_tests_failed\n";
	print "Not run tests: $man_tests_notrun\n";
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
	splice @lines, $n, 0, "<step name=\"Smoketest - manual tests\">"; $n++;
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
	print FILE "<step name=\"Smoketest - manual tests\">\n";
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

exit 0;

sub usage($)
{
    my $error = shift;
    my $fh = $error == 0 ? *STDOUT : *STDERR;
    print $fh "update_brag_status.pl\n" .
            "Specify the BRAG xml file and ATS report\n" .
            "synopsis:\n" .
            "  update_brag_status.pl --help\n" .
            "  update_brag_status.pl [--brag-file=XML_FILE] [--ats-report=HTML_REPORT] [--test-report=XML_REPORT] \n" .
            "options:\n" .
            "  --help                        Display this help and exit.\n" .
            "  --brag-file=XML_FILE          XML_FILE is the full path to the file containing BRAG summary.\n" .
            "  --ats-report=HTML_REPORT      HTML_REPORT is the full path to the ATS report.\n" .
            "  --test-report=XML_REPORT      XML_REPORT is the name of the test report for manually executed tests.\n" .
			"                                If not specified only automated tests results will be taken into account.\n";
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
