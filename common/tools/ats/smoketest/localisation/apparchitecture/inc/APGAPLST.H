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

#if !defined(__APGAPLST_H__)
#define __APGAPLST_H__

#if !defined(__APAID_H__)
#include <apaid.h>
#endif
#if !defined(__APMSTD_H__)
#include <apmstd.h>
#endif
#if !defined(__BADESCA_H__)
#include <badesca.h>
#endif

#include <s32file.h>

// classes defined:
class CApaAppData;
class CApaAppList;
class CApaAppViewData;
//
// classes referenced:
class CApaMaskedBitmap;
class TEntry;
class RFs;
class CApaAppRegFinder;
class CApaAppIconArray;
class CApaIconLoader;
//

/**
KApaMaxAppIcons

@publishedPartner
@released
*/
const TInt KApaMaxAppIcons=3;

/**
KApaIconIndexSmall

@publishedPartner
@released
*/
const TInt KApaIconIndexSmall=0;

/**
KApaIconIndexMedium

@publishedPartner
@released
*/
const TInt KApaIconIndexMedium=1;

/**
KApaIconIndexLarge

@publishedPartner
@released
*/
const TInt KApaIconIndexLarge=2;

/**
KIgnoreScreenMode

@internalTechnology
*/
const TInt KIgnoreScreenMode=-1;


_LIT(KDefaultAppIconMbm,"Z:\\Resource\\Apps\\default_app_icon.mbm");
_LIT(KLitPathForUntrustedRegistrationResourceFiles, "?:\\private\\10003a3f\\import\\apps\\*");


class CApaAppEntry : public CBase
/**
Utility class with smaller RAM footprint than TApaAppEntry

@internalComponent
*/
	{
public:
	static CApaAppEntry* NewL(const TApaAppEntry& aAppEntry);
	~CApaAppEntry();
	void Get(TApaAppEntry& aAppEntry) const;
private:
	CApaAppEntry(const TUidType& aUidType);
	void ConstructL(const TDesC& aFileName);
private:
	TUidType iUidType;
	HBufC* iFullName;
	};

class CApaAppData : public CBase
/** Represents the data associated with a CApaApplication, such as its 
caption, capabilities and icons. This should be accessed through the Apparc Server.

@internalComponent
*/
	{
public:
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	IMPORT_C static CApaAppData* NewL(const TApaAppEntry& aAppEntry, RFs& aFs);
#else
	IMPORT_C static CApaAppData* NewL(const TApaAppEntry& aAppEntry, RFs& aFs, const TDesC& aDefaultAppIconFileName);
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	IMPORT_C ~CApaAppData();
	IMPORT_C TApaAppEntry AppEntry() const;
	inline TPtrC Caption() const;
	inline TPtrC ShortCaption() const;
	IMPORT_C CApaMaskedBitmap* Icon(TInt aIconIndex) const;
	IMPORT_C void Capability(TDes8& aCapabilityBuf)const;
	inline TBool IsPresent() const;
	// ER5
	IMPORT_C TDataTypePriority DataType(const TDataType& aDataType) const;
	// ER6
	IMPORT_C CApaMaskedBitmap* Icon(TSize aSize) const;
	IMPORT_C CArrayFixFlat<TSize>* IconSizesL() const;
	// ER6.1
	IMPORT_C CArrayPtrFlat<CApaAppViewData>* Views() const;
	IMPORT_C CDesCArray* OwnedFiles() const;
	IMPORT_C TBool CanUseScreenMode(TInt aScreenMode);
	// 7.0s
	IMPORT_C void GetIconInfo(TInt& aIconCount, TInt& aDefaultIconsUsed) const;
	// 8.1
	IMPORT_C TUint DefaultScreenNumber() const;
	IMPORT_C TBool RegistrationFileUsed() const;
	IMPORT_C TPtrC IconFileName() const;
	IMPORT_C TBool NonMbmIconFile() const;

	// 9.0
	IMPORT_C TBool ImplementsService(TUid aServiceUid) const;
	TInt ImplementsServiceWithDataType(TUid aServiceUid, const TDataType& aDataType) const;

	// 9.1
	IMPORT_C TLanguage ApplicationLanguage() const;
	IMPORT_C TPtrC RegistrationFileName() const;
	IMPORT_C TPtrC8 OpaqueData() const;
	IMPORT_C TUid NonNativeApplicationType() const;
	IMPORT_C TPtrC LocalisableResourceFileName() const;
	IMPORT_C void SetShortCaptionL(const TDesC& aShortCaption);
	IMPORT_C TBool IsPending()const;
	
	// Icon/caption overrides
	IMPORT_C void SetCaptionL(const TDesC& aCaption);
	IMPORT_C void SetIconsL(const TDesC& aFileName, TInt aNumIcons);
	
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	void ExternalizeL(RWriteStream& aWriteStream) const;	
	void LoadIconsL();
	TBool MbmIconsRequireLoading() const;
	inline CApaAppData* Next() const;
#endif // SYMBIAN_APPARC_APPINFO_CACHE

private:
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	CApaAppData(RFs& aFs);
	TBool Update();
	void SetAppPending();

	void ConstructL(const TApaAppEntry& aAppEntry);
	TInt StoreApplicationInformation();
#else
	CApaAppData(RFs& aFs);
	TBool Update(const TDesC& aDefaultAppIconFileName);	
	void ConstructL(const TApaAppEntry& aAppEntry, const TDesC& aDefaultAppIconFileName);
	TInt StoreApplicationInformation(const TDesC& aDefaultAppIconFileName);
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	void UpdateServiceArray(CArrayFixFlat<TApaAppServiceInfo>* aNewServiceArray);
	TDataTypePriority DataType(const TDataType& aDataType, const CArrayFixFlat<TDataTypeWithPriority>& aDataTypeArray) const;
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	void InternalizeL(RReadStream& aReadStream);
	TBool ViewMbmIconsRequireLoading() const;
#endif // SYMBIAN_APPARC_APPINFO_CACHE
private:
	enum { ENotPresent, ENotPresentPendingUpdate, EPresentPendingUpdate, EIsPresent, ESuperseded };
private:
	CApaAppIconArray* iIcons;
	HBufC* iCaption;
	HBufC* iShortCaption;
	HBufC* iFullName; // filename of application binary
	TInt iIsPresent; // uses enum
	TUidType iUidType;
	CApaAppData* iNext;
	TApaAppCapabilityBuf iCapabilityBuf;
	CApaAppEntry* iSuccessor;
	TTime iTimeStamp;
	CArrayPtrFlat<CApaAppViewData>* iViewDataArray;
	CDesCArray* iOwnedFileArray;
 	RFs& iFs;
 	HBufC* iRegistrationFile;
 	TUint iDefaultScreenNumber;
 	HBufC* iIconFileName;
 	TBool iNonMbmIconFile;
 	HBufC* iLocalisableResourceFileName;
 	TTime iLocalisableResourceFileTimeStamp;
	TTime iIconFileTimeStamp;
 	TLanguage iApplicationLanguage;
 	CArrayFixFlat<TApaAppServiceInfo>* iServiceArray;
 	TInt iIndexOfFirstOpenService;
	TUid iNonNativeApplicationType;
	HBufC8* iOpaqueData;
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
 	TInt iNumOfAppIcons;
 	TInt iNumOfAppIconsFromResourceFile;
 	HBufC* iIconFileNameFromResourceFile; // Icon file name as found in the localisable resource file
 	TBool iNonMbmIconFileFromResourceFile; // A Flag that tells whether the icon in resource file is non MBM file format
 	TTime iIconFileTimeStampFromResourceFile;
	HBufC* iShortCaptionFromResourceFile;	// Short Caption as found in the localisable resource file
	HBufC* iCaptionFromResourceFile;		// Caption as found in the localisable resource file
	CApaIconLoader* iIconLoader;
#endif // SYMBIAN_APPARC_APPINFO_CACHE

private:
	friend class CApaAppList;
	};

class CApaAppViewData : public CBase
/**
The CApaAppViewData class represents the data associated with an application view.

@internalComponent
*/
	{
public:
	IMPORT_C TUid Uid() const;
	inline TPtrC Caption() const;
	IMPORT_C CApaMaskedBitmap* Icon(const TSize& aSize) const;
	IMPORT_C CArrayFixFlat<TSize>* IconSizesL() const;
	IMPORT_C TInt ScreenMode() const;
	IMPORT_C TPtrC IconFileName() const;
	IMPORT_C TBool NonMbmIconFile() const;
public:
	~CApaAppViewData();
	static CApaAppViewData* NewLC();
	void SetUid(TUid aUid);
	void SetCaptionL(const TDesC& aCaption);
	void SetIconArray(CApaAppIconArray* aIcons);
	void SetScreenMode(TInt aScreenMode);
	void SetIconFileNameL(const TDesC& aFileName);
	void SetNonMbmIconFile(TBool aNonMbmIconFile);
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	void SetNumOfViewIcons(TInt aNumOfViewIcons);
#endif // SYMBIAN_APPARC_APPINFO_CACHE	
private:
	CApaAppViewData();
	void ConstructL();
private:
	CApaAppIconArray* iIcons;
	HBufC* iCaption;
	TUid iUid;
	TInt iScreenMode;
	HBufC* iIconFileName;
	TBool iNonMbmIconFile;
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	TInt iNumOfViewIcons;
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	friend class CApaAppData;
	};


class MApaAppListObserver
/**
 The MApaAppListObserver interface allows a class to be informed when a CApaAppList is 
 updated.

 @internalComponent
*/
	{
public:
	virtual void NotifyUpdate(TInt aReason)=0;
	virtual void InitialListPopulationComplete()=0;
	virtual void NotifyScanComplete()=0;
 	};

class CApaAppList : public CBase
/**
The CApaAppList class provides a list of all available applications present on a device.
Its functionality should be accessed through the Apparc Server.

@internalComponent
*/
	{
public: 
	IMPORT_C static CApaAppList* NewL(RFs& aFs,CApaAppRegFinder* aAppRegFinder, TBool aLoadMbmIconsOnDemand, TInt aTimeoutDelay = 50000); // takes ownership of aAppRegFinder
public:
	IMPORT_C void PurgeL();
	inline void Purge();
	IMPORT_C TInt Count() const;
	IMPORT_C CApaAppData* FirstApp() const;
	IMPORT_C CApaAppData* FirstApp(TInt aScreenMode) const; 
	IMPORT_C CApaAppData* NextApp(const CApaAppData* aApp) const;
	IMPORT_C CApaAppData* NextApp(const CApaAppData* aApp, TInt aScreenMode) const;
	IMPORT_C CApaAppData* AppDataByUid(TUid aAppUid) const;
	IMPORT_C void StopScan(TBool aNNAInstall = EFalse);
	IMPORT_C void RestartScanL();
	IMPORT_C TBool AppListUpdatePending();
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
	IMPORT_C TInt UpdateCounter() const;
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	// ER5
	IMPORT_C TUid PreferredDataHandlerL(const TDataType& aDataType) const;
	IMPORT_C void StartIdleUpdateL();
	IMPORT_C void StartIdleUpdateL(MApaAppListObserver* aObserver);
	IMPORT_C void InitListL(MApaAppListObserver* aObserver);
	IMPORT_C TBool IsIdleUpdateComplete() const;
	//
	IMPORT_C TBool IsFirstScanComplete() const;
	IMPORT_C TBool AppScanInProgress() const;
	IMPORT_C CBufFlat* ServiceArrayBufferL(TUid aAppUid) const;
	IMPORT_C CBufFlat* ServiceImplArrayBufferL(TUid aServiceUid) const;
	IMPORT_C CBufFlat* ServiceImplArrayBufferL(TUid aServiceUid, const TDataType& aDataType) const;	
	IMPORT_C CBufFlat* ServiceUidBufferL(TUid aAppUid) const;
	IMPORT_C CBufFlat* ServiceOpaqueDataBufferL(TUid aAppUid, TUid aServiceUid) const;
	IMPORT_C CApaAppData* FindAndAddSpecificAppL(CApaAppRegFinder* aFinder, TUid aAppUid);
	IMPORT_C TUid PreferredDataHandlerL(const TDataType& aDataType, const TUid* aServiceUid, 
		TInt& aPriority) const;
	IMPORT_C ~CApaAppList();
	// 9.1
	IMPORT_C CApaAppData* AppDataByFileName(const TDesC& aFullFileName) const;
	/*IMPORT_C*/ RFs& ShareProtectedFileServer();
	IMPORT_C void AddForcedRegistrationL(HBufC* aRegistrationFile);
	IMPORT_C void ResetForcedRegistrations();
	IMPORT_C static TInt CompareStrings(const HBufC& aFirst, const HBufC& aSecond);
	IMPORT_C TBool IsLanguageChangePending() const;
	IMPORT_C static CApaAppList* Self();
    IMPORT_C CArrayFixFlat<TUid>* UninstalledAppArray();
	void AcquireDefaultIconArrayL() const;
	const CApaAppIconArray& DefaultIconArray() const;
	void ReleaseDefaultIconArray() const;
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	void StoreL();
	void NotifyObserver();
	void DeleteAppListStorer();
	void DeleteAppIconLoader();
	void InitiateStoringOfAppList();
#endif // SYMBIAN_APPARC_APPINFO_CACHE
private:
	enum
		{
		EFirstScanComplete = 0x01
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
		,EAppListHasChanged = 0x02,	// This flag is used to check if the applist has really changed after a re-scan/update scan.
		ENotifyUpdateOnFirstScanComplete = 0x04	// This flag is used to notify clients for applist update on first boot when AppsList.Bin is used.
#endif // SYMBIAN_APPARC_APPINFO_CACHE
		,ELangChangePending = 0x08 // This flag is used to check if applist update is in progress on language change event.
		};

#ifndef SYMBIAN_APPARC_APPINFO_CACHE		
	enum
		{
		EFirstStageScan=0x01,
		EScanFinished=0x02,
		};
#endif // SYMBIAN_APPARC_APPINFO_CACHE

private:
	CApaAppList(RFs& aFs, CApaAppRegFinder* aAppRegFinder, TBool aLoadMbmIconsOnDemand, TInt aIdlePeriodicDelay);
	void UpdateNextAppL(const TApaAppEntry& aAppEntry,TBool& aHasChanged);
	void AddToList( CApaAppData* aAppData );
	static void SetPending(CApaAppData* aAppData);
	static void SetNotFound(CApaAppData* aAppData, TBool& aHasChanged);
	static TInt IdleUpdateCallbackL(TAny* aObject);
	TInt IdleUpdateL();
	void ScanComplete();
	void UndoSetPending(CApaAppData* aAppData);

#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	void StopIdler();
	void DeleteAppData();
#else	
	void StopIdlerL();
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	CArrayFixFlat<TDataTypeWithPriority>* DataTypeArrayDeepCopyLC(const CArrayFixFlat<TDataTypeWithPriority>& aOriginal) const;
	CApaAppIconArray* LoadDefaultIconsL() const;
	void UpdateDefaultIconsL();
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	void StartIconLoadingL();
	void DeleteAppsListBackUpAndTempFiles();
	void ScanRemovableDrivesAndUpdateL();
	void CreateDefaultAppIconFileNameL();
#endif // SYMBIAN_APPARC_APPINFO_CACHE

private: 
	// Persistence Layer
	void RestoreL();
	void ConstructL();
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
	void StoreL();
	void StoreEntryL(RFileWriteStream& theWriteStream,CApaAppData* aApp);
	void DoStoreL(RFileWriteStream& aWriteStream);
#endif // SYMBIAN_APPARC_APPINFO_CACHE

	void GetAppsListCachePathL();
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	NONSHARABLE_CLASS(CApaIdleIconLoader) : public CActive
		{
	/**
	Utility class used to Load Icons once applist is populated
	@internalComponent
	*/	
	public:
		CApaIdleIconLoader(CApaAppData* aFirstAppData, RFs& aFs, CApaAppList& aAppList);
		~CApaIdleIconLoader();
		void Start();

	private:	// from CActive
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);

	private:
		CApaAppData* iCurrentAppData;
		RFs& iFs;
		CApaAppList& iAppList;
		};

	NONSHARABLE_CLASS(CApaAppListStorer) : public CActive
		{
	/**
	Utility class used to externalize applist to file
	@internalComponent
	*/	
	public:
		static CApaAppListStorer* NewL(CApaAppData* aFirstAppData, RFs& aFs, CApaAppList& aAppList);
		~CApaAppListStorer();
		void StartL(const TTimeIntervalMicroSeconds32& aDelay);

	private:
		CApaAppListStorer(CApaAppData* aFirstAppData, RFs& aFs, CApaAppList& aAppList);
		static void StoreEntryL(RWriteStream& aWriteStream, const CApaAppData& aApp);
		void ConstructL();

	private:	// from CActive
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);

	private:
		CApaAppData* iCurrentAppData;
		TFileName iTempFilename;
		RFileWriteStream iWriteStream;
		RTimer iTimer;
		RFs& iFs;
		CApaAppList& iAppList;
		};
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	
private:
	RFs& iFs;
	CApaAppData* iAppData; // linked list of apps
	CPeriodic* iAppIdler;
	MApaAppListObserver* iObserver;
	CApaAppData* iValidFirstAppData; //First valid app data in linked list!
	TInt iFlags;
	CApaAppRegFinder* iAppRegFinder;
	TInt iIdlePeriodicDelay; 	// idle timeout periodic delay
	RFs iFsShareProtected;
	mutable CApaAppIconArray* iDefaultIconArray;
	mutable TInt iDefaultIconUsageCount;
	RPointerArray<HBufC> iForcedRegistrations;
	class CApaLangChangeMonitor; //inner class of CApaAppList.
	CApaLangChangeMonitor* iAppLangMonitor; // Active Object used for language change monitoring.		

	RBuf iAppsListCacheFileName;
	RBuf iAppsListCacheBackUpFileName;
	RBuf iAppsListCachePath;

#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	HBufC* iDefaultAppIconMbmFileName; // This member data lives only during the scan
	CApaAppListStorer* iAppListStorer;	//Active Object used for writing applist to file.
	CApaIdleIconLoader* iAppIconLoader;	//Active Object used for icon handling
#else
	TInt iScanStage;
	TInt iUpdateCounter;
	TApaAppEntry iCurrentApp;
	TInt iOldUpdateCounter;
	HBufC* iDefaultAppIcon; // This member data lives only during the scan
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	TBool iLoadMbmIconsOnDemand;
	TBool iNNAInstallation;
    CArrayFixFlat<TUid>* iUninstalledApps; 

private:
	friend class CApaLangChangeMonitor;
	};

//
// inlines
//

inline TPtrC CApaAppViewData::Caption() const
	{ return *iCaption; }

inline TPtrC CApaAppData::Caption() const
/** Gets the application's caption.

@return The application's caption. */
	{ return *iCaption; }

inline TPtrC CApaAppData::ShortCaption() const
/** Gets the application's short caption.

@return The application's short caption. */
	{ return *iShortCaption; }

inline TBool CApaAppData::IsPresent() const
/** Tests whether the application is present or not on the device.

@return True if application exists, else false. */
	{ return iIsPresent; }

#ifdef SYMBIAN_APPARC_APPINFO_CACHE
inline CApaAppData* CApaAppData::Next() const
/** Gets the Next Appdata in the list

@return iNext */
	{ return iNext; }
#endif // SYMBIAN_APPARC_APPINFO_CACHE

/**
 * Use PurgeL.
 *
 * @deprecated
 */
inline void CApaAppList::Purge()
	{	TRAP_IGNORE(PurgeL());	}

#endif //__APGAPLST_H__
