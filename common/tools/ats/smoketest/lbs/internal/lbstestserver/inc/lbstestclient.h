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
// Client-side code for the Lbs test server
// 
//



/**
 @file
 @internalTechnology
 @test
*/

#ifndef LBSERVERCLIENT_H__
#define LBSERVERCLIENT_H__


class CLbsTestServerImpl;

/** Class used to access the LBS test server.
*/
NONSHARABLE_CLASS(CLbsTestServer) : public CBase
	{
public:
	IMPORT_C static CLbsTestServer* NewL();
	~CLbsTestServer();

public:
	IMPORT_C TInt StopLbsSystem();
	IMPORT_C TInt StartLbsSystem();
	IMPORT_C TInt ResetLbsSystem();

protected:
	CLbsTestServer();
	void ConstructL();

private:
	CLbsTestServerImpl* iImpl;
	
	TUint8 iReserved[12]; // reserved for future use (to preserve BC).
	};	
	
#endif // LBSERVERCLIENT_H__
