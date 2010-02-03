// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestServer - server classes
// 
//

#ifndef __SMOKETEST_SECURE_FS_SERVER_H__
#define __SMOKETEST_SECURE_FS_SERVER_H__

#include <e32base.h>
#include "SmokeTestSecureFSclientserver.h"

enum TTestPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving
	};

void PanicClient(const RMessage2& aMessage,TTestPanic TMyPanic);

class CTestSecureFSshutdown : public CTimer
	{
	enum {KMyShutdownDelay=0x500000};	// approx 5s
public:
	inline CTestSecureFSshutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};

class CTestSecureFSserver : public CServer2
	{
public:
	static CServer2* NewLC();
	void AddSession();
	void DropSession();
	void Send(const TDesC& aMessage);
private:
	CTestSecureFSserver();
	void ConstructL();
	// CSharableSession* NewSessionL(const TVersion& aVersion, const RMessage& aMessage) const;
        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	TInt iSessionCount;
	CTestSecureFSshutdown iShutdown;
	};

//class CTestSecureFSSession : public CSharableSession
class CTestSecureFSSession : public CSession2
	{
public:
	CTestSecureFSSession();
	void CreateL();
	void Send(const TDesC& aMessage);
private:
	~CTestSecureFSSession();
	inline CTestSecureFSserver& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
	inline TBool ReceivePending() const;
private:
	void DoSetHomeTime(const RMessage2& aMessage);
	
	// General File Manipulation
	TInt DoDeleteFileL(const RMessage2& aMessage);

	// Power User
	void DoKillProcess(const RMessage2& aMessage);
	TInt DoChangeLocale(const RMessage2& aMessage);
	TInt DoCheckForFile(const RMessage2& aMessage);
	TInt DoCopyFile(const RMessage2& aMessage);
	void DoSetUpFbs();

private:
	RMessage2 iReceive;
	};

#endif // __SMOKETEST_SECURE_FS_SERVER_H__
