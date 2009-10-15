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
# Script to build ATS test drop for Thread Safe Template Library BC tests


use strict;
use File::Copy;
use File::Path;

unlink "tstltest.zip";
rmtree "temp";
mkpath "temp/tstltest/general/testframework";
mkpath "temp/tstltest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("tstl.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/tstl_1.dll",						"temp/tstltest/winscw_udeb/tstl_1.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_tstl_1.ini",	"temp/tstltest/general/testframework/testframework_tstl_1.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tstl_1.cfg",					"temp/tstltest/general/testframework/tstl_1.cfg") or die "failed : $!";

system("7z a -tzip tstltest.zip ./temp/*");
