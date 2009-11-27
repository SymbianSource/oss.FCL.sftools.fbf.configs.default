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
// Source file for the implementation of the 
// application class - CChainChildApplication
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#include "t_winChainChild.h"


/**   The function is called by the UI framework to ask for the
      application's UID. The returned value is defined by the
      constant KUidTChainChildApp and must match the second value
      defined in the project definition file.
*/ 
TUid CChainChildApplication::AppDllUid() const
	{
	return KUidTChainChildApp;
	}

/**  This function is called by the UI framework at
     application start-up. It creates an instance of the
     document class.
*/
CApaDocument* CChainChildApplication::CreateDocumentL()
	{
	return new (ELeave) CChainChildDocument(*this);
	}
	
	LOCAL_C CApaApplication* NewApplication()
	{
	return new CChainChildApplication;
	}

TFileName CChainChildApplication::BitmapStoreName() const
	{
	return KNullDesC();
	}

TFileName CChainChildApplication::ResourceFileName() const
	{
	return KNullDesC();
	}


GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}



/**       The constructor of the document class just passes the
          supplied reference to the constructor initialisation list.
          The document has no real work to do in this application.
*/
CChainChildDocument::CChainChildDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}


/**     This is called by the UI framework as soon as the 
        document has been created. It creates an instance
        of the ApplicationUI. The Application UI class is
        an instance of a CEikAppUi derived class.
*/
CEikAppUi* CChainChildDocument::CreateAppUiL()
	{
    return new(ELeave) CChainChildAppUi;
	}

#include <eikserverapp.h>
/**  The second phase constructor of the application UI class.
     The application UI creates and owns the one and only view.
*/ 
void CChainChildAppUi::ConstructL()
    {
    //CEikAppUi::ConstructL();	
    CEikAppUi::BaseConstructL(ENoAppResourceFile|ENoScreenFurniture);
	}

// Check if parent ID is the KExoticOrdinalPriority	
TInt CMyAppService::TestL(TInt aParentWindowGroupID)
	{
	CCoeEnv* coeEnv = CCoeEnv::Static();
		
	
	TInt wgCount=coeEnv->WsSession().NumWindowGroups(KExoticOrdinalPriority);
		
	RDebug::Print(_L("Child - TestL: wgCount = %d"), wgCount);
	
    RArray<RWsSession::TWindowGroupChainInfo>* wgIds=new(ELeave) RArray<RWsSession::TWindowGroupChainInfo>(wgCount);
    CleanupStack::PushL(wgIds);
    User::LeaveIfError(coeEnv->WsSession().WindowGroupList(KExoticOrdinalPriority,wgIds));
        
    TBool testPassed=EFalse;

    // The root identifier of this window
    TInt rootIdentifier = coeEnv->RootWin().Identifier();
    
    RDebug::Print(_L("Child - TestL: rootIdentifier = %d"), rootIdentifier);
    
    RDebug::Print(_L("Child - TestL: Entering loop.."));
    // Go through all window group IDs looking for the current one
    for (TInt i=0;i<wgCount;i++)
        {
        RWsSession::TWindowGroupChainInfo wgId=(*wgIds)[i];
        
        RDebug::Print(_L("Child - TestL: wgId[%d].iId = %d"), i, wgId.iId);
        RDebug::Print(_L("Child - TestL: wgId[%d].iParentId = %d"), i, wgId.iParentId);
        
        // If this is the current window group ID
        if (wgId.iId == rootIdentifier)
        	{
        	RDebug::Print(_L("Child - TestL: Root Identifier = wgId.Id on %d"),i);
        	testPassed=(wgId.iParentId == aParentWindowGroupID);
        	RDebug::Print(_L("Child - TestL: TestPassed = %d"), testPassed);
        	break;
        	}
        }
        
	CleanupStack::PopAndDestroy();  // wgids
	
	return testPassed;
	}
	
//
// CMyAppService
//
void CMyAppService::ServiceL(const RMessage2& aMessage)
	{
	RDebug::Print(_L("Child - ServiceL: message function = %d"),aMessage.Function());
	
	switch (aMessage.Function())
	{
	case KQueryChainChild1:
		if (TestL(aMessage.Int0())) 
			{
			aMessage.Complete(KChainPass);
			}
		else
			{
			aMessage.Complete(KChainFail);	
			}
			User::Exit(-1);
		break;
	case KQueryChainChild2:
		aMessage.Complete(KErrNone);
		User::Exit(-1);
		break;
	default:
		aMessage.Complete(KErrNotSupported);
		User::Leave(KErrNotSupported);
		break;
		}
	}
	
//
// CMyEikAppServer
//
CApaAppServiceBase* CMyEikAppServer::CreateServiceL(TUid aServiceType) const
	{
	RDebug::Print(_L("Child - CreateServiceL: Creating Service Type %d"),aServiceType);
	if (aServiceType==KUidTChainServerApp)
		{
		return new(ELeave) CMyAppService;
		}
	return CEikAppServer::CreateServiceL(aServiceType);
	}	
	
	
CChainChildAppUi::~CChainChildAppUi()
	{

	}

void CChainChildAppUi::HandleCommandL(TInt /*aCommand*/)
	{
	
	}

void CChainChildAppUi::RunTestStepL(TInt /*aNextStep*/)
	{
	}
	


    
