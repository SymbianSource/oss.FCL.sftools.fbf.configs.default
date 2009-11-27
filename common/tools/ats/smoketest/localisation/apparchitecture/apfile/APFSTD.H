// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__APFSTD_H__)
#define __APFSTD_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

enum TApfPanic
	{
	EPanicFindAllAppsNotCalled,
	EPanicLibraryNotInList,
	EPanicInvalidFilename,
	EPanicIndexOutOfRange,
	EPanicNegativeIndex,
	//
	EDPanicBadDirName,
	EDPanicDriveOutOfRange,
	EDPanicStartDriveOutOfRange
	};


GLREF_C void Panic(TApfPanic aPanic);


#endif
