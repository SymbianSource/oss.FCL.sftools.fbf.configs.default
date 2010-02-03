// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Tests to meet REQ758.2: "Captions localization must be efficient
// (i.e. not waste disk space) and extensible (i.e. support non-ROM language packs)"\n
// Test for UIKON GT0143 Typhoon Work Series 60 Changes
// REQ758.2: Captions localization must be efficient (i.e. not waste disk space)
// and extensible (i.e. support non-ROM language packs)
// Test for CR0902 - Enable Dynamic Language Switching in APPARC.
// Application's localisable information should be updated by apparc on language change event.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#include <e32test.h>
#include <f32file.h>
#include <apgicnfl.h>
#include <hal.h>
#include <apgcli.h>
#include "T_LocaleStep.h"

const TUid KUidTestApp = { 10 }; //uid of tstapp.
const TUid KUidCustomiseDefaultIconApp = {0x10208181}; // uid of CustomiseDefaultIconApp.
const TInt KDelayForOnDemand = 20000; //a small delay
const TInt KDelay = 4000000; // Most apparc tests have 2.5 secs wait time to let apparc update the app-list, but on safer side let us give 4 secs.
const TInt KViewCount = 3; // Total no of views in tstapp


/**
  Auxiliary Fn for Test Case ID T-LocaleStep-TestAllLanguages
 
  This method loads the required DLL for the language specified as argument
  and changes the locale accordingly.
 
*/
void CT_LocaleStep::ChangeLocaleL(TLanguage aLanguage)
	{
	_LIT(KLitLocaleDllNameBase, "ELOCL");
	_LIT(KLitLocaleDllNameExtension, ".LOC");
	RLibrary localeDll;
	TBuf<16> localeDllName(KLitLocaleDllNameBase);
	CleanupClosePushL(localeDll);
	const TUidType uidType(TUid::Uid(0x10000079),TUid::Uid(0x100039e6));
	_LIT(TwoDigExt,".%02d");
	localeDllName.AppendFormat(TwoDigExt, aLanguage);
	TInt error=localeDll.Load(localeDllName, uidType);
	if (error==KErrNotFound)
		{
		localeDllName=KLitLocaleDllNameBase;
		localeDllName.Append(KLitLocaleDllNameExtension);
		error=localeDll.Load(localeDllName, uidType);
		}
	User::LeaveIfError(error);
	User::LeaveIfError(UserSvr::ChangeLocale(localeDllName));
	CleanupStack::PopAndDestroy(); // localeDll
	}

// CheckIcons is a function used in testcase TestLocaleDefaultIconL to check the size of the default icons
void CT_LocaleStep::CheckIcons(CArrayFixFlat<TSize>* aNewIconSizes,TSize aSmall,TSize aMedium,TSize aLarge, TSize& afstIcon, TSize& asecIcon, TSize& atrdIcon)
	{
	afstIcon = (*aNewIconSizes)[0];
	asecIcon = (*aNewIconSizes)[1];
	atrdIcon = (*aNewIconSizes)[2];
	TEST((afstIcon==aSmall) || (afstIcon==aMedium) || (afstIcon==aLarge) && ((afstIcon!=asecIcon) && (afstIcon!=atrdIcon)));
	TEST((asecIcon==aSmall) || (asecIcon==aMedium) || (asecIcon==aLarge) && ((asecIcon!=afstIcon) && (asecIcon!=atrdIcon)));
	TEST((atrdIcon==aSmall) || (atrdIcon==aMedium) || (atrdIcon==aLarge) && ((atrdIcon!=afstIcon) && (atrdIcon!=asecIcon)));	
	}

/**
  @SYMTestCaseID APPFWK-APPARC-0070
 
  @SYMPREQ
 
  @SYMTestCaseDesc Test whether the default app icons change with respect to the locale/language.
  This testcase checks whether BaflUtils::NearestLanguageFile is called when the locale has been changed.
 
  @SYMTestPriority High  
 
  @SYMTestStatus Implemented

  @SYMTestActions  Call User::Language() to check that the default locale is English; Call CT_LocaleStep::CheckIcons 
  to check the size of the current default icons; Call UserSvr::ChangeLocale to change the Locale to French; 
  Call CT_LocaleStep::CheckIcons to check the size of the new icons; Restore the file system and the locale 
  and check if the icons are restored.
  API Calls:\n	
  
  @SYMTestExpectedResults Returns KErrNone
 */	
void CT_LocaleStep::TestLocaleDefaultIconL()
	{
	INFO_PRINTF1(_L("APPFWK-APPARC-0070: TestLocaleDefaultIconL started..."));	
	
	TInt ret = 0;
		
	//first checks that language is not currently set to French (any other language except English since English is the default language
	TEST(User::Language()!=ELangFrench);

	// getappiconsizes must be called to check if the icons are the default ones
	// KUidCustomiseDefaultIconApp - uid of CustomiseDefaultIconApp_reg.RSS; this app has a localisable resource file, CustomiseDefaultIconApp_loc.RSS, that does not define any icon
	// Hence, the default icons must be picked up when the language changes to French. The default icons for French are defined in default_app_icon.m02

	CArrayFixFlat<TSize>* newIconSizes = new(ELeave) CArrayFixFlat<TSize>(3);
	CleanupStack::PushL(newIconSizes);
	TRAP(ret,iLs.GetAppIconSizes(KUidCustomiseDefaultIconApp, *newIconSizes));
	TEST(ret==KErrNone);
	TEST(newIconSizes->Count()!=0);
	// these are the sizes of the default icons
	TSize small(24,24);
	TSize medium(32,32);
	TSize large(48,48);
	TSize fstIcon;
	TSize secIcon;
	TSize trdIcon;
	CheckIcons(newIconSizes, small, medium,large, fstIcon, secIcon, trdIcon);
	
	// Change the locale to French
	TRAP(ret,ChangeLocaleL(ELangFrench));
	TEST(ret==KErrNone);
	TEST(User::Language()==ELangFrench);//checks that the language has been set to French

	//Wait for a small period to let apparc receive language change notification
	User::After(KDelayForOnDemand);
	//call getappiconsizes to see if the new icons have been changed as as expected 
	// The size of the icons indicates whether the icon corresponding to French Locale has been picked up
	newIconSizes->Reset();
	TRAP(ret,iLs.GetAppIconSizes(KUidCustomiseDefaultIconApp, *newIconSizes));
	TEST(ret==KErrNone);
	TEST(newIconSizes->Count()!=0);
	small.SetSize(25,25);
	medium.SetSize(35,35);
	large.SetSize(50,50);
	CheckIcons(newIconSizes, small, medium,large, fstIcon, secIcon, trdIcon);

	//have to restore the locale before exiting the testcase
	TRAP(ret,ChangeLocaleL(ELangEnglish));
	TEST(ret==KErrNone);
	TEST(User::Language()==ELangEnglish);

	//Again wait for a small period to let apparc receive language change notification
	User::After(KDelayForOnDemand);

	//call getappiconsizes to see if the new icons have been changed as as expected 
	// The size of the icons indicates whether the icon corresponding to English Locale has been picked up
	newIconSizes->Reset();
	TRAP(ret,iLs.GetAppIconSizes(KUidCustomiseDefaultIconApp, *newIconSizes));
	TEST(ret==KErrNone);
	TEST(newIconSizes->Count()!=0);
	small.SetSize(24,24);
	medium.SetSize(32,32);
	large.SetSize(48,48);
	CheckIcons(newIconSizes, small, medium,large, fstIcon, secIcon, trdIcon);

	// Icon cleanup
	CleanupStack::PopAndDestroy(newIconSizes);
	newIconSizes=NULL;
	INFO_PRINTF1(_L("APPFWK-APPARC-0070: TestLocaleDefaultIconL finished..."));	
	}
	
/**
  @SYMTestCaseID APPFWK-APPARC-0061
 
  @SYMPREQ CR0902 - Enable Dynamic Language Switching in APPARC.
 
  @SYMTestCaseDesc Test whether caption, short caption and app icon, is updated with respect to the change 
  in locale/language. This test case checks on language change event apparc updates application 
  localisable information for all installed applications, without forcing a change in the part of 
  file system which apparc monitors. 
  This test also tests, on a language change event, application's localisable information is 
  updated immediately, if not yet updated by the re-scan (On demand loading). This is done by not 
  giving enough time to finish rescan of app-list.
 
  @SYMTestPriority Critical 
 
  @SYMTestStatus Implemented

  @SYMTestActions  Check if language is English by calling User::Language(). 
  
  1. Call GetAppInfo() by passing an application's UID which provides support for multiple languages.
  Check if the caption and short caption, both are in English. Change locale to French.
  2. Call GetAppIcon() by passing same application's UID. Check app icons are same as sepcified in 
  app icon file for English locale.
  
  Wait for a small period (20 milli seconds) to let apparc receive language change notification. 
  Again - 
  1. Call GetAppInfo() by passing same application's UID and check if the caption and short caption, 
  both are in French.
  2. Call GetAppIcon() by passing same application's UID. Check app icons are same as sepcified in 
  app icon file for French locale.
  
  Restore locale to English and again check - 
  1. If caption and short caption are in English after a call to GetAppInfo() for the same application.
  2. Call GetAppIcon() by passing same application's UID. Check app icons are same as sepcified in 
  app icon file for English locale.
  
  @SYMTestExpectedResults 
  1. GetAppInfo() should fetch caption and short caption in the newly-set language.
  2. GetAppIcon() should fetch icons from the icon file selected for current locale.
 */	
void CT_LocaleStep::TestLocaleApplicationInfoOnDemandL()
	{
	INFO_PRINTF1(_L("APPFWK-APPARC-0061: TestLocaleApplicationInfoOnDemandL started..."));
	TestAppInfoL(ETrue);
	INFO_PRINTF1(_L("APPFWK-APPARC-0061: TestLocaleApplicationInfoOnDemandL finished..."));	
	}

/**
  @SYMTestCaseID APPFWK-APPARC-0062
 
  @SYMPREQ CR0902 - Enable Dynamic Language Switching in APPARC.
 
  @SYMTestCaseDesc Test whether caption, short caption and app icon, is updated with respect to the change in 
  locale/language. This test case checks on language change event apparc updates application localisable
  information for all installed applications, without forcing a change in the part of file system which 
  apparc monitors.
 
  @SYMTestPriority Critical
 
  @SYMTestStatus Implemented

  @SYMTestActions  Check if language is English by calling User::Language(). 
  
  1. Call GetAppInfo() by passing an application's UID which provides support for multiple languages.
  Check if the caption and short caption, both are in English. Change locale to French.
  2. Call GetAppIcon by passing same application's UID. Check app icons are same as sepcified in 
  app icon file for English locale.
  
  Change locale to French. Wait for some time (e.g. 4 seconds) to let apparc complete rescanning of app-list.
  Again - 
  1. Call GetAppInfo() by passing same application's UID and check if the caption and short caption, 
  both are in French.
  2. Call GetAppIcon() by passing same application's UID. Check app icons are same as sepcified in 
  app icon file for French locale.
  
  Restore locale to English and again check - 
  1. If caption and short caption are in English after a call to GetAppInfo() for the same application.
  2. Call GetAppIcon() by passing same application's UID. Check app icons are same as sepcified in 
  app icon file for English locale.
  
  @SYMTestExpectedResults 
  1. GetAppInfo() should fetch caption and short caption in the newly-set language.
  2. GetAppIcon() should fetch icons from the icon file selected for current locale.  
 */	
void CT_LocaleStep::TestLocaleApplicationInfoL()
	{
	INFO_PRINTF1(_L("APPFWK-APPARC-0062: TestLocaleApplicationInfoL started..."));	
	TestAppInfoL(EFalse);	
	INFO_PRINTF1(_L("APPFWK-APPARC-0062: TestLocaleApplicationInfoL finished..."));	
	}
			
/** 
  This function performs steps to test application related information (caption, short caption and app icon)
  on language change event . 
  It is called from TestLocaleApplicationInfoOnDemandL and TestLocaleApplicationInfoL.
    
  @param aOnDemand Pass true if on demand loading is required, otherwise false.
*/
void CT_LocaleStep::TestAppInfoL(TBool aOnDemand)
	{
	TInt ret = RFbsSession::Connect();
	TEST(ret == KErrNone);
	
	// If it is on-demand loading delay would be very less.
	TInt delay = aOnDemand ? KDelayForOnDemand : KDelay;
	
	// Check language is not currently set to French (any other language except English since English is the default language.
	TEST(User::Language() != ELangFrench);
	
	// Caption and Short-caption should be same as defined in English localisable strings file (tstapp01.rls)
	_LIT(KCaptionEng, "TstCap UK"); 
	_LIT(KShortCaptionEng, "TC UK");
	
	// Call GetAppInfo() by passing KUidTestApp as tstapp's UID. tstapp provides support for multiple languages.
	TApaAppInfo appInfo;
	ret = iLs.GetAppInfo(appInfo, KUidTestApp);
	TEST(ret == KErrNone);
	
	// Caption and short-caption should be in English.
	TEST(appInfo.iCaption == KCaptionEng);
	TEST(appInfo.iShortCaption == KShortCaptionEng);
	
	// Test application's icons, should be same as sepcified in app icon file for English locale.
	TestAppIconL(TSize(40,55));
		
	// Change the locale to French
	TRAP(ret, ChangeLocaleL(ELangFrench));
	TEST(ret == KErrNone);
	TEST(User::Language() == ELangFrench);//check language is set to French

	// Caption and Short-caption should be same as defined in French localisable strings file (tstapp02.rls)
	_LIT(KCaptionFrench, "TstCap FR");
	_LIT(KShortCaptionFrench, "TC FR");
	
	User::After(delay);
	
	// Again call GetAppInfo() by passing KUidTestApp as UID. 	
	ret = iLs.GetAppInfo(appInfo, KUidTestApp);
	TEST(ret == KErrNone);
	
	// Caption and short-caption should be in French.
	TEST(appInfo.iCaption == KCaptionFrench);
	TEST(appInfo.iShortCaption == KShortCaptionFrench);
	
	// Test application's icons, should be same as sepcified in app icon file for French locale.
	TestAppIconL(TSize(50,50));

	// Restore locale before exiting the testcase.
	TRAP(ret, ChangeLocaleL(ELangEnglish));
	TEST(ret == KErrNone);
	TEST(User::Language() == ELangEnglish);
	
	User::After(delay);
	
	ret = iLs.GetAppInfo(appInfo, KUidTestApp);
	TEST(ret == KErrNone);
	
	// Caption and short-caption should again be in English.
	TEST(appInfo.iCaption == KCaptionEng);
	TEST(appInfo.iShortCaption == KShortCaptionEng);
	
	// Test application's icons, should be same as sepcified in app icon file for English locale.
	TestAppIconL(TSize(40,55));
	
	RFbsSession::Disconnect();
	}
/**
  This function tests application's icon. It is called from TestAppInfoL function.
*/	
void CT_LocaleStep::TestAppIconL(TSize aSize)
	{
	CApaMaskedBitmap* icon = CApaMaskedBitmap::NewLC();
	TSize lastSize(0,0);

	TInt ret = iLs.GetAppIcon(KUidTestApp, aSize, *icon);
	TEST(ret == KErrNone);
	// Returned icon size should be exactly same as asked.
	TEST(icon->SizeInPixels().iWidth * icon->SizeInPixels().iHeight == aSize.iWidth * aSize.iHeight);
	
	CleanupStack::PopAndDestroy(icon);
	}

/**
  @SYMTestCaseID APPFWK-APPARC-0063
 
  @SYMPREQ CR0902 - Enable Dynamic Language Switching in APPARC.
 
  @SYMTestCaseDesc Test whether caption and short caption remains same on language change, if application
   does not provide support for multiple languages. 
 
  @SYMTestPriority Critical 
 
  @SYMTestStatus Implemented

  @SYMTestActions  Check if language is English by calling User::Language(). Call GetAppInfo() by passing an
  application's UID which does not provide support for multiple languages. Check if the caption and short 
  caption, both are in English. Change locale to French.Wait for a small period (20 milli seconds) to let 
  apparc receive language change notification. Again Call GetAppInfo() by passing same application's UID and 
  check if the caption and short caption, both are still in English. Restore locale to English and again check 
  if caption and short caption are in English after a call to GetAppInfo() for the same application.
  
  @SYMTestExpectedResults GetAppInfo() should fetch caption and short caption in the same language. 
 */	
void CT_LocaleStep::TestLocaleApplicationInfo1L()
	{
	INFO_PRINTF1(_L("APPFWK-APPARC-0063: TestLocaleApplicationInfo1L started..."));	
	TInt ret = KErrNotFound;	
		
	// Check language is not currently set to French (any other language except English since English is the default language.
	TEST(User::Language() != ELangFrench);
	
	// Caption and Short-caption should be same as defined in the localisable resource file. (CustomiseDefaultIconApp_loc.RSS)
	_LIT(KCaptionEng, "CustomiseDefaultIconAppLong"); 
	_LIT(KShortCaptionEng, "CustomiseDefaultIconApp");
	
	// KUidCustomiseDefaultIconApp - uid of CustomiseDefaultIconApp_reg.RSS; this app does not provide support for multiple languages.
		
	// Call GetAppInfo() by passing above UID.
	TApaAppInfo appInfo;
	ret = iLs.GetAppInfo(appInfo, KUidCustomiseDefaultIconApp);
	TEST(ret == KErrNone);
	
	// Caption and short-caption should be in English.	
	TEST(appInfo.iCaption == KCaptionEng);
	TEST(appInfo.iShortCaption == KShortCaptionEng);
		
	// Change the locale to French
	TRAP(ret,ChangeLocaleL(ELangFrench));
	TEST(ret == KErrNone);
	TEST(User::Language() == ELangFrench);//check language is set to French.

	// Wait for a small period (20 milli seconds) to let apparc receive language change notification. 
	User::After(KDelayForOnDemand);	
	
	// Again call GetAppInfo() by passing KUidTestApp as UID. 	
	ret = iLs.GetAppInfo(appInfo, KUidCustomiseDefaultIconApp);
	TEST(ret == KErrNone);
	
	// Caption and short-caption should be in English.
	TEST(appInfo.iCaption == KCaptionEng);
	TEST(appInfo.iShortCaption == KShortCaptionEng);	

	// Restore locale before exiting the testcase.
	TRAP(ret,ChangeLocaleL(ELangEnglish));
	TEST(ret == KErrNone);
	TEST(User::Language() == ELangEnglish);
	
	User::After(KDelayForOnDemand);
	ret = iLs.GetAppInfo(appInfo, KUidCustomiseDefaultIconApp);
	TEST(ret == KErrNone);
	
	// Caption and short-caption should be in English again.
	TEST(appInfo.iCaption == KCaptionEng);
	TEST(appInfo.iShortCaption == KShortCaptionEng);
	
	INFO_PRINTF1(_L("APPFWK-APPARC-0063: TestLocaleApplicationInfo1L finished..."));	
	}

/**
  @SYMTestCaseID APPFWK-APPARC-0064 APPFWK-APPARC-0066 APPFWK-APPARC-0068
 
  @SYMPREQ CR0902 - Enable Dynamic Language Switching in APPARC.
 
  @SYMTestCaseDesc Test whether 
  1. View's caption is updated with respect to the change in locale/language. 
  2. View's icons is updated with respect to the change in locale/language. 
  3. View's icon file name changes with respect to the change in locale/language. 
  
  This test case checks apparc updates application localisable information for all installed applications 
  on language change event without forcing a change in the part of file system which apparc monitors.
  This test also tests, on a language change event, application's localisable information is updated 
  immediately, if not yet updated by the re-scan (On demand loading). This is done by not giving enough 
  time to finish rescan of app-list.
 
  @SYMTestPriority High 
 
  @SYMTestStatus Implemented

  @SYMTestActions  Check if language is English by calling User::Language(). Call GetAppViews() by passing 
  an application's UID which provides support for multiple languages and different size icons for French 
  language. 
  
  1. Check if all view captions are same as specified in localisable file and in English.  
  2. Call GetAppViewIcon() passing above application's ID. Check view icon size, it should be as 
  specified in English icon file.
  3. Call overload of GetAppViewIcon() which returns view's icon file, passing above application's ID. 
  
  Change locale to French.Wait for a small period (20 milli seconds) to let apparc receive language change 
  notification. Again - 
  
  1. Call GetAppViews() by passing same application's UID and check if the view captions for all views 
  are in French.
  2. Call GetAppViewIcon() by passing same application's UID.Check view icon size, it should be as specified 
  in French icon file.
  3. Call GetAppViewIcon() by passing same application's UID. Check view icon file, it should be same as 
  specified in French locale.
  
  Restore locale to English and again check 
  1. if view captions are same as specified in English resource file.
  2. if view icon size is same as specified in English icon file.
  3. if view icon file is same as specified in English resource file.
  
  @SYMTestExpectedResults Following are the expected results.
  1. GetAppViews() should fetch captions for all views in the newly-set language.
  2. GetAppViewIcon() should fetch icons specified in the newly-set language.
  3. GetAppViewIcon() should fetch view's icon file specified in the newly-set language.
 */	
void CT_LocaleStep::TestLocaleViewCaptionAndIconOnDemandL()
	{
	INFO_PRINTF1(_L("APPFWK-APPARC-0064: TestLocaleViewCaptionOnDemandL started..."));		
	INFO_PRINTF1(_L("APPFWK-APPARC-0066: TestLocaleViewIconSizeOnDemandL started..."));
	INFO_PRINTF1(_L("APPFWK-APPARC-0068: TestLocaleViewIconFileOnDemandL started..."));
	TestViewCaptionAndIconL(ETrue);
	INFO_PRINTF1(_L("APPFWK-APPARC-0064: TestLocaleViewCaptionOnDemandL finished..."));
	INFO_PRINTF1(_L("APPFWK-APPARC-0066: TestLocaleViewIconSizeOnDemandL finished..."));
	INFO_PRINTF1(_L("APPFWK-APPARC-0068: TestLocaleViewIconFileOnDemandL finished..."));	
	}
	
/**
  @SYMTestCaseID APPFWK-APPARC-0065 APPFWK-APPARC-0067 APPFWK-APPARC-0069
 
  @SYMPREQ CR0902 - Enable Dynamic Language Switching in APPARC.
  @SYMDEF PDEF113054 - Apparc fails to detect the presence of localized Mbm files
 
  @SYMTestCaseDesc Test whether 
  1. View's caption changes with respect to the change in locale/language.
  2. View's icons is updated with respect to the change in locale/language. 
  3. View's icon file name changes with respect to the change in locale/language. 
  
  This test case checks apparc updates application localisable information for all installed applications 
  on language change event without forcing a change in the part of file system which apparc monitors.
  
  It checks for localized MBM files and MBM icon files with generic extension for an application. 
  @SYMTestPriority High 
 
  @SYMTestStatus Implemented

  @SYMTestActions  Check if language is English by calling User::Language(). Call GetAppViews() by passing 
  an application's UID which provides support for multiple languages and different size icons for French 
  language. 
  1. Check if all view captions are same as specified in localisable file and in English.
  2. Call GetAppViewIcon() passing above application's ID. Check view icon size, it should be as 
  specified in English icon file.
  3. Call overload of GetAppViewIcon() which returns view's icon file, passing above application's ID. 
  
  Change locale to French. Wait for some time (e.g. 4 seconds) to let apparc complete rescanning of app-list.
  Again - 
  1. Call GetAppViews() by passing same application's UID and check if the view captions for all views 
  are in French.
  2. Call GetAppViewIcon() by passing same application's UID.Check view icon size, it should be as specified 
  in French icon file.
  3. Call GetAppViewIcon() by passing same application's UID. Check view icon file, it should be same as 
  specified in French locale.
  
  Restore locale to English and again check - 
  1. if view captions are same as specified in English resource file.
  2. if view icon size is same as specified in English icon file.
  3. if view icon file is same as specified in English resource file.
  
  @SYMTestExpectedResults Following are the expected results.
  1. GetAppViews() should fetch captions for all views in the newly-set language.
  2. GetAppViewIcon() should fetch icons specified in the newly-set language. 
  3. GetAppViewIcon() should fetch view's icon file specified in the newly-set language.  		 
 */	
 void CT_LocaleStep::TestLocaleViewCaptionAndIconL()
	{
	INFO_PRINTF1(_L("APPFWK-APPARC-0065: TestLocaleViewCaption started..."));
	INFO_PRINTF1(_L("APPFWK-APPARC-0067: TestLocaleViewIconSizeL started..."));
	INFO_PRINTF1(_L("APPFWK-APPARC-0069: TestLocaleViewIconFileL started..."));
	TestViewCaptionAndIconL(EFalse);
	INFO_PRINTF1(_L("APPFWK-APPARC-0065: TestLocaleViewCaption finished..."));
	INFO_PRINTF1(_L("APPFWK-APPARC-0067: TestLocaleViewIconSizeL finished..."));
	INFO_PRINTF1(_L("APPFWK-APPARC-0069: TestLocaleViewIconFileL finished..."));
	}
		

/** 
  This function performs steps to test view related information (view caption, view icon size and view icon 
  file name) on language change event.
  and view icon file name.
  
  This function is called from following test cases -
  TestLocaleViewCaptionAndIconOnDemandL,
  TestLocaleViewCaptionAndIconL
   
  @param aTesCaption Pass true if caption test is required, otherwise false for view icon test.
  @param aOnDemand Pass true if on demand loading is required, otherwise false.   
*/
void CT_LocaleStep::TestViewCaptionAndIconL(TBool aOnDemand)
	{	
	TInt ret = RFbsSession::Connect();
	TEST(ret == KErrNone);
	
	// If it is on-demand loading delay would be very less.
	TInt delay = aOnDemand ? KDelayForOnDemand : KDelay;
	
	// Check language is not currently set to French (any other language except English since English is the default language.
	TEST(User::Language() != ELangFrench);
	
	// Create view array to hold views for TstApp.
	CApaAppViewArray* appViews = new(ELeave) CApaAppViewArray(KViewCount);
	CleanupStack::PushL(appViews);
	
	// Call GetAppViews() by passing tstapp's KUidTestApp as UID. 
	// tstapp provides support for multiple languages. 
	ret = iLs.GetAppViews(*appViews, KUidTestApp);
	TEST(ret == KErrNone);
	
	// Check application's view count. It should have three views.One of them is invalid
	TInt count = appViews->Count();	
	TEST(count == KViewCount);	
	
	// Check application's view's captions or icons. They should be in English
	CheckViewCaptionAndIconL(ELangEnglish, appViews);
	CleanupStack::PopAndDestroy(appViews);		
		
	// Switch locale to French
	TRAP(ret, ChangeLocaleL(ELangFrench));
	TEST(ret == KErrNone);
	TEST(User::Language() == ELangFrench);//check language is set to French

	// Wait for a small period to let apparc receive language change notification. 
	User::After(delay);	
	
	// Create view array to hold views for TstApp.
	appViews = NULL;
	appViews = new(ELeave) CApaAppViewArray(KViewCount);
	CleanupStack::PushL(appViews);
	
	ret = iLs.GetAppViews(*appViews, KUidTestApp);
	TEST(ret == KErrNone);	
	
	count = appViews->Count();
	TEST(count == KViewCount);
	
	//Check application's view's captions or icons. They should be in French.
	CheckViewCaptionAndIconL(ELangFrench, appViews);	
	CleanupStack::PopAndDestroy(appViews);		
	
	//Restore locale to English before exiting the testcase
	TRAP(ret, ChangeLocaleL(ELangEnglish));
	TEST(ret == KErrNone);
	TEST(User::Language() == ELangEnglish);
	
	User::After(delay);
	
	appViews = NULL;
	appViews = new(ELeave) CApaAppViewArray(KViewCount);
	CleanupStack::PushL(appViews);
	ret = iLs.GetAppViews(*appViews, KUidTestApp);
	
	TEST(ret == KErrNone);	
	count = appViews->Count();
	TEST(count == KViewCount);
	
	//check every thing is restored to English.
	CheckViewCaptionAndIconL(ELangEnglish, appViews);
	CleanupStack::PopAndDestroy(appViews);
	RFbsSession::Disconnect();	
	}

/** 
  Tests view's caption and view's icon.
  It is called from TestViewCaptionAndIconL function.
  This function is hardcoded to test view captions and icons for two languages - 
  ELangEnglish and ELangFrench, 
  if any other language is passed from any test case, this function will cause the test case to fail.
     
  @param aLanguage Language for which to test caption/icon.
  @param aAppViews application's view array.  
*/	
void CT_LocaleStep::CheckViewCaptionAndIconL(TLanguage aLanguage, CApaAppViewArray* aAppViews)
	{
	TInt testNumberOfIcons=KViewCount-1;
	// the for loop will check for all view icons except the last one which is invalid
	// Hence the upper limit is KViewCount-1
	for (TInt ii=0; ii < testNumberOfIcons ; ii++)

		{
		const TApaAppViewInfo& viewInfo = (*aAppViews)[ii];
		TEST(viewInfo.iViewCaption.Length() > 0);
		TBuf<256> viewCaption;
		TBuf<256> viewIconFileName;
		TSize iconSize;
		
		switch(aLanguage)
			{
			case ELangEnglish:
				viewCaption.Format(_L("V%d UK"), ii+1);
				iconSize.iWidth  = 50;
				iconSize.iHeight = 50;
				break;
							
			case ELangFrench:
				viewCaption.Format(_L("V%d FR"), ii+1);
				iconSize.iWidth  = 40;
				iconSize.iHeight = 55;
				break;
			
			default:
				// This case will cause test case to fail if any other language is passed apart from English and French.
				break; 
			}
		
		// View's captions should be same as defined in language's localisable strings file (tstappXX.rls)
		TEST(viewInfo.iViewCaption == viewCaption);
		
		// View's icon size should be same as defined in icon file for the selected locale/language.
		CApaMaskedBitmap* viewBitmap = CApaMaskedBitmap::NewLC();
		TInt ret = iLs.GetAppViewIcon(KUidTestApp, viewInfo.iUid, iconSize, *viewBitmap);
		TEST(ret == KErrNone);
		CleanupStack::PopAndDestroy(viewBitmap);
		
		// View's icon file should be same as defined in language's localisable strings file (tstappXX.rls)
		HBufC* fullFileName = NULL;
		ret = iLs.GetAppViewIcon(KUidTestApp, viewInfo.iUid, fullFileName);
		TEST(ret == KErrNone);
		TEST(fullFileName != NULL);
		INFO_PRINTF2(_L("View's icon file name is - %S"), fullFileName);
		TEST(fullFileName->Length() > 0);
		
		delete fullFileName;
		fullFileName = NULL;		
		} //end of for
/*The invalid View icon for language French has the file name "tstappview.mbm" This should 
return KErrNotSupported. Refer to PDEF113054 for details */
	if (aLanguage==ELangFrench)
		{
		TSize iconSize;
		const TApaAppViewInfo& viewInfo = (*aAppViews)[2];
		CApaMaskedBitmap* viewBitmap = CApaMaskedBitmap::NewLC();
		TInt ret = iLs.GetAppViewIcon(KUidTestApp, viewInfo.iUid, iconSize, *viewBitmap);
		TEST(ret == KErrNotSupported);
		CleanupStack::PopAndDestroy(viewBitmap);
		HBufC* fullFileName = NULL;
		ret = iLs.GetAppViewIcon(KUidTestApp, viewInfo.iUid, fullFileName);
		TEST(ret == KErrNone);
		TEST(fullFileName != NULL);
		INFO_PRINTF2(_L("Negative View icon file name is - %S"), fullFileName);
		TEST(fullFileName->Length() > 0);
		delete fullFileName;
		fullFileName = NULL;		
		}
	}
		
CT_LocaleStep::~CT_LocaleStep()
/**
   Destructor
 */
	{
	}

CT_LocaleStep::CT_LocaleStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_LocaleStep);
	}

TVerdict CT_LocaleStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_LocaleStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}


TVerdict CT_LocaleStep::doTestStepL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
{
	INFO_PRINTF1(_L("Locale tests started"));
	
	FbsStartup();
	TEST(KErrNone == iFs.Connect());
	TEST(KErrNone == iLs.Connect());

	// run the tests
	//DONT_CHECK due to changes to file system	
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestLocaleApplicationInfoOnDemandL() ,NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestLocaleApplicationInfoL() ,NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestLocaleApplicationInfo1L() ,NO_CLEANUP);		
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestLocaleViewCaptionAndIconOnDemandL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestLocaleViewCaptionAndIconL() ,NO_CLEANUP);	
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestLocaleDefaultIconL() ,NO_CLEANUP);
	
	iLs.Close();
	iFs.Close();
	
	INFO_PRINTF1(_L("Test completed!"));
	return TestStepResult();
}
