// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// server side exmaple of how to use the abstract server framework
// 
//

#include <e32debug.h>
#include "lbstestserver.h"
#include "lbstestserverdata.h"
#include "lbstestmessageenums.h"
#include "lbstestsession.h"


///////////////////////////////////////////////////////////////////////////////
// Security Policy information
///////////////////////////////////////////////////////////////////////////////
const TUint myRangeCount = 4;
const TInt myRanges[myRangeCount] = 
    {
    0, //range is 0-1 inclusive
    ESecureSubSessionBaseClose, //range is ESecureSubSessionBaseClose to (EFirstLbsTestServerMessage - 1)
    EFirstLbsTestServerMessage, //range is EFirstLbsTestServerMessage to (ELastLbsTestServerMessage - 1)
    ELastLbsTestServerMessage // range is ELastLbsTestServerMessage to KMaxTInt inclusive
    };
    
const TUint8 myElementsIndex[myRangeCount] = 
    {
    CPolicyServer::EAlwaysPass,
    CPolicyServer::ENotSupported,
    0,
    CPolicyServer::ENotSupported
    };
    
const CPolicyServer::TPolicyElement myElements[] = 
    {
    {_INIT_SECURITY_POLICY_C1(ECapabilityLocation), CPolicyServer::EFailClient}
    };
    
const CPolicyServer::TPolicy myPolicy =
    {
    CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
    myRangeCount,                   
    myRanges,
    myElementsIndex,
    myElements,
    };

///////////////////////////////////////////////////////////////////////////////
// Implementation of Server Framework functions
///////////////////////////////////////////////////////////////////////////////

MCreateServerImpl* CSecureASBase::GetImplLC()
	{
	CLbsTestServerImpl* impl = new(ELeave) CLbsTestServerImpl();
	CleanupStack::PushL(impl);
	return impl;
	}

void CLbsTestServerImpl::CreateServerLC(TServerStartParams& aParams)
	{
	CLbsTestServer* s = new(ELeave) CLbsTestServer(CActive::EPriorityStandard, myPolicy);
	CleanupStack::PushL(s);
	s->ConstructL(aParams.GetServerName());
	// leave the server object on the CS
	}
	
///////////////////////////////////////////////////////////////////////////////
// CTestServer
///////////////////////////////////////////////////////////////////////////////

CLbsTestServer::CLbsTestServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy) : 
	CSecureServerBase(aPriority, aSecurityPolicy),
	iVersion(KTestServerMajorVersionNumber,
			 KTestServerMinorVersionNumber,
			 KTestServerBuildVersionNumber)
	{
	}
	
CLbsTestServer::~CLbsTestServer()
	{
	}
	
void CLbsTestServer::ConstructL(const TDesC& aServerName)
	{
	StartL(aServerName);
	BaseConstructL(); // MUST BE CALLED	
	}
	
CSession2* CLbsTestServer::DoNewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	return new (ELeave) CLbsTestSession(); // version number already checked at this point
	}
	
	
TVersion CLbsTestServer::GetServerVersion() const
	{
	return iVersion;
	}
