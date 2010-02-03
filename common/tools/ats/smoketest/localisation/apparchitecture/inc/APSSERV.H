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

#if !defined(__APSSERV_H__)
#define __APSSERV_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif
#if !defined(__APMFNDR_H__)
#include <apmfndr.h>
#endif
#if !defined(__APRULEBASED_H__)
#include <aprulebased.h>
#endif
#if !defined(__W32STD_H__)
#include <w32std.h>
#endif


#include <apgaplst.h>

// classes defined
class CApaAppListServer;
class CCustomAppInfoData;
//
// classes referenced
class CApaCommandLine;
class CApaAppList;
class CApaAppData;
class CApaFileRecognizer;
class CApaAppRegFinder;
class CApaScanningFileRecognizer;
class CApaFsMonitor;
class CTypeStoreManager;
class MApaAppStarter;
class CApaEComMonitor;
class CBaBackupSessionWrapper;
class CApsRecognitionCache;
class CRecognitionResult;
class CApaAppUnInstallMonitor;
class CApaIconCaptionOverrides;
class CApaIconCaptionCenrepObserver;
//

// server name

/**
KAppListServerName 
@internalAll
*/
_LIT(KAppListServerName,"!AppListServer");

class CApaAppListServer : public CPolicyServer, public MApaAppListObserver, public MBackupOperationObserver
/**
CApaAppListServer 
@internalAll 
*/
	{
public:
	inline static CApaAppListServer* NewL(MApaAppStarter& aAppStarter) {return NewL(&aAppStarter);}
	IMPORT_C static CApaAppListServer* NewL(MApaAppStarter* aAppStarter);
	IMPORT_C static CApaAppListServer* Self();
	inline CApaAppList& AppList();
	CApaFileRecognizer* FileRecognizer();
	inline CApaDataRecognizer* DataRecognizer();
	inline CApaCommandLine* AppCmdLine();
	inline void SetAppCmdLine(CApaCommandLine* aAppCmdLine);
	inline CApaScanningRuleBasedPlugIns* RuleBasedPlugIns();
	inline RWsSession& WsSession();
	void RegisterNonNativeApplicationTypeL(TUid aApplicationType, const TDesC& aNativeExecutable);
	void DeregisterNonNativeApplicationTypeL(TUid aApplicationType);
	TPtrC NativeExecutableL(TUid aNonNativeApplicationType) const;
	inline const CApaFsMonitor& AppFsMonitor() const {return *iAppFsMonitor;}
	void UpdateApps();
	IMPORT_C TCallBack RescanCallBack();
	CRecognitionResult* RecognizeDataAsCRecognitionResultL(RFile& aFile, TInt aPreferredBufSize);
	TDataRecognitionResult RecognizeDataL(const TDesC& aName, const TDesC8& aBuffer);
	TDataRecognitionResult RecognizeDataL(RFile& aFile, TInt aPreferredBufSize);
	TBool RecognizeDataL(const TDesC& aName, const TDesC8& aBuffer, const TDataType& aDataType);
	TBool RecognizeDataL(RFile& aFile, TInt aPreferredBufSize, const TDataType& aDataType);
	TInt DataRecognizerPreferredBufSizeL();
	void DataTypeL(CDataTypeArray& aArray);

	
	void GetAppByDataType(const TDataType& aDataType, TUid& aUid) const;
	void GetAppByDataType(const TDataType& aDataType, TUid aServiceUid, TUid& aUid) const;
	TBool InsertAndStoreIfHigherL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid);
	void InsertAndStoreDataMappingL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid, TUid aServiceUid);
	void DeleteAndStoreDataMappingL(const TDataType& aDataType, TUid aServiceUid);

    IMPORT_C void HandleStartUninstallEvent();
	IMPORT_C void HandleEndUninstallEventL();
#ifdef _DEBUG
	void FlushRecognitionCache();
	void SetLoadRecognizersOnDemandL(TBool aLoadRecognizersOnDemand);
	void PerformOutstandingRecognizerUnloading();
#endif //_DEBUG
public: // from MApaAppListObserver
	void NotifyUpdate(TInt aReason);
	void InitialListPopulationComplete();
    // from MBackupOperationObserver
	void HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes);
	//
	IMPORT_C ~CApaAppListServer();
	TBool NonNativeRecovery() const;
	void SetNonNativeRecovery(TBool aValue);
	void AddCustomAppInfoInListL(CCustomAppInfoData* aCustomAppInfo);
	TBool LoadMbmIconsOnDemand() const;
	void UpdateAppListByShortCaptionL();
private:
	CApaAppListServer(TInt aPriority);
	void ConstructL(MApaAppStarter* aAppStarter);
	virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	static TInt AppFsNotifyWithForcedRegistrationsResetCallBack(TAny* aPtr);
	static TInt AppFsNotifyCallBack(TAny* aPtr);
	static TInt PlugInNotifyCallBack(TAny* aPtr);
	static TInt TypeStoreNotifyCallback(TAny* aPtr);
	void UpdatePlugIns();
	void UpdateTypeStore();
	void DoUpdateTypeStoreL();
	void InternalizeNonNativeApplicationTypeArrayL();
	void ExternalizeNonNativeApplicationTypeArrayL(TInt aIndexToIgnore=-1) const;
	static void DeleteLastNonNativeApplicationType(TAny* aThis);
	void NotifyScanComplete();
	void DeleteCustomAppInfoList();
	void UpdateAppListByIconCaptionOverridesL();
	void LoadRecognizersLC();
	void LoadRecognizersL();
	static void RecognizerCleanup(TAny* aAny);
	TInt UnloadRecognizers();
	static TInt DoUnloadRecognizersCallback(TAny* aAny);
	TInt DoUnloadRecognizers();
	TBool CachedRecognitionResult(const TParseBase& aParser, TDataRecognitionResult& aResult) const;
	CRecognitionResult* CachedRecognitionResult(const RFile& aFile, const TParseBase& aParser) const;
	void CacheRecognitionResultL(const TParseBase& aParser, const TDataRecognitionResult& aResult);
	void CacheRecognitionResultL(const RFile& aFile, const TParseBase& aParser, const TDataRecognitionResult& aResult);
	void ConstructPathsToMonitorL();
private:
	enum
		{
		EOwnsFileRecognizer=0x01,
		EOwnsAppList=0x02
		};
	struct SNonNativeApplicationType
		{
		TUid iTypeUid;
		HBufC* iNativeExecutable;
		};
private:
	RFs iFs;
	TInt iRecognizerUsageCount;
	CApaAppList* iAppList;
	CApaScanningFileRecognizer* iScanningFileRecognizer;
	CPeriodic* iRecognizerUnloadTimer;
	CApaScanningDataRecognizer* iDataRecognizer;
	CApaFsMonitor* iAppFsMonitor;
	CApaFsMonitor* iTypeStoreMonitor;
	CTypeStoreManager* iTypeStoreManager;
	TTime iTypeStoreModified;
	CBaBackupSessionWrapper* iBaBackupSessionWrapper;
	CApaEComMonitor* iRecEcomMonitor;
	CApaCommandLine* iAppCmdLine;
	CApsRecognitionCache* iRecognitionCache;
	CApaScanningRuleBasedPlugIns* iRuleBasedPlugIns;
	RWsSession iWsSession;
	RArray<SNonNativeApplicationType> iNonNativeApplicationTypeArray;
	/**
	This is ETrue if there was a power failure during the non-native applications list
	update. It means that there is a list fo updated apps that needs to be rolled back.
	*/
	TBool iNonNativeRecovery;
	RPointerArray<CCustomAppInfoData> iCustomAppList;
	TBool iLoadRecognizersOnDemand;
	CApaAppUnInstallMonitor* iAppUnInstallMonitor;
	TBool iLoadMbmIconsOnDemand;
	RBuf iNonNativeApplicationTypeRegistry;
	CApaIconCaptionOverrides* iIconCaptionOverrides;
	CApaIconCaptionCenrepObserver* iIconCaptionObserver;
	};

/**
The CCustomAppInfoData class encapsulate the attributes of customised application information
which will be used to modify short caption dynamically.

@internalComponent
*/
class CCustomAppInfoData: public CBase
	{
public:
	static CCustomAppInfoData* NewL(TUid aAppUid, TLanguage aLanguage, const TDesC& aShortCaption);
	~CCustomAppInfoData();
	inline TUid Uid() const;
	inline TLanguage Language() const;
	inline HBufC* ShortCaption() const;
private:
	CCustomAppInfoData(TUid aAppUid, TLanguage aLanguage);
	void ConstructL(const TDesC& aShortCaption);
private:
	TUid iUid;
	TLanguage iLanguage;
	HBufC* iShortCaption;
	};

class TApaAppViewIconSizeData
/**
The TApaAppViewIconSizeData class encapsulates the attributes that specify how to identify an icon for a particular
application and view in a certain size.

@internalComponent
*/
	{
public:
	inline TApaAppViewIconSizeData();
	inline TApaAppViewIconSizeData(TUid aAppUid,TUid aViewUid, const TSize& aSize);
public:
	TUid iAppUid;
	TUid iViewUid;
	TSize iSize;
	};

typedef TPckgBuf<TApaAppViewIconSizeData> TApaAppViewIconSizeDataBuf;

//
// inlines
//

inline TApaAppViewIconSizeData::TApaAppViewIconSizeData()
	: iAppUid(KNullUid), iViewUid(KNullUid), iSize(TSize(0,0))
	{}

inline TApaAppViewIconSizeData::TApaAppViewIconSizeData(TUid aAppUid,TUid aViewUid, const TSize& aSize)
	: iAppUid(aAppUid), iViewUid(aViewUid), iSize(aSize)
	{}

inline CApaAppList& CApaAppListServer::AppList()
	{ return *iAppList; }

inline CApaDataRecognizer* CApaAppListServer::DataRecognizer()
	{ return iDataRecognizer; }

/**
@internalComponent */
inline CApaCommandLine* CApaAppListServer::AppCmdLine()
	{ return iAppCmdLine;}

/**
@internalComponent */	
inline void CApaAppListServer::SetAppCmdLine(CApaCommandLine* aAppCmdLine)
	{ iAppCmdLine=aAppCmdLine; }

inline CApaScanningRuleBasedPlugIns* CApaAppListServer::RuleBasedPlugIns()
	{ return iRuleBasedPlugIns; }

inline RWsSession& CApaAppListServer::WsSession()
		{ return iWsSession; } //lint !e1536 Exposing low access member

inline TUid CCustomAppInfoData::Uid() const
	{ return iUid; }
	
inline TLanguage CCustomAppInfoData::Language() const
	{ return iLanguage; }
	
inline HBufC* CCustomAppInfoData::ShortCaption() const
	{ return iShortCaption; }
	
#endif
