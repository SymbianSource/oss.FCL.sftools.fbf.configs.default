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
// The main startup of the AppArc server
// 
//

#include <apsserv.h>
#include <fbs.h>

#include "APASVST.H"
#include "APAFLREC.H"

#include <eikdll.h>

NONSHARABLE_CLASS(CSvActiveScheduler) : public CActiveScheduler
	{
public:
	static void NewLC();
	virtual void Error(TInt anError) const;
	};

GLDEF_C void CSvActiveScheduler::NewLC()
//
// Create and install the active scheduler.
//
	{
	CSvActiveScheduler* pA=new(ELeave) CSvActiveScheduler;
	CleanupStack::PushL(pA);
	CActiveScheduler::Install(pA);
	}

GLDEF_C void CSvActiveScheduler::Error(TInt) const
//
// Called if any Run() method leaves.
//
	{
	}

static void CleanupRFbsSession(TAny*)
	{
	RFbsSession::Disconnect();
	}

static void RunServerL(MApaAppStarter* aAppStarter)
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	MApaAppStarter* appStarter = aAppStarter;
	// create and install the active scheduler we need
	CSvActiveScheduler::NewLC();
	// create a RFbsSession
	User::LeaveIfError(RFbsSession::Connect());
	CleanupStack::PushL(TCleanupItem(CleanupRFbsSession, NULL));
	//
	// create the server (leave it on the cleanup stack)
	CApaAppListServer* appListServer = CApaAppListServer::NewL(appStarter);
	CleanupStack::PushL(appListServer);
	//
	// Initialisation complete, now signal the client
#ifdef APA_SERVER_IN_THREAD
	RThread::Rendezvous(KErrNone);
#else
	if(aAppStarter)
		{
		// Launching in a thread within an existing process.
		RThread::Rendezvous(KErrNone);
		}
	else
		{
		RProcess::Rendezvous(KErrNone);
		}
#endif
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server, RFbsSession and scheduler
	CleanupStack::PopAndDestroy(3);
	}

static TInt RunServer(MApaAppStarter* aAppStarter)
//
// Main entry-point for the server thread
//
	{
	__UHEAP_MARK;
	TInt r;
	// naming the server thread after the server helps to debug panics
	r=RThread::RenameMe(NameApaServServerThread());
	//
	if (r == KErrNone)
		{
		CTrapCleanup* cleanup=CTrapCleanup::New();
		r=KErrNoMemory;
		if (cleanup)
			{
			TRAP(r,RunServerL(aAppStarter));
			REComSession::FinalClose();
			delete cleanup;
			}
		}
	//
	__UHEAP_MARKEND;
	return r;
	}

/**
ApaServThreadStart

@internalTechnology
*/
EXPORT_C TInt ApaServThreadStart(TAny* aAppStarter)
//
// thread entry-point function.
//
	{
	MApaAppStarter* appStarter = reinterpret_cast<MApaAppStarter*>(aAppStarter);
	return RunServer(appStarter);
	}
