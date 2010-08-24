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
# Script to build ATS test drop for Calendar BC tests


use strict;
use File::Copy;
use File::Path;

unlink "calendartest.zip";
rmtree "temp";
mkpath "temp/calendartest/general/TestFramework";
mkpath "temp/calendartest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("calendar.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/BCTestCalendar2.dll",				"temp/calendartest/winscw_udeb/BCTestCalendar2.dll") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/TestFramework_PIMCalendar.ini","temp/calendartest/general/TestFramework/TestFramework_PIMCalendar.ini") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/tcBCTestCalendar2.cfg",		"temp/calendartest/general/TestFramework/tcBCTestCalendar2.cfg") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/UI_BCTestCalendar2.cfg",		"temp/calendartest/general/TestFramework/UI_BCTestCalendar2.cfg") or die "failed : $!";

system("7z a -tzip calendartest.zip ./temp/*");