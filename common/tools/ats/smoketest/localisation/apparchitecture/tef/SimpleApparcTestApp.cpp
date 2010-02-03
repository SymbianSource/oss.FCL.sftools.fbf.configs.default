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
// This is a support app used by apparc\tsrc\t_foreground
// Test for UIKON GT0143 Typhoon Work Series 60 Changes
// REQ758.8: Change TApaTask::BringToForeground to send a 
// EApaSystemEventBroughtToForeground to the relevant window group
// t_foreground sends a window server message to this app, this 
// message is handled in CExampleAppUi::HandleSystemEventL
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/



#include <coeccntx.h>

#include <apgtask.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>

#include <eikon.hrh>

#include <SimpleApparcTestApp.rsg>
#include "SimpleApparcTestApp.hrh"

#include <eikstart.h>


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


void CExampleAppUi::HandleSystemEventL(const TWsEvent& aEvent)
	{
	switch (*(TApaSystemEvent*)(aEvent.EventData()))
		{
		case EApaSystemEventBroughtToForeground:
			RProcess::Rendezvous(KErrNone);
			break;
         default:
			break;
		}	
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

const TUid KUidSimpleApparcTestApp = { 0X12008ACE }; 

TUid CExampleApplication::AppDllUid() const
	{
	return KUidSimpleApparcTestApp;
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
	
