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
// The following test case is used to test that the information read from "old format" resource files 
// isn't thrown  away because of an error while trying to read the group name member that 
// isn't present in localisable resource file.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#include "testableapalssession.h"
#include "APAID.H"
#include "T_groupNametest_ver2.h"
#include "ApparcTestServer.h"
#include "TestExecuteStepBase.h"

// CT_GroupNameStep_ver2


const TUid KUidTGroupName={0x10208184};

/**
  Constructor
*/
CT_GroupNameStep_ver2::CT_GroupNameStep_ver2()
	{
	}


/**
  Destructor
*/
CT_GroupNameStep_ver2::~CT_GroupNameStep_ver2()
	{
	}

/**
   @SYMTestCaseID T-GroupNameStep-Ver2-DoTestCaptionNameL
  
   @SYMPREQ

   @SYMTestPriority High
   
   @SYMTestStatus Implemented
	 
   @SYMTestCaseDesc 
   T-groupname to test localisable resource file
  
   @SYMTestActions
   It connects to the Application Architecture server to perform the following:
   To test that information read from "old format" resource files isn't thrown away because
   of an error while trying to read the group name member that isn't present in localisable resource file..
   It calls following function to get the information about the groupname.
   RApaLsSession::GetAppInfo(TApaAppInfo& aInfo,TUid aAppUid) const\n
  
   @SYMTestExpectedResults
   GetAppInfo should return the caption name and ShortCaption Name 
   defined in the localisable resource file.
  
 */
void CT_GroupNameStep_ver2::DoTestCaptionNameL(RApaLsSession& aLs)
	{
	TApaAppInfo info;
	TInt err = aLs.GetAppInfo(info,KUidTGroupName);
	TEST(err == KErrNone);
	_LIT(KCaption,"T_groupname_ver2");
	_LIT(KShortCaption,"TGrpNameV2");	
			
	TEST(info.iCaption == KCaption);
	TEST(info.iShortCaption == KShortCaption);
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CT_GroupNameStep_ver2::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Started"));

	RTestableApaLsSession ls;
	TEST(KErrNone == ls.Connect());
	CleanupClosePushL(ls);

	HEAP_TEST_LS_SESSION(ls, 0, 0, DoTestCaptionNameL(ls), NO_CLEANUP);

	CleanupStack::PopAndDestroy(&ls);

	INFO_PRINTF1(_L("Test Finished"));

	return TestStepResult();

	}
