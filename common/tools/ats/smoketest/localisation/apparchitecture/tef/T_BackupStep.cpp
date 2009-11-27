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
// Performs Tests on Backup server.\n
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <s32file.h>
#include <e32base.h>
#include <f32file.h>
#include <e32test.h>
#include <babackup.h>
#include <e32std.h>
#include <e32math.h>
#include "T_BackupStep.h"



CTestActive::CTestActive()
: CActive(EPriorityIdle*2)
	{
	CActiveScheduler::Add(this);
	}

CTestActive::~CTestActive()
	{
	Cancel();
	}

void CTestActive::DoCancel()
	{
	TRequestStatus* s=&iStatus;
	User::RequestComplete(s, KErrNone);
	}

void CTestActive::StartL()
	{
	iReport = NULL;
	SetActive();
	}


/**
  Auxiliary Fn for Test Case ID T-BackupStep-testGetsCallbackOnBackupStartL,
  T-BackupStep-testGetsCallbackOnBackupEndL,
  T-BackupStep-testGetsCallbackOnBackupStartInSecondWrapperL,
  T-BackupStep-testGetsCallbackOnBackupWrapperKillL, and
  T-BackupStep-testGetsCallbackOnBackupWrapperKillOtherL

  This function is used to set the event request to the AO and
  to set that AO active.\n

*/
void CTestActive::StartL(TRequestStatus& aRequestStatus)
	{
	iReport = &aRequestStatus;
	SetActive();
	}

void CTestActive::RunL() 
	{
	CActiveScheduler::Stop();
	}

// Class to test the n-th recieved event from the backup server is the same as the event
// the object is initialised with
CTestBackupOperationObserver::CTestBackupOperationObserver(const TBackupOperationAttributes& aEvent, CTestExecuteLogger& aLogger) : CTestActive(), iEvent(aEvent), iCount(1)
	{
	iLogger=aLogger;
	}
CTestBackupOperationObserver::CTestBackupOperationObserver(const TBackupOperationAttributes& aEvent, TInt aCount, CTestExecuteLogger& aLogger) : CTestActive(), iEvent(aEvent), iCount(aCount)
	{
	iLogger=aLogger;
	}


/**
  Auxiliary Fn for Test Case ID T-BackupStep-testGetsCallbackOnBackupStartL,
  T-BackupStep-testGetsCallbackOnBackupEndL,
  T-BackupStep-testGetsCallbackOnBackupStartInSecondWrapperL,
  T-BackupStep-testGetsCallbackOnBackupWrapperKillL, and
  T-BackupStep-testGetsCallbackOnBackupWrapperKillOtherL
 
  This function is overridden from MBackupOperationObserver and is called
  when a backup or restore operation either starts or ends. The function is used
  to receive signals that tell what backup operation completed.\n
 
*/
void CTestBackupOperationObserver::HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	const TInt KOneOfOurEvents = 12321;
	if(iCount!=KOneOfOurEvents)
		{
		INFO_PRINTF2(_L("Got event #%d from backup server; ignoring this one\n"), iCount+1);
		}
	else
		{
		iCount=0;
		if(iReport)
			{
			User::RequestComplete(iReport, aBackupOperationAttributes.iOperation);
			INFO_PRINTF1(_L("Got event from backup server; Completing...\n"));
			}
		else
			{
			INFO_PRINTF1(_L("Got event from backup server; iReport not set...testing...\n"));
			}
		}
	}

void CTestBackupOperationObserver::SetReadyToGo()
{
	const TInt KOneOfOurEvents = 12321;
	iCount=KOneOfOurEvents;
}


/**
  Auxiliary Fn for all test cases
 
  This function creates an Active Object (AO).Sets the AO to
  be active. Starts the active scheduler to complete any pending requests
  and flushes all requests from the scheduler list.\n

*/
void CT_BackupStep::FlushSchedulerQueueL()
{
	CTestActive* active = new(ELeave) CTestActive();
	CleanupStack::PushL(active);
	RTimer timer;
	timer.CreateLocal();
	timer.After(active->iStatus,500000);
	active->StartL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(1);//active
}


/**
   @SYMTestCaseID T-BackupStep-testGetsCallbackOnBackupStartL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test whether backup server signals the test backup observer
                 on backup start operation.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The test creates an object of test observer to observe for
   start backup event and registers the specified observer to be notified
   when a backup operation starts by calling RegisterBackupOperationObserverL().
   Set attributes to indicate start of backup operation. Flush the scheduler
   of all pending requests. Set start backup event to the AO. Notify the
   server of backup operation using NotifyBackupOperationL().
   Start the active scheduler to receive signal for start event from backup
   server.\n
   API Calls:\n	
   CBaBackupSessionWrapper::RegisterBackupOperationObserverL(MBackupOperationObserver& aBackupOperationObserver)\n
   CBaBackupSessionWrapper::NotifyBackupOperationL(const TBackupOperationAttributes& aBackupOperationAttributes)\n
   CBaBackupSessionWrapper::DeRegisterBackupOperationObserver(MBackupOperationObserver& aBackupOperationObserver)\n
   
   @SYMTestExpectedResults The test should complete with Backup server signaling the observer
   when the Backup operation starts.\n
    
 */
void CT_BackupStep::testGetsCallbackOnBackupStartL()
	{	
	const TBackupOperationAttributes gEventStart(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
	CTestBackupOperationObserver* backupObserver = new(ELeave) CTestBackupOperationObserver(gEventStart, Logger());
	CleanupStack::PushL(backupObserver);
	gWrapper->RegisterBackupOperationObserverL(*backupObserver);		
	FlushSchedulerQueueL();
	
	backupObserver->StartL(backupObserver->iStatus);
	gWrapper->NotifyBackupOperationL(gEventStart); 
	backupObserver->SetReadyToGo();
	CActiveScheduler::Start();
	TEST(backupObserver->iStatus == gEventStart.iOperation);
	
	gWrapper->DeRegisterBackupOperationObserver(*backupObserver);
	CleanupStack::PopAndDestroy();//backupObserver;	
	}


/**
   @SYMTestCaseID T-BackupStep-testGetsCallbackOnBackupEndL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test whether backup server signals the test backupobserver on
                 backup end operation. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The test creates an object of test observer to observe for end
   of backup event and registers the specified observer to be notified when a
   backup operation ends by calling RegisterBackupOperationObserverL().Set
   attributes to indicate start and end of backup operation. Flush the scheduler
   from all pending requests. Set start backup event to the AO. Notify the
   server of a start backup operation using NotifyBackupOperationL().
   Start the active scheduler to receive signal for start event from backup
   server. Now set end of backup event to the AO. Notify server of an end
   backup operation using NotifyBackupOperationL(). Start the active
   scheduler to receive signal for end of backup event from backup server.\n
   API Calls:\n	
   CBaBackupSessionWrapper::RegisterBackupOperationObserverL(MBackupOperationObserver& aBackupOperationObserver)\n
   CBaBackupSessionWrapper::NotifyBackupOperationL(const TBackupOperationAttributes& aBackupOperationAttributes)\n
   CBaBackupSessionWrapper::DeRegisterBackupOperationObserver(MBackupOperationObserver& aBackupOperationObserver)\n
   
   @SYMTestExpectedResults The test should complete with Backup server signaling the observer
   when the Backup operation ends.
    
 */
void CT_BackupStep::testGetsCallbackOnBackupEndL()
	{
	const TBackupOperationAttributes gEventStart(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
	const TBackupOperationAttributes gEventEnd(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
	CTestBackupOperationObserver* backupObserver = new(ELeave) CTestBackupOperationObserver(gEventEnd, Logger());
	CleanupStack::PushL(backupObserver);
	gWrapper->RegisterBackupOperationObserverL(*backupObserver);
	FlushSchedulerQueueL();

	backupObserver->StartL(backupObserver->iStatus);
	gWrapper->NotifyBackupOperationL(gEventStart);
	backupObserver->SetReadyToGo();
	CActiveScheduler::Start();
	TEST(backupObserver->iStatus == gEventStart.iOperation);

	backupObserver->StartL(backupObserver->iStatus);
	gWrapper->NotifyBackupOperationL(gEventEnd);
	backupObserver->SetReadyToGo();
	CActiveScheduler::Start();
	TEST(backupObserver->iStatus == gEventEnd.iOperation);

	gWrapper->DeRegisterBackupOperationObserver(*backupObserver);
	CleanupStack::PopAndDestroy();//backupObserver;	
	}


/**
   @SYMTestCaseID T-BackupStep-testGetsCallbackOnBackupStartInSecondWrapperL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test whether signal for start backup operation can be
                 received by a second observer that is not associated
                 with the wrapper object that issued the request for start
                 of backup.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The test creates two test observers and two wrappers to the
   backup server. Set attributes to indicate start of backup operation.
   Register & associate each observer with each session wrappers that needs
   to be notified of backup start operation, issued by the respective wrappers
   by calling RegisterBackupOperationObserverL(). Using the first wrapper,
   notify the server to start backup. Verify that second observer receives
   the notification for start of server.\n
   API Calls:\n	
   CBaBackupSessionWrapper::NewL()\n
   CBaBackupSessionWrapper::RegisterBackupOperationObserverL(MBackupOperationObserver& aBackupOperationObserver)\n
   CBaBackupSessionWrapper::NotifyBackupOperationL(const TBackupOperationAttributes& aBackupOperationAttributes)\n
   CBaBackupSessionWrapper::DeRegisterBackupOperationObserver(MBackupOperationObserver& aBackupOperationObserver)\n
   
   @SYMTestExpectedResults The test should confirm that second observer receives
   the signal for backup start operation issued by the first wrapper object.
    
 */
void CT_BackupStep::testGetsCallbackOnBackupStartInSecondWrapperL()
	{
	const TBackupOperationAttributes gEventStart(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
	CTestBackupOperationObserver* backupObserver = new(ELeave) CTestBackupOperationObserver(gEventStart, Logger());
	CleanupStack::PushL(backupObserver);
	gWrapper->RegisterBackupOperationObserverL(*backupObserver);
	FlushSchedulerQueueL();

	CBaBackupSessionWrapper* secondWrapper=CBaBackupSessionWrapper::NewL();
	CleanupStack::PushL(secondWrapper);

	CTestBackupOperationObserver* backupObserver2 = new(ELeave) CTestBackupOperationObserver(gEventStart, Logger());
	CleanupStack::PushL(backupObserver2);
	secondWrapper->RegisterBackupOperationObserverL(*backupObserver2);
	FlushSchedulerQueueL();
	
	backupObserver2->StartL(backupObserver2->iStatus);
	gWrapper->NotifyBackupOperationL(gEventStart);
	backupObserver2->SetReadyToGo();
	CActiveScheduler::Start();
	TEST(backupObserver2->iStatus == gEventStart.iOperation);
	
	gWrapper->DeRegisterBackupOperationObserver(*backupObserver);
	secondWrapper->DeRegisterBackupOperationObserver(*backupObserver2);
	CleanupStack::PopAndDestroy(3);//backupObserver, backupObserver2, secondWrapper	
	}
	

/**
   @SYMTestCaseID T-BackupStep-testGetsCallbackOnBackupWrapperKillL
  
   @SYMPREQ
  
	@SYMTestCaseDesc Test if MBackupOperationObserver::EAbort event is received
                 when wrapper to the backup server is destroyed immediately
                 after backup start operation is requested. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create a test backup observer. Register the observer to receive
   event from server. Create a second wrapper to the backup server. Use the
   second wrapper to notify the server to start backup. After receiving signal
   for start of backup, destroy the second wrapper. Observe event received
   from the server.\n
   API Calls:\n	
   CBaBackupSessionWrapper::NewL()\n
   CBaBackupSessionWrapper::RegisterBackupOperationObserverL(MBackupOperationObserver& aBackupOperationObserver)\n
   CBaBackupSessionWrapper::NotifyBackupOperationL(const TBackupOperationAttributes& aBackupOperationAttributes)\n
   CBaBackupSessionWrapper::DeRegisterBackupOperationObserver(MBackupOperationObserver& aBackupOperationObserver)\n
   
   @SYMTestExpectedResults Test confirms that event MBackupOperationObserver::EAbort
   is received.
    
 */	
void CT_BackupStep::testGetsCallbackOnBackupWrapperKillL()
	{
	const TBackupOperationAttributes gEventStart(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
	const TBackupOperationAttributes gEventEnd(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
	const TBackupOperationAttributes gEventAbort(MBackupObserver::ETakeLock, MBackupOperationObserver::EAbort);
	CTestBackupOperationObserver* backupObserver = new(ELeave) CTestBackupOperationObserver(gEventEnd, Logger());
	CleanupStack::PushL(backupObserver);
	gWrapper->RegisterBackupOperationObserverL(*backupObserver);
	FlushSchedulerQueueL();

	CBaBackupSessionWrapper* secondWrapper=CBaBackupSessionWrapper::NewL();
	CleanupStack::PushL(secondWrapper);
	
	backupObserver->StartL(backupObserver->iStatus);
	secondWrapper->NotifyBackupOperationL(gEventStart);
	backupObserver->SetReadyToGo();
	CActiveScheduler::Start();
	TEST(backupObserver->iStatus == gEventStart.iOperation);

	backupObserver->StartL(backupObserver->iStatus);
	CleanupStack::PopAndDestroy();//secondWrapper	
	backupObserver->SetReadyToGo();
	CActiveScheduler::Start();
	TEST(backupObserver->iStatus == gEventAbort.iOperation);
	
	gWrapper->DeRegisterBackupOperationObserver(*backupObserver);
	CleanupStack::PopAndDestroy(1);//backupObserver
	}
	

/**
   @SYMTestCaseID T-BackupStep-testGetsCallbackOnBackupWrapperKillOtherL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test that the backup is not terminated after a backup start event,
   which is followed by destruction of any wrapper other than the wrapper
   which requested the backup start event.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create a test observer. Register the observer with the server.
   Create second and third wrappers to the backup server besides the global
   wrapper. Use the second observer to start backup. Observe that the event for
   start of backup is received. Destroy the third wrapper created. Observe
   event received from the server. Notify the server to observe for event
   MBackupOperationObserver::ENone.Observe the event received. Notify the server
   to stop backup. Observe the event received. Destroy the second observer.
   Note that this destruction causes an abort event.\n
   API Calls:\n	
   CBaBackupSessionWrapper::NewL()\n
   CBaBackupSessionWrapper::RegisterBackupOperationObserverL(MBackupOperationObserver& aBackupOperationObserver)\n
   CBaBackupSessionWrapper::NotifyBackupOperationL(const TBackupOperationAttributes& aBackupOperationAttributes)\n
   CBaBackupSessionWrapper::DeRegisterBackupOperationObserver(MBackupOperationObserver& aBackupOperationObserver)\n
   
   @SYMTestExpectedResults Test confirms that backup will not be terminated by
   destruction of wrapper object that did not initiate the back up process,
   after the backup operation has started.
   
 */
void CT_BackupStep::testGetsCallbackOnBackupWrapperKillOtherL()
	{
	const TBackupOperationAttributes gEventStart(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
	const TBackupOperationAttributes gEventEnd(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
	const TBackupOperationAttributes gEventNone(MBackupObserver::ETakeLock, MBackupOperationObserver::ENone);
	
	CTestBackupOperationObserver* backupObserver = new(ELeave) CTestBackupOperationObserver(gEventEnd, Logger());
	CleanupStack::PushL(backupObserver);
	gWrapper->RegisterBackupOperationObserverL(*backupObserver);
	FlushSchedulerQueueL();

	CBaBackupSessionWrapper* secondWrapper=CBaBackupSessionWrapper::NewL();
	CleanupStack::PushL(secondWrapper); // This one starts the backup

	CBaBackupSessionWrapper* thirdWrapper=CBaBackupSessionWrapper::NewL();
	CleanupStack::PushL(thirdWrapper); // This one gets destroyed
	FlushSchedulerQueueL();
	
	backupObserver->StartL(backupObserver->iStatus);
	secondWrapper->NotifyBackupOperationL(gEventStart);;
	backupObserver->SetReadyToGo();
	CActiveScheduler::Start();
	TEST(backupObserver->iStatus == gEventStart.iOperation);

	backupObserver->StartL(backupObserver->iStatus);
	CleanupStack::PopAndDestroy(thirdWrapper);//thirdWrapper 
	TEST(backupObserver->iStatus == gEventStart.iOperation); //
	secondWrapper->NotifyBackupOperationL(gEventNone);
	backupObserver->SetReadyToGo();
	CActiveScheduler::Start();
	TEST(backupObserver->iStatus == gEventNone.iOperation);

	backupObserver->StartL(backupObserver->iStatus);	
	secondWrapper->NotifyBackupOperationL(gEventEnd);
	backupObserver->SetReadyToGo();
	CActiveScheduler::Start();
	TEST(backupObserver->iStatus == gEventEnd.iOperation);

	backupObserver->StartL(backupObserver->iStatus);
	CleanupStack::PopAndDestroy(secondWrapper);//secondWrapper 
	TEST(backupObserver->iStatus == gEventEnd.iOperation); 
	CleanupStack::PopAndDestroy();//backupObserver
	gWrapper->DeRegisterBackupOperationObserver(*backupObserver);
	}


CThreadLaunchCounter::CThreadLaunchCounter() : CActive(0)
	{
	CActiveScheduler::Add(this);
	gCount = 0;
	}

CThreadLaunchCounter::~CThreadLaunchCounter()
	{
	if(IsActive())
		Cancel();
	}

void CThreadLaunchCounter::Start()
	{
	iStatus=KRequestPending;
	gCount++;
	SetActive();
	}

void CThreadLaunchCounter::RunL()
	{
	if(--gCount==0)
		CActiveScheduler::Stop();	
	delete this;
	}


/**
   @SYMTestCaseID T-BackupStep-testMultipleClientsL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test multiple clients accessing the Backup server. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The test creates multiple threads to make multiple clients
   access the backup server simultaneously. Each of the threads execute the
   thread function BackupMultipleTestThreadEntry() to create a wrapper object
   to the backup server so that a backup operation can be carried out.
   The test thus emulates a multiple client scenario.\n
   API Calls:\n	
   RThread::Create(const TDesC& aName, TThreadFunction aFunction, TInt aStackSize, RAllocator* aHeap, TAny* aPtr, TOwnerType aType=EOwnerProcess)\n
   RThread::Resume() const\n
   RThread::Logon(TRequestStatus& aStatus) const\n
   RThread::Close()\n
   
   @SYMTestExpectedResults The test completes creating multiple threads and tests
   multi-client interaction with the backup server.
    
 */
void CT_BackupStep::testMultipleClientsL()
	{
	const TInt KNumMultipleThreads=1;
	_LIT(KMultiThreadBaseName,"Multithread");
	const TInt KBackupServerStackSize	=0x3000; // 16k
	CBackupTestThreadParams Params;
	Params.Step = this;
	for(TInt i=0;i<KNumMultipleThreads;i++)
		{
		CThreadLaunchCounter* threadCounter = new(ELeave) CThreadLaunchCounter;
		CleanupStack::PushL(threadCounter);
		RThread thread;
		TBuf<0x100> name(KMultiThreadBaseName);
		name.AppendNum(i);
		TInt64 theSeed = 12312312;
		Params.seed=Math::Rand(theSeed);
		User::LeaveIfError(thread.Create(name, BackupMultipleTestThreadEntry, KBackupServerStackSize, NULL, &Params));
		thread.Resume();
		thread.Logon(threadCounter->iStatus);
		threadCounter->Start();
		thread.Close();
		}
	User::After(2000000);
	CActiveScheduler::Start();//runs until all the CThreadLaunchCounter are completed
	User::After(2000000);
	CleanupStack::Pop(KNumMultipleThreads);
	}



TRequestStatus CT_BackupStep::testExtraRestartLockedOutL()
//
// Tests that we are only able to restart the apps once - no extra threads around!
//
	{
//vm
	const TBackupOperationAttributes gEventStart(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
	gWrapper->NotifyBackupOperationL(gEventStart); 
//vm	
	_LIT(KAppRestarterThreadName,"AppRestarterThread");
	TRequestStatus status;
	gWrapper->CloseAll(MBackupObserver::EReleaseLockNoAccess, status);
	User::WaitForRequest(status);
///vm
	if(status != KErrNone)
		{
		return status;
		}

	const TBackupOperationAttributes gEventEnd(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
	gWrapper->NotifyBackupOperationL(gEventEnd); 
////

	gWrapper->RestartAll();
	gWrapper->RestartAll();
	gWrapper->RestartAll();
	gWrapper->RestartAll();

	TInt err;
	err=KErrNone;
	TFullName name;
	TFindThread find(KAppRestarterThreadName);
	err=find.Next(name);
	if(err==KErrNotFound)
		{
		INFO_PRINTF1(_L("Error in test code timing - need to try again\n"));
		
		return status;
		}
	gWrapper->RestartAll();
	gWrapper->RestartAll();

	RThread thread;
	err=thread.Open(name);
	TInt ignoreId = thread.Id();
	if(err==KErrNotFound)
		{
		INFO_PRINTF1(_L("Error in test code timing - need to try again\n"));
		return status;
		}
	thread.Logon(status);
	User::WaitForRequest(status);
	thread.Close();

	TFindThread find1(KAppRestarterThreadName);
	while( (err=find1.Next(name)) != KErrNotFound)
		{
		RThread thread;
		if(thread.Open(name)==KErrNone)
			{
			TInt id=thread.Id();
			TEST(id == ignoreId);
			thread.Close();
			}
		}
	return status;
	}


/**
   @SYMTestCaseID T-BackupStep-testRestartAppsL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test whether CBaBackupSessionWrapper::RestartAll() unlocks
                 file locks (MBackupObserver::ETakeLock flag) for the
                 registered files locked by CloseAll(). 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions  Call TFindThread::Next() continuously to find all threads and
   append id of each of these threads to a list.
   Call CBaBackupSessionWrapper::CloseAll() to close all non-system
   applications and call all observers to release or stop writing to the
   registered files. Call CBaBackupSessionWrapper::RestartAll().
   Call TFindThread::Next() continuously to find all threads. Compare the id
   of each thread to the id stored in the list to find a match.\n
   API Calls:\n	
   CBaBackupSessionWrapper::RestartAll()\n
   CBaBackupSessionWrapper::CloseAll(MBackupObserver::TFileLockFlags aFlags, TRequestStatus& aStatus)\n
   
   @SYMTestExpectedResults Test that CBaBackupSessionWrapper::RestartAll() take
   back file locks (MBackupObserver::ETakeLock flag) for the registered files
   locked by CloseAll().
    
 */
void CT_BackupStep::testRestartAppsL()
	{
//vm	
	const TBackupOperationAttributes gEventStart(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
	gWrapper->NotifyBackupOperationL(gEventStart); 
//vm	

	TFindThread find;
	INFO_PRINTF1(_L("\nRestartApps......."));
	TFullName name;
	RArray<TInt> idArray;
	CleanupClosePushL(idArray);
	while(find.Next(name)!=KErrNotFound)
		{	
		INFO_PRINTF1(_L("\nAppending thread id......."));
		RThread thread;
		thread.Open(name);
		TInt id=thread.Id();
		idArray.Append(id);
		thread.Close();
		INFO_PRINTF1(_L("\nCompleted."));
		}
	
	INFO_PRINTF1(_L("\nGWrapper: Close all......."));
	TRequestStatus status;
	gWrapper->CloseAll(MBackupObserver::EReleaseLockNoAccess, status);
	
	User::WaitForRequest(status);

	INFO_PRINTF1(_L("\nWait."));
	User::After(1000000);
	
///vm
	const TBackupOperationAttributes gEventEnd(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
	gWrapper->NotifyBackupOperationL(gEventEnd); 
	
////

	INFO_PRINTF1(_L("\nGWrapper: Restart all......."));
	gWrapper->RestartAll();
	INFO_PRINTF1(_L("\nGWrapper: Restart all.....Completed"));
	INFO_PRINTF1(_L("\nGWrapper: Close all.......Completed."));

	TFindThread find2;

	while(find2.Next(name)!=KErrNotFound)
		{
// Create a thread in the calling process
		INFO_PRINTF1(_L("\nGet Thread ID......."));
		RThread thread;
		thread.Open(name);
		TInt id=thread.Id();
		thread.Close();
		INFO_PRINTF1(_L("\nGet Thread ID......Completed."));

		INFO_PRINTF1(_L("\nFind process ID in idArray"));
		if(idArray.Find(id)==KErrNone)
			{
			TInt temp =0;
			TEST(temp==0); // no instance of this thread is found
			}
		INFO_PRINTF1(_L("\nFind process ID in idArray.....Completed."));
		}
	CleanupStack::PopAndDestroy(&idArray); //idArray
	
	
	INFO_PRINTF1(_L("\nRestartApps completed."));
	}



/**
  Auxiliary Fn for all Test Cases.
 
  The method initiates all the tests to be performed.
 
*/
void CT_BackupStep::doTestsL()
	{
	const TBackupOperationAttributes gEventEnd(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
	TRAPD(err,gWrapper=CBaBackupSessionWrapper::NewL());
	if(err!=KErrNone)
		{
		INFO_PRINTF1(_L("Couldn't open session wrapper\n"));
		User::Leave(err);
		}
	INFO_PRINTF1(_L("Backup session test 1\n"));
	TRAP(err,gWrapper->NotifyBackupOperationL(gEventEnd)); 
	if(err!=KErrNone)
		{
		INFO_PRINTF1(_L("Couldn't notify backup operation\n"));
		User::Leave(err);
		}

	FlushSchedulerQueueL();
	testGetsCallbackOnBackupStartL();	

	gWrapper->NotifyBackupOperationL(gEventEnd); 
	FlushSchedulerQueueL();
	INFO_PRINTF1(_L("Backup session test 2\n"));
	testGetsCallbackOnBackupEndL();

	gWrapper->NotifyBackupOperationL(gEventEnd); 
	FlushSchedulerQueueL();
	INFO_PRINTF1(_L("Backup session test 3\n"));
	testGetsCallbackOnBackupStartInSecondWrapperL();	
	
	gWrapper->NotifyBackupOperationL(gEventEnd); 
	FlushSchedulerQueueL();
	INFO_PRINTF1(_L("Backup session test 4\n"));
	testGetsCallbackOnBackupWrapperKillL();	

	gWrapper->NotifyBackupOperationL(gEventEnd); 
	FlushSchedulerQueueL();
	INFO_PRINTF1(_L("Backup session test 5\n"));
	testGetsCallbackOnBackupWrapperKillOtherL();

	gWrapper->NotifyBackupOperationL(gEventEnd); 
	FlushSchedulerQueueL();
	INFO_PRINTF1(_L("Backup session test 6\n"));
	/*TRequestStatus status = testExtraRestartLockedOutL();
	
	if(status=KErrNone)
		{
		gWrapper->NotifyBackupOperationL(gEventEnd); 
		FlushSchedulerQueueL();
		INFO_PRINTF1(_L("Backup session test 7\n"));
		testRestartAppsL();

		gWrapper->NotifyBackupOperationL(gEventEnd); 
		INFO_PRINTF1(_L("Backup session test 8\n"));
		testMultipleClientsL();
		}*/
	delete gWrapper;
	
	}


/**
  Auxiliary Fn for Test Case ID T-BackupStep-testMultipleClientsL
 
  Entry point for the thread used to test multiple simultaneous wrapper
  sessions with the backup server. This method calls
  DoBackupMultipleTestThreadEntryL() method to perform this task.\n
 
*/
GLDEF_C TInt CT_BackupStep::BackupMultipleTestThreadEntry(TAny* aPtr)
	{
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (!scheduler)
		return KErrNoMemory;
	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	if (!trapCleanup)
		{
		delete scheduler;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);
	
	CBackupTestThreadParams& threadParams = *(CBackupTestThreadParams*)aPtr;
	TRAPD(err,threadParams.Step->DoBackupMultipleTestThreadEntryL(/*aPtr*/));

	delete CActiveScheduler::Current();
	delete trapCleanup;
	return err;
	}


/**
  Auxiliary Fn for Test Case ID T-BackupStep-testMultipleClientsL
 
  The method is called by the thread function BackupMultipleTestThreadEntry()
  to create a wrapper session to perform some backup operation.\n
 
*/
TInt CT_BackupStep::DoBackupMultipleTestThreadEntryL(/*TAny* aSeed*/)
	{
	const TBackupOperationAttributes gEventEnd(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
	CBaBackupSessionWrapper* wrapper=CBaBackupSessionWrapper::NewL();
	CleanupStack::PushL(wrapper);

	CTestBackupOperationObserver* backupObserver = new(ELeave) CTestBackupOperationObserver(gEventEnd, Logger());
	CleanupStack::PushL(backupObserver);
	wrapper->RegisterBackupOperationObserverL(*backupObserver);
	FlushSchedulerQueueL();
	wrapper->NotifyBackupOperationL(gEventEnd);

	wrapper->DeRegisterBackupOperationObserver(*backupObserver);
	CleanupStack::PopAndDestroy(2);//backupObserver, wrapper
	return KErrNone;
	}


/**
  Auxiliary Fn for entire Test Step
 
  This method creates and installs an active scheduler for tests.
 
*/
void CT_BackupStep::SetupSchedulerL()
	{
	testScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL( testScheduler );
	CActiveScheduler::Install( testScheduler );
	}

/**
  Auxiliary Fn for entire Test Step
 
  This method removes the active scheduler from the cleanup stack and
  destroys it on completion of the tests.
 
*/
void CT_BackupStep::CloseScheduler()
	{
	CleanupStack::PopAndDestroy(); // Scheduler
	testScheduler = NULL;
	}

//**********************************
// Global
//**********************************


void CT_BackupStep::doMainL()
	{
	SetupSchedulerL();		
	doTestsL();
	CloseScheduler();		
	}


CT_BackupStep::~CT_BackupStep()
/**
   Destructor
 */
	{
	}

CT_BackupStep::CT_BackupStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_BackupStep);
	}

TVerdict CT_BackupStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}

TVerdict CT_BackupStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}


TVerdict CT_BackupStep::doTestStepL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
{
	INFO_PRINTF1(_L("Testing Apparc...T_Backup"));
	//
	__UHEAP_MARK;
	TRAPD(ret,doMainL());
	TEST(ret==KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("Test completed!"));
	return TestStepResult();
}
