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
# Script to build ATS test drop for Connection Settings BC tests


use strict;
use File::Copy;
use File::Path;

unlink "connsettest.zip";
rmtree "temp";
mkpath "temp/connsettest/general/testframework";
mkpath "temp/connsettest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("connset.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/BcCMManagerEngineTester.dll",		"temp/connsettest/winscw_udeb/BcCMManagerEngineTester.dll") or die "failed : $!";
#copy($epoc . "release/winscw/udeb/connset.exe",						"temp/connsettest/winscw_udeb/connset.exe") or die "failed : $!";
copy($epoc . "winscw/c/testframework/TestFramework_connset.ini",	"temp/connsettest/general/testframework/TestFramework_connset.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/BcCMManagerEngineTester.cfg",	"temp/connsettest/general/testframework/BcCMManagerEngineTester.cfg") or die "failed : $!";

system("7z a -tzip connsettest.zip ./temp/*");
