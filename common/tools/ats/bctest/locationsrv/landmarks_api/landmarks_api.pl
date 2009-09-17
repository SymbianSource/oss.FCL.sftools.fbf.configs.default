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
# Description:
# Script to build ATS test drop for landmarks_api tests


use strict;
use File::Copy;
use File::Path;

unlink "landmarks_api.zip";
##rmtree "temp";
mkpath "temp/landmarks/general/testframework";
mkpath "temp/landmarks/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("landmarks.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/testlandmarksapi.dll",				"temp/landmarks/winscw_udeb/testlandmarksapi.dll");
copy($epoc . "winscw/c/testframework/TestFramework_Landmarks.ini",		"temp/landmarks/general/testframework/TestFramework_Landmarks.ini");
copy($epoc . "winscw/c/testframework/testlandmarksapi.cfg",				"temp/landmarks/general/testframework/testlandmarksapi.cfg");
copy($epoc . "winscw/c/testing/data/smiltemplate.mif",					"temp/landmarks/general/smiltemplate.mif");
copy($epoc . "winscw/c/testing/data/testEposLmImport.lmx",				"temp/landmarks/general/testEposLmImport.lmx");
copy($epoc . "winscw/c/testing/data/testEposLmImportCollection.lmx",	"temp/landmarks/general/testEposLmImportCollection.lmx");

system("7z a -tzip landmarks_api.zip ./temp/*");
