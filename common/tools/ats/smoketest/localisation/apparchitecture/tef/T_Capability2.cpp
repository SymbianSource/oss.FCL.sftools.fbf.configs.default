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
// The following test case is used to test whether apparctestserver has the capability
// to access writable apis
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include "T_Capability2.h"
#include "tstapp.h"
#include "testableapalssession.h"
#include <appfwk_test.h>

/**
   @SYMTestCaseID		APPFWK-APPARC-0030
  
   @SYMPREQ			Policing server apis		
  
   @SYMTestCaseDesc 	Testing whether apparctestserver has the capability to access writable apis
  
   @SYMTestPriority 	High
  
   @SYMTestStatus 		Implemented
   
   @SYMTestActions  
   To connect to RApaLsSession and to check whether uiktestserver process
   has the permission to access the mentioned apis
   API Calls:\n	
   RApaLsSession::Connect();
   RApaLsSession::SetAcceptedConfidence(TInt aConfidence);
   RApaLsSession::InsertDataMapping(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid);
   RApaLsSession::InsertDataMappingIfHigher(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid, TBool& aInserted);
   RApaLsSession::DeleteDataMapping(const TDataType& aDataType);
   
   @SYMTestExpectedResults Test should complete without any panic.
  
 */
void CT_Capability2::ExecuteL(RApaLsSession& aLs)
	{
	TInt ret;
	TInt temp=1234;
	TBool added=EFalse;
	_LIT8(KLitPlainText,"text/plain");
	const TDataTypePriority KPriLow = 1;
	const TDataTypePriority KPriHigh = 2;
	
	TSecurityInfo info;
	info.Set(RProcess());
	
	ret=aLs.SetAcceptedConfidence(temp);
	INFO_PRINTF1(_L("Capability check of SetAcceptedConfidence"));
	INFO_PRINTF3(_L("Expected return code: %d, Actual return code: %d"), KErrNone, ret);
	TEST(ret==KErrNone);

	ret=aLs.InsertDataMapping(TDataType(KLitPlainText), KPriLow, KUidTestApp);
	INFO_PRINTF1(_L("Capability check of InsertDataMapping"));
	INFO_PRINTF3(_L("Expected return code: %d, Actual return code: %d"), KErrNone, ret);
	TEST(ret==KErrNone);

	ret=aLs.InsertDataMappingIfHigher(TDataType(KLitPlainText), KPriHigh, KUidTestApp, added);
	INFO_PRINTF1(_L("Capability check of InsertDataMappingIfHigher"));
	INFO_PRINTF3(_L("Expected return code: %d, Actual return code: %d"), KErrNone, ret);
	TEST(ret==KErrNone);

	ret=aLs.DeleteDataMapping(TDataType(KLitPlainText));
	INFO_PRINTF1(_L("Capability check of DeleteDataMapping"));
	INFO_PRINTF3(_L("Expected return code: %d, Actual return code: %d"), KErrNone, ret);
	TEST(ret==KErrNone);

	}

CT_Capability2::CT_Capability2()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_Capability2);
	}

CT_Capability2::~CT_Capability2()
	{
	}

TVerdict CT_Capability2::doTestStepL()
    {
	INFO_PRINTF1(_L("APPFWK-APPARC-0031: Capability2 - Started"));
	
	RTestableApaLsSession apaLsSession;
	TEST(apaLsSession.Connect()==KErrNone);
	CleanupClosePushL(apaLsSession);

	//DONT_CHECK since array in type store is uncompressed
	HEAP_TEST_LS_SESSION(apaLsSession, 0, DONT_CHECK, ExecuteL(apaLsSession), NO_CLEANUP);

	CleanupStack::PopAndDestroy(&apaLsSession);


	INFO_PRINTF1(_L("APPFWK-APPARC-0031: Capability2 - Finished"));

	return TestStepResult();
	}
