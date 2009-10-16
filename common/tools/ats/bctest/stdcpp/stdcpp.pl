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
# Script to build ATS test drop for STDCPP BC tests


use strict;
use File::Copy;
use File::Path;

unlink "stdcpptest.zip";
rmtree "temp";
mkpath "temp/stdcpptest/general/testframework";
mkpath "temp/stdcpptest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("stdcpp.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/bccppwrap.dll",					"temp/stdcpptest/winscw_udeb/bccppwrap.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/BCCSRuntimeSup.dll",				"temp/stdcpptest/winscw_udeb/BCCSRuntimeSup.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/BCException.dll",					"temp/stdcpptest/winscw_udeb/BCException.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/BCNoNamespace.dll",				"temp/stdcpptest/winscw_udeb/BCNoNamespace.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/BCOpenCHeaders.dll",				"temp/stdcpptest/winscw_udeb/BCOpenCHeaders.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/BCOpenCWrap.dll",					"temp/stdcpptest/winscw_udeb/BCOpenCWrap.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/BCThread.dll",					"temp/stdcpptest/winscw_udeb/BCThread.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/BCTypeAttrib.dll",				"temp/stdcpptest/winscw_udeb/BCTypeAttrib.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/BCTypeInfo.dll",					"temp/stdcpptest/winscw_udeb/BCTypeInfo.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/chartypes.dll",					"temp/stdcpptest/winscw_udeb/chartypes.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tmath_operations.dll",			"temp/stdcpptest/winscw_udeb/tmath_operations.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tmemory.dll",						"temp/stdcpptest/winscw_udeb/tmemory.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tutility.dll",					"temp/stdcpptest/winscw_udeb/tutility.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/widecharclassapiBCTest.dll",		"temp/stdcpptest/winscw_udeb/widecharclassapiBCTest.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tlocale.dll",						"temp/stdcpptest/winscw_udeb/tlocale.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tstl_1.dll",						"temp/stdcpptest/winscw_udeb/tstl_1.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_cppwrap.ini",	"temp/stdcpptest/general/testframework/testframework_cppwrap.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrtruntime.ini",	"temp/stdcpptest/general/testframework/testframework_mrtruntime.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrtexp.ini",		"temp/stdcpptest/general/testframework/testframework_mrtexp.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrtnons.ini",	"temp/stdcpptest/general/testframework/testframework_mrtnons.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrtopench.ini",	"temp/stdcpptest/general/testframework/testframework_mrtopench.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_ocw.ini",		"temp/stdcpptest/general/testframework/testframework_ocw.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrtthread.ini",	"temp/stdcpptest/general/testframework/testframework_mrtthread.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrttypea.ini",	"temp/stdcpptest/general/testframework/testframework_mrttypea.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrttypei.ini",	"temp/stdcpptest/general/testframework/testframework_mrttypei.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_ct.ini",			"temp/stdcpptest/general/testframework/testframework_ct.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrtmath.ini",	"temp/stdcpptest/general/testframework/testframework_mrtmath.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrtmem.ini",		"temp/stdcpptest/general/testframework/testframework_mrtmem.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mrtutil.ini",	"temp/stdcpptest/general/testframework/testframework_mrtutil.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_wc.ini",			"temp/stdcpptest/general/testframework/testframework_wc.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_tlocale.ini",	"temp/stdcpptest/general/testframework/testframework_tlocale.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_tstl_1.ini",		"temp/stdcpptest/general/testframework/testframework_tstl_1.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/bccppwrap.cfg",				"temp/stdcpptest/general/testframework/bccppwrap.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/BCCSRuntimeSup.cfg",			"temp/stdcpptest/general/testframework/BCCSRuntimeSup.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/BCException.cfg",				"temp/stdcpptest/general/testframework/BCException.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/BCNoNamespace.cfg",			"temp/stdcpptest/general/testframework/BCNoNamespace.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/BCOpenCHeaders.cfg",			"temp/stdcpptest/general/testframework/BCOpenCHeaders.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/BCOpenCWrap.cfg",				"temp/stdcpptest/general/testframework/BCOpenCWrap.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/BCThread.cfg",					"temp/stdcpptest/general/testframework/BCThread.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/BCTypeAttrib.cfg",				"temp/stdcpptest/general/testframework/BCTypeAttrib.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/BCTypeInfo.cfg",				"temp/stdcpptest/general/testframework/BCTypeInfo.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tmath_operations.cfg",			"temp/stdcpptest/general/testframework/tmath_operations.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tmemory.cfg",					"temp/stdcpptest/general/testframework/tmemory.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tutility.cfg",					"temp/stdcpptest/general/testframework/tutility.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tlocale.cfg",					"temp/stdcpptest/general/testframework/tlocale.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tstl_1.cfg",					"temp/stdcpptest/general/testframework/tstl_1.cfg") or die "failed : $!";

system("7z a -tzip stdcpptest.zip ./temp/*");
