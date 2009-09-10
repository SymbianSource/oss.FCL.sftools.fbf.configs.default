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
mkpath "temp/phonebooktest/general/bcardtest2";
mkpath "temp/phonebooktest/winscw_udeb/z/resource/plugins";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("phonebook.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/bctestpbkdataaddedit.dll",					"temp/phonebooktest/winscw_udeb/bctestpbkdataaddedit.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkdataaddedit.ini",			"temp/phonebooktest/general/testframework/testframework_pbkdataaddedit.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkdataaddedit.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkdataaddedit.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkdataaddedit.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkdataaddedit.cfg");

copy($epoc . "release/winscw/udeb/mtrpbkviewresourcefile.dll",					"temp/phonebooktest/winscw_udeb/mtrpbkviewresourcefile.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkvrsc.ini",				"temp/phonebooktest/general/testframework/testframework_pbkvrsc.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkvrsc.cfg",						"temp/phonebooktest/general/testframework/tcbctestpbkvrsc.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkvrsc.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkvrsc.cfg");

copy($epoc . "release/winscw/udeb/mtpbkidlefinder.dll",							"temp/phonebooktest/winscw_udeb/mtpbkidlefinder.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkifinder.ini",				"temp/phonebooktest/general/testframework/testframework_pbkifinder.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkifinder.cfg",					"temp/phonebooktest/general/testframework/tcbctestpbkifinder.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkifinder.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkifinder.cfg");

copy($epoc . "release/winscw/udeb/mtpbkiconinfo.dll",							"temp/phonebooktest/winscw_udeb/mtpbkiconinfo.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkinfo.ini",				"temp/phonebooktest/general/testframework/testframework_pbkinfo.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkinfo.cfg",						"temp/phonebooktest/general/testframework/tcbctestpbkinfo.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkinfo.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkinfo.cfg");

copy($epoc . "release/winscw/udeb/mtpbkiconid.dll",								"temp/phonebooktest/winscw_udeb/mtpbkiconid.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkicon.ini",				"temp/phonebooktest/general/testframework/testframework_pbkicon.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkicon.cfg",						"temp/phonebooktest/general/testframework/tcbctestpbkicon.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkicon.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkicon.cfg");

copy($epoc . "release/winscw/udeb/mtpbkfields.dll",								"temp/phonebooktest/winscw_udeb/mtpbkfields.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkfeilds.ini",				"temp/phonebooktest/general/testframework/testframework_pbkfeilds.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkfeilds.cfg",					"temp/phonebooktest/general/testframework/tcbctestpbkfeilds.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkfeilds.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkfeilds.cfg");

copy($epoc . "release/winscw/udeb/mtpbkfielddata.dll",							"temp/phonebooktest/winscw_udeb/mtpbkfielddata.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkfdata.ini",				"temp/phonebooktest/general/testframework/testframework_pbkfdata.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkfdata.cfg",						"temp/phonebooktest/general/testframework/tcbctestpbkfdata.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkfdata.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkfdata.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactiter.dll",						"temp/phonebooktest/winscw_udeb/mtpbkcontactiter.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkiter.ini",				"temp/phonebooktest/general/testframework/testframework_pbkiter.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkiter.cfg",						"temp/phonebooktest/general/testframework/tcbctestpbkiter.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkiter.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkiter.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactitemfield.dll",					"temp/phonebooktest/winscw_udeb/mtpbkcontactitemfield.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkcif.ini",					"temp/phonebooktest/general/testframework/testframework_pbkcif.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkcif.cfg",						"temp/phonebooktest/general/testframework/tcbctestpbkcif.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkcif.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkcif.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactengine.dll",						"temp/phonebooktest/winscw_udeb/mtpbkcontactengine.dll");
copy($epoc . "winscw/c/testframework/testframework_cengine.ini",				"temp/phonebooktest/general/testframework/testframework_cengine.ini");
copy($epoc . "winscw/c/testframework/tcbctestcengine.cfg",						"temp/phonebooktest/general/testframework/tcbctestcengine.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestcengine.cfg",						"temp/phonebooktest/general/testframework/ui_bctestcengine.cfg");

copy($epoc . "release/winscw/udeb/mtpbkfieldsinfo.dll",							"temp/phonebooktest/winscw_udeb/mtpbkfieldsinfo.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkfinfo.ini",				"temp/phonebooktest/general/testframework/testframework_pbkfinfo.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkfinfo.cfg",						"temp/phonebooktest/general/testframework/tcbctestpbkfinfo.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkfinfo.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkfinfo.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactitem.dll",						"temp/phonebooktest/winscw_udeb/mtpbkcontactitem.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkcitem.ini",				"temp/phonebooktest/general/testframework/testframework_pbkcitem.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkcitem.cfg",						"temp/phonebooktest/general/testframework/tcbctestpbkcitem.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkcitem.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkcitem.cfg");

copy($epoc . "release/winscw/udeb/mtpbkcontactchangenotifier.dll",				"temp/phonebooktest/winscw_udeb/mtpbkcontactchangenotifier.dll");
copy($epoc . "winscw/c/testframework/testframework_pbknotifier.ini",			"temp/phonebooktest/general/testframework/testframework_pbknotifier.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbknotifier.cfg",					"temp/phonebooktest/general/testframework/tcbctestpbknotifier.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkpbknotifier.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkpbknotifier.cfg");

copy($epoc . "release/winscw/udeb/mtpbksmsaddressselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbksmsaddressselect.dll");
copy($epoc . "winscw/c/testframework/testframework_smsaddsel.ini",				"temp/phonebooktest/general/testframework/testframework_smsaddsel.ini");
copy($epoc . "winscw/c/testframework/tcbctestsmsaddsel.cfg",					"temp/phonebooktest/general/testframework/tcbctestsmsaddsel.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestsmsaddsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestsmsaddsel.cfg");

copy($epoc . "release/winscw/udeb/mtpbksingleentryfetchdlg.dll",				"temp/phonebooktest/winscw_udeb/mtpbksingleentryfetchdlg.dll");
copy($epoc . "winscw/c/testframework/testframework_sefielddlg.ini",				"temp/phonebooktest/general/testframework/testframework_sefielddlg.ini");
copy($epoc . "winscw/c/testframework/tcbctestsefielddlg.cfg",					"temp/phonebooktest/general/testframework/tcbctestsefielddlg.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestsefielddlg.cfg",					"temp/phonebooktest/general/testframework/ui_bctestsefielddlg.cfg");

copy($epoc . "release/winscw/udeb/mtpbkpocaddressselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbkpocaddressselect.dll");
copy($epoc . "winscw/c/testframework/testframework_pocadd.ini",					"temp/phonebooktest/general/testframework/testframework_pocadd.ini");
copy($epoc . "winscw/c/testframework/tcbctestpocadd.cfg",						"temp/phonebooktest/general/testframework/tcbctestpocadd.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpocadd.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpocadd.cfg");

copy($epoc . "release/winscw/udeb/mtpbkphonenumberselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbkphonenumberselect.dll");
copy($epoc . "winscw/c/testframework/testframework_phonenumber.ini",			"temp/phonebooktest/general/testframework/testframework_phonenumber.ini");
copy($epoc . "winscw/c/testframework/tcbctestphonenumber.cfg",					"temp/phonebooktest/general/testframework/tcbctestphonenumber.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestphonenumber.cfg",					"temp/phonebooktest/general/testframework/ui_bctestphonenumber.cfg");

copy($epoc . "release/winscw/udeb/mtpbkmultipleentryfetchdlg.dll",				"temp/phonebooktest/winscw_udeb/mtpbkmultipleentryfetchdlg.dll");
copy($epoc . "winscw/c/testframework/testframework_fetchdlg.ini",				"temp/phonebooktest/general/testframework/testframework_fetchdlg.ini");
copy($epoc . "winscw/c/testframework/tcbctestfetchdlg.cfg",						"temp/phonebooktest/general/testframework/tcbctestfetchdlg.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestfetchdlg.cfg",					"temp/phonebooktest/general/testframework/ui_bctestfetchdlg.cfg");

copy($epoc . "release/winscw/udeb/mtpbkmmsaddressselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbkmmsaddressselect.dll");
copy($epoc . "winscw/c/testframework/testframework_mmsaddsel.ini",				"temp/phonebooktest/general/testframework/testframework_mmsaddsel.ini");
copy($epoc . "winscw/c/testframework/tcbctestmmsaddsel.cfg",					"temp/phonebooktest/general/testframework/tcbctestmmsaddsel.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestmmsaddsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestmmsaddsel.cfg");

copy($epoc . "release/winscw/udeb/mtpbkmemoryentryadditemdlg.dll",				"temp/phonebooktest/winscw_udeb/mtpbkmemoryentryadditemdlg.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkadditemdlg.ini",			"temp/phonebooktest/general/testframework/testframework_pbkadditemdlg.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkadditemdlg.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkadditemdlg.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkadditemdlg.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkadditemdlg.cfg");

copy($epoc . "release/winscw/udeb/mtpbkfetchdlgselection.dll",					"temp/phonebooktest/winscw_udeb/mtpbkfetchdlgselection.dll");
copy($epoc . "winscw/c/testframework/testframework_fetchsel.ini",				"temp/phonebooktest/general/testframework/testframework_fetchsel.ini");
copy($epoc . "winscw/c/testframework/tcbctestfetchsel.cfg",						"temp/phonebooktest/general/testframework/tcbctestfetchsel.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestfetchsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestfetchsel.cfg");

copy($epoc . "release/winscw/udeb/mtpbkfetchcallbacks.dll",						"temp/phonebooktest/winscw_udeb/mtpbkfetchcallbacks.dll");
copy($epoc . "winscw/c/testframework/testframework_fcallback.ini",				"temp/phonebooktest/general/testframework/testframework_fcallback.ini");
copy($epoc . "winscw/c/testframework/tcbctestfcallback.cfg",					"temp/phonebooktest/general/testframework/tcbctestfcallback.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestfcallback.cfg",					"temp/phonebooktest/general/testframework/ui_bctestfcallback.cfg");

copy($epoc . "release/winscw/udeb/mtpbkemailoversms.dll",						"temp/phonebooktest/winscw_udeb/mtpbkemailoversms.dll");
copy($epoc . "winscw/c/testframework/testframework_emailoversms.ini",			"temp/phonebooktest/general/testframework/testframework_emailoversms.ini");
copy($epoc . "winscw/c/testframework/tcbctestemaioversms.cfg",					"temp/phonebooktest/general/testframework/tcbctestemaioversms.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestemaioversms.cfg",					"temp/phonebooktest/general/testframework/ui_bctestemaioversms.cfg");

copy($epoc . "release/winscw/udeb/mtpbkemailaddressselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbkemailaddressselect.dll");
copy($epoc . "winscw/c/testframework/testframework_addemailsel.ini",			"temp/phonebooktest/general/testframework/testframework_addemailsel.ini");
copy($epoc . "winscw/c/testframework/tcbctestemailaddsel.cfg",					"temp/phonebooktest/general/testframework/tcbctestemailaddsel.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestemailaddsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestemailaddsel.cfg");

copy($epoc . "release/winscw/udeb/mtpbkaddressselect.dll",						"temp/phonebooktest/winscw_udeb/mtpbkaddressselect.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkaddsel.ini",				"temp/phonebooktest/general/testframework/testframework_pbkaddsel.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkaddsel.cfg",					"temp/phonebooktest/general/testframework/tcbctestpbkaddsel.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkaddsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkaddsel.cfg");
copy($epoc . "release/winscw/udeb/z/resource/pbkaddressselect.rsc",				"temp/phonebooktest/winscw_udeb/z/resource/plugins/pbkaddressselect.rsc");

copy($epoc . "release/winscw/udeb/mtpbkphonenumberselectbase.dll",				"temp/phonebooktest/winscw_udeb/mtpbkphonenumberselectbase.dll");
copy($epoc . "winscw/c/testframework/testframework_numsel.ini",					"temp/phonebooktest/general/testframework/testframework_numsel.ini");
copy($epoc . "winscw/c/testframework/tcbctestnumsel.cfg",						"temp/phonebooktest/general/testframework/tcbctestnumsel.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestnumsel.cfg",						"temp/phonebooktest/general/testframework/ui_bctestnumsel.cfg");
copy($epoc . "release/winscw/udeb/z/resource/mt_cpbkphonenumberselectbase.rsc",	"temp/phonebooktest/winscw_udeb/z/resource/plugins/mt_cpbkphonenumberselectbase.rsc");

copy($epoc . "release/winscw/udeb/mtpbkselectfielddlg.dll",						"temp/phonebooktest/winscw_udeb/mtpbkselectfielddlg.dll");
copy($epoc . "winscw/c/testframework/testframework_fielddlg.ini",				"temp/phonebooktest/general/testframework/testframework_fielddlg.ini");
copy($epoc . "winscw/c/testframework/tcbctestfielddlg.cfg",						"temp/phonebooktest/general/testframework/tcbctestfielddlg.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestfielddlg.cfg",					"temp/phonebooktest/general/testframework/ui_bctestfielddlg.cfg");

copy($epoc . "release/winscw/udeb/mtpbksingleitemfetchdlg.dll",					"temp/phonebooktest/winscw_udeb/mtpbksingleitemfetchdlg.dll");
copy($epoc . "winscw/c/testframework/testframework_sifielddlg.ini",				"temp/phonebooktest/general/testframework/testframework_sifielddlg.ini");
copy($epoc . "winscw/c/testframework/tcbctestsifielddlg.cfg",					"temp/phonebooktest/general/testframework/tcbctestsifielddlg.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestsifielddlg.cfg",					"temp/phonebooktest/general/testframework/ui_bctestsifielddlg.cfg");

copy($epoc . "release/winscw/udeb/bctestpbkuiutil.dll",							"temp/phonebooktest/winscw_udeb/bctestpbkuiutil.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkuiutil.ini",				"temp/phonebooktest/general/testframework/testframework_pbkuiutil.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkuiutil.cfg",					"temp/phonebooktest/general/testframework/tcbctestpbkuiutil.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkuiutil.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkuiutil.cfg");

copy($epoc . "release/winscw/udeb/bctestpbkvcardimportexport.dll",				"temp/phonebooktest/winscw_udeb/bctestpbkvcardimportexport.dll");
copy($epoc . "winscw/c/testframework/testframework_pbkvcardimportexport.ini",	"temp/phonebooktest/general/testframework/testframework_pbkvcardimportexport.ini");
copy($epoc . "winscw/c/testframework/tcbctestpbkvcardimportexport.cfg",			"temp/phonebooktest/general/testframework/tcbctestpbkvcardimportexport.cfg");
copy($epoc . "winscw/c/testframework/ui_bctestpbkvcardimportexport.cfg",		"temp/phonebooktest/general/testframework/ui_bctestpbkvcardimportexport.cfg");
copy($epoc . "winscw/c/bcardtest2/basic02.src",									"temp/phonebooktest/general/bcardtest2/basic02.src");
copy($epoc . "winscw/c/bcardtest2/basic03.vcf",									"temp/phonebooktest/general/bcardtest2/basic03.vcf");
copy($epoc . "winscw/c/bcardtest2/basiccc01.ccf",								"temp/phonebooktest/general/bcardtest2/basiccc01.ccf");
copy($epoc . "winscw/c/bcardtest2/basiccc02.ccf",								"temp/phonebooktest/general/bcardtest2/basiccc02.ccf");
copy($epoc . "winscw/c/bcardtest2/basiccompactmaxsize.ccf",						"temp/phonebooktest/general/bcardtest2/basiccompactmaxsize.ccf");
copy($epoc . "winscw/c/bcardtest2/basiccompactnobusiness.ccf",					"temp/phonebooktest/general/bcardtest2/basiccompactnobusiness.ccf");
copy($epoc . "winscw/c/bcardtest2/dummy.vcf",									"temp/phonebooktest/general/bcardtest2/dummy.vcf");
copy($epoc . "winscw/c/bcardtest2/johnpic.gif",									"temp/phonebooktest/general/bcardtest2/johnpic.gif");
copy($epoc . "winscw/c/bcardtest2/picture2.vcf",								"temp/phonebooktest/general/bcardtest2/picture2.vcf");

copy($epoc . "release/winscw/udeb/bctestspeeddial.dll",							"temp/phonebooktest/winscw_udeb/bctestspeeddial.dll");
copy($epoc . "winscw/c/testframework/testframework_spdial.ini",					"temp/phonebooktest/general/testframework/testframework_spdial.ini");
copy($epoc . "winscw/c/testframework/tcspeeddial.cfg",							"temp/phonebooktest/general/testframework/tcspeeddial.cfg");
copy($epoc . "winscw/c/testframework/ui_speeddial.cfg",							"temp/phonebooktest/general/testframework/ui_speeddial.cfg");

system("7z a -tzip phonebooktest.zip ./temp/*");
