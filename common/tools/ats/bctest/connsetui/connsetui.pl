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
# Script to build ATS test drop for Connection Settings UI BC tests


use strict;
use File::Copy;
use File::Path;

unlink "connsetuitest.zip";
rmtree "temp";
mkpath "temp/connsetuitest/general/testframework";
mkpath "temp/connsetuitest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("connsetui.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/BCConnSettingsUITest.dll",		"temp/connsetuitest/winscw_udeb/BCConnSettingsUITest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/TestFramework_connsetui.ini",	"temp/connsetuitest/general/testframework/TestFramework_connsetui.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/UI_ConnSet.cfg",				"temp/connsetuitest/general/testframework/UI_ConnSet.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcConnSet.cfg",				"temp/connsetuitest/general/testframework/tcConnSet.cfg") or die "failed : $!";

system("7z a -tzip connsetuitest.zip ./temp/*");
