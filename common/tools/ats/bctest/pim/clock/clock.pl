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
# Louis Henry Nayegon <louisn@symbian.org>
#
# Description:
# Script to build ATS test drop contacts phonebook tests


use strict;
use File::Copy;
use File::Path;

unlink "clocktest.zip";
##rmtree "temp";
mkpath "temp/clocktest/general/testframework";
mkpath "temp/clocktest/general/bcardtest2";
mkpath "temp/clocktest/winscw_udeb/z/resource/plugins";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("clock.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/bctestclock.dll",					"temp/clocktest/winscw_udeb/bctestclock.dll");
copy($epoc . "winscw/c/testframework/testframework_pimclock.ini",	"temp/clocktest/general/testframework/testframework_pimclock.ini");
copy($epoc . "winscw/c/testframework/tcbctestclock.cfg",			"temp/clocktest/general/testframework/tcbctestclock.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestclock.cfg",			"temp/clocktest/general/testframework/ui_bctestclock.cfg");

system("7z a -tzip clocktest.zip ./temp/*");
