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
// AppArc server session
// 
//

#include <e32svr.h>
#include <apacmdln.h>
#include <apaflrec.h>
#include <apsserv.h>
#include "APSSES.H"
#ifdef _DEBUG
#include "APSSTD.H"
#endif //_DEBUG
#include "APSCLSV.H"
#include <apsscan.h>
#include <apgaplst.h>
#include <apgicnfl.h>
#include <apmrec.h>
#include <apmstd.h>
#include <apmfndr.h>
#include <datastor.h>
#include <s32mem.h>
#include <s32strm.h>
#include <s32file.h>
#include "../apgrfx/apprivate.h"
#include "apgnotif.h"
#include "../apfile/aprfndr.h"
#include "ApLaunchChecker.h"
#include "apsnnapps.h"

#include "apsecutils.h"

const TInt KApaAppListServMaxBuffer=256;
#include "APSRECCACHE.h"
const TInt KApaAppInfoArrayGranularity=4;
const TInt KApaAppInfoDesMaxLength=sizeof(TApaAppInfo);

#if defined(__PROFILE)
_LIT(KProfileAppForDocumentL, "AppForDocumentL - %d.%06d seconds");
_LIT(KProfileAppForDocumentPassedByFileHandleL, "AppForDocumentPassedByFileHandleL - %d.%06d seconds");
#endif
_LIT(KApaPanicCli,"APSERV-CLI");
const TInt KFinishedScanning=-2;


class MArrayItemWriter
	{
public:
	virtual TInt ArrayItemCount() const=0;
	virtual TInt ArrayItemSize() const=0;
	virtual void WriteArrayItemL(RWriteStream& aWriteStream,TInt aIndex) const=0;
	};

class TSizeArrayItemWriter : public MArrayItemWriter
	{
public:
	inline TSizeArrayItemWriter(const CArrayFix<TSize>& aArray) : iArray(aArray) {}
	
	// from MArrayItemWriter
	TInt ArrayItemCount() const;
	TInt ArrayItemSize() const;
	void WriteArrayItemL(RWriteStream& aWriteStream,TInt aIndex) const;
private:
	const CArrayFix<TSize>& iArray;
	};

class TViewDataArrayItemWriter : public MArrayItemWriter
	{
public:
	inline TViewDataArrayItemWriter(const CArrayPtr<CApaAppViewData>& aArray) : iArray(aArray) {}
	
	// from MArrayItemWriter
	TInt ArrayItemCount() const;
	TInt ArrayItemSize() const;
	void WriteArrayItemL(RWriteStream& aWriteStream,TInt aIndex) const;
private:
	const CArrayPtr<CApaAppViewData>& iArray;
	};

class TDesCArrayItemWriter : public MArrayItemWriter
	{
public:
	inline TDesCArrayItemWriter(const CDesCArray& aArray) : iArray(aArray) {}
	
	// from MArrayItemWriter
	TInt ArrayItemCount() const;
	TInt ArrayItemSize() const;
	void WriteArrayItemL(RWriteStream& aWriteStream,TInt aIndex) const;
private:
	const CDesCArray& iArray;
	};


class CApaAppListServSession::CApaAppInfo
	{
public:
	CApaAppInfo();
	~CApaAppInfo();
	void SetUid(const TUid aUid);
	void SetCaptionL(const TDesC& aCaption);
	void SetShortCaptionL(const TDesC& aShortCaption);
	void SetFullNameL(const TDesC& aFullName);
	inline TPtrC Caption() const;
	inline TPtrC ShortCaption() const;
	inline TPtrC FullName() const;
	inline TUid Uid() const;
private:
	TUid iUid;
	HBufC* iCaption;
	HBufC* iShortCaption;
	HBufC* iFullName;
	};

inline TPtrC CApaAppListServSession::CApaAppInfo::Caption() const
	{ return *iCaption; }

inline TPtrC CApaAppListServSession::CApaAppInfo::ShortCaption() const
	{ return *iShortCaption; }

inline TPtrC CApaAppListServSession::CApaAppInfo::FullName() const
	{ return *iFullName; }

inline TUid CApaAppListServSession::CApaAppInfo::Uid() const
	{ return iUid; }

// CApaAppListServSession

CApaAppListServSession* CApaAppListServSession::NewL(CApaAppListServer& aServer, RFs& aFs)
	{
	CApaAppListServSession* self=new(ELeave) CApaAppListServSession(aServer, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CApaAppListServSession::CApaAppListServSession(CApaAppListServer& aServer, RFs& aFs)
	: CSession2(),
	iServ(aServer),
	iFs(aFs),
	iMaxBufSize(KApaAppListServMaxBuffer),
	iApaAppInfoArray(KApaAppInfoArrayGranularity),
	iOpaqueData_pendingDispatchToClient(NULL)
	{}

void CApaAppListServSession::ConstructL()
	{
	iFileRecognitionUtility = new (ELeave) CFileRecognitionUtility(iServ, iMaxBufSize, iFs);
	iNonNativeApplicationsManager = CApsNonNativeApplicationsManager::NewL(iServ,iFs);
	}

CApaAppListServSession::~CApaAppListServSession()
	{
	delete iNonNativeApplicationsManager;
	delete iBuffer;
	iApaAppInfoArray.ResetAndDestroy();
	iApaAppInfoArray.Close();
	delete iFileRecognitionUtility;
	delete iRecognitionResult;
	delete iOpaqueData_pendingDispatchToClient;
	}

void CApaAppListServSession::ServiceL(const RMessage2& aMessage)
	{
	TBool completeMessage=ETrue;
	switch (aMessage.Function())
		{
	case ESetNotify:
		SetNotify(aMessage);
		completeMessage=EFalse;
		break;
	case ERegisterListPopulationCompleteObserver:
		RegisterListPopulationCompleteObserver(aMessage);
		completeMessage=EFalse;
		break;
	case ECancelListPopulationCompleteObserver:
		CancelListPopulationCompleteObserver();
		break;
	case EAppListServInitFullList:
		User::Leave(KErrNotSupported);
	case EAppListServInitEmbedList:
		User::Leave(KErrNotSupported);
	case EAppListServInitFilteredEmbedList:
		InitListL(aMessage,EListFilteredEmbeddedApps);
		break;
	case EAppListServInitAttrFilteredList:
		InitListL(aMessage,EListCapabilityAttrFilteredApps);
		break;
	case EAppListServInitServerAppList:
		InitListL(aMessage,EListServerApps);
		break;
	case EAppListServGetNextApp:
		GetNextAppL(aMessage);
		break;
	case EAppListServEmbedCount:
		EmbedCount(aMessage);
		break;
	case EAppListServAppCount:
		AppCount(aMessage);
		break;
	case EAppListServGetAppInfo:
		GetAppInfoL(aMessage);
		break;
	case EAppListServGetAppCapability:
		GetAppCapabilityL(aMessage);
		break;
	case EAppListServGetDefaultScreenNumber:
		GetDefaultScreenNumberL(aMessage);
		break;
	case EAppListServStartAppWithoutReturningThreadId:
		StartAppL(aMessage,EFalse);
		break;
	case EAppListServStartAppReturningThreadId:
		StartAppL(aMessage,ETrue);
		break;
	case EAppListServRecognizeData:
		RecognizeDataL(aMessage);
		break;
	case EAppListServRecognizeDataPassedByFileHandle:
		RecognizeDataPassedByFileHandleL(aMessage);
		break;
	case EAppListServRecognizeSpecificData:
		RecognizeSpecificDataL(aMessage);
		break;
	case EAppListServRecognizeSpecificDataPassedByFileHandle:
		RecognizeSpecificDataPassedByFileHandleL(aMessage);
		break;
	case EAppListServAppForDataType:
		AppForDataTypeL(aMessage);
		break;
	case EAppListServStartDocument:
		StartDocumentL(aMessage,EStart);
		break;
	case EAppListServStartDocumentByDataType:
		StartDocumentL(aMessage,EStartByDataType);
		break;
	case EAppListServStartDocumentByUid:
		StartDocumentL(aMessage,EStartByUid);
		break;
	case EAppListServCreateDocumentByUid:
		StartDocumentL(aMessage,ECreateByUid);
		break;
	case EAppListServGetExecutableNameGivenDocument:
		GetExecutableNameGivenDocumentL(aMessage);
		break;
	case EAppListServGetExecutableNameGivenDocumentPassedByFileHandle:
		GetExecutableNameGivenDocumentPassedByFileHandleL(aMessage);
		break;
	case EAppListServGetExecutableNameGivenDataType:
		GetExecutableNameGivenDataTypeL(aMessage);
		break;
	case EAppListServGetExecutableNameGivenAppUid:
		GetExecutableNameGivenAppUidL(aMessage);
		break;
	case EAppListServGetOpaqueData:
		GetOpaqueDataL(aMessage);
		break;
	case EAppListServGetNativeExecutableNameIfNonNative:
		GetNativeExecutableNameIfNonNativeL(aMessage);
		break;
	case EAppListServAppIconByUid:
		IconForAppL(aMessage);
		break;
	case EAppListServAppForDocument:
		AppForDocumentL(aMessage, NULL);
		break;
	case EAppListServAppForDocumentPassedByFileHandle:
		AppForDocumentPassedByFileHandleL(aMessage, NULL);
		break;
	case EAppListServGetConfidence:
		GetConfidenceL(aMessage);
		break;
	case EAppListServSetConfidence:
		SetConfidence(aMessage);
		break;
	case EAppListServGetBufSize:
		GetBufSize(aMessage);
		break;
	case EAppListServSetBufSize:
		SetBufSize(aMessage);
		break;
	case EAppListServGetDataTypesPhase1:
		GetDataTypesCountL(aMessage);
		break;
	case EAppListServGetDataTypesPhase2:
		GetDataTypesL(aMessage);
		break;
	case ECancelNotify:
		CancelNotify();
		break;
	case EAppListServAppIconByUidAndSize:
		IconForAppBySizeL(aMessage);
		break;
	case EAppListServAppIconFileHandle:
		IconFileHandleForAppL(aMessage);
		break;	
	case EAppListServGetAppIconSizes:
		AppIconSizesL(aMessage);
		break;
	case EAppListServViewIconByUidAndSize:
		IconForViewBySizeL(aMessage);
		break;
	case EAppListServGetAppViews:
		AppViewsL(aMessage);
		break;
	case EAppListServGetFileOwnershipInfo:
		AppFileOwnershipInfoL(aMessage);
		break;
	case EAppListServNumberOfOwnDefinedIcons:
		NumberOfOwnDefinedIconsL(aMessage);
		break;
	case EAppListServApplicationLanguage:
		ApplicationLanguageL(aMessage);
		break;
	case EAppListServAppInfoProvidedByRegistrationFile: // private OpCode for CEikApplication's use only
		AppInfoProvidedByRegistrationFileL(aMessage);
		break;
	case EAppListServAppIconFileName:
		IconFileNameL(aMessage);
		break;
	case EAppListServAppViewIconFileName:
		ViewIconFileNameL(aMessage);
		break;
	case EAppListInsertDataMapping:
	case EAppListInsertDataMappingIfHigher:
		InsertDataMappingL(aMessage);
		break;
	case EAppListDeleteDataMapping:
		DeleteDataMappingL(aMessage);
		break;
	case EAppListServGetAppByDataType:
		GetAppByDataTypeL(aMessage);
		break;		
	case EAppListServGetAppServices:
	case EAppListServGetServiceImplementations:
	case EAppListServGetServiceImplementationsDataType:
	case EAppListServGetAppServiceUids:
	case EAppListServGetAppServiceOpaqueData:
		GetAppServicesL(aMessage);
		break;
	case EAppListServAppForDataTypeAndService:
		AppForDataTypeAndServiceL(aMessage);
		break;
	case EAppListServAppForDocumentAndService:
		{
		const TUid serviceUid=TUid::Uid(aMessage.Int1());
		AppForDocumentL(aMessage, &serviceUid);
		}
		break;
	case EAppListServAppForDocumentAndServicePassedByFileHandle:
		{
		const TUid serviceUid(TUid::Uid(aMessage.Int1()));
		AppForDocumentPassedByFileHandleL(aMessage, &serviceUid);
		}
		break;
	case EAppListServRegisterNonNativeApplicationType:
		RegisterNonNativeApplicationTypeL(aMessage);
		break;
	case EAppListServDeregisterNonNativeApplicationType:
		DeregisterNonNativeApplicationTypeL(aMessage);
		break;
	case EAppListServPrepareNonNativeApplicationsUpdates:
		iNonNativeApplicationsManager->PrepareNonNativeApplicationsUpdatesL();
		break;
	case EAppListServRegisterNonNativeApplication:
		iNonNativeApplicationsManager->RegisterNonNativeApplicationL(aMessage);
		break;
	case EAppListServDeregisterNonNativeApplication:
		iNonNativeApplicationsManager->DeregisterNonNativeApplicationL(aMessage);
		break;
	case EAppListServCommitNonNativeApplications:
		iNonNativeApplicationsManager->CommitNonNativeApplicationsUpdatesL(aMessage);
		completeMessage=EFalse;
		break;
	case EAppListServRollbackNonNativeApplications:
		iNonNativeApplicationsManager->RollbackNonNativeApplicationsUpdates();
		break;
	case EAppListServGetAppType:
		GetAppTypeL(aMessage);
		break;
	case EAppListServForceRegistration:
		ForceRegistrationL(aMessage);
		completeMessage=EFalse;
		break;
	case EAppListServPreferredBufSize:
		aMessage.Complete(PreferredBufSize());
		break;
	case EAppListServRecognizeFiles:
		RecognizeFilesL(aMessage);
		break;
	case EAppListServTransferRecognitionResult:
		TransferRecognitionResultL(aMessage);
		break;
	case EAppListServRecognizeFilesAsync:
		RecognizeFilesAsyncL(aMessage);
		completeMessage=EFalse;
		break;
	case ECancelRecognizeFiles:
		CancelRecognizeFiles();
		break;
	case EAppListServRuleBasedLaunching:
		RuleBasedLaunchingL(aMessage);
		break;
	case EMatchesSecurityPolicy:
		MatchesSecurityPolicyL(aMessage);
		break;
	case EAppListServSetAppShortCaption:
		SetAppShortCaptionL(aMessage);
		break;
	case ENotifyOnDataMappingChange:
		NotifyOnDataMappingChange(aMessage);
		completeMessage=EFalse;
		break;
	case ECancelNotifyOnDataMappingChange:
		CancelNotifyOnDataMappingChange();
		break;
	case EDebugHeapMark:
	#ifdef _DEBUG
		__UHEAP_MARK;
	#endif
		break;
	case EDebugHeapMarkEnd:
	#ifdef _DEBUG
		__UHEAP_MARKENDC(aMessage.Int0());
	#endif
		break;
	case EDebugHeapFailNext:
	#ifdef _DEBUG
		__UHEAP_FAILNEXT(aMessage.Int0());
	#endif
		break;
	case EDebugClearAppInfoArray:
	#ifdef _DEBUG
		iApaAppInfoArray.ResetAndDestroy();
		iApaAppInfoArray.Compress();
	#endif
		break;
	case EDebugFlushRecognitionCache:
	#ifdef _DEBUG
		iServ.FlushRecognitionCache();
	#endif
		break;
	case EDebugSetLoadRecognizersOnDemand:
	#ifdef _DEBUG
		iServ.SetLoadRecognizersOnDemandL(aMessage.Int0());
	#endif
		break;
	case EDebugPerformOutstandingRecognizerUnloading:
	#ifdef _DEBUG
		iServ.PerformOutstandingRecognizerUnloading();
		REComSession::FinalClose();
	#endif
		break;
	case EDebugAddFailingNonNativeApplicationsUpdate:
	#ifdef _DEBUG
		iNonNativeApplicationsManager->ForceFailInNonNativeApplicationsUpdatesL();
	#endif
		break;
	case EDebugAddPanicingNonNativeApplicationsUpdate:
	#ifdef _DEBUG
		iNonNativeApplicationsManager->ForcePanicInNonNativeApplicationsUpdatesL();
	#endif
		break;
	case EDebugAddRollbackPanicingNonNativeApplicationsUpdate:
	#ifdef _DEBUG
		iNonNativeApplicationsManager->ForcePanicInNonNativeApplicationsRollbackL();
	#endif
		break;
	default:
		aMessage.Panic(KApaPanicCli,EClientBadRequest);
		break;
		}
		
	if (completeMessage && !aMessage.IsNull())
		{
		aMessage.Complete(KErrNone);
		}
	}
void CApaAppListServSession::NotifyOnDataMappingChange(const RMessage2& aMessage)
	{ 
	if (!iMessage_NotifyOnDataMappingChange.IsNull())
			{
			aMessage.Panic(KApaPanicCli,ENotifyOnDataMappingChangeRequestOutstanding);
			}
	else
			{
			iMessage_NotifyOnDataMappingChange=aMessage;
			}
	}
void CApaAppListServSession::CancelNotifyOnDataMappingChange()
	{
	if (!iMessage_NotifyOnDataMappingChange.IsNull())
			{
			iMessage_NotifyOnDataMappingChange.Complete(KErrCancel);
			}
	} //lint !e1762 Suppress member function could be made const


TInt CApaAppListServSession::PreferredBufSize() const
	{
	TInt preferredBufferSize = 0;
	TRAPD(err, preferredBufferSize = iServ.DataRecognizerPreferredBufSizeL());
	return (err==KErrNone) ? Min(iMaxBufSize, preferredBufferSize) : iMaxBufSize;
	}

void CApaAppListServSession::RegisterNonNativeApplicationTypeL(const RMessage2& aMessage)
	{
	const TUid applicationType(TUid::Uid(aMessage.Int0()));
	HBufC* const nativeExecutable=HBufC::NewLC(User::LeaveIfError(aMessage.GetDesLength(1)));
	{TPtr nativeExecutable_asWritable(nativeExecutable->Des());
	aMessage.ReadL(1, nativeExecutable_asWritable);}
	iServ.RegisterNonNativeApplicationTypeL(applicationType, *nativeExecutable);
	CleanupStack::PopAndDestroy(nativeExecutable);
	}

void CApaAppListServSession::DeregisterNonNativeApplicationTypeL(const RMessage2& aMessage)
	{
	const TUid applicationType(TUid::Uid(aMessage.Int0()));
	iServ.DeregisterNonNativeApplicationTypeL(applicationType);
	}

void CApaAppListServSession::GetAppTypeL(const RMessage2& aMessage)
	{
	TInt uid = aMessage.Int0();
	CApaAppData* appData = iServ.AppList().AppDataByUid(TUid::Uid(uid));
	if (!appData)
		{
		aMessage.Complete(KErrNotFound);
		return;
		}
	TPckgBuf<TUid> typeUid(appData->NonNativeApplicationType());
	aMessage.WriteL(1,typeUid);
	aMessage.Complete(KErrNone);
	}
	
void CApaAppListServSession::ForceRegistrationL(const RMessage2& aMessage)
	{
	TInt bufferSize = aMessage.GetDesLength(0);
	User::LeaveIfError(bufferSize);
	HBufC8* const buffer=HBufC8::NewLC(bufferSize);
	TPtr8 buffer_asWritable(buffer->Des());
	aMessage.ReadL(0,buffer_asWritable);
		
	RDesReadStream readStream(*buffer);
	CleanupClosePushL(readStream);
	const TUint count=readStream.ReadUint32L();
	for (TUint i = 0; i < count; ++i)
		{
		TUint length = readStream.ReadUint32L();
		HBufC* regFile = HBufC::NewLC(length);
  		TPtr ptr(regFile->Des());
  		readStream.ReadL(ptr, length);
		iServ.AppList().AddForcedRegistrationL(regFile);
		CleanupStack::Pop(regFile);
		}
	CleanupStack::PopAndDestroy(&readStream);
	
	CleanupStack::PopAndDestroy(buffer);
	
	// Trigger a rescan, when rescan completes it will complete iNotifyOnScanCompleteMsg
	iNotifyOnScanCompleteMsg=aMessage;
	iServ.UpdateApps();
	}
	
void CApaAppListServSession::AppForDocumentPassedByFileHandleL(const RMessage2& aMessage, const TUid* aServiceUid)
	{
#if defined(__PROFILE)
	TProfile profile;
	RDebug::ProfileReset(5,1);
	RDebug::ProfileStart(5);
#endif
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.AdoptFromClient(aMessage, 2, 3));
	SReturnData_AppForDocument returnData;
	returnData.iDataType=iServ.RecognizeDataL(file, PreferredBufSize()).iDataType;
	returnData.iUid=(returnData.iDataType!=TDataType())? AppForDataTypeL(returnData.iDataType, aServiceUid): TUid::Null();
	CleanupStack::PopAndDestroy(&file);
	aMessage.WriteL(0,TPckgC<SReturnData_AppForDocument>(returnData));
#if defined(__PROFILE)
	RDebug::ProfileEnd(5);
	RDebug::ProfileResult(&profile,5,1);
	RDebug::Print(KProfileAppForDocumentPassedByFileHandleL,profile.iTime/1000000,profile.iTime%1000000);
	RDebug::ProfileStart(5);
#endif
	}

void CApaAppListServSession::AppForDocumentL(const RMessage2& aMessage, const TUid* aServiceUid)
	{
#if defined(__PROFILE)
	TProfile profile;
	RDebug::ProfileReset(5,1);
	RDebug::ProfileStart(5);
#endif
	HBufC* const fileName=HBufC::NewLC(User::LeaveIfError(aMessage.GetDesLength(2)));
	{TPtr fileName_asWritable(fileName->Des());
	aMessage.ReadL(2,fileName_asWritable);}
	HBufC8* const buffer=HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesLength(3)));
	{TPtr8 buffer_asWritable(buffer->Des());
	aMessage.ReadL(3,buffer_asWritable);}
	SReturnData_AppForDocument returnData;
	returnData.iDataType=iServ.RecognizeDataL(*fileName, *buffer).iDataType;
	returnData.iUid=(returnData.iDataType!=TDataType())? AppForDataTypeL(returnData.iDataType, aServiceUid): TUid::Null();
#if defined(__PROFILE)
	RDebug::ProfileEnd(5);
	RDebug::ProfileResult(&profile,5,1);
	RDebug::Print(KProfileAppForDocumentL,profile.iTime/1000000,profile.iTime%1000000);
	RDebug::ProfileStart(5);
#endif
	CleanupStack::PopAndDestroy(2, fileName);
	aMessage.WriteL(0,TPckgC<SReturnData_AppForDocument>(returnData));
	}

void CApaAppListServSession::GetConfidenceL(const RMessage2& aMessage)
// void GetAcceptedConfidence(TInt& aConfidence);
	{
	aMessage.WriteL(0,TPckgBuf<TInt>(iServ.DataRecognizer()->AcceptedConfidence()));
	}

void CApaAppListServSession::SetConfidence(const RMessage2& aMessage)
// SetAcceptedConfidence(TInt aConfidence);
	{
	__ASSERT_DEBUG(iServ.DataRecognizer(), Panic(EPanicNullPointer));
	iServ.DataRecognizer()->SetAcceptedConfidence(aMessage.Int0());
	}

void CApaAppListServSession::GetBufSize(const RMessage2& aMessage)
// GetMaxDataBufSize(TInt& aBufSize);
	{
	aMessage.Complete(iMaxBufSize);
	}

void CApaAppListServSession::SetBufSize(const RMessage2& aMessage)
// SetMaxDataBufSize(TInt aBufSize);
	{
	iMaxBufSize=aMessage.Int0();
	}

void CApaAppListServSession::GetDataTypesCountL(const RMessage2& aMessage)
	{
	delete iBuffer;
	iBuffer=NULL;
	CDataTypeArray* const dataTypes=new(ELeave) CDataTypeArray(5);
	CleanupStack::PushL(dataTypes);
	iServ.DataTypeL(*dataTypes);
	TInt completionCode=0; // not KErrNone, as completion code of zero tells the client that zero data types were found
	if (dataTypes->Count()>0)
		{
		CBufBase* const buffer=CBufFlat::NewL(sizeof(TDataType)); 
		CleanupStack::PushL(buffer); 
		RBufWriteStream writeStream(*buffer); 
		writeStream<<*dataTypes; 
		CleanupStack::Pop(buffer); 
		iBuffer=buffer;
		completionCode=iBuffer->Ptr(0).Size(); // number of bytes in buffer (not number of data types)
		}
	CleanupStack::PopAndDestroy(dataTypes);
	aMessage.Complete(completionCode);
	}

void CApaAppListServSession::GetDataTypesL(const RMessage2& aMessage)
// GetSupportedDataTypes(CDataTypeArray& aDataTypes);
	{
	if(iBuffer==NULL)
		{
		aMessage.Panic(KApaPanicCli,ENoSupportedDataTypes);
		}
	else
		{
		aMessage.WriteL(0,iBuffer->Ptr(0));
		delete iBuffer;
		iBuffer=NULL;
		}
	}

CApaAppData& CApaAppListServSession::FindAppInListL(TUid aUid)
	{
	TApaAppEntry dummy;
	CApaAppData* app=NULL;
	if (!FindAppInList(app, dummy, aUid))
		{
		User::Leave(KErrNotFound);
		}
	return *app;
	}

void CApaAppListServSession::SendArrayL(const MArrayItemWriter& aArrayItemWriter,const RMessage2& aMessage) const
	{
	const TInt sizeOfBuffer=aMessage.Int1();
	const TInt arrayItemCount=aArrayItemWriter.ArrayItemCount();
	const TInt sizeRequired=sizeof(TInt)+(arrayItemCount*aArrayItemWriter.ArrayItemSize());
	__ASSERT_DEBUG(sizeRequired>0,User::Invariant());
	if (sizeRequired>sizeOfBuffer)
		{
		User::Leave(sizeRequired); // causes aMessage to complete with sizeRequired
		}
	CBufFlat* const buf=CBufFlat::NewL(sizeRequired);
	CleanupStack::PushL(buf);
	buf->ExpandL(0,sizeRequired);
	RBufWriteStream writeStream;
	writeStream.Open(*buf);
	writeStream.WriteUint32L(arrayItemCount);
	for (TInt i=0; i<arrayItemCount; ++i)
		{
		aArrayItemWriter.WriteArrayItemL(writeStream,i);
		}
	writeStream.CommitL();
	aMessage.WriteL(2,buf->Ptr(0));
	CleanupStack::PopAndDestroy(buf);
	}

void CApaAppListServSession::AppIconSizesL(const RMessage2& aMessage)
	{
	const TUid uid=TUid::Uid(aMessage.Int0());
	CApaAppData& app = FindAppInListL(uid);
	if (app.NonMbmIconFile())
		{
		User::Leave(KErrNotSupported);
		}
		
	CArrayFixFlat<TSize>* array = app.IconSizesL();
	CleanupStack::PushL(array);
	TSizeArrayItemWriter arrayItemWriter(*array);		
	SendArrayL(arrayItemWriter, aMessage);
	CleanupStack::PopAndDestroy(array);
	}

void CApaAppListServSession::AppViewsL(const RMessage2& aMessage)
	{
	const TUid uid=TUid::Uid(aMessage.Int0());
	CApaAppData& app = FindAppInListL(uid);
	TViewDataArrayItemWriter arrayItemWriter(*app.Views());
	SendArrayL(arrayItemWriter,aMessage);
	}

void CApaAppListServSession::AppFileOwnershipInfoL(const RMessage2& aMessage)
	{
	const TUid uid=TUid::Uid(aMessage.Int0());
	CApaAppData& app = FindAppInListL(uid);
	TDesCArrayItemWriter arrayItemWriter(*app.OwnedFiles());
	SendArrayL(arrayItemWriter,aMessage);
	}

void CApaAppListServSession::NumberOfOwnDefinedIconsL(const RMessage2& aMessage)
	{
	const TUid uid=TUid::Uid(aMessage.Int0());
	TApaAppEntry entry;
	CApaAppData* app=NULL;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}
	if (app->NonMbmIconFile())
		{
		User::Leave(KErrNotSupported);
		}
	TInt count, defaultIconsUsed;
	app->GetIconInfo(count, defaultIconsUsed);
	if (defaultIconsUsed)
		{
		count=0;
		}
	TPckgC<TInt> pckg(count); 
	aMessage.Write(1,pckg);
	}
	
void CApaAppListServSession::ApplicationLanguageL(const RMessage2& aMessage)
	{
	const TUid appUid = TUid::Uid(aMessage.Int0());
	TApaAppEntry appEntry;
	CApaAppData* appData = NULL;
	if (!FindAppInList(appData, appEntry, appUid))
		{
		User::Leave(KErrNotFound);
		}
	const TLanguage appLanguage = appData->ApplicationLanguage();
	TPckgC<TLanguage> pckg(appLanguage); 
	aMessage.Write(1,pckg);
	}
	

void CApaAppListServSession::IconForViewBySizeL(const RMessage2& aMessage)
	// Passes back handles to the icon and mask bitmaps for bitmap sharing 
	{
	TApaAppViewIconSizeData appViewIconSizeData;
	{TPckg<TApaAppViewIconSizeData> appViewIconSizeData_asDescriptor(appViewIconSizeData);
	aMessage.ReadL(0,appViewIconSizeData_asDescriptor);}
	TApaAppEntry entry;
	CApaAppData* app=NULL;
	if (!FindAppInList(app,entry,appViewIconSizeData.iAppUid))
		{
		User::Leave(KErrNotFound);
		}
	ASSERT(app->Views());
	const CArrayPtr<CApaAppViewData>& viewDataArray=*app->Views();
	CApaMaskedBitmap* icon=NULL;
	const TInt count=viewDataArray.Count();
	for (TInt ii=0; ii<count; ii++)
		{
		const CApaAppViewData& appViewData=*viewDataArray[ii];
		if (appViewData.Uid()==appViewIconSizeData.iViewUid)
			{
			if (appViewData.NonMbmIconFile())
				{
				User::Leave(KErrNotSupported);
				}
			icon=appViewData.Icon(appViewIconSizeData.iSize);
			break;
			}
		}
	if (icon==NULL)
		{
		User::Leave(KErrNotFound);
		}
	SReturnData_ViewIconByUidAndSize returnData;
	returnData.iIcon=icon->Handle();
	returnData.iIconMask=icon->Mask()->Handle();
	aMessage.WriteL(1,TPckgC<SReturnData_ViewIconByUidAndSize>(returnData));
	}

void CApaAppListServSession::IconForAppBySizeL(const RMessage2& aMessage)
	{
	const TUid uid=TUid::Uid(aMessage.Int0());
	const TSize size(aMessage.Int1(),aMessage.Int2());
	TApaAppEntry entry;
	CApaAppData* app=NULL;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}
	if (app->NonMbmIconFile())
		{
		User::Leave(KErrNotSupported);
		}
	CApaMaskedBitmap* const icon=app->Icon(size);
	if (icon==NULL)
		{
		User::Leave(KErrNotFound);
		}
	SReturnData_AppIconByUidAndSize returnData;
	returnData.iIcon=icon->Handle();
	returnData.iIconMask=icon->Mask()->Handle();
	aMessage.WriteL(3,TPckgC<SReturnData_AppIconByUidAndSize>(returnData));

	}

void CApaAppListServSession::IconFileHandleForAppL(const RMessage2& aMessage)
	{
	const TUid uid=TUid::Uid(aMessage.Int0());
	TApaAppEntry entry;
	CApaAppData* app=NULL;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}
	TPtrC iconFileName = app->IconFileName();
	if (iconFileName.Length()==0)
		{
		User::Leave(KErrNotFound);
		}
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(fs, iconFileName, EFileShareReadersOnly));
	User::LeaveIfError(file.TransferToClient(aMessage, 1));
	CleanupStack::PopAndDestroy(2, &fs); //file and fs
	}

void CApaAppListServSession::IconForAppL(const RMessage2& aMessage)
// from GetAppIcon(TUid aAppUid, TInt aSideInPixels, CApaMaskedBitmap& aAppBitmap);
// BUT!  It's interface is uid, side, icon handle, mask handle for bitmap sharing 
// and avoiding IPC overhead
	{
	const TUid uid=TUid::Uid(aMessage.Int0());
	const TInt side=aMessage.Int1();
	TApaAppEntry entry;
	CApaAppData* app=NULL;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}
	if (app->NonMbmIconFile())
		{
		User::Leave(KErrNotSupported);
		}
	CApaMaskedBitmap* const icon=app->Icon(side);
	if (icon==NULL)
		{
		User::Leave(KErrNotFound);
		}
	SReturnData_AppIconByUid returnData;
	returnData.iIcon=icon->Handle();
	returnData.iIconMask=icon->Mask()->Handle();
	aMessage.WriteL(2,TPckgC<SReturnData_AppIconByUid>(returnData));
	}

void CApaAppListServSession::AppForDataTypeL(const RMessage2& aMessage)
// from AppForDataType(const TDataType& aDataType, TUid& aAppUid);
	{
	if (sizeof(TDataType) != aMessage.GetDesLengthL(0))
		{
		//Leave with KErrArgument if client passes other than TDataType
		User::Leave(KErrArgument);
		}
	TDataType dataType;
	{TPckg<TDataType> dataType_asDescriptor(dataType);
	aMessage.ReadL(0,dataType_asDescriptor);}
	TPckgBuf<TUid> uid_asDescriptor(AppForDataTypeL(dataType, NULL));
	aMessage.WriteL(1,uid_asDescriptor);
	aMessage.Complete(KErrNone);
	}

void CApaAppListServSession::AppForDataTypeAndServiceL(const RMessage2& aMessage)
	{
	TDataType dataType;
	{TPckg<TDataType> dataType_asDescriptor(dataType);
	aMessage.ReadL(0,dataType_asDescriptor);}
	const TUid serviceUid=TUid::Uid(aMessage.Int1());
	TPckgBuf<TUid> uid(AppForDataTypeL(dataType, &serviceUid));
	aMessage.WriteL(2,uid);
	aMessage.Complete(KErrNone);
	}

TUid CApaAppListServSession::AppForDataTypeL(const TDataType& aDataType, const TUid* aServiceUid)
	{
	// It is possible to register apps as datatype handlers with system priority,
	// which means that they are not overridable by user mappings.
	// So search the list of apps for a datahandler and get the associated
	// priority
	TUid uid;
	TInt priority;
	uid=AppList().PreferredDataHandlerL(aDataType, aServiceUid, priority);
	if (priority == KDataTypePrioritySystem)
		{
		// We have found a handler with system priority
		return uid;
		}
		
	// No handler with system priority so see if there is a user mapping
	TUid userUid = KNullUid;
	if (aServiceUid)
		{
		iServ.GetAppByDataType(aDataType,*aServiceUid,userUid);
		}
	else
		{
		iServ.GetAppByDataType(aDataType,userUid);
		}
	TApaAppEntry entry;
	CApaAppData* app=NULL;
	if ((userUid.iUid!=0) && FindAppInList(app,entry,userUid))
		{
		// The user mapping is valid
		return userUid;
		}
		
	// A user mapping was not found or is invalid so try to use
	// the uid returned by PreferredDataHandlerL.
	if (uid.iUid==0 && aDataType.IsNative())
		{
		uid=aDataType.Uid();
		}
	return uid;
	}

void CApaAppListServSession::InsertDataMappingL(const RMessage2& aMessage)
	{
	TPckgBuf<TDataType> dataType;
	aMessage.ReadL(0, dataType);
	TDataTypePriority priority = aMessage.Int1();
	const TUid appUid = { aMessage.Int2() };

	if(priority>KDataTypeUnTrustedPriorityThreshold || priority == KDataTypePrioritySystem ) 
	   {   
	   CApaAppData* appData = AppList().AppDataByUid(appUid);
	   if( appData )	
		  {
          TBool hasWriteDeviceDataCap( EFalse );
          TBool isSidTrusted( EFalse );
          
          CApaSecurityUtils::CheckAppSecurity( appData->AppEntry().iFullName, 
                                               hasWriteDeviceDataCap,
                                               isSidTrusted);
          
          if (priority == KDataTypePrioritySystem )
              {
              // Check if the app has capability WriteDeviceData
              if ( !hasWriteDeviceDataCap )
                  {
                  priority = KDataTypePriorityNormal;
                  }
              }
          else
              {
              TPtrC registrationFilePath = appData->RegistrationFileName ( );
              TInt match = registrationFilePath.MatchF (
                                          KLitPathForUntrustedRegistrationResourceFiles );
              //Check if registration file is in path for untrusted apps 
              //and its SID is untrusted
              if (match != KErrNotFound && !isSidTrusted )
                  {
                  // "cap" the priority if UnTrusted apps claim for priority higher 
                  // than UnTrusted apps Threshold priority
                  priority = KDataTypeUnTrustedPriorityThreshold;
                  }
              }
          }
	   else
		  {
              //if the application is not present in the applist 
              //then the priority will be reduced to Threshold
              priority = KDataTypeUnTrustedPriorityThreshold;
		  }
	   }
	const TUid serviceUid = { aMessage.Int3() };
	if(aMessage.Function() == EAppListInsertDataMappingIfHigher)
		{
		const TBool response = iServ.InsertAndStoreIfHigherL(dataType(), priority, appUid);
		aMessage.WriteL(3, TPckgC<TBool>(response));

		}
	else
		{
		iServ.InsertAndStoreDataMappingL(dataType(), priority, appUid, serviceUid);
		}
	}

void CApaAppListServSession::DeleteDataMappingL(const RMessage2& aMessage)
	{
	TPckgBuf<TDataType> dataType;
	aMessage.ReadL(0, dataType);
	const TUid serviceUid = { aMessage.Int1() };
	TUid uid;
	iServ.GetAppByDataType(dataType(),serviceUid,uid);
	if (uid != KNullUid)
		{
		// TypeStore doesn't support deletion of an inexistent mapping
		iServ.DeleteAndStoreDataMappingL(dataType(), serviceUid);
		aMessage.Complete(KErrNone);
		}
	else
		{
		aMessage.Complete(KErrNotFound);
		}
	}
	
void CApaAppListServSession::GetAppByDataTypeL(const RMessage2& aMessage) const
	{
	TPckgBuf<TDataType> dataType;
	aMessage.ReadL(0,dataType);
	const TUid serviceUid = { aMessage.Int1() };
	TUid uid;
	iServ.GetAppByDataType(dataType(),serviceUid,uid);
	TPckgC<TUid> uidpckg(uid);
	aMessage.WriteL(2,uidpckg);
	aMessage.Complete(KErrNone);
	}

void CApaAppListServSession::StartDocumentL(const RMessage2& aMessage,TAppListDocumentStart aStartType)
// from StartDocument(const TDesC& aFileName, TThreadId& aId, TLaunchType aLaunchType);
// from StartDocument(const TDesC& aFileName, const TDataType& aDataType, TThreadId& aId, TLaunchType aLaunchType);
// from StartDocument(const TDesC& aFileName, TUid aAppUid, TThreadId& aId, TLaunchType aLaunchType);
// from CreateDocument(const TDesC& aFileName, TUid aAppUid, TThreadId& aId, TLaunchType aLaunchType);
// This method needs to open the file, mime type it then launch it.
	{
	HBufC* const fileName=HBufC::NewLC(User::LeaveIfError(aMessage.GetDesLength(1)));
	{TPtr fileName_asWritable(fileName->Des());
	aMessage.ReadL(1,fileName_asWritable);}

	TUid uid;
	if ((aStartType==EStartByUid) || (aStartType==ECreateByUid))
		{
		uid.iUid=aMessage.Int2();
		}
	else
		{
		TDataType* const dataType=new(ELeave) TDataType();
		CleanupStack::PushL(dataType);
		if (aStartType==EStart)
			{
			HBufC8* const buffer=HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesLength(2)));
			{TPtr8 buffer_asWritable(buffer->Des());
			aMessage.ReadL(2,buffer_asWritable);}
			*dataType=iServ.RecognizeDataL(*fileName, *buffer).iDataType;
			CleanupStack::PopAndDestroy(buffer);
			}
		else
			{
			__ASSERT_DEBUG(aStartType==EStartByDataType,User::Invariant());
			TPckg<TDataType> dataType_asDescriptor(*dataType);
			aMessage.ReadL(2,dataType_asDescriptor);
			}
		uid=AppForDataTypeL(*dataType, NULL);
		CleanupStack::PopAndDestroy(dataType);
		}
	const TThreadId threadId=StartDocumentL(*fileName,uid,(aStartType==ECreateByUid)? EApaCommandCreate: EApaCommandOpen);
	CleanupStack::PopAndDestroy(fileName);
	aMessage.WriteL(0, TPckgC<TThreadId>(threadId));
	}

TThreadId CApaAppListServSession::StartDocumentL(const TDesC& aFileName, TUid aUid, TApaCommand aCommand)
// Launch the document of aFileName with the app with Uid aUid
	{
	CApaAppData* app=NULL;
	CApaFileRecognizerType* type=NULL;
	TApaAppEntry entry;
	const TBool findApp=FindAppInList(app,entry,aUid);
	if (findApp)
	    {
	    if (app->RegistrationFileUsed())
			{
			TApaAppCapabilityBuf buf;
			app->Capability(buf);
	 		if (((buf().iEmbeddability == TApaAppCapability::EEmbeddableOnly) || (buf().iEmbeddability == TApaAppCapability::EEmbeddableUiNotStandAlone)) && !(buf().iAttributes & TApaAppCapability::EBuiltAsDll))
	              {
				   User::Leave(KErrNotSupported);		
                  }
	 		}
	    }

	if (!findApp || aUid.iUid==0)
		{
		// if we can't bind the type from the Mime type stuff then use the old scheme
		TRAP_IGNORE(type=FileRecognizer()->RecognizeFileL(aFileName));
		}
	else
		{
		if (findApp)
			{
			TRAP_IGNORE(type=FileRecognizer()->RecognizeFileL(entry.iFullName));
			}
		}
	if (!type)
		{
		User::Leave(KErrNotFound);
		}
	if (aFileName.Length()==0)
		{
		return type->RunL(EApaCommandRun,NULL,NULL);
		}
	return type->RunL(aCommand,&aFileName,NULL);
	}

void CApaAppListServSession::GetExecutableNameGivenDocumentL(const RMessage2& aMessage)
	{
	HBufC* const name=HBufC::NewLC(User::LeaveIfError(aMessage.GetDesLength(2)));
	{TPtr name_asWritable(name->Des());
	aMessage.ReadL(2, name_asWritable);}
	HBufC8* const buffer=HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesLength(3)));
	{TPtr8 buffer_asWritable(buffer->Des());
	aMessage.ReadL(3, buffer_asWritable);}
	const TDataType dataType(iServ.RecognizeDataL(*name, *buffer).iDataType);
	CleanupStack::PopAndDestroy(2, name);

	const TUid appUid(AppForDataTypeL(dataType, NULL));
	GetExecutableNameL(aMessage, appUid);
	}

void CApaAppListServSession::GetExecutableNameGivenDocumentPassedByFileHandleL(const RMessage2& aMessage)
	{
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.AdoptFromClient(aMessage, 2, 3));
	const TDataType dataType(iServ.RecognizeDataL(file, PreferredBufSize()).iDataType);
	CleanupStack::PopAndDestroy(&file);

	const TUid appUid(AppForDataTypeL(dataType, NULL));
	GetExecutableNameL(aMessage, appUid);
	}

void CApaAppListServSession::GetExecutableNameGivenDataTypeL(const RMessage2& aMessage)
	{
	if (sizeof(TDataType) != aMessage.GetDesLengthL(2))
		{
		//Leave with KErrArgument if client passes other than TDataType
		User::Leave(KErrArgument);
		}
	TDataType dataType;
	{TPckg<TDataType> dataType_asDescriptor(dataType);
	aMessage.ReadL(2, dataType_asDescriptor);}
	const TUid appUid(AppForDataTypeL(dataType, NULL));
	GetExecutableNameL(aMessage, appUid);
	}

void CApaAppListServSession::GetExecutableNameGivenAppUidL(const RMessage2& aMessage)
	{
	const TUid appUid(TUid::Uid(aMessage.Int2()));
	GetExecutableNameL(aMessage, appUid);
	}

void CApaAppListServSession::GetExecutableNameL(const RMessage2& aMessage, TUid aAppUid)
	{
	CApaAppData* appData=NULL;
	TApaAppEntry entry;
	if (!FindAppInList(appData, entry, aAppUid))
		{
		User::Leave(AppList().IsFirstScanComplete() ?
					KErrNotFound : RApaLsSession::EAppListInvalid);
		}
	const TDesC& executableName(entry.iFullName);
	if (executableName.Length() == 0)
		{
		User::Leave(KErrNotFound);
		}
	aMessage.WriteL(1, executableName); // the "logical" executable name - for non-native applications this is the name of the MIDlet, Python script, etc
	WriteNativeExecutableIfNonNativeAndPrepareForClientRetrievalOfOpaqueDataL(aMessage, 0, *appData);
	}

void CApaAppListServSession::GetNativeExecutableNameIfNonNativeL(const RMessage2& aMessage)
	{
	RBuf logicalExecutableName;
	logicalExecutableName.CreateL(User::LeaveIfError(aMessage.GetDesLength(1)));
	CleanupClosePushL(logicalExecutableName);
	aMessage.ReadL(1, logicalExecutableName);
	CApaAppData* const appData=AppList().AppDataByFileName(logicalExecutableName);
	if (appData!=NULL)
		{
		WriteNativeExecutableIfNonNativeAndPrepareForClientRetrievalOfOpaqueDataL(aMessage, 0, *appData);
		}
	CleanupStack::PopAndDestroy(&logicalExecutableName);
	}

void CApaAppListServSession::WriteNativeExecutableIfNonNativeAndPrepareForClientRetrievalOfOpaqueDataL(const RMessage2& aMessage, TInt aMessageSlotForNativeExecutable, const CApaAppData& aAppData)
	{
	HBufC8* opaqueData=NULL;
	const TPtrC8 opaqueData_asTPtrC8(aAppData.OpaqueData());
	const TInt lengthOfOpaqueData(opaqueData_asTPtrC8.Length());
	if (lengthOfOpaqueData>0)
		{
		opaqueData=opaqueData_asTPtrC8.AllocLC();
		}
	const TUid nonNativeApplicationType(aAppData.NonNativeApplicationType());
	if (nonNativeApplicationType!=TUid::Null())
		{
		aMessage.WriteL(aMessageSlotForNativeExecutable, iServ.NativeExecutableL(nonNativeApplicationType));
		}
	delete iOpaqueData_pendingDispatchToClient; // only done when the potentially leaving stuff has all succeeded
	iOpaqueData_pendingDispatchToClient=opaqueData; // want to do this, even if opaqueData is NULL
	if (opaqueData!=NULL)
		{
		CleanupStack::Pop(opaqueData);
		aMessage.Complete(lengthOfOpaqueData);
		}
	}

void CApaAppListServSession::GetOpaqueDataL(const RMessage2& aMessage)
	{
	if (iOpaqueData_pendingDispatchToClient==NULL)
		{
		User::Leave(KErrGeneral); // the protocol was broken: EAppListServGetOpaqueData can only be called immediately after one of the EAppListServGetExecutableNameGivenXxx or EAppListServGetNativeExecutableNameGivenXxx opcodes - see the client-side implementation of this protocol in RApaLsSession::GetOpaqueData (and the places that call it)
		}
	aMessage.WriteL(0, *iOpaqueData_pendingDispatchToClient);
	delete iOpaqueData_pendingDispatchToClient;
	iOpaqueData_pendingDispatchToClient=NULL;
	}

void CApaAppListServSession::GetAppInfoL(TUid aUid, TApaAppInfo& aInfo)
	{
	CApaAppData* app=NULL;
	TApaAppEntry entry;
	if (!FindAppInList(app,entry,aUid))
		{
		User::Leave(KErrNotFound);
		}
	aInfo.iUid = entry.iUidType[2];
	aInfo.iFullName = entry.iFullName;
	aInfo.iCaption = app->Caption();
	aInfo.iShortCaption = app->ShortCaption();
	}
	
void CApaAppListServSession::DoRecognizeUnpackLC(HBufC*& aName, HBufC8*& aBuffer, const RMessage2& aMessage)
	{
	__ASSERT_DEBUG(aName==NULL,User::Invariant());
	__ASSERT_DEBUG(aBuffer==NULL,User::Invariant());
	aName=HBufC::NewLC(User::LeaveIfError(aMessage.GetDesLength(1)));
	TPtr name(aName->Des());
	aMessage.ReadL(1, name);
	aBuffer=HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesLength(2)));
	TPtr8 buffer(aBuffer->Des());
	aMessage.ReadL(2, buffer);
	}

void CApaAppListServSession::RecognizeDataL(const RMessage2& aMessage)
// Recognize the data type of an object
	{
	HBufC* name=NULL;
	HBufC8* buffer=NULL;
	DoRecognizeUnpackLC(name,buffer,aMessage);

	const TDataRecognitionResult result = iServ.RecognizeDataL(*name, *buffer);

	CleanupStack::PopAndDestroy(2); // name & buffer
	aMessage.WriteL(0,TPckgC<TDataRecognitionResult>(result));
	}


void CApaAppListServSession::RecognizeFilesL(const RMessage2& aMessage)
	{
	// if there is an outstanding async. request, we even don't allow 
	// a synchronous request at the same time (due to the two required
	// server messages)
	if (iAsyncRecognitionActive)
		{
		User::Leave(KErrInUse);
		}

	_LIT8(KAllDataTypes,"*");

	// retrieve pathname
	HBufC* const path=HBufC::NewLC(User::LeaveIfError(aMessage.GetDesLength(0)));
	TPtr pathPtr(path->Des());
	aMessage.ReadL(0,pathPtr);

	// retrieve data type filter
	HBufC8* const dataType = HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesLength(2)));
	TPtr8 dataTypePtr(dataType->Des());
	aMessage.ReadL(2,dataTypePtr);

	delete iRecognitionResult;
	iRecognitionResult = NULL;

	if(dataType->Compare(KAllDataTypes) == 0)
		{
		iRecognitionResult = new (ELeave) CDirectoryRecognitionResult(path, NULL);
		CleanupStack::PopAndDestroy(dataType);
		}
	else
		{
		iRecognitionResult = new (ELeave) CDirectoryRecognitionResult(path,dataType);	
		CleanupStack::Pop(dataType);
		}

	CleanupStack::Pop(path);		// ownership transferred to CDirectoryRecognitionResult
	ASSERT(iFileRecognitionUtility);
	iFileRecognitionUtility->RecognizeSynchronouslyL(*iRecognitionResult);
	aMessage.WriteL(1,TPckgBuf<TUint>(iRecognitionResult->RequiredBufferSize()));
	}

void CApaAppListServSession::TransferRecognitionResultL(const RMessage2& aMessage)
	{
	if(iRecognitionResult == NULL)
		User::Leave(KErrNotReady);

	iAsyncRecognitionActive = EFalse;

	// if data is too big for buffer, tell client
	const TInt sizeOfBuffer=aMessage.Int2();	
	if(sizeOfBuffer < iRecognitionResult->RequiredBufferSize())
		User::Leave(KErrTooBig);

	// buffer is big enough -> write result to buffer
	CBufFlat* const buf=CBufFlat::NewL(iRecognitionResult->RequiredBufferSize());
	CleanupStack::PushL(buf);
	buf->ExpandL(0,iRecognitionResult->RequiredBufferSize());

	RBufWriteStream writeStream;
	writeStream.Open(*buf);
	iRecognitionResult->WriteToStreamL(writeStream);
	aMessage.WriteL(1,buf->Ptr(0));

	delete iRecognitionResult;
	iRecognitionResult = NULL;

	CleanupStack::PopAndDestroy(buf);
	}

void CApaAppListServSession::RecognizeFilesAsyncL(const RMessage2& aMessage)
	{
	if (iAsyncRecognitionActive)
		{
		User::Leave(KErrInUse);
		}
	else
		{
		_LIT8(KAllDataTypes,"*");

		HBufC* path=HBufC::NewLC(User::LeaveIfError(aMessage.GetDesLength(0)));
		TPtr pathPtr(path->Des());
		aMessage.ReadL(0,pathPtr);

		// retrieve data type filter
		HBufC8* dataType = 0;
		dataType=HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesLength(2)));
		TPtr8 dataTypePtr(dataType->Des());
		aMessage.ReadL(2,dataTypePtr);

		delete iRecognitionResult;
		iRecognitionResult = NULL;

		if(dataType->Compare(KAllDataTypes) == 0)
			{
			iRecognitionResult = new (ELeave) CDirectoryRecognitionResult(path,NULL);
			CleanupStack::PopAndDestroy(dataType);
			}
		else
			{
			iRecognitionResult = new (ELeave) CDirectoryRecognitionResult(path,dataType);
			CleanupStack::Pop(dataType);
			}

		CleanupStack::Pop(path);	// ownership transferred to CDirectoryRecognitionResult
		ASSERT(iFileRecognitionUtility);
		iFileRecognitionUtility->RecognizeAsynchronously(*iRecognitionResult,aMessage);
		iAsyncRecognitionActive = ETrue;
		}
	}

/** The function interrogates all available rule based plug-ins if apparc can launch application with 
given full name. It loops through all plug-ins until gets value different from 
CAppLaunchChecker::EAppLaunchIndifferent. 
The application will be launched if the return code is not equal to
CAppLaunchChecker::EAppLaunchDecline 
*/
void CApaAppListServSession::RuleBasedLaunchingL(const RMessage2& aMessage)
	{
	CApaScanningRuleBasedPlugIns* theRuleBasedPlugIns = iServ.RuleBasedPlugIns();
	if(!theRuleBasedPlugIns)
		{
		//we proceed with launching even if rule based plug-ins framework was not initialized
		aMessage.Complete(ETrue);
		return;
		}

	HBufC* theFullFileName=HBufC::NewLC(User::LeaveIfError(aMessage.GetDesLength(0)));
	TPtr theFullFileNamePtr(theFullFileName->Des());
	aMessage.ReadL(0, theFullFileNamePtr);
	TUid theUid = AppUidFromFullFileNameL(theFullFileNamePtr);
	CleanupStack::PopAndDestroy(theFullFileName); 

	CAppLaunchChecker::TAppLaunchCode theLaunchCode = CAppLaunchChecker::EAppLaunchIndifferent;	
	TInt theNumImp = theRuleBasedPlugIns->ImplementationCount();

	TApaTaskList theTaskList(iServ.WsSession());
	for(TInt ii = 0; ii < theNumImp; ii++)
		{
		CAppLaunchChecker* theLauncherChecker = (*theRuleBasedPlugIns)[ii];
		TRAP_IGNORE((theLaunchCode = theLauncherChecker->OkayToLaunchL(theUid, theTaskList)));
		if(theLaunchCode > CAppLaunchChecker::EAppLaunchIndifferent)
			break;
		}
	//writing the result
	TBool okayToLaunch = theLaunchCode != CAppLaunchChecker::EAppLaunchDecline;
	aMessage.Complete(okayToLaunch);
	}

/** 
@param aFullFileName This filename is parsed and the path is replaced with "\\sys\\bin\\". 
		And uses ".exe" if no other is provided in the filename passed.  If drive name is 
		present in the filename then it scans through the \\sys\\bin of that particular drive,
		otherwise it scans through the \\sys\\bin folders in all the avaliable drives.
@return Returns the Application Uid for the aFullFileName application.
*/
TUid CApaAppListServSession::AppUidFromFullFileNameL(const TDesC& aFullFileName) const
	{
	_LIT(KSysBin, "\\sys\\bin\\");
	_LIT(KFileExtension, ".EXE");
	
	RLoader loader;
	User::LeaveIfError(loader.Connect());
	CleanupClosePushL(loader);
	TPckgBuf<RLibrary::TInfo> dllInfo;
	TInt error = KErrNotFound;

	TParse parse;
	parse.Set(aFullFileName,NULL,NULL);
	if (parse.DrivePresent())
		{
		const TPtrC appDrive = parse.Drive();
		TBuf<KMaxFileName>fileName(appDrive);
		fileName.Append(KSysBin);
		User::LeaveIfError(parse.SetNoWild(fileName, &aFullFileName, &KFileExtension));
		error = loader.GetInfo(parse.FullName(), dllInfo);
		}
	else
		{
		// scan all drives
		User::LeaveIfError(parse.SetNoWild(KSysBin, &aFullFileName, &KFileExtension)); 
		TFileName tempFileName(parse.FullName());
		TDriveList driveList;
		User::LeaveIfError(iFs.DriveList(driveList));
		for (TInt driveNumber = EDriveY; driveNumber != KFinishedScanning; driveNumber = NextDriveToScan(driveNumber))
			{
			if (driveList[driveNumber]!=0)
				{
				User::LeaveIfError(parse.SetNoWild(TDriveUnit(driveNumber).Name(), &tempFileName, NULL));
				error = loader.GetInfo(parse.FullName(), dllInfo);
				if (error == KErrNone)
					{
					break;
					}
				}
			}
		}
	CleanupStack::PopAndDestroy(&loader);
	if (error == KErrNone)
		{
		return dllInfo().iUids[2];
		}

	//we can't use RFs::Entry if path refers to a system directory i.e. \\sys\\bin
	User::LeaveIfError(parse.SetNoWild(aFullFileName, NULL, NULL));
	if(parse.Path().FindF(KSysBin) == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	//the following valid for non-native applications
	TEntry entry;
	error = iFs.Entry(aFullFileName, entry);
	if (error != KErrNone)
		{
		// Since we cannot get the Uid of NonNative apps by passing filename to RFs::Entry
		CApaAppData* appData = AppList().AppDataByFileName(aFullFileName);
		if (appData)
			{
			TApaAppEntry appEntry = appData->AppEntry();
			return appEntry.iUidType[2];
			}
		User::Leave(KErrNotFound);
		}
			
	return entry.iType[2];
	}

TInt CApaAppListServSession::NextDriveToScan(TInt aCurrentDrive)
// applies the scanning order y:->a: then z:
	{
	if (aCurrentDrive == EDriveZ)
		{
		return KFinishedScanning;
		}
	else if (aCurrentDrive == 0)
		{
		return EDriveZ; // finally scan the last one
		}
	else if (aCurrentDrive > 0 && aCurrentDrive < KMaxDrives)
		{
		return aCurrentDrive - 1;
		}
	else
		{
		return KErrGeneral; // never gets here, but it wont compile otherwise
		}
	}

void CApaAppListServSession::CancelRecognizeFiles()
	{
	if (iAsyncRecognitionActive)
		{
		ASSERT(iFileRecognitionUtility);
		iFileRecognitionUtility->CancelRecognitionRequest();
		iAsyncRecognitionActive = EFalse;
		}
	}

void CApaAppListServSession::RecognizeDataPassedByFileHandleL(const RMessage2& aMessage)
// Recognize the data type of an object
	{
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.AdoptFromClient(aMessage, 1, 2));
	const TDataRecognitionResult result(iServ.RecognizeDataL(file, PreferredBufSize()));
	CleanupStack::PopAndDestroy(&file);
	aMessage.WriteL(0, TPckgC<TDataRecognitionResult>(result));
	}
	
void CApaAppListServSession::RecognizeSpecificDataL(const RMessage2& aMessage)
// Determine whether an object is of a specific data type
	{
	HBufC* name=NULL;
	HBufC8* buffer=NULL;
	DoRecognizeUnpackLC(name,buffer,aMessage);
	TDataType dataType;
	{TPckg<TDataType> dataType_asDescriptor(dataType);
	aMessage.ReadL(0, dataType_asDescriptor);}
	aMessage.Complete(iServ.RecognizeDataL(*name,*buffer,dataType));
	CleanupStack::PopAndDestroy(2); // name & buffer
	}

void CApaAppListServSession::RecognizeSpecificDataPassedByFileHandleL(const RMessage2& aMessage)
	{
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.AdoptFromClient(aMessage, 1, 2));
	TDataType dataType;
	{TPckg<TDataType> dataType_asDescriptor(dataType);
	aMessage.ReadL(0,dataType_asDescriptor);}
	aMessage.Complete(iServ.RecognizeDataL(file, PreferredBufSize(), dataType));
	CleanupStack::PopAndDestroy(&file);
	}

void CApaAppListServSession::InitListL(const RMessage2& aMessage, TAppListType aType)
// carries out initialisation prior to starting to pass a new list across
	{
	iAppListType = aType;
	iAppListScreenMode = aMessage.Int0();
	if (aType == EListFilteredEmbeddedApps)
		{
		TApaEmbeddabilityFilter filter;
		{TPckg<TApaEmbeddabilityFilter> filter_asDescriptor(filter);
		aMessage.ReadL(1,filter_asDescriptor);}
		iEmbeddabilityFilter = filter;
		}
	if (aType == EListCapabilityAttrFilteredApps)
		{
		iCapabilityAttrFilterMask = aMessage.Int1();
		iCapabilityAttrFilterValue = aMessage.Int2();
		}
	if (aType == EListServerApps)
		{
		iServiceUid = TUid::Uid(aMessage.Int1());
		}
	iApaAppInfoArray.ResetAndDestroy();
	iFlags|=EAppListPopulationPending;
	}

void CApaAppListServSession::EmbedCount(const RMessage2& aMessage) const
// writes back the number of embedded apps in the list
	{
	TInt count=0;
	const CApaAppList& list=AppList();
	CApaAppData* app = list.FirstApp();
	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableOnly);
	while (app)
		{
		if (!AppIsControlPanelItem(*app) && AppMatchesEmbeddabilityFilter(*app, filter))
			{
			count++;
			}
		app = list.NextApp(app);
		}
	aMessage.Complete(count);
	}

void CApaAppListServSession::AppCount(const RMessage2& aMessage) const
// writes back the number of apps in the list
	{
	TInt count=0;
	const CApaAppList& list=AppList();
	CApaAppData* app = list.FirstApp();
	while (app)
		{
		if (!AppIsControlPanelItem(*app))
			{
			count++;
			}
		app = list.NextApp(app);
		}
	aMessage.Complete(count);
	}

void CApaAppListServSession::GetNextAppL(const RMessage2& aMessage)
	{
	if (iFlags&EAppListPopulationPending)
		{
		const CApaAppList& list=AppList();
		if (!(list.IsFirstScanComplete()))
			{
			User::Leave(KErrCorrupt);
			}

		iApaAppInfoArray.ResetAndDestroy();
		for (CApaAppData* appData = list.FirstApp(iAppListScreenMode); appData != NULL; appData = list.NextApp(appData, iAppListScreenMode))
			{
			if (iAppListType==EListFilteredEmbeddedApps && (AppIsControlPanelItem(*appData) || !AppMatchesEmbeddabilityFilter(*appData, iEmbeddabilityFilter)))
				{
				continue;
				}
			if (iAppListType==EListCapabilityAttrFilteredApps && !AppMatchesCapabilityAttrFilter(*appData))
				{
				continue;
				}
			if (iAppListType==EListServerApps && !appData->ImplementsService(iServiceUid))
				{
				continue;
				}
			CApaAppInfo* apaAppInfo= new (ELeave)CApaAppInfo();
			CleanupStack::PushL(apaAppInfo);
			apaAppInfo->SetCaptionL(appData->Caption());
			apaAppInfo->SetShortCaptionL(appData->ShortCaption());
			apaAppInfo->SetFullNameL(appData->AppEntry().iFullName);
			apaAppInfo->SetUid(appData->AppEntry().iUidType[2]);
			User::LeaveIfError(iApaAppInfoArray.Append(apaAppInfo));
			CleanupStack::Pop(apaAppInfo);
			}
		iFlags&=~EAppListPopulationPending;
		}

	if (iApaAppInfoArray.Count()==0)
		{
		User::Leave(KErrNotFound);
		}
	TApaAppInfo* info=new(ELeave)TApaAppInfo; 
	CleanupStack::PushL(info);
	CApaAppInfo* apaAppInfo = iApaAppInfoArray[0];
	info->iUid = apaAppInfo->Uid();
	info->iFullName = apaAppInfo->FullName();
	info->iCaption = apaAppInfo->Caption();
	// Get the length of the target descriptor
	TInt targetLen=aMessage.GetDesMaxLength(1);
	if (targetLen==KApaAppInfoDesMaxLength)
		{
		info->iShortCaption = apaAppInfo->ShortCaption();
		}
	//
	iApaAppInfoArray.Remove(0);
	delete apaAppInfo;
	TPckgC<TApaAppInfo> infoPk(*info);
	if (targetLen<KApaAppInfoDesMaxLength)
		{
		infoPk.Set(infoPk.Left(_FOFF(TApaAppInfo,iShortCaption))); // reduce the length of infoPk to the 7.0 size of TApaAppInfo
		}
	aMessage.WriteL(1,infoPk);
	CleanupStack::PopAndDestroy(info);
	}

TBool CApaAppListServSession::AppMatchesEmbeddabilityFilter(const CApaAppData& aAppData, const TApaEmbeddabilityFilter& aEmbeddabilityFilter) const
// returns True if aAppData's embeddability matches the filter set by InitListL
	{
	TApaAppCapabilityBuf capabilityBuf;
	aAppData.Capability(capabilityBuf);
	if (aEmbeddabilityFilter.MatchesEmbeddability(capabilityBuf().iEmbeddability))
		{
		return ETrue;
		}
	return EFalse;
	}

TBool CApaAppListServSession::AppMatchesCapabilityAttrFilter(const CApaAppData& aAppData) const
// returns True if aAppData's capability attributes match the filter set by InitListL
	{
	TApaAppCapabilityBuf capabilityBuf;
	aAppData.Capability(capabilityBuf);
	if ((capabilityBuf().iAttributes & iCapabilityAttrFilterMask) == (iCapabilityAttrFilterValue & iCapabilityAttrFilterMask))
		{
		return ETrue;
		}
	return EFalse;
	}

TBool CApaAppListServSession::AppIsControlPanelItem(const CApaAppData& aAppData)
// returns True if aAppData represents a control panel app
	{
	TApaAppCapabilityBuf capabilityBuf;
	aAppData.Capability(capabilityBuf);
	if (capabilityBuf().iAttributes & TApaAppCapability::EControlPanelItem)
		{
		return ETrue;
		}
	return EFalse;
	}

TBool CApaAppListServSession::FindAppInList(CApaAppData*& aApp,TApaAppEntry& aEntry,TUid aAppUid)
// locate app in list, return EFalse if it isn't present
// search is regardless of screen mode.
	{
	const CApaAppList& list=AppList();
	aApp = list.AppDataByUid(aAppUid);
	TBool found=EFalse;
	if (aApp)
		{
		found = ETrue;
		aEntry = aApp->AppEntry();
		}
	
	TBool appPendingOnLangChange = found && list.IsLanguageChangePending() && aApp->IsPending();	
		
	if ((!found || appPendingOnLangChange) && !list.IsIdleUpdateComplete())
		{
		// 1. app wasn't found, but an app scan is currently in progress,
		// so try to find and add the specific app we're looking for to the list
		
		// 2. On language change event, current app scan could not yet update the found app, 
		// so try to update the specific app we're looking for, in the list.
		CApaAppData* app = NULL;
		if(aAppUid!=KNullUid)
			{
			TRAPD(ret, app = FindSpecificAppL(aAppUid));
			if (ret == KErrNone && app)
				{
				// app has been found and added to the app list
				aApp = app;
				aEntry = aApp->AppEntry();
				found = ETrue;
				}
 			}
 		}
	return found;
	}

CApaAppData* CApaAppListServSession::FindSpecificAppL(TUid aAppUid)
	{
	//Scans the drives and folder lists for the specific app
	CApaAppRegFinder* regFinder = CApaAppRegFinder::NewLC(iFs);
	CApaAppData* app = iServ.AppList().FindAndAddSpecificAppL(regFinder, aAppUid);
	CleanupStack::PopAndDestroy(regFinder);
	return app;
	}

void CApaAppListServSession::GetAppInfoL(const RMessage2& aMessage)
	{
	// get UID of required app
	const TUid uid=TUid::Uid(aMessage.Int0());
	TApaAppInfo* info=new(ELeave) TApaAppInfo; // on heap to avoid running out of stack
	CleanupStack::PushL(info);
	GetAppInfoL(uid, *info);
	TPckgC<TApaAppInfo> infoPk(*info);	
	aMessage.WriteL(1,infoPk);
	CleanupStack::PopAndDestroy(info);
	}

void CApaAppListServSession::GetAppCapabilityL(const RMessage2& aMessage)
	{	  								  
	const TUid uid=TUid::Uid(aMessage.Int1());
	CApaAppData* app=NULL;
	TApaAppEntry entry;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}
	TInt targetLen=aMessage.GetDesMaxLength(0);
	HBufC8* buf=HBufC8::NewLC(User::LeaveIfError(targetLen));
	TPtr8 ptr=buf->Des();
	app->Capability(ptr);
	ptr.SetLength(targetLen);
	aMessage.WriteL(0,*buf);
	CleanupStack::PopAndDestroy(buf); 
	}

void CApaAppListServSession::GetDefaultScreenNumberL(const RMessage2& aMessage)
	{
	const TUid uid=TUid::Uid(aMessage.Int0());
	CApaAppData* app=NULL;
	TApaAppEntry entry;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}
	aMessage.Complete(app->DefaultScreenNumber());
	}

void CApaAppListServSession::StartAppL(const RMessage2& aMessage, TBool aReturnThreadId)
	{
	CApaCommandLine* commandLine=CApaCommandLine::NewLC();
	commandLine->ConstructCmdLineFromMessageL(aMessage);

	CApaFileRecognizerType* type=NULL;
	TRAP_IGNORE(type=FileRecognizer()->RecognizeFileL(commandLine->ExecutableName()));
	if (type==NULL)
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PushL(TCleanupItem(&NullifyAppCommandLinePointer,&iServ));
	iServ.SetAppCmdLine(commandLine);
	TPtrC fileName=commandLine->DocumentName();
	TPtrC8 tailEnd=commandLine->TailEnd();
	const TThreadId threadId(type->RunL(commandLine->Command(),(fileName.Length()?&fileName:NULL),(tailEnd.Length()?&tailEnd:NULL))); // pass in NULL for components that are not present
	CleanupStack::PopAndDestroy(&iServ); // calls iServ.SetAppCmdLine(NULL);
	CleanupStack::PopAndDestroy(commandLine);

	if (aReturnThreadId)
		{
		aMessage.WriteL(CApaCommandLine::EIpcFirstFreeSlot,TPckgC<TThreadId>(threadId));
		}
	}

void CApaAppListServSession::SetNotify(const RMessage2& aMessage)
	{
	if (!iNotifyMessage.IsNull())
		{
		aMessage.Panic(KApaPanicCli,ENotifierAlreadyPresent);
		}
	else
		{
		const TBool completeImmediatelyIfNoScanImpendingOrInProgress=aMessage.Int0();
		if ((!completeImmediatelyIfNoScanImpendingOrInProgress) ||
			iServ.AppFsMonitor().AnyNotificationImpending() ||
			AppList().AppScanInProgress())
			{
			iNotifyMessage=aMessage;
			}
		else
			{
			aMessage.Complete(KErrNone);
			}
		}
	}

void CApaAppListServSession::CancelNotify()
	{
	NotifyClients(KErrCancel);
	}

void CApaAppListServSession::NotifyClients(TInt aReason)
	{
	if (!iNotifyMessage.IsNull())
		{
		iNotifyMessage.Complete(aReason);
		}
	//Notify client for scan complete.
	NotifyScanComplete();
	}

void CApaAppListServSession::AppInfoProvidedByRegistrationFileL(const RMessage2& aMessage)
	{
	// get UID of required app
	const TUid uid=TUid::Uid(aMessage.Int0());

	// locate app in list
	CApaAppData* app=NULL;
	TApaAppEntry entry;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}

	const TBool registrationFileUsed = app->RegistrationFileUsed();
	TPckgC<TBool> pckg(registrationFileUsed);
	aMessage.WriteL(1, pckg);
	}

void CApaAppListServSession::IconFileNameL(const RMessage2& aMessage)
	{
	// get UID of required app
	const TUid uid=TUid::Uid(aMessage.Int0());

	// locate app in list
	CApaAppData* app=NULL;
	TApaAppEntry entry;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}

	if (!app->RegistrationFileUsed())
		{
		User::Leave(KErrNotSupported);
		}
	else
		{
		TPtrC iconFileName(app->IconFileName());
		if (iconFileName.Length() == 0)
			{
			User::Leave(KErrNotFound);
			}
		else
			{
			TFileName fileName = iconFileName;
			TPckgC<TFileName> pckg(fileName);
			aMessage.WriteL(1, pckg);
			}
		}
	}

void CApaAppListServSession::ViewIconFileNameL(const RMessage2& aMessage)
	{
	// get UID of required app
	const TUid uid=TUid::Uid(aMessage.Int0());
	// get UID of required view
	const TUid viewUid=TUid::Uid(aMessage.Int1());

	TPtrC viewIconFileName;

	// locate app in list
	CApaAppData* app=NULL;
	TApaAppEntry entry;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}

	if (!app->RegistrationFileUsed())
		{
		User::Leave(KErrNotSupported);
		}
	else
		{
		const CArrayPtr<CApaAppViewData>& viewDataArray=*app->Views();
		const TInt count=viewDataArray.Count();
		for (TInt ii=0; ii<count; ii++)
			{
			const CApaAppViewData& appViewData=*viewDataArray[ii];
			if (appViewData.Uid()==viewUid)
				{
				viewIconFileName.Set(appViewData.IconFileName());
				break;
				}
			}
		if (viewIconFileName.Length() == 0)
			{
			User::Leave(KErrNotFound);
			}
		else
			{
			TFileName fileName = viewIconFileName;
			TPckgC<TFileName> pckg(fileName);
			aMessage.WriteL(2, pckg);
			}
		}
	}

void CApaAppListServSession::GetAppServicesL(const RMessage2& aMessage)
	{
	const TInt initialBufSize = aMessage.Int2();
	if (initialBufSize)
		{
		delete iBuffer;
		iBuffer = NULL;
		iBuffer = GetServiceBufferL(aMessage);
		if (iBuffer->Size() > initialBufSize)
			{
			// default buffer provided by client is too small, ask client to provide buffer of correct size
			User::Leave(iBuffer->Ptr(0).Size());
			}
		}
	__ASSERT_ALWAYS(iBuffer, aMessage.Panic(KApaPanicCli,EClientBadRequest));
	aMessage.WriteL(3, iBuffer->Ptr(0));
	delete iBuffer;
	iBuffer = NULL;
	}

CBufBase* CApaAppListServSession::GetServiceBufferL(const RMessage2& aMessage) const
	{
	CBufBase* buffer = NULL;
	const TUid uid = TUid::Uid(aMessage.Int0());
	switch (aMessage.Function())
		{
	case EAppListServGetAppServices:
		buffer = AppList().ServiceArrayBufferL(uid);
		break;
	case EAppListServGetServiceImplementations:
		buffer = AppList().ServiceImplArrayBufferL(uid);
		break;
	case EAppListServGetServiceImplementationsDataType:
		{
		TDataType dataType;
		TPckg<TDataType> dataType_asDescriptor(dataType);
		aMessage.ReadL(1,dataType_asDescriptor);
		buffer = AppList().ServiceImplArrayBufferL(uid, dataType);
		}
		break;
	case EAppListServGetAppServiceUids:
		buffer = AppList().ServiceUidBufferL(uid);
		break;
	case EAppListServGetAppServiceOpaqueData:
		buffer = AppList().ServiceOpaqueDataBufferL(uid, TUid::Uid(aMessage.Int1()));
		break;
	default:
		aMessage.Panic(KApaPanicCli,EClientBadRequest);
		User::Leave(KErrNotSupported);
		break;
		}
	return buffer;
	}

void CApaAppListServSession::NullifyAppCommandLinePointer(TAny* aServer)
	{
	static_cast<CApaAppListServer*>(aServer)->SetAppCmdLine(NULL);
	}

CApaAppListServSession::CApaAppInfo::CApaAppInfo()
	:iUid(KNullUid), iCaption(NULL), iShortCaption(NULL), iFullName(NULL)
	{
	}

CApaAppListServSession::CApaAppInfo::~CApaAppInfo()
	{
	delete iCaption;
	delete iShortCaption;
	delete iFullName;
	}

void CApaAppListServSession::CApaAppInfo::SetUid(const TUid aUid)
	{
	iUid=aUid;
	}

void CApaAppListServSession::CApaAppInfo::SetCaptionL(const TDesC& aCaption)
	{
	HBufC* caption = aCaption.AllocL();
	delete iCaption;
	iCaption = caption;
	}

void CApaAppListServSession::CApaAppInfo::SetShortCaptionL(const TDesC& aShortCaption)
	{
	HBufC* shortCaption = aShortCaption.AllocL();
	delete iShortCaption;
	iShortCaption = shortCaption;
	}

void CApaAppListServSession::CApaAppInfo::SetFullNameL(const TDesC& aFullName)
	{
	HBufC* fullName = aFullName.AllocL();
	delete iFullName;
	iFullName = fullName;
	}

void CApaAppListServSession::RegisterListPopulationCompleteObserver(const RMessage2& aMessage)
	{
	if (!iCompletionOfListPopulationObserverMsg.IsNull())
		{
		aMessage.Panic(KApaPanicCli,EObserverAlreadyPresent);
		}
	else
		{
		if(AppList().IsFirstScanComplete())
			{
			aMessage.Complete(KErrNone);
			}
		else
			{
			iCompletionOfListPopulationObserverMsg=aMessage;
			}
		}
	}

void CApaAppListServSession::NotifyClientForCompletionOfListPopulation()
	{
	if (!iCompletionOfListPopulationObserverMsg.IsNull())
		{
		iCompletionOfListPopulationObserverMsg.Complete(KErrNone);
		}
	} //lint !e1762 Suppress member function could be made const


void CApaAppListServSession::CancelListPopulationCompleteObserver()
	{
	if (!iCompletionOfListPopulationObserverMsg.IsNull())
		{
		iCompletionOfListPopulationObserverMsg.Complete(KErrCancel);
		}
	} //lint !e1762 Suppress member function could be made const
	
void CApaAppListServSession::NotifyClientOfDataMappingChange()
	{
	if (!iMessage_NotifyOnDataMappingChange.IsNull())
		{
		iMessage_NotifyOnDataMappingChange.Complete(KErrNone);
		}
	} //lint !e1762 Suppress member function could be made const

void CApaAppListServSession::MatchesSecurityPolicyL(const RMessage2& aMessage)
	{
	const TUid appUid=TUid::Uid(aMessage.Int0());
	TApaAppInfo* const appInfo=new(ELeave) TApaAppInfo;
	CleanupStack::PushL(appInfo);
	//Get the app info for the given App uid.
	GetAppInfoL(appUid, *appInfo);
	const TPtrC executableName(appInfo->iFullName);
	//Create a process only if the executable name is of non zero length
	if (executableName.Length() != 0)
		{
		RProcess process;
		TInt result = process.Create(executableName, KNullDesC);
		//Proceed with checking the security policy if the process is created normally
		if (result == KErrNone)
			{
			TPckgBuf<TSecurityPolicy> securityPolicy;
			aMessage.ReadL(1,securityPolicy);
			aMessage.Complete(securityPolicy().CheckPolicy(process));
			process.Close();
			}
		else
			{
			aMessage.Complete(result);
			}
		}
	else
		{
		aMessage.Complete(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(appInfo);
	}
	
void CApaAppListServSession::SetAppShortCaptionL(const RMessage2& aMessage)
	{
	const TUid uid=TUid::Uid(aMessage.Int0());
	CApaAppData* app=NULL;
	TApaAppEntry entry;
	if (!FindAppInList(app,entry,uid))
		{
		User::Leave(KErrNotFound);
		}

	TInt length=aMessage.GetDesLength(1);
	if(length < 0)
		{
		User::Leave(length);
		}

	HBufC* const shortCaption=HBufC::NewLC(length);
	TPtr captionPtr(shortCaption->Des());
	aMessage.ReadL(1, captionPtr);

	TLanguage appLanguage=TLanguage(aMessage.Int2());
	CCustomAppInfoData* customAppInfo=CCustomAppInfoData::NewL(uid, appLanguage, *shortCaption);
	CleanupStack::PushL(customAppInfo);
	iServ.AddCustomAppInfoInListL(customAppInfo);
	CleanupStack::Pop(customAppInfo);
	if(app->ApplicationLanguage() == appLanguage)
		{
		app->SetShortCaptionL(*shortCaption);
		}
	else if(appLanguage==ELangNone)
		{
		iServ.UpdateAppListByShortCaptionL(); 
		}
	CleanupStack::PopAndDestroy(shortCaption);
	}

void CApaAppListServSession::NotifyScanComplete()
	{
	//See if the session is intrested in scan complete notification.
	if (!iNotifyOnScanCompleteMsg.IsNull())
		{
		iNotifyOnScanCompleteMsg.Complete(KErrNone);
		}
	iNonNativeApplicationsManager->NotifyScanComplete();
	} //lint !e1762 Suppress member function could be made const
	
// TSizeArrayItemWriter

TInt TSizeArrayItemWriter::ArrayItemCount() const
	{
	return iArray.Count();
	}

TInt TSizeArrayItemWriter::ArrayItemSize() const
	{
	return sizeof(TSize);
	}

void TSizeArrayItemWriter::WriteArrayItemL(RWriteStream& aWriteStream,TInt aIndex) const
	{
	const TSize& size = iArray[aIndex];
	aWriteStream.WriteUint32L(size.iWidth);
	aWriteStream.WriteUint32L(size.iHeight);
	}

// TViewDataArrayItemWriter

TInt TViewDataArrayItemWriter::ArrayItemCount() const
	{
	return iArray.Count();
	}

TInt TViewDataArrayItemWriter::ArrayItemSize() const
	{
	return sizeof(TApaAppViewInfo);
	}

void TViewDataArrayItemWriter::WriteArrayItemL(RWriteStream& aWriteStream,TInt aIndex) const
	{
	const CApaAppViewData& appViewData=*(iArray[aIndex]);
	aWriteStream << TApaAppViewInfo(appViewData.Uid(),appViewData.Caption(),appViewData.ScreenMode());
	}

// TDesCArrayItemWriter

TInt TDesCArrayItemWriter::ArrayItemCount() const
	{
	return iArray.Count();
	}

TInt TDesCArrayItemWriter::ArrayItemSize() const
	{
	return sizeof(TFileName);
	}

void TDesCArrayItemWriter::WriteArrayItemL(RWriteStream& aWriteStream,TInt aIndex) const
	{
	aWriteStream << iArray[aIndex];
	}
