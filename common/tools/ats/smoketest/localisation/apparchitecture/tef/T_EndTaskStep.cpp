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
// The following test cases test the funtionality of RuleBased plugins framework.
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <apacmdln.h>
#include <apgwgnam.h>

#include <appfwk_test.h>
#include "T_EndTaskStep.h"
#include "TEndTaskTestApp\EndTaskTestAppExternalInterface.h"

CTEndTaskStep::CTEndTaskStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_EndTaskStep);		
	}

CTEndTaskStep::~CTEndTaskStep()
	{
	}

void CTEndTaskStep::ExecuteL()
	{
	RApaLsSession theLs;
	User::LeaveIfError(theLs.Connect());
	CleanupClosePushL(theLs);

	User::LeaveIfError(iWs.Connect());
	CleanupClosePushL(iWs);
	
	EndTaskTest1L(theLs);
	EndTaskTest2L(theLs);
	EndTaskTest3L(theLs);
	
	CleanupStack::PopAndDestroy();	//iWs.Close();
	CleanupStack::PopAndDestroy(&theLs);
	}

/**
@SYMTestCaseID 		APPFWK-APPARC-0057  

@SYMDEF 			PDEF092171   

@SYMTestCaseDesc 	Test case for defect "TApaTask::EndTask() allows any application kill any task"

@SYMTestPriority 	High   

@SYMTestStatus 		Implemented

@SYMTestActions 	Launch and close both a standard and a system application using TApaTask::EndTask().

@SYMTestExpectedResults The app should be launched and closed accordingly
*/
void CTEndTaskStep::EndTaskTest1L(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Test case 1 started"));
	
	// Launch App
	const TUid appUid = {KUidEndTaskAppUid};
	TEST(KErrNone == LaunchAppL(aLs, appUid));

	// Get a handle to App
	TApaTaskList taskList(iWs);
	TApaTask task1 = taskList.FindApp(appUid);
	TEST(task1.Exists());

	// Close it
	task1.EndTask();
	User::After(500000);
	TApaTask refresh1 = taskList.FindApp(appUid); 
	TEST(!refresh1.Exists());

	//
	// Now do the same again but with a system app
	//
	
	// Launch App
	TEST(KErrNone == LaunchAppL(aLs, appUid));

	// Get a handle to App
	TApaTask task2 = taskList.FindApp(appUid);
	TEST(task2.Exists());

	// Mark as a system app
	TWsEvent event;
	event.SetType(EEndTaskTestAppSetSystem);
	iWs.SendEventToWindowGroup(task2.WgId(), event);

	// Close it
	task2.EndTask();
	User::After(500000);
	TApaTask refresh2 = taskList.FindApp(appUid); 
	TEST(!refresh2.Exists());
	
	INFO_PRINTF1(_L("Test case 1 finished"));
	}


/**
@SYMTestCaseID 		APPFWK-APPARC-0058

@SYMDEF 			PDEF092171   

@SYMTestCaseDesc 	Test case for defect "TApaTask::EndTask() allows any application kill any task"

@SYMTestPriority 	High   

@SYMTestStatus 		Implemented

@SYMTestActions 	Try to close a systemapp from a process that do not have PwrMgmt

@SYMTestExpectedResults The app should be closed accordingly
*/
void CTEndTaskStep::EndTaskTest2L(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Test case 2 started"));
	
	// Launch App
	const TUid appUid = {KUidEndTaskAppUid};
	TEST(KErrNone == LaunchAppL(aLs, appUid));

	// Get a handle to App
	TApaTaskList taskList(iWs);
	TApaTask task = taskList.FindApp(appUid);
	TEST(task.Exists());

	// Mark as a system task
	TWsEvent event;
	event.SetType(EEndTaskTestAppSetSystem);
	iWs.SendEventToWindowGroup(task.WgId(), event);

	// Call TApaTask::EndTask() on the systemtask from an arbitrary process which don't have PwrMgmt
	event.SetType(ECallEndTaskWithoutPwrMgmt);
	APPFWK_NEGATIVE_PLATSEC_START;
	iWs.SendEventToWindowGroup(task.WgId(), event);
	APPFWK_NEGATIVE_PLATSEC_FINISH;
	User::After(500000);
	TApaTask refresh = taskList.FindApp(appUid); 
	TEST(refresh.Exists()); // application should have stayed open
	
	// Close app from this process (which do have PwrMgmt)
	task.EndTask();
	User::After(500000);
	TApaTask refresh2 = taskList.FindApp(appUid); 
	TEST(!refresh2.Exists());
	
	INFO_PRINTF1(_L("Test case 2 finished"));
	}

/**
@SYMTestCaseID 		APPFWK-APPARC-0059

@SYMDEF 			PDEF092171   

@SYMTestCaseDesc 	Test case for defect "TApaTask::EndTask() allows any application kill any task"

@SYMTestPriority 	High   

@SYMTestStatus 		Implemented

@SYMTestActions 	Launch and close application using wserv APIs (way of faking new secure close-message)

@SYMTestExpectedResults This test should pass if the process doesn't have PowerMgmt
*/
void CTEndTaskStep::EndTaskTest3L(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Test case 3 started"));
	
	// Launch App
	const TUid appUid = {KUidEndTaskAppUid};
	TEST(KErrNone == LaunchAppL(aLs, appUid));

	// Get a handle to App
	TApaTaskList taskList(iWs);
	TApaTask task = taskList.FindApp(appUid);
	TEST(task.Exists());

	// Mark as a system app
	TWsEvent event;
	event.SetType(EEndTaskTestAppSetSystem);
	iWs.SendEventToWindowGroup(task.WgId(), event);

	// First variant should be sorted by CCoeAppUi::HandleWsEventL
	event.SetType(ESimulateHackerAttack1);
	iWs.SendEventToWindowGroup(task.WgId(), event);
	User::After(500000);
	TApaTask refresh = taskList.FindApp(appUid); 
	TEST(refresh.Exists());
	
	if(refresh.Exists())
		{
		// Second variant should be handled by wserv's Client::CommandL
		event.SetType(ESimulateHackerAttack2);
		APPFWK_NEGATIVE_PLATSEC_START;
		iWs.SendEventToWindowGroup(task.WgId(), event);
		APPFWK_NEGATIVE_PLATSEC_FINISH;
		User::After(500000);
		TApaTask refresh2 = taskList.FindApp(appUid); 
		TEST(refresh2.Exists());
		
		if(refresh2.Exists())
			{
			// Third variant should also be handled by wserv's Client::CommandL in another switch-case
			event.SetType(ESimulateHackerAttack3);
			APPFWK_NEGATIVE_PLATSEC_START;
			iWs.SendEventToWindowGroup(task.WgId(), event);
			APPFWK_NEGATIVE_PLATSEC_FINISH;
			User::After(500000);
			TApaTask refresh3 = taskList.FindApp(appUid); 
			TEST(refresh3.Exists());

			if(refresh3.Exists())
				{
				// Remove system property
				event.SetType(EEndTaskTestAppSetNormal);
				iWs.SendEventToWindowGroup(task.WgId(), event);
				
				// Close app 
				task.EndTask();
				User::After(500000);
				TApaTask refresh5 = taskList.FindApp(appUid); 
				TEST(!refresh5.Exists());
				}
			}
		}
	
	INFO_PRINTF1(_L("Test case 3 finished"));
	}
	
/**
The function is used to launch an app whose UID is supplied as its second input parameter.
The function returns KErrNone upon success, KErrGeneral otherwise.
*/
TInt CTEndTaskStep::LaunchAppL(RApaLsSession& aLs, const TUid& aAppUid)
	{	
	TApaAppInfo info; 
	TFileName fileName;
	User::LeaveIfError(aLs.GetAppInfo(info, aAppUid));
	
	//Wait 1 sec for function to complete
	User::After(1000000);
	fileName = info.iFullName; 
	CApaCommandLine* cmdLn = CApaCommandLine::NewLC();     
	cmdLn->SetDocumentNameL(KNullDesC);					//need to start without resource file
	cmdLn->SetExecutableNameL(fileName);
	TInt result = aLs.StartApp(*cmdLn);
	//Wait 1 sec for App to start
	User::After(1000000);
	CleanupStack::PopAndDestroy(cmdLn);
	return result;
	}

TVerdict CTEndTaskStep::doTestStepL()
	{
	INFO_PRINTF1(_L("TEndTaskStep test started...."));
	
	__UHEAP_MARK;			
	TRAPD(err, ExecuteL()); 	
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("execute tests ended with return value '%d'"), err);
		TEST(EFalse);
		}
	__UHEAP_MARKEND; 			
	
	INFO_PRINTF1(_L(".... TEndTaskStep test finished!!"));
	return TestStepResult();
	}
