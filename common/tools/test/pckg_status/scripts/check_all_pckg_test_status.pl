# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
#
# Script name: check_all_pckg_test_status.pl
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#
# Contributors:
# Arnaud Lenoir <arnaudl@symbian.org>
#
# Created 30/11/2009
#
# Description:
# Task 286 - Package test status prep - Compile a global view for all packages that we want to analyse
#
# Parameters passed:
#	Param1: Path to the file containing the directories to check for test status
#	Param2: Path to the directory containing the source code to analyse (generate a file)
#	Param3: Path to the directory where DB will be published

# Modules necessary to run this script
use Getopt::Long;
use strict;
use Text::CSV;

# Parameters passed to this script
my $help;
my $source_path_to_analyse;
my $dirs_file_path_to_analyse;
my $source_csv_path_to_analyse;
my $publication_directory_path;
my $pattern_file_path;

# Variables
my @tab_package_extraction;
my @tab_package_extraction_S2;
my @tab_package_extraction_S3;

my $system_cmd;

# Constantes declaration
my $default_publication_directory_path = "\.\\results";
my $default_pattern_file_path = ".\\pckg_test_status_patterns.csv";
my $pckg_dir_paths_file_name = "pckg_test_status_paths.txt";
my $pckg_dir_paths_file_name_S2 = "pckg_test_status_paths_S2.txt";
my $pckg_dir_paths_file_name_S3 = "pckg_test_status_paths_S3.txt";
my $all_pckgs_from_source_csv= "all_pckgs_from_source_csv.txt";
my $pckg_definition_pattern = "package_definition*.xml";
my $pckg_definition_pattern_S2 = "package_definition.xml";
my $pckg_definition_pattern_S3 = "package_definition_*.xml";
my $pckg_definition_extraction_pattern = "(^[\\w\\W]+)\\package_definition";
my $csv_status_file_name = "packages_tests_supported_status.csv";

my $nb_arg_passed = scalar(@ARGV);
if(!$nb_arg_passed)
{
	print "No arguments/parameters passed then print help!\n";
	helpme();
}

my %optmap = (  'source_path_to_analyse' => \$source_path_to_analyse,
			    'dirs_file_path_to_analyse' => \$dirs_file_path_to_analyse,
			    'publication_directory_path' => \$publication_directory_path,
				'source_csv_path_to_analyse' => \$source_csv_path_to_analyse,
			    'pattern_file_path' => \$pattern_file_path,
                'help' => \$help);

GetOptions(\%optmap,
		  'source_path_to_analyse=s',
		  'dirs_file_path_to_analyse=s',
		  'publication_directory_path=s',
		  'source_csv_path_to_analyse=s',
		  'pattern_file_path=s',
          'help');

print "The value for help is: <$help>\n";
print "source_path_to_analyse: <$source_path_to_analyse>\n";
print "dirs_file_path_to_analyse: <$dirs_file_path_to_analyse>\n";
print "publication_directory_path: <$publication_directory_path>\n";
print "source_csv_path_to_analyse: <$source_csv_path_to_analyse>\n";
print "pattern_file_path: <$pattern_file_path>\n";

if ($help)
{
	print "We want to print the help\n";
	helpme();
}
else
{
	print "\nIt's from here that the real work starts!\n";
}

# Check if we need default values for some of the parameters
# Publication directory
if (!defined($publication_directory_path))
{
	print "No publication_directory_path passed as argument, therefore use default path\n";
	$publication_directory_path = $default_publication_directory_path;
	print "publication_directory_path = <$publication_directory_path>\n";
}
# Check if we have a directory use for publishing. If not create it
print "Check if $publication_directory_path exist or not!\n";
if (!(-d $publication_directory_path))
{
	print "The directory $publication_directory_path, doesn't exist, therefore we create it\n";
	$system_cmd = "mkdir $publication_directory_path";	
	print "Exec: $system_cmd\n";
	system($system_cmd);
}

# We are going to packages type of tests supported in a csv file and before to do that we are going to make sure that if a file exist, then we rename it.
# File will be renamed before .csv with date & time in the format:
# year-month-day@hour-minute
# Eg: packages_tests_supported_status-2009-12-11@10-57.csv

my $csv_status_file_path = "$publication_directory_path\\$csv_status_file_name";
print "The full path for the csv packages tests status file is: $csv_status_file_path\n";

if(-f $csv_status_file_path)
{
	print "the file exists, we need to rename it to allow a new file to be created with new data\n";
	my $date;
	my $time;
	
	open (GETTIME,"time /t |");
	$time = <GETTIME>;
	chomp($time);
	
	open (GETDATE,"date /t |");
	$date = <GETDATE>;
	chomp($date);
	
	print "date is <$date>\n";
	print "time is <$time>\n";
	
	$time =~ s/:/-/g;
	
	$date =~ m~([0-9]+)\/([0-9]+)\/([0-9]+)~;
	print "$3-$2-$1\n";
	$date = "$3-$2-$1";
	
	print "date is <$date>\n";
	print "time is <$time>\n";

	my $csv_status_file_path_renamed = $csv_status_file_name;
	print "Current file name: $csv_status_file_path_renamed\n";	
	$csv_status_file_path_renamed =~ s/\.csv/\-$date\@$time.csv/;
	print "Renamed file name: $csv_status_file_path_renamed\n";

	# Rename file to be saved and to use new one to work with.
	$system_cmd = "rename $csv_status_file_path $csv_status_file_path_renamed";	
	print "Exec: $system_cmd\n";
	system($system_cmd);
}
else
{
	print "The file doesn't exist, we are starting from scratch! Nothing else to do!\n";
}

# Define Pacakge list & path by searching for "package_definition*.xml" file.
# The directory having that file is considerated as the short package name.
# Store list in a file? DB? or table?

if (defined($source_csv_path_to_analyse))
{
	print "We have been given the path to a source.csv file to extract all the packages to be used\n";
	if(!(-f $source_csv_path_to_analyse))
	{
		print "The file $source_csv_path_to_analyse doesn't exist! Please provide a new path!";
		exit(0);
	}
	open (CSVFILE, "$source_csv_path_to_analyse");
	
	my @local_tab = <CSVFILE>;
	
	# Need to discard the first line of the file
	print "local_tab[0]=<$local_tab[0]>\n";
	$local_tab[0] = undef;
	print "local_tab[0]=<$local_tab[0]> has been removed as garbage for us\n";
	
	my $nb_packgs_in_csv = 0;

	# Extract package names
	foreach my $line (@local_tab)
	{
		#Typical line: <E:\hgcache\mercurial_master_prod/sfl/MCL/sf/app/contentcontrol/,/sf/app/contentcontrol,changeset,8cb47bc684d4,layers.sysdef.xml>
		#print "Line from file is: $line\n";
		#if($line =~ m;,(/sf/[\w\/_]+),;) # Use for the second column of the csv file
		if($line =~ m;([\w\W]+)\/,\/sf;) # Use for the first column of the csv file
		{
			#print "We have detected $1\n";
			my $transform = $1;
			$transform =~ s/\//\\/ig;
			#print "transformed to: $transform\n";
			$tab_package_extraction[$nb_packgs_in_csv++] = $transform;
		}
	}
	print "# of packages is: <$nb_packgs_in_csv>\n";
	
	close (CSVFILE);
	
	# Save data in a file to be reused later on.
	my $file2use = "$publication_directory_path\\$all_pckgs_from_source_csv";
	print "File to open=$file2use\n";
	
	open(SAVINGDATAFILE,">$file2use");
	foreach my $val2save (@tab_package_extraction)
	{
		print SAVINGDATAFILE "$val2save\n";
	}
	close(SAVINGDATAFILE);
}
elsif (defined($dirs_file_path_to_analyse))
{
	print "Use the file <$dirs_file_path_to_analyse> that contains the directories to scan\n";
	
	open(FILEWITHPCKGPATHS,"$dirs_file_path_to_analyse");
	my $count_line = 0;
	
	while(<FILEWITHPCKGPATHS>)
	{
		#print "$_";
		$tab_package_extraction[$count_line] = $_;
		#print "tab_package_extraction[$count_line]=$tab_package_extraction[$count_line]\n";
		$count_line++;
	}
	my $total_pckg_found = scalar(@tab_package_extraction);
	print "nb of packages found in $dirs_file_path_to_analyse is: <$total_pckg_found>\n";
	close(FILEWITHPCKGPATHS);
}
elsif (defined($source_path_to_analyse))
{
	print "Use the directory \\sf that contains the code to scan\n";
	generate_file_with_path_2_pckgs($source_path_to_analyse);
}
else
{
	print "No parameters <source_path_to_analyse> or <dirs_file_path_to_analyse> or <source_csv_path_to_analyse> passed, therefore can't work\n";
	helpme();
}

# Patterns file used
if (!defined($pattern_file_path))
{
	print "No specific patterns file has been passed as parameters, therefore we use default patterns file\n";
	$pattern_file_path = $default_pattern_file_path;
	print "pattern_file_path = <$pattern_file_path>\n";
}
# End check if default values are necessary

print "\n\n";

# Call script to extract test data from each packge.
# foreach package in the list, call check_pckg_test_status.pl script with right parameters
# check_pckg_test_status.pl will automatically fill the DB with the right info	

# May need to remove trailing \n using chomp($path2passed);
foreach my $path2passed (@tab_package_extraction)
{
	#print "Remove trailing \\n for each line\n";
	chomp($path2passed);
	#print "The path we pass is: <$path2passed>\n";
	
	$system_cmd = "check_pckg_test_status.pl --package_path=$path2passed --pattern_file_path=$pattern_file_path --publication_directory_path=$publication_directory_path";	
	print "Exec: $system_cmd\n";
	system($system_cmd);
}

# Create / update the global DB files.
# Can be adding all the smaller files generated to a bigger unigue file
# Use XSLT using all the xml files generated?

# End of the script
exit(0);


# Functions section

# If no parameters entered or help selected, display help
sub helpme
{
	print "\nfct: helpme\n";
	print "check_all_pckg_test_status.pl\n";
	print "Usage:\n";
	print "check_all_pckg_test_status.pl --help\n";
	print "check_all_pckg_test_status.pl --dirs_file_path_to_analyse=I:\\Task-286\\Scripts\\pckg_test_status_paths.txt --publication_directory_path=.\\result\n";
	print "check_all_pckg_test_status.pl --source_path_to_analyse=I:\\sf --publication_directory_path=.\\result --pattern_file_path=I:\\Task-286\\Scripts\\pckg_test_status_patterns.csv\n";
	print "check_all_pckg_test_status.pl --source_csv_path_to_analyse=I:\\sf --publication_directory_path=.\\result --pattern_file_path=I:\\Task-286\\Scripts\\pckg_test_status_patterns.csv\n";
	print "Options:\n";
	print "--help:\t\t\t\t display this help and exit\n";
	print "--dirs_file_path_to_analyse:\t path to a file that contains the path to the directories containing package_definition.xml\n";
	print "--source_path_to_analyse:\t path of the root of the source code \\sf\n";
	print "--publication_directory_path:\t is the path to the directory where the results will be published (optional)\n";
	print "--source_csv_path_to_analyse:\t is path to sources.csv file in case packages haven't got a package_definition.xml (optional)\n";
	print "\t\t\t\t default value is $default_publication_directory_path\n";
	print "--pattern_file_path:\t is the path to file that contain the patterns to be search for (optional)\n";
	print "\t\t\t\t default value is $default_pattern_file_path\n";

	exit(0);
}
# End section related to help


# This function is called with the path to the directory \\sf and then we scan for all the directories / packages that contains package_definition*.xml
# Result should be a file with something like this: "I:\sf\os\kernelhwsrv"
# Parameter passed is the path to the directory \\sf
sub generate_file_with_path_2_pckgs
{
	my ($path_2_dir_sf_2_analyse) = @_;
	my @tab_package_definition_list;
	
	print "The path to the \\sf directory we want to analyse is: <$path_2_dir_sf_2_analyse>\n";
	
	print "The pattern used to find out the root directory for a pacakge is: <$pckg_definition_pattern>\n";
	
	# Ananlyse directories for package_definition*.xml
	# Find a way to parse all the directories and get results as fast as possible
	# something like <dir I:\sf\package_definition*.xml /s/b>

	print "\n";
	
	open(FINDPCKDEFPATHS,"dir $path_2_dir_sf_2_analyse\\$pckg_definition_pattern /s/b |");
	my $local_ctr = 0;
	
	while(<FINDPCKDEFPATHS>)
	{
		print "$_";
		$tab_package_definition_list[$local_ctr] = $_;
		#print "tab_package_definition_list[$local_ctr]=$tab_package_definition_list[$local_ctr]\n";
		$local_ctr++;
	}
	my $total_pckg_def_files_found = scalar(@tab_package_definition_list);
	print "nb of $pckg_definition_pattern found in $path_2_dir_sf_2_analyse are: <$total_pckg_def_files_found>\n";
	
	close(FINDPCKDEFPATHS);
	
	print "\n";
	
	my $local_ctr1=0;
	my $local_ctr2=0;
	my $local_ctr3=0;
		
	# What to extract
	#I:\sf\adaptation\qemu\package_definition.xml
	#I:\sf\adaptation\qemu\package_definition_2.0.1.xml
	# Anything before the last \\, whcih should not be included!
	foreach my $val (@tab_package_definition_list)
	{		
		if ($val =~ m#(^[\w\W]+)\\package_definition#i)
		#if ($val =~ m,$pckg_definition_extraction_pattern,i)
		{
			#print "Value to analyse is: $val\n";
			#print "Managed to extract: $1\n";
			$tab_package_extraction[$local_ctr1] = $1;
			#print "tab_package_extraction[$local_ctr1]=$tab_package_extraction[$local_ctr1]\n";
			$local_ctr1++;
		}
		
		#print "\n";
		
		# Create tables for S^2 & S^3 package_definition.xml file
		# Store them in a table and save it in 2 different files

		#S^2
		if ($val =~ m#(^[\w\W]+)\\package_definition.xml#i)
		{
			#print "Value to analyse is: $val\n";
			#print "Managed to extract: $1\n";
			$tab_package_extraction_S2[$local_ctr2] = $1;
			#print "tab_package_extraction_S2[$local_ctr2]=$tab_package_extraction_S2[$local_ctr2]\n";
			$local_ctr2++;
		}

		#S^3
		if ($val =~ m#(^[\w\W]+)\\package_definition_#i)
		{
			#print "Value to analyse is: $val\n";
			#print "Managed to extract: $1\n";
			$tab_package_extraction_S3[$local_ctr3] = $1;
			#print "tab_package_extraction_S3[$local_ctr3]=$tab_package_extraction_S3[$local_ctr3]\n";
			$local_ctr3++;
		}
	}

	# Eliminate duplicates packages paths!
	my %seen;
	for ( my $i = 0; $i <= $#tab_package_extraction ; )
	{
    splice @tab_package_extraction, --$i, 1
        if $seen{$tab_package_extraction[$i++]}++;
	}
	#print "Sorted out table:\n @tab_package_extraction\n";

	# Save data in a file that will be reused later on
	# Create a new file everytime. Save old one if exist with date and time?
	print "Path to the file where data will be saved is called: <$publication_directory_path>\n";
	print "File name where data will be saved is called: <$pckg_dir_paths_file_name>\n";
	
	my $data_2_save_file_path = "$publication_directory_path\\$pckg_dir_paths_file_name";
	my $path2save;
	
	print "The path to the file to write to is: <$data_2_save_file_path>\n";
	
	# Save generic table in a file
	# Open the file
	open(FILE2SAVEDATA, ">$data_2_save_file_path");
	
	foreach $path2save (@tab_package_extraction)
	{
		print FILE2SAVEDATA "$path2save\n";
	}
	
	# Close the file
	close(FILE2SAVEDATA);

	# Save generic table for S^2 in a file
	$data_2_save_file_path = "$publication_directory_path\\$pckg_dir_paths_file_name_S2";
	print "The path to the file to write to is: <$data_2_save_file_path>\n";
	# Open the file
	open(FILE2SAVEDATA, ">$data_2_save_file_path");
	
	foreach $path2save (@tab_package_extraction_S2)
	{
		print FILE2SAVEDATA "$path2save\n";
	}

	# Save generic table for S^3 in a file
	$data_2_save_file_path = "$publication_directory_path\\$pckg_dir_paths_file_name_S3";
	print "The path to the file to write to is: <$data_2_save_file_path>\n";
	# Open the file
	open(FILE2SAVEDATA, ">$data_2_save_file_path");
	
	foreach $path2save (@tab_package_extraction_S3)
	{
		print FILE2SAVEDATA "$path2save\n";
	}
	
	# Close the file
	close(FILE2SAVEDATA);
}
