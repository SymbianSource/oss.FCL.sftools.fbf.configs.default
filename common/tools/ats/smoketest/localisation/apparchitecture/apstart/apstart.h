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
//

#ifndef __APSTART_H
#define __APSTART_H

#include <apgcli.h>
#include <apastarter.h>

/**
Start applications using RApaLsSession.

@internalTechnology
@prototype
*/
NONSHARABLE_CLASS(CApaAppStart) : public CApaStarter
	{
public:	
	IMPORT_C static CApaAppStart* NewL();
	
	//from CBase			
	~CApaAppStart();
	
	//from CApaStarter
	void WaitForApparcToInitialiseL();
	void StartAppL(const TDesC& aFileName, const TDesC& aArgs, TBool aViewLess, TBool aStartInBackground, TThreadId& aThreadId);
	void StartAppL(const TDesC& aFileName, const TDesC& aArgs, TBool aViewLess, TBool aStartInBackground, TThreadId& aThreadId, TRequestStatus& aRequestStatusForRendezvous);
	
private:
	CApaAppStart();
	void ConstructL();			
	static void SetupCommandLineL(CApaCommandLine& aCmdLine, const TDesC& aFileName, const TDesC8& aArgs, TBool aViewLess, TBool aStartInBackground);
	void DoStartAppL(const TDesC &aFileName, const TDesC &aArgs, TBool aViewLess, TBool aStartInBackground, TThreadId& aThreadId, TRequestStatus* aRequestStatusForRendezvous);

private:
	RApaLsSession iApaLsSession;		
	};

#endif
