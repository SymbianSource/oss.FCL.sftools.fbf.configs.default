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


#if !defined(__APGSTD_H__)
#define __APGSTD_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif


enum TApgPanic
	{
	EPanicDefaultIcnFileDoesNotExist,
	EIllegalCallToGlassDoorSize,
	EPanicGetAppsNotInitialized,
	EPanicNullWindowGroupName,
	EPanicMethodNotSupported,
	EPanicIconAlreadyExists,
	EPanicDocAlreadyExists,
	EPanicInvalidParameter,
	EPanicNoDocumentOnConstruction,
	EPanicNoDocOrStoreWhenStoring,
	EPanicIndexOutOfRange,
	EPanicNoDocumentOnStore,
	EPanicNoAppDataSupplied,
	EPanicNoStoreOnRestore,
	EPanicNoStoreOnDetach,
	EPanicNoStoreOnAppUid,
	EPanicNoPictureInDoor,
	EPanicNoGlassPicture,
	EPanicNoStreamDic,
	EPanicNoCaption,
	//
	EDPanicNoCaptionInControl,
	EDPanicNoHostForStore,
	EDPanicNoPictureOnDrawing,
	EDPanicNoStoreOnGlassing,
	EDPanicNoStoreOnIconify,
	EDPanicNoIconInPicture,
	EDPanicWrongCommand,
	EDPanicNoProcess,
	EDPanicBadList,
	EDPanicArrayNotEmpty,
	EDPanicBadDataType,
	EDPanicBadIconSize,
	EDPanicInvalidObserver,
	EDPanicInvalidDataTypeArray,
	EDPanicInvalidViewArray,
	EDPanicInvalidFileOwnershipArray,
	EDPanicInvalidHandle,
	EPanicCaptionAlreadyExists,
	EPanicShortCaptionAlreadyExists,
	EFilenameTooLong,
	EPanicNoAppName,
	EPanicInconsistentResourceSizeInBytes,
	EPanicBadUnicodeCompression1,
	EPanicBadUnicodeCompression2,
	EPanicWrongResourceFormat1,
	EPanicWrongResourceFormat2,
	EPanicBadCompressedUnicodeRun,
	EPanicBadRunLengthParameter,
	EPanicNullPointer,
	EPanicIconFileWithoutLocalisableResourceFile,
	EPanicUnexpectedUid,
	EPanicBadRunLength,
	EPanicUpdatedAppsListNotEmpty, // Unused
	EPanicNullDefaultAppIconFileName,
	EDPanicNoPicture,
	EDPanicNoApplication,
	EDPanicInvalidIconIndexArray,
	EDPanicHandleAlreadySet
	};


GLREF_C void Panic(TApgPanic aPanic);


#endif
