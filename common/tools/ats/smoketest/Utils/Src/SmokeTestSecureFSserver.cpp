// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestServer - server implementation
// 
//

#include "e32std.h"
#include "SmokeTestSecureFSserver.h"
#include <f32file.h>

TRequestStatus aStatus;
TRequestStatus* aSt = &aStatus;


inline CTestSecureFSshutdown::CTestSecureFSshutdown()
	:CTimer(-1)
	{CActiveScheduler::Add(this);}
inline void CTestSecureFSshutdown::ConstructL()
	{CTimer::ConstructL();}
inline void CTestSecureFSshutdown::Start()
	{After(KMyShutdownDelay);}

inline CTestSecureFSserver::CTestSecureFSserver()
	:CServer2(0,ESharableSessions)
	{}

inline CTestSecureFSSession::CTestSecureFSSession()
	{}
inline CTestSecureFSserver& CTestSecureFSSession::Server()
	{return *static_cast<CTestSecureFSserver*>(const_cast<CServer2*>(CSession2::Server()));}
inline TBool CTestSecureFSSession::ReceivePending() const
	{return !iReceive.IsNull();}

///////////////////////

void CTestSecureFSSession::CreateL()
//
// 2nd phase construct for sessions - called by the CServer framework
//
	{
	//RDebug::Print(_L("Server::CTestSecureFSSession::CreateL - start"));

	Server().AddSession();
	//RDebug::Print(_L("Server::CTestSecureFSSession::CreateL - end"));
	}

CTestSecureFSSession::~CTestSecureFSSession()
	{
	Server().DropSession();
	}

void CTestSecureFSSession::Send(const TDesC& aMessage)
//
// Deliver the message to the client, truncating if required
// If the write fails, panic the client, not the sender
//
	{
	if (ReceivePending())
		{
		TPtrC m(aMessage);
		if (iReceive.Int1()<aMessage.Length())
			m.Set(m.Left(iReceive.Int1()));
		TInt r=iReceive.Write(0,m);
		if (r==KErrNone)
			iReceive.Complete(KErrNone);
		else
			PanicClient(iReceive,EPanicBadDescriptor);
		}
	}

void CTestSecureFSSession::ServiceL(const RMessage2& aMessage)
//
// Handle a client request.
// Leaving is handled by CMyServer::ServiceError() which reports
// the error code to the client
//
	{
	TInt result = KErrNone;

	switch (aMessage.Function())
		{
	case ETestIpcSetHomeTime:
		DoSetHomeTime(aMessage);
		break;

	case ETestIpcDeleteFile:
		result = DoDeleteFileL(aMessage);
		break;
	
	case ETestIpcKillProcess:
		DoKillProcess(aMessage);
		break;

	case ETestIpcChangeLocale:
		result = DoChangeLocale(aMessage);
		break;
		
	case ETestIpcCheckForFile:
	    result = DoCheckForFile(aMessage);
	    break;
	    
	case ETestIpcCopyFile:
	    result = DoCopyFile(aMessage);
	    break; 
	    
	case ETestIpcSetUpFbs:
	    DoSetUpFbs(); 
	    break;
	     
	default:
		PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	aMessage.Complete(result);
	}

////////////////////////////////////////

void CTestSecureFSSession::DoSetHomeTime(const RMessage2& aMessage)
	{
	TPckgBuf<TTime> pckg;
	aMessage.Read(0,pckg);
	User::SetHomeTime(pckg());
	}

TInt CTestSecureFSSession::DoDeleteFileL(const RMessage2& aMessage)
	{
	RFs fs;
	TFileName* const fileName=new(ELeave) TFileName;
	CleanupStack::PushL(fileName);
	User::LeaveIfError(fs.Connect());

	CleanupClosePushL(fs);
	aMessage.ReadL(0,*fileName);
	TInt err=fs.Delete(*fileName);
	if (!(err==KErrNone || err==KErrNotFound || err==KErrPathNotFound)) // ignore not found error
		{
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(&fs);
	CleanupStack::PopAndDestroy(fileName);
	return err;
	}

void CTestSecureFSSession::DoKillProcess(const RMessage2& aMessage)
	{
	TFullName* name = new(ELeave) TFullName;
	CleanupStack::PushL(name);

	aMessage.ReadL(0,*name);
	TFindProcess* finder = new (ELeave) TFindProcess(*name);
	CleanupStack::PushL(finder);
	RProcess* process = new (ELeave) RProcess;
	CleanupStack::PushL(process);

	if (finder->Next(*name) == KErrNone)
		{
		if (process->Open(*name) == KErrNone)
			{
			process->Kill(KErrNone);
			}
		}

	if (process != NULL)
		{
		process->Close();
		}
	CleanupStack::PopAndDestroy(process);
	CleanupStack::PopAndDestroy(finder);
	CleanupStack::PopAndDestroy(name);		
	}


TInt CTestSecureFSSession::DoChangeLocale(const RMessage2& aMessage)
	{
	TBuf<50> locname;
	TInt result;
	aMessage.ReadL(0,locname);

	result = UserSvr::ChangeLocale(locname);

	return result;
	}

TInt CTestSecureFSSession::DoCheckForFile(const RMessage2& aMessage)
{
    RFs fs;
    TInt err;
	TFileName* const fileName=new(ELeave) TFileName;
	CleanupStack::PushL(fileName);
	User::LeaveIfError(fs.Connect());

	CleanupClosePushL(fs);
	aMessage.ReadL(0,*fileName);
	
	TUint attribs;
	err=fs.Att(*fileName, attribs);
	fs.Close();
		
	CleanupStack::PopAndDestroy(&fs);
	CleanupStack::PopAndDestroy(fileName);
	return err;
	
}

TInt CTestSecureFSSession::DoCopyFile(const RMessage2& aMessage)
{

    RFs fs;
    TInt result;
 
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    TFileName fileSource;
    TFileName fileDest;

    aMessage.ReadL(0,fileSource);
    aMessage.ReadL(1,fileDest);
    	   
	CFileMan* fileMan = NULL;
	
	fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL( fileMan );
	
	fs.MkDirAll(fileDest);
	result = fileMan->Copy( fileSource, fileDest, CFileMan::EOverWrite );
	
	User::LeaveIfError(fs.Connect());
	fs.SetAtt(fileDest,KEntryAttNormal,KEntryAttReadOnly);
	fs.Close();

    CleanupStack::PopAndDestroy(fileMan);
    CleanupStack::PopAndDestroy(&fs);
	return result;
	
}

void CTestSecureFSSession::DoSetUpFbs()
{
//	FbsStartup();
//    User::LeaveIfError( RFbsSession::Connect() );
//   CleanupStack::PushL( TCleanupItem( TCleanupOperation(&RFbsSession::Disconnect ), NULL ) );
}

////////////////////////////////////////

void CTestSecureFSSession::ServiceError(const RMessage2& aMessage,TInt aError)
//
// Handle an error from CMySession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise use the default handling (report the error to the client)
//
	{
	if (aError==KErrBadDescriptor)
		PanicClient(aMessage,EPanicBadDescriptor);
	CSession2::ServiceError(aMessage,aError);
	}

void CTestSecureFSshutdown::RunL()
//
// Initiate server exit when the timer expires
//
	{
	CActiveScheduler::Stop();
	}

CServer2* CTestSecureFSserver::NewLC()
	{
	CTestSecureFSserver* self=new(ELeave) CTestSecureFSserver;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CTestSecureFSserver::ConstructL()
//
// 2nd phase construction - ensure the timer and server objects are running
//
	{
	StartL(KTestServerName);
	iShutdown.ConstructL();
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown.Start();
	}


CSession2* CTestSecureFSserver::NewSessionL(const TVersion&,const RMessage2&) const
//
// Cretae a new client session. This should really check the version number.
//
	{
	return new(ELeave) CTestSecureFSSession();
	}

void CTestSecureFSserver::AddSession()
//
// A new session is being created
// Cancel the shutdown timer if it was running
//
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

void CTestSecureFSserver::DropSession()
//
// A session is being destroyed
// Start the shutdown timer if it is the last session.
//
	{
	if (--iSessionCount==0)
		iShutdown.Start();
	}

void CTestSecureFSserver::Send(const TDesC& aMessage)
//
// Pass on the signal to all clients
//
	{
	iSessionIter.SetToFirst();
	CSession2* s;
	while ((s=iSessionIter++)!=0)
		static_cast<CTestSecureFSSession*>(s)->Send(aMessage);
	}

void PanicClient(const RMessage2& aMessage,TTestPanic aPanic)
//
// RMessage::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
//
	{
	_LIT(KPanic,"TestServer");
 	aMessage.Panic(KPanic,aPanic);
	}



/**
Perform all server initialisation, in particular creation of the
scheduler and server and then run the scheduler
*/

static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
  	User::LeaveIfError(RThread::RenameMe(KTestServerName));
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	CTestSecureFSserver::NewLC(); // create the server (leave it on the cleanup stack)
	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2); 	// Cleanup the server and scheduler
	}




TInt E32Main()
//
// Server process entry-point
//
	{
	__UHEAP_MARK;
	//
	RDebug::Print(_L("Server::E32Main Version 0.0.0"));
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}
