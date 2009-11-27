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
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "T_StartAppStep.h"
#include "tstapp.h"
#include "TRApaLsSessionStartAppTest.h"
#include <apacmdln.h>

_LIT(KCompleted, "Completed.");
_LIT8(KLitPlainText,"text/plain");

const TInt KTUnProtectedAppTestPassed = 1234;

class RIpcApparcFuzzTest : public RSessionBase
	{
public: 
	RIpcApparcFuzzTest(CTestStep& aTestFuzz);// Constructor
	~RIpcApparcFuzzTest();//Destructor
	void RunFuzzTestL();
private:
	CTestStep&  iTestFuzz;
	};
	
RIpcApparcFuzzTest::RIpcApparcFuzzTest(CTestStep& aTestFuzz):iTestFuzz(aTestFuzz) 
	{
	}
	
RIpcApparcFuzzTest::~RIpcApparcFuzzTest() 
	{
	Close();
	}
/**
   @SYMTestCaseID APPFWK-APPARC-0086
 
   @SYMDEF DEF116002 IpcFuzz Test on CApaAppListServSession::GetExecutableNameGivenDataTypeL and AppForDataTypeL
    
   @SYMTestPriority Medium 
 
   @SYMTestStatus Implemented
  
   @SYMTestActions Create AppListServer server session and pass the HBufC descriptor to the 
   apparc server APIs GetExecutableNameGivenDataTypeL() and AppForDataTypeL().

   @SYMTestExpectedResults Test should return  KErrArgument if client passes other than TDataType.
 */

void RIpcApparcFuzzTest::RunFuzzTestL()
	{
	iTestFuzz.INFO_PRINTF1(_L("Testing appserv APIs GetExecutableNameGivenDataTypeL and AppForDataTypeL  "));	
	TInt ret;
	TVersion version(1,1,1);
	ret = CreateSession(_L("!AppListServer"),version,4);
	iTestFuzz.TEST(ret ==KErrNone);
	HBufC* buf = HBufC::NewLC(268);
	TPtr ptr = buf->Des();
	TIpcArgs args;
	args.Set(0, &ptr);	
	ret = SendReceive(14, args); // Send bad args to CApaAppListServSession::AppForDataTypeL using enum value of EAppListServAppForDataType
	iTestFuzz.TEST(ret ==KErrArgument);
	iTestFuzz.INFO_PRINTF1(_L("Testing CApaAppListServSession::AppForDataTypeL() is completed "));	
	args.Set(2, &ptr);	
	ret = SendReceive(21, args);// Send bad args to CApaAppListServSession::GetExecutableNameGivenDataTypeL using enum value EAppListServGetExecutableNameGivenDocument
	iTestFuzz.TEST(ret ==KErrArgument);
	iTestFuzz.INFO_PRINTF1(_L("Tesing CApaAppListServSession::GetExecutableNameGivenDataTypeL() is completed "));	
	iTestFuzz.INFO_PRINTF2(_L("Test successfully returns KErrArgument %d"),ret);	
	CleanupStack::PopAndDestroy(buf);	
	}

/**
   Constructor
 */	
CT_StartAppTestStep::CT_StartAppTestStep()
	{
	// Call base class method to set up the human readable name for logging*/
	SetTestStepName(KT_StartAppStep);
	}

/**
   Destructor
 */
CT_StartAppTestStep::~CT_StartAppTestStep()
	{
	iApaLsSession.Close();
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */	
TVerdict CT_StartAppTestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	TInt error = iApaLsSession.Connect();
	TEST(error==KErrNone);
	return TestStepResult();
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CT_StartAppTestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}


TVerdict CT_StartAppTestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparc...T_StartApp Test Cases Running..."));
	TRAPD(ret,RunTestCasesL());
	TEST(ret==KErrNone);

	TRAP(ret,TestIpcFuzzL());
	TEST(ret==KErrNone);

	INFO_PRINTF1(_L("...T_StartApp Test Cases Completed."));
	return TestStepResult();
	}

void CT_StartAppTestStep::RunTestCasesL()
	{
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestStartApp1L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestStartApp2L(), iApaLsSession.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestStartApp3L(), iApaLsSession.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestStartApp4L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestStartApp5L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestStartApp6L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestStartApp7L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestStartApp8L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestStartApp9L(), NO_CLEANUP);
	// The following two APIs InsertDataMappingL() & DeleteDataMappingL(), update the type store on the server side.
	// This update takes place on the server side while the test case is still running, which causes the heap check to fail.
	// To avoid the heap check on the server side, DONT_CHECK macro is used.

	HEAP_TEST_LS_SESSION(iApaLsSession, 0, DONT_CHECK, TestInsertDataTypeL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestAppForDataTypeL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, DONT_CHECK, TestDeleteDataTypeL(), NO_CLEANUP);
	}
	
/**
   @SYMTestCaseID T-Serv2Step-StartAppTests1L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test Launching of an application. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Prepare command line information to start an application using
   CApaCommandLine Apis.Call RApaLsSession::StartApp() to start an
   application defined by the command line information.\n
   Test the launching of application for following scenarios:\n
   (1) When Application specified by command line exists.\n
   (2) When Application specified by command line does not exist.\n
   API Calls:\n	
   RApaLsSession::StartApp(const CApaCommandLine& aCommandLine)\n
   
   @SYMTestExpectedResults The test observes that StartApp() returns KErrNone and
   starts the app in the first scenario. In the second case KErrNotFound is
   returned.
   
 */
void CT_StartAppTestStep::TestStartApp1L()
	{
	INFO_PRINTF1(_L("Test RApaLsSession::StartApp using CApaCommandLine....."));
	CApaCommandLine* cmdLn=CApaCommandLine::NewLC();
	TFileName filename;
	_LIT(KLitExePath,"\\sys\\bin\\texe.exe");
	TFullName exePath(KLitExePath);
	filename = SearchAndReturnCompleteFileName(exePath);
	
	_LIT8(KLitDogfish,"dogfish");
	_LIT(KLitWibble,"wibble");
	cmdLn->SetExecutableNameL(filename);
	cmdLn->SetTailEndL(KLitDogfish);
	TInt ret = iApaLsSession.StartApp(*cmdLn); // explicit
	TEST(ret==KErrNone);

	// start a non-existant app
	cmdLn->SetExecutableNameL(KLitWibble);

	ret = iApaLsSession.StartApp(*cmdLn);
	TEST(ret==KErrNotFound);

	CleanupStack::PopAndDestroy(cmdLn); // cmdLn
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestStartApp2L
  
   @SYMPREQ
  
   @SYMTestCaseDesc. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   API Calls:\n	
   \n
   
   @SYMTestExpectedResults .
    
 */
void CT_StartAppTestStep::TestStartApp2L()
	{
	INFO_PRINTF1(_L("Test RApaLsSession::AppForDocument using filename and app uid...."));
	TUid testUid={0x10004c4f};
	iStartAppTestsUid.iUid=0;
	TDataType data;
	_LIT(KLitDocPath,"\\system\\data\\TRApaLsSessionStartAppTest.tst");
	iStartAppTestsFilename = SearchAndReturnCompleteFileName(KLitDocPath);
	TInt ret=iApaLsSession.AppForDocument(iStartAppTestsFilename,iStartAppTestsUid,data);
	TEST(ret==KErrNone);
	TEST(iStartAppTestsUid==testUid);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestStartApp3L
  
   @SYMPREQ
  
   @SYMTestCaseDesc  
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   API Calls:\n	
   \n
   
   @SYMTestExpectedResults 
   
 */	
void CT_StartAppTestStep::TestStartApp3L()
	{
	INFO_PRINTF1(_L("Test RApaLsSession::StartDocument.....start the app from the recognized doc given."));

	TThreadId startAppThreadID;
	TPtrC startAppTestsDoc(KLitTRApaLsSessionStartAppTest);
	TInt ret=iApaLsSession.StartDocument(startAppTestsDoc,startAppThreadID);
	TEST(ret==KErrNone);
	
	//we need to close the started thread, if appropiate
	if (ret==KErrNone)	
		{
		RThread thread;
		User::LeaveIfError(thread.Open(startAppThreadID));
		CleanupClosePushL(thread);

		RProcess process;
		User::LeaveIfError(thread.Process(process));
		CleanupClosePushL(process);

		process.Kill(0);
		
		CleanupStack::PopAndDestroy(&process);
		CleanupStack::PopAndDestroy(&thread);
		}
		
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestStartApp4L
  
   @SYMPREQ
  
   @SYMTestCaseDesc  
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   API Calls:\n	
   \n
   
   @SYMTestExpectedResults 
    
 */	
void CT_StartAppTestStep::TestStartApp4L()
	{
	INFO_PRINTF1(_L("Test RApaLsSession::StartDocument....known data type"));
	TThreadId startAppThreadID;
	TInt ret=iApaLsSession.StartDocument(iStartAppTestsFilename, TDataType(KLitMimeType_TRApaLsSessionStartAppTest),startAppThreadID);
	TEST(ret==KErrNone);
	
	//we need to close the started thread, if appropiate
	if (ret==KErrNone)	
		{
		RThread thread;
		User::LeaveIfError(thread.Open(startAppThreadID));
		CleanupClosePushL(thread);

		RProcess process;
		User::LeaveIfError(thread.Process(process));
		CleanupClosePushL(process);

		process.Kill(0);
		
		CleanupStack::PopAndDestroy(&process);
		CleanupStack::PopAndDestroy(&thread);
		}
		
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T_RApaLsSessionTestStep_TestStartApp5L
  
   @SYMPREQ
  
   @SYMTestCaseDesc  
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   API Calls:\n	
   \n
   
   @SYMTestExpectedResults 
    
 */
void CT_StartAppTestStep::TestStartApp5L()
	{
	INFO_PRINTF1(_L("Test RApaLsSession::StartDocument....unknown data type"));

	TThreadId startAppThreadID;
	TInt ret=iApaLsSession.StartDocument(iStartAppTestsFilename, TDataType(_L8("text/plain/no thanks")),startAppThreadID);
	TEST(ret==KErrNotFound);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T_RApaLsSessionTestStep_TestStartApp6L
  
   @SYMPREQ
  
   @SYMTestCaseDesc  
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   API Calls:\n	
   \n
   
   @SYMTestExpectedResults 
    
 */
void CT_StartAppTestStep::TestStartApp6L()
	{	
	INFO_PRINTF1(_L("Test RApaLsSession::StartDocument....known App Uid"));

	TThreadId startAppThreadID;
	TInt  ret=iApaLsSession.StartDocument(iStartAppTestsFilename,iStartAppTestsUid,startAppThreadID);
	TEST(ret==KErrNone);
	
	//we need to close the started thread, if appropiate
	if (ret==KErrNone)	
		{
		RThread thread;
		User::LeaveIfError(thread.Open(startAppThreadID));
		CleanupClosePushL(thread);

		RProcess process;
		User::LeaveIfError(thread.Process(process));
		CleanupClosePushL(process);

		process.Kill(0);
		
		CleanupStack::PopAndDestroy(&process);
		CleanupStack::PopAndDestroy(&thread);
		}
		
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T_RApaLsSessionTestStep_TestStartApp7L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test mapping of a MIME type to an application. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   API Calls:\n	
   \n
   
   @SYMTestExpectedResults Checks mapping of a MIME type to an application.
    
 */
void CT_StartAppTestStep::TestStartApp7L()
	{	
	INFO_PRINTF1(_L("Test RApaLsSession::StartDocument....non existent file."));

	TThreadId startAppThreadID;
	iStartAppTestsFilename = SearchAndReturnCompleteFileName(_L("\\system\\data\\not_here_thanks.foo"));
	TInt ret=iApaLsSession.StartDocument(iStartAppTestsFilename, startAppThreadID);
	TEST(ret==KErrNotFound);
	INFO_PRINTF1(KCompleted);	
	}
	
/**
   @SYMTestCaseID APPFWK-APPARC-0071
 
   @SYMDEF PDEF100072 -- CApaWindowGroupName::SetAppUid() and FindByAppUid panic
 
   @SYMTestCaseDesc Test Launching of an application with unprotected application UID
  
   @SYMTestPriority High 
 
   @SYMTestStatus Implemented
  
   @SYMTestActions Prepare command line information to start an application using
   CApaCommandLine Apis.Call RApaLsSession::StartApp() to start an
   application defined by the command line information.\n
   Test the launching of application for following scenario:\n
   When Application specified by command line has unprotected application UID(negative uid).\n
   API Calls:\n	
   RApaLsSession::StartApp(const CApaCommandLine &aCommandLine, TThreadId &aThreadId);\n
  
   @SYMTestExpectedResults The test checks whether the thread has terminated with the exit reason KTUnProtectedAppTestPassed
 */
void CT_StartAppTestStep::TestStartApp8L()
	{
	INFO_PRINTF1(_L("Checking launching of an application which has unprotected UID"));
	CApaCommandLine* cmdLine=CApaCommandLine::NewLC();
	TFileName filename;
	_LIT(KAppFileName, "z:\\sys\\bin\\UnProctectedUidApp.exe");
	TFullName exePath(KAppFileName);
	filename = SearchAndReturnCompleteFileName(exePath);
	cmdLine->SetExecutableNameL(filename);
	
	TThreadId appThreadId(0U);
	TInt ret = iApaLsSession.StartApp(*cmdLine, appThreadId);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupStack::PopAndDestroy(cmdLine); // cmdLine
	
	RThread appThread;
	User::LeaveIfError(appThread.Open(appThreadId));
	
	TRequestStatus logonRequestStatus;
	appThread.Logon(logonRequestStatus);

	// wait for UnProctectedUidApp.exe to terminate
	INFO_PRINTF1(_L("Waiting for application to terminate..."));
	User::WaitForRequest(logonRequestStatus);

	const TExitType exitType = appThread.ExitType();
	const TInt exitReason = appThread.ExitReason();
	TExitCategoryName categoryName = appThread.ExitCategory();
	appThread.Close();

	TBuf<50> msg;
	if (exitType == EExitPanic)
		{
		_LIT(KAppPanicInfo, "Application panic: %S %d");
		msg.Format(KAppPanicInfo, &categoryName, exitReason);
		}
	else
		{
		_LIT(KAppExitInfo, "Application exited with code %d");
		msg.Format(KAppExitInfo, exitReason);
		}
	INFO_PRINTF1(msg);

	TEST(logonRequestStatus == KTUnProtectedAppTestPassed);
	TEST(exitType == EExitKill);
	TEST(exitReason == KTUnProtectedAppTestPassed);
	INFO_PRINTF1(KCompleted);
	}
/**
   @SYMTestCaseID APPFWK-APPARC-0076
  
   @SYMDEF INC104463
  
   @SYMTestCaseDesc  Testing StartApp API by forcing memory leak.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Apparc should kill the child process if a leave occurs in StartApp API. 
   API Calls:\n	
   RApaLsSession::StartApp \n
   
   @SYMTestExpectedResults Test should complete without any panic.
    
 */

void CT_StartAppTestStep::TestStartApp9L()
	{
	INFO_PRINTF1(_L("Test to check the StartApp API with forced memory failures..."));
	CApaCommandLine* cmdLn=CApaCommandLine::NewLC();
	TFileName filename;
	TThreadId threadId(0);
	TInt fail;
	_LIT(KLitExePath,"\\sys\\bin\\texe.exe");
	TFullName exePath(KLitExePath);
	filename = SearchAndReturnCompleteFileName(exePath);	
	cmdLn->SetExecutableNameL(filename);
	TInt ret = KErrNoMemory;
	//Without the fix for the Incident INC104463, the OOM test causes panic 
	//KERN-EXEC 56.	
	for(fail=1; ret == KErrNoMemory; fail++)
		{	
		__UHEAP_SETFAIL(RHeap::EDeterministic, fail);
		__UHEAP_MARK;
		ret = iApaLsSession.StartApp(*cmdLn, threadId);
		__UHEAP_MARKEND;
		__UHEAP_RESET;
		TEST((ret==KErrNoMemory || ret==KErrNone));
		if(ret == KErrNone)
			{
			TEST(threadId.Id() != 0);
			}
		}	
	INFO_PRINTF3(_L("Iteration count is %d and the value of return is %d "),fail-1 ,ret);
	CleanupStack::PopAndDestroy(cmdLn); 
	INFO_PRINTF1(KCompleted);
	}
	
 

/**
   @SYMTestCaseID CT_RApaLsSessionTestStep_TestInsertDataTypeL
  
   @SYMPREQ Data caging
  
   @SYMTestCaseDesc  
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions 
   API Calls:\n	
   RApaLsSession::InsertDataMapping \n
   RApaLsSession::InsertDataMappingIfHigher \n
   @SYMTestExpectedResults
 *  
 */
void CT_StartAppTestStep::TestInsertDataTypeL()
	{
	INFO_PRINTF1(_L("Test RApaLsSession::InsertDataMapping....Check data type mapping addition"));
	const TDataTypePriority KPriLow = 1;
	const TDataTypePriority KPriHigh = 2;
	TBool added=EFalse;

	TInt err=iApaLsSession.InsertDataMapping(TDataType(KLitPlainText), KPriLow, KUidTestApp);
	INFO_PRINTF2(_L("returned, %d"), err);
	TSecurityInfo info;
	info.Set(RProcess());
	
	info.iCaps.HasCapability(ECapabilityWriteDeviceData) ? TEST(err==KErrNone) :  TEST(err==KErrPermissionDenied);
	
	err=iApaLsSession.InsertDataMappingIfHigher(TDataType(KLitPlainText), KPriHigh, KUidTestApp, added);
	INFO_PRINTF2(_L("returned, %d"), err);
	info.iCaps.HasCapability(ECapabilityWriteDeviceData) ? TEST(err==KErrNone && added)  :  TEST(err==KErrPermissionDenied);
	if (err==KErrNone)
		{
		// Waits till the data type mappings are restored from the data store ini file
		iApaLsSession.WaitForTypeStoreUpdate();
		}
	INFO_PRINTF1(_L("Test RApaLsSession::InsertDataMapping....Check data type mapping addition....Done"));
	}

/**
   @SYMTestCaseID T_RApaLsSessionTestStep_TestAppForDataTypeL
  
   @SYMPREQ
  
   @SYMTestCaseDesc . 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   API Calls:\n
   
   @SYMTestExpectedResults 
    
 */
void CT_StartAppTestStep::TestAppForDataTypeL()
	{
	INFO_PRINTF1(_L("Test RApaLsSession::AppForDataType....Check an app binding"));
	_LIT8(KLitPlainText,"text/plain");
	TUid testUid={KTestAppUidValue};
	
	TInt ret=iApaLsSession.AppForDataType(TDataType(KLitPlainText),iStartAppTestsUid);
	TEST(ret==KErrNone);
	TEST(iStartAppTestsUid==testUid);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID CT_RApaLsSessionTestStep::TestDeleteDataTypeL
  
   @SYMPREQ Data caging
  
   @SYMTestCaseDesc  
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions 
   API Calls:\n	
   RApaLsSession::DeleteDataMapping \n
   @SYMTestExpectedResults
    
 */

void CT_StartAppTestStep::TestDeleteDataTypeL()
	{
	INFO_PRINTF1(_L("Test RApaLsSession::DeleteDataMapping....Check data type mapping deletion"));

	TInt err=iApaLsSession.DeleteDataMapping(TDataType(KLitPlainText));
	INFO_PRINTF2(_L("returned, %d"), err);
	TSecurityInfo info;
	info.Set(RProcess());
	
	info.iCaps.HasCapability(ECapabilityWriteDeviceData) ? TEST(err==KErrNone) :  TEST(err==KErrPermissionDenied);
	if(err==KErrNone)
	{
	// Waits till the data type mappings are restored from the data store ini file
	iApaLsSession.WaitForTypeStoreUpdate();	
	}
	INFO_PRINTF1(_L("Test RApaLsSession::DeleteDataMapping....Check data type mapping deletion....Done"));
	}
	
TFileName CT_StartAppTestStep::SearchAndReturnCompleteFileName(const TDesC& aFileName)
	{
	RFs fs;
	TInt ret = fs.Connect();
	TEST(ret==KErrNone);

	TParsePtrC parse(aFileName);
	TFindFile findFile(fs);
	ret = findFile.FindByDir(parse.NameAndExt(), parse.Path());
	if (ret!=KErrNone)
		{
		TBuf<64> buffer;
		buffer.Format(_L("Error while finding the file, %d"), ret);
		}
	fs.Close();
	return findFile.File();
	}

void CT_StartAppTestStep::TestIpcFuzzL()
	{
	INFO_PRINTF1(_L("Test IpcFuzz DEF116002 Started"));	
	RIpcApparcFuzzTest testIpc(*this);
	CleanupClosePushL(testIpc);
	TRAPD(err,testIpc.RunFuzzTestL());
	TEST(err==KErrNone);
	CleanupStack::PopAndDestroy(&testIpc);
	INFO_PRINTF1(_L("Test IpcFuzz DEF116002 Completed"));
	}
