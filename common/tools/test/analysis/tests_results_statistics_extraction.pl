# Copyright (c) 2010 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Script name: tests_results_statistics_extraction.pl
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#
# Contributors:
# Arnaud Lenoir <arnaudl@symbian.org>
#
# Created 18/01/2010
#
# Description:
# Allow the extraction of the statistics (total, passed, failed, etc) for Syborg (QEMU) tests
#
#
# Parameters passed:
#	Param1: Log file containing all the information collected during the Syborg tests
#	Param2: File containing the values to look for. DO WE NEED TO DO IT THAT WAY? COULD BE NICE TO MAKE THAT TOOLS GENERIC
#	Param3: NA for the moment

# Modules necessary to run this script
use Getopt::Long;
use strict;
use Data::Dumper;

# Variables for parameters
my $help;
my $file_to_analyse;

# variables
my $system_cmd;		# Used to execute DOS commands
my $results_destination_file;
my $full_list_destination_file;
my $total_nb_of_tests = 0;					# Count the number total of tests 
my $total_nb_of_passed_tests = 0;			# Count the number of tests that have passed
my $total_nb_of_failed_tests = 0;			# Count the number of tests that have failed
my $total_nb_of_error_tests = 0;			# Count the number of tests that have failed due to an error
my $total_nb_of_timeout_tests = 0;			# Count the number of tests that have failed due to a timeout
my $total_nb_of_unknown_tests_results = 0;	# If ever one of the string we are looking for is not reported
my @list_unknown_tests_results;				# If ever one of the string we are looking for is not reported, then save it and display it later on in the file we are saving
# Magic formula: $total_nb_of_tests = $total_nb_of_passed_tests + $total_nb_of_failed_tests + $total_nb_of_error_tests + $total_nb_of_timeout_tests + $total_nb_of_unknown_tests_results;
my %list_all_tests_results;					# List all the tests that are done and their final status. Use test name as key.
my %list_all_errors;						# List all the tests that errors of any sort. Use test name as key.

# Constantes declaration
my $results_destination_extension_file = "_statistics\.csv";
my $full_list_destination_extension_file = "_full_list\.csv";
# Strings to look for in the file to get statistics
my $running_test_string = "RUNTESTS: Test ";
my $passed_test_string = "OK";
my $failed_test_string = "FAIL";
my $error_test_string = " ERROR";
my $timeout_test_string = "TIMEOUT";
# Strings used to extract data for each tests (name, status, error)
my $search4test_name_string = $running_test_string."([\\w._-]*[\.]+[\\w]*)\\s";
#print "search4test_name_string = <$search4test_name_string>\n";
my $search4failling_error_string = $failed_test_string."\\s[-]+\\s([\\w\\s,]*)";
#print "search4failling_error_string = <$search4failling_error_string>\n";
my $search4error_string = $error_test_string."\\s([\\w\\s,-]*)";
#print "search4error_string = <$search4error_string>\n";


my $total_nb_tests_save = "Total nb of tests";
my $total_nb_tests_passed_save = "Total nb of tests passed";
my $total_nb_tests_failed_save = "Total nb of tests failed";
my $total_nb_tests_error_save = "Total nb of tests failed due to an error";
my $total_nb_tests_timeout_save = "Total nb of tests failed on timeout";
my $total_nb_tests_unknown_save = "Total nb of tests unknown";
my $check_result_ok = "Check that total nb of tests Vs results is CORRECT?";
my $display_analyse_and_update_script = "Analyse the following data and uppdate script accordingly!!!!!!";
my $display_total_nb_of_unknown_tests_results = "Here is the number of unknown tests results:";
my $display_unknown_tests_results = "Here is the list of unknown tests results lines that need to be analysed:";
my $test_name_column_title = "Test Name";
my $test_status_column_title = "Test Status";
my $test_error_column_title = "What Error?";


# Check if some arguments have been passed to the script
my $nb_arg_passed = scalar(@ARGV);
if(!$nb_arg_passed)
{
	print "No arguments/parameters passed then print help!\n";
	helpme();
}

# Retrieve the parameters passed to the script
my %optmap = (  'file_to_analyse' => \$file_to_analyse,
                'help' => \$help);

GetOptions(\%optmap,
		  'file_to_analyse=s',
          'help');

print "The value for help is: <$help>\n";
print "file_to_analyse: <$file_to_analyse>\n";

if ($help)
{
	print "The user required the display of help\n";
	helpme();
}

if (!defined($file_to_analyse))
{
		print "No files has been passed as parameter!!!\n";
		helpme();
}

# Check if we need the file to examine exists or not
if (-f $file_to_analyse)
{
	print "<$file_to_analyse> exists!!!\n";
	
	# Define the name of the ouptut file that will contain the results of our analysis
	$results_destination_file = "$file_to_analyse" . "$results_destination_extension_file";	
	print "The path and file where the statistics of this script will be saved is: <$results_destination_file>\n";
	$full_list_destination_file = "$file_to_analyse" . "$full_list_destination_extension_file";
	print "The path and file where all the tests results of this script will be saved is: <$full_list_destination_file>\n";
}
else
{
	print "File <$file_to_analyse> doesn't exist!!!\n";
	# Exit program because no files to analyse.
	exit(0);
}

# Analysis of the file
# 1st open file and check line by line.
# 2nd record statistics analysis

# Example of line we are looking for to analyse:
# Name of the test
# RUNTESTS: Started test t_lat2.EXE
# Result:
# RUNTESTS: Test t_lat2.EXE OK
# RUNTESTS: Test t_hcr.EXE FAIL - Exit code 2,84,USER
# RUNTESTS: Test t_dcnone.EXE ERROR Could not load file, error -1
# RUNTESTS: Test t_scan.EXE TIMEOUT

open (INPUTFILE, "$file_to_analyse");

foreach my $local_line (<INPUTFILE>)
{
	chomp($local_line);	# Remove trailig \n	
	#print "Line read from the file is: <$local_line>\n";
	
	if ( $local_line =~ /$running_test_string/)
	{
		#print "this line indicates that a test has been run\n";
		$total_nb_of_tests++;
		
		# Extract the name of the test
		$local_line =~ /$search4test_name_string/;
		my $test_name = $1;
		#print "test_name = <$test_name>\n";
		
		if ($local_line =~ /$passed_test_string/)
		{
			#print "this line indicates that a test has passed\n";
			$total_nb_of_passed_tests++;
			
			$list_all_tests_results {"$test_name"} = $passed_test_string;
			#print "list_all_tests_results {$test_name} = <".$list_all_tests_results {"$test_name"}.">\n";
		}
		elsif ($local_line =~ /$failed_test_string/)
		{
			#print "this line indicates that a test has failed\n";
			$total_nb_of_failed_tests++;
			
			$list_all_tests_results {"$test_name"} = $failed_test_string;
			#print "list_all_tests_results {$test_name} = <".$list_all_tests_results {"$test_name"}.">\n";
	
			# Look for the failling error to save it.
			$local_line =~ /$search4failling_error_string/;
			my $test_failling_error = $1;
			$test_failling_error =~ s/,/\;/g;	# To remove the "," otherwise this is not good for the writting of the data in the csv file 
			#print "test_failling_error = <$test_failling_error>\n";
			$list_all_errors {"$test_name"} = $test_failling_error;
			#print "list_all_errors {$test_name} = <".$list_all_errors {"$test_name"}.">\n";
		}
		elsif ($local_line =~ /$error_test_string/)
		{
			#print "this line indicates that a test has failed due to an error\n";
			$total_nb_of_error_tests++;
			
			$list_all_tests_results {"$test_name"} = $error_test_string;
			#print "list_all_tests_results {$test_name} = <".$list_all_tests_results {"$test_name"}.">\n";
			
			# Look for the error, which make the test to fail
			$local_line =~ /$search4error_string/;
			my $test_error = $1;
			$test_error =~ s/,/\;/g;	# To remove the "," otherwise this is not good for the writting of the data in the csv file 
			#print "test_error = <$test_error>\n";
			$list_all_errors {"$test_name"} = $test_error;
			#print "list_all_errors {$test_name} = <".$list_all_errors {"$test_name"}.">\n";
		}
		elsif ($local_line =~ /$timeout_test_string/)
		{
			#print "this line indicates that a test has failed on timeout\n";
			$total_nb_of_timeout_tests++;
			
			$list_all_tests_results {"$test_name"} = $timeout_test_string;
			#print "list_all_tests_results {$test_name} = <".$list_all_tests_results {"$test_name"}.">\n";
		}
		else
		{
			print "\n";
			chomp($local_line);
			print "This line contains a result that is unknown to us\n";
			print "Please analyse this line and add the appropriate test to solve the problem: <$local_line>\n";
			push(@list_unknown_tests_results, $local_line);
		}
	}
	else
	{
		#print "This line is of no interest to us!\n";
	}
}

print "\n";

close (INPUTFILE);

$total_nb_of_unknown_tests_results = scalar(@list_unknown_tests_results);

# Result section
print "Here are the values obtained:\n";
print "total_nb_of_tests = <$total_nb_of_tests>\n";
print "total_nb_of_passed_tests = <$total_nb_of_passed_tests>\n";
print "total_nb_of_failed_tests = <$total_nb_of_failed_tests>\n";
print "total_nb_of_error_tests = <$total_nb_of_error_tests>\n";
print "total_nb_of_timeout_tests = <$total_nb_of_timeout_tests>\n";
print "total_nb_of_unknown_tests_results = <$total_nb_of_unknown_tests_results>\n";
if ($total_nb_of_unknown_tests_results)
{
	print "Here are the lines containing unknown tests results:\n<\n@list_unknown_tests_results>\n";
}

print "\n\n";
print "Display tables to populate in the csv file to list all tests\n";
if(%list_all_tests_results)
{
	print Dumper(\%list_all_tests_results);
	print "\n";
}
if(%list_all_errors)
{
	print Dumper(\%list_all_errors);
	print "\n";
}


# List all the tests with their results & errors if some
open(OUTPUTFILE,">$full_list_destination_file");

print OUTPUTFILE $test_name_column_title.",".$test_status_column_title.",".$test_error_column_title."\n";	# Working, just easier to read
#print OUTPUTFILE "$test_name_column_title,$test_status_column_title,$test_error_column_title\n";			# Working

foreach (keys %list_all_tests_results)
{
	my $val = $_;
	#print "Test Name: <$val>\n";
	print OUTPUTFILE "$val,".$list_all_tests_results {"$val"}.",".$list_all_errors {"$val"}."\n";	# Do we need to check if list_all_errors is empty or not?
}

close (OUTPUTFILE);

# Store statistics in the file
open(OUTPUTFILE,">$results_destination_file");

print OUTPUTFILE $total_nb_tests_save.",$total_nb_of_tests\n";
print OUTPUTFILE $total_nb_tests_passed_save.",$total_nb_of_passed_tests\n";
print OUTPUTFILE $total_nb_tests_failed_save.",$total_nb_of_failed_tests\n";
print OUTPUTFILE $total_nb_tests_error_save.",$total_nb_of_error_tests\n";
print OUTPUTFILE $total_nb_tests_timeout_save.",$total_nb_of_timeout_tests\n";
print OUTPUTFILE $total_nb_tests_unknown_save.",$total_nb_of_unknown_tests_results\n";

print OUTPUTFILE "\n";

if ($total_nb_of_tests == ($total_nb_of_passed_tests + $total_nb_of_failed_tests + $total_nb_of_error_tests + $total_nb_of_timeout_tests + $total_nb_of_unknown_tests_results) )
{
	print OUTPUTFILE $check_result_ok.",YES"."\n";
}
else
{
	print OUTPUTFILE $check_result_ok.",NO"."\n";
}

if($total_nb_of_unknown_tests_results !=0)
{
	#print "\nThere are tests that have not been counted because not in the list of things we are looking for, please analyse file: <$results_destination_file>!\n";
	
	print OUTPUTFILE "\n\n";
	print OUTPUTFILE $display_analyse_and_update_script;
	print OUTPUTFILE "\n";
	print OUTPUTFILE $display_total_nb_of_unknown_tests_results.",".$total_nb_of_unknown_tests_results."\n";
	print OUTPUTFILE "\n";
	print OUTPUTFILE $display_unknown_tests_results."\n";
	foreach (@list_unknown_tests_results)
	{
		print OUTPUTFILE $_."\n";
	}
}
close (OUTPUTFILE);

print "\n\nHere is your output file: <$results_destination_file> !!!!!\n\n";

# End of the program!!!
exit(0);


# Functions section

# Help
sub helpme
{
	print "\nfct: helpme\n";
	print "tests_results_statistics_extraction.pl\n";
	print "Usage:\n";
	print "tests_results_statistics_extraction.pl --help\n";
	print "tests_results_statistics_extraction.pl --file_to_analyse=c:\\temp\\f32tests_log.txt\n";
	print "Options:\n";
	print "--help:\t\t\t\t display this help and exit\n";
	print "--file_to_analyse:\t\t is the path to the directory containing the package_definition.xml file\n";
	
	exit(0);
}


