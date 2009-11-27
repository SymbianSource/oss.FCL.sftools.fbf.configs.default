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
// The following test case is used to test if apparctestserver 
// can return app data for a specific app by caching that data when requested.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <apgcli.h>
#include "T_AppList.h"

CTestAppListStep::CTestAppListStep()
	{
	}

CTestAppListStep::~CTestAppListStep()
	{
	iApaLsSession.Close();
	}


/**
   @SYMTestCaseID CTestAppListStep_TestAppListL
  
   @SYMDEF DEF056494: Apparc App Info access APIs require the applist to be complete, delaying startup
  
   @SYMTestCaseDesc Testing whether apparctestserver return app info when the list is not completely populated.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions To query for a particular app Info and check if the info is available.
   API Calls:\n
   RApaLsSession::GetAppInfo();
  
   @SYMTestExpectedResults Test should complete without any panic.
 */
void CTestAppListStep::TestAppList()
	{
	TApaAppInfo appInfo;
	TUid uid = {0x100048F3};
	TInt ret = iApaLsSession.GetAppInfo(appInfo,uid);
	INFO_PRINTF2(_L(" Call to GetAppInfo returned : %d  "), ret);
	TEST(ret==KErrNone);
	}


TVerdict CTestAppListStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Test T_AppList Started"));
	
	// Connect to RApaLsSession without waiting for app list population
	TEST(iApaLsSession.ConnectWithoutWaitingForListPopulation()==KErrNone);
	
	// Run the test
	//DONT_CHECK since apps are added to app list by CApaAppList::FindAndAddSpecificAppL()
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, DONT_CHECK, TestAppList(), NO_CLEANUP);

	INFO_PRINTF1(_L("Test Finished"));	
	return TestStepResult();
	}
