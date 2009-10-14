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
# Script to build ATS test drop for Location Acquisition BC tests


use strict;
use File::Copy;
use File::Path;

unlink "locaqutest.zip";
rmtree "temp";
mkpath "temp/locaqutest/general/testframework";
mkpath "temp/locaqutest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("locaqu.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/testlbslocacquisition.dll",		"temp/locaqutest/winscw_udeb/testlbslocacquisition.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_LocAqu.ini",		"temp/locaqutest/general/testframework/testframework_LocAqu.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testlbslocacquisition.cfg",	"temp/locaqutest/general/testframework/testlbslocacquisition.cfg") or die "failed : $!";

system("7z a -tzip locaqutest.zip ./temp/*");
