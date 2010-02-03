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
// Client side code for the LBS test server.
// 
//

#include <e32base.h>
#include <e32debug.h>
#include "lbstestclient.h"
#include "lbstestclientimpl.h"

	
///////////////////////////////////////////////////////////////////////////////
// CLbsTestServer
///////////////////////////////////////////////////////////////////////////////

CLbsTestServer::CLbsTestServer()
	{
	
	}
	
CLbsTestServer::~CLbsTestServer()
	{
	delete iImpl;
	}

EXPORT_C CLbsTestServer* CLbsTestServer::NewL()
	{
	CLbsTestServer* self = new (ELeave) CLbsTestServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLbsTestServer::ConstructL()
	{
	iImpl = CLbsTestServerImpl::NewL();
	}

EXPORT_C TInt CLbsTestServer::StopLbsSystem()
	{
	return iImpl->StopLbsSystem();
	}

EXPORT_C TInt CLbsTestServer::StartLbsSystem()
	{
	return iImpl->StartLbsSystem();
	}

EXPORT_C TInt CLbsTestServer::ResetLbsSystem()
	{
	return iImpl->ResetLbsSystem();
	}
