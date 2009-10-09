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
# Brendan Donegan <brendand@symbian.org>
#
# Description:
# Script to build ATS test drop for BC Web Services tests


use strict;
use File::Copy;
use File::Path;

unlink "messaging.zip";
rmtree "temp";

mkpath "temp/bcmessaging/testframework/general";
mkpath "temp/bcmessaging/pluginbiocontrolapitest/general";
mkpath "temp/bcmessaging/resource/general";
mkpath "temp/bcmessaging/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("messaging.xml",       "temp/test.xml");

copy($epoc . "release/winscw/udeb/MMSClientMTMBCTester.dll",                "temp/bcmessaging/winscw_udeb/MMSClientMTMBCTester.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/MMSClientMTMBCTester.ini",             "temp/bcmessaging/testframework/general/MMSClientMTMBCTester.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/MMSClientMTMBCTester_TestImage.jpg",   "temp/bcmessaging/testframework/general/MMSClientMTMBCTester_TestImage.jpg") or die "failed : $!";
copy($epoc . "release/winscw/udeb/PluginBioControlAPITest.dll",             "temp/bcmessaging/winscw_udeb/PluginBioControlAPITest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcTestUIBioCombiner.cfg",              "temp/bcmessaging/testframework/general/tcTestUIBioCombiner.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_PluginBioControlAPITest.cfg",       "temp/bcmessaging/testframework/general/ui_PluginBioControlAPITest.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/test.txt",                             "temp/bcmessaging/pluginbiocontrolapitest/general/test.txt") or die "failed : $!";
copy($epoc . "winscw/c/testframework/PluginBioControlAPITest.rsc",          "temp/bcmessaging/testframework/general/PluginBioControlAPITest.rsc") or die "failed : $!";
copy($epoc . "data/z/resource/pluginbiocontrolapitest.rsc",                 "temp/bcmessaging/resource/general/pluginbiocontrolapitest.rsc") or die "failed : $!";

system("7z a -tzip messaging.zip ./temp/*");