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
# Script to build ATS test drop for Landmarks Selector BC tests


use strict;
use File::Copy;
use File::Path;

unlink "locseltest.zip";
rmtree "temp";
mkpath "temp/locseltest/general/testframework";
mkpath "temp/locseltest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("locsel.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/lmkselector.dll",					"temp/locseltest/winscw_udeb/lmkselector.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_locsel.ini",		"temp/locseltest/general/testframework/testframework_locsel.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_lmkselector.cfg",			"temp/locseltest/general/testframework/ui_lmkselector.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tclmkselector.cfg",			"temp/locseltest/general/testframework/tclmkselector.cfg") or die "failed : $!";

system("7z a -tzip locseltest.zip ./temp/*");
