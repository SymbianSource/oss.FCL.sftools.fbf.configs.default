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
// The classes CTAppLaunchChecker and CTAppLaunchChecker1 each implement an
// ECOM RuleBased plugin interface.
// 
//




/**
 @file 
 @internalComponent - Internal Symbian test code
*/

#include <ImplementationProxy.h>
#include "tAppLaunchChecker.h"
#include <apacmdln.h>
#include <W32STD.H>
#include <apgtask.h>
#include <coemain.h>
#include <apaid.h>
#include <apgcli.h>
#include "..\tef\tRuleBasedApps\RuleBasedUID.H"


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x1020d467, CTAppLaunchChecker::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x1020d468, CTAppLaunchChecker1::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

CTAppLaunchChecker*	CTAppLaunchChecker::NewL()
	{
	CTAppLaunchChecker* theAppLaunchChecker = new (ELeave) CTAppLaunchChecker();
	return theAppLaunchChecker;
	}

CTAppLaunchChecker::~CTAppLaunchChecker()
	{
	}

/**
The function CAppLaunchChecker::TAppLaunchCode  recieves the UID of the application to be launched.
The function implements a rule for launching the application whose UID is supplied as the first parameter.
The function returns  CAppLaunchChecker::EAppLaunchIndifferent if the rule is executed completly.
*/
CAppLaunchChecker::TAppLaunchCode CTAppLaunchChecker::OkayToLaunchL(const TUid aAppToLaunch, TApaTaskList& aTaskList)
	{ 
	if(KUidApp4== aAppToLaunch)
		{
		TApaTask task = aTaskList.FindApp(KUidApp2);
		if (task.Exists())
			{
			task.SendSystemEvent(EApaSystemEventShutdown);		
			}	
		}		
	return CAppLaunchChecker::EAppLaunchIndifferent;
	}

//second plug-in implementation
CTAppLaunchChecker1*	CTAppLaunchChecker1::NewL()
	{
	CTAppLaunchChecker1* theAppLaunchChecker = new (ELeave) CTAppLaunchChecker1();
	return theAppLaunchChecker;
	}

CTAppLaunchChecker1::~CTAppLaunchChecker1()
	{
	}
/**
The function CAppLaunchChecker1::TAppLaunchCode  recieves the UID of the application to be launched.
The function implements a rule for launching the application whose UID is supplied as the first parameter.
The function returns  CAppLaunchChecker::EAppLaunchIndifferent if the rule is executed completly.
*/
CAppLaunchChecker::TAppLaunchCode CTAppLaunchChecker1::OkayToLaunchL(const TUid aAppToLaunch, TApaTaskList& aTaskList)
	{
	                                                                                                                                                        
	if(KUidApp3 == aAppToLaunch)
		{
		TApaTask task = aTaskList.FindApp(KUidApp1);
		if (task.Exists())
			{
			task.SendSystemEvent(EApaSystemEventShutdown);
			}	
		}
	else if (KUidApp1 == aAppToLaunch)
		{
		TApaTask task = aTaskList.FindApp(KUidApp3);
		if (task.Exists())
			{
			return CAppLaunchChecker::EAppLaunchDecline;	
			}
		}
		
	return CAppLaunchChecker::EAppLaunchIndifferent;
	}

GLDEF_C TInt E32Dll()
//
// DLL entry point
//
	{
	return KErrNone;
	}
