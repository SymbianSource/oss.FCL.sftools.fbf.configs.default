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
# Script to build ATS test drop contacts phonebook tests


use strict;
use File::Copy;
use File::Path;

unlink "phonebooktest.zip";
rmtree "temp";
mkpath "temp/phonebooktest/general/testframework";
mkpath "temp/phonebooktest/general/resource";
mkpath "temp/phonebooktest/general/bcardtest2";
mkpath "temp/phonebooktest/winscw_udeb/z/resource";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("phonebook.xml",	"temp/test.xml");

copy($epoc . "release/winscw/udeb/bctestpbkdataaddedit.dll",					"temp/phonebooktest/winscw_udeb/bctestpbkdataaddedit.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_pbkdataaddedit.ini",			"temp/phonebooktest/general/testframework/testframework_pbkdataaddedit.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestpbkdataaddedit.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkdataaddedit.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkdataaddedit.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkdataaddedit.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtrpbkviewresourcefile.dll",					"temp/phonebooktest/winscw_udeb/mtrpbkviewresourcefile.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkvrsc.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkvrsc.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkidlefinder.dll",							"temp/phonebooktest/winscw_udeb/mtpbkidlefinder.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkifinder.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkifinder.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkiconinfo.dll",							"temp/phonebooktest/winscw_udeb/mtpbkiconinfo.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkinfo.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkinfo.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkiconid.dll",								"temp/phonebooktest/winscw_udeb/mtpbkiconid.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkicon.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkicon.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkfields.dll",								"temp/phonebooktest/winscw_udeb/mtpbkfields.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkfeilds.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkfeilds.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkfielddata.dll",							"temp/phonebooktest/winscw_udeb/mtpbkfielddata.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkfdata.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkfdata.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkcontactiter.dll",						"temp/phonebooktest/winscw_udeb/mtpbkcontactiter.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkiter.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkiter.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkcontactitemfield.dll",					"temp/phonebooktest/winscw_udeb/mtpbkcontactitemfield.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkcif.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpbkcif.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkcontactengine.dll",						"temp/phonebooktest/winscw_udeb/mtpbkcontactengine.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestcengine.cfg",						"temp/phonebooktest/general/testframework/ui_bctestcengine.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkfieldsinfo.dll",							"temp/phonebooktest/winscw_udeb/mtpbkfieldsinfo.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkfinfo.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkfinfo.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkcontactitem.dll",						"temp/phonebooktest/winscw_udeb/mtpbkcontactitem.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkcitem.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkcitem.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkcontactchangenotifier.dll",				"temp/phonebooktest/winscw_udeb/mtpbkcontactchangenotifier.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkpbknotifier.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkpbknotifier.cfg") or die "failed : $!";

copy($epoc . "winscw/c/testframework/testframework_pbkdmngt.ini",				"temp/phonebooktest/general/testframework/testframework_pbkdmngt.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestpbkdatamngt.cfg",					"temp/phonebooktest/general/testframework/tcbctestpbkdatamngt.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbksmsaddressselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbksmsaddressselect.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_smsaddsel.ini",				"temp/phonebooktest/general/testframework/testframework_smsaddsel.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestsmsaddsel.cfg",					"temp/phonebooktest/general/testframework/tcbctestsmsaddsel.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestsmsaddsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestsmsaddsel.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbksingleentryfetchdlg.dll",				"temp/phonebooktest/winscw_udeb/mtpbksingleentryfetchdlg.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_sefielddlg.ini",				"temp/phonebooktest/general/testframework/testframework_sefielddlg.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestsefielddlg.cfg",					"temp/phonebooktest/general/testframework/tcbctestsefielddlg.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestsefielddlg.cfg",					"temp/phonebooktest/general/testframework/ui_bctestsefielddlg.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkpocaddressselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbkpocaddressselect.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_pocadd.ini",					"temp/phonebooktest/general/testframework/testframework_pocadd.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestpocadd.cfg",						"temp/phonebooktest/general/testframework/tcbctestpocadd.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpocadd.cfg",						"temp/phonebooktest/general/testframework/ui_bctestpocadd.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkphonenumberselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbkphonenumberselect.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_phonenumber.ini",			"temp/phonebooktest/general/testframework/testframework_phonenumber.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestphonenumber.cfg",					"temp/phonebooktest/general/testframework/tcbctestphonenumber.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestphonenumber.cfg",					"temp/phonebooktest/general/testframework/ui_bctestphonenumber.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkmultipleentryfetchdlg.dll",				"temp/phonebooktest/winscw_udeb/mtpbkmultipleentryfetchdlg.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_fetchdlg.ini",				"temp/phonebooktest/general/testframework/testframework_fetchdlg.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestfetchdlg.cfg",						"temp/phonebooktest/general/testframework/tcbctestfetchdlg.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestfetchdlg.cfg",					"temp/phonebooktest/general/testframework/ui_bctestfetchdlg.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkmmsaddressselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbkmmsaddressselect.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_mmsaddsel.ini",				"temp/phonebooktest/general/testframework/testframework_mmsaddsel.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestmmsaddsel.cfg",					"temp/phonebooktest/general/testframework/tcbctestmmsaddsel.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestmmsaddsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestmmsaddsel.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkmemoryentryadditemdlg.dll",				"temp/phonebooktest/winscw_udeb/mtpbkmemoryentryadditemdlg.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_pbkadditemdlg.ini",			"temp/phonebooktest/general/testframework/testframework_pbkadditemdlg.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestpbkadditemdlg.cfg",				"temp/phonebooktest/general/testframework/tcbctestpbkadditemdlg.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkadditemdlg.cfg",				"temp/phonebooktest/general/testframework/ui_bctestpbkadditemdlg.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkfetchdlgselection.dll",					"temp/phonebooktest/winscw_udeb/mtpbkfetchdlgselection.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_fetchsel.ini",				"temp/phonebooktest/general/testframework/testframework_fetchsel.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestfetchsel.cfg",						"temp/phonebooktest/general/testframework/tcbctestfetchsel.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestfetchsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestfetchsel.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkfetchcallbacks.dll",						"temp/phonebooktest/winscw_udeb/mtpbkfetchcallbacks.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_fcallback.ini",				"temp/phonebooktest/general/testframework/testframework_fcallback.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestfcallback.cfg",					"temp/phonebooktest/general/testframework/tcbctestfcallback.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestfcallback.cfg",					"temp/phonebooktest/general/testframework/ui_bctestfcallback.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkemailoversms.dll",						"temp/phonebooktest/winscw_udeb/mtpbkemailoversms.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_emailoversms.ini",			"temp/phonebooktest/general/testframework/testframework_emailoversms.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestemaioversms.cfg",					"temp/phonebooktest/general/testframework/tcbctestemaioversms.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestemaioversms.cfg",					"temp/phonebooktest/general/testframework/ui_bctestemaioversms.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkemailaddressselect.dll",					"temp/phonebooktest/winscw_udeb/mtpbkemailaddressselect.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_addemailsel.ini",			"temp/phonebooktest/general/testframework/testframework_addemailsel.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestemailaddsel.cfg",					"temp/phonebooktest/general/testframework/tcbctestemailaddsel.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestemailaddsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestemailaddsel.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkaddressselect.dll",						"temp/phonebooktest/winscw_udeb/mtpbkaddressselect.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_pbkaddsel.ini",				"temp/phonebooktest/general/testframework/testframework_pbkaddsel.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestpbkaddsel.cfg",					"temp/phonebooktest/general/testframework/tcbctestpbkaddsel.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkaddsel.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkaddsel.cfg") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/resource/pbkaddressselect.rsc",				"temp/phonebooktest/general/resource/pbkaddressselect.rsc") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkphonenumberselectbase.dll",				"temp/phonebooktest/winscw_udeb/mtpbkphonenumberselectbase.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_numsel.ini",					"temp/phonebooktest/general/testframework/testframework_numsel.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestnumsel.cfg",						"temp/phonebooktest/general/testframework/tcbctestnumsel.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestnumsel.cfg",						"temp/phonebooktest/general/testframework/ui_bctestnumsel.cfg") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/resource/mt_cpbkphonenumberselectbase.rsc",	"temp/phonebooktest/winscw_udeb/z/resource/mt_cpbkphonenumberselectbase.rsc") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbkselectfielddlg.dll",						"temp/phonebooktest/winscw_udeb/mtpbkselectfielddlg.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_fielddlg.ini",				"temp/phonebooktest/general/testframework/testframework_fielddlg.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestfielddlg.cfg",						"temp/phonebooktest/general/testframework/tcbctestfielddlg.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestfielddlg.cfg",					"temp/phonebooktest/general/testframework/ui_bctestfielddlg.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/mtpbksingleitemfetchdlg.dll",					"temp/phonebooktest/winscw_udeb/mtpbksingleitemfetchdlg.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_sifielddlg.ini",				"temp/phonebooktest/general/testframework/testframework_sifielddlg.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestsifielddlg.cfg",					"temp/phonebooktest/general/testframework/tcbctestsifielddlg.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestsifielddlg.cfg",					"temp/phonebooktest/general/testframework/ui_bctestsifielddlg.cfg") or die "failed : $!";

copy($epoc . "winscw/c/testframework/testframework_pbkfetch.ini",				"temp/phonebooktest/general/testframework/testframework_pbkfetch.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestpbkfetch.cfg",						"temp/phonebooktest/general/testframework/tcbctestpbkfetch.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/bctestpbkuiutil.dll",							"temp/phonebooktest/winscw_udeb/bctestpbkuiutil.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_pbkuiutil.ini",				"temp/phonebooktest/general/testframework/testframework_pbkuiutil.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestpbkuiutil.cfg",					"temp/phonebooktest/general/testframework/tcbctestpbkuiutil.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkuiutil.cfg",					"temp/phonebooktest/general/testframework/ui_bctestpbkuiutil.cfg") or die "failed : $!";

copy($epoc . "release/winscw/udeb/bctestpbkvcardimportexport.dll",				"temp/phonebooktest/winscw_udeb/bctestpbkvcardimportexport.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_pbkvcardimportexport.ini",	"temp/phonebooktest/general/testframework/testframework_pbkvcardimportexport.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcbctestpbkvcardimportexport.cfg",			"temp/phonebooktest/general/testframework/tcbctestpbkvcardimportexport.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_bctestpbkvcardimportexport.cfg",		"temp/phonebooktest/general/testframework/ui_bctestpbkvcardimportexport.cfg") or die "failed : $!";
copy($epoc . "winscw/c/bcardtest2/basic02.src",									"temp/phonebooktest/general/bcardtest2/basic02.src") or die "failed : $!";
copy($epoc . "winscw/c/bcardtest2/basic03.vcf",									"temp/phonebooktest/general/bcardtest2/basic03.vcf") or die "failed : $!";
copy($epoc . "winscw/c/bcardtest2/basiccc01.ccf",								"temp/phonebooktest/general/bcardtest2/basiccc01.ccf") or die "failed : $!";
copy($epoc . "winscw/c/bcardtest2/basiccc02.ccf",								"temp/phonebooktest/general/bcardtest2/basiccc02.ccf") or die "failed : $!";
copy($epoc . "winscw/c/bcardtest2/basiccompactmaxsize.ccf",						"temp/phonebooktest/general/bcardtest2/basiccompactmaxsize.ccf") or die "failed : $!";
copy($epoc . "winscw/c/bcardtest2/basiccompactnobusiness.ccf",					"temp/phonebooktest/general/bcardtest2/basiccompactnobusiness.ccf") or die "failed : $!";
copy($epoc . "winscw/c/bcardtest2/dummy.vcf",									"temp/phonebooktest/general/bcardtest2/dummy.vcf") or die "failed : $!";
copy($epoc . "winscw/c/bcardtest2/johnpic.gif",									"temp/phonebooktest/general/bcardtest2/johnpic.gif") or die "failed : $!";
copy($epoc . "winscw/c/bcardtest2/picture2.vcf",								"temp/phonebooktest/general/bcardtest2/picture2.vcf") or die "failed : $!";

copy($epoc . "release/winscw/udeb/bctestspeeddial.dll",							"temp/phonebooktest/winscw_udeb/bctestspeeddial.dll") or die "failed : $!";
copy($epoc . "winscw/c/testframework/testframework_spdial.ini",					"temp/phonebooktest/general/testframework/testframework_spdial.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/tcspeeddial.cfg",							"temp/phonebooktest/general/testframework/tcspeeddial.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/ui_speeddial.cfg",							"temp/phonebooktest/general/testframework/ui_speeddial.cfg") or die "failed : $!";

system("7z a -tzip phonebooktest.zip ./temp/*");
