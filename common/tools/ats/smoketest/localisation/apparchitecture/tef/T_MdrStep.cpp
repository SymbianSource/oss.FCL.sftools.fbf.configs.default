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
// Test for CApaDoor and CApaModelDoor Apis\n
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <f32file.h>
#include <s32std.h> 
#include <s32stor.h> 
#include <s32file.h> 
#include <s32mem.h>
#include <fbs.h>
//
#include <apamdr.h>
#include <apgdoor.h>
#include "tstapp.h"
#include "T_MdrStep.h"
//
#include <e32test.h>

/**
  Auxiliary Fn for Test Case ID T-MdrStep-testModelDoorL,
  T-MdrStep-testConversionL
 
  This method overrides CApaModelHeader::AppId(). It returns a
  TApaAppIdentifier object which identifies the application.
 
*/
TApaAppIdentifier CTestModelHeader::AppId()const
	{
	TFileName dllname=_L("tstapp.app");
	#define KTestAppId TApaAppIdentifier(KUidTestApp,dllname)
	return KTestAppId;
	}


/**
  Auxiliary Fn for Test Case ID T-MdrStep-testModelDoorL,
  T-MdrStep-testConversionL
 
  This method overrides CApaModelHeader::StoreL().It stores the
  data in the stream to a store.
 
*/
void CTestModelHeader::StoreL(CStreamStore& aStore,CStreamDictionary& aDict) const
	{
	// write out some data
	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(aStore);
	stream.WriteInt32L(iData);
	stream.WriteInt32L(0); // write testapp doc format - no subdocuments
	stream.CommitL();
	aDict.AssignL(KUidTestAppHeadStream,id);
	CleanupStack::PopAndDestroy(); // stream
	}


/**
   Auxiliary Fn for Test Case ID T-MdrStep-testModelDoorL,
   T-MdrStep-testConversionL
 
   This method reads the data stored in the store.
 
*/
void CTestModelHeader::RestoreL(const CStreamStore& aStore,const CStreamDictionary& aDict)
	{
	// read in some data
	RStoreReadStream stream;
	stream.OpenLC(aStore,aDict.At(KUidTestAppHeadStream));
	iData = stream.ReadInt32L();
	CleanupStack::PopAndDestroy(); // stream
	}


void CTestModelHeader::DetachFromStoreL(CPicture::TDetach /*aDetach*/)
	{}


/**
  Auxiliary Fn for Test Case ID T-MdrStep-testModelDoorL,
  T-MdrStep-testConversionL
 
  This method overrides MApaModelHeaderFactory::NewHeaderL().It creates and
  restores model header from the store.
 
*/
CApaModelHeader* TTestModelHeaderFactory::NewHeaderL(const CStreamStore& aStore,const CStreamDictionary& aDict,
													 const TApaAppIdentifier& aAppId)const
	{
	TFileName dllname=_L("tstapp.app");
	#define KTestAppId TApaAppIdentifier(KUidTestApp,dllname)
	if (aAppId.iAppUid!=KTestAppId.iAppUid)
		User::Leave(KErrNotSupported);
	CTestModelHeader* header = new(ELeave) CTestModelHeader();
	CleanupStack::PushL(header);
	header->RestoreL(aStore,aDict);
	CleanupStack::Pop(); // header
	return header;
	}


/**
   @SYMTestCaseID T-MdrStep-testModelDoorL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests storing and restoring of Model door.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The test creates a test model header and initializes its
   member variable iData. A model door object is created by passing
   application model wrapper object to be embedded to CApaModelDoor::NewL().
   A temporary store is created and CTestModelHeader::StoreL() is called to
   store the model door. Call CApaModelDoor::NewL() specifying the store and
   stream to restore the stored model door. Test values from the restored
   model door.\n
   API Calls:\n	
   CApaModelDoor::NewL(CApaModelHeader* aHeader)\n
   CApaModelDoor::StoreL(CStreamStore& aStore) const\n
   CApaModelDoor::NewL(const CStreamStore& aStore,TStreamId aHeadStreamId,const MApaModelHeaderFactory* aFactory)\n
   CApaModelDoor::ModelHeader()\n
   
   @SYMTestExpectedResults Test completes restoration of model door successfully.
    
 */
void CT_MdrStep::testModelDoorL()
	{
	const TUid KTestSourceId={458};
	INFO_PRINTF1(_L("Creating a model door"));

	// create a model header
	CTestModelHeader* header = new CTestModelHeader();
	TEST(header!=NULL);
	header->iData = 7;

	// embed the header in a door
	CApaModelDoor* door=NULL;
	TRAPD(ret, door=CApaModelDoor::NewL(header) );
	TEST(ret==KErrNone);

	// set the source
	door->SetSource(KTestSourceId);

	// create an in-memory store
	CBufStore* store=NULL;
	TRAP(ret, store=CBufStore::NewL(2) );
	TEST(ret==KErrNone);

	// store the door
	INFO_PRINTF1(_L("Storing the model door"));
	TStreamId id = TStreamId(NULL);
	TRAP(ret, id=door->StoreL(*store) );
	TEST(ret==KErrNone);
	delete door; // deletes header also

	// restore the door
	INFO_PRINTF1(_L("Restoring the model door"));
	TTestModelHeaderFactory factory;
	TRAP(ret, door=CApaModelDoor::NewL(*store,id,&factory) );
	TEST(ret==KErrNone);
	TEST( ((CTestModelHeader*)door->ModelHeader())->iData==7 );
	TEST((door->Source()==KTestSourceId));
	delete door;
	delete store;
	}


/**
   @SYMTestCaseID T-MdrStep-testConversionL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests conversion of door format to model door format and back. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create a new document for the tstapp and create a door for the
   document by calling CApaDoor::NewL(). Create a temporary store to store
   the door format by calling CTestModelHeader::StoreL(). Restore the door
   to a model door from the temporary store by calling CApaModelDoor::NewL().
   Delete the store and store the model door format to a newly created store.
   Change the format and size of the model door by calling CApaModelDoor::SetFormat()
   and CApaModelDoor::SetSizeInTwips(). Store the new model door format to
   the temporary store. Restore the saved model door format to a new door.
   Observe the conversion of format & size from the saved format to default
   while it is restored as door.\n
   API Calls:\n	
   CApaDoor::NewL(RFs& aFs,CApaDocument& aDoc,const TSize& aDefaultIconSizeInTwips)\n
   CApaDoor::StoreL(CStreamStore& aStore) const\n
   CApaModelDoor::NewL(const CStreamStore& aStore,TStreamId aHeadStreamId,const MApaModelHeaderFactory* aFactory)\n
   CApaModelDoor::StoreL(CStreamStore& aStore) const\n
   CApaModelDoor::SetFormat(TFormat aFormat)\n
   CApaModelDoor::SetSizeInTwips(const TSize& aSize)\n
   
   @SYMTestExpectedResults Test confirms that there is a conversion from the saved model format to
   default format while it is restored as a door.
    
 */
void CT_MdrStep::testConversionL()
	{
	INFO_PRINTF1(_L("Testing Conversions"));
	const TUid KTestSourceId={458};

	// set things up
	TRAPD(ret,iProcess = CApaProcess::NewL(iFs));
	TEST(ret==KErrNone);
	CBufStore* store=NULL;
	//
	// create a door
	INFO_PRINTF1(_L("Restoring a full door as a model door"));
	CApaDocument* doc=NULL;
	TRAP(ret,doc=iProcess->AddNewDocumentL(KUidTestApp));

	TEST(ret==KErrNone);
	doc->EditL(NULL); // increments value to 1
	CApaDoor* door=NULL;
	TRAP(ret, door=CApaDoor::NewL(iFs,*doc,TSize(1000,1000)) );
	TEST(ret==KErrNone);

	// set the source
	door->SetSource(KTestSourceId);

	// create an in-memory store
	TRAP(ret, store=CBufStore::NewL(2) );
	TEST(ret==KErrNone);

	// store the door
	TStreamId id = TStreamId(NULL);
	TRAP(ret, id=door->StoreL(*store) );
	TEST(ret==KErrNone);
	delete door; // deletes doc also
	door = NULL;
	doc = NULL;

	// restore the door into a model door
	TTestModelHeaderFactory factory;
	CApaModelDoor* modelDoor=NULL;
	TRAP(ret, modelDoor=CApaModelDoor::NewL(*store,id,&factory) );
	TEST(ret==KErrNone);
	TEST( ((CTestModelHeader*)modelDoor->ModelHeader())->iData==1 );
	TEST(modelDoor->Format()==CApaDoorBase::EIconic);
	TEST(modelDoor->Source()==KTestSourceId);
	TSize size;
	modelDoor->GetSizeInTwips(size);
	TEST(size==TSize(1000,1000));

	// store the model door
	INFO_PRINTF1(_L("Restoring a model door as a full door"));
	delete store;
	store = NULL;
	TRAP(ret, store=CBufStore::NewL(2) );
	TEST(ret==KErrNone);
	TRAP(ret, id=modelDoor->StoreL(*store) );
	TEST(ret==KErrNone);

	// restore the model door into a full door
	TRAP(ret, door=CApaDoor::NewL(iFs,*store,id,*iProcess) );	
	TEST(ret==KErrNone);
	delete door;
	door = NULL;
	delete store;
	store = NULL;

	// change the model door format to glass & store it
	TRAP(ret, store=CBufStore::NewL(2) );
	TEST(ret==KErrNone);
	modelDoor->SetFormat(CApaDoorBase::EGlassDoor);
	modelDoor->SetSizeInTwips(TSize(2500,27));
	TRAP(ret, id=modelDoor->StoreL(*store) );
	TEST(ret==KErrNone);

	delete modelDoor; // deletes header also
	modelDoor = NULL;

	// restore into a full door - this should switch the format to iconic and use the default icon size
	TRAP(ret, door=CApaDoor::NewL(iFs,*store,id,*iProcess) );	
	TEST(ret==KErrNone);
	TEST(door->Format()==CApaDoorBase::EIconic);
	TEST(door->Source()==KTestSourceId);
	door->GetSizeInTwips(size);
	TEST(size==TSize(500,500));
	delete door;
	door = NULL;
	delete store;
	store = NULL;

	delete iProcess;
	}


// Tests various functions to increase API test coverage

/**
   @SYMTestCaseID T-MdrStep-testMiscellaneousL
  
   @SYMPREQ
 
   @SYMTestCaseDesc Tests capabalities of CApaDoor Apis. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create a document for tstapp. Create a door to this document.\n
   Test the following APIs:\n
   CApaDoor::Capability() const\n
   CApaDoor::AppUidL() const\n
   CApaDoor::SetFormatToTemporaryIconL()\n
   CApaDoor::SetCropInTwips()\n
   CApaDoor::SetScaleFactor()\n
   API Calls:\n	
   CApaDoor::Capability() const\n
   CApaDoor::AppUidL() const\n
   CApaDoor::SetFormatToTemporaryIconL(TBool aEnabled=ETrue)\n
   CApaDoor::SetCropInTwips(const TMargins& aMargins)\n
   CApaDoor::SetScaleFactor(TInt aScaleFactorWidth,TInt aScaleFactorHeight)\n
   
   @SYMTestExpectedResults Test checks results against expected values.
    
 */
void CT_MdrStep::testMiscellaneousL()
	{
	INFO_PRINTF1(_L("Testing Misc."));
	const TUid KTestSourceId={458};
	
	// set things up
	TRAPD(ret,iProcess = CApaProcess::NewL(iFs));
	
	CApaDocument* doc=NULL;
	TApaApplicationFactory appFact(KUidTestApp);
	TRAP(ret,doc=iProcess->AddNewDocumentL(appFact));

	TEST(ret==KErrNone);
	doc->EditL(NULL); // increments value to 1
	CApaDoor* door=NULL;
	TRAP(ret, door=CApaDoor::NewL(iFs,*doc,TSize(1000,1000)));
	TEST(ret==KErrNone);

	// set the source
	door->SetSource(KTestSourceId);

	INFO_PRINTF1(_L("Testing CApaDoor's picture capability"));
	TPictureCapability  pictCap = door->Capability();
	TEST(ret==KErrNone);
	TEST(pictCap.iScalingType == TPictureCapability::ENotScaleable);
	TEST(pictCap.iIsCroppable == EFalse);

	INFO_PRINTF1(_L("Testing CApaDoor AppUidL"));
	TUid uid = door->AppUidL();
	TEST(uid ==KUidTestApp);
	
	INFO_PRINTF1(_L("Testing SetFormatToTemporaryIconL"));
	TRAP(ret, door->SetFormatToTemporaryIconL(ETrue));
	TEST(ret==KErrNone);

	INFO_PRINTF1(_L("Testing SetCropInTwips"));
	TMargins margins;
	TRAP(ret, door->SetCropInTwips(margins));
	TEST(ret==KErrNone);

	INFO_PRINTF1(_L("Testing SetScaleFactor"));
	TRAP(ret, door->SetScaleFactor(500, 500));
	TEST(ret==KErrNone);

	delete door;
	delete iProcess;
	}


/**
  Auxiliary Fn for entire Test Step. 

  This method creates and installs an active scheduler and puts the
  test code on the scheduler as a CIdle object. The method initiates all
  tests by calling the static method CT-MdrTestCallBackWrapper::CallBack().
 
*/
void CT_MdrStep::DoTestsInScheldulerLoopL()
	{
	// create an active scheduler
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	// put the test code onto the scheduler as an idle object
	CIdle* idle=CIdle::NewL(-20);
	CleanupStack::PushL(idle);

	CT_MdrTestCallBackWrapper* callBack = new(ELeave) CT_MdrTestCallBackWrapper(this);
	CleanupStack::PushL(callBack);

	idle->Start(TCallBack(CT_MdrTestCallBackWrapper::CallBack,callBack));
	// start the test code
	CActiveScheduler::Start();

	// all outstanding requests complete - kill the scheduler
	CleanupStack::PopAndDestroy(3); //scheduler, callBack, idle
	}



CT_MdrTestCallBackWrapper::CT_MdrTestCallBackWrapper(CT_MdrStep* aTestStep)
/**
   Constructor
 */
	{
	iTestStep=aTestStep;
	}


CT_MdrTestCallBackWrapper::~CT_MdrTestCallBackWrapper()
/**
   Destructor
 */
	{
	}

TInt CT_MdrTestCallBackWrapper::CallBack(TAny* aPtr)
/**
  This static method is the callback function of CIdle object. The method
  calls the non-static method DoStepTests() which initiates all the tests. 
*/
	{
	((CT_MdrTestCallBackWrapper *)aPtr)->iTestStep->DoStepTests();

	CActiveScheduler::Stop();
	return EFalse; // don't call back again
	}

CT_MdrStep::~CT_MdrStep()
/**
   Destructor
 */
	{
	}

CT_MdrStep::CT_MdrStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_MdrStep);
	}

TVerdict CT_MdrStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_MdrStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}


TVerdict CT_MdrStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing model doors..."));

	// set up an fbs
	FbsStartup();

	// set up the directory structure
	iFs.Connect();

	// run the testcode
	TRAPD(ret,DoTestsInScheldulerLoopL())
	TEST(ret==KErrNone);
	
	iFs.Close();
	//delete TheTrapCleanup;
	return TestStepResult();
	}

/**
  Auxiliary Fn for entire Test Step.
 
  The method initiates all tests to be performed.
 
*/
void CT_MdrStep::DoStepTests()
	{
	__UHEAP_MARK;
	TRAPD(r,testModelDoorL());
	TEST(r==KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TInt ret=RFbsSession::Connect();
	TEST(ret==KErrNone);
	TRAP(r,testConversionL());

		TEST(r==KErrNone);
/** The memory that is allocated in AllocScanLineBuffer() is shared 
    between all bitmaps using the same session and is only released when 
    RFbsSession::Disconnect();  is called.  */
	RFbsSession::Disconnect();
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	ret=RFbsSession::Connect();
	TEST(ret==KErrNone);
	TRAP(r,testMiscellaneousL());
	TEST(r==KErrNone);
	RFbsSession::Disconnect();
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	}
