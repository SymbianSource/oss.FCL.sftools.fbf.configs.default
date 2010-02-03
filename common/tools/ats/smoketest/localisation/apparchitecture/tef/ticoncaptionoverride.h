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
 @internalComponent - Internal Symbian test code
*/

#ifndef __TICONCAPTIONOVERRIDE_H__
#define __TICONCAPTIONOVERRIDE_H__

#include <coecntrl.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikon.hrh>
#include <w32std.h>

const TUid KUidTestIconCapOverride = {0x2001B674};

class CIconCaptionOverrideView : public CCoeControl
    {
    public: // New methods
        static CIconCaptionOverrideView* NewL( const TRect& aRect );
        static CIconCaptionOverrideView* NewLC( const TRect& aRect );
        virtual ~CIconCaptionOverrideView();

    public:  // Functions from base classes
        void Draw( const TRect& aRect ) const;
        virtual void SizeChanged();

    private: // Constructors
        void ConstructL(const TRect& aRect);
        CIconCaptionOverrideView();
    };

class CIconCaptionOverrideAppUi : public CEikAppUi
    {
    public: // Constructors and destructor
        void ConstructL();
        CIconCaptionOverrideAppUi();
        virtual ~CIconCaptionOverrideAppUi();

    private:  // Functions from base classes
        void HandleCommandL( TInt aCommand );
		//void HandleStatusPaneSizeChange();
        
    private: // Data
        CIconCaptionOverrideView* iAppView;
    };

class CIconCaptionOverrideDocument : public CEikDocument
    {
    public: // Constructors and destructor

        static CIconCaptionOverrideDocument* NewL( CEikApplication& aApp );
        static CIconCaptionOverrideDocument* NewLC( CEikApplication& aApp );
        virtual ~CIconCaptionOverrideDocument();

    public: // Functions from base classes
        CEikAppUi* CreateAppUiL();

    private: // Constructors
        void ConstructL();
        CIconCaptionOverrideDocument( CEikApplication& aApp );
    };

class CIconCaptionOverrideApplication : public CEikApplication
    {
    public: // Functions from base classes
        TUid AppDllUid() const;

    protected: // Functions from base classes
        CApaDocument* CreateDocumentL();
    };

#endif // __TICONCAPTIONOVERRIDE_H__
