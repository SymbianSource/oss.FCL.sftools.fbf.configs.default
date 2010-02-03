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
// |___ (APPFWK-APPARC-0034) Tests that the Short Caption associated with an app
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
#include "T_AppListFileUpdateStep.h"

_LIT(KTestAppZPath,"Z:\\ApparcTest\\TestAppInstall_reg.RSC");
_LIT(KAppListFileName,"C:\\private\\10003a3f\\AppsListCache\\AppsList.bin");
_LIT(KAppTimeFormat,"%:0%H%:1%T%:2%S%:3");
_LIT(KAppDirectory,"C:\\Private\\10003a3f\\Import\\apps\\");
_LIT(KTestAppObsolutePath1,"C:\\Private\\10003a3f\\Import\\apps\\TestAppInstall_reg.RSC");

const TInt KMaxTimeCount = 18;			// 18 * 10 is 180 Seconds


CT_AppListFileUpdateStep::CT_AppListFileUpdateStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_AppListFileUpdate);
	}


CT_AppListFileUpdateStep::~CT_AppListFileUpdateStep()
	{
	}

TVerdict CT_AppListFileUpdateStep::doTestStepPreambleL()
	{
 	User::LeaveIfError(iSession.Connect());
 	User::LeaveIfError(iUtils.Connect());
 	User::LeaveIfError(iFs.Connect());
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_AppListFileUpdateStep::doTestStepPostambleL()
	{
	iFs.Close();
	iUtils.Close();
	iSession.Close();
	return TestStepResult();
	}


TVerdict CT_AppListFileUpdateStep::doTestStepL()
	{
	INFO_PRINTF1(_L("AppListFileUpdateStep test step started..."));

 	__UHEAP_MARK;
	INFO_PRINTF1(_L("APPFWK-APPARC-0035 Test Started"));
	TRAPD(ret, TestDeleteAppListFileL());
	TEST(ret == KErrNone);
 	INFO_PRINTF2(_L("TestDeleteAppListFileL() finished with return code '%d'\n"), ret);
	INFO_PRINTF1(_L("APPFWK-APPARC-0035 Test Finished\n"));
  	__UHEAP_MARKEND;

 	__UHEAP_MARK;
	INFO_PRINTF1(_L("APPFWK-APPARC-0032 Test Started"));
	TRAP(ret, TestTimeStampL());
	TEST(ret == KErrNone);
 	INFO_PRINTF2(_L("TestTimeStampL() finished with return code '%d'"), ret);
	INFO_PRINTF1(_L("APPFWK-APPARC-0032 Test Finished\n"));
  	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("...AppListFileUpdateStep test step finished!"));
	return TestStepResult();
	}


/**
   @SYMTestCaseID 	APPFWK-APPARC-0032
  
   @SYMPREQ 		PREQ1228 (CR0831)
   
   @SYMREQ			REQ5896
  
   @SYMTestCaseDesc Tests that the application information file (AppsList.bin) is updated 
					whenever a new application is installed.

   @SYMTestType		CIT
   
   @SYMTestPriority High 
  
   @SYMTestStatus 	Implemented
   
   @SYMTestActions 	This tests the update of the applist file in \\private\\10003a3f\\import\\\apps\\.
   			The sequence is as follows:
   			1.- Check the file exists. It must exists.
   			2.- Record timestamp for applist file.
   			3.- Install a new test application.
   			4.- Record timestamp for applist file, allowing some time for the file to be updated
   			4.- Compare timestamps before and after test app installation.
	   
   @SYMTestExpectedResults	Modified time of AppsList.bin after installation must be greater than before installation. 
 */
 
 void CT_AppListFileUpdateStep::TestTimeStampL()
 	{
	// Create KAppDirectory
	TInt err = iUtils.CreateDirectoryL(KAppDirectory);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	// Wait until KAppListFileName is present and check that the file has been created indeed
	TBool present = CheckForFilePresent();
	TEST(present);

	// Record time before app list was modified	
	INFO_PRINTF1(_L("Record time before app list was modified..."));
	TTime timeBeforeAppListModification;
	AppsListModifiedTimeL(timeBeforeAppListModification);
		
	// Install an application
	INFO_PRINTF1(_L("Install application..."));
	InstallApplicationL(KTestAppObsolutePath1);

	// wait 5 seconds for the app to be properly installed
	User::After(5 * 1000000);
	
	// Wait until KAppListFileName is present and check that the file has been created indeed.
	present = CheckForFilePresent();
	TEST(present);

	// Record time after app list was modified	
	INFO_PRINTF1(_L("Record time after app list was modified..."));
	TTime timeAfterAppListModification;
	AppsListModifiedTimeL(timeAfterAppListModification);
    
    // Check that the modification time "after" is after the modification time "before"
    TEST(timeAfterAppListModification > timeBeforeAppListModification);

	// Uninstall & delete...
	INFO_PRINTF1(_L("Uninstalling application..."));
	DeleteApplicationL(KTestAppObsolutePath1);
 	}
 
 
/*
Delete a registration resource file (TestAppInstall.rsc) in the path  "C:\private\10003a3f\import\apps" .
*/
void CT_AppListFileUpdateStep::DeleteApplicationL(const TDesC& aAppName)
	{
	INFO_PRINTF2(_L("Deleting file '%S'"), &aAppName);

	TInt ret = iUtils.SetReadOnly(aAppName, 0);
	TEST(ret == KErrNone);
	ret = iUtils.DeleteFileL(aAppName);
	TEST(ret == KErrNone);
	}


/*
Copy a registration resource file (TestAppInstall.rsc) in the path  "c:\private\10003a3f\import\apps" .
*/
void CT_AppListFileUpdateStep::InstallApplicationL(const TDesC& aAppName)
	{
	INFO_PRINTF3(_L("Copying file '%S' to folder '%S'"), &aAppName, &KTestAppZPath);

	TInt ret = iUtils.CopyFileL(KTestAppZPath, aAppName);
	TEST(ret == KErrNone);
	}


/*
Gets the modified time for AppsList.bin
*/
void CT_AppListFileUpdateStep::AppsListModifiedTimeL(TTime& aTime)
	{
	INFO_PRINTF2(_L("Retrieving modification time of file '%S'"), &KAppListFileName);
	TInt ret = iFs.Modified(KAppListFileName, aTime);
	TEST(ret == KErrNone);
	
	TBuf<40> timeString; // Holds the formatted date and time
    aTime.FormatL(timeString,KAppTimeFormat);
	INFO_PRINTF2(_L("Recorded modified time is '%S'"), &timeString);
	}


/**
   @SYMTestCaseID 	APPFWK-APPARC-0035
  
   @SYMPREQ 		PREQ1228 (CR0831)
   
   @SYMREQ			REQ5896
  
   @SYMTestCaseDesc Apparc component repopulates the file list if it has been deleted and a registration is forced.

   @SYMTestType		CIT
   
   @SYMTestPriority High 
  
   @SYMTestStatus 	Implemented
   
   @SYMTestActions 	This tests the update of the applist file in \\private\\10003a3f\\import\\\apps\\.
   			The sequence is as follows:
   			1.- Check the file exists. It must exists.
   			2.- Delete the file and check it does not exist, and that is not created either.
   			3.- Force Registration, which means that the applist file needs to be created again.
   			4.- Check that the above mentioned file exists again after waiting for a short period of time.
	   
   @SYMTestExpectedResults	AppsList.bin is deleted and created according to the test actions.
 */
 
 void CT_AppListFileUpdateStep::TestDeleteAppListFileL()
 	{
	// Check the file IS present
	TBool present = CheckForFilePresent();			
	TEST(present);

	// Delete applist file
	DeleteAppListFileL(KAppListFileName);

	// Check the file is NOT present
	present = CheckForFilePresent();			
	TEST(!present);
	
	// Do a rescan and check that the file exists again.
	INFO_PRINTF1(_L("Do a rescan and check that the file exists again...."));
	RPointerArray<TDesC> dummy;
	TEST(iSession.ForceRegistration(dummy) == KErrNone);
	present = CheckForFilePresent();			
	TEST(present);
 	}


/*
Checks whether the "AppList.Bin" exists, if not waits for a maximum time of (KMaxTimeCount * 10) seconds for the file to create. 
Else the test fails.
*/
TBool CT_AppListFileUpdateStep::CheckForFilePresent()
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
It will delete the applistfile created by apparc component
*/
void CT_AppListFileUpdateStep::DeleteAppListFileL(const TDesC& aFileName)
	{
	INFO_PRINTF2(_L("Delete AppList file '%S'..."), &aFileName);

	TInt ret = iUtils.SetReadOnly(aFileName, 0);
	TEST(ret == KErrNone);
	ret = iUtils.DeleteFileL(aFileName);
	TEST(ret == KErrNone);
	}
