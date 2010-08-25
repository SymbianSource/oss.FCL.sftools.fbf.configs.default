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
# Script to build ATS test drop for Location Triggering BC tests


use strict;
use File::Copy;
use File::Path;

unlink "loctriggeringtest.zip";
rmtree "temp";
mkpath "temp/loctriggeringtest/general/testframework";
mkpath "temp/loctriggeringtest/general/systemdata";
mkpath "temp/loctriggeringtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("loctriggering.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/lbtbctest.dll",			"temp/loctriggeringtest/winscw_udeb/lbtbctest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/Testframework_LT.ini",	"temp/loctriggeringtest/general/testframework/Testframework_LT.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/lbtbctest.cfg",		"temp/loctriggeringtest/general/testframework/lbtbctest.cfg") or die "failed : $!";
copy($epoc . "winscw/c/system/data/simu_move2.sps",			"temp/loctriggeringtest/general/systemdata/simu_move2.sps") or die "failed : $!";

system("7z a -tzip loctriggeringtest.zip ./temp/*");
