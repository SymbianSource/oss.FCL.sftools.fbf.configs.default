// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Tests CApaAppRegFinder APIs.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <apgicnfl.h>
#include <e32property.h>
#include "T_File3Step.h"
#include "..\apfile\aprfndr.h"
#include "TSidChecker\TestSidChecker.h"

#include <e32test.h>
#include <f32file.h>

_LIT(KCallBackAppRsc,"\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\10281fe0_reg.rsc");
_LIT(KCallBackAppIst,"\\TestSidCheckerCallBackApp.ist");
_LIT(KBadAppRsc,"\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\10281fdc_reg.rsc");
_LIT(KBadAppIst,"\\TestSidCheckerBadApp.ist");
_LIT(KGoodAppRsc,"\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\10281fdd_reg.rsc");
_LIT(KGoodAppIst,"\\TestSidCheckerGoodApp.ist");
_LIT(KForcedAppRsc, "\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\102825b5_reg.rsc");
_LIT(KForcedAppIst, "\\TestSidCheckerForcedApp.ist");
_LIT(KTriggerRescanRsc,"\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\102825b6_reg.rsc");
_LIT(KTriggerRescanIst,"\\TestSidCheckerTriggerRescan.ist");
_LIT(KMmcPrivateDir,"\\private\\10003a3f\\");
_LIT(KAppRsc1,"\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\A000D107_reg.rsc");
_LIT(KAppRsc2,"\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\A000D107_r02.rsc");
_LIT(KAppRsc3,"\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\A000D107_r03.rsc");
_LIT(KAppIst, "\\TestSidCheckerMultipleRegistrationFilesApp.ist");

CT_File3Step::CT_File3Step()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_File3Step);
	}

_LIT(KLitNativeExecutable, "z:\\sys\\bin\\SidCheckerTestDummy.exe");

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CT_File3Step::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	TInt err = iSession.Connect();
	TEST(err==KErrNone);
	err = iUtils.Connect();
	TEST(err==KErrNone);
	return TestStepResult();
	}

TVerdict CT_File3Step::doTestStepL()
/**
  @return - TVerdict code
  Override of base class virtual
*/
	{
	INFO_PRINTF1(_L("Testing the CAppSidChecker api"));
	FindRemovableDriveAndSetPathL();
	// Must be first, since the plugin must not be loaded when this is started.
	//DONT_CHECK since changes on file system changes the heap
 	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestRescanCallBackL(), REComSession::FinalClose() );
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestAppPresenceL(), REComSession::FinalClose() );
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestForceRegistrationL(), REComSession::FinalClose() );
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestCorruptedRegFileL(), NO_CLEANUP );
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestMultipleRegistrationFilesForSameAppL(), REComSession::FinalClose() );
	
	INFO_PRINTF1(_L("Test completed!"));
	return TestStepResult();
	}


/**
   @SYMTestCaseID UIFRAMEWORKS-APPARC-0047
  
   @SYMDEF INC069526
  
   @SYMTestCaseDesc When an implementation of the ApSidChecker interface invokes the rescan callback that was passed to it,
                    the App list server should scan for application registration files. This tests that it does so.
   
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Trigger the loading of a special plugin implementing the ApSidChecker interface
                   Tell the loaded instance of the aforementioned implementation to trigger a rescan via pub&sub
		    -> The plugin will invoke its callback
		    -> The plugin will start a timer
		    -> The timer will publish a result when either:
		       a) it recieves an AppRegisteredAt() call -- this is part of the scan we tried to trigger. (success)
		       b) the timer completes (failure)
		   Retrieve the results from the plugin, again via pub & sub
  
   @SYMTestExpectedResults The Apparc App list server scans for new/removed apps when a plugins requests it
 */
void CT_File3Step::TestRescanCallBackL()
	{
	INFO_PRINTF1(_L("Testing CApaAppRegFinder::TestRescanCallBackL()"));

	CleanupRegFilesL();

	User::LeaveIfError(RProperty::Define(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, RProperty::EInt));	
	User::LeaveIfError(RProperty::Set(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, CTestSidChecker::EPluginLoad));
	
	TProcessPriority oldPriority = RProcess().Priority();
	RProcess().SetPriority(EPriorityLow); // so that we're pre-empted by the Apparc server when it does its scan

	iSession.RegisterNonNativeApplicationTypeL(KApFileTestAppType, KLitNativeExecutable);
	TRAPD(err, PrepareRegFileL(KApFileTestCallBackApp, iCallBackAppRsc, iCallBackAppIst));
	TEST(err==KErrNone);
	User::After(20 * 1000000);

	TRequestStatus status;
	RProperty property;
	CleanupClosePushL(property);
	User::LeaveIfError(RProperty::Set(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, CTestSidChecker::ETriggerScan));
	User::LeaveIfError(property.Attach(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey));
	property.Subscribe(status);

	RTimer timer;
	CleanupClosePushL(timer);
	User::LeaveIfError(timer.CreateLocal());
	TRequestStatus timerStatus;
	timer.After(timerStatus,30 * 1000000);
	
	User::WaitForRequest(status,timerStatus);
	TEST(status != KRequestPending);
	if(status != KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		TInt result;	
		User::LeaveIfError(RProperty::Get(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, result));
		TEST(result == CTestSidChecker::EScanOccurred);
		}
	else
		{
		property.Cancel();
		User::WaitForRequest(status);
		ASSERT(timerStatus != KRequestPending);
		INFO_PRINTF1(_L("Timeout exceeded"));
		}
	
	CleanupStack::PopAndDestroy(2,&property);
	
	RProcess().SetPriority(oldPriority);
	
	CleanupRegFilesL();
	}

/**
   @SYMTestCaseID UIFRAMEWORKS-APPARC-0048
  
   @SYMDEF INC069526
  
   @SYMTestCaseDesc Test that the results of the AppRegisteredAt() function in the ApSidChecker ECOM interface are handled correctly
   
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create application registration files on a removable drive
                   Iterate over all registration files with an instance of CApaAppRegFinder
  
   @SYMTestExpectedResults CApaAppRegFinder's list of entries contains KApFileTestGoodApp but not KApFileTestBadApp
 */
void CT_File3Step::TestAppPresenceL()
	{
	INFO_PRINTF1(_L("Testing CApaAppRegFinder::TestAppPresenceL()"));
	
	/*
	 * Prepare registration files
	 */
	
	CleanupRegFilesL();

	iSession.RegisterNonNativeApplicationTypeL(KApFileTestAppType, KLitNativeExecutable);
	TRAPD(err, PrepareRegFileL(KApFileTestBadApp, iBadAppRsc, iBadAppIst));
	TEST(err==KErrNone);
	TRAP(err, PrepareRegFileL(KApFileTestGoodApp, iGoodAppRsc, iGoodAppIst));
	TEST(err==KErrNone);

	/*
	 * Manually scan files
	 */
	RFs fSession;
	User::LeaveIfError(fSession.Connect());
	CleanupClosePushL(fSession);
	CApaAppRegFinder* regFinder=CApaAppRegFinder::NewLC(fSession);

	TRAPD(ret, regFinder->FindAllAppsL() );
	TEST(ret==KErrNone);

	TBool more = ETrue;
	TBool foundGoodApp = EFalse;
	TBool foundBadApp = EFalse;
	while (more)
		{
		TApaAppEntry entry;
		RPointerArray<HBufC> dummy;
		TRAPD(ret, more=regFinder->NextL(entry, dummy) );
		TEST(ret==KErrNone);
		if(entry.iUidType[2] == KApFileTestBadApp)
			foundBadApp = ETrue;
		if(entry.iUidType[2] == KApFileTestGoodApp)
			foundGoodApp = ETrue;
		}
	TEST(foundGoodApp);
	TEST(!foundBadApp);

	CleanupStack::PopAndDestroy(2,&fSession);	

	/*
	 * Cleanup
	 */	
	CleanupRegFilesL();
	}

/**
   @SYMTestCaseID UIFRAMEWORKS-APPARC-0051
  
   @SYMDEF INC084770
  
   @SYMTestCaseDesc Test that the AppRegisteredAt check can be overridden
   
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Installs an application for which AppRegisteredAt returns EFalse. Then adds this
   application to a list of applications for which the results of AppRegisteredAt will be ignored and
   tests that the application appears in the list of apps.
  
   @SYMTestExpectedResults CApaAppList contains the application.
 */	
void CT_File3Step::TestForceRegistrationL()
	{
	INFO_PRINTF1(_L("Testing RApaLsSession::ForceRegistrationL()"));
	
	/*
	 * Prepare registration files
	 */
	CleanupRegFilesL();
	
	// Install the test application
	iSession.RegisterNonNativeApplicationTypeL(KApFileTestAppType, KLitNativeExecutable);
	TRAPD(err, PrepareRegFileL(KApFileTestForcedRegistration, iForcedAppRsc, iForcedAppIst));
	TEST(err==KErrNone);
	
	// Wait for a rescan and check that the application is not in the list
	RPointerArray<TDesC> dummy;
	err = iSession.ForceRegistration(dummy);
	TApaAppInfo info;
	err = iSession.GetAppInfo(info,KApFileTestForcedRegistration);
	TEST(err==KErrNotFound);
	
	// Add the application to the list of applications that ignore the AppRegisteredAt test
	RPointerArray<TDesC> regFiles;
	_LIT(KRegFile2, "test2");	// Add a second dummy registration file just to check that this works
	regFiles.Append(iForcedAppRsc.AllocL());
	regFiles.Append(KRegFile2().AllocL());
	err = iSession.ForceRegistration(regFiles);
	TEST(err==KErrNone);
	regFiles.ResetAndDestroy();
	err = iSession.GetAppInfo(info,KApFileTestForcedRegistration);
	TEST(err==KErrNone);
	
	// triggers a rescan from the sid checker, this will cause the 
	// list to be emptied
	TRAP(err, PrepareRegFileL(KApTriggerRescan, iTriggerRescanRsc, iTriggerRescanIst));
	TEST(err==KErrNone);
	
	// Now the app should not be listed anymore as we have removed it from the "force" list
	err = iSession.ForceRegistration(dummy);			
	err = iSession.GetAppInfo(info,KApFileTestForcedRegistration);
	TEST(err==KErrNotFound);
	
	/*
	 * Cleanup
	 */	
	CleanupRegFilesL();
	}

void CT_File3Step::PrepareRegFileL(const TUid& aUid, const TDesC& aRscFile, const TDesC& aLogicalExecutable)
	{
	CApaRegistrationResourceFileWriter* const registrationResourceFileWriter =
			CApaRegistrationResourceFileWriter::NewL(aUid,
				                                         aLogicalExecutable,
		    	                                     TApaAppCapability::ENonNative);
	CleanupStack::PushL(registrationResourceFileWriter);

	iSession.PrepareNonNativeApplicationsUpdatesL();
	iSession.RegisterNonNativeApplicationL(KApFileTestAppType, iDrive, *registrationResourceFileWriter, NULL, NULL);
	iSession.CommitNonNativeApplicationsUpdatesL();
	TBool present = EFalse;
	User::LeaveIfError(iUtils.IsFilePresent(aRscFile, present));
	TEST(present);
	
	CleanupStack::PopAndDestroy(registrationResourceFileWriter);
	}

void CT_File3Step::CleanupRegFilesL()
	{
	iSession.PrepareNonNativeApplicationsUpdatesL();
	iSession.DeregisterNonNativeApplicationL(KApFileTestBadApp);
	iSession.DeregisterNonNativeApplicationL(KApFileTestGoodApp);
	iSession.DeregisterNonNativeApplicationL(KApFileTestCallBackApp);
	iSession.DeregisterNonNativeApplicationL(KApFileTestForcedRegistration);
	iSession.DeregisterNonNativeApplicationL(KApTriggerRescan);
	iSession.DeregisterNonNativeApplicationL(KApFileTestAppWithMultipleRegistrationFiles);
	iSession.CommitNonNativeApplicationsUpdatesL();
	iSession.DeregisterNonNativeApplicationTypeL(KApFileTestAppType);
	RFs fSession;
	User::LeaveIfError(fSession.Connect());
	CleanupClosePushL(fSession);
	CFileMan* fMan = CFileMan::NewL(fSession);
	CleanupStack::PushL(fMan);
	TInt err = fMan->Delete(iMmcPrivateDir, CFileMan::ERecurse);
	if(err != KErrNone && err != KErrNotFound && err != KErrPathNotFound)
		{
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(2,&fSession);
	}
	
/**
   @SYMTestCaseID UIFRAMEWORKS-APPARC-0052
  
   @SYMDEF INC87806
  
   @SYMTestCaseDesc Test that apparc does not stop scanning its list of registration files after scanning a corrupted registration file.
   
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Copy a corrupted registration file into C:\\Private\\10003a3f\\import\\apps\\. Register a non-native application. Force apparc to rescan. Since non-native applications
   					are scanned after native ones, we eliminate the risk that the corrupted registration file is scanned after the non-native one. Call GetAppInfo on non-native application.
  
   @SYMTestExpectedResults Apparc successfully scans the non-native app. Calling GetAppInfo on the non-native app returns KErrNone. 
 */	
void CT_File3Step::TestCorruptedRegFileL()
	{
	INFO_PRINTF1(_L("TestCorruptedRegFileL about to start..."));	
	_LIT(KCorruptedSourceFileName, "Z:\\ApparcTest\\Corrupted_reg.RSC"); // corrupted_reg.rsc file is pre-built.
	_LIT(KCorruptedTargetFileName, "C:\\Private\\10003a3f\\import\\apps\\Corrupted_reg.RSC");
	_LIT(KDir, "C:\\private\\10003a3f\\import\\apps\\");
	_LIT(KNonNativeFileName,"C:\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\NonCorrupted_reg.rsc");
	_LIT(KLitNoncorruptedExe, "z:\\sys\\bin\\NonCorrupted.exe");
	const TUid KNonCorruptedAppUid = {0x102826DE};

	RFs	fs;
 	User::LeaveIfError(fs.Connect());
 	CleanupClosePushL(fs);
  	TInt ret = fs.MkDirAll(KDir);
	if (ret != KErrAlreadyExists)
		{
		User::LeaveIfError(ret);			
		}
  	CFileMan* fileManager = CFileMan::NewL(fs);
 	CleanupStack::PushL(fileManager);
	ret = fileManager->Copy(KCorruptedSourceFileName, KCorruptedTargetFileName, CFileMan::EOverWrite);
	TEST(ret==KErrNone);

	CApaRegistrationResourceFileWriter* const registrationResourceFileWriter=CApaRegistrationResourceFileWriter::NewL(KNonCorruptedAppUid,KNonNativeFileName,TApaAppCapability::ENonNative);
	CleanupStack::PushL(registrationResourceFileWriter);
	iSession.PrepareNonNativeApplicationsUpdatesL();
	iSession.RegisterNonNativeApplicationTypeL(KApFileTestAppType,KLitNoncorruptedExe);
	iSession.RegisterNonNativeApplicationL(KApFileTestAppType, EDriveC, *registrationResourceFileWriter, NULL, NULL);
	iSession.CommitNonNativeApplicationsUpdatesL();
	
	RPointerArray<TDesC> empty;
	ret = iSession.ForceRegistration(empty);
	TEST(ret==KErrNone);
	TApaAppInfo appInfo;
	ret = iSession.GetAppInfo(appInfo,KNonCorruptedAppUid);
	TEST(ret==KErrNone);
	
	iSession.PrepareNonNativeApplicationsUpdatesL();
	iSession.DeregisterNonNativeApplicationL(KNonCorruptedAppUid);
	iSession.DeregisterNonNativeApplicationTypeL(KApFileTestAppType);
	iSession.CommitNonNativeApplicationsUpdatesL();
	fileManager->RmDir(KDir);

	CleanupStack::PopAndDestroy(registrationResourceFileWriter);
	CleanupStack::PopAndDestroy(fileManager);
	CleanupStack::PopAndDestroy(&fs);
	}

/**
   @SYMTestCaseID UIFRAMEWORKS-APPARC-0102
  
   @SYMDEF PDEF139147
  
   @SYMTestCaseDesc Test that apparc calls sid checker only once for multiple registration resource file for same application.
   
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Make copies of a registration resource file in the removable drive. Plugin implementing the ApSidChecker 
   					interface will set a Pub&Sub value everytime it is called. Check that it has been called only once.
  
   @SYMTestExpectedResults Pub&Sub value has been set only once by the SidChecker plugin. 
 */	
void CT_File3Step::TestMultipleRegistrationFilesForSameAppL()
	{
	INFO_PRINTF1(_L("Testing Multiple registration files for same application"));
	
	//Cleanup registration files
	CleanupRegFilesL();
	
 	// Install the test application
	iSession.RegisterNonNativeApplicationTypeL(KApFileTestAppType, KLitNativeExecutable);
	TRAPD(err, PrepareRegFileL(KApFileTestAppWithMultipleRegistrationFiles, iAppRsc1, iAppIst));
	TEST(err==KErrNone);
	INFO_PRINTF1(_L("Reg File 1 commited"));
 	
	RFs	fs;
 	User::LeaveIfError(fs.Connect());
 	CleanupClosePushL(fs);
  	TInt ret = fs.MkDirAll(KMmcPrivateDir);
	if (ret != KErrAlreadyExists)
		{
		User::LeaveIfError(ret);			
		}
	CFileMan* fileManager = CFileMan::NewL(fs);
 	CleanupStack::PushL(fileManager);
 	//Make copies of the registration resource files.
	ret = fileManager->Copy(iAppRsc1, iAppRsc2, CFileMan::EOverWrite);
	TEST(ret==KErrNone);
	TBool present = EFalse;
	User::LeaveIfError(iUtils.IsFilePresent(iAppRsc2, present));
	TEST(present);
	INFO_PRINTF1(_L("Reg File 2 created"));
	
	ret = fileManager->Copy(iAppRsc1, iAppRsc3, CFileMan::EOverWrite);
	TEST(ret==KErrNone);
	present = EFalse;
	User::LeaveIfError(iUtils.IsFilePresent(iAppRsc3, present));
	TEST(present);
	INFO_PRINTF1(_L("Reg File 3 created"));
	
	TProcessPriority oldPriority = RProcess().Priority();
	RProcess().SetPriority(EPriorityLow); // so that we're pre-empted by the Apparc server when it does its scan
	
	TRequestStatus status;
	RProperty property;
	CleanupClosePushL(property);
	User::LeaveIfError(RProperty::Set(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, CTestSidChecker::EPropertyDefault));
	User::LeaveIfError(property.Attach(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey));
	property.Subscribe(status);

	RTimer timer;
	CleanupClosePushL(timer);
	User::LeaveIfError(timer.CreateLocal());
	TRequestStatus timerStatus;
	timer.After(timerStatus,50 * 1000000);
	
	RPointerArray<TDesC> empty;
	ret = iSession.ForceRegistration(empty);
	TEST(ret==KErrNone);
	
	// Wait for a rescan
	User::WaitForRequest(status,timerStatus);
	TEST(status != KRequestPending);
	if(status != KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		TInt result;	
		User::LeaveIfError(RProperty::Get(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, result));
		TEST(result == CTestSidChecker::ECheckedOnce);
		}
	else
		{
		property.Cancel();
		User::WaitForRequest(status);
		TEST(timerStatus != KRequestPending);
		INFO_PRINTF1(_L("Timeout exceeded"));
		}
		
	RProcess().SetPriority(oldPriority);
	
	INFO_PRINTF1(_L("Removing duplicate reg files"));
	TTime tempTime(0); // added tempTime to avoid asynch CFileMan::Attribs request completing with KErrArgument
	
	TEST(fileManager->Attribs(iAppRsc2,0,KEntryAttReadOnly,tempTime,CFileMan::ERecurse,status)==KErrNone);
	User::WaitForRequest(status);
	TEST(status.Int()==KErrNone);
 	TEST(KErrNone==fileManager->Delete(iAppRsc2,CFileMan::ERecurse));
 	
 	TEST(fileManager->Attribs(iAppRsc3,0,KEntryAttReadOnly,tempTime,CFileMan::ERecurse,status)==KErrNone);
	User::WaitForRequest(status);
	TEST(status.Int()==KErrNone);
 	TEST(KErrNone==fileManager->Delete(iAppRsc3,CFileMan::ERecurse));
 
 	CleanupStack::PopAndDestroy(4,&fs);
	CleanupRegFilesL();
	}
	
void CT_File3Step::FindRemovableDriveAndSetPathL()
{
#if defined(__WINS__)
	// The removable media is expected at X: on emulator.
	iDrive = EDriveX;
#else

	RFs	fs;
 	User::LeaveIfError(fs.Connect());

	// The removable media is expected at D: on NAND ROM and at E: on normal ROMs.
	//The following code works on techview but not guaranteed to work on all platforms. 
	TDriveInfo driveInfo;
	TInt err = fs.Drive(driveInfo, EDriveD);
	if(err == KErrNone && ((driveInfo.iDriveAtt & KDriveAttRemovable) != 0))
		{
	 	 // Use drive D
	 	 iDrive = EDriveD;
	 	}
	 else
	 	{
		err = fs.Drive(driveInfo, EDriveE);
		if(err == KErrNone && ((driveInfo.iDriveAtt & KDriveAttRemovable) != 0))
			{
		 	// Use drive E
		 	iDrive = EDriveE;
		 	}
		}
	fs.Close();
#endif
	iCallBackAppRsc = iDrive.Name();
	iCallBackAppIst = iDrive.Name();
	iBadAppRsc = iDrive.Name();
	iBadAppIst = iDrive.Name();
	iGoodAppRsc = iDrive.Name();
	iGoodAppIst = iDrive.Name();
	iForcedAppRsc = iDrive.Name();
	iForcedAppIst = iDrive.Name();
	iTriggerRescanRsc = iDrive.Name();
	iTriggerRescanIst = iDrive.Name();
	iMmcPrivateDir = iDrive.Name();
	iAppRsc1 = iDrive.Name();
	iAppRsc2 = iDrive.Name();
	iAppRsc3 = iDrive.Name();
	iAppIst = iDrive.Name();
	
	iCallBackAppRsc.Append(KCallBackAppRsc);
	iCallBackAppIst.Append(KCallBackAppIst);
	iBadAppRsc.Append(KBadAppRsc);
	iBadAppIst.Append(KBadAppIst);
	iGoodAppRsc.Append(KGoodAppRsc);
	iGoodAppIst.Append(KGoodAppIst);
	iForcedAppRsc.Append(KForcedAppRsc);
	iForcedAppIst.Append(KForcedAppIst);
	iTriggerRescanRsc.Append(KTriggerRescanRsc);
	iTriggerRescanIst.Append(KTriggerRescanIst);
	iMmcPrivateDir.Append(KMmcPrivateDir);
	iAppRsc1.Append(KAppRsc1);
	iAppRsc2.Append(KAppRsc2);
	iAppRsc3.Append(KAppRsc3);
	iAppIst.Append(KAppIst);
}

TVerdict CT_File3Step::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	iSession.Close();
	iUtils.Close();
	return TestStepResult();
	}

CT_File3Step::~CT_File3Step()
/**
   Destructor
 */
	{
	}
