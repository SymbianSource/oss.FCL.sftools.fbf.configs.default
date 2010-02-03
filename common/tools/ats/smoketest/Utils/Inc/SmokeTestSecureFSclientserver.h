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
// clientserver - shared client/server definitions
// 
//

#ifndef __SMOKETEST_SECURE_FS_CLIENT_SERVER_H__
#define __SMOKETEST_SECURE_FS_CLIENT_SERVER_H__

#include <e32std.h>

_LIT(KTestServerName,"SmokeTestSecureFSserver");
_LIT(KTestServerImg,"SmokeTestSecureFSserver");		// EXE name
const TUid KServerUid3={0x101FF346};

const TInt KMaxTestMessage=8;

enum TTestIpc
	{
	ETestIpcSetHomeTime,
	ETestIpcDeleteFile,
	ETestIpcKillProcess,
	ETestIpcChangeLocale,
	ETestIpcCheckForFile,
    ETestIpcCopyFile,
    ETestIpcSetUpFbs,
	};

#endif // __SMOKETEST_SECURE_FS_CLIENT_SERVER_H__
