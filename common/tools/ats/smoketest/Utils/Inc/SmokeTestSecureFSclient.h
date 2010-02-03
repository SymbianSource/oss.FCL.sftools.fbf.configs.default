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

#ifndef __SMOKETEST_SECURE_FS_CLIENT_H__
#define __SMOKETEST_SECURE_FS_CLIENT_H__

#include <e32std.h>

class RTestSecureFSclient : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C TInt SetHomeTime(const TTime& aTime);
	IMPORT_C TInt DeleteFileL(const TDesC& aFileName);
	IMPORT_C void KillProcessL(const TDesC& aProcessName);
	IMPORT_C TInt ChangeLocaleName(const TDesC& aLocaleDllName);
	IMPORT_C TInt CheckForFile(const TDesC& aFileName);
    IMPORT_C TInt CopyFile(const TDesC& Source, const TDesC& Dest);
    IMPORT_C void SetUpFbs();
	};

#endif // __SMOKETEST_SECURE_FS_CLIENT_H__
