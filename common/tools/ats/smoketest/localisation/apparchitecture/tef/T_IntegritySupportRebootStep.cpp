// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



/**
 @file
 @internalComponent - Internal Symbian test code  
*/

#include "T_IntegritySupportRebootStep.h"

/**
 * Constructor
 */	
CT_IntegritySupportReboot1TestStep::CT_IntegritySupportReboot1TestStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_IntegritySupportReboot1Step);
	}

/**
 * Destructor
 */
CT_IntegritySupportReboot1TestStep::~CT_IntegritySupportReboot1TestStep()
	{
	iSession.Close();
	iUtils.Close();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */	
TVerdict CT_IntegritySupportReboot1TestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	doAbstractNonNativeAppsTestStepPreambleL();
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_IntegritySupportReboot1TestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CT_IntegritySupportReboot1TestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparc...T_IntegritySupportReboot1 Test Cases Running..."));

	TRAPD(ret,RunTestCasesL())
	TEST(ret==KErrNone);
	
	INFO_PRINTF1(_L("...T_IntegritySupportReboot1 Test Cases Completed."));

	return TestStepResult();
	}
	
TInt CT_IntegritySupportReboot1TestStep::RunTestCasesL()
	{
	//DONT_CHECK due to recreation of applist on server
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestSetupL(), NO_CLEANUP);
	
	return KErrNone;
	}
	
_LIT(KLitLogicalExecutable1, "c:\\TestIntegritySupportReboot1.ist");
_LIT(KLitRegistrationFile1,"c:\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\102081ce_reg.rsc");
static const CT_AbstractNonNativeAppsTestStep::TTestAppDetails KApplication1(0x102081CE,KLitLogicalExecutable1,KLitRegistrationFile1);


_LIT(KLitLogicalExecutable2, "c:\\TestIntegritySupportReboot2.ist");
_LIT(KLitRegistrationFile2,"c:\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\102081cf_reg.rsc");
static const CT_AbstractNonNativeAppsTestStep::TTestAppDetails KApplication2(0x102081CF,KLitLogicalExecutable2,KLitRegistrationFile2);

/**
   @SYMTestCaseID T-IntegritySupportReboot1TestStep-TestSetupL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc This is only the first half of the test case. It will simulate a
   reboot in the middle of an installation. 
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Simulate a reboot in the middle of an installation.
   
   @SYMTestExpectedResults None. See CT_IntegritySupportReboot2TestStep for the actual test.
 */	
void CT_IntegritySupportReboot1TestStep::TestSetupL()
	{
	INFO_PRINTF1(_L("Setting up reboot recovery test..."));

	// so that we can check the presence/absence of stuff before apparc has respawned
	RProcess().SetPriority(EPriorityHigh); 

	// make sure we're starting from a clean sheet
	CleanupAndReset(iSession, KApplication1);
	CleanupAndReset(iSession, KApplication2);

	// prepare the starting state: KApp1 uninstalled, KApp2 installed
	CompleteRegisterL(iSession, KApplication2);
	
	TEST(AppAbsent(iSession, KApplication1));
	TEST(AppPresent(iSession, KApplication2));

	// Prepare to install KApp1 and remove KApp2, and then panic after actually performing these steps
	iSession.PrepareNonNativeApplicationsUpdatesL();
	CallRegisterL(iSession, KApplication1);
	CallDeregisterL(iSession, KApplication2);
	iSession.AddPanicingNonNativeApplicationsUpdate();

	// test that nothing has changed yet
	TEST(AppAbsent(iSession, KApplication1));
	TEST(AppPresent(iSession, KApplication2));

	// commit, expect Apparc to die horribly
	// this function will actually restart apparc if it fails the first time
	TRAPD(err,iSession.CommitNonNativeApplicationsUpdatesL());
	TEST(err == KErrServerTerminated);
	
	// the files should be as-if the update happened, but we can't check
	// the app list because apparc is now dead.
	TEST(FilePresent(KLitUpdatedAppsList));
	TEST(FilePresent(KApplication1.iRegistrationFile));
	TEST(FileAbsent(KApplication2.iRegistrationFile));
	}

/**
 * Constructor
 */	
CT_IntegritySupportReboot2TestStep::CT_IntegritySupportReboot2TestStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_IntegritySupportReboot2Step);
	}

/**
 * Destructor
 */
CT_IntegritySupportReboot2TestStep::~CT_IntegritySupportReboot2TestStep()
	{
	iSession.Close();
	iUtils.Close();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */	
TVerdict CT_IntegritySupportReboot2TestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	doAbstractNonNativeAppsTestStepPreambleL();
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_IntegritySupportReboot2TestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CT_IntegritySupportReboot2TestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparc...T_IntegritySupportReboot2 Test Cases Running..."));

	TRAPD(ret,RunTestCasesL())
	TEST(ret==KErrNone);
	
	INFO_PRINTF1(_L("...T_IntegritySupportReboot2 Test Cases Completed."));

	return TestStepResult();
	}
	
TInt CT_IntegritySupportReboot2TestStep::RunTestCasesL()
	{
	HEAP_TEST_LS_SESSION(iSession, 0, 0, TestRollbackL(), NO_CLEANUP);
	return KErrNone;
	}
	
/**
   @SYMTestCaseID T-IntegritySupportReboot2TestStep-TestRollbackL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc This test case assumes IntegritySupportReboot1TestStep has been run before
  IntegritySupportReboot1TestStep have been rolled back and that the device is in a consistent state.
      
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Check the presence/absence of KApplication2 and KApplication1 respectively
   
   @SYMTestExpectedResults KApplication1 is absent, KApplication2 is present.
 */	
void CT_IntegritySupportReboot2TestStep::TestRollbackL()
	{
	INFO_PRINTF1(_L("Checking that the device is in a consistent state, with all updates rolled back"));
	TEST(AppAbsent(iSession, KApplication1));
	TEST(AppPresent(iSession, KApplication2));
	}
