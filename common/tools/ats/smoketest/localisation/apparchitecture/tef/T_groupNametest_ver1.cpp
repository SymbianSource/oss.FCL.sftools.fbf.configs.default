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
// The following test case is used to test whether the groupname specified in the 
// registration file is read if its not defined in the localisable resource file
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#include "testableapalssession.h"
#include <APAID.H>
#include "T_groupNametest_ver1.h"
#include "ApparcTestServer.h"
#include "TestExecuteStepBase.h"

// CT_GroupNameStep_ver1

const TUid KUidTGroupName={0x10208183};

/**
   Constructor
*/
CT_GroupNameStep_ver1::CT_GroupNameStep_ver1()
	{
	}


/**
  Destructor
*/
CT_GroupNameStep_ver1::~CT_GroupNameStep_ver1()
	{
	}


/**
   @SYMTestCaseID T-GroupNameStep-ver1-DoTestGroupNameL
  
   @SYMPREQ

   @SYMTestCaseDesc 
   T_groupname to test the Groupname in localisable resource file
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions
   It connects to the Application Architecture server to perform the following:
   To test whether the groupname defined in registration file is read if its
   not defined in the localisable resource file
   It calls the following function to get the information about the groupname.
   RApaLsSession::GetAppCapability(TDes8& aCapabilityBuf,TUid aAppUid) const\n

   @SYMTestExpectedResults
   GetAppCapability should return the groupname
   defined in the application's registration file.
 
 */
void CT_GroupNameStep_ver1::DoTestGroupNameL(RApaLsSession& aLs)
	{
	TApaAppCapabilityBuf cbuf;
	TApaAppCapability capability;
	 
	// To read the groupname defined in registration resource file

	TInt err = aLs.GetAppCapability(cbuf,KUidTGroupName);
	TEST(err == KErrNone);
	capability = cbuf();
	_LIT(KGroupname,"Reg_groupname");
	
	TEST(capability.iGroupName == KGroupname);
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CT_GroupNameStep_ver1::doTestStepL()
	{
	INFO_PRINTF1(_L(" Test Started"));

	RTestableApaLsSession ls;
	TEST(KErrNone == ls.Connect());
	CleanupClosePushL(ls);

    //To test whether the groupname defined in the 
	//registration file is read if its not defined in localisable resource file

	HEAP_TEST_LS_SESSION(ls, 0, 0, DoTestGroupNameL(ls), NO_CLEANUP);
	
	CleanupStack::PopAndDestroy(&ls);

	INFO_PRINTF1(_L(" Test Finished"));
		
	return TestStepResult();

	}
