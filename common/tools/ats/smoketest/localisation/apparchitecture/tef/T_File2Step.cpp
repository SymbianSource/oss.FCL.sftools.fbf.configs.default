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
// Tests CApaAppRegFinder APIs.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include "T_File2Step.h"
#include "..\apfile\aprfndr.h"

//
#if !defined(__E32TEST_H__)
#include <e32test.h>
#endif

/**
   @SYMTestCaseID T-FileStep-testFindAllAppsRegL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test CApaAppRegFinder::FindAllAppsL() API. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The method creates an Application Finder object and calls
   CApaAppRegFinder::FindAllAppsL() to perform a complete scan of all
   available applications. It traverses through the application list by calling
   CApaAppRegFinder::NextL() to confirm that the list contains entries.\n
   API Calls:\n	
   CApaAppRegFinder::FindAllAppsL()\n
  
   @SYMTestExpectedResults Test checks the number of applications present in the list.
    
 */
void CT_File2Step::testFindAllAppsRegL()
	{
	INFO_PRINTF1(_L("Testing CApaAppRegFinder::FindAllAppsL()"));
	RFs fSession;
	fSession.Connect();
	CApaAppRegFinder* regFinder=CApaAppRegFinder::NewL(fSession);
	//
	TRAPD(ret, regFinder->FindAllAppsL() );
	TEST(ret==KErrNone);
	//
	TBool more=ETrue;
	TInt count=0;
	while (more) 
		{
		TApaAppEntry entry;
		RPointerArray<HBufC> dummy;
		TRAPD(ret, more=regFinder->NextL(entry, dummy) );
		TEST(ret==KErrNone);
		if (more)
			count++;
		}
	TEST(count>0);
	INFO_PRINTF2(_L("     Apps found: %D"),count);
	//
	delete regFinder;
	fSession.Close();
	}

CT_File2Step::~CT_File2Step()
/**
   Destructor
 */
	{
	}

CT_File2Step::CT_File2Step()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_File2Step);
	}

TVerdict CT_File2Step::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_File2Step::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}

TVerdict CT_File2Step::doTestStepL()
/**
  @return - TVerdict code
  Override of base class virtual
*/
	{
	INFO_PRINTF1(_L("Testing the APFILE dll..."));
	//

 	__UHEAP_MARK;
	TRAPD(r,testFindAllAppsRegL());
	TEST(r==KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("Test completed!"));
	return TestStepResult();
	}
