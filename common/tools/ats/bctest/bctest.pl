#!/usr/bin/perl

use strict;
use File::Copy;
use File::Path;

unlink "bctest.zip";
rmtree "temp";
mkpath "temp/bctest/general";
mkpath "temp/bctest/winscw_udeb/z/resource/apps";
mkpath "temp/bctest/winscw_udeb/z/private/10003a3f/apps";
mkpath "temp/bctest/BCTestLog/general";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("bctest.xml",																"temp/test.xml");
copy($epoc . "winscw/c/UIFW.bat",												"temp/bctest/general/UIFW.bat");
copy($epoc . "winscw/c/BCTestLog/config.xml",									"temp/bctest/BCTestLog/general/config.xml");
copy($epoc . "winscw/c/BCTestLog/release_note.txt",								"temp/bctest/BCTestLog/general/release_note.txt");

copy($epoc . "data/z/resource/apps/bctestlauncher_aif.mbm",						"temp/bctest/winscw_udeb/z/resource/apps/bctestlauncher_aif.mbm");
copy($epoc . "data/z/resource/apps/bctestlauncher.r01",							"temp/bctest/winscw_udeb/z/resource/apps/bctestlauncher.r01");
copy($epoc . "data/z/private/10003a3f/apps/bctestlauncher_reg.r01",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestlauncher_reg.r01");

copy($epoc . "data/z/resource/apps/bctestbuttons.mbm",							"temp/bctest/winscw_udeb/z/resource/apps/bctestbuttons.mbm");
copy($epoc . "data/z/resource/apps/bctestbuttons.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestbuttons.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestbuttons_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestbuttons_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestsliderandicons.rsc",					"temp/bctest/winscw_udeb/z/resource/apps/bctestsliderandicons.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestsliderandicons_reg.rsc",		"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestsliderandicons_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestaknsound.mbm",							"temp/bctest/winscw_udeb/z/resource/apps/bctestaknsound.mbm");
copy($epoc . "data/z/resource/apps/bctestaknsound_aif.mbm",						"temp/bctest/winscw_udeb/z/resource/apps/bctestaknsound_aif.mbm");
copy($epoc . "data/z/resource/apps/bctestaknsound.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestaknsound.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestaknsound_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestaknsound_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestappfrm.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestappfrm.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestappfrm_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestappfrm_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestcmdlg.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestcmdlg.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestcmdlg_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestcmdlg_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestdocandinit.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestdocandinit.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestdocandinit_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestdocandinit_reg.rsc");

copy($epoc . "data/z/resource/apps/bctesteditor.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctesteditor.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctesteditor_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctesteditor_reg.rsc");

copy($epoc . "data/z/resource/apps/bctesteh.rsc",								"temp/bctest/winscw_udeb/z/resource/apps/bctesteh.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctesteh_reg.rsc",					"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctesteh_reg.rsc");

copy($epoc . "data/z/resource/apps/bctesteikbctrl_aif.mbm",						"temp/bctest/winscw_udeb/z/resource/apps/bctesteikbctrl_aif.mbm");
copy($epoc . "data/z/resource/apps/bctesteikbctrl.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctesteikbctrl.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctesteikbctrl_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctesteikbctrl_reg.rsc");

copy($epoc . "data/z/resource/apps/bctesteikfrm.r01",							"temp/bctest/winscw_udeb/z/resource/apps/bctesteikfrm.r01");
copy($epoc . "data/z/private/10003a3f/apps/bctesteikfrm_reg.r01",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctesteikfrm_reg.r01");

copy($epoc . "data/z/resource/apps/bctestfontinput.mbm",						"temp/bctest/winscw_udeb/z/resource/apps/bctestfontinput.mbm");
copy($epoc . "data/z/resource/apps/bctestfontinput_aif.mbm",					"temp/bctest/winscw_udeb/z/resource/apps/bctestfontinput_aif.mbm");
copy($epoc . "data/z/resource/apps/bctestfontinput.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestfontinput.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestfontinput_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestfontinput_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestform.rsc",								"temp/bctest/winscw_udeb/z/resource/apps/bctestform.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestform_reg.rsc",					"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestform_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestgrids.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestgrids.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestgrids_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestgrids_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestimage_aif.mbm",						"temp/bctest/winscw_udeb/z/resource/apps/bctestimage_aif.mbm");
copy($epoc . "data/z/resource/apps/bctestimage.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestimage.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestimage_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestimage_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestkeylock.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestkeylock.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestkeylock_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestkeylock_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestlabelinfoindicators.rsc",				"temp/bctest/winscw_udeb/z/resource/apps/bctestlabelinfoindicators.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestlabelinfoindicators_reg.rsc",	"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestlabelinfoindicators_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestlist_aif.mbm",							"temp/bctest/winscw_udeb/z/resource/apps/bctestlist_aif.mbm");
copy($epoc . "data/z/resource/apps/bctestlist.rsc",								"temp/bctest/winscw_udeb/z/resource/apps/bctestlist.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestlist_reg.rsc",					"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestlist_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestmenu.rsc",								"temp/bctest/winscw_udeb/z/resource/apps/bctestmenu.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestmenu_reg.rsc",					"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestmenu_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestnote.mbm",								"temp/bctest/winscw_udeb/z/resource/apps/bctestnote.mbm");
copy($epoc . "data/z/resource/apps/bctestnote.rsc",								"temp/bctest/winscw_udeb/z/resource/apps/bctestnote.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestnote_reg.rsc",					"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestnote_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestnotifier.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestnotifier.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestnotifier_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestnotifier_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestpane.rsc",								"temp/bctest/winscw_udeb/z/resource/apps/bctestpane.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestpane_reg.rsc",					"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestpane_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestpopups.mbm",							"temp/bctest/winscw_udeb/z/resource/apps/bctestpopups.mbm");
copy($epoc . "data/z/resource/apps/bctestpopups.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestpopups.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestpopups_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestpopups_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestprivateuifrm.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestprivateuifrm.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestprivateuifrm_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestprivateuifrm_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestptieng.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestptieng.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestptieng_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestptieng_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestquery.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestquery.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestquery_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestquery_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestscreenclearer.mbm",					"temp/bctest/winscw_udeb/z/resource/apps/bctestscreenclearer.mbm");
copy($epoc . "data/z/resource/apps/bctestscreenclearer_aif.mbm",				"temp/bctest/winscw_udeb/z/resource/apps/bctestscreenclearer_aif.mbm");
copy($epoc . "data/z/resource/apps/bctestscreenclearer.rsc",					"temp/bctest/winscw_udeb/z/resource/apps/bctestscreenclearer.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestscreenclearer_reg.rsc",		"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestscreenclearer_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestscroller.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestscroller.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestscroller_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestscroller_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestsearchfield.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestsearchfield.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestsearchfield_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestsearchfield_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestserverapp.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestserverapp.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestserverapp_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestserverapp_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestsettingpage.mbm",						"temp/bctest/winscw_udeb/z/resource/apps/bctestsettingpage.mbm");
copy($epoc . "data/z/resource/apps/bctestsettingpage_aif.mbm",					"temp/bctest/winscw_udeb/z/resource/apps/bctestsettingpage_aif.mbm");
copy($epoc . "data/z/resource/apps/bctestsettingpage.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestsettingpage.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestsettingpage_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestsettingpage_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestskins.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestskins.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestskins_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestskins_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestutilities.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestutilities.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestutilities_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestutilities_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestvolume.mbm",							"temp/bctest/winscw_udeb/z/resource/apps/bctestvolume.mbm");
copy($epoc . "data/z/resource/apps/bctestvolume.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestvolume.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestvolume_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestvolume_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestocr.rsc",								"temp/bctest/winscw_udeb/z/resource/apps/bctestocr.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestocr_reg.rsc",					"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestocr_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestmisc.rsc",								"temp/bctest/winscw_udeb/z/resource/apps/bctestmisc.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestmisc_reg.rsc",					"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestmisc_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestlocation.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestlocation.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestlocation_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestlocation_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestuniteditor.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestuniteditor.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestuniteditor_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestuniteditor_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestmisc32.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestmisc32.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestmisc32_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestmisc32_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestlocalizer.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestlocalizer.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestlocalizer_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestlocalizer_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestpreviewpopup.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestpreviewpopup.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestpreviewpopup_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestpreviewpopup_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestservicehandler.rsc",					"temp/bctest/winscw_udeb/z/resource/apps/bctestservicehandler.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestservicehandler_reg.rsc",		"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestservicehandler_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestbutton.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestbutton.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestbutton_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestbutton_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestchoicelist.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestchoicelist.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestchoicelist_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestchoicelist_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestmix50.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctestmix50.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestmix50_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestmix50_reg.rsc");

copy($epoc . "data/z/resource/apps/bctesttoolbar.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctesttoolbar.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctesttoolbar_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctesttoolbar_reg.rsc");

copy($epoc . "data/z/resource/apps/bctesttouchpane.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctesttouchpane.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctesttouchpane_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctesttouchpane_reg.rsc");

copy($epoc . "data/z/resource/apps/bctesttreelist.rsc",							"temp/bctest/winscw_udeb/z/resource/apps/bctesttreelist.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctesttreelist_reg.rsc",				"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctesttreelist_reg.rsc");

copy($epoc . "data/z/resource/apps/bctestmisccontrol.mbm",						"temp/bctest/winscw_udeb/z/resource/apps/bctestmisccontrol.mbm");
copy($epoc . "data/z/resource/apps/bctestmisccontrol.rsc",						"temp/bctest/winscw_udeb/z/resource/apps/bctestmisccontrol.rsc");
copy($epoc . "data/z/private/10003a3f/apps/bctestmisccontrol_reg.rsc",			"temp/bctest/winscw_udeb/z/private/10003a3f/apps/bctestmisccontrol_reg.rsc");

copy($epoc . "release/winscw/udeb/bctestutil.dll",								"temp/bctest/winscw_udeb/bctestutil.dll");
copy($epoc . "release/winscw/udeb/bctestlauncher.exe",							"temp/bctest/winscw_udeb/bctestlauncher.exe");
copy($epoc . "release/winscw/udeb/bctestaknsound.exe",							"temp/bctest/winscw_udeb/bctestaknsound.exe");
copy($epoc . "release/winscw/udeb/bctestappfrm.exe",							"temp/bctest/winscw_udeb/bctestappfrm.exe");
copy($epoc . "release/winscw/udeb/bctestbutton.exe",							"temp/bctest/winscw_udeb/bctestbutton.exe");
copy($epoc . "release/winscw/udeb/bctestbuttons.exe",							"temp/bctest/winscw_udeb/bctestbuttons.exe");
copy($epoc . "release/winscw/udeb/bctestchoicelist.exe",						"temp/bctest/winscw_udeb/bctestchoicelist.exe");
copy($epoc . "release/winscw/udeb/bctestcmdlg.exe",								"temp/bctest/winscw_udeb/bctestcmdlg.exe");
copy($epoc . "release/winscw/udeb/bctestdocandinit.exe",						"temp/bctest/winscw_udeb/bctestdocandinit.exe");
copy($epoc . "release/winscw/udeb/bctesteditor.exe",							"temp/bctest/winscw_udeb/bctesteditor.exe");
copy($epoc . "release/winscw/udeb/bctesteh.exe",								"temp/bctest/winscw_udeb/bctesteh.exe");
copy($epoc . "release/winscw/udeb/bctesteikbctrl.exe",							"temp/bctest/winscw_udeb/bctesteikbctrl.exe");
copy($epoc . "release/winscw/udeb/bctesteikfrm.exe",							"temp/bctest/winscw_udeb/bctesteikfrm.exe");
copy($epoc . "release/winscw/udeb/bctestfontinput.exe",							"temp/bctest/winscw_udeb/bctestfontinput.exe");
copy($epoc . "release/winscw/udeb/bctestform.exe",								"temp/bctest/winscw_udeb/bctestform.exe");
copy($epoc . "release/winscw/udeb/bctestgrids.exe",								"temp/bctest/winscw_udeb/bctestgrids.exe");
copy($epoc . "release/winscw/udeb/bctestimage.exe",								"temp/bctest/winscw_udeb/bctestimage.exe");
copy($epoc . "release/winscw/udeb/bctestkeylock.exe",							"temp/bctest/winscw_udeb/bctestkeylock.exe");
copy($epoc . "release/winscw/udeb/bctestlabelinfoindicators.exe",				"temp/bctest/winscw_udeb/bctestlabelinfoindicators.exe");
copy($epoc . "release/winscw/udeb/bctestlist.exe",								"temp/bctest/winscw_udeb/bctestlist.exe");
copy($epoc . "release/winscw/udeb/bctestlocalizer.exe",							"temp/bctest/winscw_udeb/bctestlocalizer.exe");
copy($epoc . "release/winscw/udeb/bctestlocation.exe",							"temp/bctest/winscw_udeb/bctestlocation.exe");
copy($epoc . "release/winscw/udeb/bctestmenu.exe",								"temp/bctest/winscw_udeb/bctestmenu.exe");
copy($epoc . "release/winscw/udeb/bctestmisc.exe",								"temp/bctest/winscw_udeb/bctestmisc.exe");
copy($epoc . "release/winscw/udeb/bctestmisc32.exe",							"temp/bctest/winscw_udeb/bctestmisc32.exe");
copy($epoc . "release/winscw/udeb/bctestmisccontrol.exe",						"temp/bctest/winscw_udeb/bctestmisccontrol.exe");
copy($epoc . "release/winscw/udeb/bctestmix50.exe",								"temp/bctest/winscw_udeb/bctestmix50.exe");
copy($epoc . "release/winscw/udeb/bctestnote.exe",								"temp/bctest/winscw_udeb/bctestnote.exe");
copy($epoc . "release/winscw/udeb/bctestnotifier.exe",							"temp/bctest/winscw_udeb/bctestnotifier.exe");
copy($epoc . "release/winscw/udeb/bctestocr.exe",								"temp/bctest/winscw_udeb/bctestocr.exe");
copy($epoc . "release/winscw/udeb/bctestpane.exe",								"temp/bctest/winscw_udeb/bctestpane.exe");
copy($epoc . "release/winscw/udeb/bctestpopups.exe",							"temp/bctest/winscw_udeb/bctestpopups.exe");
copy($epoc . "release/winscw/udeb/bctestpreviewpopup.exe",						"temp/bctest/winscw_udeb/bctestpreviewpopup.exe");
copy($epoc . "release/winscw/udeb/bctestprivateuifrm.exe",						"temp/bctest/winscw_udeb/bctestprivateuifrm.exe");
copy($epoc . "release/winscw/udeb/bctestptieng.exe",							"temp/bctest/winscw_udeb/bctestptieng.exe");
copy($epoc . "release/winscw/udeb/bctestquery.exe",								"temp/bctest/winscw_udeb/bctestquery.exe");
copy($epoc . "release/winscw/udeb/bctestscreenclearer.exe",						"temp/bctest/winscw_udeb/bctestscreenclearer.exe");
copy($epoc . "release/winscw/udeb/bctestscroller.exe",							"temp/bctest/winscw_udeb/bctestscroller.exe");
copy($epoc . "release/winscw/udeb/bctestsearchfield.exe",						"temp/bctest/winscw_udeb/bctestsearchfield.exe");
copy($epoc . "release/winscw/udeb/bctestserverapp.exe",							"temp/bctest/winscw_udeb/bctestserverapp.exe");
copy($epoc . "release/winscw/udeb/bctestservicehandler.exe",					"temp/bctest/winscw_udeb/bctestservicehandler.exe");
copy($epoc . "release/winscw/udeb/bctestsettingpage.exe",						"temp/bctest/winscw_udeb/bctestsettingpage.exe");
copy($epoc . "release/winscw/udeb/bctestskins.exe",								"temp/bctest/winscw_udeb/bctestskins.exe");
copy($epoc . "release/winscw/udeb/bctestsliderandicons.exe",					"temp/bctest/winscw_udeb/bctestsliderandicons.exe");
copy($epoc . "release/winscw/udeb/bctesttoolbar.exe",							"temp/bctest/winscw_udeb/bctesttoolbar.exe");
copy($epoc . "release/winscw/udeb/bctesttouchpane.exe",							"temp/bctest/winscw_udeb/bctesttouchpane.exe");
copy($epoc . "release/winscw/udeb/bctesttreelist.exe",							"temp/bctest/winscw_udeb/bctesttreelist.exe");
copy($epoc . "release/winscw/udeb/bctestuniteditor.exe",						"temp/bctest/winscw_udeb/bctestuniteditor.exe");
copy($epoc . "release/winscw/udeb/bctestutilities.exe",							"temp/bctest/winscw_udeb/bctestutilities.exe");
copy($epoc . "release/winscw/udeb/bctestvolume.exe",							"temp/bctest/winscw_udeb/bctestvolume.exe");

system("7z a -tzip bctest.zip ./temp/*");
