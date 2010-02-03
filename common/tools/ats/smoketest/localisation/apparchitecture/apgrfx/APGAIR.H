// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__APGAIR_H__)
#define __APGAIR_H__

#if !defined(__APAID_H__)
#include <apaid.h>
#endif
#if !defined(__APMSTD_H__)
#include <apmstd.h>
#endif

#include <barsc.h>
#include <barsread.h>

/**
@internalComponent
*/
class ApaUtils
	{
public:
	IMPORT_C static TBool HandleAsRegistrationFile(const TUidType& aUidType);
	};

/** The 2nd UID that defines a resource file as being an application registration resource file.

@internalComponent
*/
const TUid KUidAppRegistrationFile ={0x101F8021};

class CApaMaskedBitmap;
class CApaAppViewData;
class TEntry;
class RFs;
class CResourceFile;
class RResourceReader;
class CApaAppList;
class CApaAppIconArray;

/**
@internalComponent
*/

NONSHARABLE_CLASS(CApaIconLoader) : public CBase
	{
public:
	static CApaIconLoader* NewL(RFs& aFs);
	static CApaIconLoader* NewLC(RFs& aFs);
	~CApaIconLoader();

	void AddIconArrayL(const CApaAppIconArray& aIcons);
	void LoadAllIconsL();
	TBool LoadIconsL(TInt aNumOfIcons, const TDesC& aMbmFileName, CArrayPtr<CApaMaskedBitmap>& aIcons);

private:
	CApaIconLoader(RFs& aFs);
	void ConstructL();
	TInt IconIndexL(const TDesC& aFileName, TBool& aUseCache);
	static TInt CompareIcons(const CApaMaskedBitmap& aFirst, const CApaMaskedBitmap& aSecond);

private:
	struct TKeyValuePair
		{
	public:
		HBufC* iFileName; // key
		TInt iIndex; // value
		};
private:
	RFs& iFs;
	RArray<TKeyValuePair> iIconIndexArray;
	TInt iCachedArrayIndex;
	RPointerArray<CApaAppIconArray> iIconArrays;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CApaAppIconArray) : public CBase
	{
public:
	static CApaAppIconArray* NewL(); // creates a dummy icon array
	static CApaAppIconArray* NewAppIconsL(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader);
	static CApaAppIconArray* NewViewIconsL(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader);
	static CApaAppIconArray* NewDefaultIconsL(); // creates a CApaAppIconArray that uses the default icons
	static CApaAppIconArray* NewRealDefaultIconsLC(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader); // creates the CApaAppIconArray that contains the default icons
	~CApaAppIconArray();

	TBool LoadIconsL();

	CApaMaskedBitmap* IconBySize(const TSize& aSize) const;
	CArrayFixFlat<TSize>* IconSizesL() const;
	TInt Count() const;
	TBool DefaultIconsUsed() const;
	CApaMaskedBitmap* operator[](TInt aIndex) const;
#ifdef SYMBIAN_APPARC_APPINFO_CACHE	
	TBool AreAppIconsLoaded() const;
	TBool AreViewIconsLoaded() const;
#endif	// SYMBIAN_APPARC_APPINFO_CACHE
	
private:
	static CApaAppIconArray* NewL(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader, TBool aFallbackToDefaultIcons);
	static CApaAppIconArray* NewLC(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader, TBool aFallbackToDefaultIcons);
	CApaAppIconArray();
	CApaAppIconArray(TInt aNumOfIcons, CApaIconLoader& aIconLoader, TBool aFallbackToDefaultIcons);
	void ConstructL(const TDesC& aMbmFileName);
	void GetDefaultIconsL();
	const CApaAppIconArray& DefaultIcons() const;
	
private:
	TInt iNumOfIcons;
	CApaIconLoader* iIconLoader;
	TBool iFallbackToDefaultIcons;
	HBufC* iMbmFileName;
	TBool iDefaultIconsUsed;
	CArrayPtrFlat<CApaMaskedBitmap>* iIcons;
	CApaAppList* iDefaultIconsProvider;
	};

class CApaAppInfoReader : public CBase
/**
Abstract base class for application information readers.

Concrete implementations should read information from some
data source and implement policy specific to each reader.

@internalComponent
*/
	{
public:
	~CApaAppInfoReader();
public:
	HBufC* AppBinaryFullName();
	TUidType AppBinaryUidType() const;
	TTime TimeStamp() const;
	TTime IconFileTimeStamp() const;
	void Capability(TDes8& aCapabilityBuf) const;
	TUint DefaultScreenNumber() const;
	HBufC* Caption();
	HBufC* ShortCaption();
	CApaAppIconArray* Icons();
#ifdef SYMBIAN_APPARC_APPINFO_CACHE	
	TInt NumOfAppIcons() const;
	virtual CApaIconLoader* IconLoader() = 0;
#else	
	TBool DefaultIconsUsed() const;
#endif	// SYMBIAN_APPARC_APPINFO_CACHE
	CArrayPtrFlat<CApaAppViewData>* Views();
	CDesCArray* OwnedFiles();
	HBufC* IconFileName();
	TBool NonMbmIconFile() const;
	HBufC* LocalisableResourceFileName();
	TTime LocalisableResourceFileTimeStamp() const;
	TLanguage AppLanguage() const;
	CArrayFixFlat<TApaAppServiceInfo>* ServiceArray(TInt& aIndexOfFirstOpenService);
	HBufC8* OpaqueData();

public:
	virtual TBool Read()=0;
protected:
	CApaAppInfoReader(RFs& aFs, const TUid aAppUid);
	
protected:
	RFs& iFs;
	TUid iAppUid;
	HBufC* iAppBinaryFullName;
	TUidType iAppBinaryUidType;
	TTime iTimeStamp;
	TTime iIconFileTimeStamp;
	TApaAppCapability iCapability;
	TUint iDefaultScreenNumber;
	HBufC* iCaption;
	HBufC* iShortCaption;
	CApaAppIconArray* iIcons;
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	TInt iNumOfAppIcons;
#else
	TBool iDefaultIconsUsed;
#endif
	CArrayPtrFlat<CApaAppViewData>* iViewDataArray;
	CDesCArray* iOwnedFileArray;
	HBufC* iIconFileName;
	TBool iNonMbmIconFile; // ETrue if icon filename is not an MBM file, however, EFalse does not necessarily mean it is an MBM file
	HBufC* iLocalisableResourceFileName;
	TTime iLocalisableResourceFileTimeStamp;
	TLanguage iApplicationLanguage;
	CArrayFixFlat<TApaAppServiceInfo>* iServiceArray;
	TInt iIndexOfFirstOpenService;
	TBool iOpenServiceIsLegacy;
	HBufC8* iOpaqueData;
	};

class CApaAppInfoReaderV2 : public CApaAppInfoReader
/**
Reads application information from a combination of registration file,
optional localisable resource file and optional icon file.
Application information stored in this way is referred to as version 2.

@internalComponent
*/
	{
public:
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	static CApaAppInfoReaderV2* NewL(RFs& aFs, const TDesC& aRegistrationFileName, const TUid aAppUid);
	virtual CApaIconLoader* IconLoader();
#else
	static CApaAppInfoReaderV2* NewL(RFs& aFs, const TDesC& aRegistrationFileName, const TUid aAppUid,const TDesC& aDefaultAppIconFileName);
#endif	// SYMBIAN_APPARC_APPINFO_CACHE
	~CApaAppInfoReaderV2();

public:
	virtual TBool Read();
	static TBool FileIsMbm(const TDesC& aFileName);

private:
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	CApaAppInfoReaderV2(RFs& aFs, const TDesC& aRegistrationFileName, const TUid aAppUid);
#else
	CApaAppInfoReaderV2(RFs& aFs, const TDesC& aRegistrationFileName, const TUid aAppUid, const TDesC& aDefaultAppIconFileName);
#endif
	void ConstructL();
	void ReadMandatoryInfoL(CResourceFile*& aRegistrationFile, RResourceReader& aResourceReader);
	void ReadNonLocalisableInfoL(RResourceReader& aResourceReader, CResourceFile*& aLocalisableResourceFile, TUint& aLocalisableResourceId);
	void ReadNonLocalisableOptionalInfoL(RResourceReader& aResourceReader, const CResourceFile* aRegistrationFile, CResourceFile* aLocalisableResourceFile);
	void ReadDataTypesL(RResourceReader& aResourceReader, CArrayFixFlat<TDataTypeWithPriority>& aDatatypes);
	void ReadLocalisableInfoL(const CResourceFile* aResourceFile, TUint aResourceId, TBool& aUseDefaultIcons);
	HBufC* FullIconFileNameL(const TDesC& aIconFileName) const;
	TBool HasWriteDeviceDataCap();
    void ReadAppSecurityInfo();
	
    TBool FileIsMbmWithGenericExtensionL(const TDesC& aFileName) const;
	static HBufC8* ReadOpaqueDataL(TUint aResourceId, const CResourceFile* aRegistrationFile, CResourceFile* aLocalisableResourceFile);

private:
	const TDesC& iRegistrationFileName;
	TBool iHasWriteDeviceDataCap;
    TBool iIsSidTrusted;
    // This flag is used to determine if app security info was allready read
    TBool iSecurityInfoHasBeenRead;
	CApaIconLoader* iIconLoader;	
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
	const TDesC& iDefaultAppIconFileName;
#endif
	};

#endif
