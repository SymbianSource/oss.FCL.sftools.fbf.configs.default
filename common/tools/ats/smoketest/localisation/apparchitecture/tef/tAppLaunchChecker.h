// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#ifndef __TAPPLUNCHCHEKER_H__
#define __TAPPLUNCHCHEKER_H__

#include "..\inc\ApLaunchChecker.h"

class CTAppLaunchChecker : public CAppLaunchChecker
	{
public:
	// Two-phase constructor
	static CTAppLaunchChecker* NewL();
	~CTAppLaunchChecker();
	//Inherited from CApplaunchChecker
	TAppLaunchCode virtual OkayToLaunchL(const TUid aAppToLaunch, TApaTaskList& aTaskList);
	};

class CTAppLaunchChecker1 : public CAppLaunchChecker
	{
public:
	// Two-phase constructor
	static CTAppLaunchChecker1* NewL();
	~CTAppLaunchChecker1();
	//Inherited from CApplaunchChecker
	TAppLaunchCode virtual OkayToLaunchL(const TUid aAppToLaunch, TApaTaskList& aTaskList);
	};

#endif
