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
# Script to build ATS test drop multimedia svs tests


use strict;
use File::Copy;
use File::Path;

unlink "phonebooktest.zip";
##rmtree "temp";
mkpath "temp/phonebooktest/general/testframework";
mkpath "temp/phonebooktest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("phonebook.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/bctestpbkdataaddedit.dll",			"temp/phonebooktest/winscw_udeb/bctestpbkdataaddedit.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkdataaddedit.ini",	"temp/phonebooktest/general/testframework/testframework_pbkdataaddedit.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkdataaddedit.cfg",		"temp/phonebooktest/general/testframework/tcbctestpbkdataaddedit.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkdataaddedit.cfg",		"temp/phonebooktest/general/testframework/ui_bctestpbkdataaddedit.cfg");

copy($epoc . "release/winscw/udeb/mtrpbkviewresourcefile.dll",			"temp/phonebooktest/winscw_udeb/mtrpbkviewresourcefile.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkvrsc.ini",		"temp/phonebooktest/general/testframework/testframework_pbkvrsc.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkvrsc.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkvrsc.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkvrsc.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkvrsc.cfg");

copy($epoc . "release/winscw/udeb/mtpbkidlefinder.dll",					"temp/phonebooktest/winscw_udeb/mtpbkidlefinder.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkifinder.ini",		"temp/phonebooktest/general/testframework/testframework_pbkifinder.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkifinder.cfg",			"temp/phonebooktest/general/testframework/tcbctestpbkifinder.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkifinder.cfg",			"temp/phonebooktest/general/testframework/ui_bctestpbkifinder.cfg");

copy($epoc . "release/winscw/udeb/mtpbkiconinfo.dll",					"temp/phonebooktest/winscw_udeb/mtpbkiconinfo.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkinfo.ini",		"temp/phonebooktest/general/testframework/testframework_pbkinfo.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkinfo.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkinfo.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkinfo.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkinfo.cfg");

copy($epoc . "release/winscw/udeb/mtpbkiconid.dll",						"temp/phonebooktest/winscw_udeb/mtpbkiconid.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkicon.ini",		"temp/phonebooktest/general/testframework/testframework_pbkicon.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkicon.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkicon.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkicon.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkicon.cfg");

copy($epoc . "release/winscw/udeb/mtpbkfields.dll",						"temp/phonebooktest/winscw_udeb/mtpbkfields.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkfeilds.ini",		"temp/phonebooktest/general/testframework/testframework_pbkfeilds.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkfeilds.cfg",			"temp/phonebooktest/general/testframework/tcbctestpbkfeilds.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkfeilds.cfg",			"temp/phonebooktest/general/testframework/ui_bctestpbkfeilds.cfg");

copy($epoc . "release/winscw/udeb/mtpbkfielddata.dll",					"temp/phonebooktest/winscw_udeb/mtpbkfielddata.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkfdata.ini",		"temp/phonebooktest/general/testframework/testframework_pbkfdata.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkfdata.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkfdata.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkfdata.cfg",			"temp/phonebooktest/general/testframework/ui_bctestpbkfdata.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactiter.dll",				"temp/phonebooktest/winscw_udeb/mtpbkcontactiter.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkiter.ini",		"temp/phonebooktest/general/testframework/testframework_pbkiter.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkiter.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkiter.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkiter.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkiter.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactitemfield.dll",			"temp/phonebooktest/winscw_udeb/mtpbkcontactitemfield.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkcif.ini",			"temp/phonebooktest/general/testframework/testframework_pbkcif.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkcif.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkcif.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkcif.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkcif.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactengine.dll",				"temp/phonebooktest/winscw_udeb/mtpbkcontactengine.dll");
copy($epoc . "winscw/c/testframework/testframework_cengine.ini",		"temp/phonebooktest/general/testframework/testframework_cengine.ini");
copy($epoc . "winscw/c/testframework/tcbctestcengine.cfg",				"temp/phonebooktest/general/testframework/tcbctestcengine.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestcengine.cfg",				"temp/phonebooktest/general/testframework/ui_bctestcengine.cfg");

copy($epoc . "release/winscw/udeb/mtpbkfieldsinfo.dll",					"temp/phonebooktest/winscw_udeb/mtpbkfieldsinfo.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkfinfo.ini",		"temp/phonebooktest/general/testframework/testframework_pbkfinfo.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkfinfo.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkfinfo.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkfinfo.cfg",			"temp/phonebooktest/general/testframework/ui_bctestpbkfinfo.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactitem.dll",				"temp/phonebooktest/winscw_udeb/mtpbkcontactitem.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkcitem.ini",		"temp/phonebooktest/general/testframework/testframework_pbkcitem.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkcitem.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkcitem.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkcitem.cfg",			"temp/phonebooktest/general/testframework/ui_bctestpbkcitem.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactchangenotifier.dll",		"temp/phonebooktest/winscw_udeb/mtpbkcontactchangenotifier.dll");
copy($epoc . "winscw/c/testframework/testframework_pbknotifier.ini",	"temp/phonebooktest/general/testframework/testframework_pbknotifier.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbknotifier.cfg",			"temp/phonebooktest/general/testframework/tcbctestpbknotifier.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkpbknotifier.cfg",		"temp/phonebooktest/general/testframework/ui_bctestpbkpbknotifier.cfg");

copy($epoc . "release/winscw/udeb/bctestpbkuiutil.dll",                 "temp/phonebooktest/winscw_udeb/bctestpbkuiutil.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkuiutil.ini",      "temp/phonebooktest/general/testframework/testframework_pbkuiutil.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkuiutil.cfg",            "temp/phonebooktest/general/testframework/tcbctestpbkuiutil.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkuiutil.cfg",           "temp/phonebooktest/general/testframework/ui_bctestpbkuiutil.cfg");

system("7z a -tzip phonebooktest.zip ./temp/*");
