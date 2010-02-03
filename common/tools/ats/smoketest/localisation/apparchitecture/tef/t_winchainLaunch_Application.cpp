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
// Source file for the implementation of the 
// application class - CChainLaunchApplication
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/
#include "t_winChainLaunch.h"

/**   The function is called by the UI framework to ask for the
      application's UID. The returned value is defined by the
      constant KUidTChainServerApp and must match the second value
      defined in the project definition file.
*/ 
TUid CChainLaunchApplication::AppDllUid() const
	{
	return KUidTChainServerApp;
	}
  
/**   This function is called by the UI framework at
      application start-up. It creates an instance of the
      document class.
*/
CApaDocument* CChainLaunchApplication::CreateDocumentL()
	{
	return new (ELeave) CChainLaunchDocument(*this);
	}
	
	LOCAL_C CApaApplication* NewApplication()
	{
	return new CChainLaunchApplication;
	}

TFileName CChainLaunchApplication::BitmapStoreName() const
	{
	return KNullDesC();
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}



/**   The constructor of the document class just passes the
      supplied reference to the constructor initialisation list.
      The document has no real work to do in this application.
*/
CChainLaunchDocument::CChainLaunchDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}


/**  This is called by the UI framework as soon as the 
     document has been created. It creates an instance
     of the ApplicationUI. The Application UI class is
     an instance of a CEikAppUi derived class.
*/
CEikAppUi* CChainLaunchDocument::CreateAppUiL()
	{
    return new(ELeave) CChainLaunchAppUi;
	}

#include <eikserverapp.h>

/** The second phase constructor of the application UI class.
    The application UI creates and owns the one and only view.
*/ 
void CChainLaunchAppUi::ConstructL()
    {
    //CEikAppUi::ConstructL();
    CEikAppUi::BaseConstructL(ENoAppResourceFile|ENoScreenFurniture);
    
    TInt rootIdentifier = iCoeEnv->RootWin().Identifier();
    
    RDebug::Print(_L("Launch - ConstructL: rootIdentifier = %d"), rootIdentifier);
    
	CEikDocument *doc = Document(); 
    CApaApplication* app = doc->Application();
    iCoeEnv->RootWin().SetOrdinalPosition(0, KExoticOrdinalPriority);	 

	//Connnect using ConnectNewChildAppL with a valid TSecurityPolicy
	ConnectNewChildAppL(KUidTChainChildApp, iCoeEnv->RootWin(),TSecurityPolicy(TVendorId(0x70000001)));
  	TInt result1=User::LeaveIfError(SendReceive(KQueryChainChild1, TIpcArgs(iCoeEnv->RootWin().Identifier())));
    Close();
	
	//Connnect using ConnectNewAppL with a valid TSecurityPolicy
	ConnectNewAppL(KUidTChainChildApp, TSecurityPolicy(TVendorId(0x70000001)));
	TInt result2=User::LeaveIfError(SendReceive(KQueryChainChild2, TIpcArgs(iCoeEnv->RootWin().Identifier())));
	Close();
	
	RDebug::Print(_L("Launch - ConstructL: result1 = %d, result2 = %d"), result1, result2);
	
	if (result1 == KChainPass && result2 ==0 )
		{
		User::Exit(KTWindowChainingTestPassed);
		}
	else
		{
		User::Exit(-1);
		}
	}

TUid CChainLaunchAppUi::ServiceUid() const
	{
	CEikDocument *doc = Document();
    CApaApplication* app = doc->Application();
    
    return app->AppDllUid();
	}

	
/**  The app Ui owns the two views and is. 
     therefore, responsible for destroying them
*/
CChainLaunchAppUi::~CChainLaunchAppUi()
	{
	}


void CChainLaunchAppUi::RunTestStepL(TInt /*aNextStep*/)
	{
	}
	
