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
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <eikmenub.h>
#include <eikbtgpc.h>
#include <eikenv.h>
#include <hal.h>

#include "appfwk_test_AppUi.h"


EXPORT_C void PreallocateHALBuffer()
	{
	HAL::SEntry* pE=NULL;
	TInt entries=0;
	HAL::GetAll(entries, pE);
	User::Free(pE);
	}

EXPORT_C void CloseAllPanicWindowsL()
	{
	RWsSession	ws;
	User::LeaveIfError(ws.Connect());

	TInt wgFocus = ws.GetFocusWindowGroup();

	const TUint32 ENullWsHandle = 0xFFFFFFFF;	// Events delivered to this handle are thrown away
	RWindowGroup wg = RWindowGroup(ws);

	wg.Construct(ENullWsHandle);
	TInt wgId = wg.Identifier();

	TWsEvent event;
	event.SetType(EEventKey);
	TKeyEvent *keyEvent = event.Key();
	keyEvent->iCode = EKeyEscape;
	keyEvent->iScanCode = EStdKeyEscape;
	keyEvent->iModifiers = 0;

	TInt limit = 0;
	for(limit = 0; wgFocus != wgId && (limit < 50); limit++)
	{
		ws.SendEventToAllWindowGroups(event);
		wgFocus = ws.GetFocusWindowGroup();
		RDebug::Print(_L("CloseAllPanicWindowsL() - EKeyEscape sent to Windows Group"));
	}

	// close everything
	wg.Close();
	ws.Close();
	}


EXPORT_C CAutoTestManager::CAutoTestManager(MAutoTestApp* aAutoTestApp) :
	CActive(EPriorityIdle), iAutoTestApp(aAutoTestApp)
	{
	}


EXPORT_C CAutoTestManager::~CAutoTestManager()
	{
	Cancel();
	}
	

EXPORT_C CAutoTestManager* CAutoTestManager::NewL(MAutoTestApp* aAutoTestApp)
	{
     CAutoTestManager *theTestManager=new (ELeave) CAutoTestManager(aAutoTestApp);
     CActiveScheduler::Add(theTestManager);
	 return theTestManager;
	}


void CAutoTestManager::RunL()
	{
	__ASSERT_DEBUG(iAutoTestApp, User::Panic(KPanicNullPointer, KNullPointer));
	iAutoTestApp->RunTestStepL(++iNumStep);
	StartAutoTest();
	}
	

void CAutoTestManager::DoCancel()
	{
	}


/**
  Init active object and set it active
 */
EXPORT_C void CAutoTestManager::StartAutoTest()
	{
	TRequestStatus *pS=(&iStatus);
	User::RequestComplete(pS,0);
	SetActive();
	}


/**
  This function must be called after all test cases will be completed 	
 */
EXPORT_C void CAutoTestManager::FinishAllTestCases(TTestResult aResult)
	{
	iResult = aResult;
	if(aResult != EPass)
		{
		RDebug::Print(_L("\nTest completed with error %d"), aResult);
		}
	CCoeEnv::Static()->Flush();
	CBaActiveScheduler::Exit();
	}
	
	
/**
  This function will set the internal test state variable to the user defined state.
 */
EXPORT_C void CAutoTestManager::SetTestCaseNumber(TInt aNumber)
	{
	iNumStep = aNumber;
	}


EXPORT_C CTestCoeAppUi::CTestCoeAppUi(CTestStep* aStep) :
		iStep(aStep) 
	{
	}


EXPORT_C CTestCoeAppUi::~CTestCoeAppUi()
	{
	if(iAutoTestManager)
		{
		iAutoTestManager->Cancel();
		}
	delete iAutoTestManager;
	}
	

/**
  Read thread local storage read and free thread local storage STL is used for storage  Auto attribute,  
  it sets in an export function void ProcessParam(TAny* aParam)	
 */
EXPORT_C void CTestCoeAppUi::ConstructL()
	{
	iAutoTestManager = CAutoTestManager::NewL(this);
	
	CCoeAppUi::ConstructL();
	}


EXPORT_C CTestAppUi::CTestAppUi(CTestStep* aStep, const	TDesC& aRes, TInt aResourceHotKeysId,  TInt aResourceMenuBarId, TInt aResourceToolBarId) :
		CEikAppUi(),
		iResourceHotKeysId(aResourceHotKeysId), 
		iResourceMenuBarId(aResourceMenuBarId),
		iResourceToolBarId(aResourceToolBarId),
		iStep(aStep)
	{
	iRes.Copy(aRes);
	}


EXPORT_C CTestAppUi::~CTestAppUi()
	{
	if(iResourceFileFlag)
		{
		CCoeEnv::Static()->DeleteResourceFile(iResourceFileFlag); 
		}
	
	if(iAutoTestManager)
		{
		iAutoTestManager->Cancel();
		}
	delete iAutoTestManager;
	
	if(iMenuBar)
		{
		RemoveFromStack(iMenuBar);
		}
	delete iMenuBar;
	
	if(iToolBar)
		{
		RemoveFromStack(iToolBar);
		}
	delete iToolBar;
	}


/**
   Load resource, create toolbar & menubar, read and free thread local storage	
 */
EXPORT_C void CTestAppUi::ConstructL()
	{
	iAutoTestManager = CAutoTestManager::NewL(this);
	
	if(iStep == 0)
		{
		BaseConstructL(ENoAppResourceFile);
		}
	else
		{
		CCoeAppUi::ConstructL();
		}

	if(iRes.Length() > 0)
		{
		CCoeEnv* coeEnv = CCoeEnv::Static();
		TBuf8<256>	buf;
		buf.Copy(iRes);
		iResourceFileFlag=coeEnv->AddResourceFileL(iRes);
		}

	if(iResourceMenuBarId)
		{
		iMenuBar = new (ELeave) CEikMenuBar;
		iMenuBar->ConstructL(this,iResourceHotKeysId,iResourceMenuBarId);
		iMenuBar->MakeVisible(ETrue);
		AddToStackL(iMenuBar);
		}

	if(iResourceToolBarId)
		{
		iToolBar=CEikButtonGroupContainer::NewL(CEikButtonGroupContainer::EToolbar, CEikButtonGroupContainer::EVertical, this, iResourceToolBarId);
		const TRect boundingRect = ClientRect(); // make toolband stretch to the screen width by default
		iToolBar->SetBoundingRect(boundingRect);
		iToolBar->MakeVisible(ETrue);
		AddToStackL(iToolBar);
		}
	}


/**
   Reduce client rectangle	
 */
EXPORT_C void CTestAppUi::ReduceRect(TRect& aRect) const
	{
	if(iMenuBar)
		{
		iMenuBar->ReduceRect(aRect);
		}

	if(iToolBar)
		{
		iToolBar->ReduceRect(aRect);
		}
	}


/**
  Hide menu from screen and pass on to the parent method
 */
EXPORT_C void CTestAppUi::ProcessCommandL(TInt aCommand)
	{
 	if (iMenuBar && iMenuBar->IsVisible())
		{
		iMenuBar->StopDisplayingMenuBar();
		}
	if(aCommand != EEikCmdCanceled)
		{
		CEikAppUi::ProcessCommandL(aCommand);
		}
	}


TInt  E32Dll()
	{
	return 0;
	}
