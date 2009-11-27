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
// Supporting ECOM plugin for use by  test programs that excercise
// rule based launching of applications.
// The class CTNonNativeAppLaunchChecker implements an 
// ECOM RuleBased plugin interface. 
// 
//




/**
 @file 
 @internalComponent - Internal Symbian test code
*/

#include <ImplementationProxy.h>
#include "tNonNativeAppLaunchChecker.h"
#include <e32cmn.h>

const TUid KUidNonNativeApp = {0xA0000B6E};

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0xA0000B6F, CTNonNativeAppLaunchChecker::NewL),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

CTNonNativeAppLaunchChecker*	CTNonNativeAppLaunchChecker::NewL()
	{
	CTNonNativeAppLaunchChecker* theAppLaunchChecker = new (ELeave) CTNonNativeAppLaunchChecker();
	return theAppLaunchChecker;
	}

CTNonNativeAppLaunchChecker::~CTNonNativeAppLaunchChecker()
	{
	}

/**
The function CAppLaunchChecker::OkayToLaunchL recieves the UID of the application to be launched.
The function implements a rule for not launching the application whose UID is supplied as the first parameter.
The function returns  CAppLaunchChecker::EAppLaunchDecline if the rule is executed completely.
*/
CAppLaunchChecker::TAppLaunchCode CTNonNativeAppLaunchChecker::OkayToLaunchL(const TUid aAppToLaunch, TApaTaskList& /* aTaskList */)
	{ 
	if(KUidNonNativeApp == aAppToLaunch)
		{
		return CAppLaunchChecker::EAppLaunchDecline;
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
