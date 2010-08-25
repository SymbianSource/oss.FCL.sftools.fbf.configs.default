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
# Script to build ATS test drop for Metadata Services BC tests


use strict;
use File::Copy;
use File::Path;

unlink "metadatasrvtest.zip";
rmtree "temp";
mkpath "temp/metadatasrvtest/general/TestFramework";
mkpath "temp/metadatasrvtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("metadatasrv.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/BCTestContentManagement.dll",			"temp/metadatasrvtest/winscw_udeb/BCTestContentManagement.dll") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/TestFramework_MMConMgmt.ini",		"temp/metadatasrvtest/general/TestFramework/TestFramework_MMConMgmt.ini") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/TestFramework_MMConMgmt_std.ini",	"temp/metadatasrvtest/general/TestFramework/TestFramework_MMConMgmt_std.ini") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/tcBCTestContentManagement.cfg",	"temp/metadatasrvtest/general/TestFramework/tcBCTestContentManagement.cfg") or die "failed : $!";
copy($epoc . "winscw/c/TestFramework/UI_BCTestContentManagement.cfg",	"temp/metadatasrvtest/general/TestFramework/UI_BCTestContentManagement.cfg") or die "failed : $!";

system("7z a -tzip metadatasrvtest.zip ./temp/*");
