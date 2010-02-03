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



/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#include <apgcli.h>
#include "T_WindowChainingStep.h"
#include "TWindowChaining.h"
#include <apacmdln.h>

/**
   @SYMTestCaseID TODO
  
   @SYMPREQ DEFECT 053778 - Window Chaining
  
   @SYMTestCaseDesc
   Stuff
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   1. Create a child ui application (t_winchainlaunch.exe)
   2. From t_winchainlaunch.exe, connect a child application (t_winchainchild.exe) with
      a specific ordinal
   3. Send message to child application from launching application
   4. When message is received by child application, check they share the same ordinal
   5. Child application reports back to launch application the result of the test
   6. Launch application quits with the test result as an exit code
  
   @SYMTestExpectedResults
   Window group ordinals should be the same.  
   t_winchainlaunch.exe should exit with KTWindowChainingTestPassed.
  
 */
void CT_WindowChainingStep::TestWindowChainingL()
	{

	TThreadId appThreadId(0U);

	RApaLsSession ls;
	CleanupClosePushL(ls);
	TInt ret = ls.Connect();
	TEST(ret == KErrNone);
	
	CApaCommandLine* cmdLine=CApaCommandLine::NewLC();
    
	cmdLine->SetExecutableNameL(KWinChainChildAppFileName);
	cmdLine->SetCommandL(EApaCommandOpen);

	// start document, causing GuiBaseChildApp.exe to be launched
	INFO_PRINTF1(_L("Launching Child Application"));

	// start child application
	ret = ls.StartApp(*cmdLine, appThreadId);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupStack::PopAndDestroy(2);  // ls and cmdLine
	
	RThread appThread;
	User::LeaveIfError(appThread.Open(appThreadId));
	
	TRequestStatus logonRequestStatus;
	appThread.Logon(logonRequestStatus);

	// wait for t_windowchainLaunch.exe to terminate
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

	TEST(logonRequestStatus == KTWindowChainingTestPassed);
	TEST(exitType == EExitKill);
	TEST(exitReason == KTWindowChainingTestPassed);
	
	if (exitReason == KTWindowChainingTestPassed)
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);
	}
	
TVerdict CT_WindowChainingStep::doTestStepL()
{
	__UHEAP_MARK;
		
	INFO_PRINTF1(_L("Test Started"));

	// run the test
	TRAPD(err, TestWindowChainingL());
	INFO_PRINTF2(_L("  test completes with %d"), err);
	TEST(err == KErrNone);

	INFO_PRINTF1(_L("Test Finished"));
		
	__UHEAP_MARKEND;
	return TestStepResult();
}
