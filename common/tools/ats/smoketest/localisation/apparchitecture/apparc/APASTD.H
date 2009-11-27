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


#if !defined(__APASTD_H__)
#define __APASTD_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

enum TApaPanic
	{
	EPanicApplicationAlreadyExists,
	EPanicDocumentNotCreated,
	EPanicCapabilityNotSet,
	EPanicNotExactlyOneDoc,
	EPanicMoreThanOneDoc,
	EPanicAppListNotEmpty,
	EPanicDocListNotEmpty,
	EPanicFileNameTooLong,
	EPanicNoGlassDoorMethodSupplied,
	EPanicNoModelHeaderWhenStoring,
	EPanicNoBaseDoorStream,
	EPanicAppNotInList,
	EPanicNoCleanupItem,
	EPanicNoApplication,
	EPanicNoAppStarter,
	EPanicNoAppLocator,
	EPanicNoIconInDoor,
	EPanicNotSupported,
	EPanicNoStreamDic,
	EPanicNoAppFinder,
	EPanicNoDocument,
	EPanicNoCaption,
	EPanicNoFactory,
	EPanicNoStore,
	EPanicNoStoreOnDetach,
	EPanicWrongStoreType,
	//
	EDPanicNoApp,
	EDPanicDocWithNoApp,
	EDPanicRemovingNullApp,
	EDPanicIllegalDoorFormat,
	EDPanicNoHostForStore,
	EDPanicWrongCommand,
	//
	EPanicEmbeddabilityOutOfRange,
	//
	EPanicBadApplicationFactoryType,
	EPanicUidsDoNotMatch,
	//
	EPanicServerDifferentiatorZero,
	//
	EPanicInvalidHandle,
	EPanicHandleAlreadySet,
	EPanicInvalidSubSession,
	EPanicEnvironmentSlotNotForPublicUse,
	EPanicBadHeapCellRestorerState,
	//
	EPanicInvalidScreenNumber,
	EPanicNullPointer,
	EDPanicInvalidToken,
	EDPanicNoAppRemover,
	EDPanicNoProcess,
	EDPanicNoAppHolder,
	EDPanicInvalidVersionNumber
	};


GLREF_C void Panic(TApaPanic aPanic);


#endif
