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
# Script to build ATS test drop for Standard Libraries BC tests


use strict;
use File::Copy;
use File::Path;

unlink "stdlibstest.zip";
rmtree "temp";
mkpath "temp/stdlibstest/tstdapis/general";
mkpath "temp/stdlibstest/winscw_udeb";
mkpath "temp/stdlibstest/systemdata/general";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("stdlibs.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/ConfTimeZone.exe",		"temp/stdlibstest/winscw_udeb/ConfTimeZone.exe") or die "failed : $!";

#libm

copy($epoc . "release/winscw/udeb/tdouble_blr.exe",			"temp/stdlibstest/winscw_udeb/tdouble_blr.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tldouble_blr.exe",		"temp/stdlibstest/winscw_udeb/tldouble_blr.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tfloat_blr.exe",			"temp/stdlibstest/winscw_udeb/tfloat_blr.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tdouble_blr.ini",			"temp/stdlibstest/tstdapis/general/tdouble_blr.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tdouble_blr.script",		"temp/stdlibstest/tstdapis/general/tdouble_blr.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tdouble_pow.script",		"temp/stdlibstest/tstdapis/general/tdouble_pow.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tdouble_misc.script",		"temp/stdlibstest/tstdapis/general/tdouble_misc.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tfloat_blr.script",			"temp/stdlibstest/tstdapis/general/tfloat_blr.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tfloat_blr.ini",			"temp/stdlibstest/tstdapis/general/tfloat_blr.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tldouble_blr.script",		"temp/stdlibstest/tstdapis/general/tldouble_blr.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tldouble_pow.script",		"temp/stdlibstest/tstdapis/general/tldouble_pow.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tldouble_misc.script",		"temp/stdlibstest/tstdapis/general/tldouble_misc.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tldouble_blr.ini",			"temp/stdlibstest/tstdapis/general/tldouble_blr.ini") or die "failed : $!";

#libpthread
copy($epoc . "release/winscw/udeb/testharness.dll",			"temp/stdlibstest/winscw_udeb/testharness.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tcondbroadcast.exe",		"temp/stdlibstest/winscw_udeb/tcondbroadcast.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tcondbroadcast.script",		"temp/stdlibstest/tstdapis/general/tcondbroadcast.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tconddestroy.exe",		"temp/stdlibstest/winscw_udeb/tconddestroy.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tconddestroy.script",		"temp/stdlibstest/tstdapis/general/tconddestroy.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tcondinit.exe",			"temp/stdlibstest/winscw_udeb/tcondinit.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tcondinit.script",			"temp/stdlibstest/tstdapis/general/tcondinit.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tcondsignal.exe",			"temp/stdlibstest/winscw_udeb/tcondsignal.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tcondsignal.script",		"temp/stdlibstest/tstdapis/general/tcondsignal.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tcondwait.exe",			"temp/stdlibstest/winscw_udeb/tcondwait.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tcondwait.script",			"temp/stdlibstest/tstdapis/general/tcondwait.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tmutex.exe",				"temp/stdlibstest/winscw_udeb/tmutex.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tmutex.script",				"temp/stdlibstest/tstdapis/general/tmutex.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tmutex.ini",				"temp/stdlibstest/tstdapis/general/tmutex.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tpthread.exe",			"temp/stdlibstest/winscw_udeb/tpthread.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tpthread.ini",				"temp/stdlibstest/tstdapis/general/tpthread.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tpthread.script",			"temp/stdlibstest/tstdapis/general/tpthread.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tpthreadonce.exe",		"temp/stdlibstest/winscw_udeb/tpthreadonce.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tpthreadonce.script",		"temp/stdlibstest/tstdapis/general/tpthreadonce.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsemdestroy.exe",			"temp/stdlibstest/winscw_udeb/tsemdestroy.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsemdestroy.script",		"temp/stdlibstest/tstdapis/general/tsemdestroy.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsemgetvalue.exe",		"temp/stdlibstest/winscw_udeb/tsemgetvalue.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsemgetvalue.script",		"temp/stdlibstest/tstdapis/general/tsemgetvalue.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tseminit.exe",			"temp/stdlibstest/winscw_udeb/tseminit.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tseminit.script",			"temp/stdlibstest/tstdapis/general/tseminit.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsemopen.exe",			"temp/stdlibstest/winscw_udeb/tsemopen.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsemopen.script",			"temp/stdlibstest/tstdapis/general/tsemopen.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsempost.exe",			"temp/stdlibstest/winscw_udeb/tsempost.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsempost.script",			"temp/stdlibstest/tstdapis/general/tsempost.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsemtimedwait.exe",		"temp/stdlibstest/winscw_udeb/tsemtimedwait.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsemtimedwait.script",		"temp/stdlibstest/tstdapis/general/tsemtimedwait.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsemtrywait.exe",			"temp/stdlibstest/winscw_udeb/tsemtrywait.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsemtrywait.script",		"temp/stdlibstest/tstdapis/general/tsemtrywait.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsemwait.exe",			"temp/stdlibstest/winscw_udeb/tsemwait.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsemwait.script",			"temp/stdlibstest/tstdapis/general/tsemwait.script") or die "failed : $!";

#libdl
copy($epoc . "release/winscw/udeb/tdl.exe",					"temp/stdlibstest/winscw_udeb/tdl.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tdl.script",				"temp/stdlibstest/tstdapis/general/tdl.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tdl.ini",					"temp/stdlibstest/tstdapis/general/tdl.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/arithmeticoper1.dll",		"temp/stdlibstest/winscw_udeb/arithmeticoper1.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/arithmeticoper2.dll",		"temp/stdlibstest/winscw_udeb/arithmeticoper2.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/arithmeticoper3.dll",		"temp/stdlibstest/winscw_udeb/arithmeticoper3.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/arithmeticoper4.dll",		"temp/stdlibstest/winscw_udeb/arithmeticoper4.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/dll1.dll",				"temp/stdlibstest/winscw_udeb/dll1.dll") or die "failed : $!";

#libc
copy($epoc . "release/winscw/udeb/tcomport.exe",			"temp/stdlibstest/winscw_udeb/tcomport.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tcomport.script",			"temp/stdlibstest/tstdapis/general/tcomport.script") or die "failed : $!";
copy($epoc . "winscw/c/system/data/config.ini",				"temp/stdlibstest/systemdata/general/config.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tctype.exe",				"temp/stdlibstest/winscw_udeb/tctype.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tctype.script",				"temp/stdlibstest/tstdapis/general/tctype.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tdb_blr.exe",				"temp/stdlibstest/winscw_udeb/tdb_blr.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tdb_blr.script",			"temp/stdlibstest/tstdapis/general/tdb_blr.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tdb_blr.ini",				"temp/stdlibstest/tstdapis/general/tdb_blr.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tftw.exe",				"temp/stdlibstest/winscw_udeb/tftw.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tftw.script",				"temp/stdlibstest/tstdapis/general/tftw.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tftw.ini",					"temp/stdlibstest/tstdapis/general/tftw.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tlibcwchar.exe",			"temp/stdlibstest/winscw_udeb/tlibcwchar.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tlibcwchar.script",			"temp/stdlibstest/tstdapis/general/tlibcwchar.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tlink.exe",				"temp/stdlibstest/winscw_udeb/tlink.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tlink.script",				"temp/stdlibstest/tstdapis/general/tlink.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tlink.ini",					"temp/stdlibstest/tstdapis/general/tlink.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tloc_blr.exe",			"temp/stdlibstest/winscw_udeb/tloc_blr.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr.script",			"temp/stdlibstest/tstdapis/general/tloc_blr.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_iconv.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_iconv.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_posix.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_posix.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr1.ini",				"temp/stdlibstest/tstdapis/general/tloc_blr1.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr2.ini",				"temp/stdlibstest/tstdapis/general/tloc_blr2.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant1.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_variant1.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant2.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_variant2.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant3.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_variant3.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant4.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_variant4.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant5.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_variant5.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant6.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_variant6.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant7.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_variant7.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant8.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_variant8.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant9.ini",		"temp/stdlibstest/tstdapis/general/tloc_blr_variant9.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant10.ini",	"temp/stdlibstest/tstdapis/general/tloc_blr_variant10.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant11.ini",	"temp/stdlibstest/tstdapis/general/tloc_blr_variant11.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant13.ini",	"temp/stdlibstest/tstdapis/general/tloc_blr_variant13.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant14.ini",	"temp/stdlibstest/tstdapis/general/tloc_blr_variant14.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant15.ini",	"temp/stdlibstest/tstdapis/general/tloc_blr_variant15.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant16.ini",	"temp/stdlibstest/tstdapis/general/tloc_blr_variant16.ini") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tloc_blr_variant17.ini",	"temp/stdlibstest/tstdapis/general/tloc_blr_variant17.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tmkfifo.exe",				"temp/stdlibstest/winscw_udeb/tmkfifo.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tmkfifo.script",			"temp/stdlibstest/tstdapis/general/tmkfifo.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tmkfifo.ini",				"temp/stdlibstest/tstdapis/general/tmkfifo.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tmmap.exe",				"temp/stdlibstest/winscw_udeb/tmmap.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tmmap.script",				"temp/stdlibstest/tstdapis/general/tmmap.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tmmap.ini",					"temp/stdlibstest/tstdapis/general/tmmap.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tprogname.exe",			"temp/stdlibstest/winscw_udeb/tprogname.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tprogname.script",			"temp/stdlibstest/tstdapis/general/tprogname.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tregex.exe",				"temp/stdlibstest/winscw_udeb/tregex.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tregex.script",				"temp/stdlibstest/tstdapis/general/tregex.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tregex.ini",				"temp/stdlibstest/tstdapis/general/tregex.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tselect.exe",				"temp/stdlibstest/winscw_udeb/tselect.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tselect.script",			"temp/stdlibstest/tstdapis/general/tselect.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsemaphore.exe",			"temp/stdlibstest/winscw_udeb/tsemaphore.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsemaphore.script",			"temp/stdlibstest/tstdapis/general/tsemaphore.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsemaphore.ini",			"temp/stdlibstest/tstdapis/general/tsemaphore.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tshm.exe",				"temp/stdlibstest/winscw_udeb/tshm.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tshm.script",				"temp/stdlibstest/tstdapis/general/tshm.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tshm.ini",					"temp/stdlibstest/tstdapis/general/tshm.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tstdio.exe",				"temp/stdlibstest/winscw_udeb/tstdio.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tstdio_auto.script",		"temp/stdlibstest/tstdapis/general/tstdio_auto.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tstdio_manual.script",		"temp/stdlibstest/tstdapis/general/tstdio_manual.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tstdio_fileop.script",		"temp/stdlibstest/tstdapis/general/tstdio_fileop.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tstdio.ini",				"temp/stdlibstest/tstdapis/general/tstdio.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tstring.exe",				"temp/stdlibstest/winscw_udeb/tstring.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tstring.script",			"temp/stdlibstest/tstdapis/general/tstring.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tstring.ini",				"temp/stdlibstest/tstdapis/general/tstring.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsyscalls.exe",			"temp/stdlibstest/winscw_udeb/tsyscalls.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsyscalls.script",			"temp/stdlibstest/tstdapis/general/tsyscalls.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsyscalls1.script",			"temp/stdlibstest/tstdapis/general/tsyscalls1.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsyscalls2.script",			"temp/stdlibstest/tstdapis/general/tsyscalls2.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsyscalls.ini",				"temp/stdlibstest/tstdapis/general/tsyscalls.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsyssim.exe",				"temp/stdlibstest/winscw_udeb/tsyssim.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsyssim.script",			"temp/stdlibstest/tstdapis/general/tsyssim.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tsysunistd.exe",			"temp/stdlibstest/winscw_udeb/tsysunistd.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsysunistd.script",			"temp/stdlibstest/tstdapis/general/tsysunistd.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tsysunistd.ini",			"temp/stdlibstest/tstdapis/general/tsysunistd.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/twchar.exe",				"temp/stdlibstest/winscw_udeb/twchar.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/twchar.script",				"temp/stdlibstest/tstdapis/general/twchar.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/twcharapi.exe",			"temp/stdlibstest/winscw_udeb/twcharapi.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/twcharapi.script",			"temp/stdlibstest/tstdapis/general/twcharapi.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/twcharapi.ini",				"temp/stdlibstest/tstdapis/general/twcharapi.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/twctype.exe",				"temp/stdlibstest/winscw_udeb/twctype.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/twctype.script",			"temp/stdlibstest/tstdapis/general/twctype.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/twctype.ini",				"temp/stdlibstest/tstdapis/general/twctype.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/twideapis.exe",			"temp/stdlibstest/winscw_udeb/twideapis.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/twideapis.script",			"temp/stdlibstest/tstdapis/general/twideapis.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/twideapis.ini",				"temp/stdlibstest/tstdapis/general/twideapis.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tglob.exe",				"temp/stdlibstest/winscw_udeb/tglob.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/tglob_test.script",			"temp/stdlibstest/tstdapis/general/tglob_test.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ttime_blr.exe",			"temp/stdlibstest/winscw_udeb/ttime_blr.exe") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/ttime_blr.script",			"temp/stdlibstest/tstdapis/general/ttime_blr.script") or die "failed : $!";
copy($epoc . "winscw/c/tstdapis/ttime_blr.ini",				"temp/stdlibstest/tstdapis/general/ttime_blr.ini") or die "failed : $!";

system("7z a -tzip stdlibstest.zip ./temp/*");