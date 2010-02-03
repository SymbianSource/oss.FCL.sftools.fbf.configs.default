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
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TSTAPP_H__
#define __TSTAPP_H__

#if !defined(__APPARC_H__)
#include <apparc.h>
#endif

#include <eikapp.h>
#include <eikdoc.h>
#include <apgdoor.h>
#include <eikappui.h>

// classes defined:
class CTestApp;
class CTestAppDoc;
//

#define KTestAppDefaultFileName _L("TestApp.doc")
const TInt KTestAppUidValue=10;
const TUid KUidTestApp={KTestAppUidValue};
const TUid KUidTestAppHeadStream={200};
const TInt KNarrowRelUidValue=268435578;
const TUid KUidNarrowRel={KNarrowRelUidValue}; // so it'll run under arm...
#define KTestAppUidType TUidType(KUidNarrowRel,KUidApp,KUidTestApp)


class CTestApp : public CEikApplication
	{
public:
	static CTestApp* NewL(); // the gated function
	// from CApaApplication
	void PreDocConstructL();
	CEikDocument* CreateDocumentL();
	TDesC& Caption(); // return the app title in current system language
	TUid AppDllUid()const;
	
	CDictionaryStore* OpenIniFileLC(RFs& aFs)const;
	void Capability(TDes8& aInfo)const;
	~CTestApp();

private:
	CTestApp();
	void ConstructL();
	
private:
	TBuf<8> iCaption;
	RFs iFs;
	};

class CBasicAppUi : public CEikAppUi
	{
public:
	inline CBasicAppUi() {}
private:
	virtual void ConstructL();
	};

class CTestAppDoc : public CEikDocument, public MApaEmbeddedDocObserver
	{
public:
	static CTestAppDoc* NewL(CEikApplication& aApp);
	~CTestAppDoc();

	inline void EmbedNewDocL(const TApaApplicationFactory& aTestFac); // exported so I can call it from testcode
	inline void EditEmbeddedDocL(TInt aDocNum) {EmbeddedDoor(aDocNum)->DocumentL()->EditL(this);}
	inline CApaDoor* EmbeddedDoor(TInt aDocNum)const {return (CApaDoor*)((*iEmbedList)[aDocNum].iPicture.AsPtr());}
	virtual CEikAppUi* CreateAppUiL();
	
// from MApaEmbeddedDocObserver
	void NotifyExit(MApaEmbeddedDocObserver::TExitMode aMode);
// from CApaDocument
	void NewDocumentL(); 
	CFileStore* CreateFileStoreLC(RFs& aFs,const TDesC& aFileName);
	void SaveL();
	//
	void PrintL(const CStreamStore& aSourceStore);
	void EditL(MApaEmbeddedDocObserver* aContainer,TBool aReadOnly); 
	TBool IsEmpty()const; // return ETrue if the document is empty
	TBool HasChanged()const;
	//
	// storage functions
	void StoreL(CStreamStore& aStore,CStreamDictionary& aStreamDic)const;
	void RestoreL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic);
	void DetachFromStoreL(CPicture::TDetach aDegree);

protected:
	CTestAppDoc(CEikApplication& aApp);
	void ConstructL();

	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream)const;
	void StoreComponentsL(CStreamStore& aStore,CStoreMap& aMap)const;
	void RestoreComponentsL(const CStreamStore& aStore);

	// utility functions
public:
	TInt iValue; // the "contents" of this test document
	CArrayFixFlat<TPictureHeader>* iEmbedList;
	CStreamStore* iStore;
	TBool iHasChanged;
	};

inline void CTestAppDoc::EmbedNewDocL(const TApaApplicationFactory& aTestFac)
	{
	TPictureHeader header;
	header.iPictureType = KUidPictureTypeDoor;

	CApaDocument* doc=Process()->AddNewDocumentL(aTestFac);

	RFs& fs=Process()->FsSession();
	header.iPicture = CApaDoor::NewLC(fs,*doc,TSize(500,500));

	iEmbedList->AppendL(header);
	CleanupStack::Pop(); // iPicture
	}


#endif
