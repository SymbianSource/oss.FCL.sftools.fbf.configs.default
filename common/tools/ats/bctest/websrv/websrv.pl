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
##rmtree "temp";
mkpath "temp/bcwebsrv/general/testframework";
mkpath "temp/bcwebsrv/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("websrv.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/CSenServiceConnectionBCTest.dll",     "temp/bcwebsrv/winscw_udeb/CSenServiceConnectionBCTest.dll");
copy($epoc . "winscw/c/testframework/testframework_connection.ini",     "temp/bcwebsrv/general/testframework/testframework_connection.ini");
copy($epoc . "release/winscw/udeb/HostletConnectionBCTest.dll",         "temp/bcwebsrv/winscw_udeb/HostletConnectionBCTest.dll");
copy($epoc . "winscw/c/testframework/TestFramework_hostlet.ini",        "temp/bcwebsrv/general/testframework/TestFramework_hostlet.ini");
copy($epoc . "release/winscw/udeb/newsoapclassesbctest.dll",            "temp/bcwebsrv/winscw_udeb/newsoapclassesbctest.dll");
copy($epoc . "winscw/c/testframework/testframework_newsoapclasses.ini", "temp/bcwebsrv/general/testframework/testframework_newsoapclasses.ini");

system("7z a -tzip websrv.zip ./temp/*");