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
# Script to build ATS test drop for Notepad BC tests


use strict;
use File::Copy;
use File::Path;

unlink "notepadtest.zip";
rmtree "temp";
mkpath "temp/notepadtest/general/testingdata";
mkpath "temp/notepadtest/general/TestFramework";
mkpath "temp/notepadtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("notepad.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/TestNpdApi.dll",				"temp/notepadtest/winscw_udeb/TestNpdApi.dll") or die "failed : $!";
copy($epoc . "winscw/c/TestNpdApi.txt",							"temp/notepadtest/general/testingdata/TestNpdApi.txt") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/testframework_npd.ini",	"temp/notepadtest/general/TestFramework/testframework_npd.ini") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/TestNpdApi.cfg",			"temp/notepadtest/general/TestFramework/TestNpdApi.cfg") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/ui_TestNpdApi.cfg",		"temp/notepadtest/general/TestFramework/ui_TestNpdApi.cfg") or die "failed : $!";

system("7z a -tzip notepadtest.zip ./temp/*");