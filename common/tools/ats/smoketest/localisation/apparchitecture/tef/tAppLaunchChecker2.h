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
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#ifndef __TAPPLUNCHCHEKER2_H__
#define __TAPPLUNCHCHEKER2_H__


#include "..\inc\ApLaunchChecker.h"
#include <apgtask.h>

class CTAppLaunchChecker2 : public CAppLaunchChecker
	{
public:
	// Two-phase constructor
	static CTAppLaunchChecker2* NewL();
	~CTAppLaunchChecker2();
	//Inherited from CApplaunchChecker
	TAppLaunchCode virtual OkayToLaunchL(const TUid aAppToLaunch, TApaTaskList& aTaskList);
	};

#endif
