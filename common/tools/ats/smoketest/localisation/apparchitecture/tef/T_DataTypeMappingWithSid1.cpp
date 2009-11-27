// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The following test case is used to test whether untrusted applications 
// are able to override MIME types mappings using InsertDataMappingIfHigher API.
// It is to ensure that priority checking mechanism (downgrading priority of 
// untrusted apps) works as was designed.
// 
//



/**
 @file T_DataTypeMappingWithSid1.cpp
 @internalComponent - Internal Symbian test code
*/

#include "T_DataTypeMappingWithSid1.h"
#include "tstapp.h"
#include "testableapalssession.h"
#include <appfwk_test.h>

/**
   @SYMTestCaseID		APPFWK-APPARC-0036
  
   @SYMPREQ			    Policing server apis		
  
   @SYMTestCaseDesc 	Testing whether untrusted applications are able to override MIME types mapping
  
   @SYMTestPriority 	High
  
   @SYMTestStatus 		Implemented
   
   @SYMTestActions      To connect to RApaLsSession and try to register trusted and untrusted apps for mime types.
   @SYMTestExpectedResults Test should complete without any panic.
  
 */
void CT_DataTypeMappingWithSid1::ExecuteL(RApaLsSession& aLs)
	{
	TInt ret;
	TBool added=EFalse;
	_LIT8(KLitAudioFake, "audio/fakedata");
    
    TDataType dataType( KLitAudioFake );
    
    TUid trustedAppUid = {0x10207f8f};
    TUid untrustedAppUid = {0xA3010010};
    TUid retAppUid( KNullUid );

	ret=aLs.InsertDataMapping(dataType, KDataTypePriorityHigh, trustedAppUid);
	INFO_PRINTF1(_L("Trusted app sets a MimeType mapping: InsertDataMapping with PriorityHigh"));
	INFO_PRINTF3(_L("Expected return code: %d, Actual return code: %d"), KErrNone, ret);
	TEST(ret==KErrNone);
    ret = aLs.AppForDataType(dataType,retAppUid);
    INFO_PRINTF3(_L("Testing SID: Expected return code: %d, Actual return code: %d"), KErrNone, ret);
    TEST(ret==KErrNone);
    INFO_PRINTF3(_L("Expected SID: %d, Actual SID value: %d"), trustedAppUid, retAppUid);
    TEST(retAppUid == trustedAppUid);
    
    ret=aLs.InsertDataMappingIfHigher(dataType, KDataTypePriorityTrustedHigh, untrustedAppUid, added);
    INFO_PRINTF1(_L("UnTrusted app sets a MimeType mapping: InsertDataMapping with PriorityTrustedHigh"));
    INFO_PRINTF3(_L("Expected return code: %d, Actual return code: %d"), KErrNone, ret);
    TEST(ret==KErrNone);
    ret = aLs.AppForDataType(dataType,retAppUid);
    INFO_PRINTF3(_L("Testing SID: Expected return code: %d, Actual return code: %d"), KErrNone, ret);
    TEST(ret==KErrNone);
    INFO_PRINTF3(_L("Expected SID: %d, Actual SID value: %d"), untrustedAppUid, retAppUid);
    TEST(retAppUid == untrustedAppUid);
    
    ret=aLs.InsertDataMappingIfHigher(dataType, KDataTypePriorityTrustedHigh, trustedAppUid, added);
    INFO_PRINTF1(_L("Trusted app sets a MimeType mapping: InsertDataMapping with PriorityTrustedHigh"));
    INFO_PRINTF3(_L("Expected return code: %d, Actual return code: %d"), KErrNone, ret);
    TEST(ret==KErrNone);
    ret = aLs.AppForDataType(dataType,retAppUid);
    INFO_PRINTF3(_L("Testing SID: Expected return code: %d, Actual return code: %d"), KErrNone, ret);
    TEST(ret==KErrNone);
    INFO_PRINTF3(_L("Expected SID: %d, Actual SID value: %d"), trustedAppUid, retAppUid);
    TEST(retAppUid == trustedAppUid);

    ret=aLs.InsertDataMappingIfHigher(dataType, KDataTypePriorityTrustedHigh+1, untrustedAppUid, added);
    INFO_PRINTF1(_L("UnTrusted app sets a MimeType mapping: InsertDataMapping with KDataTypePriorityTrustedHigh+1"));
    INFO_PRINTF3(_L("Expected return code: %d, Actual return code: %d"), KErrNone, ret);
    TEST(ret==KErrNone);
    ret = aLs.AppForDataType(dataType,retAppUid);
    INFO_PRINTF3(_L("Testing SID: Expected return code: %d, Actual return code: %d"), KErrNone, ret);
    TEST(ret==KErrNone);
    INFO_PRINTF3(_L("Expected SID: %d, Actual SID value: %d"), trustedAppUid, retAppUid);
    TEST(retAppUid == trustedAppUid);
    
    //delete data type mapping
    ret=aLs.DeleteDataMapping(dataType);
    INFO_PRINTF3(_L("Deleting data mapping: Expected return code: %d, Actual return code: %d"), KErrNone, ret);
    TEST(ret==KErrNone);
	}

CT_DataTypeMappingWithSid1::CT_DataTypeMappingWithSid1()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_DataTypeMappingWithSid1);
	}

CT_DataTypeMappingWithSid1::~CT_DataTypeMappingWithSid1()
	{
	}

TVerdict CT_DataTypeMappingWithSid1::doTestStepL()
    {
	INFO_PRINTF1(_L("APPFWK-APPARC-0036: DataTypeMappingWithSid1 - Started"));
	
	RTestableApaLsSession apaLsSession;
	TEST(apaLsSession.Connect()==KErrNone);
	CleanupClosePushL(apaLsSession);

	//DONT_CHECK since array in type store is uncompressed
	HEAP_TEST_LS_SESSION(apaLsSession, 0, DONT_CHECK, ExecuteL(apaLsSession), NO_CLEANUP);

	CleanupStack::PopAndDestroy(&apaLsSession);


	INFO_PRINTF1(_L("APPFWK-APPARC-0036: DataTypeMappingWithSid1 - Finished"));

	return TestStepResult();
	}
