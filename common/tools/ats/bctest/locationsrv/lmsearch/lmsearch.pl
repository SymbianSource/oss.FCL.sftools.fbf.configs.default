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
# Script to build ATS test drop Landmarks Search BC tests


use strict;
use File::Copy;
use File::Path;

unlink "lmsearchtest.zip";
##rmtree "temp";
mkpath "temp/lmsearchtest/general/testframework";
mkpath "temp/lmsearchtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("lmsearch.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/testlmksearchapi.dll",			"temp/lmsearchtest/winscw_udeb/testlmksearchapi.dll");
copy($epoc . "winscw/c/testframework/TestFramework_lmsearch.ini",	"temp/lmsearchtest/general/testframework/TestFramework_lmsearch.ini");
copy($epoc . "winscw/c/testframework/testlmksearchapi.cfg",			"temp/lmsearchtest/general/testframework/testlmksearchapi.cfg");

system("7z a -tzip lmsearchtest.zip ./temp/*");
