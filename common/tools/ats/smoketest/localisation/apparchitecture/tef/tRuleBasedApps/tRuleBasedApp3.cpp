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
// This is a simple Application containing a single view with
// text drawn on it.
// The app includes code for displaying a very simple menu.
// This source file contains the single exported function required by
// all UI Applications and the E32Dll function which is also required
// but is not used here.
// The class definitions are:
// CTRuleBasedApplication
// CTRuleBasedAppUi
// CTRuleBasedAppView
// CTRuleBasedDocument
// 
//




/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <coeccntx.h>
#include <eikenv.h>
#include <eikAppui.h>
#include <eikApp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <eikstart.h>

#include <eikon.hrh>

#include <tRuleBasedApp3.rsg>
#include "tRuleBasedApp.hrh"

//const TInt KLeft 	= 0; // Display app window on the left
const TInt KCentre 	= 1; // Display app window in the centre
const TInt KRight 	= 2; // Display app window on the right

////////////////////////////////////////////////////////////////////////
//
// CTRuleBasedApplication
//
////////////////////////////////////////////////////////////////////////

class CTRuleBasedApplication : public CEikApplication
	{
private:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};
	
////////////////////////////////////////////////////////////////////////
//
// CTRuleBasedAppView
//
////////////////////////////////////////////////////////////////////////
class CTRuleBasedAppView : public CCoeControl
 	{
public:
	static CTRuleBasedAppView* NewL(const TRect& aRect);
	CTRuleBasedAppView();
	~CTRuleBasedAppView();
	void ConstructL(const TRect& aRect);
public:
	TInt iAppType;

private:
	// Inherited from CCoeControl
	void Draw(const TRect& /*aRect*/) const;
	const CFont* CreateCustomFontL( const TDesC& aFontName, TInt aHeightInPixels, TBool aBold ) const;

private:
	HBufC* iRuleBasedText;
 	};

////////////////////////////////////////////////////////////////////////
//
// CTRuleBasedAppUi
//
////////////////////////////////////////////////////////////////////////
class CTRuleBasedAppUi : public CEikAppUi
	{
public:
	void ConstructL();
	~CTRuleBasedAppUi();

private:
	// Inherirted from class CEikAppUi
	void HandleCommandL(TInt aCommand);

private:
	CTRuleBasedAppView* iAppView;
	};

////////////////////////////////////////////////////////////////////////
//
// CTRuleBasedDocument
//
////////////////////////////////////////////////////////////////////////
class CTRuleBasedDocument : public CEikDocument
	{
public:
	static CTRuleBasedDocument* NewL(CEikApplication& aApp);
	CTRuleBasedDocument(CEikApplication& aApp);
	void ConstructL();
private:
	// Inherited from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

//////////////////////////////////////////////////////////////////////
//
// Application Document class - CTRuleBasedAppDocument
//
//////////////////////////////////////////////////////////////////////

/**
The constructor of the document class just passes the
supplied reference to the constructor initialisation list.
The document has no real work to do in this Application.
*/

CTRuleBasedDocument::CTRuleBasedDocument(CEikApplication& aApp)
	: CEikDocument(aApp)
	{
	}

/**
This is called by the UI framework as soon as the
document has been created. It creates an instance
of the ApplicationUI. The Application UI class is
an instance of a CEikAppUi derived class.
*/

CEikAppUi* CTRuleBasedDocument::CreateAppUiL()
	{
 	return new(ELeave) CTRuleBasedAppUi;
	}

////////////////////////////////////////////////////////////////////////
//
// Source file for the implementation of the
// Application view class - CTRuleBasedAppView
//
////////////////////////////////////////////////////////////////////////


// Constructor for the view.

CTRuleBasedAppView::CTRuleBasedAppView():iAppType(KRight)
	{
	}
/**
Static NewL() function to start the standard two
phase construction.
*/

CTRuleBasedAppView* CTRuleBasedAppView::NewL(const TRect& aRect)
	{
	CTRuleBasedAppView* self = new(ELeave) CTRuleBasedAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

//
// Destructor for the view.

CTRuleBasedAppView::~CTRuleBasedAppView()
	{
	delete iRuleBasedText;
	}

/**
Second phase construction.
*/

void CTRuleBasedAppView::ConstructL(const TRect& aRect)
 	{
	// Fetch the text from the resource file.
	iRuleBasedText = iEikonEnv->AllocReadResourceL(R_RULEBASED_TEXT);
	// Control is a window owning control

	CreateWindowL();
	// Extent of the control. This is
	// the whole rectangle available to Application.
	// The rectangle is passed to us from the Application UI.
	SetRect(aRect);
	// At this stage, the control is ready to draw so
	// we tell the UI framework by activating it.
	ActivateL();
	}

/**
Drawing the view - in this example, 
consists of drawing a simple outline rectangle
and then drawing the text in the middle.

In this example, we don't use the redraw
region because it's easier to redraw to
the whole client area.
*/

const CFont* CTRuleBasedAppView::CreateCustomFontL( const TDesC& aFontName, TInt aHeightInPixels, TBool aBold ) const
	 {
	 // Create a Symbian font spec
	 TFontSpec fontSpec;

	 // Enable antialising
	 TOpenFontSpec openSpec;

	 openSpec.SetBitmapType(EAntiAliasedGlyphBitmap);

	 openSpec.GetTFontSpec(fontSpec); // sets the antialiasing into the TFontSpec
	 fontSpec.iTypeface.iName = aFontName;
	 fontSpec.iHeight = aHeightInPixels;

	 if (aBold)
	 	{
	 	fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold );
		}
	 CFbsFont* font;
	 CWsScreenDevice& screenDev=*(CEikonEnv::Static()->ScreenDevice());
	 User::LeaveIfError(screenDev.GetNearestFontInPixels((CFont*&)font,fontSpec));
	 return font;
	 }
    
void CTRuleBasedAppView::Draw(const TRect& aRect) const
	{
	// Window graphics context
	CWindowGc& gc = SystemGc();
	// Area in which we shall draw
	TRect	drawRect = Rect();
	
	// Font used for drawing text
	const CFont* fontUsed = NULL;
	
	// Start with a clear screen
	gc.Clear();
			
	gc.DrawRect(drawRect);

	TRAPD(err, fontUsed = CreateCustomFontL(_L("Times New Roman"),20,1));
	if (err!=KErrNone)
		{
		return;
		}
		
	gc.SetPenColor(KRgbRed);
	gc.UseFont(fontUsed);
	// Draw the text in the middle of the rectangle.
	TInt baselineOffset=(aRect.Height() - fontUsed->HeightInPixels())/2;
	gc.DrawText(*iRuleBasedText,aRect,baselineOffset,CGraphicsContext::ECenter, 0);
	// Finished using the font
	gc.DiscardFont();
	}

////////////////////////////////////////////////////////////////////////
//
// Application UI class - CTRuleBasedAppUi
//
////////////////////////////////////////////////////////////////////////

/**
The second phase constructor of the Application UI class.
The Application UI creates and owns the one and only view.
*/

void CTRuleBasedAppUi::ConstructL()
	{
	// BaseConstructL() completes the UI framework's
	// construction of the App UI.
	BaseConstructL();
	// Create the single Application view in which to
	// draw the text to display the name of the Application that has been launched, passing into it
	// the rectangle available to it.
	
 	TRect cliRect = ClientRect();
 	const TInt width = 580/3;
 	
	cliRect.SetWidth(width);
	
	iAppView = CTRuleBasedAppView::NewL(cliRect);

	// The application window occupies 1/3 of the screen.
	if (iAppView->iAppType == KCentre)
		{
		TPoint Origin( width,0);
		iAppView->SetPosition(Origin);
		}
	else if (iAppView->iAppType == KRight)
		{
		TPoint Origin( (2*width),0);
		iAppView->SetPosition(Origin);
		}
	}

/**
The App Ui owns the two views and is.
therefore, responsible for destroying them
*/

CTRuleBasedAppUi::~CTRuleBasedAppUi()
	{
	delete iAppView;
	}

////////////////////////////////////////////////////////////////////////
//
// Implementation of the
// Application class - CTRuleBasedApplication
//
////////////////////////////////////////////////////////////////////////

const TUid KUidRuleBasedApp = { 0X1020D6FE };

/**
The function is called by the UI framework to ask for the
Application's UID. The returned value is defined by the
constant KUidRuleBasedAppe and must match the second value
defined in the project definition file.
*/

TUid CTRuleBasedApplication::AppDllUid() const
	{
	return KUidRuleBasedApp;
	}

/**
This function is called by the UI framework at
Application start-up. It creates an instance of the
document class.
*/

CApaDocument* CTRuleBasedApplication::CreateDocumentL()
	{
	return new (ELeave) CTRuleBasedDocument(*this);
	}

void CTRuleBasedAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	case EEikCmdExit:
		Exit();
		break;
		}
	}

/**
The entry point for the Application code. It creates
an instance of the CApaApplication derived
class, CTRuleBasedApplication.
*/

LOCAL_C CApaApplication* NewApplication()
	{
	return new CTRuleBasedApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}


#if defined( __WINS__ ) && !defined(EKA2)
GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}
	
EXPORT_C TInt WinsMain( TDesC* aCommandLine )
	{
	return EikStart::RunApplication( NewApplication, aCommandLine );
	}
#endif
