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


#include "T_DataMappingPersistenceC.h"



/**
 * Constructor
 */	
CT_DataMappingPersistenceCTestStep::CT_DataMappingPersistenceCTestStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_DataMappingPersistenceCStep);
	}

/**
 * Destructor
 */
CT_DataMappingPersistenceCTestStep::~CT_DataMappingPersistenceCTestStep()
	{
	iSession.Close();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */	
TVerdict CT_DataMappingPersistenceCTestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	TInt error = iSession.Connect();
	TEST(error==KErrNone);
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_DataMappingPersistenceCTestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CT_DataMappingPersistenceCTestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparc...T_DataMappingPersistenceC Test Cases Running..."));

	TRAPD(ret,RunTestCasesL());
	TEST(ret==KErrNone);
	
	INFO_PRINTF1(_L("...T_DataMappingPersistenceC Test Cases Completed."));

	return TestStepResult();
	}
	
TInt CT_DataMappingPersistenceCTestStep::RunTestCasesL()
	{
	HEAP_TEST_LS_SESSION(iSession, 0, 0, TestCheckDeleteDataMappingL(), NO_CLEANUP);
	
	return KErrNone;
	}	

void CT_DataMappingPersistenceCTestStep::TestCheckDeleteDataMappingL()
	{
	TUid appUid = {0x10004c58}; 
	_LIT8(KLitMimeTypeData,"text/TestDataMappingPersistence");
	TDataType dataType (KLitMimeTypeData);
	TUid retAppUid;
	TInt error = iSession.AppForDataType(dataType,retAppUid);
	TEST(error==KErrNone);
	TEST(appUid != retAppUid);
	}
