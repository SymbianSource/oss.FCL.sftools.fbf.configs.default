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



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#ifndef __TNONNATIVEAPPLAUNCHCHECKER_H__
#define __TNONNATIVEAPPLAUNCHCHECKER_H__

#include "..\inc\ApLaunchChecker.h"

class CTNonNativeAppLaunchChecker : public CAppLaunchChecker
	{
public:
	static CTNonNativeAppLaunchChecker* NewL();
	~CTNonNativeAppLaunchChecker();
	//Inherited from CApplaunchChecker
	TAppLaunchCode virtual OkayToLaunchL(const TUid aAppToLaunch, TApaTaskList& aTaskList);
	
private:
	CTNonNativeAppLaunchChecker() {};
	};

#endif
