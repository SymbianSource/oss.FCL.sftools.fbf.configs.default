// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests Application Apparc base classes and utility functions to get application's data.\n
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <f32file.h>
#include <fbs.h>
#include <s32std.h> 
#include <s32stor.h> 
#include <s32file.h> 
#include <ecom.h>

#include <apaid.h>
#include "..\apparc\apadll.h"
#include "..\apgrfx\apgstd.h"
#include <apgaplst.h>
#include <apgicnfl.h>
#include <apgdoor.h>
#include <apfrec.h>
#include <apfctlf.h>
#include <apgctl.h>
#include <apgaplst.h>
#include <apaflrec.h>
#include "testableapalssession.h"
#include <apacmdln.h>

#include "T_ProStep.h"
#include "tstapp.h"
#include <appfwk_test_utils.h>

#if !defined(__E32TEST_H__)
#include <e32test.h>
#endif

TInt PanicTestThread(TAny* aOption);

enum TPanicOption 
	{
	EFirst,
	ENegativePanicTest,
	EUnknownPanicOption,	
	ELast
	};


_LIT(KCTLDIR,"C:\\private\\10003a3f\\import\\apps\\");
_LIT(KSOURCEPATH,"z:\\private\\10003a3f\\import\\apps\\m_ctrl_reg.rsc");
_LIT(KNEWCTLPATH,"C:\\private\\10003a3f\\import\\apps\\m_ctrl_reg.rsc");
_LIT(KNEWPATH,"C:\\cm.txt");
_LIT(KEMPTYFILEPATH,"z:\\system\\data\\Testpath\\FilterTests\\testfile1.txt");

_LIT(KRSCDIR,"C:\\Resource\\apps\\");
_LIT(KLOCPATH,"z:\\Resource\\apps\\M_ctrl_loc.rsc");
_LIT(KNEWLOCPATH,"C:\\Resource\\apps\\M_ctrl_loc.rsc");
_LIT(KCTRLNAME,"C:\\sys\\bin\\m_ctrl.exe");
TFileName ctlPath=_L("z:\\sys\\bin\\m_ctrl.exe");


LOCAL_D TInt SimulateKeyL(TAny*)
	{
	User::After(3000000);
	RWsSession session;
	User::LeaveIfError(session.Connect());	

	//control down
	TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::EKeyDown,EStdKeyLeftCtrl);
	session.SimulateRawEvent(rawEvent);

	// e down
	rawEvent.Set(TRawEvent::EKeyDown,'E');
	session.SimulateRawEvent(rawEvent);

	// e up
	rawEvent.Set(TRawEvent::EKeyUp,'E');
	session.SimulateRawEvent(rawEvent);

	//control up
	rawEvent.Set(TRawEvent::EKeyUp,EStdKeyLeftCtrl);
	session.SimulateRawEvent(rawEvent);

	session.Flush();
	session.Close();
	
	return KErrNone;
	}

void CT_ProStep::setup()
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
   @SYMTestCaseID T-ProStep-testAppIdentifierL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests TApaAppIdentifier API.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create a temporary store. Construct an application identifier
   for the specified application DLL, and Uid. Call WriteAppIdentifierL() to
   store application identifier details. Read the stored application identifier
   details.\n
   API Calls:\n	
   TApaAppIdentifier::TApaAppIdentifier(TUid aAppUidType,const TFileName& aDllName)\n
   CApaProcess::WriteAppIdentifierL(CStreamStore& aStore,CStreamDictionary& aStreamDic,const TApaAppIdentifier& aAppId)\n
   CApsProcess::ReadAppIdentifierL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic)\n
  
   @SYMTestExpectedResults Test checks whether stored and read application
   identifier details match.
   
 */
void CT_ProStep::testAppIdentifierL()
	{
	const TFullName tempPath=_L("c:\\system\\temp\\");
	const TUid testUid={1};

	INFO_PRINTF1(_L("Testing TApaAppIdentifier"));
	
	// create a process
	TRAPD(ret,iProcess = CApaProcess::NewL(iFs));	
	TEST(ret==KErrNone);
	
	// create a temporary store and stream dictionary
	TParse newFilePath;
	HBufC* related = HBufC::NewL(20);
	(*related)=_L("temp.idf");
	newFilePath.Set(tempPath,related,NULL);
	delete related;
	iFs.MkDirAll(newFilePath.DriveAndPath());
	CDirectFileStore* store = CDirectFileStore::ReplaceLC(iFs,newFilePath.FullName(),EFileWrite);
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid,testUid,testUid));
	CStreamDictionary* streamDic=CStreamDictionary::NewL();
	CleanupStack::PushL(streamDic);

	// set up an id
	TApaAppIdentifier* origId=new(ELeave) TApaAppIdentifier (testUid,_L("FileName"));
	CleanupStack::PushL(origId);

	// write it out and read it in again
	iProcess->WriteAppIdentifierL(*store,*streamDic,*origId);
	TApaAppIdentifier* copyId=new(ELeave) TApaAppIdentifier();
	*copyId=iProcess->ReadAppIdentifierL(*store,*streamDic);
	
	CleanupStack::PushL(copyId);
	TEST((origId->iAppUid==copyId->iAppUid));
	TEST((origId->iFullName==copyId->iFullName));
		
	// tidy up
	CleanupStack::PopAndDestroy(4); // store,streamDic,origId,copyId
	delete iProcess;
	iFs.Delete(newFilePath.FullName());
	}


/**
   @SYMTestCaseID T-ProStep-doTestGenerateFileName
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaApplication::GenerateFileName().
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GenerateFileName() under following scenarios:\n
   (1) when root name doesn't exist\n
   (2) when root name exists\n
   (3) when illegal paths are mentioned.\n
   (4) when no memory is available to perfom the operation.\n
   API Calls:\n	
   CApaApplication::GenerateFileName(RFs& aFs,TFileName& aRootName)\n
   
   @SYMTestExpectedResults Test should return the desired filename or error
   code for each scenario
   
 */
void CT_ProStep::doTestGenerateFileName()
	{
	TFileName rootName;

	// delete the test files just in case...
	iFs.Delete(_L("c:\\path\\name"));
	iFs.Delete(_L("c:\\path\\name.ext"));

	// try it with a root that doesn't exist
	rootName = _L("c:\\path\\name");
	TInt ret=CApaApplication::GenerateFileName(iFs,rootName);
	TEST(ret==KErrNone);
	TEST(rootName.CompareF(_L("c:\\path\\name"))==0);

	// try it with a root (inc ext) that doesn't exist
	rootName = _L("c:\\path\\name.ext");
	ret=CApaApplication::GenerateFileName(iFs,rootName);
	TEST(ret==KErrNone);
	TEST(rootName.CompareF(_L("c:\\path\\name.ext"))==0);

	// create a couple of files
	RFile file;
	file.Create(iFs,_L("c:\\path\\name"),EFileWrite);
	file.Close();
	file.Create(iFs,_L("c:\\path\\name.ext"),EFileWrite);
	// file.Close(); // purposely don't close this one

	// try it with a root that exists
	rootName = _L("c:\\path\\name");
	ret=CApaApplication::GenerateFileName(iFs,rootName);
	TEST(ret==KErrNone);
	TEST(rootName.CompareF(_L("c:\\path\\name(01)"))==0);

	// try it with a root (inc ext) that exists
	rootName = _L("c:\\path\\name.ext");
	ret=CApaApplication::GenerateFileName(iFs,rootName);
	TEST(ret==KErrNone);
	TEST(rootName.CompareF(_L("c:\\path\\name(01).ext"))==0);

	// try with illegal paths
	rootName = _L("c:name");
	ret=CApaApplication::GenerateFileName(iFs,rootName);
	TEST((ret==KErrArgument));
	rootName = _L("\\dir\\name");
	ret=CApaApplication::GenerateFileName(iFs,rootName);
	TEST((ret==KErrArgument));
	rootName = _L("c:\\dir\\");
	ret=CApaApplication::GenerateFileName(iFs,rootName);
	TEST((ret==KErrBadName));

	// try when no memory is available to perform the operation. 
	// 	(we haven't used a conventional OOM loop, but this is not a leaving function)
	__UHEAP_MARK;
	__UHEAP_SETFAIL(RHeap::EDeterministic,1);
	rootName = _L("c:\\path\\nomemory.ext");
	ret=CApaApplication::GenerateFileName(iFs,rootName);
	TEST(ret==KErrNoMemory);
	__UHEAP_MARKEND;
	__UHEAP_RESET;

	// tidy up
	file.Close();
	iFs.Delete(_L("c:\\path\\name"));
	iFs.Delete(_L("c:\\path\\name.ext"));
	
	}

/**
   @SYMTestCaseID APPFWK-APPARC-0055
  
   @SYMDEF DEF092509
  
   @SYMTestCaseDesc Tests CApaProcess::ReadRootStreamLC ().
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call ReadRootStreamLC() under following scenarios:\n
   (1) when File name Doesnt Exists\n
   (2) When an empty file is present \n
 
   
   
   @SYMTestExpectedResults Test should return  KErrNotFound for scenario (1). And  error 
   KErrEof orKErrCorrupt for scenario (2).
      
 */
 
 void CT_ProStep::testReadStreamL()

	{
	CFileStore* docStore=NULL;
	TUint appFileMode=EFileRead|EFileWrite;
	CStreamDictionary* streamDic=NULL;
	TRAPD(err,streamDic=CApaProcess::ReadRootStreamLC(iFs,docStore,KNEWPATH,appFileMode));
	TEST(err==KErrNotFound);

	delete docStore;
	delete streamDic;
	docStore=NULL;
	streamDic=NULL;
	appFileMode=EFileRead;
	TRAP(err,streamDic=CApaProcess::ReadRootStreamLC(iFs,docStore,KEMPTYFILEPATH,appFileMode));
	TEST(err==KErrCorrupt || err==KErrEof); 
	}
/**
   @SYMTestCaseID T-ProStep-testDocCreation1L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaProcess::AddNewDocumentL().
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create a process, call AddNewDocumentL() to create a new
   document for an app DLL by\n
   (1) passing full path of app.\n
   (2) passing full path of app and the Uid.\n
   In both cases call DestroyDocument() to destroy the document thus created.\n
   API Calls:\n	
   CApaProcess::NewL(const RFs& aFs,CApaAppFinder& aAppFinder)\n
   CApaProcess::AddNewDocumentL(const TDesC& aDllFileName,TUid aDllUid=KNullUid)\n
   CApaProcess::DestroyDocument(CApaDocument* aDoc)\n
   CApaProcess::ResetL()\n
   
   @SYMTestExpectedResults Test checks results against expected values.
   
 */
void CT_ProStep::testDocCreation1L()
	{
	const TFullName filePath=_L("c:\\docs\\tstapp.doc");

	INFO_PRINTF1(_L("Testing CApaProcess"));

	iFs.MkDirAll(filePath);
	// delete the file to be created by the testcode
	iFs.Delete(filePath);

	// create a process
	TRAPD(ret,iProcess = CApaProcess::NewL(iFs));
	TEST(ret==KErrNone);

	// create a new doc passing the full path of the app dll
	CApaDocument* doc=NULL;
	TApaApplicationFactory appFact(KUidTestApp);
	TRAP(ret,doc=iProcess->AddNewDocumentL(appFact));
	TEST(ret==KErrNone);
	iProcess->DestroyDocument(doc);
	doc = NULL;
	TRAP(ret,iProcess->ResetL());
	TEST(ret==KErrNone);
	}


/**
   @SYMTestCaseID T-ProStep-testDocCreation2L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaProcess::AddNewDocumentL().
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call AddNewDocumentL() to create a new document for an app DLL.
   Set it as the main document. Associate it with a temporary store. Initialize
   the document to default settings. Store the document.\n
   API Calls:\n	
   CApaProcess::AddNewDocumentL(const TDesC& aDllFileName,TUid aDllUid=KNullUid)\n
   CApaDocument::NewDocumentL()\n
   CApaDocument::SaveL()\n
   
   @SYMTestExpectedResults Test checks results against expected values.
   
 */
void CT_ProStep::testDocCreation2L()
	{
	const TFullName filePath=_L("c:\\docs\\tstapp.doc");
	CApaDocument* doc=NULL;
	
	TApaApplicationFactory appFact(KUidTestApp);
	TRAPD(ret,doc=iProcess->AddNewDocumentL(appFact));
	TEST(ret==KErrNone);
	iProcess->SetMainDocument(doc);
	CFileStore* store=NULL;

	// create the store and initialise it
	TRAP(ret, {
		store=doc->CreateFileStoreLC(iProcess->FsSession(),filePath);
		CleanupStack::Pop(); // store 
		});
	TEST(ret==KErrNone);
	((CTestAppDoc*)iProcess->MainDocument())->iStore = store;
	iProcess->SetMainDocFileName(filePath);

	// initialise the document with factory settings
	TRAP(ret, doc->NewDocumentL() );
	TEST(ret==KErrNone);

	// close it 
	TRAP(ret,doc->SaveL());
	TEST(ret==KErrNone);
	TRAP(ret, iProcess->ResetL() );
	TEST(ret==KErrNone);
	store = NULL;
	doc = NULL;
	}

/**
   @SYMTestCaseID T-ProStep-testDocCreation3L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaProcess::OpenNewDocumentL() and CApaAppFileReader APIs.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create a document from a specified file. Set it as main
   document file. Create a CApaAppInfoFileReader object by calling
   OpenAppInfoFileL(). Use CaptionL() and CreateMaskedBitmapL() to read
   application information like caption and bitmap.\n
   API Calls:\n	
   CApaProcess::OpenNewDocumentL(CFileStore*& aStore,CStreamDictionary*& aStreamDic,const TDesC& aDocFullFileName,TUint aFileMode);\n
   CApaApplication::OpenAppInfoFileL() const\n
   CApaAppInfoFileReader::CaptionL(TLanguage aLanguage)\n
   CApaAppInfoFileReader::CreateMaskedBitmapL(TInt aIconSideInPixels)\n
   
   @SYMTestExpectedResults Test checks results against expected values.
    
 */
CApaDocument* CT_ProStep::testDocCreation3L()
	{
	// try re-opening it
	const TFullName filePath=_L("c:\\docs\\tstapp.doc");
	CStreamDictionary* streamDic=NULL;
	CApaDocument* doc=NULL;
	CFileStore* store=NULL;
	TRAPD(ret,doc=iProcess->OpenNewDocumentL(store,streamDic,filePath,EFileShareExclusive|EFileWrite));
	TEST(ret==KErrNone);
	delete streamDic;
	streamDic = NULL;
	iProcess->SetMainDocument(doc);
	iProcess->SetMainDocFileName(filePath);
	((CTestAppDoc*)iProcess->MainDocument())->iStore = store;

	// remove the main document
	iProcess->DestroyDocument(iProcess->MainDocument());
	TEST(iProcess->MainDocument()==NULL);

	// try re-opening the doc without the app in memory
	TRAP(ret,iProcess->ResetL());
	TEST(ret==KErrNone);
	delete iProcess;
	return doc;
	}


/**
   @SYMTestCaseID T-ProStep-testDocCreation4L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaProcess::OpenNewDocumentL(). 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create a Process. Create a document for an app DLL from a 
   specified file by calling OpenNewDocumentL(). Call DestroyDocument() to
   delete the document.\n
   API Calls:\n	
   CApaProcess::OpenNewDocumentL(CFileStore*& aStore,CStreamDictionary*& aStreamDic,const TDesC& aDocFullFileName,TUint aFileMode);\n
   CApaProcess::DestroyDocument(CApaDocument* aDoc)\n
   
   @SYMTestExpectedResults Test checks results against expected values.
    
 */
void CT_ProStep::testDocCreation4L(CApaDocument* aDoc)
	{
	const TFullName filePath=_L("c:\\docs\\tstapp.doc");
	CStreamDictionary* streamDic=NULL;
	CFileStore* store=NULL;
	TRAPD(ret,iProcess = CApaProcess::NewL(iFs));
	TEST(ret==KErrNone);
	TRAP(ret,aDoc=iProcess->OpenNewDocumentL(store,streamDic,filePath,EFileShareExclusive|EFileWrite));
	TEST(ret==KErrNone);
	iProcess->DestroyDocument(aDoc);
	delete store;
	delete streamDic;

	// tidy up
	TRAP(ret,iProcess->ResetL());
	TEST(ret==KErrNone);
	delete iProcess;

	// delete the file created by the testcode
	iFs.Delete(filePath);
	}


/**
  Auxiliary Fn for Test Case ID T-ProStep-testDocCreation1L, T-ProStep-testDocCreation2L,
  T-ProStep-testDocCreation3L, T-ProStep-testDocCreation4L.
 
  The method initiates tests on operation such as creation, storing
  and deletion of a document.
  
*/
void CT_ProStep::testDocCreationL()
	{
	//Create a session with F & B server
	TInt ret = RFbsSession::Connect();
	TEST(ret == KErrNone);

	testDocCreation1L();
	testDocCreation2L();
	testDocCreation4L(testDocCreation3L());

	//Close the session F & B server.
	RFbsSession::Disconnect();
	}

/**
   @SYMTestCaseID T-ProStep-testEmbeddingL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests embedding of a document and accessing the embedded document.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The test connects to the Font and Bitmap Server and creates a
   process by calling CApaProcess::NewL().
   The method first tests the embedding of document. To do so a document of
   type tstapp is added to the process and set as the main document. The main
   document is opened for editing by calling CApaDocument::EditL() and another
   document of type tstapp is created to embed it within the main document.
   Tests for the number of embedded documents and editing of the embedded
   document is done. The main document is closed and then reloaded to verify
   that the embedded document is saved.\n
   A test to access the embedded document is then done. For this purpose the
   main document is detached from its store by calling CApaDocument::DetachFromStoreL()
   and an attempt to access the embedded document is done by calling CApaDocument::DocumentL().
   The same test to access the embedded document is again done by reloading
   the main document without detaching it from its store.\n
   Finally embedded document is tested to change the format of the door to
   glass by calling CApaDoor::SetFormatToGlassL(). The test ensures that the
   embedded document does not support being represented by a glass door by
   checking for leave KErrNotSupported.\n Then the same embedded document is supported when being 
   called by CApaDoor::SetFormatToIconL() and returns KErrNone.
   API Calls:\n	
   CApaProcess::NewL(const RFs& aFs,CApaAppFinder& aAppFinder)\n
   CApaProcess::AddNewDocumentL(const TDesC& aDllFileName,TUid aDllUid=KNullUid)\n
   CApaDocument::EditL(MApaEmbeddedDocObserver* aContainer,TBool aReadOnly=EFalse) = 0\n
   CApaDocument::DetachFromStoreL(CPicture::TDetach aDegree = CPicture::EDetachFull)\n
   CApaDoor::DocumentL(TBool aCheckPassword=EFalse)\n
   CApaDoor::SetFormatToGlassL()\n
   CApaDoor::SetFormatToIconL()\n
   
   @SYMTestExpectedResults All tests completes successfully by checking the
   results obtained against expected results.
    
 */
void CT_ProStep::testEmbeddingL()
	{
	const TFullName filePath=_L("c:\\docs\\tstapp.doc");

	//Create a session with F & B server
	TInt ret=RFbsSession::Connect();
	TEST(ret == KErrNone);
	
	// delete the file to be created by the testcode
	iFs.Delete(filePath);
	// create a process
	TRAP(ret,iProcess = CApaProcess::NewL(iFs));	
	TEST(ret==KErrNone);

	// create a new main doc of type tstapp
	CApaDocument* doc=NULL;	
	TApaApplicationFactory appFact(KUidTestApp);
	TRAP(ret,doc=iProcess->AddNewDocumentL(appFact));
	TEST(ret==KErrNone);
	iProcess->SetMainDocument(doc);
	CFileStore* store=NULL;

	// create the store and initialise it
	TRAP(ret, {
		store=doc->CreateFileStoreLC(iProcess->FsSession(),filePath);
		CleanupStack::Pop(); // store 
		});
	TEST(ret==KErrNone);
	((CTestAppDoc*)iProcess->MainDocument())->iStore = store;
	iProcess->SetMainDocFileName(filePath);
	
	// initialise the document with factory settings
	TRAP(ret, doc->NewDocumentL() );
	TEST(ret==KErrNone);

	// edit it
	TRAP(ret,iProcess->MainDocument()->EditL(NULL));
	TEST(ret==KErrNone);

	// embed another tstapp doc inside it
	INFO_PRINTF1(_L("...Embed a doc"));
	TRAP(ret,((CTestAppDoc*)iProcess->MainDocument())->EmbedNewDocL(appFact));
	TEST(ret==KErrNone);
	TEST(((CTestAppDoc*)iProcess->MainDocument())->iEmbedList->Count()==1);

	// edit that doc
	TRAP(ret,((CTestAppDoc*)iProcess->MainDocument())->EditEmbeddedDocL(0));
	TEST(ret==KErrNone);

	// close the main doc and reload it
	TRAP(ret,iProcess->MainDocument()->SaveL());
	TEST(ret==KErrNone);
	TRAP(ret, iProcess->ResetL() );
	TEST(ret==KErrNone);
	CStreamDictionary* streamDic=NULL;
	TRAP(ret,doc=iProcess->OpenNewDocumentL(store,streamDic,filePath,EFileShareExclusive|EFileWrite));
	TEST(ret==KErrNone);
	delete streamDic;
	streamDic = NULL;
	iProcess->SetMainDocument(doc);
	iProcess->SetMainDocFileName(filePath);
	((CTestAppDoc*)iProcess->MainDocument())->iStore = store;
	TEST(((CTestAppDoc*)iProcess->MainDocument())->iEmbedList->Count()==1);

	// close the main doc and reload it again...
	TRAP(ret,iProcess->MainDocument()->SaveL());
	TEST(ret==KErrNone);
	iProcess->ResetL();
	TRAP(ret,doc=iProcess->OpenNewDocumentL(store,streamDic,filePath,EFileShareExclusive|EFileWrite));
	TEST(ret==KErrNone);
	delete streamDic;
	streamDic = NULL;
	iProcess->SetMainDocument(doc);
	iProcess->SetMainDocFileName(filePath);
	((CTestAppDoc*)iProcess->MainDocument())->iStore = store;
	TEST(((CTestAppDoc*)iProcess->MainDocument())->iEmbedList->Count()==1);

	// detach the main doc from it's store & access the embedded doc
	TRAP(ret, iProcess->MainDocument()->DetachFromStoreL(CPicture::EDetachFull) );
	TEST(ret==KErrNone);
	TRAP(ret, ((CTestAppDoc*)iProcess->MainDocument())->EmbeddedDoor(0)->DocumentL() );
	TEST(ret==KErrNone);

	// save it, then save it again immediately
	TRAP(ret,iProcess->MainDocument()->SaveL());
	TEST(ret==KErrNone);
	TRAP(ret,iProcess->MainDocument()->SaveL());
	TEST(ret==KErrNone);

	// load the doc and get a handle to the embedded doc
	iProcess->ResetL();
	TRAP(ret,doc=iProcess->OpenNewDocumentL(store,streamDic,filePath,EFileShareExclusive|EFileWrite));
	TEST(ret==KErrNone);
	delete streamDic;
	streamDic = NULL;
	iProcess->SetMainDocument(doc);
	iProcess->SetMainDocFileName(filePath);
	((CTestAppDoc*)iProcess->MainDocument())->iStore = store;
	TEST(((CTestAppDoc*)iProcess->MainDocument())->iEmbedList->Count()==1);
	TRAP(ret, ((CTestAppDoc*)iProcess->MainDocument())->EmbeddedDoor(0)->DocumentL() );
	TEST(ret==KErrNone);

	// try to set the embedded doc's format to glass
	TRAP(ret, ((CTestAppDoc*)iProcess->MainDocument())->EmbeddedDoor(0)->SetFormatToGlassL() );
	TEST((ret==KErrNotSupported));
	
	// try to set the embedded docs to temporary Icon 	
	TRAP(ret, ((CTestAppDoc*)iProcess->MainDocument())->EmbeddedDoor(0)->SetFormatToTemporaryIconL(ETrue) );
	TEST((ret==KErrNone));
	
	// try to set embedded doc's format to Icon
	TRAP(ret, ((CTestAppDoc*)iProcess->MainDocument())->EmbeddedDoor(0)->SetFormatToIconL() );
	TEST((ret==KErrNone));

	// tidy up and delete the file created by the testcode
	delete iProcess;
	iFs.Delete(filePath);

	//Close the session F & B server.
	RFbsSession::Disconnect();
	}

_LIT(KMaxLengthDocFileName, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"); // length must be equal to KMaxFileName

void CT_ProStep::MainDocFileNameTestsL()
	{
	testMainDocFileName1L();
 	testMainDocFileName2L();
 	testMainDocFileName3L();
	}
 /**
   @SYMTestCaseID CT-ProStep-testMainDocFileName1L
   
   @SYMPREQ PHAR-5NTCWY
   
   @SYMTestCaseDesc
   Checks that the CApaProcess's MainDocFileName is large enough by default.
   Clients of CApaProcess::SetMainDocFileName API expect to be able to
   pass it a descriptor with a length between zero and KMaxFileName inclusive.
   Check that passing a descriptor of length KMaxFileName doesn't give an
   APPARC 7 panic (EPanicFileNameTooLong)
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions
   Construct a CApaProcess object, and call SetMainDocFileName on it, passing
   a file name KMaxFileName characters long
   
   @SYMTestExpectedResults
   The call to CApaProcess::SetMainDocFileName should not cause a panic
   
 */
void CT_ProStep::testMainDocFileName1L()
  	{
 	CApaProcess* process = CApaProcess::NewL(iFs);
 	process->SetMainDocFileName(KMaxLengthDocFileName); // should not panic
 	delete process;
  	}
 
 /**
   @SYMTestCaseID CT-ProStep-testMainDocFileName2L
   
   @SYMPREQ PHAR-5NTCWY
   
   @SYMTestCaseDesc
   Checks that the maximum length of CApaProcess's MainDocFileName is still large
   enough to store a file name KMaxFileName characters long after setting a file name using
   CApaProcess::SetMainDocFileNameL which is less than KMaxFileName characters long
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions
   1. Construct a CApaProcess object, and call SetMainDocFileNameL on it, passing
   a file name which is less than KMaxFileName characters long
   2. Call SetMainDocFileName passing a file name which is KMaxFileName characters long
   
   @SYMTestExpectedResults
   The call to CApaProcess::SetMainDocFileName should not cause a panic
   
 */
void CT_ProStep::testMainDocFileName2L()
 	{
 	_LIT(KShortDocFileName, "ShortDocumentFileName"); // must be < KMaxFileName in length
 	CApaProcess* process = CApaProcess::NewL(iFs); 
 	CleanupStack::PushL(process);
 	process->SetMainDocFileNameL(KShortDocFileName);
 	process->SetMainDocFileName(KMaxLengthDocFileName); // should not panic
 	CleanupStack::PopAndDestroy(process);
 	}

 /**
   @SYMTestCaseID CT-ProStep-testMainDocFileName3L
  
   @SYMPREQ PHAR-5NTCWY
  
   @SYMTestCaseDesc
   Checks that CApaProcess's MainDocFileName can store a filename longer than KMaxFileName
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   1. Construct a CApaProcess object, and call SetMainDocFileNameL on it, passing
   a file name which is greater than KMaxFileName characters long
   2. Check the length of the file name returned by MainDocFileName
  
   @SYMTestExpectedResults
   The file name returned by MainDocFileName should be the same length as the file name
   passed to SetMainDocFileNameL
  
 */
void CT_ProStep::testMainDocFileName3L()
	{
	_LIT(KLongDocFileName, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdefLong"); // must be > KMaxFileName in length
	TPtrC longDocFileName(KLongDocFileName);
	CApaProcess* process = CApaProcess::NewL(iFs); 
	CleanupStack::PushL(process);
	process->SetMainDocFileNameL(longDocFileName);
	TEST(process->MainDocFileName().Length() == longDocFileName.Length());
	CleanupStack::PopAndDestroy(process);
	}

/**
   @SYMTestCaseID T-ProStep-testIniFilesL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests creation of ini file and reading or writing of data to an ini file. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create a Process. Add a new document for an app DLL by calling
   AddNewDocumentL().Create a store and initialize the document. Create
   an .ini file by calling OpenIniFileLC().Write data to ini file using
   RDictionaryWriteStream. Save changes by calling CDictionaryStore::CommitL()
   and close the ini file. Re-open the ini file and read data from it using
   RDictionaryReadStream.\n
   API Calls:\n	
   CApaApplication::OpenIniFileLC(RFs& aFs) \n
   RDictionaryWriteStream::AssignL(CDictionaryStore& aDictStore,TUid aUid)\n
   RDictionaryWriteStream::WriteInt8L(TInt aValue)\n
   RDictionaryWriteStream::CommitL()\n
   CDictionaryStore::CommitL()\n
   RDictionaryReadStream::OpenL(const CDictionaryStore& aDictStore,TUid aUid)\n
   RDictionaryReadStream::ReadInt8L()\n
   
   @SYMTestExpectedResults Test checks whether stored and read values from the
   ini file match. 
 */
void CT_ProStep::testIniFilesL()
	{
	TFullName filePath=_L("c:\\docs\\tstapp.doc");
	const TFullName iniPath=_L("c:\\system\\data\\tstapp.ini");		// defined in tstapp
	const TUid testUid={1};

	// delete the files to be created by the testcode
	iFs.Delete(filePath);
	iFs.Delete(iniPath);
	// create a process
	TRAPD(ret,iProcess = CApaProcess::NewL(iFs));
	TEST(ret==KErrNone);

	// create a new main doc of type tstapp
	CApaDocument* doc=NULL;
	TApaApplicationFactory appFact(KUidTestApp);
	TRAP(ret,doc=iProcess->AddNewDocumentL(appFact));
	TEST(ret==KErrNone);
	iProcess->SetMainDocument(doc);
	CFileStore* store=NULL;

	// create the store and initialise it
	TRAP(ret, {
		store=doc->CreateFileStoreLC(iProcess->FsSession(),filePath);
		CleanupStack::Pop(); // store 
		});
	TEST(ret==KErrNone);
	((CTestAppDoc*)iProcess->MainDocument())->iStore = store;
	iProcess->SetMainDocFileName(filePath);

	// initialise the document with factory settings
	TRAP(ret, doc->NewDocumentL() );
	TEST(ret==KErrNone);

	// create a new ini file for the app
	CDictionaryStore* iniFile=NULL;
	TRAP(ret, {
		iniFile=iProcess->MainDocument()->Application()->OpenIniFileLC(iFs);
		CleanupStack::Pop();
		});
		TEST(ret==KErrNone);

	// add some data to it
	RDictionaryWriteStream writeStream;
	TRAP(ret, writeStream.AssignL(*iniFile,testUid) );
	TEST(ret==KErrNone);
	TInt data=17;
	TRAP(ret, writeStream.WriteInt8L(data) );
	TEST(ret==KErrNone);
	TRAP(ret, writeStream.CommitL() );
	TEST(ret==KErrNone);
	writeStream.Close();

	// close it
	TRAP(ret, iniFile->CommitL() );
	TEST(ret==KErrNone);
	delete iniFile;
	iniFile = NULL;

	// re-open it
	TRAP(ret, {
		iniFile=iProcess->MainDocument()->Application()->OpenIniFileLC(iFs);
		CleanupStack::Pop();
		});
		TEST(ret==KErrNone);

	// check the contents
	RDictionaryReadStream readStream;
	TRAP(ret, readStream.OpenL(*iniFile,testUid) );
	TEST(ret==KErrNone);
	TInt check=0;
	TRAP(ret, check=readStream.ReadInt8L() );
	TEST(ret==KErrNone);
	TEST(check==data);
	readStream.Close();

	// close it
	delete iniFile;

	// tidy up
	delete iProcess;
	iFs.Delete(filePath);
	iFs.Delete(iniPath);
	}

/**
  Auxiliary Fn for Test Case ID T-ProStep-testControls1L,
  T-ProStep-testControls2L, T-ProStep-testControls3L
 
  The method finds the index of control m_ctrl.exe in the control list.
  Depending on the Boolean value of argument aIsNewPath the search is
  done on ROM / RAM.\n
 
*/

TInt CT_ProStep::ControlIndexInList(CApaSystemControlList* aList, TBool aIsNewPath)
	{
	TFileName newCtlPath=_L("c:\\sys\\bin\\m_ctrl.exe");

	const TInt count = aList->Count();
	TInt retVal = KErrNotFound;
	for(TInt ii = 0; ii < count; ++ii)
		{
		if(((aList->Control(ii)->FileName().CompareF(ctlPath)==0) && (!aIsNewPath)) ||
			((aList->Control(ii)->FileName().CompareF(newCtlPath)==0) && (aIsNewPath)))
			{
			retVal = ii;
			break;
			}
		}
	return retVal;
	}


/**
   @SYMTestCaseID T-ProStep-testControls1L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaSystemControlList::UpdateL().
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Check the control list to find the number of controls in the
   list and the number of updates to the list. Call UpdateL(). Check the list
   again for no change in the count of controls and updates to the list.\n
   API Calls:\n	
   CApaSystemControlList::UpdateL()\n
   
   @SYMTestExpectedResults Test confirms that there is no change in the control
   list after updating.
 */
void CT_ProStep::testControls1L()
	{
	iControlCount1 = iControlList->Count();
	TEST((iControlCount1>=1));
	TEST((iControlList->UpdateCount()>=1));

	// do an update - there should be no changes
	TInt ret;
	TRAP(ret, iControlList->UpdateL());
	TEST(ret==KErrNone);
	iControlCount2 = iControlList->Count();
	TEST(iControlCount2 == iControlCount1);	// no change in the count (finds a m_ctrl.exe file on z, not c)
	TEST((iControlList->UpdateCount()>=1));
	TInt index = ControlIndexInList(iControlList,EFalse);
	TEST((index >= 0) && (index < iControlCount2));
	}


/**
   @SYMTestCaseID T-ProStep-testControls2L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaSystemControlList::UpdateL() and UpdateCount().
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
  
   @SYMTestActions Copy control from Z: drive to C: drive. Call UpdateL() to
   update the control list. Check if UpdateL() has incremented iUpdateCount
   by calling UpdateCount(). Check the filename of control to ensure that the
   control copied to C: drive has been updated in the list. Delete the newly
   copied control.\n
   API Calls:\n	
   CApaSystemControlList::UpdateL()\n
   CApaSystemControlList::UpdateCount()const\n
   
   @SYMTestExpectedResults The test shows updating of the copied control in control list.
 */
void CT_ProStep::testControls2L()
	{
	RSmlTestUtils testSession;
	User::LeaveIfError(testSession.Connect());

	testSession.CreateDirectoryL(KCTLDIR);
	testSession.CreateDirectoryL(KRSCDIR);

	TInt ret=testSession.CopyFileL(KSOURCEPATH,KNEWCTLPATH);
	TEST(ret==KErrNone);
	ret=testSession.CopyFileL(KLOCPATH,KNEWLOCPATH);
	TEST(ret==KErrNone);

	TInt controlCount=iControlList->UpdateCount();
	while(iControlList->UpdateCount()<=controlCount)
		{
		TRAP(ret, iControlList->UpdateL());
		if(ret!=KErrNone)
			{
			break;				
			}
		User::After(100000);	
		}
	TEST(ret==KErrNone);

	iControlCount3 = iControlList->Count();
	TEST(iControlCount3 == iControlCount1);	// still no changes in the count (finds a m_ctrl.exe file on c, not z)
	TEST((iControlList->UpdateCount()==controlCount+1));

	TInt index = ControlIndexInList(iControlList,ETrue);
	TEST((index>=0)&&(index<iControlCount3));
	if(index>=0)
		{
		TFileName name=iControlList->Control(index)->FileName();
		TEST(name.CompareF(KCTRLNAME)==0);
		}
	
	// hide the control and do an update - there should be changes
	testSession.SetReadOnly(KNEWCTLPATH,0);  // remove the read only attribute
	ret=testSession.DeleteFileL(KNEWCTLPATH);
	TEST(ret==KErrNone);
	testSession.SetReadOnly(KNEWLOCPATH,0);  // remove the read only attribute
	ret=testSession.DeleteFileL(KNEWLOCPATH);
	TEST(ret==KErrNone);
	controlCount=iControlList->UpdateCount();
	while(iControlList->UpdateCount()<=controlCount)
		{
		TRAPD(ret, iControlList->UpdateL());
		if(ret!=KErrNone)
			{
			break;				
			}
		User::After(100000);
		}

	testSession.Close();
	}


/**
   @SYMTestCaseID T-ProStep-testControls3L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaSystemControlList::Control() and CApaSystemControl APIs Type() and Caption().
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Update the control list to ensure that the deleted control is
   removed from the list and the control on the Z: Drive is added to the list.
   Retrieve this control from the list using CApaSystemControlList::Control(). 
   Check the type and caption of the control calling Type() and Caption() methods.\n
   API Calls:\n	
   CApaSystemControlList::Control(TInt aIndex)const\n
   CApaSystemControl::Type()const\n
   CApaSystemControl::Caption()const\n
   
   @SYMTestExpectedResults The test shows the addition of the control present on 
   Z: drive in control list. Test confirms the retrieved info on the control
   obtained from the control list.
  
 */
void CT_ProStep::testControls3L()
   {
	TUid KUidMinimalControl={0x13008AEE};
	TFileName ctlCaption=_L("m_ctrl");
	// This method is called to close the panic dialog generated by previous test
	ClosePanicWindowL();

	TRAPD(ret, iControlList->UpdateL());
	TEST(ret==KErrNone);
	const TInt controlCount4 = iControlList->Count();
	TEST(controlCount4 == iControlCount1);	// still no changes in the count (finds a .ctl file on z, not c)
	TEST((iControlList->UpdateCount()==3));
	TInt index = ControlIndexInList(iControlList,EFalse);
	TEST((index >= 0) && (index < controlCount4));
	if(index>=0)
		{
		TEST(iControlList->Control(index)->FileName().CompareF(ctlPath)==0);
	
		INFO_PRINTF1(_L("Testing CApaSystemControl"));

		//get the control from the list and check it's type and caption
		iControl=iControlList->Control(index);
		TEST(iControl!=NULL);
		TEST((iControl->Type()==KUidMinimalControl));
		TFileName caption=iControl->Caption();
		TEST(caption.CompareF(ctlCaption)==0);
		TEST(iControl->Icon()!=NULL);
		RThread thread;
		TFullName name=_L("Control Panel Test");
		TInt r=thread.Create(name,SimulateKeyL,KDefaultStackSize,KDefaultStackSize,KDefaultStackSize,NULL);
		TEST(r==KErrNone);
		thread.Resume();

		// run the control
		TRAP(ret, iControl->CreateL()); // this control is synchronous
		TEST(ret==KErrNone);
		thread.Close();
		}
	}

/**
   Auxiliary Fn for T-ProStep-testControls1L, T-ProStep-testControls2L, T-ProStep-testControls3L
   
   This method creates a control list by calling CApaSystemControlList::NewL() and 
   initiates subsequent tests on CApaSystemControlList and CApaSystemControl APIs.
    
 */
void CT_ProStep::testControlsL()
	{
	INFO_PRINTF1(_L("Testing CApaSystemControlList"));

	//Create a session with F & B server
	TInt ret = RFbsSession::Connect();
	TEST(ret == KErrNone);

	CFileMan* fileMan=CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);

	TRAP(ret,iControlList=CApaSystemControlList::NewL(iFs));
	TEST(ret==KErrNone);

	testControls1L();
	testControls2L();
	testControls3L();

	// tidy up
	delete iControlList;
	CleanupStack::PopAndDestroy(fileMan);

	//Close the session F & B server.
	RFbsSession::Disconnect();

     }

 TInt PanicTestThread(TAny* aOption)
	{
	CTrapCleanup::New();
	const TInt option = TInt(aOption);
	//Create a session with F & B server
	TInt ret = RFbsSession::Connect();
	switch(option)
		{
	case ENegativePanicTest:
	 	
		if(ret==KErrNone)
			{
			RFs fs;
			fs.Connect();
			CApaSystemControlList *controlList=NULL;
			TRAP(ret,controlList=CApaSystemControlList::NewL(fs));
			if(ret==KErrNone)
				{
				TInt count=controlList->Count();
				controlList->Control(count+10);
				}
			delete controlList;
			fs.Close();
			}
     break;
	 default:
		User::Panic(_L("TProStep Error"), EUnknownPanicOption);
	 	}
	RFbsSession::Disconnect(); 	
	return KErrNone;
	}
	
/**
   @SYMTestCaseID APPFWK-APPARC-0056
  
   @SYMDEF DEF092293
  
   @SYMTestCaseDesc Tests CApaSystemControlList::Control() Throws the right Panic on Invalid Count.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Create  Seperate Thread to call the PanicTestThread Method where we create a new system ControlList
   and get the actual count of the lists present inside . now call the CApaSystemControlList::Control(Index);where Index
   is a number greater than the actual count list.
   CApaSystemControlList::Control(TInt aIndex)const\n
  
   
   @SYMTestExpectedResults Expect a APGRFX Panic EPanicIndexOutOfRange.
  
 */	
	
void CT_ProStep::testPanicForControlListL()
	{
	RThread thrd;
	TRequestStatus stat;
	TInt ret=thrd.Create(_L("ptt"),PanicTestThread,KDefaultStackSize,0x2000,0x20000,(TAny*)ENegativePanicTest);
	User::LeaveIfError(ret);
	thrd.SetPriority(EPriorityMuchMore);
	thrd.Logon(stat);
	User::SetJustInTime(EFalse);
	thrd.Resume();
	User::WaitForRequest(stat);
	TEST(thrd.ExitType()==EExitPanic); 
	TEST(thrd.ExitReason()==EPanicIndexOutOfRange);
	INFO_PRINTF2(_L("TestPanicForControlList panic=%d"), thrd.ExitReason());
	thrd.Close();
	User::SetJustInTime(ETrue);
	ClosePanicWindowL();
	
	}
	
// This method is called to close the panic generated by the testPanicForControlListL() test.	
void CT_ProStep::ClosePanicWindowL()
	{
	RWsSession	ws;
	User::LeaveIfError(ws.Connect());

	TInt wgFocus = ws.GetFocusWindowGroup();

	const TUint32 ENullWsHandle = 0xFFFFFFFF;	// Events delivered to this handle are thrown away
	RWindowGroup wg = RWindowGroup(ws);

	wg.Construct(ENullWsHandle);
	TInt wgId = wg.Identifier();

	TWsEvent event;
	event.SetType(EEventKey);
	TKeyEvent *keyEvent = event.Key();
	keyEvent->iCode = EKeyEscape;
	keyEvent->iScanCode = EStdKeyEscape;
	keyEvent->iModifiers = 0;

	TInt limit = 0;
	for(limit = 0; wgFocus != wgId && (limit < 50); limit++)
	{
		ws.SendEventToAllWindowGroups(event);
		wgFocus = ws.GetFocusWindowGroup();
		RDebug::Print(_L("CloseAllPanicWindowsL() - EKeyEscape sent to Windows Group"));
	}

	// close everything
	wg.Close();
	ws.Close();
	
	}
/**
   @SYMTestCaseID T_ProStep_DoAppListInvalidTestL
  
   @SYMPREQ
 
   @SYMTestCaseDesc Tests GetNextApp() never returns RApaLsSession::EAppListInvalid.
  
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Copy tstapp files from z: drive to c: drive. Create a session
   with the Application Architecture server. Populate the list of applications
   to cache by calling RApaLsSession::GetAllApps().Remove the copied files
   from C: drive. Traverse through the list by calling
   RApaLsSession::GetNextApp(). Check return value to ensure that
   RApaLsSession::EAppListInvalid is never returned.\n
   API Calls:\n	
   RApaLsSession::GetNextApp(TApaAppInfo& aInfo) const
   
   @SYMTestExpectedResults Test confirms that RApaLsSession::EAppListInvalid is never returned.
    
 */
void CT_ProStep::DoAppListInvalidTestL(RApaLsSession& aLs)
	{
	_LIT(KTempAppDir, "C:\\private\\10003a3f\\import\\apps\\");
	_LIT(KTempRegPath, "C:\\private\\10003a3f\\import\\apps\\tstapp_reg.rsc");
	TFullName regPath=_L("z:\\private\\10003a3f\\import\\apps\\tstapp_reg.rsc");
	
	CFileMan* fileMan = CFileMan::NewL (iFs);
	CleanupStack::PushL(fileMan);
	
	INFO_PRINTF1(_L("Copy tstapp files to C: drive......."));
	TInt ret = iFs.MkDir(KTempAppDir);
	TEST(ret==KErrNone || ret==KErrAlreadyExists);
	TEST(fileMan->Copy(regPath, KTempRegPath)==KErrNone);	//Just to start the idle update.

	User::After(8000000);
	
	INFO_PRINTF1(_L("Get app list......."));
	ret = aLs.GetAllApps();
	TEST(ret==KErrNone);

	INFO_PRINTF1(_L("Remove temp files from C: drive......."));
	TRequestStatus status;
	TTime tempTime(0); // added tempTime to avoid asynch CFileMan::Attribs request completing with KErrArgument
	TInt err=fileMan->Attribs(KTempAppDir,0,KEntryAttReadOnly, tempTime, CFileMan::ERecurse, status);
	TEST(err==KErrNone);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	TEST(fileMan->Delete(KTempRegPath)==KErrNone);	//Just to start the idle update.
	TEST(fileMan->RmDir(KTempAppDir)==KErrNone);

	User::After(8000000);
	
	INFO_PRINTF1(_L("Testing GetNextApp() never returns RApaLsSession::EAppListInvalid."));
	TApaAppInfo info;
	while(ret==KErrNone)
		{
		ret=aLs.GetNextApp(info);
		}
	TEST(ret!=RApaLsSession::EAppListInvalid);
	TEST(ret==RApaLsSession::ENoMoreAppsInList);

	CleanupStack::PopAndDestroy(fileMan);
	}


/**
  Auxiliary Fn for all Test Cases.
 
  This method creates and installs an active scheduler and puts the
  test code on the scheduler as a CIdle object. The method initiates
  all tests by calling the static method CT_ProStepCallBack::CallBack().
 
*/
void CT_ProStep::DoStepTestsInCallbackL()
	{
	// create an active scheduler
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	// put the test code onto the scheduler as an idle object
	CIdle* idle=CIdle::NewL(-20);
	CleanupStack::PushL(idle);

	CT_ProStepCallBack* callBack = new(ELeave) CT_ProStepCallBack(this);
	CleanupStack::PushL(callBack);

	idle->Start(TCallBack(CT_ProStepCallBack::CallBack,callBack));
	// start the test code
	CActiveScheduler::Start();

	// all outstanding requests complete - kill the scheduler
	CleanupStack::PopAndDestroy(3); //scheduler, callBack, idle
	}

/**
  This static method is the callback function of CIdle object. The method
  calls the non-static method DoStepTests() which initiates all the tests. 
*/
TInt CT_ProStepCallBack::CallBack(TAny* callBack /*aThis*/)
{
	//Call Test Step func
	((CT_ProStepCallBack *)callBack)->iTestStep->DoStepTests();
	
	CActiveScheduler::Stop();
	return EFalse; // don't call back again
	}


/**
   Constructor
 */
CT_ProStepCallBack::CT_ProStepCallBack(CT_ProStep* aTestStep)
	{
	iTestStep = aTestStep;
	}

/**
   Destructor
 */
CT_ProStepCallBack::~CT_ProStepCallBack()
	{
	}

/**
  Auxiliary Fn for all Test Cases. 
  The method initiates all tests to be performed.
*/
void CT_ProStep::DoStepTests()
	{
	INFO_PRINTF1(_L("Test AppListInvalidTest......"));
	RTestableApaLsSession ls;
	TEST(KErrNone == ls.Connect());
	CleanupClosePushL(ls);
	//DONT_CHECK due to changes to file system
	HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, DoAppListInvalidTestL(ls), REComSession::FinalClose() );
	CleanupStack::PopAndDestroy(&ls);
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Test AppIdentifier......"));
	TRAPD(r,testAppIdentifierL());
	TEST(r==KErrNone);
	INFO_PRINTF2(_L("Test AppIdentifier completed with code %d\n"), r);
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Test TestGenerateFileName......"));
	TRAP(r,doTestGenerateFileName());
	TEST(r==KErrNone);
	INFO_PRINTF2(_L("Test TestGenerateFileName completed with code %d\n"), r);
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	INFO_PRINTF1(_L("Test TestReadStreamL......"));
	TRAP(r,testReadStreamL());
	TEST(r==KErrNone);
	INFO_PRINTF2(_L("Test TestReadStream completed with code %d\n"), r);
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	INFO_PRINTF1(_L("Test DocCreation......"));
	TRAP(r,testDocCreationL());
	TEST(r==KErrNone);
	INFO_PRINTF2(_L("Test DocCreation completed with code %d\n"), r);
	REComSession::FinalClose();
	__UHEAP_MARKEND;	
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Test Embedding......"));
	TRAP(r,testEmbeddingL());
	TEST(r==KErrNone);
	INFO_PRINTF2(_L("Test Embedding completed with code %d\n"), r);
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	INFO_PRINTF1(_L("Test MainDocFileName......"));
	TRAP(r,MainDocFileNameTestsL());
	TEST(r==KErrNone);
	INFO_PRINTF2(_L("Test MainDocFileName completed with code %d\n"), r);
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	INFO_PRINTF1(_L("Test IniFiles......"));
	TRAP(r,testIniFilesL());
	TEST(r==KErrNone);
	INFO_PRINTF2(_L("Test IniFiles completed with code %d\n"), r);
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	INFO_PRINTF1(_L("Test Controls......"));
	TRAP(r,testControlsL());
	TEST(r==KErrNone);
	INFO_PRINTF2(_L("Test Controls completed with code %d\n"), r);
	REComSession::FinalClose();
	__UHEAP_MARKEND;

        __UHEAP_MARK;
	INFO_PRINTF1(_L("Test Panic Controls......"));
	TRAP(r,testPanicForControlListL());
	TEST(r==KErrNone);
	INFO_PRINTF2(_L("Test Panic Controls completed with code %d\n"), r);
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	} 

/**
   Destructor
 */
CT_ProStep::~CT_ProStep()
	{
	}

/**
   Constructor
 */
CT_ProStep::CT_ProStep()
	{
	SetTestStepName(KT_ProStep);
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CT_ProStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CT_ProStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

/**
   @return - TVerdict code
   Override of base class virtual
*/
TVerdict CT_ProStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparch...T_Pro"));

	FbsStartup();

	iFs.Connect();
	setup();

	TRAPD(ret,DoStepTestsInCallbackL())
	TEST(ret==KErrNone);

	iFs.Close();

	INFO_PRINTF1(_L("T_Pro Completed."));
	return TestStepResult();
	}
