# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# The SmokeTestROM package contains all the subroutines needed to build a ROM for
# automated smoke testing.
# This script was born on 18/04/2005
# Revision number 1: 11/08/2005
# 
#

#!/usr/bin/perl -w

package SmokeTestROM;
use strict;

# Define Core IMAGE file name as created by rombuild
my $coreImage = "*techview.nand.img";

# Define the target filename for the Core ROM
my $newCoreImage = "core.img";

# Define ROFS IMAGE file name as created by rombuild
my $ROFSImage = "*techview.nand.rofs.img";

# Define the target file name for the ROFS ROM
my $newROFSImage = "rofs1.img";

# Define NAND image file name.
my $nandImage = "*ARMV5.IMG";
#
# CR 0844 (Both H2 and H4 use -D_NAND2)
# PrepareNANDROM  - subroutine to create the NAND ROM for smoke testing; zero arguments
# USAGE: PrepareNANDROM("h2"); 
# USAGE: PrepareNANDROM("h4hrp"); 
#
sub PrepareNANDROM
{
	my ($arg) = @_; # Grab the first and only argument
	
	# Change dir to NAND Loader directory
	chdir "$SmokeTest::NandDir" or &SmokeTest::DieLog("Can't cd to $SmokeTest::NandDir");
	
	if(lc($arg) eq "h2") # Here we are in the H2 NAND case
	{
		# Run the rom command to create the image ... H2 also to use -D_NAND2
		SmokeTest::ExecCommand ("rom -v=h2 -i=armv5 -t=nandtest_load_noext -d=_NAND2 -b=urel");
	}
	elsif(lc($arg) eq "h4hrp") # Here we are in the H4 NAND case
	{
		SmokeTest::ExecCommand ("rom -v=h4hrp -i=armv5 -t=nandtest_load_noext -d=_NAND2 -b=urel");
	}
	
	# Change directory to Smoketest directory
	chdir "$SmokeTest::SmokeTestDir" or &SmokeTest::DieLog("Can't cd to $SmokeTest::SmokeTestDir");
	
	# Get the filename of the NAND image file
	my @NANDname = glob("$SmokeTest::NandDir\\$nandImage");
	
	# There should be exactly one file matching $nandImage. If not give warning.
	if(@NANDname != 1)
	{
		print "PrepareNANDROM: warning: " . scalar(@NANDname) . " files matching $nandImage in $SmokeTest::NandDir\\: [@NANDname].\n";
	}
	
	# Attempt to delete any files already in this directory left over from previous runs
	print "PrepareNANDROM: attempting to delete any debris from the last run:\n";
	SmokeTest::ExecCommand ("DEL /Q $nandImage");

	for my $filename(@NANDname)
	{
		if((lc($arg) eq "h2") && ($filename =~ /h2/i)) # This is the H2 NAND case, so copy H2 image
		{
			# Copy $nandImage to the current working directory
			print "PrepareNANDROM: copying $filename to .\n";
			SmokeTest::ExecCommand ("COPY $filename");
		}
		elsif((lc($arg) eq "h4hrp") && ($filename =~ /h4hrp/i)) # This is the H4 NAND case, so copy H4 image
		{
			# Copy $nandImage to the current working directory
			print "PrepareNANDROM: copying $filename to .\n";
			SmokeTest::ExecCommand ("COPY $filename");
		}
	}
}

#
# CR 0844 (Both H2 and H4 use -D_NAND2)
# CreateAndZipROM creates the various ROMS; up to 5 arguments.
# Usage:
# CreateAndZipROM("ARMV5", "lubbock", "-DRVCT", "PlatSec");
# CreateAndZipROM("ARMV5", "h2", "-DRVCT", "PlatSec");
# CreateAndZipROM("ARMV5", "h2", "-DRVCT", "PlatSec", "-D_NAND2");
# CreateAndZipROM("ARMV5", "h4hrp", "-DRVCT", "PlatSec", "-D_NAND2");
#
sub CreateAndZipROM
{
	my @args = @_; # Array to hold the arguments
	
	# This sixth argument $args[5] determines whether to build the NAND version
	# of the ROM or not. Set it equal to the empty string if the argument
	# isn't defined otherwise it should equal the argument
	my $nandROM;
	if(defined $args[5])
	{
		if($args[5] =~  /-D_NAND/)
		{
			$nandROM = $args[5];
		}
		else
		{
			die "Invalid final argument in CreateAndZipROM. Exiting.\n";
		}
	}
	else
	{
		$nandROM = "";
	}
	
	print "\n";
	# 
	# Build ROM with STAT run automatically
	#
	print "\n";
	print "#\n# Build $args[0] $args[1] ROM with STAT run automatically\n#\n"; 
	print "\n";
	SmokeTest::PrintDateTime();
	
	my $eabi = "";
	if ($args[0] eq "ARMV5")
	{
		$eabi = "-D_EABI=$args[0]";
	}

    my ($iProductNum,$buildrom);
	$iProductNum = $args[4];
	
	if ($iProductNum eq 'Future' || $iProductNum  >= 9.5)
	{
####????DavidZjang#$buildrom = SmokeTest::ExecCommand ("buildrom -D_STARTUPMODE2 -D_EABI=ARMV5 -fm=\\epoc32\\rom\\include\\featuredatabase.xml h4hrp techview statauto.iby -osys$rom.bin");
####????MCL########$buildrom = SmokeTest::ExecCommand ("buildrom -D_STARTUPMODE2 $eabi -fm=\\epoc32\\rom\\include\\featuredatabase.XML GTC_Standard statauto.iby testconfigfileparser.iby");
	    $buildrom = SmokeTest::ExecCommand ("buildrom -D_STARTUPMODE2 $eabi -fm=\\epoc32\\rom\\include\\featuredatabase.XML GTC_Standard statauto.iby testconfigfileparser.iby");
	}
	else
	{
	    $buildrom = SmokeTest::ExecCommand ("buildrom -D_STARTUPMODE2 $eabi -fr=\\epoc32\\rom\\include\\featureUIDs.XML GTC_Standard statauto.iby testconfigfileparser.iby");
	}
	print $buildrom;
	
	if ( ($buildrom =~ m/rombuild.*failed/i) )
	{
		print "\n*** Error In Smoke Test ***\n Building the ROM failed\n$!\n";
		#&SmokeTest::DieLog("Error In Smoke Test: Building the ROM failed");
	} 
	else 
	{
		print "\n*** ROM built successfully ***\n";	
	}
	
	if ($nandROM =~ /-D_NAND/)
	{
		# Need to rename each on individually as there are several files ending in ".img"
		print "\n";
		print "#\n# Renaming NAND ROMs\n#\n"; 
		print "\n";
		SmokeTest::ExecCommand ("MOVE /Y $ROFSImage $newROFSImage");
		SmokeTest::ExecCommand ("MOVE /Y $coreImage $newCoreImage");

		# Copy each of the image files to the \epoc32\rom directory
		print "\n";
		print "#\n# Copying NAND ROMs\n#\n"; 
		print "\n";
		SmokeTest::ExecCommand ("COPY $newROFSImage \\epoc32\\rom\\");
		SmokeTest::ExecCommand ("COPY $newCoreImage \\epoc32\\rom\\");
		
		# Delete the images
		print "\n";
		print "#\n# Deleting images\n#\n"; 
		print "\n";
		SmokeTest::ExecCommand ("DEL *.img");

#  passing $args[1] i.e. h2 / h4hrp to PrepareNANDROM due to CR 0844 (Both H2 and H4 use -D_NAND2)
    	&PrepareNANDROM("$args[1]");
	}
	
	# Rename the IMG file to sys$rom.bin
	my $ImgDir = SmokeTest::ExecCommand ("DIR *.img");
		
	$ImgDir =~ /(\S+\.img)/i;
		
	#&SmokeTest::DieLog("Building ROMs failed: Cannot find .IMG file") unless defined $1;
	print "Name of ROM : $1 \n";
	SmokeTest::ExecCommand ("MOVE /Y $1 sys\$rom.bin");
	print "\n";
		
	# 
	# ZIP ROM using \epoc32\tools\zip.exe
	# 
	print "\n";
	print "#\n# ZIP ROM using \\epoc32\\tools\\zip.exe\n#\n";
	print "\n";
	SmokeTest::PrintDateTime();
	
	# Now zip the file
	my $zipresult = SmokeTest::ExecCommand ("$SmokeTest::BuildDir$ENV{EPOCROOT}epoc32\\tools\\zip.exe -Tv sys\$rom sys\$rom.bin");
	print "\n";
	
	# Test for zip errors
	if ($zipresult =~ m/No errors detected/i) # Good case: no errors detected.
	{
		print "*** ROM zip successful: no errors detected ***\n";	
	}
	elsif ($zipresult =~ m/zip error/i) # Zip error
	{
		print "*** Error In Smoke Test ***\n Zipping the ROM failed $!\n";
		#&SmokeTest::DieLog("Error In Smoke Test: Zipping the ROM failed");
	}
	else # Worst case
	{
		print "*** Error In Smoke Test ***\n Zipping unable to start $!\n";
	}
	
	# Form the string for the temporary zip file locations
	my $tempZipLocation = "$SmokeTest::SmokeTestDir\\$args[0]\\$args[1]";
	if ($nandROM =~ /-D_NAND/) # Add \NAND folder if a NAND ROM is being built
	{
		
		if ($args[2] eq "pagedrom") # Add \NAND_DP folder if a Demand Paging NAND ROM is being built
		{
			$tempZipLocation .= "\\NAND\(DP\)";
		}
		else
		{
			$tempZipLocation .= "\\NAND";
		}
	}

	# Make the temp zip folder
	SmokeTest::ExecCommand ("MKDIR $tempZipLocation");
	print "\n";
	
	# Copy the ROM to the appropriate directory to be copied later
	SmokeTest::ExecCommand ("MOVE /Y sys\$rom.zip $tempZipLocation\\sys\$rom.zip");
	
	print "\n";
	
	SmokeTest::PrintDateTime();
}

1;
