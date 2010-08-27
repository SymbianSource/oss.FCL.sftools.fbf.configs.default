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
# Maciej Seroka <maciejs@symbian.org>
#
# Description:
# Script to build ATS test drop for SMS Utilities BC tests


use strict;
use File::Copy;
use File::Path;

unlink "smsutilitiestest.zip";
rmtree "temp";
mkpath "temp/smsutilitiestest/general/TestFramework";
mkpath "temp/smsutilitiestest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("smsutilities.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/BCSmsUtilitiesTest.dll",		"temp/smsutilitiestest/winscw_udeb/BCSmsUtilitiesTest.dll") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/testframework_su.ini",		"temp/smsutilitiestest/general/TestFramework/testframework_su.ini") or die "failed : $!";

system("7z a -tzip smsutilitiestest.zip ./temp/*");