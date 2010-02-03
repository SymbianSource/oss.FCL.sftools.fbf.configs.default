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

#ifndef __APUNINSTALLMONITOR_H__
#define __APUNINSTALLMONITOR_H__

#include <e32base.h>
#include <e32property.h>
#include <swi/swispubsubdefs.h>
#include <apsserv.h>


class CApaAppListServer;

NONSHARABLE_CLASS(CApaAppUnInstallMonitor) : public CActive
/** 
CApaAppUnInstallMonitor

A low priority (EPriorityLow) active object which monitors software
install for installations/uninstallation of applications.

@internalComponent
*/
	{
public:
	IMPORT_C static CApaAppUnInstallMonitor* NewL(CApaAppListServer*);
	IMPORT_C ~CApaAppUnInstallMonitor();
	IMPORT_C void Start();
private:
	CApaAppUnInstallMonitor(CApaAppListServer*);
	void ConstructL();
	void RunL();
	void RunError();
	void DoStart();
	void DoCancel();
	TBool UnInstallInProgress() const;
private:
	CApaAppListServer* iApaServer;
	RProperty iSwisProperty;
	TInt iSwisState;
	TBool iWaitingForStartUninstall;
	};
	
#endif
