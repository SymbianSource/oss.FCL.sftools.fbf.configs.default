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
// serverside.cpp
// server side exmaple of how to use the abstract server framework
// 
//

#include <e32debug.h>
#include "lbstestsession.h"
#include "lbstestmessageenums.h"
#include "lbssystemcontroller.h"
#include "tprocessstartparams.h"
#include "cprocesslaunch.h"


/* Called by the CServer2 code to complete construction of the session 
*/
void CLbsTestSession::CreateL()
	{
	CSecureSessionBase::CreateL(); // MUST do this
	}
	
/* Service new client requests.
*/
void CLbsTestSession::DispatchL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case ELbsTestStopLbsSystem:
			{
			StopLbsSystem(aMessage);
			break;
			}
		case ELbsTestStartLbsSystem:
			{
			StartLbsSystem(aMessage);
			break;
			}
		case ELbsTestResetLbsSystem:
			{
			ResetLbsSystem(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			break;
			}
		}
	}

/* Handle leaves from DispatchL
*/
void CLbsTestSession::DispatchError(const RMessage2& /*aMessage*/, const TInt /*aError*/)
	{
	// Do nothing; leave error handling to the default behaviour
	}

/* Force the LBS system to stop.

THIS FUNCTION SHOULD ONLY EVER BE USED FOR TEST PURPOSES.

This function will tell LbsRoot to stop the LBS system immediately.
It will not check to see if there are any active requests still
being processed.
*/
void CLbsTestSession::StopLbsSystem(const RMessage2& aMessage)
	{
	TInt err(KErrUnknown);
	RLbsSystemController systemController;
	
	// Open the system controller interface
	RProcess process;
	systemController.OpenL(process.SecureId());
	CleanupClosePushL(systemController);
	
	// Logon to the root process to detect when it has exited
	if (FindRootProcess(process))
		{
		TRequestStatus status;
		process.Logon(status);
		
		// Force a complete close down of LBS
		switch (systemController.RequestCompleteCloseDown())
			{
			case ELbsCloseDownRejected:
				{
				err = KErrInUse;
				process.LogonCancel(status);
				break;
				}
			case ELbsCloseDownComplete:
				{
				err = KErrNone;
				break;
				}
			case ELbsCloseDownRootNotFound:
				{
				err = KErrDied;
				process.LogonCancel(status);
				break;
				}
			}
		
		User::WaitForRequest(status);
		process.Close();
		}

	/* Wait until the LBS Root process has definitely gone down */
	while(FindRootProcess(process))
	/* do nothing ... */;
	
	CleanupStack::PopAndDestroy(&systemController);
	aMessage.Complete(err);
	}

/* Start the LBS system

THIS FUNCTION SHOULD ONLY EVER BE USED FOR TEST PURPOSES.

If LbsRoot is not running then this function will start it.
If LbsRoot is already running then this function will request
it to start the system.
*/
void CLbsTestSession::StartLbsSystem(const RMessage2& aMessage)
	{
	TProcessStartParams params;
	_LIT(KLbsRootFileName, "\\sys\\bin\\lbsroot.exe");
	_LIT(KLbsRootProcessName, "lbsroot.exe");
	params.SetProcessFileName(KLbsRootFileName);
	params.SetProcessName(KLbsRootProcessName);
	params.SetRendezvousRequired(ETrue);
	TInt err = CProcessLaunch::ProcessLaunch(params);

	aMessage.Complete(err);
	}

/* Reset the internal state of the LBS system

THIS FUNCTION SHOULD ONLY EVER BE USED FOR TEST PURPOSES.

This function tells the LBS system to reset its internal state
and caches. Currently this means that:
- The assistance data cache between lbsnetgateway and lbsagpsmanager is cleared.
*/
void CLbsTestSession::ResetLbsSystem(const RMessage2& aMessage)
	{
	RLbsSystemController systemController;
	
	// Open the system controller interface
	RProcess process;
	systemController.OpenL(process.SecureId());
	CleanupClosePushL(systemController);
	
	// request a reset of the LBS system
	systemController.RequestSystemReset();
	
	CleanupStack::PopAndDestroy(&systemController);
	aMessage.Complete(KErrNone);
	}

TBool CLbsTestSession::FindRootProcess(RProcess& aProcess)
	{
	_LIT(KLbsRootProcessName, "lbsroot.exe*");
	TInt err(KErrNotFound);
	TFullName fullName;			
	TFindProcess processFinder(KLbsRootProcessName);
	while (err = processFinder.Next(fullName), err == KErrNone)
		{
		RProcess process;
		TInt processOpenErr = process.Open(processFinder);
		if (processOpenErr == KErrNone)
			{
			TExitType exitType = process.ExitType();
			if (exitType == EExitPending)
				{
				// Found a running instance of lbsroot.exe,
				aProcess = process;
				return ETrue;
				}
			}
		process.Close();		
		}
		
	return EFalse;
	}
