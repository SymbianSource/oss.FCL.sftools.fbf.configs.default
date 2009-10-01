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
# Louis Henry Nayegon <louisn@symbian.org>
#
# Description:
# Script to build ATS test drop syslibs svs tests


use strict;
use File::Copy;
use File::Path;

unlink "syslibstest.zip";
rmtree "temp";
mkpath "temp/syslibstest/general/syslibs";
mkpath "temp/syslibstest/general/ecom";
mkpath "temp/syslibstest/winscw_udeb/z/resource/plugins";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("syslibstest.xml",	"temp/test.xml");

copy($epoc . "data/z/syslibs/t_syslibs.tcs",		"temp/syslibstest/general/syslibs/t_syslibs.tcs") or die "failed : $!";
copy($epoc . "data/z/syslibs/winscw/t_syslibs.ini",	"temp/syslibstest/general/syslibs/t_syslibs.ini") or die "failed : $!";

copy($epoc . "data/z/ecom/syslib-ecom-publicapi.script",	"temp/syslibstest/general/ecom/syslib-ecom-publicapi.script") or die "failed : $!";
copy($epoc . "data/z/ecom/syslib-ecom-publicapi.ini",		"temp/syslibstest/general/ecom/syslib-ecom-publicapi.ini") or die "failed : $!";
copy($epoc . "data/z/ecom/testproduct_plugin.rsc",			"temp/syslibstest/general/ecom/testproduct_plugin.rsc") or die "failed : $!";
copy($epoc . "release/winscw/udeb/testproduct_plugin.dll",	"temp/syslibstest/general/ecom/testproduct_plugin.dll") or die "failed : $!";

copy($epoc . "release/winscw/udeb/t_ecom.exe",				"temp/syslibstest/winscw_udeb/t_ecom.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tpcopyfileutility.exe",	"temp/syslibstest/winscw_udeb/tpcopyfileutility.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tpdeletefileutility.exe",	"temp/syslibstest/winscw_udeb/tpdeletefileutility.exe") or die "failed : $!";

system("7z a -tzip syslibstest.zip ./temp/*");
