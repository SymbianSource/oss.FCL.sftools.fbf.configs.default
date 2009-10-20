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
# Brendan Donegan <brendand@symbian.org>
#
# Description:
# Script to build ATS test drop for BC RTC Core Telephony tests


use strict;
use File::Copy;
use File::Path;

unlink "coretelephony.zip";
rmtree "temp";
mkpath "temp/rtcct/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("coretelephony.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/BCLogEngineTest.dll",                     "temp/rtcct/winscw_udeb/BCLogEngineTest.dll") or die "failed : $!";

system("7z a -tzip coretelephony.zip ./temp/*");