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
// Header for the implementation of the client-side code 
// for the Lbs test server.
// 
//

#include "rsecuresessionbase.h"
#include "tserverstartparams.h"


/* Session class that derives from the server framework session.
*/
NONSHARABLE_CLASS(RLbsTestSession) : public RSecureSessionBase
	{
public:
	TVersion Version();	

	TInt StopLbsSystem();
	TInt StartLbsSystem();
	TInt ResetLbsSystem();
	};

/* Implementation of the client code.
*/
NONSHARABLE_CLASS(CLbsTestServerImpl) : public CBase
	{
public:
	static CLbsTestServerImpl* NewL();
	~CLbsTestServerImpl();
	
	TInt StopLbsSystem();
	TInt StartLbsSystem();
	TInt ResetLbsSystem();
	
protected:
	CLbsTestServerImpl();

private:
	void ConstructL();

private:
	RLbsTestSession iSession;
	TServerStartParams 	iParams; // MUST live for as long as the client
	};
