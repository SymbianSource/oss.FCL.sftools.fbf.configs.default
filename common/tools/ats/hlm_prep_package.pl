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
#   This is a script for fixing pkg and ini files.

use strict;
use Getopt::Long;
use File::Copy;
use Tie::File;
use File::Find;

my $package_path;
my @pkg_paths; # Array needed as csv file may contain more than one.
my $pkg_path_num = 1; # By default at least one needs to be provided.
my $csv_file;
my $bld_drive;
my $line;
my $help;
my $repo;
my $destination;
my @files;
my @ini_files;
my @lines;
my $file;
my $n;
my $current_iteration;
my $file_fixed;
sub Wanted;
sub Parse_ini;

sub usage($);
sub help();
sub usage_error();

my %optmap = (  'package-path' => \$package_path,
			    'csv-file' => \$csv_file,
				'bld-drive' => \$bld_drive,
				'help' => \$help);

GetOptions(\%optmap,
          'package-path=s',
          'csv-file=s',
		  'bld-drive=s',
		  'help!') 
          or usage_error();

if ($help) {
	help();
}

usage_error(), unless ((defined($package_path)) || ((defined($csv_file)) && (defined($bld_drive))));

if (defined($csv_file)) {
	$pkg_path_num = 0;
	open FILE, "<$csv_file" or die("Failed to read $csv_file: $!\n");
	while ($line = <FILE>)
	{
		chomp $line;
		($repo,$destination) = split /,/,$line;
		if ((lc($destination) ne "dst") && (lc($destination) ne "")) { 
#			$package_path = $bld_drive . $destination;
			push @pkg_paths, $bld_drive . $destination;
			$pkg_path_num++;
#			last; 
		}
		
	}
	close FILE;
}

$current_iteration = 0;
while ($current_iteration < $pkg_path_num) {
	if (defined($csv_file)) {
		$package_path = @pkg_paths[$current_iteration];
	}
	print "Processing: $package_path \n";

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
			if (lc(@lines[$n]) =~ m/epoc32\/release\/armv5\/urel\//) {
				@lines[$n] = lc(@lines[$n]);
				@lines[$n] =~ s/\/armv5\/urel\//\/\$(platform)\/\$(target)\//;
				$file_fixed = 1;
			}
			if (lc(@lines[$n]) =~ m/epoc32\\release\\armv5\\udeb\\/) {
				@lines[$n] = lc(@lines[$n]);
				@lines[$n] =~ s/\\armv5\\udeb\\/\\\$(platform)\\\$(target)\\/;
				$file_fixed = 1;
			}
#			if (lc(@lines[$n]) =~ m/e:\\/) { # Replace e: with c:
#				@lines[$n] = lc(@lines[$n]);
#				@lines[$n] =~ s/e:\\/c:\\/;
#				$file_fixed = 1;
#			}
			$n++;
		}
		if ($file_fixed) { print $file . " fixed.\n"; }
		untie @lines;
	}

	find(\&Parse_ini, $package_path);

	foreach $file (@ini_files) {
		if ((lc($file) =~ m/\/init\//) || (lc($file) =~ m/\/group\//)) { # Only operate on files from /init/ directories
			copy($file,$file . ".orig") or die ("Cannot copy file \"$file\". $!\n");
			tie (@lines, 'Tie::File', $file, recsep => "\n") or die ("Cannot tie file \"$file\". $!\n");
			$n = 0;
			$file_fixed = 0;
			foreach (@lines) {
				if (lc(@lines[$n]) =~ m/^separateprocesses/) {
					@lines[$n] = '#' . @lines[$n];
					$file_fixed = 1;
				}
				if (lc(@lines[$n]) =~ m/^uitestingsupport/) {
					@lines[$n] = '#' . @lines[$n];
					$file_fixed = 1;
				}
				$n++;
			}
			if ($file_fixed) { print $file . " fixed.\n"; }
			untie @lines;
		}
	}

	$current_iteration++;
}

print "OK\n";
exit 0;

sub Wanted {
    # only operate on .pkg files
	/.pkg$/ or return;
    push (@files, $File::Find::name);
}

sub Parse_ini {
    # only operate on .ini files
	/\.ini$/ or return;
    push (@ini_files, $File::Find::name);
}

sub usage($)
{
    my $error = shift;
    my $fh = $error == 0 ? *STDOUT : *STDERR;
    print $fh "hlm_prep_package.pl\n" .
            "Specify package path or csv file\n" .
            "synopsis:\n" .
            "  hlm_prep_package.pl --help\n" .
            "  hlm_prep_package.pl [--package-path=DIR] [--csv-file=FILE] [--bld-drive=DRIVE] \n" .
            "options:\n" .
            "  --help                        Display this help and exit.\n" .
            "  --package-path=DIR            DIR is the package location, e.g. H:\\sf\\app\\musicplayer.\n" .
            "  --csv-file=FILE               FILE is the full path to the csv file.\n" .
            "  --bld-drive=DRIVE             DRIVE is the subst'd drive on which the build is stored.\n";
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
