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
// The class definitions for the simple test application
// which is launched by t_winchainLaunch.  The application
// checks that the window group id matches the parent app
// and returns a message based on the result.
// The class definitions are:
// CMyAppService
// CMyEikAppServer
// CChainChildApplication 
// CChainChildAppUi
// CChainChildDocument
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#ifndef __T_WINCHAINCHILD_H
#define __T_WINCHAINCHILD_H

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
// CMyAppService
////////////////////////////////////////////////////////////////////////
class CMyAppService : public CApaAppServiceBase
	{
private:
	void ServiceL(const RMessage2& aMessage);
	TInt TestL(TInt aParentWindowGroupID);
	};
	

////////////////////////////////////////////////////////////////////////
// CMyEikAppServer
////////////////////////////////////////////////////////////////////////	
class CMyEikAppServer : public CEikAppServer
	{
private:
	CApaAppServiceBase* CreateServiceL(TUid aServiceType) const;	
	};

	
////////////////////////////////////////////////////////////////////////
//
// CChainChildApplication
//
////////////////////////////////////////////////////////////////////////
class CChainChildApplication : public CEikApplication
	{
public:
	void NewAppServerL(CApaAppServer*& aAppServer)
		{
		aAppServer=new(ELeave) CMyEikAppServer;
		};
	
	TFileName BitmapStoreName() const;
	TFileName ResourceFileName() const;
	
private: 
	// Inherited from class CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

////////////////////////////////////////////////////////////////////////
//
// CChainChildAppUi
//
////////////////////////////////////////////////////////////////////////
class CChainChildAppUi : public CEikAppUi
    {
public:
	CChainChildAppUi() :
	   CEikAppUi()  
		{
		}
    void ConstructL();
	~CChainChildAppUi();
	virtual void RunTestStepL(TInt aNextStep); 
	
private:
    // Inherited from class CEikAppUi
	void HandleCommandL(TInt aCommand);

	
private:
	
	CCoeControl* iAppView;

	};


////////////////////////////////////////////////////////////////////////
//
// CChainChildDocument
//
////////////////////////////////////////////////////////////////////////
class CChainChildDocument : public CEikDocument
	{
public:
	static CChainChildDocument* NewL(CEikApplication& aApp);
	CChainChildDocument(CEikApplication& aApp);
	void ConstructL();
private: 
	// Inherited from CEikDocument
	CEikAppUi* CreateAppUiL();
	};


#endif // T_WINCHAINCHILD
