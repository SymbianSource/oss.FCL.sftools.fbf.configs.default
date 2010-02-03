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

#if !defined(__APSSES_H__)
#define __APSSES_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__APSSERV_H__)
#include <APSSERV.H>
#endif
#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif
#if !defined(__APAID_H__)
#include <apaid.h>
#endif
#if !defined(__APSCLSV_H__)
#include "APSCLSV.H"
#endif

#include "APGAPLST.H"
#include "APSRECUTIL.H"

// classes defined
class CApaAppListServSession;
//
// classes referenced
class CApaAppList;
class CApaAppData;
class CApaFileRecognizer;
class CApaScanningDataRecognizer;
class TDataType;
class CTypeStoreManager;
class CApsNonNativeApplicationsManager;
//
class MArrayItemWriter;

class CFileRecognitionUtility;

class CApaAppListServSession : public CSession2
	{
public:
	// construct/destruct
	static CApaAppListServSession* NewL(CApaAppListServer& aServer, RFs& aFs);
	~CApaAppListServSession();
	//service request
	virtual void ServiceL(const RMessage2 &aMessage);
	void NotifyClients(TInt aReason);
	void NotifyClientForCompletionOfListPopulation();
	void NotifyClientOfDataMappingChange();
	void NotifyScanComplete();
private:
	CApaAppListServSession(CApaAppListServer& aServer, RFs& aFs);
	void ConstructL();
	void NotifyOnDataMappingChange(const RMessage2& aMessage);
	void CancelNotifyOnDataMappingChange();
private:
	enum TAppListType {
		ENoList,
		EListFilteredEmbeddedApps,
		EListCapabilityAttrFilteredApps,
		EListServerApps
		};
	enum TAppListDocumentStart {
		EStart,
		EStartByDataType,
		EStartByUid,
		ECreateByUid
		};
private:
	class CApaAppInfo;
private:
	void AppCount(const RMessage2& aMessage) const;
	void EmbedCount(const RMessage2& aMessage) const;
	void GetNextAppL(const RMessage2& aMessage);
	void InitListL(const RMessage2& aMessage, TAppListType aType);
	void GetAppInfoL(const RMessage2& aMessage);
	void GetAppInfoL(TUid aUid, TApaAppInfo& aInfo);
	void GetAppCapabilityL(const RMessage2& aMessage);
	void GetDefaultScreenNumberL(const RMessage2& aMessage);
	void StartAppL(const RMessage2& aMessage, TBool aReturnThreadId);
	//
	// Data recognition stuff
	void RecognizeDataL(const RMessage2& aMessage);
	void RecognizeDataPassedByFileHandleL(const RMessage2& aMessage);
	void RecognizeSpecificDataL(const RMessage2& aMessage);
	void RecognizeSpecificDataPassedByFileHandleL(const RMessage2& aMessage);
	static void DoRecognizeUnpackLC(HBufC*& aName, HBufC8*& aBuffer, const RMessage2& aMessage);
	//
	void AppForDataTypeL(const RMessage2& aMessage);
	void StartDocumentL(const RMessage2& aMessage,TAppListDocumentStart aStartType);
	TThreadId StartDocumentL(const TDesC& aFileName, TUid aUid, TApaCommand aCommand);
	void GetExecutableNameGivenDocumentL(const RMessage2& aMessage);
	void GetExecutableNameGivenDocumentPassedByFileHandleL(const RMessage2& aMessage);
	void GetExecutableNameGivenDataTypeL(const RMessage2& aMessage);
	void GetExecutableNameGivenAppUidL(const RMessage2& aMessage);
	void GetExecutableNameL(const RMessage2& aMessage, TUid aAppUid);
	void GetNativeExecutableNameIfNonNativeL(const RMessage2& aMessage);
	void WriteNativeExecutableIfNonNativeAndPrepareForClientRetrievalOfOpaqueDataL(const RMessage2& aMessage, TInt aMessageSlotForNativeExecutable, const CApaAppData& aAppData);
	void GetOpaqueDataL(const RMessage2& aMessage);
	TUid AppForDataTypeL(const TDataType& aDataType, const TUid* aServiceUid);
	void AppForDocumentL(const RMessage2& aMessage, const TUid* aServiceUid);
	//
	void IconForAppL(const RMessage2& aMessage);
	//
	void IconForAppBySizeL(const RMessage2& aMessage);
	void AppIconSizesL(const RMessage2& aMessage);
	void IconFileHandleForAppL(const RMessage2& aMessage);
	//
	void AppViewsL(const RMessage2& aMessage);
	void IconForViewBySizeL(const RMessage2& aMessage);
	void AppFileOwnershipInfoL(const RMessage2& aMessage);
	//
	void NumberOfOwnDefinedIconsL(const RMessage2& aMessage);
	//
	void AppInfoProvidedByRegistrationFileL(const RMessage2& aMessage);
	void IconFileNameL(const RMessage2& aMessage);
	void ViewIconFileNameL(const RMessage2& aMessage);
	//
	void RecognizeFilesL(const RMessage2& aMessage);
	void TransferRecognitionResultL(const RMessage2& aMessage);
	void RecognizeFilesAsyncL(const RMessage2& aMessage);
	void RuleBasedLaunchingL(const RMessage2& aMessage);
	void CancelRecognizeFiles();
	//
	void InsertDataMappingL(const RMessage2& aMessage);
	void DeleteDataMappingL(const RMessage2& aMessage);
	void GetAppByDataTypeL(const RMessage2& aMessage) const;
	//
	void GetAppServicesL(const RMessage2& aMessage);
	CBufBase* GetServiceBufferL(const RMessage2& aMessage) const;
	void AppForDataTypeAndServiceL(const RMessage2& aMessage);
	void AppForDocumentPassedByFileHandleL(const RMessage2& aMessage, const TUid* aServiceUid);

	void RegisterNonNativeApplicationTypeL(const RMessage2& aMessage);
	void DeregisterNonNativeApplicationTypeL(const RMessage2& aMessage);

	void GetAppTypeL(const RMessage2& aMessage);
	void ForceRegistrationL(const RMessage2& aMessage);
	inline const CApaAppList& AppList() const;
	inline CApaFileRecognizer* FileRecognizer() const;
	CApaAppData& FindAppInListL(TUid aUid);
	TBool FindAppInList(CApaAppData*& aApp,TApaAppEntry& aEntry,TUid aAppUid);
	TBool AppMatchesEmbeddabilityFilter(const CApaAppData& aAppData, const TApaEmbeddabilityFilter& aEmbeddabilityFilter) const;
	TBool AppMatchesCapabilityAttrFilter(const CApaAppData& aAppData) const;
	static TBool AppIsControlPanelItem(const CApaAppData& aAppData);

	void GetConfidenceL(const RMessage2& aMessage);
	void SetConfidence(const RMessage2& aMessage);
	void GetBufSize(const RMessage2& aMessage);
	void SetBufSize(const RMessage2& aMessage);
	void GetDataTypesCountL(const RMessage2& aMessage);
	void GetDataTypesL(const RMessage2& aMessage);
	void SetNotify(const RMessage2& aMessage);
	void CancelNotify();
	//
	void SendArrayL(const MArrayItemWriter& aArrayItemWriter,const RMessage2& aMessage) const;
	//
	void ApplicationLanguageL(const RMessage2& aMessage);
	void RegisterListPopulationCompleteObserver(const RMessage2& aMessage);
	void CancelListPopulationCompleteObserver();
	void MatchesSecurityPolicyL(const RMessage2& aMessage);
	//
	void SetAppShortCaptionL(const RMessage2& aMessage);

	static void NullifyAppCommandLinePointer(TAny* aServer);
	TInt PreferredBufSize() const;
	CApaAppData* FindSpecificAppL(TUid aAppUid);
	TUid AppUidFromFullFileNameL(const TDesC& aFullFileName) const;
	static TInt NextDriveToScan(TInt aCurrentDrive);
	
private:
	enum
		{
		ENotifyPresent=0x01,
		EAppListPopulationPending=0x02
		};

private:
	CApaAppListServer& iServ;
	TInt iFlags;
	RFs& iFs;
	TInt iMaxBufSize;
	CBufBase* iBuffer;
	RMessage2 iNotifyMessage;
	RMessage2 iNotifyOnScanCompleteMsg;
	RPointerArray<CApaAppInfo> iApaAppInfoArray;	//contains the most recent "snapshot" of the applist taken by GetNextAppL.
	TAppListType iAppListType;
	TInt iAppListScreenMode;
	TApaEmbeddabilityFilter iEmbeddabilityFilter;
	TUint iCapabilityAttrFilterMask; // contains bit flags from TCapabilityAttribute
	TUint iCapabilityAttrFilterValue; // contains bit flags from TCapabilityAttribute
	TUid iServiceUid;
	RMessage2 iCompletionOfListPopulationObserverMsg;
	CDirectoryRecognitionResult* iRecognitionResult;
	CFileRecognitionUtility* iFileRecognitionUtility;
	TBool iAsyncRecognitionActive;
	HBufC8* iOpaqueData_pendingDispatchToClient;
	RMessagePtr2 iMessage_NotifyOnDataMappingChange;
	CApsNonNativeApplicationsManager* iNonNativeApplicationsManager;
	};

//
// inlines
//

inline const CApaAppList& CApaAppListServSession::AppList() const
	{ return ((CApaAppListServer*)Server())->AppList(); } 

inline CApaFileRecognizer* CApaAppListServSession::FileRecognizer() const
	{ return ((CApaAppListServer*)Server())->FileRecognizer(); }

#endif
