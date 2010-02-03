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
// A simple application containing a single view with the text "Parent Process !" drawn on it.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <coeccntx.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <eikstart.h>

#include <eikon.hrh>

#include <ParentProcess.rsg>
#include "ParentProcess.hrh"

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
    
//
//             Constructor for the view.
//
CExampleAppView::CExampleAppView()
	{
	}


//             Static NewL() function to start the standard two
//             phase construction.
//
CExampleAppView* CExampleAppView::NewL(const TRect& aRect)
	{
	CExampleAppView* self = new(ELeave) CExampleAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}


//
//             Destructor for the view.
//
CExampleAppView::~CExampleAppView()
	{
	delete iExampleText;
	}


//             Second phase construction.
//
void CExampleAppView::ConstructL(const TRect& aRect)
    {
			   // Fetch the text from the resource file.
	iExampleText = iEikonEnv->AllocReadResourceL(R_EXAMPLE_TEXT_PARENTPROC);
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


//             Drawing the view - in this example, 
//             consists of drawing a simple outline rectangle
//             and then drawing the text in the middle.
//             We use the Normal font supplied by the UI.
//
//             In this example, we don't use the redraw
//             region because it's easier to redraw to
//             the whole client area.
//
void CExampleAppView::Draw(const TRect& /*aRect*/) const
	{
               // Window graphics context
	CWindowGc& gc = SystemGc();
	           // Area in which we shall draw
	TRect      drawRect = Rect();
			   // Font used for drawing text
	const CFont*     fontUsed;
	
	           // Start with a clear screen
	gc.Clear();
			   // Draw an outline rectangle (the default pen
	           // and brush styles ensure this) slightly
	           // smaller than the drawing area.
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

private:
	CCoeControl* iAppView;
	};

//             The second phase constructor of the application UI class.
//             The application UI creates and owns the one and only view.
// 
void CExampleAppUi::ConstructL()
    {
	           // BaseConstructL() completes the UI framework's
	           // construction of the App UI.
    BaseConstructL();
	           // Create the single application view in which to
	           // draw the text "Parent Process !", passing into it
	           // the rectangle available to it.
	iAppView = CExampleAppView::NewL(ClientRect());
	}


//             The app Ui owns the two views and is. 
//             therefore, responsible for destroying them
//
CExampleAppUi::~CExampleAppUi()
	{
	delete iAppView;
	}


//             Called by the UI framework when a command has been issued.
//             In this example, a command can originate through a 
//             hot-key press or by selection of a menu item.
//             The command Ids are defined in the .hrh file
//             and are 'connected' to the hot-key and menu item in the
//             resource file.
//             Note that the EEikCmdExit is defined by the UI
//             framework and is pulled in by including eikon.hrh
//
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
               // Exit the application. The call is
		       // implemented by the UI framework.

	case EEikCmdExit: 
		Exit();
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

//             The constructor of the document class just passes the
//             supplied reference to the constructor initialisation list.
//             The document has no real work to do in this application.
//
CExampleDocument::CExampleDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}


//             This is called by the UI framework as soon as the 
//             document has been created. It creates an instance
//             of the ApplicationUI. The Application UI class is
//             an instance of a CEikAppUi derived class.
//
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
	};

//             The entry point for the application code. It creates
//             an instance of the CApaApplication derived
//             class, CExampleApplication.
//

const TUid KUidParentProcess = { 0x10207f87 }; 

//             The function is called by the UI framework to ask for the
//             application's UID. The returned value is defined by the
//             constant KUidParentProcess and must match the second value
//             defined in the project definition file.
// 
TUid CExampleApplication::AppDllUid() const
	{
	return KUidParentProcess;
	}

//             This function is called by the UI framework at
//             application start-up. It creates an instance of the
//             document class.
//
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
