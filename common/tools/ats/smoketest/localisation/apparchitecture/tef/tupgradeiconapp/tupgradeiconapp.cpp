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

#include <eikstart.h>


#include "tupgradeiconapp.h"

const TUid KUidUpgradeIconApp = { 0xA0003195 };


////////////////////////////////////////////////////////////////////////
//
// Implementation of the application class - CExampleApplication
//
////////////////////////////////////////////////////////////////////////

TUid CExampleApplication::AppDllUid() const
	{
	return KUidUpgradeIconApp;
	}

CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}
	

////////////////////////////////////////////////////////////////////////
//
// Implementation of the appui class - CExampleAppUi
//
////////////////////////////////////////////////////////////////////////

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
		case EEikCmdExit: 
			Exit();
			break;
		}
	}



////////////////////////////////////////////////////////////////////////
//
// Implementation of the view class - CExampleAppView
//
////////////////////////////////////////////////////////////////////////

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
   	// Fetch the text from the resource file.
	iExampleText = iEikonEnv->AllocReadResourceL(R_UPGRADEICONAPP_TEXT_TESTAPP);

    // Control is a window owning control
	CreateWindowL();

   // Extent of the control. This is
   // the whole rectangle available to application.
   // The rectangle is passed to us from the application UI.
	SetRect(aRect);

   // At this stage, the control is ready to draw so
   // we tell the UI framework by activating it.
	ActivateL();
	}


void CExampleAppView::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();			// Window graphics context
	TRect      drawRect = Rect();		// Area in which we shall draw	
	const CFont*     fontUsed;			// Font used for drawing text
	
	// Start with a clear screen
	gc.Clear();
	
	// Draw an outline rectangle (the default pen and brush styles ensure this) slightly smaller than the drawing area.
	drawRect.Shrink(10,10);		   	
	gc.DrawRect(drawRect);
    
    // Use the title font supplied by the UI
	fontUsed = iEikonEnv->TitleFont();
	gc.UseFont(fontUsed);
	
	// Draw the text in the middle of the rectangle.
	TInt   baselineOffset=(drawRect.Height() - fontUsed->HeightInPixels())/2; 
	gc.DrawText(*iExampleText,drawRect,baselineOffset,CGraphicsContext::ECenter, 0);
    
    // Finished using the font
	gc.DiscardFont();
	}



////////////////////////////////////////////////////////////////////////
//
// Implementation of the document class - CExampleDocument
//
////////////////////////////////////////////////////////////////////////

CExampleDocument::CExampleDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}

CEikAppUi* CExampleDocument::CreateAppUiL()
	{
    return new(ELeave) CExampleAppUi;
	}


LOCAL_C CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}
	
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
	
