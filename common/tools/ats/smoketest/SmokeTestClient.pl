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
# This script was born on 07/10/2004
# 
#

#!/usr/bin/perl -w
use strict;
use SmokeTest;
use Getopt::Long;

# Process the commandline
my ($iSourceDir, $iTestSuite, $iProduct, $iServerPool, $iProductType, $iPublishLocation, $iBuildNumber, $iXML, $iUnzip, $iBuildDrive, $iTDDrive) = ProcessCommandLine();

&SmokeTest::main($iSourceDir, $iTestSuite, $iProduct, $iServerPool, $iProductType, $iPublishLocation, $iBuildNumber, $iXML, $iUnzip, $iBuildDrive, $iTDDrive);

# ProcessCommandLine
#
# Description
# This function processes the commandline

sub ProcessCommandLine
{
  my ($iHelp);
  
  GetOptions('h' => \$iHelp, 'src=s' => \$iSourceDir, 'pool=s' => \$iServerPool, 'product=s' => \$iProduct, 'suite=s' => \$iTestSuite, 'type=s' => \$iProductType, 'publish=s' => \$iPublishLocation, 'build=s' => \$iBuildNumber, , 'xml=s' => \$iXML, 'unziptd' => \$iUnzip, 'builddrive=s' => \$iBuildDrive, 'tddrive=s' => \$iTDDrive);

  if (($iHelp) || (!defined $iSourceDir) || (!defined $iTestSuite) || (!defined $iProduct) || (!defined $iBuildDrive))
  {
    Usage();
  }
  
  return ($iSourceDir, $iTestSuite, $iProduct, $iServerPool, $iProductType, $iPublishLocation, $iBuildNumber, $iXML, $iUnzip, $iBuildDrive, $iTDDrive);
}

# Usage
#
# Output Usage Information.
#

sub Usage 
{
  print <<USAGE_EOF;

  Usage: SmokeTestClient.pl [switches]

  [Switches]
  --suite Test Driver test suite (e.g. SmokeTest)
  --src source code directory (e.g .\\src\\common\\testtools)
  		[NOTE: The test suite directory should be located in this directory]
  --product product being tested (e.g. 9.1)
  --builddrive The drive where the source and epoc32 is (e.g. M:)
  
  [Optional]
  -h help
  --pool hash separated smoketest server pool (e.g. lon-sysbuild05\#lon-sysbuild06)
  --type type of build, either Master or Symbian_OS_v8.1
  --publish Publish server for the smoke test results (e.g. \\\\builds01)
  --build Build Number (e.g. 03555_Symbian_OS_v9.1)
  --xml TestDriver XML tree. Default is \\src\\common\\testtools\\SmokeTest\\xml.
  --unziptd Installs a stable version of TestDriver from \\\\builds01\\devbuilds\\BuildTeam\\TestDriver
  --tddrive The drive where testdriver is installed (default is C: and TD directory is c:\\testdriver)
  
  All the optional arguments are either specified by the user or assumed to be set as environment variables.
  These environment variables can be found in SmokeTest.pm

USAGE_EOF
	exit 1;
}
