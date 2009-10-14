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
# Script to build ATS test drop for Landmarks Item Id Db Combi Info BC tests


use strict;
use File::Copy;
use File::Path;

unlink "locdbcitest.zip";
rmtree "temp";
mkpath "temp/locdbcitest/general/testframework";
mkpath "temp/locdbcitest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("locdbci.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/landmarksitemiddbcombiinfo.dll",					"temp/locdbcitest/winscw_udeb/landmarksitemiddbcombiinfo.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_locdbci.ini",		"temp/locdbcitest/general/testframework/testframework_locdbci.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_lmkitemiddbcombiinfo.cfg",				"temp/locdbcitest/general/testframework/ui_lmkitemiddbcombiinfo.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tclmkitemiddbcombiinfo.cfg",					"temp/locdbcitest/general/testframework/tclmkitemiddbcombiinfo.cfg") or die "failed : $!";

system("7z a -tzip locdbcitest.zip ./temp/*");
