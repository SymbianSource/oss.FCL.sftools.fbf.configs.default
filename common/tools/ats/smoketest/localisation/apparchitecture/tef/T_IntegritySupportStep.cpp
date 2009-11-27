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

#include "T_IntegritySupportStep.h"
#include <bautils.h>



/**
 * Constructor
 */	
CT_IntegritySupportTestStep::CT_IntegritySupportTestStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_IntegritySupportStep);
	}

/**
 * Destructor
 */
CT_IntegritySupportTestStep::~CT_IntegritySupportTestStep()
	{
	iSession.Close();
	iUtils.Close();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */	
TVerdict CT_IntegritySupportTestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	doAbstractNonNativeAppsTestStepPreambleL();
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_IntegritySupportTestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CT_IntegritySupportTestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparc...T_IntegritySupport Test Cases Running..."));

	TRAPD(ret,RunTestCasesL())
	TEST(ret==KErrNone);
	
	INFO_PRINTF1(_L("...T_IntegritySupport Test Cases Completed."));

	return TestStepResult();
	}

TInt CT_IntegritySupportTestStep::RunTestCasesL()
	{
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestNormalInstallation1L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestManualRollback1L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestRollbackOnError1L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestRollbackOnSessionCloseL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestNormalRemoval1L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestManualRollback2L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestRollbackWithoutPrepareL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestRegisterWithoutPrepareL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestCallingPrepareTwiceL(), NO_CLEANUP); 
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestCallingPrepareFromTwoSessionsL(), NO_CLEANUP);
	/* Removed from armv5 because they seem to cause filesystem corruption */
#ifdef __WINS__
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestReregisterApplicationL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestDoubleInstallFailsL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestRollbackOnFailedUpdateStepL(), NO_CLEANUP);
#endif
	return KErrNone;
	}


_LIT(KLitLogicalExecutable1,"c:\\TestIntegritySupport1.ist");
_LIT(KLitRegistrationFile1,"c:\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\102081a1_reg.rsc");
static const CT_AbstractNonNativeAppsTestStep::TTestAppDetails KApplication1(0x102081A1,KLitLogicalExecutable1,KLitRegistrationFile1);

_LIT(KLitLogicalExecutable2,"c:\\TestIntegritySupport2.ist");
_LIT(KLitRegistrationFile2,"c:\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\102081c0_reg.rsc");
static const CT_AbstractNonNativeAppsTestStep::TTestAppDetails KApplication2(0x102081C0,KLitLogicalExecutable2,KLitRegistrationFile2);

_LIT(KLitLogicalExecutable3,"c:\\TestIntegritySupport3.ist");
_LIT(KLitRegistrationFile3,"c:\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\102081c1_reg.rsc");
static const CT_AbstractNonNativeAppsTestStep::TTestAppDetails KApplication3(0x102081C1,KLitLogicalExecutable3,KLitRegistrationFile3);

_LIT(KLitLogicalExecutable4,"c:\\TestIntegritySupport4.ist");
_LIT(KLitRegistrationFile4,"c:\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\102081c2_reg.rsc");
static const CT_AbstractNonNativeAppsTestStep::TTestAppDetails KApplication4(0x102081C2,KLitLogicalExecutable4,KLitRegistrationFile4);

// uses logical executable from KApplication4
_LIT(KLitRegistrationFile5,"c:\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\102081c3_reg.rsc");
static const CT_AbstractNonNativeAppsTestStep::TTestAppDetails KApplication5(0x102081C3,KLitLogicalExecutable4,KLitRegistrationFile5);


/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestNormalInstallation1L
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests the integrity support during a normal installation 
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Tests installation process when there are no problems.
   
   @SYMTestExpectedResults The application is successfully installed and then removed.
 */
void CT_IntegritySupportTestStep::TestNormalInstallation1L()
	{
	INFO_PRINTF1(_L("Testing normal application installation..."));

	RProcess().SetPriority(EPriorityLow); // so that we're pre-empted by the Apparc server when it does its scan
	CleanupAndReset(iSession, KApplication1);

	iSession.PrepareNonNativeApplicationsUpdatesL();

	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() "));
	CallRegisterL(iSession, KApplication1);
	
	// Since we have not committed yet, at this stage the files should not be in their
	// final locations
	TEST(AppAbsent(iSession, KApplication1));
	
	iSession.CommitNonNativeApplicationsUpdatesL();

	// the files should now be present in their final locations
	TEST(AppPresent(iSession, KApplication1));
	
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() "));
	CompleteDeregisterL(iSession, KApplication1);	

	TEST(AppAbsent(iSession, KApplication1));
	}
	

/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestManualRollback1L
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests the integrity support during an installation that is rolled back
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Tests installation process when the client decides to rollback the changes.
   
   @SYMTestExpectedResults The installtion is successfully rolled back and the device is in the
   same state as before the installation attempt.
 */
void CT_IntegritySupportTestStep::TestManualRollback1L()
	{
	INFO_PRINTF1(_L("Testing manual rollback during installation..."));
	CleanupAndReset(iSession, KApplication2);

	iSession.PrepareNonNativeApplicationsUpdatesL();
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() "));
	CallRegisterL(iSession, KApplication2);
	
	// Since we have not committed yet, at this stage the files should not be in their
	// final locations
	TEST(AppAbsent(iSession, KApplication2));
	
	iSession.RollbackNonNativeApplicationsUpdates();

	// Test that they're still all absent since we never committed
	TEST(AppAbsent(iSession, KApplication2));
	}
	
void CT_IntegritySupportTestStep::TestRollbackOnError1L()
	{
		
	// this can be tested by trying to overwrite an icon file.
	}


/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestRollbackOnSessionCloseL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests that any updates are rolled back if the session is closed in the middle
   of an update
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Tests installation process when the client closes the session before
   committing or rolling back.
   
   @SYMTestExpectedResults The installation is successfully rolled back and the device is in the
   same state as before the installation attempt.
 */	
void CT_IntegritySupportTestStep::TestRollbackOnSessionCloseL()
	{
	INFO_PRINTF1(_L("Testing rollback if RApaLsSession is closed..."));
	
	RTestableApaLsSession apaSession;
	User::LeaveIfError(apaSession.Connect());
	CleanupClosePushL(apaSession);

	CleanupAndReset(apaSession, KApplication3);

	apaSession.PrepareNonNativeApplicationsUpdatesL();
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() "));
	CallRegisterL(apaSession, KApplication3);

	// shouldn't be present until comitted
	TEST(AppAbsent(iSession, KApplication3));
	
	// We close the session before committing or rolling back.
	CleanupStack::PopAndDestroy(&apaSession);

	// We never committed, so check that nothing's changed	
	TEST(AppAbsent(iSession, KApplication3));
	}

/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestNormalRemoval1L
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests a normal removal attempt
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Tests the correct working of the removal process.
   
   @SYMTestExpectedResults The application is installed and then removed without errors.
 */	
void CT_IntegritySupportTestStep::TestNormalRemoval1L()
	{
	INFO_PRINTF1(_L("Testing removal..."));
	CleanupAndReset(iSession, KApplication4);

	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() "));
	CompleteRegisterL(iSession, KApplication4);
	TEST(AppPresent(iSession, KApplication4));
	
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() "));
	iSession.PrepareNonNativeApplicationsUpdatesL();
	CallDeregisterL(iSession,KApplication4);

	// Check that everythign is still the same as we haven't committed yet
	TEST(AppPresent(iSession, KApplication4));

	iSession.CommitNonNativeApplicationsUpdatesL();
	TEST(AppAbsent(iSession, KApplication4));
	}
	
/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestManualRollback2L
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests the integrity support during an installation that is rolled back
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Tests installation process when the client decides to rollback the changes.
   
   @SYMTestExpectedResults The installtion is successfully rolled back and the device is in the
   same state as before the installation attempt.
 */
void CT_IntegritySupportTestStep::TestManualRollback2L()
	{
	INFO_PRINTF1(_L("Testing removal rollback..."));
	CleanupAndReset(iSession, KApplication5);

	iSession.PrepareNonNativeApplicationsUpdatesL();
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() "));
	CallRegisterL(iSession, KApplication5);
	iSession.CommitNonNativeApplicationsUpdatesL();
		
	TEST(AppPresent(iSession, KApplication5));
	
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() "));
	iSession.PrepareNonNativeApplicationsUpdatesL();
	CallDeregisterL(iSession, KApplication5);

	// Check that all is still present as we haven't committed yet
	TEST(AppPresent(iSession, KApplication5));
	iSession.RollbackNonNativeApplicationsUpdates();
	
	// Check that app is in list of installed apps, still
	TEST(AppPresent(iSession, KApplication5));
	
	// Cleanup and remove app
	CompleteDeregisterL(iSession, KApplication5);
	
	// Check that app is not in list of installed apps anymore
	TEST(AppAbsent(iSession, KApplication5));
	}
	
/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestRollbackWithoutPrepareL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests that calling RollbackNonNativeApplicationsUpdates works even if 
   the client hasn't calling the PrepareNonNativeApplicationsUpdatesL function.
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Tests that calling RollbackNonNativeApplicationsUpdates works even if 
   the client hasn't calling the PrepareNonNativeApplicationsUpdatesL function.
   
   @SYMTestExpectedResults The function doesn't return an error.
 */
void CT_IntegritySupportTestStep::TestRollbackWithoutPrepareL()
	{
	INFO_PRINTF1(_L("Testing RollbackNonNativeApplicationsUpdates without prepare before..."));
	// Just call the function, if it doesn't panic, we assume it works
	iSession.RollbackNonNativeApplicationsUpdates();
	}

/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestRegisterWithoutPrepareL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests that calling RegisterNonNativeApplicationL fails if 
   PrepareNonNativeApplicationsUpdatesL hasn't been called.
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call RegisterNonNativeApplicationL without calling PrepareNonNativeApplicationsUpdatesL
   first.
   
   @SYMTestExpectedResults The function returns KErrNotReady.
 */	
void CT_IntegritySupportTestStep::TestRegisterWithoutPrepareL()
	{
	INFO_PRINTF1(_L("Testing RegisterNonNativeApplicationL without prepare before..."));
	TRAPD(err, CallRegisterL(iSession,KApplication1));
	TEST(err == KErrNotReady);
	}

/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestCallingPrepareTwiceL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests that calling PrepareNonNativeApplicationsUpdatesL twice fails.
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call PrepareNonNativeApplicationsUpdatesL twice.
   
   @SYMTestExpectedResults The second call returns KErrNotReady.
 */		
void CT_IntegritySupportTestStep::TestCallingPrepareTwiceL()
	{
	INFO_PRINTF1(_L("Testing PrepareNonNativeApplicationsUpdatesL twice..."));
	
	// Should work
	iSession.PrepareNonNativeApplicationsUpdatesL();
	
	// Shouldn't work since already called
	TRAPD(err, 	iSession.PrepareNonNativeApplicationsUpdatesL());
	TEST(err == KErrInUse);
	iSession.RollbackNonNativeApplicationsUpdates();
	
	// Should work again since we have rolled back
	iSession.PrepareNonNativeApplicationsUpdatesL();
	iSession.RollbackNonNativeApplicationsUpdates();
	}

/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestCallingPrepareFromTwoSessionsL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests that commiting an app update when another session
   has already installed it is successfull.

   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call PrepareNonNativeApplicationsUpdatesL from a session and then from
   another one.
   
   @SYMTestExpectedResults The second call returns KErrNone.
 */		
void CT_IntegritySupportTestStep::TestCallingPrepareFromTwoSessionsL()
	{
	INFO_PRINTF1(_L("Testing PrepareNonNativeApplicationsUpdatesL and other function calls from 2 sessions..."));
	INFO_PRINTF1(_L("    Cleaning up any existing reg files first..."));
	
	CleanupAndReset(iSession, KApplication1);

	INFO_PRINTF1(_L("    Running test..."));

	// open a second session
	RTestableApaLsSession session2;
	User::LeaveIfError(session2.Connect());
	CleanupClosePushL(session2);
	
	// Prepare & Register with the first session
	iSession.PrepareNonNativeApplicationsUpdatesL();
	CallRegisterL(iSession, KApplication1);

	// Prepare, Register & Deregister with the second session
	TRAPD(err,session2.PrepareNonNativeApplicationsUpdatesL());
	TEST(err == KErrNone);
	CallRegisterL(session2, KApplication1);
	
	// commit the updates with iSession, installing app 1
	TRAP(err, iSession.CommitNonNativeApplicationsUpdatesL());
	TEST(err == KErrNone);

	// both sessions should now report app 1 as installed
	TEST(AppPresent(iSession, KApplication1));
	TEST(AppPresent(session2, KApplication1));

	// Try to commit with session 2
	// This could be an upgrade of the app so completes successfully
	TRAP(err, session2.CommitNonNativeApplicationsUpdatesL());
	INFO_PRINTF2(_L("Err: %d "), err);
	TEST(err == KErrNone);

	// check that nothing was damaged
	TEST(AppPresent(iSession, KApplication1));
	TEST(AppPresent(session2, KApplication1));

	CleanupStack::PopAndDestroy(&session2);
	
	// clean up
	CompleteDeregisterL(iSession, KApplication1);
	TEST(AppAbsent(iSession, KApplication1));
	}

/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestReregisterApplicationL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Tests we can deregister an already-installed app and reregister it in
                    one transaction
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call Prepare updates, Deregister, Register, Commit
   
   @SYMTestExpectedResults All successful, no panics
 */		
void CT_IntegritySupportTestStep::TestReregisterApplicationL()
	{
	INFO_PRINTF1(_L("Testing deregister/register of an application in one transaction..."));
	INFO_PRINTF1(_L("    Cleaning up any existing reg files first..."));
	
	CleanupAndReset(iSession, KApplication1);

	INFO_PRINTF1(_L("    Registering beforehand so that the deregister has something to do..."));
	CompleteRegisterL(iSession, KApplication1);

	// test that it has installed OK
	TEST(AppPresent(iSession, KApplication1));

	INFO_PRINTF1(_L("    Running test..."));
	// Prepare, Deregister & Register
	iSession.PrepareNonNativeApplicationsUpdatesL();
	CallDeregisterL(iSession, KApplication1);
	CallRegisterL(iSession, KApplication1);
	TRAPD(err, iSession.CommitNonNativeApplicationsUpdatesL());
	TEST(err == KErrNone);

	// test that it is still installed OK
	TEST(AppPresent(iSession, KApplication1));
	
	// clean up
	CompleteDeregisterL(iSession, KApplication1);
	}

/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestDoubleInstallFailsL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc 
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions 
   
   @SYMTestExpectedResults
 */		
void CT_IntegritySupportTestStep::TestDoubleInstallFailsL()
	{
	INFO_PRINTF1(_L("Testing double register of an application in one transaction..."));
	INFO_PRINTF1(_L("    Cleaning up any existing reg files first..."));
	
	CleanupAndReset(iSession, KApplication1);

	// test that the application is absent
	TEST(AppAbsent(iSession, KApplication1));

	INFO_PRINTF1(_L("    Running test..."));
	iSession.PrepareNonNativeApplicationsUpdatesL();
	CallRegisterL(iSession, KApplication1);
	CallRegisterL(iSession, KApplication1);
	TRAPD(err, iSession.CommitNonNativeApplicationsUpdatesL());
	TEST(err == KErrAccessDenied); // Since app is already registered, KErrAccessDenied will be returned when try to delete the registered app.

	// test that the rollback happened when the second register failed,
	// and thus the app is still absent
	TEST(AppAbsent(iSession, KApplication1));
	
	// test that things still work
	TRAP(err, CompleteRegisterL(iSession, KApplication1));
	TEST(err == KErrNone);
	TEST(AppPresent(iSession, KApplication1));
	
	CompleteDeregisterL(iSession, KApplication1);
	TEST(AppAbsent(iSession, KApplication1));
	}

/**
   @SYMTestCaseID T-IntegritySupportTestStep-TestRollbackOnFailedUpdateStepL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc 
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions 
   
   @SYMTestExpectedResults
 */		
void CT_IntegritySupportTestStep::TestRollbackOnFailedUpdateStepL()
	{
	INFO_PRINTF1(_L("Testing register app1, deregister app2, then a force-failed update that causes the first two to roll back..."));
	INFO_PRINTF1(_L("    Cleaning up any existing reg files first..."));
	
	CleanupAndReset(iSession, KApplication1);
	CleanupAndReset(iSession, KApplication2);

	// test that both apps are absent
	TEST(AppAbsent(iSession, KApplication1));
	TEST(AppAbsent(iSession, KApplication2));

	// register app 2 and check
	TRAPD(err,CompleteRegisterL(iSession, KApplication2));
	TEST(err == KErrNone);
	TEST(AppAbsent(iSession, KApplication1));
	TEST(AppPresent(iSession, KApplication2));

	INFO_PRINTF1(_L("    Running test..."));
	iSession.PrepareNonNativeApplicationsUpdatesL();
	// call register/deregister/fail operations
	CallRegisterL(iSession, KApplication1);
	CallDeregisterL(iSession, KApplication2);
	iSession.AddFailingNonNativeApplicationsUpdate();
	// test that nothing has changed yet
	TEST(AppAbsent(iSession, KApplication1));
	TEST(AppPresent(iSession, KApplication2));
	// commit, expect KErrGeneral from the force-fail step
	TRAP(err, iSession.CommitNonNativeApplicationsUpdatesL());
	TEST(err == KErrGeneral);
	// make sure nothing has changed, i.e. everything rolled back ok
	TEST(AppAbsent(iSession, KApplication1));
	TEST(AppPresent(iSession, KApplication2));

	// test that things still work
	iSession.PrepareNonNativeApplicationsUpdatesL();
	CallRegisterL(iSession, KApplication1);
	CallDeregisterL(iSession, KApplication2);
	TRAP(err,iSession.CommitNonNativeApplicationsUpdatesL());
	TEST(err == KErrNone);
	TEST(AppPresent(iSession, KApplication1));
	TEST(AppAbsent(iSession, KApplication2));

	CompleteDeregisterL(iSession, KApplication1);
	TEST(AppAbsent(iSession, KApplication1));
	}
