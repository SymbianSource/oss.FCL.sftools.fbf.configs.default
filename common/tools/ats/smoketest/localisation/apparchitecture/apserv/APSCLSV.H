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

#if !defined(__APGCLSV_H__)
#define __APGCLSV_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__APAID_H__)
#include <apaid.h>
#endif
#if !defined(__APMSTD_H__)
#include <apmstd.h>
#endif

// A version must be specifyed when creating a session with the server
const TUint KAppListServMajorVersionNumber=7;
const TUint KAppListServMinorVersionNumber=1;
const TUint KAppListServBuildVersionNumber=117;

enum TAppListServRanges
	{
	EFirstUnrestrictedOpcodeInAppListServ,
	EFirstOpcodeNeedingWriteDeviceDataInAppListServ=100,
	};

// opcodes used in message passing between client and server
enum TCountServRqst
	{
	//No Capability requirement
	EAppListServFirst = EFirstUnrestrictedOpcodeInAppListServ,
	EAppListServInitFullList,
	EAppListServInitEmbedList,
	EAppListServGetNextApp,
	EAppListServEmbedCount,
	EAppListServAppCount,
	EAppListServGetAppInfo,
	EAppListServGetAppCapability,
	EAppListServStartAppWithoutReturningThreadId,
	EAppListServStartAppReturningThreadId,
	EAppListServRecognizeData,
	EAppListServRecognizeDataPassedByFileHandle,
	EAppListServRecognizeSpecificData,
	EAppListServRecognizeSpecificDataPassedByFileHandle,
	EAppListServAppForDataType,
	EAppListServStartDocument,
	EAppListServStartDocumentByDataType,
	EAppListServStartDocumentByUid,
	EAppListServCreateDocumentByUid,
	EAppListServGetExecutableNameGivenDocument,
	EAppListServGetExecutableNameGivenDocumentPassedByFileHandle,
	EAppListServGetExecutableNameGivenDataType,
	EAppListServGetExecutableNameGivenAppUid,
	EAppListServGetOpaqueData,
	EAppListServGetNativeExecutableNameIfNonNative,
	EAppListServAppIconByUid,
	EAppListServAppForDocument,
	EAppListServAppForDocumentPassedByFileHandle,
	EAppListServGetConfidence,
	EAppListServGetBufSize,
	EAppListServSetBufSize,
	EAppListServGetDataTypesPhase1,
	EAppListServGetDataTypesPhase2,
	ESetNotify,
	ECancelNotify,
	EDCloseServer,
	EAppListServAppIconByUidAndSize,
	EAppListServGetAppIconSizes,
	EAppListServGetAppViews,
	EAppListServViewIconByUidAndSize,
	EAppListServGetFileOwnershipInfo,
	EAppListServNumberOfOwnDefinedIcons,
	EAppListServInitFilteredEmbedList,
	EAppListServInitAttrFilteredList,
	EAppListServAppIconFileName,
	EAppListServAppViewIconFileName,
	EAppListServInitServerAppList,
	EAppListServGetAppServices,
	EAppListServGetServiceImplementations,
	EAppListServGetServiceImplementationsDataType,
	EAppListServGetAppServiceUids,
	EAppListServGetAppServiceOpaqueData,
	EAppListServAppForDataTypeAndService,
	EAppListServAppForDocumentAndService,
	EAppListServAppForDocumentAndServicePassedByFileHandle,
	EAppListServApplicationLanguage,
	ERegisterListPopulationCompleteObserver,
	ECancelListPopulationCompleteObserver,
	EAppListServPreferredBufSize,
	EAppListServRecognizeFiles,
	EAppListServRecognizeFilesAsync,
	ECancelRecognizeFiles,
	EAppListServTransferRecognitionResult,
	EAppListServGetAppByDataType,
	EAppListServGetDefaultScreenNumber,
	EAppListServRuleBasedLaunching,
	EAppListServRegisterNonNativeApplicationType,
	EAppListServDeregisterNonNativeApplicationType,
	EAppListServPrepareNonNativeApplicationsUpdates,
	EAppListServRegisterNonNativeApplication,
	EAppListServDeregisterNonNativeApplication,
	EAppListServCommitNonNativeApplications,
	EAppListServRollbackNonNativeApplications,
	EAppListServGetAppType,
	ENotifyOnDataMappingChange,
	ECancelNotifyOnDataMappingChange,
	EMatchesSecurityPolicy,
	EAppListServSetAppShortCaption,
	EAppListServForceRegistration,
	EDebugHeapMark,
	EDebugHeapMarkEnd,
	EDebugHeapFailNext,
	EDebugClearAppInfoArray,
	EDebugFlushRecognitionCache,
	EDebugSetLoadRecognizersOnDemand,
	EDebugPerformOutstandingRecognizerUnloading,
	EAppListServAppIconFileHandle,
	EDebugAddFailingNonNativeApplicationsUpdate,
	EDebugAddPanicingNonNativeApplicationsUpdate,
	EDebugAddRollbackPanicingNonNativeApplicationsUpdate, // = 89
	EAppListServAppInfoProvidedByRegistrationFile = 99,
	//WriteDeviceData Capability requirement
	// ER5
	EAppListServSetConfidence=EFirstOpcodeNeedingWriteDeviceDataInAppListServ,
	// 8.1
	EAppListInsertDataMapping,
	EAppListInsertDataMappingIfHigher,
	EAppListDeleteDataMapping,
	// End Marker no Capability
	EAppListFirstUnusedOpcode,
 	};


enum TApaClientPanic
	{
	EClientBadRequest,
	EClientBadDescriptor,
	ENotifierAlreadyPresent,
	ENoSupportedDataTypes,
	EObserverAlreadyPresent,
	ENotifyOnDataMappingChangeRequestOutstanding
#if defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER)
	,EInvalidFileName
#endif
	};


struct SReturnData_AppForDocument
	{
	TUid iUid;
	TDataType iDataType;
	};

struct SReturnData_AppIconByUid
	{
	TInt iIcon;
	TInt iIconMask;
	};

struct SReturnData_AppIconByUidAndSize
	{
	TInt iIcon;
	TInt iIconMask;
	};

struct SReturnData_ViewIconByUidAndSize
	{
	TInt iIcon;
	TInt iIconMask;
	};

struct SNonNativeApplicationInfo
	{
	TUid iApplicationType;
	TInt iDrive;
	};

#endif
