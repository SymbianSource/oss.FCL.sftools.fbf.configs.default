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
# Script to build ATS test drop for Landmarks Application Editor BC tests


use strict;
use File::Copy;
use File::Path;

unlink "locaddedittest.zip";
rmtree "temp";
mkpath "temp/locaddedittest/general/testframework";
mkpath "temp/locaddedittest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("locaddedit.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/bcapplmkaddedit.dll",					"temp/locaddedittest/winscw_udeb/bcapplmkaddedit.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/TestFramework_locaddedit.ini",		"temp/locaddedittest/general/testframework/TestFramework_locaddedit.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_lmkaddedit.cfg",				"temp/locaddedittest/general/testframework/ui_lmkaddedit.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcLmkAddEdit.cfg",					"temp/locaddedittest/general/testframework/tcLmkAddEdit.cfg") or die "failed : $!";

system("7z a -tzip locaddedittest.zip ./temp/*");
