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
# Adds info form a file to a CSV

use strict;
use HTTP::Date;

my $srccsvfile = shift @ARGV;
my $whatlogfile = shift @ARGV;
my @sysmodelfiles = @ARGV;

if(! -e $srccsvfile)
{
  die "cannot find $srccsvfile\n";
}

if(! -e $whatlogfile)
{
  die "cannot find $whatlogfile\n";
}

foreach my $sysmodelfile (@sysmodelfiles)
{
	if(! -e $sysmodelfile)
	{
	  die "cannot find $sysmodelfile\n";
	}
}

# Search for timestamp threshold in whatlog file
# It corresponds to the first timestamp for which the target was genuinly rebuilt
# Every target that has a timestamp below is considered as not rebuilt properly
open(WHATLOG,"<$whatlogfile") or die "Error: Couldn't open $whatlogfile\n";
my %bldtimes;
my $timestampLimit = 0;
while (my $line = <WHATLOG>)
{
	if($line =~ m/,\s*(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})\s*$/){
		$bldtimes{str2time($1)} = $1;
	}
}
my $previoustime = 0;
foreach my $decimaltime (sort(keys %bldtimes))
{
  if ($previoustime == 0) {$previoustime = $decimaltime;}
  if ( ($decimaltime - (30*60) ) > $previoustime )
  {
	$timestampLimit = $bldtimes{$decimaltime};
	print "Timestamp Threshold found: $timestampLimit\n";
	last;
  }
}
close(WHATLOG);

# Create the Package List from sources.csv
# And assign license type
# Everything that is not found FCL or MCL are considered RND
my @pkgList;
open(SRCCSV,"<$srccsvfile") or die "Error: Couldn't open $srccsvfile\n";
while (my $line = <SRCCSV>)
{
	if ($line =~ m/^source,/) {next;}
	if ($line =~ m/^([^,]+),\/([^,]+),/){
		my $pkglicense = $1;
		my $pkgpath=$2;
		if ($pkglicense =~ m/^[^,]+\/(\w+)\/(MCL|FCL)\/\w+/i) {$pkglicense = uc($1);}
		else {$pkglicense = "RND";}
		push @pkgList, "$pkgpath,$pkglicense";
	}
}
print "Number of Packages to analyse: $#pkgList\n";
close(SRCCSV);

# Create System Definition Component List
my @sysdefcomplist;
foreach my $sysdeffile (@sysmodelfiles)
{
	open(SYSDEF,"<$sysdeffile") or die "Error: Couldn't open $sysdeffile\n";
	while (my $line = <SYSDEF>)
	{
		if ($line =~ m/(\s*)<!--\s*(<.*?>)\s*-->/) {next;}
		if ($line =~m/bldFile=\"([^"]*)"/i)
		{
			#push @sysdefcomplist, &getComponentInfo($1);
			push @sysdefcomplist, $1;
		}
	}
	close(SYSDEF);
}

# For each package in the list, process the whatlog to verify if it's built clean
# And build the final summary log
open(WHATLOG,"<$whatlogfile") or die "Error: Couldn't open $whatlogfile\n";
my @whatlogs = <WHATLOG>;
my @outputList;
foreach my $package (@pkgList)
{
	my $corruptbldinfcount = 0;
	# Write a CSV entry for pkgpath, license
	my $pkgline = "$package";
	# Search whatlog and match -> extract bldfiles -> clean vs not_clean
	$package =~ s/,.*//;
	foreach my $whatlog (@whatlogs)
	{	
		if ($whatlog =~ /^[^,]+,($package\/[^,]+),[^,]*,[^,]*,[^,]*,[^,]*(,([^,]*))?(,([^,]*))?/)
		{
			my $bldfilepath = $1;
			my $status = $3;
			my $timestamp = $5;
			if (($status =~ /fail/i) || ($status eq "") ||
				(($status =~ /untouched/i) and ($timestamp ne "") and (str2time($timestamp) < str2time($timestampLimit))) )
			{
				chomp($status);chomp($timestamp);
				$status = "KO ($status - $timestamp)";
			}
			else {$status = "OK"; }
			#if ($status =~ /KO/ ) {print "$pkgline,$bldfilepath,$status\n";}
			push @outputList, "$pkgline,$bldfilepath,$status\n";
		}
	}
}

# Write the Full Analysis log
open(OUTPUT,">PkgComponentAnalysisFull.csv") or die "Error: Couldn't open PkgComponentAnalysisFull.csv for writing\n";
print OUTPUT "Package Path (from Sources.csv), License, BldFile (from whatlog), Status\n"; 
print OUTPUT @outputList;
close(OUTPUT);

# Keep only uniq bldfile in the list
# And calculate final status
#my @uniq = keys %{{ map { $_ => 1 } @outputList }};
my %seen = ();
my $value; my $remains; my $status;
foreach my $item (@outputList) {
	$item =~ /^([^,]+,[^,]+,([^,]+)),([^,]+)/;
	$remains = $1;
	$value = $2;
	$status = $3;
	chomp($status);
	if ($status ne "OK") {$status = "Bad"};
    if ($seen{$value}) 
	{
		$seen{$value} =~ /^[^,]+,([^,]+)/;
		my $currentstatus = $1;
		if ($currentstatus eq "Bad")
		{
			$status = "Bad";
		}
	}
	$seen{$value} = "$remains,$status\n";
}
my @uniq = values(%seen);

# Prepend system model info (block name, component name)
foreach my $line (@uniq)
{
  chomp $line;
	if ($line =~ /^[^,]+,[^,]+,([^,]+),/)
	{
		my $bldfile = $1;
		my $sysmodelinfo = &getSysModelInfo($bldfile);
		$line = "$line, $sysmodelinfo\n";
	}
}


# Write the summary log
open(OUTPUT,">PkgComponentAnalysisSummary.csv") or die "Error: Couldn't open PkgComponentAnalysisSummary.csv for writing\n";
print OUTPUT "Source Path (from Sources.csv), License, BldFile (from whatlog), Package Name, Component Name\n"; 
print OUTPUT @uniq;
close(OUTPUT);
close(WHATLOG);

# Status statistics
my $componentOkCount = 0;
my $componentKoCount = 0;
my $totalcompCount = 0;
foreach my $line (@uniq)
{
	if ($line =~ /OK$/) {$componentOkCount++;}
	else {$componentKoCount++;}
	$totalcompCount++;
}
print "Total number of Components: $totalcompCount\n";
print "Number of Components Cleanly rebuilt: $componentOkCount\n";
print "Number of Components not rebuilt properly: $componentKoCount\n";



sub getSysModelInfo
{
	my $bldfilepath = shift;
	
	# remove prepending sf dir, and trailing bld.inf
	$bldfilepath =~ s/^sf\///;
	$bldfilepath =~ s/\/bld.inf$//i;
	#print "Looking for bldfile path: $bldfilepath ...\n";
	
	my $packageName = "";
	my $componentName = "";
	
	MODEL_LOOP:	foreach my $sysmodelfile (@sysmodelfiles)
	{
		open(SYSFILE,"<$sysmodelfile") or die "Error: Couldn't open $sysmodelfile\n";
		my @model_lines = <SYSFILE>;
		foreach my $line (@model_lines)
		{
			$line =~ s/\\/\//g;
			
			if ($line =~ m/bldFile=\"$bldfilepath"/i)
			{
				#print "Component $packageName/$componentName found in model $sysmodelfile\n"; 
				last MODEL_LOOP;
			}
			elsif ($line =~ m/<component/)
			{
				$line =~m/\slong-name="([^"]*)"/;
				$componentName = $1;
			}
			elsif ($line=~ m/<\/component/)
			{
				$componentName = "";
			}
			elsif ($line =~ m/<block/)
			{
				$line =~m/\slong-name="([^"]*)"/;
				$packageName = $1;
			}
			elsif ($line=~ m/<\/block/)
			{
				$packageName = "";				
			}
		}
		close(SYSFILE);
	}
	
	# One mode file remains open if match found
	if ($packageName ne "") { close(SYSFILE)}
	
	return "$packageName, $componentName";
}