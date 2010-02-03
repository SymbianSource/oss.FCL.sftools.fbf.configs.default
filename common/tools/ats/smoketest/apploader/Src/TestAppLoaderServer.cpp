// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This main DLL entry point for the Smoketest_AppLoader.dll
// 
//

// USER includes
#include "TestAppLoaderServer.h"
#include "TestAppLoaderAppStart.h"
#include "TestAppLoaderKeyEvents.h"
#include "TestAppLoaderKillProcess.h"
#include "TestAppLoaderEndTask.h"
#include "TestAppLoaderTaskRunning.h"

/*@{*/
_LIT(KServerName,	"smoketest_apploader");
/*@}*/

CTestAppLoaderServer* CTestAppLoaderServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestAppLoaderServer*	server = new (ELeave) CTestAppLoaderServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->ConstructL();
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestAppLoaderServer::CTestAppLoaderServer()
	:	CTestServer()
	{
	}

void CTestAppLoaderServer::ConstructL()
	{
	}

CTestAppLoaderServer::~CTestAppLoaderServer()
	{
	}

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler*		sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestAppLoaderServer*	server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestAppLoaderServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTestAppLoaderServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep*	testStep = NULL;

	if(aStepName == _L("AppStart"))
		{
		testStep = new CTestAppLoaderAppStart();
		}
	else if(aStepName == _L("KeyEvents"))
		{
		testStep = new CTestAppLoaderKeyEvents();
		}
	else if(aStepName == _L("KillProcess"))
		{
		testStep = new CTestAppLoaderKillProcess();
		}
	else if(aStepName == _L("EndTask"))
		{
		testStep = new CTestAppLoaderEndTask();
		}
	else if(aStepName == _L("TaskRunning"))
		{
		testStep = new CTestAppLoaderTaskRunning();
		}
	return testStep;
	}
