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
# Script to build ATS test drop BLID Application Satellite Info BC tests


use strict;
use File::Copy;
use File::Path;

unlink "locblidtest.zip";
rmtree "temp";
mkpath "temp/locblidtest/general/testframework";
mkpath "temp/locblidtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("locblid.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/BCBlidTest.dll",					"temp/locblidtest/winscw_udeb/BCBlidTest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_locblid.ini",	"temp/locblidtest/general/testframework/testframework_locblid.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcLmkBlid.cfg",				"temp/locblidtest/general/testframework/tcLmkBlid.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/UI_BCBLid.cfg",				"temp/locblidtest/general/testframework/UI_BCBLid.cfg") or die "failed : $!";

system("7z a -tzip locblidtest.zip ./temp/*");
