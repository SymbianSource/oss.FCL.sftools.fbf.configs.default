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
// The class definitions for the simple test application
// which launches the child application with window chaining
// The class definitions are:
// CChainLaunchApplication 
// CChainLaunchAppUi
// CChainLaunchDocument
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#ifndef __T_WINCHAINLAUNCH_H
#define __T_WINCHAINLAUNCH_H

#include <coeccntx.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <eikstart.h> ////TKAS added for exe-app

#include <eikserverapp.h> // REikAppServiceBase
#include <eikon.hrh>

//  Test stuff
#include "twindowchaining.h"

class CPackagerAppUi;
	
////////////////////////////////////////////////////////////////////////
//
// CChainLaunchApplication
//
////////////////////////////////////////////////////////////////////////

class CChainLaunchApplication : public CEikApplication
	{
private: 
	// Inherited from class CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	TFileName BitmapStoreName() const;
	};


////////////////////////////////////////////////////////////////////////
//
// CChainLaunchAppUi
//
////////////////////////////////////////////////////////////////////////
class CChainLaunchAppUi : public CEikAppUi, REikAppServiceBase
    {
public:
	CChainLaunchAppUi() :
	   CEikAppUi()  
		{
		}
    void ConstructL();
	~CChainLaunchAppUi();
	virtual void RunTestStepL(TInt aNextStep); 
	
private:
	// from RApaAppServiceBase
	TUid ServiceUid() const; 
	
	
private:
	
	CCoeControl* iAppView;

	};


////////////////////////////////////////////////////////////////////////
//
// CChainLaunchDocument
//
////////////////////////////////////////////////////////////////////////
class CChainLaunchDocument : public CEikDocument
	{
public:
	static CChainLaunchDocument* NewL(CEikApplication& aApp);
	CChainLaunchDocument(CEikApplication& aApp);
	void ConstructL();
private: 
	// Inherited from CEikDocument
	CEikAppUi* CreateAppUiL();
	};


#endif // T_WINCHAINLAUNCH
