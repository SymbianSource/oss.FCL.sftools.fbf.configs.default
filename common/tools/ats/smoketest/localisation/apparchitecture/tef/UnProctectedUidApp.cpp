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

#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikenv.h>
#include <EIKSTART.H>

/** KTUnProtectedAppTestPassed is a number chosen at random to be used as an exit reason
    for UnProtectedApp.exe's application thread to signify that the test passed */
const TInt KTUnProtectedAppTestPassed = 1234;

///////////////////////////////////////////////////////////////////////
//
// CExampleAppUi
//
////////////////////////////////////////////////////////////////////////
class CExampleAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CExampleAppUi();
	};


void CExampleAppUi::ConstructL()
    {
    BaseConstructL();
    User::Exit(KTUnProtectedAppTestPassed);
	}

CExampleAppUi::~CExampleAppUi()
	{
	}

////////////////////////////////////////////////////////////////////////
//
// CExampleDocument
//
////////////////////////////////////////////////////////////////////////
class CExampleDocument : public CEikDocument
	{
public:
	static CExampleDocument* NewL(CEikApplication& aApp);
	CExampleDocument(CEikApplication& aApp);
	void ConstructL();
private: 
	           // Inherited from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

CExampleDocument::CExampleDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}

CEikAppUi* CExampleDocument::CreateAppUiL()
	{
    return new(ELeave) CExampleAppUi;
	}

////////////////////////////////////////////////////////////////////////
//
// CExampleApplication
//
////////////////////////////////////////////////////////////////////////

class CExampleApplication : public CEikApplication
	{
private: 
	// Inherited from class CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
private:
	CApaDocument* CreateDocumentL(CApaProcess* a) { return CEikApplication::CreateDocumentL(a); }
	};

const TUid KUidUnProctectedUidApp = { 0xA0001C5E }; 

TUid CExampleApplication::AppDllUid() const
	{
	return KUidUnProctectedUidApp;
	}

CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}

EXPORT_C CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
