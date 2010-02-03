// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Note: With PREQ1228, we introduced "REQ5896:Load application information from a single file".
// This PREQ is tested with the following two test steps:
// T_AppListBootStep (only run for emulator, as it requires epoc restart)
// |___ (APPFWK-APPARC-0033) Tests that the application information file (AppsList.bin) is created 
// |		if it does not exists, and we reboot the phone.
// |___ (APPFWK-APPARC-0034) Tests that the Short Caption associated with an app is restored to
// its original value when the phone is restarted.
// T_AppListUpdateStep
// |___ (APPFWK-APPARC-0035) Tests that the application information file (AppsList.bin) is created 
// |		if it does not exists, and we force a registration.
// |___ (APPFWK-APPARC-0032) Tests that the application information file (AppsList.bin) is updated 
// whenever a new application is installed.
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include <apgcli.h>
#include "T_AppListFileBootStep.h"


_LIT(KAppListFileName,"c:\\private\\10003a3f\\AppsListCache\\AppsList.bin");
_LIT(KShortCaption1, "SC1");
_LIT(KOriginalShortCaption, "TC UK");

const TUid KUidTestApp={10};
const TInt KMaxTimeCount = 18;						// 18 * 10 sec == 180 seconds


/**
Call base class method to set up the human readable name for logging TestStepName
iBootType value set as per aBootType
*/
CT_AppListFileBootStep::CT_AppListFileBootStep(TInt aBootType)
	{
	iBootType = aBootType;
	if(iBootType == 1)
		{
		SetTestStepName(KT_AppListFile_Boot1);
		}
	else if(iBootType == 2)
		{
		SetTestStepName(KT_AppListFile_Boot2);
		}
	else
		{
		SetTestStepName(KT_AppListFile_Boot3);
		}
	}


CT_AppListFileBootStep::~CT_AppListFileBootStep()
	{
	}


TVerdict CT_AppListFileBootStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Test step started...."));

#ifdef __WINS__
 	__UHEAP_MARK;
	TRAPD(ret, DoTestL());
	TEST(ret == KErrNone);
 	INFO_PRINTF2(_L("DoTestL() finished with return code '%d'"), ret);
  	__UHEAP_MARKEND;
#else
 	INFO_PRINTF1(_L("TEST NOT RUN! - T_AppListFile_BootX tests are meant only to run on Emulator"));
 	TEST(EFalse);
#endif

	INFO_PRINTF1(_L("....test step finished!"));
	return TestStepResult();	
	}


/**
   @SYMTestCaseID 	APPFWK-APPARC-0033, APPFWK-APPARC-0034
  
   @SYMPREQ 		PREQ1228 (CR0831)
   
   @SYMREQ			REQ5896
   
   @SYMTestPriority High 
   
   @SYMTestType		CIT
  
   @SYMTestCaseDesc Tests that the application information file (AppsList.bin) is created if it does 
   					not exists, and we reboot the phone.
   					Tests that the Short Caption associated with an app is restored to its original 
   					value when the phone is restarted.
     
   @SYMTestStatus 	Implemented
   
   @SYMTestActions This tests the update of the applist file in \\private\\10003a3f\\import\\\apps\\.
   			The sequence is as follows:
   			(BOOT - 1)
   				1.- Check the applist file exists. It must exists.
   				2.- Delete the applist file.
   			(BOOT - 2)
   				3.- Check the applist file does not exist.
   				4.- Check the applist is created after a short period of time.
   				5.- Set short caption of a test app to a custom value.
   			(BOOT - 3)
   				6.- Check short caption of the test app is restored to its original value.
   
   @SYMTestExpectedResults	For Boot1 AppList File must exist when the test starts, but not when it finishes.
   							For Boot2 AppList File must not exist when the test starts, but is created later on.
   							For Boot3 ShortCaption for tstapp.exe must return to original short caption
 */
 
 void CT_AppListFileBootStep::DoTestL()
 	{
 	TInt ret = iUtils.Connect();
	TEST(ret==KErrNone);
	
	TBool present = EFalse;
	ret = iUtils.IsFilePresent(KAppListFileName, present);
	TEST(ret == KErrNone);
	
	switch(iBootType)
		{
		case 1:
			{
			INFO_PRINTF1(_L("Starting test case: APPFWK-APPARC-0033"));
			// Check the file is present, or wait until it is. When/If present, delete it.
 			INFO_PRINTF1(_L("The should file exists (as no other test have removed it), so then this test should delete it:"));			
			present = CheckForFilePresent();			
			TEST(present);
			if (present)
				{
				DeleteAppListFileL(KAppListFileName);
				}
 			INFO_PRINTF1(_L("APPFWK-APPARC-0033 Test Continues... with Boot2 script"));			
			}
			break;
			
		case 2:
			{
 			INFO_PRINTF1(_L("APPFWK-APPARC-0033 Test Continues... from Boot1 script"));			
			// Check the file not present, and then wait until it is.
 			INFO_PRINTF1(_L("The file should not exist as it has been deleted by Boot1, otherwise the test fails:"));			
			TEST(!present);
			present = CheckForFilePresent();			
			TEST(present);
	 		INFO_PRINTF1(_L("APPFWK-APPARC-0033 Test Finished!"));

			INFO_PRINTF1(_L("Starting test case: APPFWK-APPARC-0034"));
			SetShortCaptionSettings();
 			INFO_PRINTF1(_L("APPFWK-APPARC-0034 Test Continues... with Boot3 script"));			
			}
			break;
		
		case 3:
			{
 			INFO_PRINTF1(_L("APPFWK-APPARC-0034 Test Continues... from Boot2 script"));
	 		TestShortCaptionSettings();
	 		INFO_PRINTF1(_L("APPFWK-APPARC-0034 Test Finished!"));
	 		}
			break;
		}
 	iUtils.Close();
 	}


/*
It will delete the applistfile created by apparc component to test the change in the bootup time.
*/
void CT_AppListFileBootStep::DeleteAppListFileL(const TDesC& aFileName)
	{
	INFO_PRINTF2(_L("Delete AppList file '%S'..."), &aFileName);

	TInt ret = iUtils.SetReadOnly(aFileName, 0);
	TEST(ret == KErrNone);
	
	ret = iUtils.DeleteFileL(aFileName);
	TEST(ret == KErrNone);
	}


/*
Checks whether the "AppList.Bin" exists, if not waits for a maximum time of (KMaxTimeCount * 10) seconds for the file to create. 
Else the test fails.
*/
TBool CT_AppListFileBootStep::CheckForFilePresent()
	{
	INFO_PRINTF2(_L("Wait for a maximum time of '%d' secs for the file to be created"), KMaxTimeCount * 10);

	TBool present = EFalse;
	for (TInt index = 0; present == EFalse && index < KMaxTimeCount; index++)
		{
		TEST(iUtils.IsFilePresent(KAppListFileName, present) == KErrNone);
		if (present)
			{
			INFO_PRINTF2(_L("AppList file is successfully created after '%d' seconds!"), index * 10);
			break;
			}
		// wait 10 secs before trying again in the next loop
		User::After(10 * 1000000);	
		}

	if (!present)
		{
		INFO_PRINTF2(_L("AppList file was NOT created after '%d' seconds"), KMaxTimeCount * 10);
		}

	return present;
	}


/*
Testing whether short caption for test app is set as the original short caption
*/
void CT_AppListFileBootStep::TestShortCaptionSettings()
	{
	INFO_PRINTF1(_L("Testing whether short caption for test app is set as the original short caption"));

	TInt err = iSession.Connect();
	TEST(err == KErrNone);

	TApaAppInfo info1;
	err = iSession.GetAppInfo(info1, KUidTestApp);
	TEST(err == KErrNone);
	
	TBuf<10> expected;
	expected.Format(KOriginalShortCaption);
	TBuf<10> actual;
	actual.Format(info1.iShortCaption);
	INFO_PRINTF3(_L("Short Caption Settings: Expected '%S', Actual '%S'"), &expected, &actual);
	TEST(info1.iShortCaption==KOriginalShortCaption);

	// close session and finish
	iSession.Close();
	}
	

/*
Sets the short caption for tstapp.exe to KShortCaption1
*/
void CT_AppListFileBootStep::SetShortCaptionSettings()
	{
	INFO_PRINTF2(_L("Setting short caption settings to '%S'..."), &KShortCaption1);

	TInt err = iSession.Connect();
	TEST(err == KErrNone);
	err = iSession.SetAppShortCaption(KShortCaption1, ELangEnglish, KUidTestApp);
	TEST(err == KErrNone);
	
	// wait 2.5 secs for the caption to be updated
	User::After(2500000);

	TApaAppInfo info1;
	err = iSession.GetAppInfo(info1, KUidTestApp);
	TEST(err == KErrNone);
	
	TBuf<10> expected;
	expected.Format(KShortCaption1);
	TBuf<10> actual;
	actual.Format(info1.iShortCaption);
	INFO_PRINTF3(_L("Short Caption Settings: Expected '%S', Actual '%S'"), &expected, &actual);
	TEST(info1.iShortCaption==KShortCaption1);

	// close session and finish
	iSession.Close();
	}
