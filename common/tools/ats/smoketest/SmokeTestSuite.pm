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
# The SmokeTestSuite package contains subroutines that are needed to build a 
# testdriver test suite.
# This script was born on 18/04/2005
# 
#

#!/usr/bin/perl -w
package SmokeTestSuite;
use strict;

my $servicename;

#
# BuildAndPackage: Used to create a test package for remote execution on the ROM
#
sub BuildAndPackage
{
	#
	# Build Test Package for $_[0]
	#
	print "\n#\n# Build Test Package for $_[0]\n#\n";
	SmokeTest::PrintDateTime();
	my $testbuild = SmokeTest::ExecCommand ("$SmokeTest::TestDriverCmd build -p $_[0] -b udeb -s $SmokeTest::TestSuite");

	# now check for results
	$testbuild =~ m/fail\:\s(\d+)\s\%/i;
	
	if ( ($testbuild =~ m/\*\*error\:/i) || $1 == 100 )
	{
		print "\nREMARK: Smoke Test *** Building the Test failed $!\n";
		#&SmokeTest::DieLog("Error In Smoke Test: Building the Test failed");
	} 
	else 
	{
		print "\n*** Test Building successful ***\n";	
	}
	
	SmokeTest::PrintDateTime();
	
	my $testpackage = SmokeTest::ExecCommand ("$SmokeTest::TestDriverCmd package -p $_[0] -b udeb -s $SmokeTest::TestSuite --tp $SmokeTest::SmokeTestDir\\$_[0].tpkg");
	print "\n";
	
	if (($testpackage =~ m/\*\*error/i))
	{
		print "\nREMARK: Smoke Test *** Building the Test Package failed\n\"$1\"\n";
		#&SmokeTest::DieLog("Error In Smoke Test: Building the Test Package failed");
	} 
	else 
	{
		print "\n*** Building the Test Package successful ***\n";	
	}
	
	SmokeTest::PrintDateTime();
}

#
# BuildAndPublish: used to build and test the smoketest test suite on an Emulator
#
sub BuildAndPublish
{
	my $runid = 0;
	my $buildname;
	
	#
	# Build Test Suite for $_[0]
	#
	print "\n#\n# Build Test Suite for $_[0]\n#\n";
	SmokeTest::PrintDateTime();
	my $testbuildwinscw = SmokeTest::ExecCommand ("$SmokeTest::TestDriverCmd build -p $_[0] -b udeb -s $SmokeTest::TestSuite");

	# now check for results
	$testbuildwinscw =~ m/fail\:\s(\d+)\s\%/i;
	
	if ( ($testbuildwinscw =~ m/\*\*error\:/i) || $1 == 100 )
	{
		print "\nREMARK: Smoke Test *** Building the Test for $_[0] failed $!\n";
		#&SmokeTest::DieLog("Error In Smoke Test: Building the Test failed");
	} 
	else 
	{
		print "\n*** Test Build successful ***\n";	
	}
	
	#
	# Run Test Suite for $_[0]
	#
	print "\n";
	print "#\n# Run Test Package for $_[0]\n#\n";
	print "\n";
	SmokeTest::PrintDateTime();
	
	SmokeTest::ExecCommand ("$SmokeTest::TestDriverCmd run -p $_[0] -b udeb -s $SmokeTest::TestSuite");
	
	SmokeTest::PrintDateTime();
	
	#
	# Publish results of emulator onto publish server
	#
	print "\n";
	print "#\n# Publish results of emulator onto publish server\n#\n";
	print "\n";
	
	#
	# Find the correct directory to publish
	#
	my $source = "$SmokeTest::TestDriverDrive\\TestDriver\\Results";
	if (opendir Results, $source)
	{	
		my @dirArray = readdir(Results);
		
		#print "@results";	
		# to remove "." and ".." from this list
		shift @dirArray;
		shift @dirArray;
		
		if (!scalar(@dirArray))
		{
			print "REMARK: The emulator for $_[0] has an empty $source directory";
			return;
		}
		else
		{
			print "\nContents of $source = @dirArray\n";
			$source .= "\\$dirArray[0]"; 
			# dirArray should always have just the one subdir - 0XXXX_Symbian_OS_vX.X
			
			$buildname = $dirArray[0];
			
			if (opendir tempDir, "$source")
			{
				# to remove "." and ".." from this list
				@dirArray = readdir(tempDir);
				shift @dirArray;
				shift @dirArray;
				
				print "\nContents of $source = @dirArray\n";
				
				foreach my $directory (@dirArray)
				{
					if ($directory =~ /(?:$_[0])/i)
					{
						$source .= "\\$directory";
						
						# added this to remove udeb/xx
						if (opendir tempDir, "$source")
						{
							# to remove "." and ".." from this list
							@dirArray = readdir(tempDir);
							shift @dirArray;
							shift @dirArray;
				
							print "\nContents of $source = @dirArray\n";
				
							foreach my $directory (@dirArray)
							{
								if ($directory =~ /(udeb|urel)/i)
								{
									$source .= "\\$directory";
									if (opendir tempDir, "$source")
									{
										# to remove "." and ".." from this list
										@dirArray = readdir(tempDir);
										shift @dirArray;
										shift @dirArray;
				
										print "\nContents of $source = @dirArray\n";
				
										my @iNumericDirList = ();
										foreach my $directory (@dirArray)
										{
											if ($directory =~ /(^[0-9]+)/i)
											{
    											push @iNumericDirList, $directory;
											}
										}
									    @iNumericDirList = sort { $b <=> $a } @iNumericDirList;
									    $runid = $iNumericDirList[0];
                                        $source .= "\\$runid";
									    last;
									}
									last;
								}
							}
						}
						
					}
				}
				
				print "\nThe final source directory = $source\n";
			}
			else
			{
				print "REMARK: The emulator for $_[0] has an empty $source directory";
				return;
			}
		}
		my $emulatorpublish = "$SmokeTest::PublishServer\\$SmokeTest::ProductType\\$SmokeTest::BuildNum\\logs\\SmokeTest\\Emulator\\$_[0]";
		
		SmokeTest::ExecCommand ("MKDIR $emulatorpublish"); # or GoNext("$publishlocation\\$name\\");
		
		SmokeTest::ExecCommand ("XCOPY /C /Y /E $source\\xml $emulatorpublish\\xml\\");
		
		#
		# Copy runXX_<snapshot>_SymbianOS_vYY.xml/html to <snapshot>_Symbian_OS_vYY.xml/html
		#
		
		my $xmlfile   = "$source\\run".$runid."_".$buildname."\.xml";
		my $htmlfile  = "$source\\run".$runid."_".$buildname."\.html";
		
		if (-e $xmlfile)
		{
			SmokeTest::ExecCommand ("COPY $xmlfile $emulatorpublish\\$buildname.xml");
		}
		
		if (-e $htmlfile)
		{
			SmokeTest::ExecCommand ("COPY $htmlfile $emulatorpublish\\$buildname.html");
		}
		
		#
		# Publish results of emulator onto logs dir
		#
		print "\n";
		print "#\n# Publish results of emulator onto Logs Dir\n#\n";
		print "\n";
		
		my $emulatorpublish = "$SmokeTest::LogsDir\\SmokeTest\\Emulator\\$_[0]\\";
		
		SmokeTest::ExecCommand ("MKDIR $emulatorpublish"); # or GoNext("$publishlocation\\$name\\");
		
		SmokeTest::ExecCommand ("XCOPY /C /Y /E $source $emulatorpublish");
	}
	else
	{
		print "REMARK: Cannot open dir $source: $!";
		return;
	}
	SmokeTest::PrintDateTime();
}

#
# PollServers: Polls all the servers from the Pool and picks out the one with the shortest queue.
#
sub PollServers
{
	#
	# Parse ENV and create an array of smoketest servers for $_[0]
	#
	print "\n";
	print "#\n# Parse ENV and create an array of smoketest servers for $_[0]\n#\n";
	print "\n";
	SmokeTest::PrintDateTime();
	
	if (($_[0] eq "Lubbock") || ($_[0] eq "LUBBOCK"))
	{
		$SmokeTest::servicename = "RemoteTestDriverLubbock";
	}
	elsif ($_[0] eq "H2")
	{
		$SmokeTest::servicename = "RemoteTestDriverH2";
	}
        elsif ($_[0] eq "H4HRP")
	{
		$SmokeTest::servicename = "RemoteTestDriverH4HRP";
	} 
	else
	{
		&SmokeTest::DieLog("Cannot poll serverpool for pool \"$_[0]\"");
	}
	
	my @servers = split /\#/, $SmokeTest::serverpool;
	if (@servers < 1)
	{
		&SmokeTest::DieLog("*** Error In Smoke Test ***\nNo SmokeTest Servers specified for $_[0]$!\n");
	}
	else
	{
		print "\n*** Retrieved Pool of Smoke Test Servers from ENV for $_[0]***\n";
	}
	
	#
	# Poll each of the smoke test servers and parse the results
	#
	print "\n";
	print "#\n# Poll each of the smoke test servers and parse the results\n#\n";
	print "\n";
	
	my %serverlist;
	
    print "Checking TestDriver Version\n";
    SmokeTest::ExecCommand ("$SmokeTest::TestDriverCmd version");

	foreach my $server (@servers)
	{
		print "Polling Server: $server\n";
        my $masterstatus = SmokeTest::ExecCommand ("$SmokeTest::TestDriverCmd masterstatus --srv //$server/$SmokeTest::servicename");
		
		my $value = -2;
		if ($masterstatus =~ m/Master: TestDriver is currently (....)/i)
		{
			if ($1 eq 'free')
			{
				#print "Free!\n";
				$value = 0;
			}
			else
			{
				$masterstatus =~ m/are\s+(\w+)\s+jobs/i;
				if ($1 =~ m/\D+/)
				{
					$value = 1;
				}
				else
				{
					$value = $1 + 1;
				}
			}
		}
		else
		{
			# The master is not running a service
			$value = -1;
		}
		$serverlist{$server} = $value;
		print "\n"; 
	}
	
	#
	# Decide which server to use
	#
	print "\n";
	print "#\n# STEP8 Decide which server to use\n#\n";
	print "\n";
	
	my $masterserver;
	
	# Loop through first time only to print out all the available servers
	print "\nAvailable Servers\n";
	foreach my $servername (sort { $serverlist{$a} <=> $serverlist{$b} } keys %serverlist)
	{
		print "\n$servername : number in queue are $serverlist{$servername}\n";
	}
	
	# Loop through second time to actually select the correct server
	foreach my $servername (sort { $serverlist{$a} <=> $serverlist{$b} } keys %serverlist)
	{
		if ($serverlist{$servername} > -1)
		{
			$masterserver = $servername;
			last;
		}
	}
	
	if (defined $masterserver)
	{
		print "\n*** The Smoke Test Server to be used for $_[0] is $masterserver ***\n";
	}
	else
	{
		print "\nREMARK: Smoke Test *** No available Smoke Test Servers for $_[0] \n";
		#&SmokeTest::DieLog("Error In Smoke Test: No available Smoke Test Servers for $_[0]");
	}
	SmokeTest::PrintDateTime();
	return $masterserver;
}

#
# Usage: RunRemote(Architecture, Testing Board, Server)
#
sub RunRemote
{
	my $masterserver = $_[2];
	
		
	#
	# Call TestDriver runremote for $_[0]
	#
	print "\n";
	print "#\n# Call TestDriver runremote for $_[0]\n#\n";
	print "\n";
	SmokeTest::PrintDateTime();
	
	my $tempZipLocation = "$SmokeTest::SmokeTestDir\\$_[0]\\$_[1]";
	if ($_[3] eq "NAND\\")
	{
		$tempZipLocation .= "\\NAND";
	}
	elsif ($_[3] eq "NAND\(DP\)\\")
	{
                $tempZipLocation .= "\\NAND\(DP\)";
	}

	SmokeTest::ExecCommand ("COPY /Y $tempZipLocation\\sys\$rom.zip $SmokeTest::SmokeTestDir\\sys\$rom.zip");
	
	# Decide whether this is a platsec ROM or not
	my $platsec;
	if ($SmokeTest::buildNo eq "9.1" || $SmokeTest::buildNo eq "9.2" || $SmokeTest::buildNo eq "9.3" || $SmokeTest::buildNo eq "Future" || $SmokeTest::buildNo eq "9.4" || $SmokeTest::buildNo eq "9.5" || $SmokeTest::buildNo eq "9.6" || $SmokeTest::buildNo eq "tb91")
	{
		$platsec = "ON";
	}
	else
	{
		$platsec = "OFF";
	}
	
	# calling testdriver for $_[0]
	my $runremote = SmokeTest::ExecCommand ("$SmokeTest::TestDriverCmd runremote -m async -i $SmokeTest::SmokeTestDir --tp $_[0].tpkg -r sys\$rom.zip --srv //$masterserver/$SmokeTest::servicename --platsec $platsec --testexec ON");
	
	#
	# Parse output to get the job number or error
	#
	print "\n";
	print "#\n# Parse output to get the job number or error\n#\n";
	print "\n";
	
	my $jobid;
	
	if ($runremote =~ m/job id/i)
	{
		$runremote =~ m/job id: (\d+)/i;
		$jobid = $1;
		print "\n*** Job submitted successfully ***\n\nJob Id = $jobid\n";
	}
	else
	{
		print "\nREMARK: Smoke Test Job Submission failed\n";
		#&SmokeTest::DieLog("Error In Smoke Test: Submitting the Job failed");
		return;
	}
	
	#
	# Create a text file with publish details
	#
	print "\n";
	print "#\n# Create a text file with publish details\n#\n";
	print "\n";
	
	my $publishstring;
	
	$publishstring = "Publish: $SmokeTest::PublishServer\\$SmokeTest::ProductType\\$SmokeTest::BuildNum\\logs\\Smoketest\\$_[0]\\$_[1]\\";
	print "$publishstring";
	
	if ( (defined $SmokeTest::PublishServer) && (defined $SmokeTest::ProductType) && (defined $SmokeTest::BuildNum) )
	{
		open PublishFile, ">$_[0]$_[1].txt" or &SmokeTest::DieLog("Cannot open File $_[0]$_[1].txt: $!\n");
		$publishstring = "$SmokeTest::PublishServer\\$SmokeTest::ProductType\\$SmokeTest::BuildNum\\logs\\Smoketest\\$_[0]\\$_[1]\\";
		if ($_[3] eq "NAND\\")
		{
			$publishstring .= "NAND\\";
		}
		elsif ($_[3] eq "NAND\(DP\)\\")
		{
            $publishstring .= "NAND\(DP\)\\";
	    }
		print PublishFile "$publishstring";
		$publishstring = "\n$SmokeTest::PublishServer\\$SmokeTest::ProductType\\logs\\$SmokeTest::BuildNum\\Smoketest\\$_[0]\\$_[1]\\";
		if ($_[3] eq "NAND\\")
		{
			$publishstring .= "NAND\\";
		}
		elsif ($_[3] eq "NAND\(DP\)\\")
		{
                $publishstring .= "NAND\(DP\)\\";
        }
		print PublishFile "$publishstring";
		close PublishFile;
		print "\n*** Publish file created successfully ***\n";
	}
	else
	{
		print "\nREMARK: Smoke Test *** Could not Create Publish File\n$!";
		#&SmokeTest::DieLog("Error In Smoke Test: Could not Create Publish File");
	}
	
	#
	# Copy This publish file to the correct directory 
	#
	print "\n";
	print "#\n# Copy This publish file to the correct directory \n#\n";
	print "\n";
	
	###print "\nabout to move /Y $_[0]$_[1].txt \\\\$masterserver\\Jobs\\$jobid\\publish.txt\n";
    SmokeTest::ExecCommand ("MOVE /Y $_[0]$_[1].txt \\\\$masterserver\\Jobs\\$jobid\\publish.txt");
	
	SmokeTest::PrintDateTime();
}

1;
