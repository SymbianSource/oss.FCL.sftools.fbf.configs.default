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
//



/**
 @file SmoketestContactsServer.cpp
*/

#include "SmoketestContactsServer.h"
#include "TestContactsCreateDB.h"
#include "TestContactsAddEntries.h"

// __EDIT_ME__ - Substitute the name of the test server
_LIT(KServerName,"Smoketest_Contacts_Server");
// __EDIT_ME__ - Use your own server class name
CSmoketestContactsServer* CSmoketestContactsServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CSmoketestContactsServer * server = new (ELeave) CSmoketestContactsServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
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
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CSmoketestContactsServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSmoketestContactsServer::NewL());
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
        
        if (!err) { }
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


// __EDIT_ME__ - Use your own server class name
CTestStep* CSmoketestContactsServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
// __EDIT_ME__ - Create your own test steps here
	if(aStepName == _L("AddEntries"))
		testStep = new CTestContactsAddEntries();
	if(aStepName == _L("CreateDB"))
		testStep = new CTestContactsCreateDB();

	return testStep;
	}
