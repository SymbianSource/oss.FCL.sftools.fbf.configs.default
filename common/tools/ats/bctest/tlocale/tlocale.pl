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
# Script to build ATS test drop for TLocale BC tests


use strict;
use File::Copy;
use File::Path;

unlink "tlocaletest.zip";
rmtree "temp";
mkpath "temp/tlocaletest/general/testframework";
mkpath "temp/tlocaletest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("tlocale.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/tlocale.dll",						"temp/tlocaletest/winscw_udeb/tlocale.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_tlocale.ini",	"temp/tlocaletest/general/testframework/testframework_tlocale.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tlocale.cfg",					"temp/tlocaletest/general/testframework/tlocale.cfg") or die "failed : $!";

system("7z a -tzip tlocaletest.zip ./temp/*");
