// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <eikstart.h> 
#include <eikapp.h>
#include <eikenv.h>
#include <apgtask.h>
#include "EndTaskTestApp.h"
#include "EndTaskTestAppExternalInterface.h"

/**
Standard DLL entry point function.
Creates and returns an instance of the CApaApplication-derived class.

@return an instance of the CApaApplication-derived class
*/
TInt E32Main()
	{
	return EikStart::RunApplication( CEndTaskTestApplication::NewApplication );
	}

/**
Default constructor
*/
CEndTaskTestApplication::CEndTaskTestApplication()
	{
	}

/**
Virtual destructor
*/	
CEndTaskTestApplication::~CEndTaskTestApplication()
	{
	}
		
/**
@return The application's UID 
*/
TUid CEndTaskTestApplication::AppDllUid() const
	{
	const TUid dll = {KUidEndTaskAppUid};
	return dll;
	}

/**
@return CEndTaskTestApplication or NULL if KErrNoMemory
*/
CApaApplication* CEndTaskTestApplication::NewApplication()
	{
	// As the framework has at this point not started up enough, and therefore the TRAP-harness and 
	// exception handlers aren’t available yet, this factory function is a non-leaving function and 
	// can't use the new(Eleave) operator.
	return new CEndTaskTestApplication();
	}
	
/**
Called by the UI framework at application start-up to create an instance of the document class.

@leave	KErrNoMemory
@return A CExampleDocument
*/
CApaDocument* CEndTaskTestApplication::CreateDocumentL()
	{
	return CEndTaskTestDocument::NewL(*this);
	}

/**
Constructor
*/
CEndTaskTestDocument::CEndTaskTestDocument(CEikApplication& aApp) : CEikDocument(aApp)
	{
	}

/**
Virtual destructor
*/
CEndTaskTestDocument::~CEndTaskTestDocument()
	{
	}

/**
Factory function for this class
@return a new CEndTaskTestDocument instance.
*/
CEndTaskTestDocument* CEndTaskTestDocument::NewL(CEikApplication& aApp)
	{
	return new(ELeave) CEndTaskTestDocument(aApp);
	}


/**
Called by the UI framework to construct the application UI class. 
Note that the app UI's ConstructL() is called by the UI framework.
*/
CEikAppUi* CEndTaskTestDocument::CreateAppUiL()
	{
	return new(ELeave) CEndTaskTestAppUi();
	}

/**
Constructor
*/
CEndTaskTestAppUi::CEndTaskTestAppUi()
	{
	}

/**
Virtual destructor
*/
CEndTaskTestAppUi::~CEndTaskTestAppUi()
	{
	}

/**
Second phase constructor of the application UI class.
It creates and owns a single view. The view contents are not created 
until the first time view is activated.
*/
void CEndTaskTestAppUi::ConstructL()
	{
	// Complete the UI framework's construction of the App UI.
	BaseConstructL(CEikAppUi::ENoAppResourceFile);
	
	// Set higher prio to preempt test-server as soon as we have an event
	User::SetPriorityControl(EFalse);
	RProcess p;
	p.SetPriority(EPriorityHigh);
	}

/**
Events published in externalinterface.h and utilized by T_EndTaskStep.cpp
*/
void CEndTaskTestAppUi::HandleApplicationSpecificEventL(TInt aType,const TWsEvent& /*aEvent*/)
	{
	switch (aType)
		{
		case EEndTaskTestAppSetSystem:
			static_cast<CEikonEnv*>(iCoeEnv)->SetSystem(ETrue);
			break;
		case EEndTaskTestAppSetNormal:
			static_cast<CEikonEnv*>(iCoeEnv)->SetSystem(EFalse);
			break;
		case ECallEndTaskWithoutPwrMgmt:
			{
			TApaTaskList taskList(iEikonEnv->WsSession());
			TApaTask task = taskList.FindApp(Application()->AppDllUid());
			task.EndTask();
			break;
			}
		case ESimulateHackerAttack1:
			{
			TWsEvent event;
			TApaTaskList taskList(iEikonEnv->WsSession());
			TApaTask task = taskList.FindApp(Application()->AppDllUid());
			// First variant should be sorted by CCoeAppUi::HandleWsEventL
			event.SetType(EEventUser);
			*(TApaSystemEvent*)(event.EventData())=EApaSystemEventSecureShutdown;	
			iEikonEnv->WsSession().SendEventToWindowGroup(task.WgId(), event);
			break;
			}
		case ESimulateHackerAttack2:
			{
			TWsEvent event;
			TApaTaskList taskList(iEikonEnv->WsSession());
			TApaTask task = taskList.FindApp(Application()->AppDllUid());
			// Second variant should be handled by wserv's Client::CommandL case EWsClOpSendEventToWindowGroup
			event.SetType(EEventPowerMgmt);
			*(TApaSystemEvent*)(event.EventData())=EApaSystemEventSecureShutdown;
			RWsSession wsSession;
			wsSession.Connect();
			wsSession.SendEventToWindowGroup(task.WgId(), event);
			wsSession.Close();
			break;
			}
		case ESimulateHackerAttack3:
			{
			TWsEvent event;
			// Third variant should be handled by wserv's Client::CommandL case EWsClOpSendEventToAllWindowGroup
			event.SetType(EEventPowerMgmt);
			*(TApaSystemEvent*)(event.EventData())=EApaSystemEventSecureShutdown;
			RWsSession wsSession;
			wsSession.Connect();
			wsSession.SendEventToAllWindowGroups(event);
			wsSession.Close();
			break;
			}
		default:
			break;
		}
	}
	
void CEndTaskTestAppUi::HandleCommandL(TInt aCommand)	
	{
	switch(aCommand)
		{
		case EEikCmdExit:
			Exit();
			break;
		default:
			break;
		}
	}
