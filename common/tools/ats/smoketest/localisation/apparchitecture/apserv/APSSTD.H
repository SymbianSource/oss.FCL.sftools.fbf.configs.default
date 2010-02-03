// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__APSSTD_H__)
#define __APSSTD_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__APAFLREC_H__)
#include <apaflrec.h>
#endif


enum TApaPanic
	{
	ESvrCreateServer,
	ESvrStartServer,
	EMainSchedulerError,
	//
	EDSvrNoApp,
	EEventFromBackupObserverError,
	EReferenceCountingError1,
	EReferenceCountingError2,
	EReferenceCountingError3,
	EPanicNullPointer,
	ENonNativeAppsUpdateLogOpenedTwice,
	ENonNativeAppsTFileDetailsPathNotSet,
	ENonNativeAppsTFileDetailsPathAlreadySet,
	ENonNativeAppsTFileDetailsOpenInBadState,
	ENonNativeAppsTFileDetailsOpenWithRealPathSet,
	ENonNativeAppsTFileDetailsOpenWithTempPathSet,
	ENonNativeAppsTFileDetailsRenameToRealInBadState,
	ENonNativeAppsTFileDetailsRenameToRealWithoutTempPath,
	ENonNativeAppsTFileDetailsRenameToRealWithoutRealPath,
	ENonNativeAppsTFileDetailsRenameToTempInBadState,
	ENonNativeAppsTFileDetailsRenameToTempWithoutRealPathSet,
	ENonNativeAppsTFileDetailsRenameToTempWithTempPathSet,
	ENonNativeAppsTFileDetailsCreateTempInBadState,
	ENonNativeAppsTFileDetailsCreateTempWithTempPathSet,
	ENonNativeAppsTFileDetailsDeleteInBadState,
	ENonNativeAppsTFileDetailsDeleteTemporaryInBadState,
	ENonNativeAppsTFileDetailsRestoreRealInBadState,
	ENonNativeAppsFileDeleterOverflow,
	ENonNativeAppsFileReplacerOverflow,
	ENonNativeAppsNegativeStoredFilePosition,
	ENonNativeAppsTestHandlingPanicDuringUpdate,
	ENonNativeAppsTestHandlingPanicDuringRollback,
	ENonNativeAppsNonNativeApplicationsUpdateInternalizeNewUpdateInBadState
	};


GLREF_C void Panic(TApaPanic aPanic);

#endif
