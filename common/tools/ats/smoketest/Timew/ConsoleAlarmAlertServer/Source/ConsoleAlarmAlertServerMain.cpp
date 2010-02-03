// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "consolealarmalertservermain.h"

// System includes

// User includes
#include "asaltdefs.h"
//
#include "consolealarmalertserver.h"

// Type definitions

// Constants

// Enumerations

// Classes referenced


///////////////////////////////////////////////////////////////////////////////////////
// ----> ConsoleAlarmAlertServer (source)
///////////////////////////////////////////////////////////////////////////////////////

//*************************************************************************************
EXPORT_C void ConsoleAlarmAlertServer::StartConsoleAlarmAlertServerL(TAny* instructionSet)
	{
	_LIT(KAlarmAlertServerThreadName, "ConsoleAlarmAlertServerThread");
	//
	const TInt KStackSize		= 0x0002000;	//  8KB
	const TInt KInitHeapSize	= 0x0001000;	//  4KB
	const TInt KHeapSize		= 0x1000000;	// 16MB
	//
	// Is the alarm alert server already running? If so, don't
	// start a new one...
	TFullName name;
	TFindServer finder(KAlarmAlertServerName);
	if	(finder.Next(name) == KErrNone)
		return;
	
	RThread serverThread;
	const TInt error = serverThread.Create(KAlarmAlertServerThreadName,
										   &StartServerThreadFunction,
										   KStackSize,
										   KInitHeapSize,
										   KHeapSize,
										   instructionSet
										  );
	User::LeaveIfError(error);
	//
	serverThread.SetPriority(EPriorityMore);
	serverThread.Resume();
	//
	User::After(2000000); // 2 seconds
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
TInt ConsoleAlarmAlertServer::StartServerThreadFunction(TAny* instructionSet)
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		User::Invariant();
	//
	TRAPD(err, StartServerL(instructionSet));
	delete cleanup;
	//
	__UHEAP_MARKEND;
	return err;
	}


//*************************************************************************************
void ConsoleAlarmAlertServer::StartServerL(TAny* instructionSet)
	{
	CActiveScheduler* scheduler = new CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	//

	CConsoleAlarmAlertServer * tmp = CConsoleAlarmAlertServer::NewLC();
	tmp->AddResponseArray(instructionSet);
	CActiveScheduler::Start();
	//
	CleanupStack::PopAndDestroy(2, scheduler);
	}






///////////////////////////////////////////////////////////////////////////////////////
// ----> ConsoleAlarmAlertServer (source)
///////////////////////////////////////////////////////////////////////////////////////

//*************************************************************************************
