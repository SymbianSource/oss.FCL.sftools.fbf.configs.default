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
// Supporting application for use by T_StartDocStep.cpp
// The implementation of CR PHAR-5NTCWY removes the 256 character limit
// on the length of document names.
// This application is designed to be started by test code calling
// RApaLsSession::StartDocument(const TDesC& aFileName, TThreadId& aId,
// TLaunchType aLaunchType=ELaunchNewApp)
// passing a document name (first parameter) that is longer than
// 256 characters.
// The test passes if this application is started with a document name
// that is longer than 256 characters.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>

#include <eikstart.h>

#include "TStartDoc.h"

const TUid KUidTStartDocApp = { 0x10004c4d };

enum TStartDocAppPanic
	{
	EPanicDocNameCheck1Failed,
	EPanicDocNameCheck1NotExecuted,
	EPanicDocNameCheck2Failed,
	};

void Panic(TStartDocAppPanic aPanic)
//
// Panic the process with TESTFAILED as the category.
//
	{
	_LIT(KPanicCategory,"TESTFAILED");
	User::Panic(KPanicCategory, aPanic);
	}


////////////////////////////////////////////////////////////////////////
//
// CStartDocAppUi
//
////////////////////////////////////////////////////////////////////////
class CStartDocAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CStartDocAppUi();
public:
	TBool DocNameCheck1Executed() const;
	// from CEikAppUi
	TBool ProcessCommandParametersL(TApaCommand aCommand, TFileName& aDocumentName, const TDesC8& aTail);
private:
	TBool iDocNameCheck1Executed;
	};


void CStartDocAppUi::ConstructL()
    {
    BaseConstructL(ENoAppResourceFile | ENoScreenFurniture);
	}

CStartDocAppUi::~CStartDocAppUi()
	{
	}

TBool CStartDocAppUi::DocNameCheck1Executed() const
	{
	return iDocNameCheck1Executed;
	}

TBool CStartDocAppUi::ProcessCommandParametersL(TApaCommand /*aCommand*/, TFileName& aDocumentName, const TDesC8& /*aTail*/)
	{
	if (aDocumentName != KLitLongURL)
		{
		Panic(EPanicDocNameCheck1Failed);
		}
	iDocNameCheck1Executed = ETrue;
	return EFalse; // don't try to open the document
	}


////////////////////////////////////////////////////////////////////////
//
// CStartDocDocument
//
////////////////////////////////////////////////////////////////////////
class CStartDocDocument : public CEikDocument
	{
public:
	static CStartDocDocument* NewL(CEikApplication& aApp);
	CStartDocDocument(CEikApplication& aApp);
	void ConstructL();
	CFileStore* OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);
private: 
	// from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

CStartDocDocument::CStartDocDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}

CEikAppUi* CStartDocDocument::CreateAppUiL()
	{
    return new(ELeave) CStartDocAppUi;
	}

CFileStore* CStartDocDocument::OpenFileL(TBool /*aDoOpen*/,const TDesC& /*aFilename*/,RFs& /*aFs*/)
	{
	if (!(static_cast<CStartDocAppUi*>(CEikonEnv::Static()->EikAppUi())->DocNameCheck1Executed()))
		{
		Panic(EPanicDocNameCheck1NotExecuted);
		}
	/** the process main doc filename should have been set by the framework by the time
	    this function is called, so check it's been sucessfully set to a value that's
	    longer than 256 characters
	    (this tests the new CApaProcess::SetMainDocFileNameL API)
	*/
	  if (Process()->MainDocFileName() != KLitLongURL)
		{
		Panic(EPanicDocNameCheck2Failed);
		}
	// by exiting the app here with KTStartDocTestPassed, we ensure the test cannot pass
	// in cases where something leaves during app startup
	User::After(10000000); // ensure that the test harness gets a chance to logon to us
	User::Exit(KTStartDocTestPassed);
	return NULL; // just to keep the compiler happy
	}


////////////////////////////////////////////////////////////////////////
//
// CStartDocApplication
//
////////////////////////////////////////////////////////////////////////
class CStartDocApplication : public CEikApplication
	{
private: 
	// from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
private:
	TFileName ResourceFileName() const;
private:
	CApaDocument* CreateDocumentL(CApaProcess* a) { return CEikApplication::CreateDocumentL(a); }
	//
	};

CApaDocument* CStartDocApplication::CreateDocumentL()
	{
	return new (ELeave) CStartDocDocument(*this);
	}

TUid CStartDocApplication::AppDllUid() const
	{
	return KUidTStartDocApp;
	}

TFileName CStartDocApplication::ResourceFileName() const
	{
	return TFileName(); // this app doesn't have a resource file
	}


LOCAL_C CApaApplication* NewApplication()
	{
	return new CStartDocApplication;
	}
	
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
	
