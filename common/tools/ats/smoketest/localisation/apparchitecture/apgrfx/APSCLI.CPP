// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client access to the AppArc server
// 
//

#include <e32base.h>
#include <e32svr.h>
#include <e32math.h>
#include <apasvst.h>
#include <apsserv.h>
#include <e32uid.h>

_LIT(KNameApaImage,"APSEXE");
const TUid KServerUid3  = { 0x10003A3F };
const TInt KApaServThreadMaxHeapSize = 0x100000; // 1Mb
const TInt KApaServThreadStackSize = 0x3000; // Apparc server need extra stack as it has a lot of filenames and parse objects on stack from time to time

EXPORT_C TInt StartupApaServer(MApaAppStarter& aAppStarter)
/**
Start the server in a thread, using the supplied app starter.

@internalTechnology
*/
	{
	RThread server;
	TInt r=server.Create(NameApaServServerThread(),ApaServThreadStart,KApaServThreadStackSize,KMinHeapSize,KApaServThreadMaxHeapSize,&aAppStarter,EOwnerThread);
	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	server.Close();
	return stat.Int();
	}

EXPORT_C TInt StartupApaServerProcess()
/**
StartupApaServerProcess
Start the server in a process, (or thread in EKA1 WINS). Simultaneous launching
of two such processes should be detected when the second one attempts to
create the server object, failing with KErrAlreadyExists.

@publishedPartner 
*/
	{
	const TUidType uidType(KNullUid, KNullUid, KServerUid3);
	TInt r=KErrNone;
	
#ifdef APA_SERVER_IN_THREAD
	// In WINS the server is a DLL, the exported entrypoint returns a TInt
	// which represents the real entry-point for the server thread
	RLibrary lib;
	r=lib.Load(KNameApaImage,uidType);
	if (r!=KErrNone)
		return r;
	TLibraryFunction ordinal1=lib.Lookup(1);
	TThreadFunction serverFunc=reinterpret_cast<TThreadFunction>(ordinal1());
	// To deal with the unique thread (+semaphore!) naming in EPOC, and that we may
	// be trying to restart a server that has just exited we attempt to create a
	// unique thread name for the server.
	// This uses Math::Random() to generate a 32-bit random number for the name
	TName name(NameApaServServerThread());
	name.AppendNum(Math::Random(),EHex);
	RThread server;
	r=server.Create(name, serverFunc, KApaServThreadStackSize, NULL, &lib, NULL, KMinHeapSize,KApaServThreadMaxHeapSize, EOwnerProcess);
	lib.Close();
#else
	// On ARM or any EKA2 target, the server is an EXE.
	// Create a process from it.
	RProcess server;
	r=server.Create(KNameApaImage,KNullDesC,uidType);
#endif
	
	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	server.Close();
	return stat.Int();
	}
