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
# Script to build ATS test drop system software tests


use strict;
use File::Copy;
use File::Path;

unlink "systemswtest.zip";
rmtree "temp";
mkpath "temp/systemswtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("systemsw.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/featuremanagertest.dll",		"temp/systemswtest/winscw_udeb/featuremanagertest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/finditemenginetest.dll",		"temp/systemswtest/winscw_udeb/finditemenginetest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/light.dll",					"temp/systemswtest/winscw_udeb/light.dll") or die "failed : $!";
-copy($epoc . "release/winscw/udeb/networkstatustest.dll",		"temp/systemswtest/winscw_udeb/networkstatustest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/phoneparsertest.dll",			"temp/systemswtest/winscw_udeb/phoneparsertest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/platformenvtest.dll",			"temp/systemswtest/winscw_udeb/platformenvtest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/powerstatetest.dll",			"temp/systemswtest/winscw_udeb/powerstatetest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/sysutiltest.dll",				"temp/systemswtest/winscw_udeb/sysutiltest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/stiftextresolvertest.dll",	"temp/systemswtest/winscw_udeb/stiftextresolvertest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/hwresmantest.dll",			"temp/systemswtest/winscw_udeb/hwresmantest.dll") or die "failed : $!";
##copy($epoc . "release/winscw/udeb/vibractrltest.dll",			"temp/systemswtest/winscw_udeb/vibractrltest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/platformvertest.dll",			"temp/systemswtest/winscw_udeb/platformvertest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/accmonapibctests.dll",		"temp/systemswtest/winscw_udeb/accmonapibctests.dll") or die "failed : $!";
#Added on 26 Aug 2010
copy($epoc . "release/winscw/udeb/cenrepnotifyhandlertest.dll",	"temp/systemswtest/winscw_udeb/cenrepnotifyhandlertest.dll") or die "failed : $!";

system("7z a -tzip systemswtest.zip ./temp/*");
