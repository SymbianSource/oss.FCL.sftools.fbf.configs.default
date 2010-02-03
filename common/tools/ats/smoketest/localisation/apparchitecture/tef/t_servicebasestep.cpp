// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#include "t_servicebasestep.h"
#include <apaserverapp.h>
#include <w32std.h>
#include <appfwk_test.h>
#include "testableapalssession.h"
#include "../tef/TNonNative/TNNApp1.h"

TInt PanicTest(TAny* aOption);

// RTstServiceApp

TInt RTstServiceApp::DoTestTransferSessionL(TUid aServiceUid, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath)
	{ // static
	//create the first session object
	RTstServiceApp appServiceFirst(aServiceUid);
	CleanupClosePushL(appServiceFirst);
	//connecting first session object
	appServiceFirst.ConnectL();
	
	//create the second session object
	RTstServiceApp appServiceSecond(aServiceUid);
	//transfer session for first session object to the second session object
	appServiceSecond.TransferExistingSessionL(appServiceFirst);
	CleanupClosePushL(appServiceSecond);
	
	TRequestStatus requestStatus;
	//request for a service from the second session object
	appServiceSecond.ReceiveTestResult(requestStatus, aPassingFileByHandle, aFileNameWithoutDriveOrPath);
	User::WaitForRequest(requestStatus);
	const TInt result=User::LeaveIfError(requestStatus.Int());

	CleanupStack::PopAndDestroy(&appServiceSecond);
	CleanupStack::PopAndDestroy(&appServiceFirst);
	User::After(5 * 1000000); //Wait 5sec for Apparc update due to paging
	
	return result;
	}
	
TInt RTstServiceApp::DoTestTransferSessionAndBackL(TUid aServiceUid, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath)
	{ // static
	//create the first session object
	RTstServiceApp appServiceFirst(aServiceUid);
	CleanupClosePushL(appServiceFirst);
	//connecting first session object
	appServiceFirst.ConnectL();
	
	//create the second session object
	RTstServiceApp appServiceSecond(aServiceUid);
	//transfer session from first session object to the second session object
	appServiceSecond.TransferExistingSessionL(appServiceFirst);
	CleanupClosePushL(appServiceSecond);
	
	//transfer session back to  first session object from the second session object
	appServiceFirst.TransferExistingSessionL(appServiceSecond);
	TRequestStatus requestStatus;
	//request for a service from the first session object
	appServiceFirst.ReceiveTestResult(requestStatus, aPassingFileByHandle, aFileNameWithoutDriveOrPath);
	User::WaitForRequest(requestStatus);
	const TInt result=User::LeaveIfError(requestStatus.Int());

	CleanupStack::PopAndDestroy(&appServiceSecond);
	CleanupStack::PopAndDestroy(&appServiceFirst);
	User::After(5 * 1000000); //Wait 5sec for Apparc update due to paging
	
	return result;
	}
	
TInt RTstServiceApp::DoTestTransferWithUnconnectedSessionL(TUid aServiceUid, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath)
	{ // static
	//create the first session object
	RTstServiceApp appServiceFirst(aServiceUid);
	CleanupClosePushL(appServiceFirst);
		
	//create the second session object
	RTstServiceApp appServiceSecond(aServiceUid);
	//transfer session for first session object to the second session object
	appServiceSecond.TransferExistingSessionL(appServiceFirst);
	CleanupClosePushL(appServiceSecond);
	
	TRequestStatus requestStatus;
	//request for a service from the second session object
	appServiceSecond.ReceiveTestResult(requestStatus, aPassingFileByHandle, aFileNameWithoutDriveOrPath);
	User::WaitForRequest(requestStatus);
	const TInt result=User::LeaveIfError(requestStatus.Int());

	CleanupStack::PopAndDestroy(&appServiceSecond);
	CleanupStack::PopAndDestroy(&appServiceFirst);
	User::After(5 * 1000000); //Wait 5sec for Apparc update due to paging
	
	return result;
	}
	
RTstServiceApp::RTstServiceApp(TUid aServiceUid)
	:iServiceUid(aServiceUid)
	{
	}

void RTstServiceApp::ConnectL()
	{
	ConnectExistingByNameL(KLitServerName);
	}

void RTstServiceApp::ReceiveTestResult(TRequestStatus& aRequestStatus, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath)
	{
	SendReceive(EOpcode_receiveTestResult, TIpcArgs(aPassingFileByHandle, &aFileNameWithoutDriveOrPath), aRequestStatus);
	}

TUid RTstServiceApp::ServiceUid() const
	{
	return iServiceUid;
	}

//CT_ServiceBaseStep
	
void CT_ServiceBaseStep::ClosePanicWindowL()
	{
	RWsSession	ws;
	User::LeaveIfError(ws.Connect());

	TInt wgFocus = ws.GetFocusWindowGroup();

	const TUint32 KClientHandle = 0xFFFFFFFF;	// Events delivered to this handle are thrown away
	RWindowGroup wg = RWindowGroup(ws);

	wg.Construct(KClientHandle);
	TInt wgId = wg.Identifier();

	TWsEvent event;
	event.SetType(EEventKey);
	TKeyEvent *keyEvent = event.Key();
	keyEvent->iCode = EKeyEscape;
	keyEvent->iScanCode = EStdKeyEscape;
	keyEvent->iModifiers = 0;

	TInt limit = 0;
	for(limit = 0; wgFocus != wgId && (limit < 50); limit++)
		{
		ws.SendEventToAllWindowGroups(event);
		wgFocus = ws.GetFocusWindowGroup();
		RDebug::Print(_L("ClosePanicWindowL() - EKeyEscape sent to Windows Group"));
		}

	// close everything
	wg.Close();
	ws.Close();
	}
	
void PanicFirstInstanceAfterTransferL()
	{
	TPtrC fullFileName = _L("z:\\private\\101F289C\\GIF.NNA1");
	TPtrC fileNameWithoutDriveOrPath(TParsePtrC(fullFileName).NameAndExt());
	const TUid serviceUid = {0x10207f97};
	
	RTestableApaLsSession apparcServer;
	CleanupClosePushL(apparcServer);
	User::LeaveIfError(apparcServer.Connect());

	RFs fileServer;
	CleanupClosePushL(fileServer);
	User::LeaveIfError(fileServer.Connect());
	User::LeaveIfError(fileServer.ShareProtected());

	apparcServer.FlushRecognitionCache();
	
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(fileServer, fullFileName, EFileShareReadersOnly|EFileStream|EFileRead));
		
	TThreadId threadId2;
	TRequestStatus requestStatusForRendezvous2;
	User::LeaveIfError(apparcServer.StartDocument(file, threadId2, &requestStatusForRendezvous2));
	User::WaitForRequest(requestStatusForRendezvous2);
	
	//create the first session object
	RTstServiceApp appServiceFirst(serviceUid);
	CleanupClosePushL(appServiceFirst);
	appServiceFirst.ConnectL();
	
	//create the second session object
	RTstServiceApp appServiceSecond(serviceUid);
	//transfer session from first session object to the second session object
	appServiceSecond.TransferExistingSessionL(appServiceFirst);
	CleanupClosePushL(appServiceSecond);
	TRequestStatus requestStatus;
	//request for a service from the first session object
	appServiceFirst.ReceiveTestResult(requestStatus, ETrue, fileNameWithoutDriveOrPath);
	//code should have panicked by now
	User::WaitForRequest(requestStatus);
	const TInt result=User::LeaveIfError(requestStatus.Int());
	
	CleanupStack::PopAndDestroy(&appServiceSecond);
	CleanupStack::PopAndDestroy(&appServiceFirst);
	}
		
void PanicSecondInstanceAfterTransferBackL()
	{
	TPtrC fullFileName = _L("z:\\private\\101F289C\\GIF.NNA1");
	TPtrC fileNameWithoutDriveOrPath(TParsePtrC(fullFileName).NameAndExt());
	const TUid serviceUid = {0x10207f97};
	
	RTstServiceApp appServiceFirst(serviceUid);
	CleanupClosePushL(appServiceFirst);
	appServiceFirst.ConnectL();
	
	RTstServiceApp appServiceSecond(serviceUid);
	//transfer session from first session object to the second session object
	appServiceSecond.TransferExistingSessionL(appServiceFirst);
	CleanupClosePushL(appServiceSecond);
	
	//Transfer ownership back to first instance
	appServiceFirst.TransferExistingSessionL(appServiceSecond);
	
	TRequestStatus requestStatus;
	//request for a service from the first session object
	appServiceSecond.ReceiveTestResult(requestStatus, ETrue, fileNameWithoutDriveOrPath);
	//code should have panicked by now
	User::WaitForRequest(requestStatus);
	const TInt result=User::LeaveIfError(requestStatus.Int());
	
	CleanupStack::PopAndDestroy(&appServiceSecond);
	CleanupStack::PopAndDestroy(&appServiceFirst);
	}
	
TInt PanicTest(TAny* aOption)
	{
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}
	TInt option = (TInt)aOption;
	TInt err = KErrNone;	
	if (option == 1)
		{
		TRAP(err, PanicFirstInstanceAfterTransferL());
		}
	else
		{
		TRAP(err, PanicSecondInstanceAfterTransferBackL());
		}
		
	delete trapCleanup;
	return err;
	}	
	
void CT_ServiceBaseStep::OpenFileLC(RFile& aFile, RFs& aFileServer, const TDesC& aFullFileName)
	{
	CleanupClosePushL(aFile);
	User::LeaveIfError(aFile.Open(aFileServer, aFullFileName, EFileShareReadersOnly|EFileStream|EFileRead));
	}
	
/**
   @SYMTestCaseID APPFWK-APPARC-0092
  
   @SYMCR CR1555
  
   @SYMTestCaseDesc Transfer ownership of session from one instance of RApaAppServiceBase to another
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Start the server application.
				   Create an instance of RApaAppServiceBase and connect to server.
				   Create another instance of RApaAppServiceBase and take the ownership of the connected session.
			       Request a service from the second instance.

   @SYMTestExpectedResults Check if request has been serviced correctly.
 */	
void CT_ServiceBaseStep::TestServiceAppL(RApaLsSession& aApparcServer, RFs& aFileServer)
	{
	TPtrC fullFileName = _L("z:\\private\\101F289C\\GIF.NNA1");
	TPtrC fileNameWithoutDriveOrPath(TParsePtrC(fullFileName).NameAndExt());
	const TUid serviceUid = {0x10207f97};

	//Transfer session from one RApaAppServiceBase instance to another and request for service from the latter
	__UHEAP_MARK;
	RFile file;
	OpenFileLC(file, aFileServer, fullFileName);
	TThreadId threadId;
	TRequestStatus requestStatusForRendezvous;
	User::LeaveIfError(aApparcServer.StartDocument(file, threadId, &requestStatusForRendezvous));
	User::WaitForRequest(requestStatusForRendezvous);
	INFO_PRINTF1(_L("App started....\n"));
	TEST(RTstServiceApp::DoTestTransferSessionL(serviceUid, ETrue, fileNameWithoutDriveOrPath) == KCheckPass);
  	INFO_PRINTF1(_L("Requested service completed successfully....\n"));					
	CleanupStack::PopAndDestroy(&file);
	__UHEAP_MARKEND;
	}

/**
   @SYMTestCaseID APPFWK-APPARC-0093 APPFWK-APPARC-0095
  
   @SYMCR CR1555
  
   @SYMTestCaseDesc Testing if requesting service with the wrong instance will panic
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Start the server application.
				   Create two instances of RApaAppServiceBase and connect to server.
				   Request service with the wrong instance(which does not have the session).
   
   @SYMTestExpectedResults Requesting service with the wrong instance should raise a KERN-EXEC 0 panic.
 */	
void CT_ServiceBaseStep::TestPanicWithWrongInstanceL()
	{
	RThread thread;
	TRequestStatus stat;
	TBuf<32> threadNameBuf;
	// Give each thread a unique name to avoid KErrAlreadyExists error on thread creation
	_LIT(KThreadNameFormat, "CT_ServiceBaseStep%d");
	threadNameBuf.Format(KThreadNameFormat, 1);
	
	//Transfer session from one RApaAppServiceBase instance to another and request for service from the first, this should panic with KERN-EXEC 0
	TInt threadCreationVal = thread.Create(threadNameBuf,PanicTest,KDefaultStackSize,0x2000,0x20000,(TAny*)1);
	TEST(threadCreationVal==KErrNone);	

	TRequestStatus status;
	thread.Logon(status);
	TBool jit =	User::JustInTime();
	User::SetJustInTime(EFalse);
	thread.Resume();
	User::WaitForRequest(status);

	// we are always expecting the same panic category
	TExitCategoryName category = thread.ExitCategory();
	TEST(category.Compare(_L("KERN-EXEC")) == 0);
	INFO_PRINTF2(_L("Expected exit category:KERN-EXEC, Actual exit category:  '%S'..."), &category);

	TInt exitReason = thread.ExitReason();
	TEST(exitReason == 0);
	INFO_PRINTF2(_L("Expected exit reason: 0, Actual exit reason:, %d \n"), exitReason);
	
	thread.Close();
	
	threadNameBuf.Format(KThreadNameFormat, 2);
	//Transfer session from one RApaAppServiceBase instance to another and back, request for service from the second instance, this should panic with KERN-EXEC 0
	threadCreationVal = thread.Create(threadNameBuf,PanicTest,KDefaultStackSize,0x2000,0x20000,(TAny*)2);
	TEST(threadCreationVal==KErrNone);	

	thread.Logon(status);
	User::SetJustInTime(EFalse);
	thread.Resume();
	User::WaitForRequest(status);

	// we are always expecting the same panic category
	category = thread.ExitCategory();
	TEST(category.Compare(_L("KERN-EXEC")) == 0);
	INFO_PRINTF2(_L("Expected exit category:KERN-EXEC, Actual exit category:  '%S'..."), &category);
	
	exitReason = thread.ExitReason();
	TEST(exitReason == 0);
	INFO_PRINTF2(_L("Expected exit reason: 0, Actual exit reason:, %d \n"), exitReason);
	
	thread.Close();
	User::SetJustInTime(jit);
	ClosePanicWindowL();
	}
	
/**
   @SYMTestCaseID APPFWK-APPARC-0094
  
   @SYMCR CR1555
  
   @SYMTestCaseDesc Transfer ownership of session from one instance of RApaAppServiceBase to another and transfer back to first instance
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions	Start the server application.
				    Create an instance of RApaAppServiceBase and connect to server.
				    Create another instance of RApaAppServiceBase and take the ownership of the connected session.
				    Transfer back the ownership to the first instance.
			        Request a service from the first instance.
			        
   @SYMTestExpectedResults Check if request has been serviced correctly.
 */	
void CT_ServiceBaseStep::TestTransferBackL(RApaLsSession& aApparcServer, RFs& aFileServer)
	{
	TPtrC fullFileName = _L("z:\\private\\101F289C\\GIF.NNA1");
	TPtrC fileNameWithoutDriveOrPath(TParsePtrC(fullFileName).NameAndExt());
	const TUid serviceUid = {0x10207f97};
	
	__UHEAP_MARK;
	RFile file;
	OpenFileLC(file, aFileServer, fullFileName);
	TThreadId threadId;
	TRequestStatus requestStatusForRendezvous;
	User::LeaveIfError(aApparcServer.StartDocument(file, threadId, &requestStatusForRendezvous));
	User::WaitForRequest(requestStatusForRendezvous);
	INFO_PRINTF1(_L("App started....\n"));	
	TEST(RTstServiceApp::DoTestTransferSessionAndBackL(serviceUid, ETrue, fileNameWithoutDriveOrPath) == KCheckPass);
  	INFO_PRINTF1(_L("Requested service completed successfully....\n"));						
	CleanupStack::PopAndDestroy(&file);
	__UHEAP_MARKEND;
	}

/**
   @SYMTestCaseID APPFWK-APPARC-0097
  
   @SYMCR CR1555
  
   @SYMTestCaseDesc Transfer ownership of session from one instance of RApaAppServiceBase which is not connected to another
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions	Start the server application.
				    Create an instance of RApaAppServiceBase.
				    Create another instance of RApaAppServiceBase and take the ownership of unconnected session.
				    			        
   @SYMTestExpectedResults Check if function leaves with KErrArgument.
 */	
void CT_ServiceBaseStep::TestTransferWithUnconnectedSessionL()
	{
	TPtrC fullFileName = _L("z:\\private\\101F289C\\GIF.NNA1");
	TPtrC fileNameWithoutDriveOrPath(TParsePtrC(fullFileName).NameAndExt());
	const TUid serviceUid = {0x10207f97};
	
	__UHEAP_MARK;
	TInt err;
	TRAP(err, RTstServiceApp::DoTestTransferWithUnconnectedSessionL(serviceUid, ETrue, fileNameWithoutDriveOrPath));
	TEST(err == KErrArgument);
  	INFO_PRINTF2(_L("Expected error code: -6, Actual error code:, %d \n"), err);						
	__UHEAP_MARKEND;	
	}
	
// CTestStep derived functions
	
CT_ServiceBaseStep::~CT_ServiceBaseStep()
	{
	}

CT_ServiceBaseStep::CT_ServiceBaseStep()
	{
	SetTestStepName(KT_ServiceBaseStep);
	}

TVerdict CT_ServiceBaseStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_ServiceBaseStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CT_ServiceBaseStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Test ServiceBase starting....\n"));

	RTestableApaLsSession apparcServer;
	CleanupClosePushL(apparcServer);
	User::LeaveIfError(apparcServer.Connect());

	RFs fileServer;
	CleanupClosePushL(fileServer);
	User::LeaveIfError(fileServer.Connect());
	User::LeaveIfError(fileServer.ShareProtected());

	apparcServer.FlushRecognitionCache();

	HEAP_TEST_LS_SESSION(apparcServer, 0, DONT_CHECK, TestServiceAppL(apparcServer, fileServer), apparcServer.FlushRecognitionCache());
	HEAP_TEST_LS_SESSION(apparcServer, 0, 0, TestPanicWithWrongInstanceL(), apparcServer.FlushRecognitionCache());
	HEAP_TEST_LS_SESSION(apparcServer, 0, DONT_CHECK, TestTransferBackL(apparcServer, fileServer), apparcServer.FlushRecognitionCache());
	HEAP_TEST_LS_SESSION(apparcServer, 0, 0, TestTransferWithUnconnectedSessionL(), apparcServer.FlushRecognitionCache());
	
	CleanupStack::PopAndDestroy(2, &apparcServer);

	INFO_PRINTF1(_L("....Test ServiceBase completed!"));
	return TestStepResult();
	}
