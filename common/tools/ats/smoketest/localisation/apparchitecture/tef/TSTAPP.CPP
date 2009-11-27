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
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <e32uid.h>
#include <s32std.h>
#include <s32stor.h>
#include <s32file.h>
#include <apgicnfl.h>
#include <apfdef.h>
#include "tstapp.h"
#include <apfdef.h>
#include <apgcli.h>

#include <eikstart.h>
#include <ecom.h>
#include <implementationproxy.h>

_LIT(KTestAppCaption, "Test App");
_LIT(KTempFilePath, "c:\\system\\temp\\");
_LIT(KTestAppIniFilePath, "c:\\system\\data\\tstapp.ini");


GLDEF_C TInt E32Dll(
					)
	{
	return KErrNone;
	}

LOCAL_C CApaApplication* NewApplicationL()
	{
	CApaApplication* thing=NULL;
	thing=CTestApp::NewL();
	return thing;
	//return new CExampleApplication;
	}

LOCAL_D const TImplementationProxy ImplementationTable[]=
	{
	IMPLEMENTATION_PROXY_ENTRY(KTestAppUidValue, NewApplicationL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount=sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
	return ImplementationTable;
	}


/////////////////////////////////////////
// CBasicAppUi
/////////////////////////////////////////

void CBasicAppUi::ConstructL()
	{
	BaseConstructL(ENoAppResourceFile|ENoScreenFurniture);
	}
	
/////////////////////////////////////////
// CTestApp
/////////////////////////////////////////

CTestApp* CTestApp::NewL()
// The gated function
//
	{
	CTestApp* self=new(ELeave) CTestApp();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CTestApp::CTestApp()
	:iCaption(KTestAppCaption)
	{}


CTestApp::~CTestApp()
	{
	}

void CTestApp::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}

	
void CTestApp::PreDocConstructL()
	{
	}


CEikDocument* CTestApp::CreateDocumentL()
	{	
	return CTestAppDoc::NewL(*this);
	}

TDesC& CTestApp::Caption()
// return the app title in current system language
//
	{
	return iCaption;
	}


TUid CTestApp::AppDllUid()const
	{
	return KUidTestApp;
	}
	
CDictionaryStore* CTestApp::OpenIniFileLC(RFs& aFs) const
// Opens the applications ini file if it exists, otherwise creates a new one
// The ini file is located on KTestAppIniFilePath
	{
	aFs.MkDirAll(KTestAppIniFilePath); // ignore the error
	//
	// open the ini file if it exists, otherwise create a new one
	return CDictionaryFileStore::OpenLC(aFs, KTestAppIniFilePath(), AppDllUid());
	}


void CTestApp::Capability(TDes8& aInfo)const
	{
	RApaLsSession apaLsSession;
	TInt err = apaLsSession.Connect();
	__ASSERT_ALWAYS(!err, User::Panic(_L("CTestApp::Capability"), err));	
	apaLsSession.GetAppCapability(aInfo, CTestApp::AppDllUid());
	apaLsSession.Close();
	}


/////////////////////////////////////////
// CTestAppDoc
/////////////////////////////////////////

CTestAppDoc* CTestAppDoc::NewL(CEikApplication& aApp)
	{
	CTestAppDoc* self=new(ELeave) CTestAppDoc(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


CTestAppDoc::CTestAppDoc(CEikApplication& aApp)
	: CEikDocument(aApp),
	iValue(0)
	{}

	
void CTestAppDoc::ConstructL()
	{
	iEmbedList = new(ELeave) CArrayFixFlat<TPictureHeader>(1);
	}

CEikAppUi* CTestAppDoc::CreateAppUiL()
	{
	
	return new(ELeave) CBasicAppUi();
	}

CTestAppDoc::~CTestAppDoc()
	{
	// delete all the embedded doc's first...
	if (iEmbedList)
		{
		for (TInt i=iEmbedList->Count()-1 ; i>=0 ; i--)
			delete EmbeddedDoor(i); // destroys the door (which in turn removes doc from the process and deletes it)
		}
	// then...
	delete iEmbedList;
	delete iStore;
	}


TBool CTestAppDoc::IsEmpty()const
	{
	// return ETrue if the document is empty
	if (iValue==0)
		return ETrue;
	else 
		return EFalse;
	}


TBool CTestAppDoc::HasChanged()const
	{
	return iHasChanged;
	}

/** builds a new embedded or main document without loading from a store (may create the content from 
	eg code or a template file).
*/
void CTestAppDoc::NewDocumentL()
	{}


/** Create a document file & store of the specified name, storing into the store.
    On system startup the name might not have a filename component in which case this function should parse the 
    passed name with a related default file name appropriate to this application and in an appropriate language
*/
CFileStore* CTestAppDoc::CreateFileStoreLC(RFs& aFs,const TDesC& aFileName)
	{
	// set the default name just in case...
	TFileName defaultName(_L("temp"));
	TParse parser;
	User::LeaveIfError(parser.Set(aFileName,&defaultName,NULL));
	// create the file store
	CFileStore* store;
	store = CDirectFileStore::CreateLC(aFs,parser.FullName(),EFileWrite);
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid,KUidAppDllDoc,KUidTestApp));
	CStreamDictionary* streamDic=CStreamDictionary::NewL();
	CleanupStack::PushL(streamDic);
	StoreL(*store,*streamDic); 
	Process()->WriteRootStreamL(*store,*streamDic,*Application());
	store->CommitL(); // now the store will be fully initialised
	CleanupStack::PopAndDestroy(); // streamDic
	return store;
	}


void CTestAppDoc::EditL(MApaEmbeddedDocObserver* /*aContainer*/,TBool aReadOnly)
	{
	if (aReadOnly)
		{
		if (iContainer)
			{
			iContainer->NotifyExit(MApaEmbeddedDocObserver::ENoChanges);
			iContainer = NULL; // iContainer must be nulled when editing is finished
			}
		}
	else
		{
		iValue++;
		iHasChanged = ETrue;
		if (iContainer)
			{
			iContainer->NotifyExit(MApaEmbeddedDocObserver::EKeepChanges);
			iContainer = NULL; // iContainer must be nulled when editing is finished
			}
		}
	}

void CTestAppDoc::NotifyExit(MApaEmbeddedDocObserver::TExitMode aMode)
	{
	switch (aMode)
		{
		case EKeepChanges:
			iHasChanged = ETrue; // note that my contents have changed
			break;
		case ERevertToSaved:
			// reload whole document (panic if I'm not the main doc)
			break;
		case ENoChanges:
			// no changes
			break;
		case EEmpty:
			// the embedded doc is empty
			break;
		}
	}


void CTestAppDoc::PrintL(const CStreamStore& /*aSourceStore*/)
	{}


void CTestAppDoc::SaveL()
	{

	CDirectFileStore* store;
	TParse newFilePath;
	// create temp file
	User::LeaveIfError( newFilePath.Set(Process()->MainDocFileName(),NULL,NULL) ); // abuse new file path
	TBuf<2> drive=newFilePath.Drive();
	TFileName filePath(KTempFilePath);
	User::LeaveIfError( newFilePath.Set(drive,&filePath,NULL) );
	Process()->FsSession().MkDirAll(newFilePath.DriveAndPath());
	TFileName tempName;
	store = CDirectFileStore::TempLC(Process()->FsSession(),newFilePath.DriveAndPath(),tempName,EFileWrite);
	store->SetTypeL(((CFileStore*)iStore)->Type());

	// store main in temp
	CStreamDictionary* streamDic=CStreamDictionary::NewL();
	CleanupStack::PushL(streamDic);
	StoreL(*store,*streamDic);
	iStore->CommitL();

	// write root stream
	Process()->WriteRootStreamL(*store,*streamDic,*Application());
	CleanupStack::PopAndDestroy(); // streamDic
	// close the new store
	store->CommitL();
	CleanupStack::PopAndDestroy(); // store
	// close the old store
	delete iStore;
	iStore = NULL;
	// replace the old file
	User::LeaveIfError( Process()->FsSession().Replace(tempName,Process()->MainDocFileName()) );
	// open new file
	iStore = CDirectFileStore::OpenL(Process()->FsSession(),Process()->MainDocFileName(),EFileShareExclusive);
	}


void CTestAppDoc::StoreL(CStreamStore& aStore,CStreamDictionary& aStreamDic)const
	{
	//
	// create map and store embedded doc's
	CStoreMap* map=CStoreMap::NewLC(aStore);
	StoreComponentsL(aStore,*map);
	// store the headstream
	RStoreWriteStream stream(*map);
	TStreamId id=stream.CreateLC(aStore); 
	ExternalizeL(stream);
	stream.CommitL();
	// assign the headstream in the dictionary
	aStreamDic.AssignL(KUidTestAppHeadStream,id);
	// tidy up
	map->Reset();
	CleanupStack::PopAndDestroy(2); // map,stream
	}


void CTestAppDoc::RestoreL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic)
	{
	//
	TStreamId headStreamId=aStreamDic.At(KUidTestAppHeadStream);
	RStoreReadStream stream;
	stream.OpenLC(aStore,headStreamId); 
	InternalizeL(stream);
	CleanupStack::PopAndDestroy(); // stream
	// restore the embedded bits
	RestoreComponentsL(aStore);
	}


void CTestAppDoc::DetachFromStoreL(CPicture::TDetach aDegree)
	{
	for (TInt i=0 ; i<iEmbedList->Count() ; i++)
		EmbeddedDoor(i)->DetachFromStoreL(aDegree);
	}


void CTestAppDoc::StoreComponentsL(CStreamStore& aStore,CStoreMap& aMap)const
	{
	for (TInt i=0 ; i<iEmbedList->Count() ; i++)
		{
		TStreamId id=EmbeddedDoor(i)->StoreL(aStore);
		aMap.BindL((*iEmbedList)[i].iPicture,id);
		}
	}


void CTestAppDoc::RestoreComponentsL(const CStreamStore& aStore)
	{
	TApaPictureFactory factory(Process());
	for (TInt i=0 ; i<iEmbedList->Count() ; i++)
		{
		TRAPD(ret, factory.NewPictureL((*iEmbedList)[i],aStore) );
		if ((ret!=KErrNone)&&(ret!=KErrNotFound))
			User::Leave(ret);
		}
	}


void CTestAppDoc::ExternalizeL(RWriteStream& aStream)const
	{
	aStream.WriteInt32L(iValue);
	// externalize num of embedded objects
	TInt numObjects=iEmbedList->Count();
	aStream.WriteInt32L(numObjects);
	// externalize the doors
	for (TInt i=0 ; i<numObjects ; i++)
		aStream<< (*iEmbedList)[i].iPicture;
	}


void CTestAppDoc::InternalizeL(RReadStream& aStream)
	{
	// reset();
	iValue = aStream.ReadInt32L();
	TInt numObjects=aStream.ReadInt32L();
	for (TInt i=0 ; i<numObjects ; i++)
		{
		TPictureHeader header;
		header.iPictureType = KUidPictureTypeDoor;
		aStream>> header.iPicture;
		iEmbedList->AppendL(header);
		}
	}
