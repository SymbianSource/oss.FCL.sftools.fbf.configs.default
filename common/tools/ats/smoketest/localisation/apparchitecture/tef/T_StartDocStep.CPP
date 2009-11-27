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


#include "testableapalssession.h"
#include "T_StartDocStep.h"
#include "TStartDoc.h"

/**
   @SYMTestCaseID T-StartDocStep-TestStartDocL
  
   @SYMPREQ PHAR-5NTCWY
  
   @SYMTestCaseDesc
   Checks that document names longer than 256 characters can be started.
   The test starts a document and checks that the application launched
   to handle that document is passed the full document name.
   The test uses the StartDocument overload which does not provide an
   application UID or mime type, because we want the test to check that
   the long document name is supported by the data recognizer framework.
   The test also checks that the data recognizer framework doesn't return
   an error because it incorrectly assumes the document name is a file name
   and tries to open the document to read its contents before performing
   data recognition.
   In this case, the document name isn't a file name, it's a pretend URL
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   1. Call RApaLsSession::StartDocument passing a long document name (KLitLongURL).
   2. StartDocument should cause TStartDocApp.app to be launched, so wait
      for the application thread to terminate.
   3. Check the thread has terminated in the way we expected
  
   @SYMTestExpectedResults
   Calling RApaLsSession::StartDocument should cause the apparc server to
   call CLongUrlRec::DoRecognizeL as part of trying to find the mime type
   of the document. CLongUrlRec returns a mime type which is associated with
   TStartDocApp.app, so apparc server starts TStartDocApp.app passing the
   long document name on the command line.
   The long document name should eventually be passed through to
   CStartDocAppUi::ProcessCommandParametersL which will panic with
   EPanicDocNameCheck1Failed if the document name isn't KLitLongURL.
   CStartDocDocument::OpenFileL panics with EPanicDocNameCheck1NotExecuted if
   the document name check in CStartDocAppUi::ProcessCommandParametersL wasn't
   executed.
   CStartDocDocument::OpenFileL panics with EPanicDocNameCheck2Failed if
   the document name returned by CApaProcess::MainDocFileName isn't KLitLongURL.
  
   TStartDocApp.app should exit with KTStartDocTestPassed.
  
 */
void CT_StartDocStep::TestStartDocL(RApaLsSession& aLs)
	{
	TPtrC longURL(KLitLongURL);
	// sanity check to ensure the document name we're going to pass to
	// RApaLsSession::StartDocument is longer than the maximum length
	// of a file name
	TEST(longURL.Length() > KMaxFileName);

	TThreadId appThreadId(0U);
	// start document, causing TStartDocApp.app to be launched
	INFO_PRINTF1(_L("Starting document..."));
	TInt ret = aLs.StartDocument(longURL, appThreadId);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);

	RThread appThread;
	User::LeaveIfError(appThread.Open(appThreadId));
	TRequestStatus logonRequestStatus;
	appThread.Logon(logonRequestStatus);

	// wait for TStartDocApp.app to terminate
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

	TEST(logonRequestStatus == KTStartDocTestPassed);
	TEST(exitType == EExitKill);
	TEST(exitReason == KTStartDocTestPassed);

	//avoid race conditions
	User::After(1500000);
	}

TVerdict CT_StartDocStep::doTestStepL()
{
	INFO_PRINTF1(_L("Test Started"));
	
	RTestableApaLsSession ls;
	TEST(KErrNone == ls.Connect());
	CleanupClosePushL(ls);

	//avoid race conditions
	User::After(1500000);

	// run the test
	HEAP_TEST_LS_SESSION(ls, 0, 0, TestStartDocL(ls), NO_CLEANUP);
	
	CleanupStack::PopAndDestroy(&ls);

	INFO_PRINTF1(_L("Test Finished"));

	return TestStepResult();
}
