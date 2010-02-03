// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "consolealarmalertserver.h"

// System includes

// User includes
#include "asaltdefs.h"
//
#include "consolealarmalertsession.h"

// Type definitions

// Constants

// Enumerations

// Classes referenced

// The Console Alarm Alert Server is migrated to a secure server in EKA2
#include <e32base.h>



const TUint KRangeCount = 1; 

const TInt KOpCodeRanges[KRangeCount] = 
	{	
	0 // All Op Codes  from 0 to KMaxInt should pass. No restrictions	
	};


const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::EAlwaysPass, 	//All OP Codes Allways passing no capability required 
	};


const CPolicyServer::TPolicy KConsoleAlarmAlertServerPolicy =
	{
	CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
	KRangeCount,
	KOpCodeRanges,
	KElementsIndex, 	// what each range is compared to 
	KNullHandle			//Since we have no specific policy checking
	};
 	


///////////////////////////////////////////////////////////////////////////////////////
// ----> CConsoleAlarmAlertServer (source)
///////////////////////////////////////////////////////////////////////////////////////

//*************************************************************************************
CConsoleAlarmAlertServer::CConsoleAlarmAlertServer()
 	:CPolicyServer(CActive::EPriorityStandard, KConsoleAlarmAlertServerPolicy), iInstructionSet(NULL)
	{
	}



//*************************************************************************************
CConsoleAlarmAlertServer::~CConsoleAlarmAlertServer()
	{
	if (iAttachment)
		delete iAttachment;	
	}


//*************************************************************************************
void CConsoleAlarmAlertServer::ConstructL()
	{
	StartL(KAlarmAlertServerName);
	}


//*************************************************************************************
CConsoleAlarmAlertServer* CConsoleAlarmAlertServer::NewLC()
	{
	CConsoleAlarmAlertServer* self = new(ELeave) CConsoleAlarmAlertServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
CSession2* CConsoleAlarmAlertServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage*/) const
	{
	const TVersion KServerVersion(KASAltVersionMajor, KASAltVersionMinor, KASAltVersionBuild);
	if	(!User::QueryVersionSupported(KServerVersion, aVersion))
		User::Leave(KErrNotSupported);
	//
	return CConsoleAlarmAlertSession::NewL((CConsoleAlarmAlertServer*)this);
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
TInt CConsoleAlarmAlertServer::RunError(TInt /*Error*/)
	{
	return KErrNone;
	}

void CConsoleAlarmAlertServer::SetAttachment(HBufC8* data)
	{
	if (iAttachment)
		delete iAttachment;
	iAttachment = data;
	}
