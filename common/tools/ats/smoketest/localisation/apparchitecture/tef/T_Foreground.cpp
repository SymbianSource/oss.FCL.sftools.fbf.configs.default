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
// @file
// @internalComponent - Internal Symbian test code
// Test for UIKON GT0143 Typhoon Work Series 60 Changes
// REQ758.8: Change TApaTask::BringToForeground to send a 
// EApaSystemEventBroughtToForeground to the relevant window group
// This test requires Apparc\Tsrc\SampleApp to be built first
// The test runs in the full GUI emulator and can be launched from
// z:\system\programs\t_foreground.dll
// This test is fully automated and performs the following tasks
// - Launches SampleApp
// - Calls TApaTask::BringToForeground to send a Window Server
// event to SimpleApparcTestApp
// - Waits on a Rendezvous in the process
// - SimpleApparcTestApp handles the window server event by overriding
// CCoeAppUi::HandleSystemEventL
// - SimpleApparcTestApp's HandleSystemEventL checks the window server event
// is of type EApaSystemEventBroughtToForeground and if so signals 
// using a Rendezvous in the process
// - T_Foreground closes SimpleApparcTestApp
// This test calls TApaTask::BringToForeground on SimpleApparcTestApp to send
// a window server message to SimpleApparcTestApp
// 
//



#include <coeaui.h>
#include <coemain.h>
#include <coedef.h>
#include <coecntrl.h>
#include <basched.h>
#include <ecom.h>
#include <apparc.h>
#include <e32cmn.h>

#include <apgtask.h>

#include <apacmdln.h>
#include <apgcli.h>
#include <apgwgnam.h>
#include <e32base.h>
#include <eikenv.h>

#include "t_foreground.h"

_LIT(KAppName, "SimpleApparcTestApp");
_LIT(KAppFileName, "z:\\sys\\bin\\SimpleApparcTestApp.exe");
_LIT(KAppFile, "c:\\logs\\testApp.txt");
const TInt KNonExistantWgId = KErrNotFound;

////////////////////////////////////////////////////////////////////////////////////
//
// The CTestDriver class runs a series of tests on timer callback
//
////////////////////////////////////////////////////////////////////////////////////

CTestDriver::CTestDriver(CCoeAppUi& aAppUi) : iAppUi(aAppUi)
	{
	}

CTestDriver::~CTestDriver()
	{
	}

void CTestDriver::LoadAppL()
	{
	TBuf<256> fileNameBuf(KAppFileName);
	TFileName appName = fileNameBuf;

	CApaCommandLine* cmdLine=CApaCommandLine::NewLC();
	cmdLine->SetExecutableNameL(appName);
	cmdLine->SetCommandL(EApaCommandOpen);
	RApaLsSession ls;

	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	User::LeaveIfError(ls.StartApp(*cmdLine));
	CleanupStack::PopAndDestroy(2); // ls and cmdLine
	}

void CTestDriver::BringToForegroundL()
	{
    User::After(10000000);
	TApaTaskList taskList(CCoeEnv::Static()->WsSession());
	TApaTask task = taskList.FindApp(KAppName);

	RThread thread;
	User::LeaveIfError(thread.Open(task.ThreadId()));
	CleanupClosePushL(thread);
	RProcess process;
	User::LeaveIfError(thread.Process(process));
	CleanupClosePushL(process);

	TRequestStatus requestStatus;
	process.Rendezvous(requestStatus);
	task.BringToForeground();	
	User::WaitForRequest(requestStatus);
	CleanupStack::PopAndDestroy(&process);
	CleanupStack::PopAndDestroy(&thread);

	User::LeaveIfError(requestStatus.Int());
	}

void CTestDriver::CloseAppL()
	{
	TApaTaskList taskList(CCoeEnv::Static()->WsSession());
	TApaTask task = taskList.FindApp(KAppName);
	User::LeaveIfError(task.Exists());
	task.KillTask();
	}


/** 
   @SYMTestCaseID APPFWK-APPARC-0053
  
   @SYMDEF DEF092013 
  
   @SYMTestCaseDesc Tests Whether the TApaTask::SwitchCreateFile returns the right return code.
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions	Give a NonExistant file name that has to be created.This method checks that 
   the file does not get created but on its default implementation returns KErrNone or System Wide errors.
   		
   
   @SYMTestExpectedResults The test passes if the return value is KErrNone , It means the request has been successfuly sent to 
   the windows Server session.
*/

TInt CTestDriver::SwitchCreate()
	{
	TApaTaskList taskList(CCoeEnv::Static()->WsSession());
	TApaTask task = taskList.FindApp(KAppName);
	
	TInt errForCreate=task.SwitchCreateFile(KAppFile);
	return errForCreate;
	}


/**
   @SYMTestCaseID APPFWK-APPARC-0054. 
  
   @SYMDEF DEF092013 
  
   @SYMTestCaseDesc Tests Whether the TApaTask::SwitchOpenFile returns the right return code.
   
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions	Give a NonExistant file name that has to be Opened.This method checks that 
   the file does not get opened but on its default implementation returns KErrNone or System Wide errors.
   		
   
   @SYMTestExpectedResults The test passes if the return value is KErrNone , It means the request has been successfuly sent to 
   the windows Server session.


*/
TInt CTestDriver::SwitchOpen()
	{
	TApaTaskList taskList(CCoeEnv::Static()->WsSession());
	TApaTask task = taskList.FindApp(KAppName);
	
	TInt errForOpen=task.SwitchOpenFile(KAppFile);
	return errForOpen;
	}
	
/**
   @SYMTestCaseID T_Foreground-CloseNonExistantWindowGroupTask
  
   @SYMDEF INC086383
  
   @SYMTestCaseDesc Tests CloseNonExistantWindowGroupTask to ensure that the kernel doesn't get terminated when KillTask() is called on a non-existant window group Id.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions	First, construct TApaTask with a non-existant window group Id. Then close the app by calling KillTask().
   		API Calls:\n	
   		TApaTask::SetWgId(TInt aWgId);\n
   		TApaTask::KillTask();\n
   
   @SYMTestExpectedResults The test passes if it is able to complete without the test being killed off.
 */
void CTestDriver::CloseNonExistantWindowGroupTask()
	{
	// close the app with a non-existant window group ID
	TApaTask task(CCoeEnv::Static()->WsSession());
	task.SetWgId(KNonExistantWgId);

	// KillTask has not effect on a non existing task
	task.KillTask();
	}

class CTestDriver;


////////////////////////////////////////////////////////////////////////////////////
//
// class CTestForegroundAppUi 
//
////////////////////////////////////////////////////////////////////////////////////

CTestForegroundAppUi::CTestForegroundAppUi(CTestStep* aStep) : CTestCoeAppUi(aStep)
	{	
	}

void CTestForegroundAppUi::ConstructL()
	{
	CTestCoeAppUi::ConstructL();
	iDriver=new (ELeave) CTestDriver(*this); 
	AutoTestManager().StartAutoTest();
	}


CTestForegroundAppUi::~CTestForegroundAppUi()
	{
	delete iDriver;
	}

void CTestForegroundAppUi::RunTestStepL(TInt aNumStep)
	{
	User::After(TTimeIntervalMicroSeconds32(1000000));
	switch(aNumStep)
		{	 
		case 1:	
			{
			INFO_PRINTF1(_L("LoadingApp"));
			TRAPD(ret, iDriver->LoadAppL());
			TEST(ret==KErrNone);
			}	
			break;
		case 2:
			{
			INFO_PRINTF1(_L("TApaTask::BringToForeground"));
			TRAPD(ret, iDriver->BringToForegroundL());
			TEST(ret==KErrNone);
			}
			break;
		case 3:
			INFO_PRINTF1(_L("TApaTask::SwitchCreate"));
			TEST(iDriver->SwitchCreate()==KErrNone);
			break;
		case 4:
			INFO_PRINTF1(_L("TApaTask::SwitchOpen"));
			TEST(iDriver->SwitchOpen()==KErrNone);
			break;
		case 5:
			INFO_PRINTF1(_L("Closing App"));
			TRAPD(ret, iDriver->CloseAppL());
			TEST(ret==KErrNone);
			break;
		case 6:
			INFO_PRINTF1(_L("Closing App with an invalid window group ID"));
			//There's no return value here we can check. The system doesn't complain about closing a non existant task
			iDriver->CloseNonExistantWindowGroupTask();
			break;
		case 7:
			AutoTestManager().FinishAllTestCases(CAutoTestManager::EPass);
			break;
		default:
			break;
		}
	}


////////////////////////////////////////////////////////////////////////////////////
//
// class CTestForegroundStep
//
////////////////////////////////////////////////////////////////////////////////////

CTestForegroundStep::CTestForegroundStep()
	{
	SetTestStepName(KTestForegroundStep);
	}

CTestForegroundStep::~CTestForegroundStep()
	{
	}


void CTestForegroundStep::ConstructAppL(CCoeEnv* aCoe)
    { // runs inside a TRAP harness
	aCoe->ConstructL();
	CTestForegroundAppUi* appUi= new (ELeave) CTestForegroundAppUi(this);
    aCoe->SetAppUi(appUi);
    appUi->ConstructL();
    }


TVerdict CTestForegroundStep::doTestStepL() // main function called by E32
	{
	INFO_PRINTF1(_L("Test Started"));
	
	PreallocateHALBuffer();

	__UHEAP_MARK;

	CCoeEnv* coe=new(ELeave) CCoeEnv;
	TRAPD(err,ConstructAppL(coe));

	if (!err)
		coe->ExecuteD();
	else
		{
		SetTestStepResult(EFail);
		delete coe;
		}

	REComSession::FinalClose();	

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("Test Finished"));
	return TestStepResult();
	}
