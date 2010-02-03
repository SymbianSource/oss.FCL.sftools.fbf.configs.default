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
# TestServer.pl 
# This script was born on 19/10/2004
# 
#

use strict;

my $jobsfolder = "d:\\Jobs\\";

# Loop forever
while (1)
{
	open LogFile, ">>d:\\SmokeTestServerLog.txt" or die "Cannot open log file: $!";
	
	print "\n***********************\n";
	print "\n*** START PUBLISHER ***\n";
	print "\n***********************\n";
	print "\n";
	print LogFile "\n***********************\n";
	print LogFile "\n*** START PUBLISHER ***\n";
	print LogFile "\n***********************\n";
	print LogFile "\n";
	
	my $localtimes = localtime;
	print "\n*** Current Time: $localtimes ***\n";
	print LogFile "\n*** Current Time: $localtimes ***\n";
	
	# 
	# Check if there are any results ready for publishing
	#
	print "\n";
	print "#\n# Check if there are any results ready for publishing\n#\n"; 
	print "\n";	
	print LogFile "\n";
	print LogFile "#\n# Check if there are any results ready for publishing\n#\n"; 
	print LogFile "\n";	
	
	opendir JobDir, $jobsfolder or die "Cannot open jobs dir $jobsfolder: $!";
		
	my @jobs = readdir(JobDir);
	
	# to remove "." and ".." from this list
	shift @jobs;
	shift @jobs;
	
	if (!scalar(@jobs))
	{
		print "\nNo Jobs to publish\n";
		print LogFile "\nNo Jobs to publish\n";
	}
	else
	{
		# 
		# Loop though each directory and publish logs
		#
		print "\n";
		print "#\n# Loop though each directory and publish logs\n#\n"; 
		print "\n";
		print LogFile "\n";
		print LogFile "#\n# Loop though each directory and publish logs\n#\n"; 
		print LogFile "\n";
			
		foreach my $name (sort { $a <=> $b } @jobs)
		{
			if ( -d $jobsfolder.$name)
			{
				print "\n*** Found Job: $name ***\n";
				print LogFile "\n*** Found Job: $name ***\n";
				
				# Check to see if job has been completed.
				if (isJobFinished($name))
				{
					# 
					# Publish the results
					#
					print "\n";
					print "#\n# Publish the results\n#\n"; 
					print "\n";
					print LogFile "\n";
					print LogFile "#\n# Publish the results\n#\n"; 
					print LogFile "\n";
					
					my $publishfile = $jobsfolder.$name."\\publish.txt";
					
					if (-r $publishfile)
					{
						open (PUBLISH, $publishfile) or GoNext("Cannot find publish details for $name");
						my $count = 0;
						my $index = 0;
						while (my $publishlocation = <PUBLISH>)
						{
							chomp $publishlocation;
							
							$index++;
							
							print "\nPublish Location = $publishlocation\n";
							print LogFile "\nPublish Location = $publishlocation\n";
							
							`mkdir $publishlocation`; # or GoNext("$publishlocation\\$name\\");
							
							my $lookIn = "$jobsfolder$name\\output";
							my ($source,$runid,$buildname) = getSourceDir($lookIn);
							
							my $srcdir = $source."\\xml";
							my $trgtdir= $publishlocation."xml\\";
							
							my $xcopycmd= "xcopy /C /Y /E $srcdir $trgtdir ";
							print `$xcopycmd`;
							
							#
							# Copy the contents of Output and reports -
							#
							# runXX_<snapshot>_SymbianOS_vYY.xml/html to <snapshot>_Symbian_OS_vYY.xml/html
							#
							
							my $xmlfile   = "$source\\run".$runid."_".$buildname."\.xml";
							my $htmlfile  = "$source\\run".$runid."_".$buildname."\.html";
							
							if (-e $xmlfile)  {
								
								print "copy $xmlfile to $buildname.xml";
								system "copy $xmlfile $publishlocation\\$buildname.xml";
							}
							
							if (-e $htmlfile)  {
								
								print "copy $htmlfile to $buildname.html";
								system "copy $htmlfile $publishlocation\\$buildname.html";
							}
							
							print "\nNumber of Files Copied = $1\n" if $xcopycmd =~ m/(\d+)\s\w+/;
							print LogFile "\nNumber of Files Copied = $1\n" if $xcopycmd =~ m/(\d+)\s\w+/;
							
							$count += $1;
							
							# The input should be copied onto \\devbuilds\master\.. but not \\devbuilds\logs\master\..
							if ($index < 2)
							{
								print "\n\nCopy the Input\n";
								$source = "$jobsfolder$name\\input\\*";
								
								# Copy the contents of Input
								print "\ncmd: xcopy /C /Y /E $source $publishlocation 2>&1\n";
								print LogFile "\ncmd: xcopy /C /Y /E $source $publishlocation 2>&1\n";
								my $xcopy = `xcopy /C /Y /E $source $publishlocation 2>&1`;
								print $xcopy;
								
								print "\nNumber of Files Copied = $1\n" if $xcopy =~ m/(\d+)\s\w+/;
								print LogFile "\nNumber of Files Copied = $1\n" if $xcopy =~ m/(\d+)\s\w+/;
								$count += $1;
							}
						}						
						close Publish;
						
						print "\nTotal Number of files copied = $count\n";
						print LogFile "\nTotal Number of files copied = $count\n";
						
						# Minimum 3 files have to be copied - Trace.txt, sys$rom.bin and armX.tpkg
						if ($count >= 3)
						{
							# it is fine to delete the jobs dir
							print "\ncmd: rd $jobsfolder$name /Q /S  2>&1\n";
							print LogFile "\ncmd: rd $jobsfolder$name /Q /S  2>&1\n";
							print `rd $jobsfolder$name /Q /S  2>&1`;
						}
					}
					else
					{
						print "\n*** Error publishing results ***\n Cannot open publish file: $jobsfolder"."$name"."\\publish.txt\n";
						print LogFile "\n*** Error publishing results ***\n Cannot open publish file: $jobsfolder"."$name"."\\publish.txt\n";
					}
				}
				else
				{
					print "\nThe Job $name is either still running or has encountered an error\n";				
					print LogFile "\nThe Job $name is either still running or has encountered an error\n";				
				}
			}
			else
			{
				print "*** Error ***\nDir does not exist! $jobsfolder.$name\n";
				print LogFile "*** Error ***\nDir does not exist! $jobsfolder.$name\n";
			}
		}
	}
	close JobDir;
	
	my $localtimes = localtime;
	print "\n*** Current Time: $localtimes ***\n";
	print LogFile "\n*** Current Time: $localtimes ***\n";
	
	print "\n";
	print "\n*********************\n";
	print "\n*** END PUBLISHER ***\n";
	print "\n*********************\n";
	print "\n";
	print "\n";
	print LogFile "\n";
	print LogFile "\n*********************\n";
	print LogFile "\n*** END PUBLISHER ***\n";
	print LogFile "\n*********************\n";
	print LogFile "\n";
	print LogFile "\n";
	
	close LogFile;
	sleep 900;
}

sub GoNext
{
	print "\n*** Error in Publisher ***\n $_[0] $!\n";
	print LogFile "\n*** Error in Publisher ***\n $_[0] $!\n";
	next;
}

# identify if a job has finished
sub isJobFinished($) {
	
	my $jobid = shift;
	my @cmd_out = `testdriver.cmd jobstatus -j $jobid 2>&1`;
	
	foreach (@cmd_out) {
		if (m/completed/) { return 1; }
	}
	
	return 0;
}

sub getSourceDir($)
{
	my $source = shift;
	my $runid  = 0;
	my $buildname;
	
	#
	# Find the correct directory to publish
	#
	
	if (opendir RESULTS, $source)
	{	
		my @dirArray = readdir(RESULTS);
		
		#print "@results";	
		# to remove "." and ".." from this list
		shift @dirArray;
		shift @dirArray;
		
		if (!scalar(@dirArray))
		{
			print "REMARK: The result for $_[0] has an empty $source directory";
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
				my @dirArray = readdir(tempDir);
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
				
										foreach my $directory (@dirArray)
										{
											if ($directory =~ /(^[0-9]+)/i)
											{
												$source .= "\\$directory";
												$runid = $directory;
												last;
											}
										}
									}
									last;
								}
							}
						}
						
					}
				}
			}
		}
	}
	return ($source, $runid, $buildname);
}
