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
// Tests CApaScanningFileRecognizer APIs.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <e32uid.h>
#include <f32file.h>
#include <fbs.h>
#include <apadef.h>
#include <apfrec.h>
#include <apfctlf.h>
#include <apgctl.h>
#include <ecom.h>

#include "tstapp.h"
#include "T_File1Step.h"

//
#if !defined(__E32TEST_H__)
#include <e32test.h>
#endif

const TUid KTestEcomFileRecognizerUid={0x101F7D8C};

void CT_File1Step::setup()
	{
#if defined(__EPOC32__)
	TFullName filePath=_L("c:\\docs\\tstapp.doc");
	TFullName tempPath=_L("c:\\system\\temp\\");
	// if we're on the rack create the directories we need
	TParse parser;
	parser.Set(filePath,NULL,NULL);
	iFs.MkDirAll(parser.DriveAndPath());
	parser.Set(tempPath,NULL,NULL);
	iFs.MkDirAll(parser.DriveAndPath());
#endif
	}

/**
   @SYMTestCaseID T-FileStep-testScanningFileTheRecognizerL
  
   @SYMPREQ
  
   @SYMTestCaseDesc 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The method creates a scanning file recognizer object and observes whether
   return values from RecognizerCount() and UpdateCounter() match with the
   expected results. The setter function SetRecognizerL() is also tested with a recognizer
   that doesn't exist to check if it would return KErrNotFound.\n
   API Calls:\n	
   CApaScanningFileRecognizer::NewL(RFs& aFs, MApaAppStarter* aAppStarter)\n
   CApaScanningFileRecognizer::RecognizerCount()\n
   CApaScanningFileRecognizer::UpdateCounter()\n
   CApaScanningFileRecognizer::SetRecognizerL(const TRecognizer& aRecognizer)\n
   
   @SYMTestExpectedResults Test checks results against expected values.
    
 */
void CT_File1Step::testScanningFileTheRecognizerL()
	{
	INFO_PRINTF1(_L("Testing the Scanning File-Recognizer"));
	//
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	//
	// construct - this scans for iRecognizer plug-ins
	TRAPD(ret, iRecognizer=CApaScanningFileRecognizer::NewL(iFs,NULL));
	TEST(ret==KErrNone);
	// 06/09/99 SimonC: following tests updated as they'll fail when run with a baseline build
	TEST(iRecognizer->RecognizerCount()>=2);
	TEST(iRecognizer->UpdateCounter()>=2);

	//for testing ecom style plugin
	INFO_PRINTF1(_L("Testing the ecom style TTESTECOMFILEREC recognizer is loaded or not"));
	CApaScanningFileRecognizer::TRecognizer testEcomFileRec;
	testEcomFileRec.iUid=KTestEcomFileRecognizerUid;
	testEcomFileRec.iDrive=25;
	TRAP(ret,iRecognizer->SetEcomRecognizerL(testEcomFileRec));
	TEST(ret==KErrNone);
	TEST(iRecognizer->UpdateCounter()>=2);

	INFO_PRINTF1(_L("Wait a couple of seconds to stop dlls closing too early...")); // stop the dll's being closed too early
	User::After(2000000);
	
	delete iRecognizer;
	CleanupStack::PopAndDestroy(scheduler);
	}

CT_File1Step::~CT_File1Step()
/**
   Destructor
 */
	{
	}

CT_File1Step::CT_File1Step()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_File1Step);
	}

TVerdict CT_File1Step::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_File1Step::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}

TVerdict CT_File1Step::doTestStepL()
/**
  @return - TVerdict code
  Override of base class virtual
*/
	{
	INFO_PRINTF1(_L("Testing the APFILE dll..."));
	//
	// set up the directory structure
	iFs.Connect();
	setup();
	//
	// run the testcode (inside an alloc heaven harness)

 	__UHEAP_MARK;
	TRAPD(r,testScanningFileTheRecognizerL());
	TEST(r==KErrNone);
	REComSession::FinalClose();	
	__UHEAP_MARKEND;

	iFs.Close();

	INFO_PRINTF1(_L("Test completed!"));
	return TestStepResult();
	}
