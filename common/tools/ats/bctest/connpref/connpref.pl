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
# Script to build ATS test drop for Extended Connection Pref BC tests


use strict;
use File::Copy;
use File::Path;

unlink "connpreftest.zip";
rmtree "temp";
mkpath "temp/connpreftest/general/testframework";
mkpath "temp/connpreftest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("connpref.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/econnprefbc.dll",					"temp/connpreftest/winscw_udeb/econnprefbc.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_connpref.ini",	"temp/connpreftest/general/testframework/testframework_connpref.ini") or die "failed : $!";

system("7z a -tzip connpreftest.zip ./temp/*");
