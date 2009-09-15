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

copy($epoc . "release/winscw/udeb/featuremanagertest.dll",		"temp/systemswtest/winscw_udeb/featuremanagertest.dll");
copy($epoc . "release/winscw/udeb/finditemenginetest.dll",		"temp/systemswtest/winscw_udeb/finditemenginetest.dll");
copy($epoc . "release/winscw/udeb/light.dll",					"temp/systemswtest/winscw_udeb/light.dll");
copy($epoc . "release/winscw/udeb/networkstatustest.dll",		"temp/systemswtest/winscw_udeb/networkstatustest.dll");
copy($epoc . "release/winscw/udeb/phoneparsertest.dll",			"temp/systemswtest/winscw_udeb/phoneparsertest.dll");
copy($epoc . "release/winscw/udeb/platformenvtest.dll",			"temp/systemswtest/winscw_udeb/platformenvtest.dll");
copy($epoc . "release/winscw/udeb/powerstatetest.dll",			"temp/systemswtest/winscw_udeb/powerstatetest.dll");
copy($epoc . "release/winscw/udeb/sysutiltest.dll",				"temp/systemswtest/winscw_udeb/sysutiltest.dll");
copy($epoc . "release/winscw/udeb/stiftextresolvertest.dll",	"temp/systemswtest/winscw_udeb/stiftextresolvertest.dll");
copy($epoc . "release/winscw/udeb/hwresmantest.dll",			"temp/systemswtest/winscw_udeb/hwresmantest.dll");
copy($epoc . "release/winscw/udeb/vibractrltest.dll",			"temp/systemswtest/winscw_udeb/vibractrltest.dll");
copy($epoc . "release/winscw/udeb/platformvertest.dll",			"temp/systemswtest/winscw_udeb/platformvertest.dll");
copy($epoc . "release/winscw/udeb/accmonapibctests.dll",		"temp/systemswtest/winscw_udeb/accmonapibctests.dll");

system("7z a -tzip systemswtest.zip ./temp/*");
