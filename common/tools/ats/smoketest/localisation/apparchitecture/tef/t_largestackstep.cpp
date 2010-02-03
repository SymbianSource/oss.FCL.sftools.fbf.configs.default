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
// The following test case is used to test whether or not Apparc can handle loading applications with excessively large stack frames.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <apgcli.h>
#include <apacmdln.h>
#include <apgicnfl.h>
#include "../apserv/APSCLSV.H"
#include "T_LargeStackStep.h"

const TUid KLargeStackAppUid = {0x10282B27};

TInt RTestApaLsSession::TestExeRecognizerL(const CApaCommandLine& aCommandLine)
	{
	TThreadId dummyThreadId;
	TPckg<TThreadId> threadId(dummyThreadId);
	const TInt opcode=EAppListServStartAppWithoutReturningThreadId;
	TIpcArgs ipcArgs;
	aCommandLine.GetIpcArgsLC(ipcArgs);
	ipcArgs.Set(CApaCommandLine::EIpcFirstFreeSlot, &threadId);
	User::LeaveIfError(SendReceive(opcode, ipcArgs));
	CleanupStack::PopAndDestroy(); // the TIpcArgs
	return KErrNone;
	}

CT_LargeStackStep::CT_LargeStackStep()
	{
	}

CT_LargeStackStep::~CT_LargeStackStep()
	{
	iApaLsSession.Close();
	}

/**
   @SYMTestCaseID APPFWK-APPARC-0060
  
   @SYMPREQ CR0885
  
   @SYMTestCaseDesc
   Test apparc's use of RProcess::CreateWithStackOverride
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
	1. Load an application with an overly large stack requirement via RApaLsSession::StartApp.
	2. Load the application via RApaLsSession::StartDocument
	  
   @SYMTestExpectedResults
	Application should start normally both times.
  
*/
void CT_LargeStackStep::TestLargeStackL()
	{
	TApaAppInfo info;
	INFO_PRINTF1(_L("Test that the large stack example application can be found"));
	TEST(iApaLsSession.GetAppInfo(info,KLargeStackAppUid)==KErrNone);

	INFO_PRINTF1(_L("Test that the application executes without error"));
	CApaCommandLine* commandline = CApaCommandLine::NewLC();
	commandline->SetExecutableNameL(info.iFullName);
	commandline->SetCommandL(EApaCommandRunWithoutViews);
	TEST(iApaLsSession.StartApp(*commandline)==KErrNone);
	
	INFO_PRINTF1(_L("Test running the application via the legacy recogniser code."));
	TEST(iApaLsSession.TestExeRecognizerL(*commandline)==KErrNone);

	INFO_PRINTF1(_L("Test running the application via StartDocument."));
	TThreadId threadId;
	TEST(iApaLsSession.StartDocument(_L("z:\fakename.doc"), KLargeStackAppUid, threadId, RApaLsSession::ELaunchNewApp)==KErrNone);

	// Cleanup
	CleanupStack::PopAndDestroy(commandline);
	}

TVerdict CT_LargeStackStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Test T_LargeStack Started"));
	
	__UHEAP_MARK;
	// Connect to RApaLsSession
	TEST(iApaLsSession.Connect()==KErrNone);

	// Run the test
	TRAPD(err,TestLargeStackL());
	TEST(err==KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("Test Finished"));	
	return TestStepResult();
	}
