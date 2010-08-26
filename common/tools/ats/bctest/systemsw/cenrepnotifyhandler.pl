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
# Script to build ATS test drop for (SystemSW) Central Repository Notification Handler tests


use strict;
use File::Copy;
use File::Path;

unlink "cenrepnotifyhandlertest.zip";
rmtree "temp";
mkpath "temp/cenrepnotifyhandlertest/winscw_udeb";
mkpath "temp/cenrepnotifyhandlertest/general/TestFramework";
mkpath "temp/cenrepnotifyhandlertest/general/10202be9";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("cenrepnotifyhandler.xml",	"temp/test.xml");


copy($epoc . "release/winscw/udeb/CenRepNotifyHandlerTest.dll",	"temp/cenrepnotifyhandlertest/winscw_udeb/CenRepNotifyHandlerTest.dll") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/TestFramework_cenrep.ini",	"temp/cenrepnotifyhandlertest/general/TestFramework/TestFramework_cenrep.ini") or die "failed : $!";
copy($epoc . "winscw/c/6edcba99.txt",							"temp/cenrepnotifyhandlertest/general/10202be9/6edcba99.txt") or die "failed : $!";

system("7z a -tzip cenrepnotifyhandlertest.zip ./temp/*");
