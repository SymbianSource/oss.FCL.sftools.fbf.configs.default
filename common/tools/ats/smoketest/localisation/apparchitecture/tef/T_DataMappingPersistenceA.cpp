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

#include "T_DataMappingPersistenceA.h"


/**
 * Constructor
 */	
CT_DataMappingPersistenceATestStep::CT_DataMappingPersistenceATestStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_DataMappingPersistenceAStep);
	}

/**
 * Destructor
 */
CT_DataMappingPersistenceATestStep::~CT_DataMappingPersistenceATestStep()
	{
	iSession.Close();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */	
TVerdict CT_DataMappingPersistenceATestStep::doTestStepPreambleL()
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
TVerdict CT_DataMappingPersistenceATestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CT_DataMappingPersistenceATestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparc...T_DataMappingPersistenceA Test Cases Running..."));

	TRAPD(ret,RunTestCasesL())
	TEST(ret==KErrNone);
	
	INFO_PRINTF1(_L("...T_DataMappingPersistenceA Test Cases Completed."));

	return TestStepResult();
	}

TInt CT_DataMappingPersistenceATestStep::RunTestCasesL()
	{
	//DONT_CHECK since array in type store is uncompressed
	HEAP_TEST_LS_SESSION(iSession, 0, DONT_CHECK, TestInsertDataMappingL(), NO_CLEANUP);
	
	return KErrNone;
	}
	
void CT_DataMappingPersistenceATestStep::TestInsertDataMappingL()
	{
	TUid appUid = {0x10004c58}; 
	_LIT8(KLitMimeTypeData,"text/TestDataMappingPersistence");
	TDataType dataType (KLitMimeTypeData);
	TInt error = iSession.InsertDataMapping(dataType,1,appUid);	
	TEST(error==KErrNone);
	}
