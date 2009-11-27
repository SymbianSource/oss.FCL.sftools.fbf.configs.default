// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This test case is used to verify that a recognizer plugin can be upgraded and  used 
// without reboot.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include "testableapalssession.h"
#include "T_RecUpgrade.h"
#include "appfwk_test_utils.h"
#include "trecupgrade/upgconstants.h"

/////////////////////////////////////////////////

// Should be enough for CApaEComMonitor to detect new plugin
const TInt KOneSecondDelay = 1000000;

/////////////////////////////////////////////////

CT_RecUpgradeStep::CT_RecUpgradeStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_RecUpgradeStep);
	}

CT_RecUpgradeStep::~CT_RecUpgradeStep()
	{
	}

/**
Check that the test plugin file has not been left in the filesystem from a previous run.
*/
TVerdict CT_RecUpgradeStep::doTestStepPreambleL()
	{
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	INFO_PRINTF1(_L("Verifies that recupgrade.rsc was not present during boot."));
	TBool fileExists = ETrue;
	TInt err = fs.IsFilePresent(KUpgRscTargetPath, fileExists);
	TESTE(fileExists==EFalse, err);
	
	CleanupStack::PopAndDestroy(&fs);
	return TestStepResult();
	}

/*
Delete the plugin file which was installed during the test from c:.
*/
TVerdict CT_RecUpgradeStep::doTestStepPostambleL()
	{
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	TBool fileExists = EFalse;
	fs.IsFilePresent(KUpgRscTargetPath, fileExists);
	if(fileExists)
		{
		fs.ChangeFilePermissionL(KUpgRscTargetPath); // clear readonly attribute inherited from z:
		const TInt err = fs.DeleteFileL(KUpgRscTargetPath);
		if(err == KErrNone) 
			INFO_PRINTF1(_L("Removed temporary file recupgrade.rsc"));
		else 
			INFO_PRINTF1(_L("Failed to remove temporary file recupgrade.rsc"));	
		}
		
	CleanupStack::PopAndDestroy(&fs);
	return TestStepResult();
	}
	
/*
The test entry point
*/			
TVerdict CT_RecUpgradeStep::doTestStepL()
	{
	RTestableApaLsSession apparcServer;
	CleanupClosePushL(apparcServer);
	User::LeaveIfError(apparcServer.Connect());
	//DONT_CHECK skips the heap check at server side. This heap imbalance happens due to copying recupgrade.dll to c:\sys\bin
	//and failed to delete it in cleanup (Windows OS don’t allow to delete any file which are in use).
 	HEAP_TEST_LS_SESSION(apparcServer, 0, DONT_CHECK, TestUpgradeRecognizerL(apparcServer), NO_CLEANUP);

	CleanupStack::PopAndDestroy(&apparcServer);
	
	return TestStepResult();
	}

/**
@SYMTestCaseID APPFWK-APPARC-0100

@SYMDEF INC127312

@SYMTestCaseDesc This test verify that apparc can dynamically upgrade and use an Ecom recognizer plugin.

@SYMTestPriority High

@SYMTestStatus Implemented

@SYMTestActions:
	
	1. Try to recognize .upg file with version 1 of recupgrade ECOM plugin but if shuld not be able to recognize as it is implemented like that
	2. Install version 2 of recupgrade plugin (copy recupgrade_winscw/armv5_rec.dll and recupgrade_1.rsc from z:\system\data to c:\sys\bin and c:\resource\plugin respectively)

	3. Because of installation Apparc will be notified and it will reload all recognizers plugin
	4. Now try to recognize .upg file . it shuld be able to recognize as vertion 2 is implemented like that

@SYMTestExpectedResults Test should complete without any leave, panic or error.
 */
void CT_RecUpgradeStep::TestUpgradeRecognizerL(RApaLsSession& aApparcServer)
	{
	
	TDataRecognitionResult recogResult;
	TBuf8<255> buffer;
	buffer.FillZ(255);
	recogResult.Reset();
		
	// Since recognizer for "mime\upg" mime type is installed but it will return CApaDataRecognizerType::ENotRecognized as implemented : call to RecognizeData()
	// returns CApaDataRecognizerType::ENotRecognized.
	INFO_PRINTF1(_L("MIME type recognition cache is not used..."));
	TEST(aApparcServer.RecognizeData(KUpgDocPath, buffer, recogResult)==KErrNone);

	TEST(recogResult.iConfidence == CApaDataRecognizerType::ENotRecognized);

	INFO_PRINTF1(_L("Installs upgrade of  mime/upg recognizer plugin file "));
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	// Ensure c:\\resource\\plugins is created, otherwise leave from this teststep
	
	TInt err = fs.CreateDirectoryL(KUpgRscTargetDir);
	TESTEL((err == KErrNone || err == KErrAlreadyExists), err);
	INFO_PRINTF1(_L("c:\\resource\\plugins is created successfully or already exists"));
	
	// Ensure c:\\sys\\bin is created, otherwise leave from this teststep
	
	err = fs.CreateDirectoryL(KUpgDllTargetDir);
	TESTEL((err == KErrNone || err == KErrAlreadyExists), err);
	INFO_PRINTF1(_L("c:\\sys\\bin is created successfully or already exists"));

	// Copy recupgrade.rsc from z:\\ to c:\\resource\\plugins, to trigger the rescan for plugins.
	
	err = fs.CopyFileL(KUpgRscSourcePath, KUpgRscTargetPath);
	TEST(err == KErrNone);
	INFO_PRINTF1(_L("Successfully copied recupgrade.rsc from Z:\\system\\data to c\\resource\\plugins"));
	// Copy recupgrade_*.dll from z:\\ to c:\\sys\\bin.
	#ifdef __WINS__ 
		err = fs.CopyFileL(KUpgDllSourcePathWinscw, KUpgDllTargetPath);
	#else
		err = fs.CopyFileL(KUpgDllSourcePathArmv5, KUpgDllTargetPath);
	#endif
	TEST(err == KErrNone);
	INFO_PRINTF1(_L("Successfully copied recupgrade_*.dll from Z:\\system\\data to c:\\sys\\bin"));

	INFO_PRINTF2(_L("Waits %d seconds for new mime/upg recognizer with EPossible confidence to be installed."), (4 * KOneSecondDelay)/1000000);	
	User::After(4 * KOneSecondDelay);
	
	// Without fix for INC127312: Apparc server will not reload all upgraded MIME type recognition hence new version of existing will not used for recognization 
	INFO_PRINTF1(_L("MIME type recognition cache is flushed when a new recogizer is installed"));
	recogResult.Reset();
	TEST(aApparcServer.RecognizeData(KUpgDocPath, buffer, recogResult)==KErrNone);
	TEST(recogResult.iDataType.Des8().CompareF(KMimeUpgType) == 0);
	TEST(recogResult.iConfidence == CApaDataRecognizerType::EPossible);
		
	
// Delete the plugin resource file of the recognizer with EPossible confidence which was installed during the test from z:.
	
	TInt fileExists = EFalse;
	fs.IsFilePresent(KUpgRscTargetPath, fileExists);
	if (fileExists)
		{
		fs.ChangeFilePermissionL(KUpgRscTargetPath); // clear readonly attribute inherited from z:
		err = fs.DeleteFileL(KUpgRscTargetPath);
		if(err == KErrNone) 
			INFO_PRINTF1(_L("Removed file recupgrade.rsc"));
		else 
			INFO_PRINTF1(_L("Failed to remove recupgrade.rsc"));	
		}

	fileExists = EFalse;
	fs.IsFilePresent(KUpgDllTargetPath, fileExists);
	if (fileExists)
		{
		fs.ChangeFilePermissionL(KUpgDllTargetPath); // clear readonly attribute inherited from z:
		err = fs.DeleteFileL(KUpgDllTargetPath);
		if(err == KErrNone) 
			INFO_PRINTF1(_L("Removed recupgrade.dll"));
		else 
			INFO_PRINTF1(_L("Failed to remove recupgrade.dll"));	
		}

	INFO_PRINTF2(_L("Waits %d seconds for the data recognizer list maintained by EMIME to be updated"), (10 * KOneSecondDelay)/1000000);	
 	User::After(10 * KOneSecondDelay);
	
	// Since "mime/upg" recognizer version 2 is uninstalled, mime/upg mime type is not recognized. 
	INFO_PRINTF1(_L("MIME type recognition cache is flushed when a existing recogizer is uninstalled"));
	recogResult.Reset();
	TEST(aApparcServer.RecognizeData(KUpgDocPath, buffer, recogResult)==KErrNone);
	TEST(recogResult.iConfidence == CApaDataRecognizerType::ENotRecognized);
	
	CleanupStack::PopAndDestroy(&fs);
	
	}
