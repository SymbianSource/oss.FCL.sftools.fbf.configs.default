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
# Maciej Seroka <maciejs@symbian.org>
#
# Description:
# Script to build ATS test drop for BC Web Services tests


use strict;
use File::Copy;
use File::Path;

unlink "websrv.zip";
rmtree "temp";
mkpath "temp/bcwebsrv/general/testframework";
mkpath "temp/bcwebsrv/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("websrv.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/CSenServiceConnectionBCTest.dll",         "temp/bcwebsrv/winscw_udeb/CSenServiceConnectionBCTest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_connection.ini",         "temp/bcwebsrv/general/testframework/testframework_connection.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/HostletConnectionBCTest.dll",             "temp/bcwebsrv/winscw_udeb/HostletConnectionBCTest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/TestFramework_hostlet.ini",            "temp/bcwebsrv/general/testframework/TestFramework_hostlet.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/newsoapclassesbctest.dll",                "temp/bcwebsrv/winscw_udeb/newsoapclassesbctest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_newsoapclasses.ini",     "temp/bcwebsrv/general/testframework/testframework_newsoapclasses.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/SenServDescBCTest.dll",                   "temp/bcwebsrv/winscw_udeb/SenServDescBCTest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_senservdesc.ini",        "temp/bcwebsrv/general/testframework/testframework_senservdesc.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/SenServiceManagerBCTest.dll",             "temp/bcwebsrv/winscw_udeb/SenServiceManagerBCTest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_senservicemanager.ini",  "temp/bcwebsrv/general/testframework/testframework_senservicemanager.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/SenUtilsBCTest.dll",                      "temp/bcwebsrv/winscw_udeb/SenUtilsBCTest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_senutils.ini",           "temp/bcwebsrv/general/testframework/testframework_senutils.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/SenFragmentBCTest.dll",                   "temp/bcwebsrv/winscw_udeb/SenFragmentBCTest.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/TestFramework_senfragment.ini",        "temp/bcwebsrv/general/testframework/TestFramework_senfragment.ini") or die "failed : $!";

system("7z a -tzip websrv.zip ./temp/*");