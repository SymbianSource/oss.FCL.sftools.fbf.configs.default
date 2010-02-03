// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code.
*/


#include <e32std.h>
#include <eikstart.h>
#include "ticoncaptionoverride.h"

const TUid KIconCapOverrideAppUid = {0x2001B674}; // test application UID

CIconCaptionOverrideView* CIconCaptionOverrideView::NewL( const TRect& aRect )
    {
    CIconCaptionOverrideView* self = CIconCaptionOverrideView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

CIconCaptionOverrideView* CIconCaptionOverrideView::NewLC( const TRect& aRect )
    {
    CIconCaptionOverrideView* self = new ( ELeave ) CIconCaptionOverrideView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

void CIconCaptionOverrideView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

CIconCaptionOverrideView::CIconCaptionOverrideView()
    {
    // No implementation required
    }

CIconCaptionOverrideView::~CIconCaptionOverrideView()
    {
    // No implementation required
    }

void CIconCaptionOverrideView::Draw( const TRect& /*aRect*/ ) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect drawRect( Rect());

    // Clears the screen
    gc.Clear( drawRect );
    
  	}

void CIconCaptionOverrideView::SizeChanged()
    {  
    DrawNow();
    }

void CIconCaptionOverrideAppUi::ConstructL()
    {
    // Initialise app UI. Without EAknEnableSkin the dialogs do not render properly in 3.x.
    //BaseConstructL(EAknEnableSkin);
	BaseConstructL();

	// Create view object
	iAppView = CIconCaptionOverrideView::NewL( ClientRect() );	
	}

CIconCaptionOverrideAppUi::CIconCaptionOverrideAppUi()
    {
    // No implementation required
    }

CIconCaptionOverrideAppUi::~CIconCaptionOverrideAppUi()
    {
    if ( iAppView )
        {
        delete iAppView;
        iAppView = NULL;        
        }
    }

void CIconCaptionOverrideAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        //case EAknSoftkeyExit:
            Exit();
            break;
        default:
            break;
        }
    }

/*void CIconCaptionOverrideAppUi::HandleStatusPaneSizeChange()
	{
	iAppView->SetRect( ClientRect() );
	}
*/
CIconCaptionOverrideDocument* CIconCaptionOverrideDocument::NewL( CEikApplication&
                                                          aApp )
    {
    CIconCaptionOverrideDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

CIconCaptionOverrideDocument* CIconCaptionOverrideDocument::NewLC( CEikApplication&
                                                           aApp )
    {
    CIconCaptionOverrideDocument* self =
        new ( ELeave ) CIconCaptionOverrideDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CIconCaptionOverrideDocument::ConstructL()
    {
    // No implementation required
    }

CIconCaptionOverrideDocument::CIconCaptionOverrideDocument( CEikApplication& aApp )
    : CEikDocument( aApp )
    {
    // No implementation required
    }

CIconCaptionOverrideDocument::~CIconCaptionOverrideDocument()
    {
    // No implementation required
    }
    
CEikAppUi* CIconCaptionOverrideDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave )
                                        CIconCaptionOverrideAppUi ) );
    }

CApaDocument* CIconCaptionOverrideApplication::CreateDocumentL()
    {
    return (static_cast<CApaDocument*>
                    ( CIconCaptionOverrideDocument::NewL( *this ) ) );
    }

TUid CIconCaptionOverrideApplication::AppDllUid() const
    {
    return KIconCapOverrideAppUid;
    }

LOCAL_C CApaApplication* NewApplication()
	{
	return new CIconCaptionOverrideApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}


	
