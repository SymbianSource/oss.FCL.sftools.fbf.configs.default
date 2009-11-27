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
// Startup State Aware (SSA) test component used to
// test apparc StartApp and StartDocument functionality \n
// 
//



/**
 @file
 @test
 @internalComponent  Internal Symbian test code
*/

#include <e32base.h>
#include <e32debug.h>
#include <s32file.h>
#include <startupdomaindefs.h>
#include <domainmanager.h>
#include <startup.hrh>

#include "tssaac.h"
#include "SysStartApparcTestCases.h"


CApparcFuncInSystart* CApparcFuncInSystart::NewL(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId)
	{
	RDebug::Print(_L("CApparcFuncInSystart::NewL - entry"));
	CApparcFuncInSystart* self = NewLC(aHierarchyId, aDomainId);
	CleanupStack::Pop(self);
	RDebug::Print(_L("CApparcFuncInSystart::NewL - exit"));
	return self;
	}

CApparcFuncInSystart* CApparcFuncInSystart::NewLC(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId)
	{
	RDebug::Print(_L("CApparcFuncInSystart::NewLC - entry"));
	CApparcFuncInSystart* self = new (ELeave) CApparcFuncInSystart(aHierarchyId, aDomainId);
	CleanupStack::PushL(self);
	self->ConstructL();
	RDebug::Print(_L("CApparcFuncInSystart::NewLC - exit"));
	return self;
	}

/**
  Constructor
*/
CApparcFuncInSystart::CApparcFuncInSystart(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId) :
	CDmDomain(aHierarchyId, aDomainId)
	{
	}

/**
  Destructor
*/
CApparcFuncInSystart::~CApparcFuncInSystart()
	{
	Cancel();
	}

/**
  Start - Register the requests
*/
void CApparcFuncInSystart::Start()
	{
	RDebug::Print(_L("CApparcFuncInSystart::Start - entry"));

	__ASSERT_ALWAYS(!IsActive(), User::Panic(KApparcFuncInSystartPanic,1));

	// register a request for further transition notifications
	RDebug::Print(_L("CApparcFuncInSystart: Request Transition Notification"));
	RequestTransitionNotification();
	RDebug::Print(_L("CApparcFuncInSystart: Transition Acknowlege"));
	}


/**
  DoCancel()
*/
void CApparcFuncInSystart::DoCancel()
	{
	}


/**
  RunL()
*/
void CApparcFuncInSystart::RunL()
	{
	RDebug::Print(_L("CApparcFuncInSystart::RunL - entry"));

	// Check current state
	TInt state = GetState();
	if (state != EStartupStateCriticalDynamic)
		{
		RDebug::Print(_L("CApparcFuncInSystart::RunL - Initial state check failure! Current state = %d"), state);
		User::Leave(KErrGeneral);
		}

	// Perform tests - creates a test case object used for running the apparc test cases
	CSysStartApparcTestCase* teststep = new (ELeave) CSysStartApparcTestCase();
	CleanupStack::PushL(teststep);
	teststep->doTestCasesL();
	CleanupStack::PopAndDestroy(teststep);

	// Check current state
	state = GetState();
	if (state != EStartupStateCriticalDynamic)
		{
		RDebug::Print(_L("CApparcFuncInSystart::RunL - Final state check failure! Current state = %d"), state);
		User::Leave(KErrGeneral);
		}

	// Acknowledge this state (The transition should pass)
	AcknowledgeLastState(KErrNone);

	RDebug::Print(_L("CApparcFuncInSystart::RunL - exit"));
	}



LOCAL_C void StartSystemL();

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("CApparcFuncInSystart::E32Main - entry"));
	__UHEAP_MARK; // mark heap state

	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();
	TRAPD(error, StartSystemL());
    delete TheTrapCleanup;

	__UHEAP_MARKEND; // check for no memory leak

	RDebug::Print(_L("CApparcFuncInSystart::E32Main - exit"));
	return error;
	}

LOCAL_C void StartSystemL()
	{
	RDebug::Print(_L("CApparcFuncInSystart::StartSystemL - entry"));

	//Prevent emulator closing when a panic occurs
	User::SetJustInTime(EFalse);

	CActiveScheduler* theScheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(theScheduler);
	CActiveScheduler::Install(theScheduler);
	CApparcFuncInSystart *starter = CApparcFuncInSystart::NewLC(KDmHierarchyIdStartup, KAppServicesDomain3);

	starter->Start();

	CActiveScheduler::Start();

	// This point should never be reached
	User::Panic(KApparcFuncInSystartPanic,1);

	CleanupStack::PopAndDestroy(2); // starter, theScheduler
	CleanupStack::PopAndDestroy(theScheduler);
	RDebug::Print(_L("CApparcFuncInSystart::StartSystemL - exit"));
	}
