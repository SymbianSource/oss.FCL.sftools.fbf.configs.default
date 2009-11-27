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
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#include <e32uid.h>
#include <e32base.h>
#include <e32test.h>
#include <apgctl.h>

#include <eikdll.h>
#include <coeccntx.h>
#include <apgtask.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <apgcli.h>

#include <eikon.hrh>
//#include "appfwk_test_AppUi.h"

#include <M_CTRL.rsg>
#include "M_CTRL.hrh"
#include <EIKDLL.H>
#include <eikstart.h>


//const TInt KMinimalControlUidValue=98;
//const TInt KMinimalControlUidValue=0x13008AEE;	//Changed in m_ctrl.mmp file

//_LIT(KGlobalSemaphore, "GlobalSemaphore");


////////////////////////////////////////////////////////////////////////
//
// CExampleAppView
//
////////////////////////////////////////////////////////////////////////
class CExampleAppView : public CCoeControl
    {
public:
	static CExampleAppView* NewL(const TRect& aRect);
	CExampleAppView();
	~CExampleAppView();
    void ConstructL(const TRect& aRect);

private:
	           // Inherited from CCoeControl
	void Draw(const TRect& /*aRect*/) const;

private:
	HBufC*  iExampleText;
    };

CExampleAppView::CExampleAppView()
	{
	}

CExampleAppView* CExampleAppView::NewL(const TRect& aRect)
	{
	CExampleAppView* self = new(ELeave) CExampleAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

CExampleAppView::~CExampleAppView()
	{
	delete iExampleText;
	}

void CExampleAppView::ConstructL(const TRect& aRect)
    {
	iExampleText = iEikonEnv->AllocReadResourceL(R_EXAMPLE_TEXT_TITLE);
	CreateWindowL();
	SetRect(aRect);
	ActivateL();
	}

void CExampleAppView::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();
	TRect      drawRect = Rect();
	const CFont*     fontUsed;
	
	gc.Clear();
  	fontUsed = iEikonEnv->TitleFont();
	gc.UseFont(fontUsed);
	TInt   baselineOffset=(drawRect.Height() - fontUsed->HeightInPixels())/2; 
	gc.DrawText(*iExampleText,drawRect,baselineOffset,CGraphicsContext::ECenter, 0);
	gc.DiscardFont();
	}


////////////////////////////////////////////////////////////////////////
//
// CExampleAppUi
//
////////////////////////////////////////////////////////////////////////
class CExampleAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CExampleAppUi();
private:
              // Inherirted from class CEikAppUi
	void HandleCommandL(TInt aCommand);

	// From CCoeAppUi
	void HandleSystemEventL(const TWsEvent& aEvent);

private:
	CCoeControl* iAppView;
	};


void CExampleAppUi::ConstructL()
    {
    BaseConstructL();
	iAppView = CExampleAppView::NewL(ClientRect());
	}

CExampleAppUi::~CExampleAppUi()
	{
	delete iAppView;
	}

void CExampleAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	// Just issue simple info messages to show that
	// the menu items have been selected
	case EExampleItem0:
		iEikonEnv->InfoMsg(R_EXAMPLE_TEXT_ITEM0);
		break;

	case EExampleItem1:
		iEikonEnv->InfoMsg(R_EXAMPLE_TEXT_ITEM1);
		break;
	
	case EExampleItem2:
		iEikonEnv->InfoMsg(R_EXAMPLE_TEXT_ITEM2);
		break;
	case EEikCmdExit: 
		Exit();
		break;
		}
	}


void CExampleAppUi::HandleSystemEventL(const TWsEvent& /*aEvent*/)
	{
/*
	RSemaphore semaphore;
	
	switch (*(TApaSystemEvent*)(aEvent.EventData()))
		{

		case EApaSystemEventBroughtToForeground:
			{	
			semaphore.OpenGlobal(KGlobalSemaphore);
			semaphore.Signal();
			semaphore.Close();
            break;
			}
         default:
			break;
			}	
*/
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
	//
	};

const TUid KUidSimpleApp = { 0x13008AEE }; 

TUid CExampleApplication::AppDllUid() const
	{
	return KUidSimpleApp;
	}

CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}


LOCAL_C CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}
	
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
	
