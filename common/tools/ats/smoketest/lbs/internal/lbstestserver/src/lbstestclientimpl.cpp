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
// Implementation of the client-side code 
// for the Lbs test server.
// 
//

#include <e32base.h>
#include "lbstestmessageenums.h"
#include "lbstestserverdata.h"
#include "lbstestclientimpl.h"


///////////////////////////////////////////////////////////////////////////////
// RLbsTestSession
///////////////////////////////////////////////////////////////////////////////

/** Get the version of this object.
*/
TVersion RLbsTestSession::Version()
	{
	return TVersion(KTestServerMajorVersionNumber,
					KTestServerMinorVersionNumber,
					KTestServerBuildVersionNumber);
	}

/** Force the LBS system to stop and shutdown.

THIS FUNCTION SHOULD ONLY BE USED IN TEST CODE!

This function will stop the LBS system immediately, even
if it is currently processing a location request.
*/
TInt RLbsTestSession::StopLbsSystem()
	{
	return SendReceive(ELbsTestStopLbsSystem);
	}

TInt RLbsTestSession::StartLbsSystem()
	{
	return SendReceive(ELbsTestStartLbsSystem);
	}

TInt RLbsTestSession::ResetLbsSystem()
	{
	return SendReceive(ELbsTestResetLbsSystem);
	}

///////////////////////////////////////////////////////////////////////////////
// CLbsTestServerImpl
///////////////////////////////////////////////////////////////////////////////

CLbsTestServerImpl::~CLbsTestServerImpl()
	{
	iSession.Close();
	}
	
CLbsTestServerImpl::CLbsTestServerImpl()
	{
	}

CLbsTestServerImpl* CLbsTestServerImpl::NewL()
	{
	CLbsTestServerImpl* self = new (ELeave) CLbsTestServerImpl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CLbsTestServerImpl::ConstructL()
	{
	/**
	 * See the ServerFramework code for the meaning of these params
	 */
	iParams.SetServerFileName(KTestServerFileName);
	iParams.SetServerName(KTestServerName);
	iParams.SetNumberOfServerSlots(KServerFrameworkDefaultMessageSlots);
	iParams.SetServerUids(KNullUid, KNullUid, KLbsTestServerUid);
	
	User::LeaveIfError(iSession.Connect(iParams));	
	}

TInt CLbsTestServerImpl::StopLbsSystem()
	{
	return iSession.StopLbsSystem();
	}

TInt CLbsTestServerImpl::StartLbsSystem()
	{
	return iSession.StartLbsSystem();
	}

TInt CLbsTestServerImpl::ResetLbsSystem()
	{
	return iSession.ResetLbsSystem();
	}
