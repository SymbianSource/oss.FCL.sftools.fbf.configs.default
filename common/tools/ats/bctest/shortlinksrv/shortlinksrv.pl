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
# Script to build ATS test drop for Shortlink Services BC tests


use strict;
use File::Copy;
use File::Path;

unlink "shortlinksrvtest.zip";
rmtree "temp";
mkpath "temp/shortlinksrvtest/bluetooth/general";
mkpath "temp/shortlinksrvtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("shortlinksrv.xml",	"temp/test.xml");
copy($epoc . "release/winscw/udeb/T_BTSockAddrAPI.exe",				"temp/shortlinksrvtest/winscw_udeb/T_BTSockAddrAPI.exe") or die "failed : $!";
copy($epoc . "winscw/c/bluetooth/BT-SOCK-ADDR-PublicApi.script",	"temp/shortlinksrvtest/bluetooth/general/BT-SOCK-ADDR-PublicApi.script") or die "failed : $!";
copy($epoc . "winscw/c/bluetooth/BT-SOCK-ADDR-PublicApi.ini",		"temp/shortlinksrvtest/bluetooth/general/BT-SOCK-ADDR-PublicApi.ini") or die "failed : $!";

system("7z a -tzip shortlinksrvtest.zip ./temp/*");