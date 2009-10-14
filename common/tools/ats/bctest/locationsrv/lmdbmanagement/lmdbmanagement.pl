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
# Script to build ATS test drop for Landmarks Database Management BC tests


use strict;
use File::Copy;
use File::Path;

unlink "lmdbmanagementtest.zip";
rmtree "temp";
mkpath "temp/lmdbmanagementtest/general/testframework";
mkpath "temp/lmdbmanagementtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("lmdbmanagement.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/testdbmanagementapi.dll",			"temp/lmdbmanagementtest/winscw_udeb/testdbmanagementapi.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_lmdbmanagement.ini",	"temp/lmdbmanagementtest/general/testframework/testframework_lmdbmanagement.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testdbmanagementapi.cfg",				"temp/lmdbmanagementtest/general/testframework/testdbmanagementapi.cfg") or die "failed : $!";

system("7z a -tzip lmdbmanagementtest.zip ./temp/*");
