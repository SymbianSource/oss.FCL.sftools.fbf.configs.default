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
# Script to build ATS test drop DRM tests


use strict;
use File::Copy;
use File::Path;

unlink "drmtest.zip";
rmtree "temp";
mkpath "temp/drmtest/general/testframework";
mkpath "temp/drmtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("drm.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/drm_caf.dll",					"temp/drmtest/winscw_udeb/drm_caf.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/bcdrmcafcaps.exe",			"temp/drmtest/winscw_udeb/bcdrmcafcaps.exe") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_drmcaf.ini",	"temp/drmtest/general/testframework/testframework_drmcaf.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/drm_caf.cfg",				"temp/drmtest/general/testframework/drm_caf.cfg") or die "failed : $!";
copy($epoc . "winscw/c/content-noenc-trunc.odf",				"temp/drmtest/general/content-noenc-trunc.odf") or die "failed : $!";
copy($epoc . "winscw/c/content.odf",							"temp/drmtest/general/content.odf") or die "failed : $!";
copy($epoc . "winscw/c/content.txt",							"temp/drmtest/general/content.txt") or die "failed : $!";
copy($epoc . "winscw/c/content-noenc.odf",						"temp/drmtest/general/content-noenc.odf") or die "failed : $!";
copy($epoc . "winscw/c/content.dcf",							"temp/drmtest/general/content.dcf") or die "failed : $!";

system("7z a -tzip drmtest.zip ./temp/*");
