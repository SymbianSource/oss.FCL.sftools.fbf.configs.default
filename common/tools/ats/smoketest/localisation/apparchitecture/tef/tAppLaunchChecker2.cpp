// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Supporting ECOM plugin for use by  test programs that excercise
// rule based launching of applications.
// The class CTAppLaunchChecker implements an
// ECOM RuleBased plugin interface.
// 
//




/**
 @file 
 @internalComponent - Internal Symbian test code
*/

#include <ImplementationProxy.h>
#include "tAppLaunchChecker2.h"
#include "..\tef\tRuleBasedApps\RuleBasedUID.H"


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102722bb, CTAppLaunchChecker2::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CTAppLaunchChecker2*	CTAppLaunchChecker2::NewL()
	{
	CTAppLaunchChecker2* theAppLaunchChecker = new (ELeave) CTAppLaunchChecker2();
	return theAppLaunchChecker;
	}

CTAppLaunchChecker2::~CTAppLaunchChecker2()
	{
	}
	
/**
The function CAppLaunchChecker2::TAppLaunchCode  recieves the UID of the application to be launched.
The function implements a rule for launching the application whose UID is supplied as the first parameter.
The function returns  CAppLaunchChecker::EAppLaunchIndifferent if the rule is executed completly.
*/
CAppLaunchChecker::TAppLaunchCode CTAppLaunchChecker2::OkayToLaunchL(const TUid aAppToLaunch, TApaTaskList& aTaskList)
	{
	if(KUidApp4.iUid == aAppToLaunch.iUid )
		{
		TApaTask task = aTaskList.FindApp(KUidApp3);
			if (task.Exists())
				{
				task.SendSystemEvent(EApaSystemEventShutdown);
				}	
		}	
	return CAppLaunchChecker::EAppLaunchIndifferent;	
	}

// DLL entry point
GLDEF_C TInt E32Dll()

	{
	return KErrNone;
	}
