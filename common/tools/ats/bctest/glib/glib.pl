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
# Script to build ATS test drop for GTK Library BC tests


use strict;
use File::Copy;
use File::Path;

unlink "glibtest.zip";
rmtree "temp";
mkpath "temp/glibtest/glib/general";
mkpath "temp/glibtest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("glib.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/accumulator.exe",		"temp/glibtest/winscw_udeb/accumulator.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/array-test.exe",		"temp/glibtest/winscw_udeb/array-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/asyncqueue-test.exe",	"temp/glibtest/winscw_udeb/asyncqueue-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/atomic-test.exe",		"temp/glibtest/winscw_udeb/atomic-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/child-test1.exe",		"temp/glibtest/winscw_udeb/child-test1.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/child-test2.exe",		"temp/glibtest/winscw_udeb/child-test2.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/closure-marshal.exe",	"temp/glibtest/winscw_udeb/closure-marshal.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/closures.exe",		"temp/glibtest/winscw_udeb/closures.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/completion-test.exe",	"temp/glibtest/winscw_udeb/completion-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/convert-test.exe",	"temp/glibtest/winscw_udeb/convert-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/date-test.exe",		"temp/glibtest/winscw_udeb/date-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/defaultiface.exe",	"temp/glibtest/winscw_udeb/defaultiface.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/dirname-test.exe",	"temp/glibtest/winscw_udeb/dirname-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/env-test.exe",		"temp/glibtest/winscw_udeb/env-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/error_check_mutexes.exe",	"temp/glibtest/winscw_udeb/error_check_mutexes.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/extra_tests.exe",		"temp/glibtest/winscw_udeb/extra_tests.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/file-test.exe",		"temp/glibtest/winscw_udeb/file-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/gio-test.exe",		"temp/glibtest/winscw_udeb/gio-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/gvalue-test.exe",		"temp/glibtest/winscw_udeb/gvalue-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/g_test1.exe",			"temp/glibtest/winscw_udeb/g_test1.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/g_test2.exe",			"temp/glibtest/winscw_udeb/g_test2.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/hash-test.exe",		"temp/glibtest/winscw_udeb/hash-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ifacecheck.exe",		"temp/glibtest/winscw_udeb/ifacecheck.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ifaceinherit.exe",	"temp/glibtest/winscw_udeb/ifaceinherit.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ifaceinit.exe",		"temp/glibtest/winscw_udeb/ifaceinit.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ifaceproperties.exe",	"temp/glibtest/winscw_udeb/ifaceproperties.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/iochannel-test.exe",	"temp/glibtest/winscw_udeb/iochannel-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/keyfile-test.exe",	"temp/glibtest/winscw_udeb/keyfile-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/list-test.exe",		"temp/glibtest/winscw_udeb/list-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/mainloop-test.exe",	"temp/glibtest/winscw_udeb/mainloop-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/mapping-test.exe",	"temp/glibtest/winscw_udeb/mapping-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/markup-escape-test.exe",	"temp/glibtest/winscw_udeb/markup-escape-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/markup-test.exe",		"temp/glibtest/winscw_udeb/markup-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/module-test.exe",		"temp/glibtest/winscw_udeb/module-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/node-test.exe",		"temp/glibtest/winscw_udeb/node-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/objects.exe",			"temp/glibtest/winscw_udeb/objects.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/objects2.exe",		"temp/glibtest/winscw_udeb/objects2.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/option-test.exe",		"temp/glibtest/winscw_udeb/option-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/override.exe",		"temp/glibtest/winscw_udeb/override.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/paramspec-test.exe",	"temp/glibtest/winscw_udeb/paramspec-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/patterntest.exe",		"temp/glibtest/winscw_udeb/patterntest.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/printf-test.exe",		"temp/glibtest/winscw_udeb/printf-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/properties.exe",		"temp/glibtest/winscw_udeb/properties.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/properties2.exe",		"temp/glibtest/winscw_udeb/properties2.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/qsort-test.exe",		"temp/glibtest/winscw_udeb/qsort-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/queue-test.exe",		"temp/glibtest/winscw_udeb/queue-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/rand-test.exe",		"temp/glibtest/winscw_udeb/rand-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/references.exe",		"temp/glibtest/winscw_udeb/references.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/relation-test.exe",	"temp/glibtest/winscw_udeb/relation-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/shell-test.exe",		"temp/glibtest/winscw_udeb/shell-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/signals-multithread.exe",	"temp/glibtest/winscw_udeb/signals-multithread.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/signals-singlethread.exe",	"temp/glibtest/winscw_udeb/signals-singlethread.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/slist-test.exe",		"temp/glibtest/winscw_udeb/slist-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/strfunc-test.exe",	"temp/glibtest/winscw_udeb/strfunc-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/string-test.exe",		"temp/glibtest/winscw_udeb/string-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/strtod-test.exe",		"temp/glibtest/winscw_udeb/strtod-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tasyncqueue.exe",		"temp/glibtest/winscw_udeb/tasyncqueue.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/testgdate.exe",		"temp/glibtest/winscw_udeb/testgdate.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/testgdateparser.exe",	"temp/glibtest/winscw_udeb/testgdateparser.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/testglib.exe",		"temp/glibtest/winscw_udeb/testglib.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/testgobject.exe",		"temp/glibtest/winscw_udeb/testgobject.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tgstring.exe",		"temp/glibtest/winscw_udeb/tgstring.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/thread-test.exe",		"temp/glibtest/winscw_udeb/thread-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/threadpool-test.exe",	"temp/glibtest/winscw_udeb/threadpool-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/timeloop-closure.exe",	"temp/glibtest/winscw_udeb/timeloop-closure.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/timeloop.exe",		"temp/glibtest/winscw_udeb/timeloop.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tmanual.exe",		"temp/glibtest/winscw_udeb/tmanual.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tmisc.exe",			"temp/glibtest/winscw_udeb/tmisc.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tnode.exe",			"temp/glibtest/winscw_udeb/tnode.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/toption.exe",			"temp/glibtest/winscw_udeb/toption.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tree-test.exe",		"temp/glibtest/winscw_udeb/tree-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tscanner.exe",		"temp/glibtest/winscw_udeb/tscanner.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tslist.exe",			"temp/glibtest/winscw_udeb/tslist.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tthread.exe",			"temp/glibtest/winscw_udeb/tthread.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ttrash.exe",			"temp/glibtest/winscw_udeb/ttrash.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ttree.exe",			"temp/glibtest/winscw_udeb/ttree.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tunichar.exe",		"temp/glibtest/winscw_udeb/tunichar.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/tutf8.exe",			"temp/glibtest/winscw_udeb/tutf8.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/typetest.exe",		"temp/glibtest/winscw_udeb/typetest.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/unichar_test.exe",	"temp/glibtest/winscw_udeb/unichar_test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/unicode-caseconv.exe",	"temp/glibtest/winscw_udeb/unicode-caseconv.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/unicode-collate.exe",	"temp/glibtest/winscw_udeb/unicode-collate.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/unicode-encoding.exe",	"temp/glibtest/winscw_udeb/unicode-encoding.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/unicode-normalize.exe",	"temp/glibtest/winscw_udeb/unicode-normalize.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/uri-test.exe",		"temp/glibtest/winscw_udeb/uri-test.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/utf8-pointer.exe",	"temp/glibtest/winscw_udeb/utf8-pointer.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/utf8-validate.exe",	"temp/glibtest/winscw_udeb/utf8-validate.exe") or die "failed : $!";

copy($epoc . "release/winscw/udeb/libmoduletestplugin_a.dll",	"temp/glibtest/winscw_udeb/libmoduletestplugin_a.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/libmoduletestplugin_b.dll",	"temp/glibtest/winscw_udeb/libmoduletestplugin_b.dll") or die "failed : $!";

copy($epoc . "winscw/c/temp/tmp/test.txt",				"temp/glibtest/glib/general/test.txt") or die "failed : $!";			
copy($epoc . "winscw/c/1.gmarkup",						"temp/glibtest/glib/general/1.gmarkup") or die "failed : $!";
copy($epoc . "winscw/c/iochannel-test-infile",			"temp/glibtest/glib/general/iochannel-test-infile") or die "failed : $!";
copy($epoc . "winscw/c/casemap.bin",					"temp/glibtest/glib/general/casemap.bin") or die "failed : $!";
copy($epoc . "winscw/c/casefold.bin",					"temp/glibtest/glib/general/casefold.bin") or die "failed : $!";
copy($epoc . "winscw/c/utf8.txt",						"temp/glibtest/glib/general/utf8.txt") or die "failed : $!";
copy($epoc . "winscw/c/casecollate.txt",				"temp/glibtest/glib/general/casecollate.txt") or die "failed : $!";
copy($epoc . "winscw/c/utf8_hindi.txt",					"temp/glibtest/glib/general/utf8_hindi.txt") or die "failed : $!";
copy($epoc . "winscw/c/scanfile.txt",					"temp/glibtest/glib/general/scanfile.txt") or die "failed : $!";
copy($epoc . "winscw/c/NormalizationTest.txt",			"temp/glibtest/glib/general/NormalizationTest.txt") or die "failed : $!";

system("7z a -tzip glibtest.zip ./temp/*");
