# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Script name: check_pckg_test_status.pl
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#
# Contributors:
# Arnaud Lenoir <arnaudl@symbian.org>
#
# Created 27/11/2009
#
# Description:
# Task 286 - Package test status prep
#
# This script must be used at package level only!!!!
# For a general search to several packages, PDK, etc then use check_all_pckg_test_status.pl script
#
# Parameters passed:
#	Param1: Path to the root directory for the package (where the package_definition.xml is present)
#	Param2: Path to the file containing the different patterns to have a look for
#	Param3: Path to the directory where DB will be published

# Modules necessary to run this script
use Getopt::Long;
use strict;
use Text::CSV;
use Switch;
use Data::Dumper;


# Variables for parameters
my $help;
my $package_path;
my $pattern_file_path;
my $publication_directory_path;

# variables
my $system_cmd;		# Used to execute DOS commands
my $pckg_name="";	# Contains the name of the package
my @key_dirs_tab;	# Used to trace the patterns/keys for directories search
my %dirs_hashes;	# Contains the list of directories found hash_array=fct(directory_name(key),directory_path(values that are tables)
my @key_files_tab;	# Used to trace the patterns/keys for files search
my %files_hashes;	# Contains the list of files found hash_array=fct(file_name(key),file_path(values that are tables)
my @key_pattern_in_file_tab;	# Used to trace the patterns/keys for pattern in file search
my %pattern_in_file_hashes;		# Contains the list of files that contains the pattern found hash_array=fct(file_name(key),file_path(values that are tables)
my @key_file_in_dir_tab;	# Used to trace the patterns/keys for file in directory search
my %file_in_dir_hashes;		# Contains the list of files that are in a specific directory hash_array=fct(file_name(key),file_path(values that are tables)
my @directory;		# Used to extract data from the csv file that contains the different patterns to look for
my @file;			# Used to extract data from the csv file that contains the different patterns to look for
my @pattern;		# Used to extract data from the csv file that contains the different patterns to look for
my @test_type;		# Used to extract data from the csv file that contains the different patterns to look for
my %existing_tests;	# Hash array that record the tests that are supported / exist for the package.
my %patterns_anywhere_hashes;	# Hash array that record the list of file / directory that contain a specific pattern using pattern as key which point to a table with the data gathered.
my %patterns_anywhere_in_dir_hashes;	# Hash array that record the list of files that contain a specific pattern in a directory.
my %patterns_in_dir_in_file_hashes;	# Hash array that record the list of files that contain a specific pattern in a directory and in a file that is included in tha directory (or sub-directory).

# Constantes declaration
my $default_publication_directory_path = "\.\\results";
my $default_pattern_file_path = ".\\pckg_test_status_patterns.csv";
my $output_file_base_name = "package_test_status_";
my $output_file_extension = ".xml";
my $csv_status_file_name = "packages_tests_supported_status.csv";
# Used for search algorithm
# Directory		File	Pattern		Value	Action
#	0			 0			1		  1		Look for pattern anywhere in a file, directory/file name
#	0			 1			0		  2		Look for a file
#	0			 1			1		  3		Look for a pattern in a file
#	1			 0			0		  4		Look for a directory
#	1			 0			1		  5		Look for a pattern in the directory anywhere in a directory/file name
#	1			 1			0		  6		Look for a file in a directory
#	1			 1			1		  7		Look for a pattern in a specific file in a specific directory
my $search_algo_4_pattern_anywhere = 1;
my $search_algo_4_file = 2;
my $search_algo_4_pattern_in_file = 3;
my $search_algo_4_directory = 4;
my $search_algo_4_pattern_anywhere_in_directory = 5;
my $search_algo_4_file_in_directory = 6;
my $search_algo_4_pattern_in_specific_file_directory = 7;


my $nb_arg_passed = scalar(@ARGV);
if(!$nb_arg_passed)
{
	print "No arguments/parameters passed then print help!\n";
	helpme();
}

my %optmap = (  'package_path' => \$package_path,
			    'pattern_file_path' => \$pattern_file_path,
				'publication_directory_path' => \$publication_directory_path,
                'help' => \$help);

GetOptions(\%optmap,
		  'package_path=s',
		  'pattern_file_path=s',
		  'publication_directory_path=s',
          'help');

print "The value for help is: <$help>\n";
print "package_path: <$package_path>\n";
print "pattern_file_path: <$pattern_file_path>\n";
print "publication_directory_path: <$publication_directory_path>\n";

# Check if we need default values for some of the parameters
# Publication directory
if (!defined($publication_directory_path))
{
	print "No publication_directory_path passed as argument, therefore use default path\n";
	$publication_directory_path = $default_publication_directory_path;
	print "publication_directory_path = <$publication_directory_path>\n";
}
# Check if we have a directory use for publishing. If not create it
#print "Check if $publication_directory_path exist or not!\n";
if (!(-d $publication_directory_path))
{
	print "The directory $publication_directory_path, doesn't exist, therefore we create it\n";
	$system_cmd = "mkdir $publication_directory_path";	
	print "Exec: $system_cmd\n";
	system($system_cmd);
}
# Patterns file used
if (!defined($pattern_file_path))
{
	print "No specific patterns file has been passed as parameters, therefore we use default patterns file\n";
	$pattern_file_path = $default_pattern_file_path;
	print "pattern_file_path = <$pattern_file_path>\n";
}

if ($help)
{
	print "We want to print the help\n";
	helpme();
}
else
{
	print "\nIt's from here that the real work starts!\n";
}


# Define if the path given as a package_definition*.xml file.
# If yes, the directroy correspond to the package name.
# If no, then there is a problem, and therefore we should stop.
# 1. Check that the path contains the package_definition*.xml.
# 2. Extract package name from path given. 

print "Package name to extract from: $package_path\n";
if($package_path =~ m;\\([\w]+)$;)
{
	$pckg_name = $1;
	print "########################################################################\n";
	print "\t\tThe Package name is: <$pckg_name>\n";
	print "########################################################################\n";
}

# Load file with patterns
# If file doesn't exist, then exit
# Then
# Extract patterns from file
# Store in tables/hash array???
# Count number of lines in the file - header!
# Check for column name and stick it in the appropriate table

print "Open the patterns file .csv and extract data from it.\n";

open(PATTERNSFILE, "$pattern_file_path") or die "Can't open file $pattern_file_path. The error is <$!>\n";

#my $csv = Text::CSV->new({blank_is_undef => 1});
#my $csv = Text::CSV->new({empty_is_undef => 1});
my $csv = Text::CSV->new();

my $header=0;
my $count_nb_total_lines=0;
my @columns;

while (<PATTERNSFILE>) {
	if ($csv->parse($_)) 
	{
		@columns = $csv->fields();
		
		if(!$header)
		{
			#print "This is the first line of the csv file\n";
			$header=1;
			foreach (@columns) 
			{
				#print $_."\n";
			}			
		}
		else
		{
			#print "line #$count_nb_total_lines of the csv file\n";
			
			foreach (@columns) 
			{
				#print "<$_>\n";
			}
			push(@directory, $columns[0]);
			push(@file, $columns[1]);
			push(@pattern, $columns[2]);
			push(@test_type, $columns[3]);
			$count_nb_total_lines++;
		}
	} 
	else 
	{
		my $err = $csv->error_input;
		print "Failed to parse line: $err";
	}
}
print "The number of lines counted in the CSV file is: $count_nb_total_lines\n";
print "\n";

# Used to display the tables that extracted from the CSV file to check if data are correct
#for (my $i=0;$i<$count_nb_total_lines;$i++)
#{
#	print "$i:\t$directory[$i]\t\t$file[$i]\t\t$pattern[$i]\t\t\t$test_type[$i]\n";
#}

close(PATTERNSFILE);

# Establish the list of type file we have in the package
# 1. Create the table that contains the list of all the type of tests listed in the CSV file
# 1.a Create a hash array with table "test_type"
# 1.b Keep only one entry of each
# 2. Make sure that we fill it as we go along

#display_array("Type of test",@test_type);
foreach (@test_type)
{
	#print "The test type is: $_\n";
	$existing_tests{$_} = "";
}
#print "List of type of tests we are looking for\n";
#print Dumper(\%existing_tests);

# Look for patterns and fill tables
# foreach lines 
# 7 different outputs depending on the line contain
# directory, file and pattern (see Excel spreadsheet called "patterns_search_rules.xlsx")

my $select_search_type;
	
my $count =0;
while($count<$count_nb_total_lines)
{
	#print "count=$count\n";
	$select_search_type = 0;
	if($directory[$count] ne "")
	{
		$select_search_type += 4;
		#print "A directory value exist => select_search_type= <$select_search_type>\n";
	}
	if($file[$count] ne "")
	{
		$select_search_type += 2;
		#print "A file value exist => select_search_type= <$select_search_type>\n";
	}
	if($pattern[$count] ne "")
	{
		$select_search_type += 1;
		#print "A pattern value exist => select_search_type= <$select_search_type>\n";
	}
	#print "select_search_type= <$select_search_type>\n";

	switch($select_search_type)
	{
		case ($search_algo_4_pattern_anywhere)
		{
			print "Value is $search_algo_4_pattern_anywhere, meaning call fct_search_algo_4_pattern_anywhere\n";
			my @pattern_anywhere_returned_table = fct_search_algo_4_pattern_anywhere($pattern[$count]);
			
			push (@{ $patterns_anywhere_hashes {$pattern[$count]} }, @pattern_anywhere_returned_table);
			
			my $calcul_tab_size = scalar(@pattern_anywhere_returned_table);
			
			if($calcul_tab_size)
			{
				print "There is data for this search: <$calcul_tab_size>\n";
				$existing_tests{$test_type[$count]}++;	# Add 1 to the count for the type of test supported
			}
		}
		case ($search_algo_4_file)
		{
			print "Value is $search_algo_4_file, meaning call fct_search_algo_4_file\n";
			my @files_returned_table = fct_search_algo_4_file($file[$count]);
			
			# Display table in a nice way
			#display_array("files ".$file[$count],@files_returned_table);
			
			# Check if the key exists or not. If it doesn't exist then add it to the list
			@key_files_tab = add_key_for_dynamic_hash_array($file[$count], @key_files_tab);
			
			# Save table containing files list returned by fct in the hash array to key file pattern
			# hash array: files_hashes
			# contains a key called file_pattern1 ($file[$count]) which contains the table @files_returned_table that lists the files found having for name file_pattern1
			# contains a key called file_pattern2 ($file[$count]) which contains the table @filess_returned_table that lists the files found having for name file_pattern2
			# Contain as many key as we are looking for files
			push (@{ $files_hashes{$file[$count]} }, @files_returned_table);
			
			my $calcul_tab_size = scalar(@files_returned_table);
			if($calcul_tab_size)
			{
				print "There is data for this search: <$calcul_tab_size>\n";
				$existing_tests{$test_type[$count]}++;	# Add 1 to the count for the type of test supported
			}
		}
		case ($search_algo_4_pattern_in_file)
		{
			print "Value is $search_algo_4_pattern_in_file, meaning call fct_search_algo_4_pattern_in_file\n";
			# Hash array filling level 1 deepnees
			my @values_found_returned_table;
			my @pattern_in_file_returned_table = fct_search_algo_4_pattern_in_file($package_path."\\".$file[$count],$pattern[$count],"/s /m");
			
			# Display table in a nice way
			#display_array("files with pattern ".$pattern[$count],@pattern_in_file_returned_table);

			# Check if the key exists or not. If it doesn't exist then add it to the list
			@key_pattern_in_file_tab = add_key_for_dynamic_hash_array($pattern[$count], @key_pattern_in_file_tab);

			# Used to verify if we have updated the table properly by replacing it with the table passed back
			#display_array("keys list after",@key_dirs_tab);
			
			# Save table containing files list returned by fct in the hash array to key pattern pattern
			# hash array: pattern_in_file_hashes
			# contains a key called pattern_pattern1 ($pattern[$count]) which contains the table @dirs_returned_table that lists the directories found having for name directory_pattern1
			# contains a key called pattern_pattern2 ($pattern[$count]) which contains the table @dirs_returned_table that lists the directories found having for name directory_pattern2
			# Contain as many key as we are looking for directories
			foreach (@pattern_in_file_returned_table)
			{
				#print "Value: $_\n";
				$pattern_in_file_hashes{$pattern[$count]}->{$_} = "";
				#print "Value entered: <$pattern_in_file_hashes{$pattern[$count]}->{$_}>\n";
			}
			#print "\nDump contain of hash array:\n";
			#print Dumper(\%pattern_in_file_hashes);
			#print "Contain of hash array dumped\n\n";
			
			# Hash array filling level 2 deepnees
			# Take keys (file) taken in the 1st level deepness of the file to do next search
			foreach my $path2file2look_into (sort keys %{$pattern_in_file_hashes{$pattern[$count]}} )
			{
				print "path2file2look_into=<$path2file2look_into>\n";
				
				@values_found_returned_table = fct_search_algo_4_pattern_in_file($path2file2look_into,$pattern[$count],"/s");
				#display_array("lines in <$path2file2look_into>",@values_found_returned_table);
			
				# Do we need to remove?????????:
				#	"I:\sf\os\kernelhwsrv\baseintegtests\baseintegrationtest\testsuites\sd\group\bld.inf:"
				# Keep only what is after ":" (not included)

				# To remove????? - Beginning
#				my @pattern_in_file_cleanedup_table;
#				my $local_string;
#				$local_string = "\....:([\\w\\W]+)";
#				print "local_string: $local_string\n";
#				foreach my $line2transform (@values_found_returned_table)
#				{
#					print "line2transform: <$line2transform>\n";
#					$line2transform =~ /$local_string/;
#					print "line2transformed: <$line2transform>\n";
#					push (@pattern_in_file_cleanedup_table,$line2transform);
#				}
				# To remove????? - End
				
				# Populate data in the hash array{pattern}{file}[(lines]	{key} - [table]
				$pattern_in_file_hashes{$pattern[$count]}->{$path2file2look_into} = \@values_found_returned_table;
				# To remove????? - Beginning
				#$pattern_in_file_hashes{$pattern[$count]}->{$path2file2look_into} = \@pattern_in_file_cleanedup_table;
				# To remove????? - End
			}
			#print "\nDump full contain of hash array:\n";
			#print Dumper(\%pattern_in_file_hashes);
			#print "Full contain of hash array dumped\n\n";
			
			my $calcul_tab_size = scalar(@values_found_returned_table);
			if($calcul_tab_size)
			{
				print "There is data for this search: <$calcul_tab_size>\n";
				$existing_tests{$test_type[$count]}++;	# Add 1 to the count for the type of test supported
			}
		}
		case ($search_algo_4_directory)
		{
			print "Value is $search_algo_4_directory, meaning call fct_search_algo_4_directory\n";
			my @dirs_returned_table = fct_search_algo_4_directory($directory[$count]);
			
			# Display table in a nice way
			#display_array("directories ".$directory[$count],@dirs_returned_table);
			
			# Used to verify if table for the keys is managed properly
			#display_array("keys list before",@key_dirs_tab);

			# Check if the key exists or not. If it doesn't exist then add it to the list
			@key_dirs_tab = add_key_for_dynamic_hash_array($directory[$count], @key_dirs_tab);

			# Used to verify if we have updated the table properly by replacing it with the table passed back
			#display_array("keys list after",@key_dirs_tab);
			
			# Save table containing directories list returned by fct in the hash array to key directory pattern
			# hash array: dirs_hashes
			# contains a key called directory_pattern1 ($directory[$count]) which contains the table @dirs_returned_table that lists the directories found having for name directory_pattern1
			# contains a key called directory_pattern2 ($directory[$count]) which contains the table @dirs_returned_table that lists the directories found having for name directory_pattern2
			# Contain as many key as we are looking for directories
			push (@{ $dirs_hashes{$directory[$count]} }, @dirs_returned_table);
			
			my $calcul_tab_size = scalar(@dirs_returned_table);
			if($calcul_tab_size)
			{
				print "There is data for this search: <$calcul_tab_size>\n";
				$existing_tests{$test_type[$count]}++;	# Add 1 to the count for the type of test supported
			}
		}

		case ($search_algo_4_pattern_anywhere_in_directory)
		{
			print "Value is $search_algo_4_pattern_anywhere_in_directory, meaning call fct_search_algo_4_pattern_anywhere_in_directory\n";
			my @patterns_in_dir_returned_table = fct_search_algo_4_pattern_anywhere_in_directory($directory[$count],$pattern[$count]);
			
			push (@{ $patterns_anywhere_in_dir_hashes{$pattern[$count]} }, @patterns_in_dir_returned_table);
			
			my $calcul_tab_size = scalar(@patterns_in_dir_returned_table);
			if($calcul_tab_size)
			{
				print "There is data for this search: <$calcul_tab_size>\n";
				$existing_tests{$test_type[$count]}++;	# Add 1 to the count for the type of test supported
			}
		}
		case ($search_algo_4_file_in_directory)
		{
			print "Value is $search_algo_4_file_in_directory, meaning call fct_search_algo_4_file_in_directory\n";
			my @file_in_dir_returned_table = fct_search_algo_4_file_in_directory($directory[$count],$file[$count]);
			#display_array("file <".$file[$count]."> in directory <".$directory[$count].">",@file_in_dir_returned_table);
			
			# Check if the key exists or not. If it doesn't exist then add it to the list
			@key_file_in_dir_tab = add_key_for_dynamic_hash_array($file[$count], @key_file_in_dir_tab);
			
			#@key_file_in_dir_tab
			#%file_in_dir_hashes
			push (@{ $file_in_dir_hashes{$file[$count]} }, @file_in_dir_returned_table);
			
			print "\nDump full contain of hash array:\n";
			#print Dumper(\%file_in_dir_hashes);
			print "Full contain of hash array dumped\n\n";
			
			my $calcul_tab_size = scalar(@file_in_dir_returned_table);
			if($calcul_tab_size)
			{
				print "There is data for this search: <$calcul_tab_size>\n";
				$existing_tests{$test_type[$count]}++;	# Add 1 to the count for the type of test supported
			}
		}
		case ($search_algo_4_pattern_in_specific_file_directory)
		{
			print "Value is $search_algo_4_pattern_in_specific_file_directory, call fct_meaning search_algo_4_pattern_in_specific_file_directory\n";
			my @patterns_in_dir_in_file_returned_table = fct_search_algo_4_pattern_in_specific_file_and_directory($directory[$count],$file[$count],$pattern[$count]);

			push (@{ $patterns_in_dir_in_file_hashes{$pattern[$count]} }, @patterns_in_dir_in_file_returned_table);
			
			my $calcul_tab_size = scalar(@patterns_in_dir_in_file_returned_table);
			print "calcul_tab_size = <$calcul_tab_size>\n";
			if($calcul_tab_size)
			{
				print "There is data for this search: <$calcul_tab_size>\n";
				$existing_tests{$test_type[$count]}++;	# Add 1 to the count for the type of test supported
			}
		}		
		else
		{
			# default for the switch
			# Added one for the line error as we need to add 1st line of the csv file, which is the header line
			print "No correct values was passed <$select_search_type>, which is impossible in our system!!! Error on line ". ($count+1). " of file $pattern_file_path\n";
		}
	}
	$count++;
}

# Output generated in a file which is following special naming convention because used to create the globabl picture
# pckg_test_atatus[pckgname].xml?
# Create file if doesn't exist. If exist already, update? delete & create from scratch?
#
# 1. Extract package name
# 2. Create a file called "package_test_status_[package_name].xml
# 3. Fill xml file using data gathered in this script
# 4. Close xml file

print "\n";

#
# Display data
#
print "!!!!This section is related to the display of the data gatehered by this script!!!!\n\n";

# To remove????? - Beginning
#my @display_hash_tab;
#@display_hash_tab = keys(%dirs_hashes);
#foreach my $key_val (@display_hash_tab)
#{
#	print "Key: <$key_val>:\n";
#	my @read_table_in_hash_array = @{$dirs_hashes{$key_val}};
#	foreach my $tab_val (@read_table_in_hash_array)
#	{
#		print "\tValue: <$tab_val>\n";
#	}
#}
#print "\n";
# To remove????? - End

# Display of the search we have done for the directories we need information from
if(%dirs_hashes)
{
	print "Look for directories:\n";
	#display_hash_array_1_level_deep ("directories", %dirs_hashes);
	print Dumper({%dirs_hashes});
	print "\n\n\t\t.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-\n\n";
}

# Display of the search we have done for the files we need information from
if(%files_hashes)
{
	print "Look for files:\n";
	#display_hash_array_1_level_deep ("files", %files_hashes);
	print Dumper({%files_hashes});
	print "\n\n\t\t.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-\n\n";
}

# Display of the search we have done for the pattern in files we need information from
if(%pattern_in_file_hashes)
{
	print "Look for pattern in a file:\n";
	#display_hash_array_2_levels_deep ("1st & 2nd levels of patterns (files & lines) in files", %pattern_in_file_hashes);	
	
	print Dumper(\%pattern_in_file_hashes);
	#print Dumper({%pattern_in_file_hashes});	# same result as above
	print "\n\n\t\t.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-\n\n";
}

# Display of the search we have done for the files we need information from
if(%file_in_dir_hashes)
{
	print "Look for files in a specific directory:\n";
	#display_hash_array_1_level_deep ("specific file in a specific directory", %file_in_dir_hashes);
	print Dumper({%file_in_dir_hashes});
	print "\n\n\t\t.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-\n\n";
}

# Display of the search we have done for the patterns anywhere in directory / file names and in the files themselves.
if(%patterns_anywhere_hashes)
{
	print "Look for a pattern everywhere:\n";
	print Dumper({%patterns_anywhere_hashes});
	print "\n\n\t\t.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-\n\n";
}

# Display of the search we have done for the patterns anywhere in a specific directory.
if(%patterns_anywhere_in_dir_hashes)
{
	print "Look for a pattern everywhere in a directory:\n";
	print Dumper({%patterns_anywhere_in_dir_hashes});
	print "\n\n\t\t.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-\n\n";
}

# Display of the search we have done for the patterns in a specific directory that contain a specific file in that directory/sub-directory.
if(%patterns_in_dir_in_file_hashes)
{
	print "Look for a pattern in a specific file in a specific directory:\n";
	print Dumper({%patterns_in_dir_in_file_hashes});
	print "\n\n\t\t.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-\n\n";
}

print "\n";

print "Indicates the type of tests supported by the package $pckg_name:\n";
print Dumper(\%existing_tests);	

print "\n\n\t\t.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-\n\n";

#
# Save data in files
#
print "!!!!This section is related to the saving of the data in files!!!!\n\n";

print "Save basic data about the type of tests supported by the package in a generic csv file\n";

my $csv_status_file_path = "$publication_directory_path\\$csv_status_file_name";
print "The full path for the csv packages tests status file is: $csv_status_file_path\n";

if(!(-f $csv_status_file_path))
{
	print "The file doesn't exist, we need to create it and fill it with the hearder row\n";
	open(CSVSTATUSFILE,">$csv_status_file_path");	# Create the file from scratch
	
	print CSVSTATUSFILE "Package_Name";
	
	foreach my $type_test_key (sort keys %existing_tests)
	{
		# Fields separated by comas
		print "Enter header name: type_test_key is $type_test_key\n";
		print CSVSTATUSFILE ",$type_test_key";
	}
	print CSVSTATUSFILE "\n";	# To make sure we are ready for the next line
}
else
{
	print "The file exists, we need to populate it with the necessary info\n";
	open(CSVSTATUSFILE,">>$csv_status_file_path");	# Append the file as it exists already and we are just going to add a new line
	
	# Do we need to check if we have the same number of info on the header row (1st row)?????
}

print CSVSTATUSFILE "$pckg_name";
foreach my $type_test_key4val (sort keys %existing_tests)
{
	# Fields separated by comas
	print "$pckg_name: The key test type is: <$type_test_key4val> and the value associated with is: <$existing_tests{$type_test_key4val}>\n";
	print CSVSTATUSFILE ",$existing_tests{$type_test_key4val}";
}
print CSVSTATUSFILE "\n";	# To make sure we are ready for the next line

close(CSVSTATUSFILE);
print "We have now updated the file <$csv_status_file_path> with the data gathered for the package $pckg_name\n\n";

my $output_file_name = $output_file_base_name.$pckg_name.$output_file_extension;
print "The output file name is: $output_file_name\n";
my $path4output_file = "$publication_directory_path\\$output_file_name";
print "The full path for the output file is: $path4output_file\n";

print "\nCreate and fill file <$path4output_file>\n";

# Should we create a function that will be called form the switch/case that will save the info directly in the appropriate xml file and / or csv file?
open(OUTPUTFILE,">$path4output_file");

print "Fill the xml file with data we have gathered in this script!!!!!\n";
print OUTPUTFILE "Fill the xml file with data we have!!!!!\n";

close (OUTPUTFILE);

# End of the script
exit(0);
# THE END OF THE END AT THE END OF THIS SCRIPT

#
# Functions section
#

# If no parameters entered or help selected, display help
sub helpme
{	
	print "\nfct: helpme\n";
	print "check_pckg_test_status.pl\n";
	print "Usage:\n";
	print "check_pckg_test_status.pl --help\n";
	print "check_pckg_test_status.pl --package_path=I:\\sf\\os\\kernelhwsrv --pattern_file_path=I:\\pckg_test_status_patterns.txt --publication_directory_path=I:\\results\\kernelhwsrv\n";
	print "Options:\n";
	print "--help:\t\t\t\t display this help and exit\n";
	print "--package_path:\t\t\t is the path to the directory containing the package_definition.xml file\n";
	print "--pattern_file_path:\t\t is the path to the file containing the patterns to be lookeed for\n";
	print "--publication_directory_path:\t is the path to the directory where the results will be published\n";
	print "\t\t\t\t default value is $default_publication_directory_path\n";
	
	exit(0);
}

# Search functions
sub fct_search_algo_4_pattern_anywhere
{
	my ($pattern) = @_;

	print "This is the function: fct_search_algo_4_pattern_anywhere\n";

	print "Search for pattern: <$pattern>\n";
	my @pattern_search_tab;
	
	# Firstly have a look for the pattern in the directories and files names
	print "Search for the pattern in any directory or file names\n";
	
	@pattern_search_tab = fct_dos_cmd_dir_search("$package_path\\*$pattern*", "/s /b");
	
	#print "Pattern <$pattern> was found in: @pattern_search_tab\n";
	#print "\n";
	
	# Secondly have a look in any files if we can find the pattern.
	# Should we use: fct_search_algo_4_pattern_in_file()?

	my @local_returned_table = fct_search_algo_4_pattern_in_file($package_path . "\\*",$pattern[$count],"/s /n");
	# If we just want to display the files and not the contain of the line. As well should elimintate the duplicate for files?
	#my @local_returned_table = fct_search_algo_4_pattern_in_file($local_package_path,$pattern[$count],"/s /m");
	push(@pattern_search_tab,@local_returned_table);
	
	#print "\n";
	#print "Pattern <$pattern> was found in: @pattern_search_tab\n";
	#print "\n";
	
	return (@pattern_search_tab);
}
sub fct_search_algo_4_file
{
	my ($file) = @_;

	print "This is the function: fct_search_algo_4_file\n";
	print "Search for file: <$file>\n";
	
	my @files;

	@files = fct_dos_cmd_dir_search("$package_path\\$file", "/s /b");

	#print "files found are: @files\n";
	
	print "\n";
	return (@files);
}

# This function was originally setup to have a look for a pattern in a specific files,
# but we can do a search in a directory by adding \\* at the end of the path, which will be like look in all the files in the directory and sub-directories.
sub fct_search_algo_4_pattern_in_file
{
	my ($file,$pattern,$options) = @_;

	print "This is the function: fct_search_algo_4_pattern_in_file\n";

	#print "Search for file: <$file>\n";
	#print "Search for pattern: <$pattern>\n";
	#print "Options: <$options>\n";
	
	my @pattern_in_files;
	
	# Check findstr /? to find out about the "regular expression"
	# "." Wildcard: any character
	# "*" Repeat: zero or more occurances of previous character or class
		
	# Check if the pattern contain a dot. If yes replace any "." by "\."
	if ( $pattern =~ /\./)
	{
		$pattern =~ s/\./\\./;
		#print "Replaced all \. by \\. to the pattern: <$pattern>\n";
	}
	else
	{
		#print "$pattern doesn't doesn't contain a \.\n";
	}
	# Check if the pattern contain a star. If yes replace any "*" by ".*"
	# For a string to have a look at "string*.txt", the "*" equivalent for findstr is ".*"
	# For more info, check out here: http://technet.microsoft.com/en-us/library/bb490907.aspx
	if ( $pattern =~ /\*/)
	{
		$pattern =~ s/\*/\.\*/g;
		#print "Replaced all \* by \.* to the pattern: <$pattern>\n";
	}
	else
	{
		#print "$pattern doesn't contain a *\n";
	}

	# If the pattern contains a starting " and and end ", we consider we are looking for a the full string included between "" and therefore we need to use the option /c:"pattern to look for"
	if ( ($pattern =~ /^"/) && ($pattern =~ /"$/))
	{
		# Replace the first <"> by </c:">
		$pattern =~ s/\"/\/c:\"/;
		#print "Replaced first <\"> by <\/c:\"> to become: <$pattern>\n";
	}
	else
	{
		#print "$pattern is not contained between\"\" \n";
	}

	open(SEARCHPATTERNINFILE,"findstr $options $pattern $file |");
	
	while(<SEARCHPATTERNINFILE>)
	{
		my $val = $_;
		
		#remove the trailing \n
		chomp($val);
		
		#print "value found is <$val>\n";
		
		if($val =~ /.hg/)
		{
			#print "Discard that line!\n"
		}
		else
		{
			#print "We need to save the value <$val>\n";
			# Push value in a table and pass it as a parameter!?
			push(@pattern_in_files,$val);
		}
	}
	close(SEARCHPATTERNINFILE);

	print "\n";
	
	return (@pattern_in_files);
}

sub fct_search_algo_4_directory
{
	#my $dir = $_[0]; # get first argument passed to the function
	my ($dir) = @_;
		
	print "This is the function: fct_search_algo_4_directory\n";
	print "Search for directory: <$dir>\n";
	
	my @dirs;
	
	# To remove????? - Beginning
	#opendir DIR, $package_path or die "can't read $package_path: $!";
	#@dirs = readdir DIR;
	#@dirs = dir "$dir", readdir DIR;
	#closedir DIR;
	#print "\n";
	#print @dirs;
	#print "\n\n";
	# To remove????? - End
	
	@dirs = fct_dos_cmd_dir_search("$package_path\\$dir", "/s /b /a:d");
	
	# To remove????? - Beginning
	if(scalar(@dirs))
	{
		#print "\n";
		#print "Here is the list of directories that we have found named <$dir>:\n";
		foreach (@dirs)
		{
			#print "\t$_\n";
		}
	}
	#print "\n";
	# To remove????? - End
	
	return (@dirs);
}

sub fct_search_algo_4_pattern_anywhere_in_directory
{
	my ($dir,$pattern) = @_;

	print "This is the function: fct_search_algo_4_pattern_anywhere_in_directory\n";

	print "Findout if there is one or more directory called: <$dir>\n";
	my @local_dir_exist_tab;
	my @table_to_return_with_search;
	
	@local_dir_exist_tab = fct_dos_cmd_dir_search("$package_path\\$dir", "/s /b /a:d");
	
	print "we found <$dir>: \n<\n@local_dir_exist_tab \n>\n";
	
	print "Search for pattern: <$pattern> in directory $dir\n";
	foreach my $dir_to_search (@local_dir_exist_tab)
	{
		print "The directory that we examine is: $dir_to_search\n";
		
		my @local_pattern_in_directory_returned_table = fct_search_algo_4_pattern_in_file($dir_to_search. "\\*",$pattern,"/s /n");
		push(@table_to_return_with_search,@local_pattern_in_directory_returned_table);
		
		#print "Here are all the values we found for $pattern in $dir_to_search:\n";
		#print Dumper(@local_pattern_in_directory_returned_table);
	}

	#print "Here are all the values we found and that will be returned: \n";
	#print Dumper(@table_to_return_with_search);
	
	print "\n";
	
	return (@table_to_return_with_search);
}

sub fct_search_algo_4_file_in_directory
{
	my ($dir,$file) = @_;

	print "This is the function: fct_search_algo_4_file_in_directory\n";

	print "Search for directory: <$dir>\n";
	print "Search for file: <$file>\n";

	print "\n";
	my @dir_list;
	my @file_in_dir;

	print "Look for directories <$dir>\n";
	@dir_list = fct_dos_cmd_dir_search("$package_path\\$dir", "/s /b /a:d");
	
	#print "we found <$dir>: \n<\n@dir_list \n>\n";

	# Look for files in the directories we have previously found
	print "Look for the file <$file> in the directory found previously\n";
	
	foreach my $dir_to_search (@dir_list)
	{
		#print "Look file <$file> in directory <$dir_to_search>\n";
		
		@file_in_dir = fct_dos_cmd_dir_search("$dir_to_search\\$file", "/s /b");
		
		open(SEARCHFILEDIR,"dir /s /b $dir_to_search\\$file |");
	}
	
	#print "we found <$file> in: \n<\n@file_in_dir \n>\n";
	
	return (@file_in_dir);
}

sub fct_search_algo_4_pattern_in_specific_file_and_directory
{
	my ($dir,$file,$pattern) = @_;

	print "This is the function: fct_search_algo_4_pattern_in_specific_file_and_directory\n";

	my @local_dir_exist_tab;
	my @local_file_exist_in_dir_tab;
	my @table_to_return_with_search;
	
	# Look for directories
	print "Look for directories <$dir>\n";
	@local_dir_exist_tab = fct_dos_cmd_dir_search("$package_path\\$dir", "/s /b /a:d");
	
	#print "we found <$dir>: \n<\n@local_dir_exist_tab \n>\n";

	# Look for files in the directories we have previously found
	print "Look for the file <$file> in the directory found previously\n";
	
	foreach my $dir_to_search (@local_dir_exist_tab)
	{
		#print "Look file <$file> in directory <$dir_to_search>\n";
		
		@local_file_exist_in_dir_tab = fct_dos_cmd_dir_search("$dir_to_search\\$file", "/s /b");
		
		open(SEARCHFILEDIR,"dir /s /b $dir_to_search\\$file |");
	}
	
	#print "we found <$file> in: \n<\n@local_file_exist_in_dir_tab \n>\n";
	
	print "Search for pattern: $pattern in directory $dir and file $file\n";
	foreach my $file_to_search (@local_file_exist_in_dir_tab)
	{
		#print "File to examine is: $file_to_search\n";
		
		my @local_pattern_in_file_returned_table = fct_search_algo_4_pattern_in_file($file_to_search,$pattern,"/s /n");
		push(@table_to_return_with_search,@local_pattern_in_file_returned_table);
		
		#print "Here are all the values we found for $pattern in $file_to_search:\n";
		#print Dumper(@local_pattern_in_file_returned_table);
	}

	#print "Here are all the values we found and that will be returned: \n";
	#print Dumper(@table_to_return_with_search);
	
	print "\n";
	
	return (@table_to_return_with_search);
}

# create a function that will look for a file or directory
sub fct_dos_cmd_dir_search
{
	my ($what_to_look_for,$options) = @_;
	
	print "This is the function: fct_dos_cmd_dir_search\n";
	my @search_result_tab;
	
	print "what_to_look_for: <$what_to_look_for>\n";
	print "options: <$options>\n";

	open(SEARCHUSINGDIRDOSCMD,"dir $options $what_to_look_for |");
	
	while(<SEARCHUSINGDIRDOSCMD>)
	{
		my $val = $_;
		
		#remove the trailing \n
		chomp($val);
		
		#print "value found is <$val>\n";
		
		if($val =~ /.hg/)
		{
			#print "Discard that line!\n"
		}
		else
		{
			#print "We need to save the value <$val>\n";
			# Push value in a table and pass it as a parameter!?
			push(@search_result_tab,$val);
		}
	}
	close(SEARCHUSINGDIRDOSCMD);
	
	return (@search_result_tab);
}

# Display an array with text indicating what it is
sub display_array
{
	# It is important to pass variable first then table, otherwise if variable comes second, you can't find it using @_. Why?
	my ($what_is_it,@table2display) = @_;

	#print "what_is_it: <$what_is_it>\n";
	print "The table containing all the ". $what_is_it ." is:\n";
	
	foreach (@table2display)
	{
		print "\t".$_."\n";
	}
	print "\n";
}

# Display a 1 level deep hash array in a nice way
sub display_hash_array_1_level_deep
{
	# It is important to pass variable first then table, otherwise if variable comes second, you can't find it using @_. Why?
	my ($what_is_it,%hash_array2display) = @_;
	
	print "The hash array containing all the ". $what_is_it ." is:\n";
	
	foreach my $k (keys %hash_array2display)
	{
	   print "$k:\n";
	   foreach (@{$hash_array2display{$k}})
	   {
		  print "\t$_\n";
	   }
	   print "\n";
	}
}

# Display a 2 levels deep hash array in a nice way
sub display_hash_array_2_levels_deep
{
	# It is important to pass variable first then table, otherwise if variable comes second, you can't find it using @_. Why?
	my ($what_is_it,%hash_array2display) = @_;
	
	print "The hash array containing all the ". $what_is_it ." is:\n";
	
	foreach my $k1 ( sort keys %hash_array2display )
	{
		print "$k1\n";

        foreach my $k2 ( sort keys %{$hash_array2display{ $k1 }} )
		{
            print "\t$k2\n";

            foreach my $k3 ( @{$hash_array2display{ $k1 }->{ $k2 }} )
			{
				print "\t\t$k3\n";
            }
        }
    }
}

# Add new key to table if the key is not already present in the table
sub add_key_for_dynamic_hash_array
{
	my ($key2look4,@table2look_into) = @_;
	
	#print "Nb of pattern keys in table2look_into is:<".scalar(@table2look_into).">\n";
	if(!scalar(@table2look_into))
	{
		#print "No keys in the table right now. Add <$directory[$count]> as a new key\n";
		push (@table2look_into,$key2look4);
	}
	#print "table2look_into:\n\t@table2look_into\n";
	
	my $counter_found_key=0;
	
	foreach my $local_var(@table2look_into)
	{
		#print "local_var= <$local_var>\n";
		if($key2look4 eq $local_var)
		{
			#print "That key exists already\n";
			$counter_found_key++;
		}
	}
	#print "counter_found_key=$counter_found_key\n";
	if(!$counter_found_key)
	{
		#print "The key <$directory[$count]> has not be found in the list\n";
		# Save key in the table to detect if exit or not
		push (@table2look_into,$key2look4);
	}
	return (@table2look_into);
}

# End section related to help
