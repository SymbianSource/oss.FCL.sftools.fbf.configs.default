// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestMessServer.h"
#include "TestMessSession.h"
#include "TestMessCleanMessageFolder.h"
#include "TestMessCreateSmsAccount.h"
//IMK #include "TestMessCreatePopAccount.h"
//IMK #include "TestMessCreateImapAccount.h"
#include "TestMessSetDefaultSc.h"
//IMK #include "TestMessCreateBIO.h"
//IMK #include "TestMessCreateEmail.h"
//IMK #include "TestMessCreateBulkEmail.h"
//IMK #include "TestMessCreateEMS.h"
#include "TestMessCreateSMS.h"
//IMK #include "TestMessCreateBulkSMS.h"
//IMK #include "TestMessEntryCount.h"
//IMK #include "TestMessSendEmail.h"
//IMK #include "TestMessSendBulkEmail.h"
//IMK #include "TestMessSendSms.h"
//IMK #include "TestMessDeleteEmailImap.h"
//IMK #include "TestMessDeleteEmailPop.h"
//IMK #include "TestMessFullSyncImap.h"
//IMK #include "TestMessReceiveEmailImap.h"
//IMK #include "TestMessReceiveEmailPop.h"
//IMK #include "TestMessOpen.h"
//IMK #include "TestMessVerifyEntry.h"
//IMK #include "TestMessReply.h"
//IMK #include "TestMessForward.h"
//IMK #include "TestMessCompareMessages.h"
//IMK #include "TestMessSetSyncRateImap.h"
//IMK #include "TestMessDeleteSharedData.h"
#include "TestMessEditEntry.h"
//IMK #include "TestMessSaveEmailAttachments.h"
//IMK #include "TestMessDeleteFolder.h"
//IMK #include "TestMessCreateFolder.h"
//IMK #include "TestMessRenameFolder.h"
//IMK #include "TestMessMoveMessages.h"
//IMK #include "TestMessSearchMessage.h"
//IMK #include "TestMessDeleteMessages.h"
//IMK #include "TestMessScheduleSendSMS.h"
//IMK #include "TestMessCancelScheduledSMS.h"
//IMK #include "TestMessVerifyScheduleSMS.h"
//IMK #include "TestMessBackupStore.h"
//IMK #include "TestMessRestoreStore.h"
//IMK #include "TestMessReceiveHeadersEmailPop.h"
//IMK #if (defined _PLATTEST_CDMA_)
//IMK #include "TestMessCreateCancelSms.h"
//IMK #endif


// EPOC includes
#include <e32base.h>
#include <es_sock.h>
#include <smsuaddr.h>
#include <TestExecuteServerUtils.h>

/**
 * @name Constant Literals used.
 */
/*@{*/
/*@}*/


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
	CActiveScheduler*	sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestMessServer*	server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestMessServer::NewL());
	if ( err==KErrNone )
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
#if (defined TRAP_IGNORE)
	TRAP_IGNORE(MainL());
#else
	TRAPD(err,MainL());
#endif
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


CTestMessServer* CTestMessServer::NewL()
	{
	CTestMessServer*	server=new(ELeave) CTestMessServer();
	CleanupStack::PushL(server);
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	server->ConstructL(serverName.Name());
	CleanupStack::Pop();
	return server;
	}

CTestMessServer::CTestMessServer()
:	CTestServer()
,	iSharedData(NULL)
	{
	}

void CTestMessServer::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);
	iSharedData=CTestMessSharedData::NewL();
	}

CTestMessServer::~CTestMessServer()
	{
	delete iSharedData;
	iSharedData=NULL;
	}

CSession2* CTestMessServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
/**
 * @param RMessage - RMessage for the session open
 * Secure version
 */
	{
	CTestSession*	session = new (ELeave) CTestMessSession(iSharedData);
	CONST_CAST(CTestMessServer*,this)->IncSessionCount();
	return session;
	}

// constructor for ESOCK test suite
// this creates all the ESOCK test steps and
// stores them inside CTestSuiteEsock
CTestStep* CTestMessServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestMessBase*	testStep = NULL;

	if(aStepName == _L("CleanMessageFolder"))
		{
		testStep = new CTestMessCleanMessageFolder();
		}
	else if(aStepName == _L("CreateSms"))
		{
		testStep = new CTestMessCreateSMS();
		}
	else if(aStepName == _L("CreateSmsAccount"))
		{
		testStep = new CTestMessCreateSmsAccount();
		}
	else if(aStepName == _L("SetSmsSc"))
		{
		testStep = new CTestMessSetDefaultSc();
		}
	return testStep;
}
