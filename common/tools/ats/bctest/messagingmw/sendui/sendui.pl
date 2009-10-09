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
# Script to build ATS test drop for BC Send UI tests


use strict;
use File::Copy;
use File::Path;

unlink "sendui.zip";
rmtree "temp";
mkpath "temp/bcsendui/general/testframework";
mkpath "temp/bcsendui/general/senduiapitest";
mkpath "temp/bcsendui/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("sendui.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/senduiapitest.dll",                   "temp/bcsendui/winscw_udeb/senduiapitest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_sendui.ini",         "temp/bcsendui/general/testframework/testframework_sendui.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_senduiapitest.cfg",             "temp/bcsendui/general/testframework/ui_senduiapitest.cfg") or die "failed : $!";
copy($epoc . "winscw/c/data/testdata/senduiapitest/attachment.txt",		"temp/bcsendui/general/senduiapitest/attachment.txt") or die "failed : $!";

system("7z a -tzip sendui.zip ./temp/*");