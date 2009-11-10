// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// serverside.h
// server side exmaple of how to use the abstract server framework
// 
//

#ifndef LBSTESTSERVER_H__
#define LBSTESTSERVER_H__

#include "tserverstartparams.h"
#include "csecureasbase.h"
#include "csecureserverbase.h"


/* 
*/
class CLbsTestServerImpl : public CBase, public MCreateServerImpl
	{
public:
	virtual void CreateServerLC(TServerStartParams& aParams);	
	};
	
/* Main server class
*/
class CLbsTestServer : public CSecureServerBase
	{
public:
	CLbsTestServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy);
	~CLbsTestServer();
	void ConstructL(const TDesC& aServerName);
	CSession2* DoNewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	TVersion GetServerVersion() const;
protected:
	// property
	TVersion iVersion;
	};
	
#endif //LBSTESTSERVER_H__
