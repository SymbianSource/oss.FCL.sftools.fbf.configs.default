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
// Test MRU list\n
// Tests MRU list by calling CApaProcess::SetMainDocFileName().\n
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <f32file.h>
#include <apaid.h>
#include <apgaplst.h>
#include <apgicnfl.h>
#include <apgdoor.h>
#include <apparc.h>
#include "tstapp.h"
#include <fbs.h>
#include <s32std.h> 
#include <s32stor.h> 
#include <s32file.h> 
#include <s32mem.h>
#include "T_MruStep.h"

#if !defined(__E32TEST_H__)
#include <e32test.h>
#endif


void CT_MruStep::setup()
	{
#if defined(__EPOC32__)
	// if we're on the rack create the directories we need
	TFullName filePath=_L("c:\\docs\\tstapp.doc");
	TFullName tempPath=_L("c:\\system\\temp\\");
	TParse parser;
	parser.Set(filePath,NULL,NULL);
	iFs.MkDirAll(parser.DriveAndPath());
	parser.Set(tempPath,NULL,NULL);
	iFs.MkDirAll(parser.DriveAndPath());
#endif
	}


/**
   @SYMTestCaseID T-MruStep-TestMRUL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test MRU list by calling CApaProcess::SetMainDocFileName() multiple times.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Test MRU list by calling CApaProcess::SetMainDocFileName()
   in the following cases:\n
   (1) Call CApaProcess::SetMainDocFileName() with no document created.\n
   (2) Call CApaProcess::SetMainDocFileName() after document creation. \n
   (3) Create a document for tstapp. Call CApaProcess::SetMainDocFileName()
   multiple times using the same filename to be set in the
   Most Recently Used (MRU) list. Ensure that only once the list is updated
   with the filename.\n
   (4) Create a document for tstapp. Call CApaProcess::SetMainDocFileName()
   multiple times using different filename to be set in the
   Most Recently Used (MRU) list on each attempt. Ensure that only a
   maximum of 50 filenames are present.\n
   (5) Open MRU.DAT and then call CApaProcess::SetMainDocFileName().
   The filename set should not appear in MRU list since MRU.DAT is open.\n
   (6) Create a document for tstapp. Call CApaProcess::SetMainDocFileName()
   multiple times when there is no memory available on the device.
   The MRU list should not reflect the newly set file name.\n
   (7) Test CApaRecentFile::GetRecentFileL() by passing KNullUid.
   Ensure that all entries for those last used documents are retrieved
   when KNullUid is passed.\n
   (8) Test CApaRecentFile::GetRecentFileListL() to ensure that it returns NULL.\n
   In the above mentioned situations tests ensure that
   there are no memory leaks.\n\n
   API Calls:\n	
   CApaProcess::SetMainDocFileName(const TDesC& aMainDocFileName)\n
   CApaRecentFile::GetRecentFileL(RFs& aFs, TUid aAppUid, TInt aIndex)\n
   CApaRecentFile::GetRecentFileListL(RFs& aFs, TUid aAppUid)\n
   
   @SYMTestExpectedResults Each of the tests should complete with the desired
   output and without any memory leaks.
    
 */
void CT_MruStep::TestMRUL()
	{
	CApaProcess* process=NULL;
	CApaDocument* doc=NULL;
	
	// Test 1
	INFO_PRINTF1(_L("SetMainDocFileName with no document"));
	__UHEAP_RESET;
	__UHEAP_MARK;
	process = CApaProcess::NewL(iFs);
	process->SetMainDocFileName(_L("FileName 01"));
	delete process;
	__UHEAP_MARKEND;

	// test 2
	INFO_PRINTF1(_L("SetMainDocFileName after document construction"));

	__UHEAP_RESET;
	__UHEAP_MARK;

	// Create document
	process = CApaProcess::NewL(iFs);
	TApaApplicationFactory appFact(KUidTestApp);
	doc = process->AddNewDocumentL(appFact);
	
	// Set file name into MRU list
	process->SetMainDocFileName(_L("FileName 02"));
	process->DestroyDocument(doc);
	delete process;
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	// test 3
	INFO_PRINTF1(_L("SetMainDocFileName x100 with same filename"));
	TInt count;

	__UHEAP_RESET;
	__UHEAP_MARK;
	
	process = CApaProcess::NewL(iFs);
	doc = process->AddNewDocumentL(appFact);
	
	// Set file name into MRU list - this name should only appear once
	for (count=0; count<100; count++)
		process->SetMainDocFileName(_L("FileName 03"));
		
	process->DestroyDocument(doc);
	delete process;

	REComSession::FinalClose();
	__UHEAP_MARKEND;

	// test 4
	INFO_PRINTF1(_L("SetMainDocFileName x100 with different filenames"));

	__UHEAP_RESET;
	__UHEAP_MARK;

	process = CApaProcess::NewL(iFs);
	doc = process->AddNewDocumentL(appFact);
    
	/** Set file name into MRU list - there should only be a maximum of 50 names in the list */
	TBuf<20> fileName;

	for (count=0; count < 100; count++)
		{
		fileName.Zero();
		fileName.AppendFormat(_L("FileName %d"),count);

		process->SetMainDocFileName(fileName);
		}

	process->DestroyDocument(doc);
	delete process;

	REComSession::FinalClose();
	__UHEAP_MARKEND;


	// test 7
	INFO_PRINTF1(_L("Testing GetRecentFileL with KNullUid"));

	__UHEAP_RESET;
	__UHEAP_MARK;
	
	process = CApaProcess::NewL(iFs);
	doc = process->AddNewDocumentL(appFact);

	process->DestroyDocument(doc);
	delete process;

	REComSession::FinalClose();
	__UHEAP_MARKEND;

	// MRU functionality has been removed in 7.0s test that GetRecentFile returns NULL
	// test 8
	INFO_PRINTF1(_L("Testing GetRecentFileListL returns NULL"));

	__UHEAP_RESET;
	__UHEAP_MARK;
	
	process = CApaProcess::NewL(iFs);
	doc = process->AddNewDocumentL(appFact);

	process->DestroyDocument(doc);
	delete process;

	REComSession::FinalClose();
	__UHEAP_MARKEND;
	}


/**
  Auxiliary Fn for all Test Cases.
 
  This method creates and installs an active scheduler and puts the
  test code onto the scheduler as a CIdle object. The method initiates
  all tests by calling the static method CT-MruTestCallBackWrapper::CallBack().
 
*/
void CT_MruStep::DoTestsInScheldulerLoopL()
	{
	// create an active scheduler
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	// put the test code onto the scheduler as an idle object
	CIdle* idle=CIdle::NewL(-20);
	CleanupStack::PushL(idle);

	CT_MruTestCallBackWrapper* callBack = new(ELeave) CT_MruTestCallBackWrapper(this);
	CleanupStack::PushL(callBack);

	idle->Start(TCallBack(CT_MruTestCallBackWrapper::CallBack,callBack));
	// start the test code
	CActiveScheduler::Start();

	// all outstanding requests complete - kill the scheduler
	CleanupStack::PopAndDestroy(3); //scheduler, callBack, idle
	}


CT_MruTestCallBackWrapper::CT_MruTestCallBackWrapper(CT_MruStep* aTestStep)
/**
   Constructor
 */
	{
	iTestStep=aTestStep;
	}

CT_MruTestCallBackWrapper::~CT_MruTestCallBackWrapper()
/**
   Destructor
 */
	{
	}
	
TInt CT_MruTestCallBackWrapper::CallBack(TAny* aPtr)
/**
  This static method is the callback function of CIdle object.The method
  calls the non-static method TestMRUL() which initiates all the tests. 
*/
	{
	__UHEAP_MARK;
	
	TRAPD(r,((CT_MruTestCallBackWrapper *)aPtr)->iTestStep->TestMRUL());
	__ASSERT_ALWAYS(!r,User::Panic(_L("TestMRUL"),r));
    __UHEAP_MARKEND;
  
	CActiveScheduler::Stop();
	return EFalse; // don't call back again
	}

CT_MruStep::~CT_MruStep()
/**
   Destructor
 */
	{
	}

CT_MruStep::CT_MruStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_MruStep);
	}

TVerdict CT_MruStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_MruStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}


TVerdict CT_MruStep::doTestStepL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
 	{
	INFO_PRINTF1(_L("Testing Apparch...T_Mru"));
	
	// set up an fbs
	FbsStartup();
	TInt ret=RFbsSession::Connect();
	TEST(!ret);

	// set up the directory structure
	iFs.Connect();
	setup();

	// run the testcode
	TRAP(ret,DoTestsInScheldulerLoopL())
	TEST(ret==KErrNone);
	
	iFs.Close();

	INFO_PRINTF1(_L("Testing T_Mru Completed!"));
	return TestStepResult();
	}
