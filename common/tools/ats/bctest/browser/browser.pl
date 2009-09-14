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

unlink "browser.zip";
##rmtree "temp";
mkpath "temp/bcbrowser/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("browser.xml", "temp/test.xml");

copy($epoc . "release/winscw/udeb/favouritesbctest.dll",     "temp/bcbrowser/winscw_udeb/favouritesbctest.dll");

system("7z a -tzip browser.zip ./temp/*");