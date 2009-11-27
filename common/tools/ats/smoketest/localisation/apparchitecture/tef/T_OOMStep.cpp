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
// Performs the Out of Memory Tests.\n
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
#include "tstapp.h"
#include <fbs.h>
#include <s32std.h> 
#include <s32stor.h> 
#include <s32file.h> 
#include <s32mem.h>
#include "T_OOMStep.h"
//
#if !defined(__E32TEST_H__)
#include <e32test.h>
#endif


void CT_OOMStep::setup()
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
  Auxiliary Fn for Test Case ID T-OOMStep-TestOOMConstructionL 
 
  This function adds a new document to the app DLL and associates a 
  temporary store. The document is then initialized with the default settings.
*/
CApaDocument* CT_OOMStep::CreateTestDocL(CApaProcess* aProcess)
	{

	TFileName dllname=_L("tstapp.app");
	TFullName filePath=_L("c:\\docs\\tstapp.doc");
	// create a new main doc of type tstapp
	CApaDocument* doc=NULL;
	
	TApaApplicationFactory appFact(KUidTestApp);
	doc=aProcess->AddNewDocumentL(appFact);

	aProcess->SetMainDocument(doc);
	//
	// create the store and initialise it
	CFileStore* store = doc->CreateFileStoreLC(aProcess->FsSession(),filePath);
	CleanupStack::Pop(); // store
	((CTestAppDoc*)aProcess->MainDocument())->iStore = store;
	aProcess->SetMainDocFileName(filePath);
	//
	// initialise the document with factory settings
	doc->NewDocumentL();
	//
	return doc;
	}


/**
  Auxiliary Fn for Test Case ID T-OOMStep-TestOOMConstructionL 
 
  This function creates a test document for app DLL and saves
  the document.
  
*/
void CT_OOMStep::CreateTestDocFileL()
	{

	TFullName filePath=_L("c:\\docs\\tstapp.doc");
	// delete the file to be created by the testcode
	iFs.Delete(filePath);
	iFs.MkDir(_L("c:\\docs\\"));

	// create an appfinder and process
	CApaProcess* process = CApaProcess::NewL(iFs);
	CApaDocument* doc = CreateTestDocL(process);

	// save it 
	doc->SaveL();

	// tidy up
	process->DestroyDocument(doc);
	delete process;
	}

/**
   @SYMTestCaseID T-OOMStep-TestOOMConstructionL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test methods CApaScanningAppFinder::NewL, CApaProcess::NewL,
   CApaProcess::AddNewDocumentL, CApaProcess::OpenNewDocumentL and CApaDocument::SaveL
   when device goes Out of Memory.
   
   @SYMTestPriority High 
 
   @SYMTestStatus Implemented
   
   @SYMTestActions \n OOM Test for CApaScanningAppFinder::NewL:\n
   Use macro __UHEAP_SETFAIL() to simulate the out of memory
   situation. Call CApaScanningAppFinder::NewL() to allocate memory. Observe
   the leave that occurs. Observe after how many allocations the leave occurs.\n\n
   OOM Test for CApaProcess::NewL:\n
   Use macro __UHEAP_SETFAIL() to simulate the out of memory situation. Call
   CApaProcess::NewL() to allocate memory. Observe the leave that occurs.
   Observe after how many allocations the leave occurs.\n\n
   OOM Test for CApaProcess::AddNewDocumentL:\n
   Use macro __UHEAP_SETFAIL() to simulate the out of memory situation. Call
   CApaProcess::AddNewDocumentL() to allocate memory. Observe the leave that
   occurs. Observe after how many allocations the leave occurs.\n\n
   OOM Test for CApaProcess::OpenNewDocumentL:\n
   Use macro __UHEAP_SETFAIL() to simulate the out of memory situation. Call
   CApaProcess::OpenNewDocumentL() to allocate memory. Observe the leave that
   occurs. Observe after how many allocations the leave occurs.\n\n
   OOM Test for CApaDocument::SaveL:\n
   Create and initialize a document for tstapp. Use macro __UHEAP_SETFAIL()
   to simulate the out of memory situation. Call CApaDocument::SaveL() to
   allocate memory for the document to be saved. Observe the leave that occurs.
   Observe after how many allocations the leave occurs.\n\n
   API Calls:\n	
   CApaScanningAppFinder::NewL(const RFs& aFs)\n
   CApaProcess::NewL(const RFs& aFs,CApaAppFinder& aAppFinder)\n
   CApaProcess::AddNewDocumentL(const TDesC& aDllFileName,TUid aDllUid=KNullUid)\n
   CApaProcess::OpenNewDocumentL(CFileStore*& aStore,CStreamDictionary*& aStreamDic,const TDesC& aDocFullFileName,TUint aFileMode)\n
   CApaDocument::SaveL()\n
   
   @SYMTestExpectedResults Tests should complete without any memory leaks. It should
   also observe the no of allocations which occur before OOM situation.
    
 */
void CT_OOMStep::TestOOMConstructionL()
	{
	TInt failRate=0;
	TFullName filePath=_L("c:\\docs\\tstapp.doc");

	__UHEAP_RESET;

	// CApaProcess
	INFO_PRINTF1(_L("CApaProcess construction under OOM"));
	CApaProcess* process=NULL;
	for (failRate=1;;failRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;

		TRAPD(ret, process = CApaProcess::NewL(iFs));
		
			TEST((ret==KErrNone || ret==KErrNoMemory));
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			TEST(process==NULL);
			}
		else
			{
			TEST(process!=NULL);
			delete process;
			process = NULL;
			REComSession::FinalClose();
			__UHEAP_MARKEND;
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d\n"),failRate-1);
	//
	// creating CApaDocument
	INFO_PRINTF1(_L("CApaDocument construction under OOM"));
	CApaDocument* doc=NULL;
	//
	for (failRate=1;;failRate++)
		{
		__UHEAP_RESET;
		process = CApaProcess::NewL(iFs);		
			TEST(process!=NULL);
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		
		TApaApplicationFactory appFact(KUidTestApp);
		TRAPD(ret,doc=process->AddNewDocumentL(appFact));

		TEST((ret==KErrNone || ret==KErrNoMemory));
		if (ret!=KErrNone)
			{
			delete process;
			REComSession::FinalClose();
			__UHEAP_MARKEND;
			TEST(doc==NULL);
			}
		else
			{
			TEST(doc!=NULL);
			process->DestroyDocument(doc);
			delete process;
			REComSession::FinalClose();
			__UHEAP_MARKEND;
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d\n"),failRate-1);
	//
	// opening a CApaDocument
	INFO_PRINTF1(_L("CApaDocument Restoration under OOM"));
	TRAPD(err,CreateTestDocFileL());
		TEST(err==KErrNone);
	//
	for (failRate=1;;failRate++)
		{
		__UHEAP_RESET;
		doc = NULL;
		process = CApaProcess::NewL(iFs);		

		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		CFileStore* store=NULL;
		CStreamDictionary* streamDic=NULL;
		TRAPD(ret, doc=process->OpenNewDocumentL(store,streamDic,filePath,EFileShareExclusive|EFileWrite));
			TEST((ret==KErrNone || ret==KErrNoMemory));
		if (ret!=KErrNone)
			{
			delete streamDic;
			delete store;
			delete process;
			REComSession::FinalClose();
			__UHEAP_MARKEND;
			TEST(doc==NULL);
			}
		else
			{
			TEST(doc!=NULL);
			TEST(streamDic!=NULL);
			TEST(store!=NULL);
			delete streamDic;
			delete store;
			process->DestroyDocument(doc);
			delete process;
			REComSession::FinalClose();
			__UHEAP_MARKEND;
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d\n"),failRate-1);
	//
	// saving a CApaDocument
	INFO_PRINTF1(_L("CApaDocument Storing under OOM"));
	//
	for (failRate=1;;failRate++)
		{
		__UHEAP_RESET;
		// delete the file to be created by the testcode
		iFs.Delete(filePath);
		process = CApaProcess::NewL(iFs);		
		doc = CreateTestDocL(process);
		// attempt to save
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		TRAPD(ret, doc->SaveL());
			TEST((ret==KErrNone || ret==KErrNoMemory));
		if (ret!=KErrNone)
			{
			process->DestroyDocument(doc);
			delete process;
			REComSession::FinalClose();
			__UHEAP_MARKEND;
			}
		else
			{
			process->DestroyDocument(doc);
			delete process;
			REComSession::FinalClose();
			__UHEAP_MARKEND;
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d\n"),failRate-1);
	//
	}

/**
   @SYMTestCaseID T-OOMStep-TestDoorOOML
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test CApaDoor APIs NewL(), StoreL(), Restore() when device
   goes Out of Memory.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The method simulates an OOM scenario to test CApaDoor APIs NewL(),
   StoreL() and Restore().\n\n
   To test CApaDoor::NewL(), the method creates a process and adds a new document
   to app DLL using AddNewDocumentL(). Using the macro __UHEAP_SETFAIL() OOM
   scenario is simulated. Call CApaDoor::NewL() to observe the leave returned.\n\n
   To test CApaDoor::StoreL(), the method creates a process and adds a new 
   document to app DLL using AddNewDocumentL(). It calls CApaDoor::NewL() to
   create a wrapper for the document. A temporary buffer store is created.
   Now Use macro __UHEAP_SETFAIL() to simulate the OOM scenario.
   Call CApaDoor::StoreL() to store the document in the specified store.
   Observe the leave returned.\n\n
   To test CApaDoor::Restore(), the method creates a process and adds a new
   document to app DLL using AddNewDocumentL(). It calls CApaDoor::NewL()
   to create a wrapper for the document. A temporary buffer store is created.
   Now Call CApaDoor::StoreL() to store the document in the specified store.
   Use macro __UHEAP_SETFAIL() to simulate the OOM scenario. Call
   CApaDoor::RestoreL() to restore the document from the specified store.
   Observe the leave returned.\n
   API Calls:\n	
   CApaDoor::NewL(RFs& aFs, CApaDocument& aDoc,const TSize& aDefaultIconSizeInTwips)\n
   CApaDoor::StoreL(CStreamStore& aStore) const\n
   CApaDoor::RestoreL(const CStreamStore& aStore,TStreamId aHeadStreamId)\n
   
   @SYMTestExpectedResults Tests should complete without any memory leaks.
    
 */
void CT_OOMStep::TestDoorOOML()
	{
	CApaProcess* process=NULL;
	CApaDocument* doc=NULL;
	CApaDoor* door=NULL;
	TInt failRate=0;
	//
	INFO_PRINTF1(_L("CApaDoor construction under OOM"));
	//
	__UHEAP_MARK;
	//Create a session with F & B server
	TInt ret=RFbsSession::Connect();
		TEST(!ret);
	
	for (failRate=1;;failRate++)
		{
		__UHEAP_RESET;
		door = NULL;
		process = CApaProcess::NewL(iFs);
		TApaApplicationFactory appFact(KUidTestApp);
		doc=process->AddNewDocumentL(appFact);

		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);

		door = NULL;
		TRAPD(ret,door=CApaDoor::NewL(iFs,*doc,TSize(400,400)));
			TEST((ret==KErrNone || ret==KErrNoMemory));
		if (ret!=KErrNone)
			{
			delete process;
			TEST(door==NULL);
			}
		else
			{
			TEST(door!=NULL);
			delete door;

			delete process;
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d\n"),failRate-1);
	//
	INFO_PRINTF1(_L("CApaDoor Store() under OOM"));
	//
	CBufStore* store=NULL;
	for (failRate=1;;failRate++)
		{
		__UHEAP_RESET;
		door = NULL;
		process = CApaProcess::NewL(iFs);	
		TApaApplicationFactory appFact(KUidTestApp);
		doc=process->AddNewDocumentL(appFact);	
		
		door = CApaDoor::NewL(iFs,*doc,TSize(400,400)); // owns doc
		store = CBufStore::NewL(10);
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);

		TStreamId id=KNullStreamId;
		TRAPD(ret,id=door->StoreL(*store));
			TEST((ret==KErrNone || ret==KErrNoMemory));
		if (ret!=KErrNone)
			{
			TEST(id==KNullStreamId);
			delete store;
			delete door;

			delete process;
			}
		else
			{
			TEST(id!=KNullStreamId);
			delete store;

			delete door;
			delete process;
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs: %d\n"),failRate-1);
	//
	INFO_PRINTF1(_L("CApaDoor Restore() under OOM"));
	//
	for (failRate=1;;failRate++)
		{
		__UHEAP_RESET;
		door = NULL;
		process = CApaProcess::NewL(iFs);
		TApaApplicationFactory appFact(KUidTestApp);
		doc = process->AddNewDocumentL(appFact);
		door = CApaDoor::NewL(iFs,*doc,TSize(400,400)); // owns doc
		store = CBufStore::NewL(10);
		TStreamId id = door->StoreL(*store);
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);

		TRAPD(ret,door->RestoreL(*store,id));
			TEST((ret==KErrNone || ret==KErrNoMemory));
		if (ret!=KErrNone)
			{
			delete store;
			delete door;

			delete process;
			}
		else
			{
			delete store;
			delete door;

			delete process;
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs: %d\n"),failRate-1);
	//
	//Close the session F & B server.
	RFbsSession::Disconnect();
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	}


/**
  Auxiliary Fn for all Test Cases.
 
  This method creates and installs an active scheduler and puts the
  test code on the scheduler as a CIdle object. The method initiates
  all tests by calling the static method CT-OOMStepCallBack::CallBack().
 
*/
void CT_OOMStep::DoTestsInCallBackL()
	{
	// create an active scheduler
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	// put the test code onto the scheduler as an idle object
	CIdle* idle=CIdle::NewL(-20);
	CleanupStack::PushL(idle);

	CT_OOMStepCallBack* callBack = new(ELeave) CT_OOMStepCallBack(this);
	CleanupStack::PushL(callBack);

	idle->Start(TCallBack(CT_OOMStepCallBack::CallBack,callBack));
	// start the test code
	CActiveScheduler::Start();

	// all outstanding requests complete - kill the scheduler
	CleanupStack::PopAndDestroy(3); //scheduler, callBack, idle
	}



CT_OOMStepCallBack::CT_OOMStepCallBack(CT_OOMStep* aTestStep)
{
	iTestStep = aTestStep;
}


CT_OOMStepCallBack::~CT_OOMStepCallBack()
{

}
/**
  Auxiliary Fn for all Test Cases.
 
  The method initiates all tests to be performed.
 
*/
void CT_OOMStep::DoStepTests()
{
	__UHEAP_MARK;
	
	INFO_PRINTF1(_L("About to test OOM resilience"));
	TRAPD(r,TestOOMConstructionL());
		TEST(r==KErrNone);
	__UHEAP_MARKEND;
	
 	__UHEAP_MARK;
	INFO_PRINTF1(_L("About to test door OOM resilience"));
	TRAP(r,TestDoorOOML());
		TEST(r==KErrNone);
	__UHEAP_MARKEND;
}

TInt CT_OOMStepCallBack::CallBack(TAny* callBack/*aThis*/)
/**
  This static method is the callback function of CIdle object. The method
  calls the non-static method DoStepTests() which initiates all the tests. 
*/
	{

	//Do Tests.
	((CT_OOMStepCallBack *)callBack)->iTestStep->DoStepTests();

	CActiveScheduler::Stop();
	return EFalse; // don't call back again
	}




CT_OOMStep::~CT_OOMStep()
/**
   Destructor
 */
	{
	}

CT_OOMStep::CT_OOMStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_OOMStep);
	}

TVerdict CT_OOMStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_OOMStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}

TVerdict CT_OOMStep::doTestStepL()
/**
  @return - TVerdict code
  Override of base class virtual
*/
{
	INFO_PRINTF1(_L("Testing Apparch...OOM tests"));
	//
	// set up an fbs
	FbsStartup();
	//
	// set up the directory structure
	iFs.Connect();
	setup();
	//
	// run the testcode

	TRAPD(ret,DoTestsInCallBackL())
		TEST(ret==KErrNone);
	
	iFs.Close();
	return TestStepResult();
}
