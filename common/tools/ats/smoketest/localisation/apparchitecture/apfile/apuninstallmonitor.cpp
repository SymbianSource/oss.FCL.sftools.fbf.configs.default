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
//

#include "apuninstallmonitor.h"

// This active object has lower priority than CApaAppListServer but higher than file scanning periodic timer.
const TInt KAppUninstallPriority=CActive::EPriorityStandard-10; 

//
// CApaAppUnInstallMonitor class
//
EXPORT_C CApaAppUnInstallMonitor* CApaAppUnInstallMonitor::NewL(CApaAppListServer* aApaServer)
	{
	CApaAppUnInstallMonitor* self = new(ELeave) CApaAppUnInstallMonitor(aApaServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CApaAppUnInstallMonitor::CApaAppUnInstallMonitor(CApaAppListServer* aApaServer) :
		CActive(KAppUninstallPriority),
		iApaServer(aApaServer),
		iWaitingForStartUninstall(ETrue)
	{
	CActiveScheduler::Add(this);
	}

void CApaAppUnInstallMonitor::ConstructL()
	{
	User::LeaveIfError(iSwisProperty.Attach(KUidSystemCategory, Swi::KUidSoftwareInstallKey));
	}

EXPORT_C CApaAppUnInstallMonitor::~CApaAppUnInstallMonitor()
	{
	Cancel();
	iSwisProperty.Close();
	}

EXPORT_C void CApaAppUnInstallMonitor::Start()
	{
	DoStart();
	}

TBool CApaAppUnInstallMonitor::UnInstallInProgress() const
	{
	return ((iSwisState & Swi::KSwisOperationMask) & Swi::ESwisUninstall)
			&& ((iSwisState & Swi::KSwisOperationStatusMask) == Swi::ESwisStatusNone);
	}

void CApaAppUnInstallMonitor::RunL()
	{
	if(iStatus.Int() == KErrNone)
		{
		DoStart();
		
		if (iWaitingForStartUninstall)
			{
			if (UnInstallInProgress())
				{
				iWaitingForStartUninstall = EFalse;
				iApaServer->HandleStartUninstallEvent();
				}
			}
		else
			{ // waiting for end uninstall
			if (!UnInstallInProgress())
				{
				iWaitingForStartUninstall = ETrue;
				TRAP_IGNORE(iApaServer->HandleEndUninstallEventL()); // let user try again if OOM
				}
			
			}
		}
	}
	
void CApaAppUnInstallMonitor::DoStart()
	{
	iSwisProperty.Subscribe(iStatus);
	TInt err = iSwisProperty.Get(KUidSystemCategory, Swi::KUidSoftwareInstallKey, iSwisState);
	if(err == KErrNone)
		{
		SetActive();
		}
	else
		{
		iSwisProperty.Cancel();
		}
	}

void CApaAppUnInstallMonitor::DoCancel()
	{
	iSwisProperty.Cancel();
	}
	

// End of file
