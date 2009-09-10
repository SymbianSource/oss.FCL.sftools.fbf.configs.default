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
# Script to build ATS test drop for BC Profiles Engine tests


use strict;
use File::Copy;
use File::Path;

unlink "profilesengine.zip";
##rmtree "temp";
mkpath "temp/bcprofilesengine/general/testframework";
mkpath "temp/bcprofilesengine/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("profilesengine.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/proengwrapapi.dll",                   "temp/bcprofilesengine/winscw_udeb/proengwrapapi.dll");
copy($epoc . "winscw/c/testframework/testframework_proengwrapapi.ini",  "temp/bcprofilesengine/general/testframework/testframework_proengwrapapi.ini");
copy($epoc . "winscw/c/testframework/proengwrapapi.cfg",                "temp/bcprofilesengine/general/testframework/proengwrapapi.cfg");

system("7z a -tzip profilesengine.zip ./temp/*");