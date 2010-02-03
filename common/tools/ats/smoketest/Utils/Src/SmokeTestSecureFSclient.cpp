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
// Test Server - client interface implementation
// 
//

#include <e32std.h>
#include <e32math.h>
#include "SmokeTestSecureFSclientserver.h"
#include "SmokeTestSecureFSclient.h"

static TInt StartServer()
//
// Start the server process. Simultaneous launching
// of two such processes should be detected when the second one attempts to
// create the server object, failing with KErrAlreadyExists.
//
	{
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);


	RProcess server;
	TInt r=server.Create(KTestServerImg, KNullDesC, serverUid);

	if (r!=KErrNone)
		return r;
	TRequestStatus stat;

	server.Rendezvous(stat);
	
	if (stat != KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server	
	User::WaitForRequest(stat);	//wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

EXPORT_C TInt RTestSecureFSclient::Connect()
	{// Connect to the server, attempting to start it if necessary
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KTestServerName,TVersion(0,0,0),1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		User::After(1000000); //Let server load up (in EKA2)

		}
	}

EXPORT_C TInt RTestSecureFSclient::SetHomeTime(const TTime& aTime) 
	{
	TPckg<TTime> pckg(aTime);
	return SendReceive(ETestIpcSetHomeTime,TIpcArgs(&pckg));
	}

EXPORT_C TInt RTestSecureFSclient::DeleteFileL(const TDesC& aFileName)
	{
	TIpcArgs args(&aFileName);
	return SendReceive(ETestIpcDeleteFile,args);
	}

EXPORT_C void RTestSecureFSclient::KillProcessL(const TDesC& aProcessName)
	{
	TIpcArgs args(&aProcessName);
	User::LeaveIfError(SendReceive(ETestIpcKillProcess,args));
	}

EXPORT_C TInt RTestSecureFSclient::ChangeLocaleName(const TDesC& aLocaleDllName)
	{
	TIpcArgs args(&aLocaleDllName);
	return SendReceive(ETestIpcChangeLocale,args);
	}

EXPORT_C TInt RTestSecureFSclient::CheckForFile(const TDesC& aFileName)
    {
	TIpcArgs args(&aFileName); 
	return SendReceive(ETestIpcCheckForFile,args);  
    }
    
EXPORT_C TInt RTestSecureFSclient::CopyFile(const TDesC& Source, const TDesC& Dest)
    {
	TIpcArgs args(&Source, &Dest); 
	return SendReceive(ETestIpcCopyFile,args);  
    }
    
EXPORT_C void RTestSecureFSclient::SetUpFbs()
    {
	SendReceive(ETestIpcSetUpFbs);
    }
