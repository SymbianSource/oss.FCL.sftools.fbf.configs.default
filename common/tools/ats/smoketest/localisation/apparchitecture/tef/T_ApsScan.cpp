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
// This test case is used to verify that a recognizer can be installed and used 
// without reboot. See comment in header file.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include "testableapalssession.h"

#include "T_ApsScan.h"
#include "appfwk_test_utils.h"
#include "tmimerec\constants.hrh"
#include "tmimerec\constants.h"

/////////////////////////////////////////////////

const TInt KDelay = 4000000; // Should be enough for CApaEComMonitor to detect new plugin

/////////////////////////////////////////////////

CT_ApsScanStep::CT_ApsScanStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_ApsScanStep);
	}

CT_ApsScanStep::~CT_ApsScanStep()
	{
	}

/**
Check that the test plugin file has not been left in the filesystem from a previous aborted test.
*/
TVerdict CT_ApsScanStep::doTestStepPreambleL()
	{
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	INFO_PRINTF1(_L("Verifies that dummy.rsc was not present during boot."));
	TBool fileExists = ETrue;
	TInt err = fs.IsFilePresent(KRscTargetPath, fileExists);
	TESTE(fileExists==EFalse, err);
	
	CleanupStack::PopAndDestroy(&fs);
	return TestStepResult();
	}

/*
Delete the plugin file which was installed during the test from c:.
*/
TVerdict CT_ApsScanStep::doTestStepPostambleL()
	{
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	TBool fileExists = EFalse;
	fs.IsFilePresent(KRscTargetPath, fileExists);
	if(fileExists)
		{
		fs.ChangeFilePermissionL(KRscTargetPath); // clear readonly attribute inherited from z:
		const TInt err = fs.DeleteFileL(KRscTargetPath);
		if(err == KErrNone) 
			INFO_PRINTF1(_L("Removed temporary file dummy.rsc"));
		else 
			INFO_PRINTF1(_L("Failed to remove temporary file dummy.rsc"));	
		}
		
	CleanupStack::PopAndDestroy(&fs);
	return TestStepResult();
	}
	
/*
The test entry point
*/			
TVerdict CT_ApsScanStep::doTestStepL()
	{
	RTestableApaLsSession apparcServer;
	CleanupClosePushL(apparcServer);
	User::LeaveIfError(apparcServer.Connect());

	HEAP_TEST_LS_SESSION(apparcServer, 0, DONT_CHECK, TestDynInstallRecognizerL(apparcServer), NO_CLEANUP);

	CleanupStack::PopAndDestroy(&apparcServer);
	
	return TestStepResult();
	}

/**
@SYMTestCaseID APPFWK-APPARC-0049

@SYMDEF DEF082982

@SYMTestCaseDesc This test verify that apparc can dynamically install an Ecom recognizer plugin.

@SYMTestPriority High

@SYMTestStatus Implemented

@SYMTestActions To avoid using a precompiled recognizer for this test, following workaround is used:
	
	1. The test plugin are copied into z: by the build tools when building apparc tef tests.
	2. The plugin will not be installed during startup because CreateRecognizerL will leave
	3. During the test a dummy plugin.rsc will be copied into c:\resource\plugins to trig Ecom
	4. Apparc will be notified and make sure all recognizers listed by Ecom are properly installed
	5. Ecom will not be able install the copied dummy file as it has no matching dll on the same drive, but
	6. The recognizer will detect existence of the dummy file and chose not to leave from CreateRecognizerL
	
	Due to platsec we can't build plugin.dll to an alternative location and we can't
	copy files from epoc32\release\winscw\udeb in runtime. Both plugin.rsc and plugin.dll 
	must be on the same drive to be accepted by Ecom. 

@SYMTestExpectedResults Test should complete without any leave, panic or error.
 */
void CT_ApsScanStep::TestDynInstallRecognizerL(RApaLsSession& aApparcServer)
	{
	INFO_PRINTF1(_L("Step 1 of 4: Verifies that recognizer isn't installed."));
	// TESTL will leave and abort this teststep if DoRecognizeL returns ETrue
	TESTL(EFalse == DoRecognizeL(aApparcServer, TDataType(KDataTypeNew)));
	
	INFO_PRINTF1(_L("Step 2 of 4: Installs recognizer plugin file "));
	CopyRecognizerFileL();
	
	INFO_PRINTF2(_L("Step 3 of 4: Waits %d seconds for new recognizer to be installed."), KDelay/1000000);	
	User::After(KDelay);

	INFO_PRINTF1(_L("Step 4 of 4: Verifies that the new recognizer was installed successfully"));
	TEST(DoRecognizeL(aApparcServer, TDataType(KDataTypeNew)));
	}

/**
Copies the test file dummy.rsc from z: to c: using RSmlTestUtils rather than aFs due to 
platsec protection of involved directories.
*/
void CT_ApsScanStep::CopyRecognizerFileL()
	{		
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	// Ensure c:\\resource\\plugins is created, otherwise leave and abort this teststep
	TInt err = fs.CreateDirectoryL(KRscTargetDir);
	TESTEL((err==KErrNone || err==KErrAlreadyExists), err);
	
	// Copy resource file from z: to c:, if unsuccessful - leave and abort this teststep
	#if defined (__WINSCW__) 
		err = fs.CopyFileL(KRscSourcePathWinscw, KRscTargetPath);
	#else
		err = fs.CopyFileL(KRscSourcePathArmv5, KRscTargetPath);
	#endif
	TESTEL(err==KErrNone, err);

	CleanupStack::PopAndDestroy(&fs);

	}

/**
Check if a certain datatype is known/recognized by apparc.

@param aApparcServer an open session to apparc
@param aDataType the datatype you want to test
@return ETrue if @c aDataType is one of those returned from RApaLsSession::GetSupportedDataTypesL
*/
TBool CT_ApsScanStep::DoRecognizeL(RApaLsSession& aApparcServer, const TDataType& aDataType)
	{
	TBool recognized = EFalse;
	
	CDataTypeArray* dataTypes=new(ELeave) CDataTypeArray(64);
	CleanupStack::PushL(dataTypes);
	aApparcServer.GetSupportedDataTypesL(*dataTypes);
	
	const TInt count = dataTypes->Count();
	for(TInt i=0; i<count; i++)
		{
		const TDataType& tmp = dataTypes->At(i);
		if(tmp == aDataType)
			{
			recognized = ETrue;
			break;
			}
		}
		
	CleanupStack::PopAndDestroy(dataTypes);
	return recognized;
	}
