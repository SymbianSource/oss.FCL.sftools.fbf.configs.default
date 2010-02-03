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
# SmokeTestSuite.pm 
# The SmokeTestSuite package contains subroutines that are needed to build a 
# testdriver test suite.
# This script was born on 18/04/2005
# 
#

#!/usr/bin/perl -w
package SmokeTest;
use SmokeTestROM;
use SmokeTestSuite;
use FindBin;
use lib "$FindBin::Bin\\..\\..\\..\\..\\os\\buildtools\\bldsystemtools\\commonbldutils\\GenResult\\";
use FileRead; #Need to include a Findbin::path
use strict;

#
# Global Variables
#
our $BuildDir = $ENV{'BuildDir'}; # usually M:
our $serverpool = $ENV{'SmokeTestServerPool'};
our $buildNo = $ENV{'BuildShortName'}; # Eg. 9.1
# The Client is run from the dir where SmokeTestClient.pl is located
# This is to ensure that any ROMs and test packages created are stored there.
our $sourceDir = "$BuildDir\\src\\common\\testtools";
our $TestSuite = "SmokeTest";
our $SmokeTestDir;
our $NandDir;
our $TestDriverLocation   = "\\epoc32\\tools\\TDEP";   # location for TD zips
our $TestDriverDrive = "M:";
our $TestDriverCmd = "$TestDriverDrive\\testdriver\\testdriver.cmd";
our $TestDriverXML;

# Publish details
our $PublishServer = $ENV{'PublishLocation'};
our $ProductType   = $ENV{'Type'};
our $BuildNum      = $ENV{'BuildNumber'};
our $LogsDir       = $ENV{'LogsDir'};

# Epocwind.out
my $tempDirLocation = $ENV{'TEMP'};

#
# CheckVars: ensures that the essential environment vars and directories exist
#
sub CheckVars
{
	print "\n** Settings **\n";
	print "Build Drive                = $BuildDir\n";
	print "Smoke Test Server Pool     = $serverpool\n";
	print "Build Product              = $buildNo\n";
	print "TestDriver Zip Location    = $TestDriverLocation\n";
	print "TestDriver XML Location    = $TestDriverXML\n";
	print "TestDriver Source Location = $sourceDir\n";
	print "TestDriver Drive           = $TestDriverDrive\n";
	print "SmokeTest working directory= $SmokeTestDir\n";
	print "NAND build directory       = $NandDir\n";
	
	print "\n** Publishing Information **\n";
	
	print "Publish server = $PublishServer\n";
	print "Product Type   = $ProductType\n";
	print "Build Number   = $BuildNum\n";
	
	DieLog("CheckVars: \"Build Directory\" not defined") if !defined $BuildDir;
	DieLog("CheckVars: \"Smoke Test Server Pool\" not defined") if !defined $serverpool;
	DieLog("CheckVars: \"Build Product\" not defined") if !defined $buildNo;
	DieLog("CheckVars: \"Publish server\" not defined") if !defined $PublishServer;
	DieLog("CheckVars: \"Product Type\" not defined") if !defined $ProductType;
	DieLog("CheckVars: \"Build Number\" not defined") if !defined $BuildNum;
	DieLog("CheckVars: Directory \"SmokeTest working directory\" does not exist") if ! -e $SmokeTestDir;
	DieLog("CheckVars: Directory \"TestDriver Zip Location\" does not exist") if ! -e $TestDriverLocation;
}

sub main
{
	print "\n\n*** START OF SMOKE TEST ***\n\n";

	PrintDateTime();
	
	my ($iSourceDir,       $iTestSuite,   $iProduct, $iServerPool, $iProductType,
	    $iPublishLocation, $iBuildNumber, $iXML,     $iUnzip,      $iBuildDrive, $iTDDrive) = @_;
	
	$TestSuite       = $iTestSuite           if defined $iTestSuite;
	$BuildDir        = $iBuildDrive          if defined $iBuildDrive;
	$sourceDir       = $BuildDir.$iSourceDir if defined $iSourceDir;
	$buildNo         = $iProduct             if defined $iProduct;
	$serverpool      = $iServerPool          if defined $iServerPool;
	$ProductType     = $iProductType         if defined $iProductType;
	$PublishServer   = $iPublishLocation     if defined $iPublishLocation;
	$BuildNum        = $iBuildNumber         if defined $iBuildNumber;
	$TestDriverDrive = $iTDDrive             if defined $iTDDrive;
	
	$SmokeTestDir    = "$sourceDir\\$TestSuite";
	if (defined $iXML)
	{
		$TestDriverXML = $iXML;
	}
	else
	{
		$TestDriverXML = $SmokeTestDir."\\xml";
	}
	$NandDir = "$BuildDir\\src\\cedar\\generic\\base\\e32\\rombuild";
	
	CheckVars();
	
	# change dir to Smoketest dir
	chdir "$SmokeTestDir" or DieLog("Can't cd to $SmokeTestDir");
	
	# unzip latest version of TD
	if (defined $iUnzip)
	{
		unpackTestDriver2();
	}
	
	# configure TD
	configureTestDriver2();
	
	#
	# Copy the correct configuration.cfg file
	# But such source only exists for these products: 8.0a, 8.0b, 8.1a, 8.1b, 9.0, 9.1
	#
	print "\n#\n# Copy the correct configuration.cfg file\n#\n";
	ExecCommand("COPY /Y $SmokeTestDir\\Group\\$buildNo\\configuration.cfg $SmokeTestDir\\Group\\configuration.cfg");
	
	#Read in hash of smoketests to be run for each product.
	my ($iCfgFileLocation) = "$FindBin::Bin\\..\\..\\..\\..\\os\\buildtools\\bldsystemtools\\commonbldutils\\GenResult";
	
	#Read in the smoketest list from a cfg file
	my $text = &FileRead::file_read ("$iCfgFileLocation\\Product.cfg");
	#Search for an entry matching (At the beginning of line there should be 1 or more alphanumberic chars
	#Followed by a "."(optional to allow codenames) followed by more alphanumberic chars followed by 0 or more spaces then an = then 0
	#or more spaces then any amount of chars till the end of the line.
	#8.1b = EMULATOR_WINSCW ARM4_LUBBOCK ARMV5_LUBBOCK
	my %iProducts = $text =~ /^\s*(\w+\.?\w+)\s*=\s*(.+)$/mg ;
	
	#Get the list of platforms to be smoketested for the product.
	#Need to turn the hash value into an array using split
	my @iPlatforms = split (/ /,$iProducts{$buildNo});
	#Build and Execute Test package for each platform as read from configuration file and create and zip rom.
	ProcessPlatforms (@iPlatforms);
	
	#
	# Build Test Package for ROMs
	#
	if ($buildNo eq '8.1a' || $buildNo eq '8.1b')
	{
		&SmokeTestSuite::BuildAndPackage("ARM4");
	}
	
	if ($buildNo ne '8.1a')
	{
		&SmokeTestSuite::BuildAndPackage("ARMv5");
	}
	
	#
	# Find out which server to use
	#
	print "\n#\n# Find out which server to use\n#\n";
	ProcessPlatformsforSmoketest(@iPlatforms);
	
	#
	# End of Smoke Test
	#
	print "\n\n*** END OF SMOKE TEST ***\n\n";
}
	
#
# DieLog: Exits script and inserts a Remark in the build log, which will be picked up by htmlscanlog
#
sub DieLog
{
	die "\nREMARK: $_[0]\n $!\n\n\n";
}


#
# Build and publish based on entries in the Product.cfg file
#
sub ProcessPlatforms 
{

my $i = 0;
my @temp;
my @iPlatforms = @_;

foreach my $platform (@iPlatforms)
 {  
  undef @temp;
  @temp = split (/_/,$iPlatforms[$i]);
  $i++;
  
  if( $temp[0] eq "EMULATOR")
  {
	&SmokeTestSuite::BuildAndPublish("$temp[1]");
	CheckPlatsecErrors("$temp[1]");
  }
  else
  {
		
	if(3 == scalar(@temp)) # there are 3 arguments, so this will be a NAND ROM
	{
		if((lc($temp[1]) eq "h4hrp") && (lc($temp[2]) eq "nand\(dp\)" )) # H4HRP Demand Paging ROM options is -D_NAND2
		{
			&SmokeTestROM::CreateAndZipROM("$temp[0]", "$temp[1]", "pagedrom", "PlatSec", "$buildNo", "-D_NAND2" );
		}
		elsif((lc($temp[1]) eq "h4hrp") && (lc($temp[2]) eq "nand" )) # H4 options is -D_NAND2
		{
			&SmokeTestROM::CreateAndZipROM("$temp[0]", "$temp[1]", "-DRVCT", "PlatSec", "$buildNo", "-D_NAND2");
		}
		elsif(lc($temp[1]) eq "h2") # H2 options is -D_NAND# H2 option is NAND2 due to CR 0844 (Both H2 and H4 use -D_NAND2)
                {
			&SmokeTestROM::CreateAndZipROM("$temp[0]", "$temp[1]", "-DRVCT", "PlatSec", "$buildNo", "-D_NAND2");		
		}
		else	# Bad option entered
		{
			die "SmokeTest::ProcessPlatforms error: invalid board type entered: $temp[1]. Check that Product.cfg is correct.\n";
		}		
	}
	else # This is the NOR ROM.
	{
		&SmokeTestROM::CreateAndZipROM("$temp[0]", "$temp[1]", "-DRVCT", "PlatSec", "$buildNo");
	}
  }
  
 }

}

#
# Build and publish based on entries in the Product.cfg file
#
sub ProcessPlatformsforSmoketest
{

my $i = 0;
my @temp;
my @iPlatforms = @_;

foreach my $platform (@iPlatforms)
 {  
    undef @temp;
    @temp = split (/_/,$iPlatforms[$i]);
    $i++;
    if( $temp[0] eq "EMULATOR")
    {
        next;
    }
	my $mServ = &SmokeTestSuite::PollServers("$temp[1]");
	unless (defined $mServ)
    {
        next;
    }
	if(3 == scalar(@temp))
	{
		if(lc($temp[2]) eq "nand\(dp\)" ) 
		{
		    &SmokeTestSuite::RunRemote("$temp[0]", "$temp[1]", $mServ, "NAND\(DP\)\\");
		}
        elsif(lc($temp[2]) eq "nand") 
	    {
		    &SmokeTestSuite::RunRemote("$temp[0]", "$temp[1]", $mServ, "NAND\\");
	    }
	}
	else
	{
		&SmokeTestSuite::RunRemote("$temp[0]", "$temp[1]", $mServ);
	}
 }
}
 
#
# Check for platsec errors - checks the epocwind.out on build machine
#
sub CheckPlatsecErrors
{
	print "\n";
	print "#\n# Check for platsec errors\n#\n";
	print "\n";
	PrintDateTime();
	
	# open epocwind.out and parse for both errors and warnings
	open EPOCWIND, "$tempDirLocation\\epocwind.out" or print "\nERROR: Cannot open EPOCWIND.OUT\n";
	my @epocWind = <EPOCWIND>;
	close EPOCWIND;
	
	foreach my $tempEW (@epocWind)
	{
		if ($tempEW =~ m/(\*PlatSec\* ERROR)(.*)/i)
		{
			print "\nERROR: $_[0] - $1$2";
		}
		elsif ($tempEW =~ m/(\*PlatSec\* WARNING)(.*)/i)
		{
			print "\nWARNING: $_[0] - $1$2";
		}
	}
}

#
# Take TestDriver and unpack into a given location.
#
sub unpackTestDriver2()
{
	#
	# Unzip TestDriver
	#
	print "\n#\n# Unzip TestDriver\n#\n";
	PrintDateTime();
	
	# first remove the "old" version of test driver
	print "\n";
	ExecCommand("RMDIR $TestDriverDrive\\testdriver\\ /Q /S");
	
	# now unzip the latest version of testdriver which has been synced down
	ExecCommand("%EPOCROOT%epoc32\\tools\\unzip.exe $TestDriverLocation\\TestDriver2.zip -d $TestDriverDrive\\testdriver\\");
	
	# now copy the latest version of testdriver_new.cmd which has been obtained
	ExecCommand("COPY \\\\builds01\\devbuilds\\buildteam\\testdriverv2\\testdriver_new.cmd  $TestDriverCmd");
	
	# now copy the latest version of logging.properties which has been obtained
	ExecCommand("COPY \\\\builds01\\devbuilds\\buildteam\\testdriverv2\\logging.properties  $TestDriverDrive\\testdriver\\");
	
    # 11-08-08 - additional steps required for SITK1.2 version of TD2
    ExecCommand("MKDIR $TestDriverDrive\\testdriver\\eclipse\\links\\");

    ExecCommand("COPY \\\\builds01\\devbuilds\\buildteam\\testdriverv2\\testdriver.link $TestDriverDrive\\testdriver\\eclipse\\links\\");
	
	#
	# Unzip JRE v1.5
	#
	print "\n";
	print "#\n# Unzip JRE 1.5 required by TestDriver v2\n#\n";
	print "\n";
	
	ExecCommand("%EPOCROOT%epoc32\\tools\\unzip.exe \\\\builds01\\devbuilds\\BuildPCs\\Software\\JDK1.5.0_13\\jre1.5.zip -d $TestDriverDrive\\testdriver\\");

}

sub configureTestDriver2()
{
	#
	# Configure TestDriver
	#
	print "\n";
	print "#\n# Configure TestDriver\n#\n";
	print "\n";
	my $localtimes = localtime;
	PrintDateTime();
	
	ExecCommand("$TestDriverDrive\\testdriver\\testdriver.cmd config -e $BuildDir\\
								-x $TestDriverXML
								-l $TestDriverDrive\\TestDriver\\Results
								--source $sourceDir
								-s smoketest
								-r $TestDriverDrive\\TestDriver\\Repos
								-c $TestDriverDrive\\TestDriver\\Results");

	ExecCommand("$TestDriverDrive\\testdriver\\testdriver.cmd config --commdb overwrite");
	ExecCommand("$TestDriverDrive\\testdriver\\testdriver.cmd config --bldclean off");

	# platsec for > 9.x only
	if ($buildNo ne "8.1a" && $buildNo ne "8.1b")
	{
		ExecCommand("$TestDriverDrive\\testdriver\\testdriver.cmd config -p ON");
	}
	
	# Setup commdb for WINSCW smoketest
	print "\n*** Setup commdb for WINSCW to use WinTAP ***\n";
	
	# export smoketest_commdb.xml to a place that ced.exe can see it
	ExecCommand("copy $SmokeTestDir\\smoketest_commdb.xml $BuildDir\\epoc32\\WINSCW\\c\\");
	ExecCommand("$BuildDir\\epoc32\\release\\WINSCW\\udeb\\ced.exe c:\\smoketest_commdb.xml");
	
	print "\n*** Confirm TestDriver Config ***\n";
	ExecCommand("$TestDriverDrive\\testdriver\\testdriver.cmd config");
	print "\n";
}

# ExecCommand
#
# Execute a system command, having first echoed it. Display and return the output of the command.
#
# Input: Command string (without final newline)
#
# Output: Command output (including STDERR) to STDOUT
#
# Return: Command output (including STDERR)
#
sub ExecCommand
{
    my $iCmd = shift;
    print "\n================================================================\n";
    print "Command: $iCmd\n";
    print   "----------------------------------------------------------------\n";
    my $iRet = `$iCmd 2>&1`;
    chomp $iRet;
    print "Return:  $iRet\n";
    print   "================================================================\n";
    return $iRet;
}

# PrintDateTime
#
# Print to STDOUT, Current Date and Time
#
# Input: none
#
# Output: text to STDOUT
#
sub PrintDateTime
{
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
    #### my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday) = gmtime(time);
    $year += 1900;
    $mon += 1;
	printf "\n*** Current Time: %04d/%02d/%02d %02d:%02d:%02d ***\n", $year,$mon,$mday,$hour,$min,$sec;
}

1;
