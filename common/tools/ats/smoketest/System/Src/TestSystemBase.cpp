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
//

#include "TestSystemBase.h"

// Global Vars
_LIT(KTestExecuteLog,"c:\\Logs\\TestExecute\\smoketest_system_temp.htm");
_LIT(KPanicText,"The following Panic has occured: Thread Name = %S Exit Reason = %d Exit Category = %S");

CSystemTestBase* CSystemTestBase::NewL()
	{
	CSystemTestBase* self=new(ELeave) CSystemTestBase;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return(self);
	}

void CSystemTestBase::ConstructL()
	{
	iUndertaker=CEikUndertaker::NewL(*this);
	User::LeaveIfError(iLogger.Connect());
	User::LeaveIfError(iLogger.CreateLog(KTestExecuteLog,RTestExecuteLogServ::ELogModeAppend));
	}

CSystemTestBase::CSystemTestBase()
	{
	// do nothing
	}

CSystemTestBase::~CSystemTestBase()
	{
	delete iUndertaker;
	iLogger.Close();
	}

void CSystemTestBase::HandleThreadExitL(RThread& aThread)
	{
	TExitType exitType=aThread.ExitType();
	if (exitType==EExitPanic)
		{
		CActiveScheduler::Stop();
		iExitReason = aThread.ExitReason();
		iExitCategory = aThread.ExitCategory();
		TBuf<100> iName(aThread.FullName());
		iLogger.WriteFormat(KPanicText, &iName, iExitReason, &iExitCategory);
		User::Panic(iExitCategory,iExitReason);
		}
	}

//LOCAL_C TInt PanicThread(TAny*)
//	{
//	_LIT(PanicReason, "Just, why not");
//	TInt iPanicInt = 666;
//	User::Panic(PanicReason, iPanicInt);
//	return iPanicInt;
//	}
	
LOCAL_C void MainL()
	{
	// Construct and install the active scheduler
	CActiveScheduler*  myScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(myScheduler);
	CActiveScheduler::Install(myScheduler); 
	
	CSystemTestBase* iTempBase = CSystemTestBase::NewL();
	
	// The following is just test code that spawns a thread and makes it panic
	//RThread iPanicThread;
	//_LIT(PanicThreadTest, "Undertaker Panic Thread Test");
		
	//User::LeaveIfError(iPanicThread.Create(PanicThreadTest(), &PanicThread, KDefaultStackSize, 0x100, 0x100, NULL));
	//iPanicThread.SetPriority(EPriorityAbsoluteHigh);
	//iPanicThread.Resume();
	
	CActiveScheduler::Start();	
	
	for (;;)
		{
		User::After(100000);
		}
	}
	
GLDEF_C TInt E32Main()
/**
 * Executable Entry Point
 * Top level always creates TRAP harness.
 * Calls MainL() inside the TRAP harness
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	// should never reach this point if some thread has panicked already
	_LIT(KTestPanicReason,"Trap");
	__ASSERT_ALWAYS(!err, User::Panic(KTestPanicReason,err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}	
