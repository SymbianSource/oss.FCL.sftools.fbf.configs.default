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

#include "apgaplst.h"
#include "apgstd.h" 
#include "apfdef.h"
#include "..\apparc\trace.h"
#include "apgnotif.h"
#include "..\apfile\aprfndr.h"
#include <e32math.h>
#include <bautils.h>
#include <s32mem.h>
#include "apgpriv.h"
#include "apgair.h"
#include "apgicnfl.h"
#include "apprivate.h"
#include <e32uid.h>


// Delays in the pseudo idle object that builds the application list
//

const TInt KIdleStartDelay=0;
const TInt KMaxOpaqueDataLength = 0x1000; // maximum length of opaque data that can be passed between client and apparc server via a TApaAppServiceInfo object - this can be increased in future if needed
const TInt KBufferExpansionGranularity = 0x100;
const TInt KNumberOfIconsInDefaultMbm = 3;
const TInt KAppListToFileStartDelay = 60000000;


//
// Class CApaLangChangeMonitor
//

NONSHARABLE_CLASS(CApaAppList::CApaLangChangeMonitor) : public CActive
 		{
 	/**
 	Utility class used to monitor locale/language change event.
 	@internalComponent
 	*/
 	public:
 		static CApaLangChangeMonitor* NewL(CApaAppList& aAppList);
 		~CApaLangChangeMonitor();
 		void Start();
 		
 	private:
 		CApaLangChangeMonitor(CApaAppList& aAppList);
 		void ConstructL();
 		
 	private:	//from CActive
 		void RunL();
 		void DoCancel();
 		TInt RunError(TInt aError);
 		
 	private:
 		RChangeNotifier iLangNotifier;
 		CApaAppList& iAppList;
 		TLanguage iPrevLanguage;
 		};
 
 		
 
//
// Local functions
//

void CleanupServiceArray(TAny* aServiceArray)
	{
	__ASSERT_DEBUG(aServiceArray, Panic(EPanicNullPointer));
	CArrayFixFlat<TApaAppServiceInfo>* serviceArray = static_cast<CArrayFixFlat<TApaAppServiceInfo>*>(aServiceArray);
	TInt serviceCount = serviceArray->Count();
	if (serviceCount > 0)
		{
		for (TInt i = serviceCount - 1; i >= 0; i--)
			{
			(*serviceArray)[i].Release();
			}
		}
	delete serviceArray;
	serviceArray = 0;
	}


//
// Class CApaAppViewData
//

CApaAppViewData::~CApaAppViewData()
	{
	delete iIcons;
	delete iCaption;
	delete iIconFileName;
	}

CApaAppViewData::CApaAppViewData()
	: iNonMbmIconFile(EFalse)
	{
	}

void CApaAppViewData::ConstructL()
	{
	iIcons=CApaAppIconArray::NewL();
	}

CApaAppViewData* CApaAppViewData::NewLC()
	{
	CApaAppViewData* self=new(ELeave) CApaAppViewData();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

void CApaAppViewData::SetUid(TUid aUid)
	{
	iUid=aUid;
	}

void CApaAppViewData::SetScreenMode(TInt aScreenMode)
	{
	iScreenMode=aScreenMode;
	}

EXPORT_C TInt CApaAppViewData::ScreenMode() const
	{
	return iScreenMode;
	}

void CApaAppViewData::SetCaptionL(const TDesC& aCaption)
	{
	HBufC* newCaption=aCaption.AllocL();
	delete(iCaption); // after the AllocL succeeds
	iCaption=newCaption;
	}

void CApaAppViewData::SetIconArray(CApaAppIconArray* aIcons)
	{
	delete iIcons;
	iIcons = aIcons;
	}

void CApaAppViewData::SetIconFileNameL(const TDesC& aFileName)
	{
	HBufC* fileName = aFileName.AllocL();
	delete iIconFileName; // after the AllocL succeeds
	iIconFileName = fileName;
	}

void CApaAppViewData::SetNonMbmIconFile(TBool aNonMbmIconFile)
	{
	iNonMbmIconFile = aNonMbmIconFile;
	}

EXPORT_C TUid CApaAppViewData::Uid() const
	{
	return iUid;
	}

EXPORT_C CApaMaskedBitmap* CApaAppViewData::Icon(const TSize& aSize) const
	{
	__ASSERT_ALWAYS(iIcons, Panic(EPanicNullPointer));
	return iIcons->IconBySize(aSize);
	}

EXPORT_C CArrayFixFlat<TSize>* CApaAppViewData::IconSizesL() const
	{
	__ASSERT_ALWAYS(iIcons, Panic(EPanicNullPointer));
	return iIcons->IconSizesL();
	}

EXPORT_C TPtrC CApaAppViewData::IconFileName() const
	{
	if (iIconFileName)
		{
		return *iIconFileName;
		}
	else
		{
		return TPtrC(KNullDesC);
		}
	}

EXPORT_C TBool CApaAppViewData::NonMbmIconFile() const
	{
	return iNonMbmIconFile;
	}


//
// class CApaAppEntry
//

CApaAppEntry* CApaAppEntry::NewL(const TApaAppEntry& aAppEntry)
	{ // static
	CApaAppEntry* self=new(ELeave) CApaAppEntry(aAppEntry.iUidType);
	CleanupStack::PushL(self);
	self->ConstructL(aAppEntry.iFullName);
	CleanupStack::Pop(self);
	return self;
	}

CApaAppEntry::~CApaAppEntry()
	{
	delete iFullName;
	}

void CApaAppEntry::Get(TApaAppEntry& aAppEntry) const
	{
	aAppEntry.iFullName=*iFullName;
	aAppEntry.iUidType=iUidType;
	}

CApaAppEntry::CApaAppEntry(const TUidType& aUidType)
	: iUidType(aUidType)
	{
	}

void CApaAppEntry::ConstructL(const TDesC& aFileName)
	{
	iFullName=aFileName.AllocL();
	}


//
// class TApaAppServiceInfo
//

TApaAppServiceInfo::TApaAppServiceInfo()
	: iUid(KNullUid),
	  iDataTypes(0),
	  iOpaqueData(NULL)
	{
	}

TApaAppServiceInfo::TApaAppServiceInfo(TUid aUid, 
	CArrayFixFlat<TDataTypeWithPriority>* aDataTypes, HBufC8* aOpaqueData)
	: iUid(aUid),
	  iDataTypes(aDataTypes),
	  iOpaqueData(aOpaqueData)
	{
	__ASSERT_DEBUG(iDataTypes, Panic(EPanicNullPointer));
	__ASSERT_DEBUG(iOpaqueData, Panic(EPanicNullPointer));
	}

void TApaAppServiceInfo::ExternalizeL(RWriteStream& aStream) const
	{
	__ASSERT_DEBUG(iDataTypes, User::Invariant());
	__ASSERT_DEBUG(iDataTypes, Panic(EPanicNullPointer));
	__ASSERT_DEBUG(iOpaqueData, Panic(EPanicNullPointer));
	aStream << iUid;
	aStream << *iDataTypes; //lint !e613 Possible use of null pointer - Asserted above
	aStream << *iOpaqueData;//lint !e613 Possible use of null pointer - Asserted above
	}

void TApaAppServiceInfo::InternalizeL(RReadStream& aStream)
	{
	aStream >> iUid;
	iDataTypes = new(ELeave) CArrayFixFlat<TDataTypeWithPriority>(1);
	aStream >> *iDataTypes;
	iOpaqueData = HBufC8::NewL(aStream, KMaxOpaqueDataLength);
	}

void TApaAppServiceInfo::Release()
	{
	if (iDataTypes)
		{
		iDataTypes->Reset();
		delete iDataTypes;		
		iDataTypes = NULL;
		}
	if (iOpaqueData)
		{
		delete iOpaqueData;
		iOpaqueData = NULL;
		}
	}

CArrayFixFlat<TDataTypeWithPriority>& TApaAppServiceInfo::DataTypes()
	{
	__ASSERT_DEBUG(iDataTypes, Panic(EPanicNullPointer));
	return *iDataTypes; //lint !e613 Possible use of null pointer - Asserted above
	}

/** Returns the service UID.

Note that some APIs may store a UID other than a service UID
in a TApaAppServiceInfo object. Such APIs clearly state what
the UID represents.

@return the service UID.
*/
EXPORT_C TUid TApaAppServiceInfo::Uid() const
	{
	return iUid;
	}
	
EXPORT_C const CArrayFixFlat<TDataTypeWithPriority>& TApaAppServiceInfo::DataTypes() const
	{
	__ASSERT_DEBUG(iDataTypes, Panic(EPanicNullPointer));	
	return *iDataTypes; //lint !e613 Possible use of null pointer - Asserted above
	}

/** Returns the service implementation's opaque data.

For each service UID registered by an application, the associated
opaque data indicates how the service is implemented by that application.

The meaning of the opaque data is not known to the framework, it will vary
according to the service.

For some services the opaque data may be a name intended for user display,
for others it may be structured data that the service's client-side code can interpret.

@return the service implementation's opaque data.
*/
EXPORT_C const TDesC8& TApaAppServiceInfo::OpaqueData() const
	{
	if (iOpaqueData)
		{
		return *iOpaqueData;
		}
	return KNullDesC8;
	}

//
// class CApaAppServiceInfoArray
//

CApaAppServiceInfoArray::CApaAppServiceInfoArray()
	{
	}

//
// class CApaAppServiceInfoArrayWrapper
//

CApaAppServiceInfoArrayWrapper* CApaAppServiceInfoArrayWrapper::NewL(CArrayFix<TApaAppServiceInfo>* aServiceInfoArray)
	{
	CApaAppServiceInfoArrayWrapper* self = new CApaAppServiceInfoArrayWrapper(aServiceInfoArray);
	if (!self)
		{
		CleanupServiceArray(aServiceInfoArray);
		User::LeaveNoMemory();
		}
	return self;
	}

CApaAppServiceInfoArrayWrapper::CApaAppServiceInfoArrayWrapper(CArrayFix<TApaAppServiceInfo>* aServiceInfoArray)
	: iServiceInfoArray(aServiceInfoArray)
	{
	}

CApaAppServiceInfoArrayWrapper::~CApaAppServiceInfoArrayWrapper()
	{
	CleanupServiceArray(iServiceInfoArray);
	iServiceInfoArray = NULL;
	}

TArray<TApaAppServiceInfo> CApaAppServiceInfoArrayWrapper::Array()
	{
	__ASSERT_ALWAYS(iServiceInfoArray, Panic(EPanicNullPointer));
	return iServiceInfoArray->Array();
	}


//
// Class CApaAppData
//

EXPORT_C CApaAppData* CApaAppData::NewL(const TApaAppEntry& aAppEntry, RFs& aFs, const TDesC& aDefaultAppIconFileName)
	{
	CApaAppData* self=new(ELeave) CApaAppData(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aAppEntry, aDefaultAppIconFileName);
	CleanupStack::Pop(); // self
	return self;
	}

CApaAppData::CApaAppData(RFs& aFs)
	:iCaption(NULL), iShortCaption(NULL), 
	iIsPresent(CApaAppData::EIsPresent), iFs(aFs),
	iNonMbmIconFile(EFalse),
	iApplicationLanguage(ELangNone), iIndexOfFirstOpenService(-1),
	iNonNativeApplicationType(TUid::Null())
	{
	}

void CApaAppData::ConstructL(const TApaAppEntry& aAppEntry, const TDesC& aDefaultAppIconFileName)
	{
	iUidType = aAppEntry.iUidType; // if the 2nd UID is KUidAppRegistrationFile, iUidType will be updated in StoreApplicationInformation to reflect the TUidType for the application binary
	if (ApaUtils::HandleAsRegistrationFile(aAppEntry.iUidType))
		{
		iRegistrationFile = aAppEntry.iFullName.AllocL();
		}
	else
		{
		iFullName = aAppEntry.iFullName.AllocL();
		}

	iCapabilityBuf.FillZ(iCapabilityBuf.MaxLength());
	iIcons = CApaAppIconArray::NewL();
	iViewDataArray=new(ELeave) CArrayPtrFlat<CApaAppViewData>(1);
	iOwnedFileArray=new(ELeave) CDesCArraySeg(1);
	User::LeaveIfError(StoreApplicationInformation(aDefaultAppIconFileName));
	}



// Return a standard error code
// The value returned only reflect the caption status
// If there is errors setting up captions the old values are retained
// All other errors are silently ignored
// General notes:
// 1. This method is deliberately very similar to the old CApaAppData::GetAifData
//    in order to maintain behavioural compatibility for V1 apps
// 2. Be very careful in this method, because it can be called on a newly constructed object,
//    or on an existing object, so don't assume member data pointers will be NULL
TInt CApaAppData::StoreApplicationInformation(const TDesC& aDefaultAppIconFileName)
	{
	HBufC* caption = NULL;
	HBufC* shortCaption = NULL;

	iTimeStamp = TTime(0); // cannot init in constructor because this function can be called on an existing CApaAppData object

	CApaAppInfoReader* appInfoReader = NULL;
	TBool readSuccessful = EFalse;
	TBool isNonNativeApp = EFalse;
	if (iRegistrationFile != NULL)
		{
		if (TParsePtrC(*iRegistrationFile).Path().CompareF(KLitPathForNonNativeResourceAndIconFiles)==0)
			{
			isNonNativeApp = ETrue;
			}
		__ASSERT_ALWAYS(&aDefaultAppIconFileName,Panic(EPanicNullDefaultAppIconFileName));

		TRAPD(err,appInfoReader = CApaAppInfoReaderV2::NewL(iFs, *iRegistrationFile, iUidType[2], aDefaultAppIconFileName));
		if(err != KErrNone)
			{
			appInfoReader = NULL;
			}

		if (appInfoReader == NULL)
			{
			if (iFullName == NULL)
				{
				// assume that if iFullName is NULL, this method has been called as part
				// of constructing a new app data object. The CApaAppInfoReader derived object
				// could not be created, therefore we have no way to determine the full filename
				// of the app binary, so give up
				return KErrNoMemory;
				}
			}
		else
			{
			readSuccessful = appInfoReader->Read();
			HBufC* appBinaryFullName=appInfoReader->AppBinaryFullName();
			if (appBinaryFullName)
				{
				delete iFullName;
				iFullName = appBinaryFullName;
				}
			if (iFullName == NULL)
				{
				delete appInfoReader;
				return KErrNoMemory;
				}
			// if this object has just been constructed, iUidType is currently the TUidType
			// of the registration file, it should be the TUidType of the app binary file
			TUidType uidType = appInfoReader->AppBinaryUidType();
			if (uidType[1].iUid != KNullUid.iUid)
				{
				iUidType = uidType;
				}
			}
		}

	if (appInfoReader != NULL)
		{
		// must get captions and icons regardless of value of readSuccessful,
		// because the V1 reader might have read captions
		// and default icons from a caption file/default AIF, even if other info wasn't read
		// from an application specific AIF file (!readSuccessful)
		// this is done to maintain behavioural compatibility with V1
		caption = appInfoReader->Caption();
		shortCaption = appInfoReader->ShortCaption();

		CApaAppIconArray* icons = appInfoReader->Icons();
		if(icons)
			{
			delete iIcons;
			iIcons = icons;
			}
		else
			{
			TRAPD(err,icons = CApaAppIconArray::NewL());
			if(err == KErrNone)
				{
				delete iIcons;
				iIcons = icons;
				}
			}
			
		iTimeStamp = appInfoReader->TimeStamp();
		delete iLocalisableResourceFileName;
		iLocalisableResourceFileName = appInfoReader->LocalisableResourceFileName();
		iLocalisableResourceFileTimeStamp = appInfoReader->LocalisableResourceFileTimeStamp();

		if (isNonNativeApp)
			{
			// In the case of a non-native app, the resource file has been prefixed with a
			// TCheckedUid, the second of whose UIDs is the non-native application type uid.
			TEntry entry;
			__ASSERT_ALWAYS(iRegistrationFile, Panic(EPanicNullPointer));
			const TInt error=iFs.Entry(*iRegistrationFile, entry);
			if (error!=KErrNone)
				{
				delete appInfoReader;
				return error;
				}
			__ASSERT_DEBUG(entry.iType[0].iUid==KUidPrefixedNonNativeRegistrationResourceFile, Panic(EPanicUnexpectedUid));
			iNonNativeApplicationType=entry.iType[1];
			}

		delete iOpaqueData;
		iOpaqueData = appInfoReader->OpaqueData();

		if (readSuccessful)
			{
			appInfoReader->Capability(iCapabilityBuf);

			iDefaultScreenNumber = appInfoReader->DefaultScreenNumber();

			delete iIconFileName;
			iIconFileName = appInfoReader->IconFileName();
			iNonMbmIconFile = appInfoReader->NonMbmIconFile();
			iApplicationLanguage = appInfoReader->AppLanguage();
					
			// views
			iViewDataArray->ResetAndDestroy();
			CArrayPtrFlat<CApaAppViewData>* viewDataArray = appInfoReader->Views();
			if (viewDataArray)
				{
				delete iViewDataArray;
				iViewDataArray = viewDataArray;
				}

			// owned files
			iOwnedFileArray->Reset();
			CDesCArray* ownedFileArray = appInfoReader->OwnedFiles();
			if (ownedFileArray)
				{
				delete iOwnedFileArray;
				iOwnedFileArray = ownedFileArray;
				}
			
			UpdateServiceArray(appInfoReader->ServiceArray(iIndexOfFirstOpenService));
			}
		delete appInfoReader;
		}

	if (!caption)
		{
		TParsePtrC parse (*iFullName);
		caption = parse.Name().Alloc();
		}

	// Put the captions into place
	if (caption)
		{
		if (!shortCaption)
			{
			shortCaption = caption->Alloc();
			if (!shortCaption)
				{
				delete caption;
				caption = NULL;
				}
			}
		}
	if (caption)
		{
		delete iCaption;
		iCaption = caption;
		delete iShortCaption;
		iShortCaption = shortCaption;
		}

	return caption ? KErrNone : KErrNoMemory;
	}

EXPORT_C CApaAppData::~CApaAppData()
// Just delete components, NOT iNext (next CApaAppData in the list).
	{
	delete iSuccessor;
	delete iCaption;
	delete iShortCaption;
	delete iFullName;
	delete iIcons;

	if(iViewDataArray)
		{
		iViewDataArray->ResetAndDestroy();
		delete iViewDataArray;
		}
	delete iOwnedFileArray;
	delete iIconFileName;
	delete iLocalisableResourceFileName;
	if (iServiceArray)
		{
		CleanupServiceArray(iServiceArray);
		iServiceArray = NULL;
		}
	delete iOpaqueData;
	delete iRegistrationFile;
	iNext = NULL;
	}

void CApaAppData::UpdateServiceArray(CArrayFixFlat<TApaAppServiceInfo>* aNewServiceArray)
	{
	// clear out any existing service info
	if (iServiceArray)
		{
		CleanupServiceArray(iServiceArray);
		iServiceArray = NULL;
		}
	// store new service array
	iServiceArray = aNewServiceArray;
	}
	
TDataTypePriority CApaAppData::DataType(const TDataType& aDataType, const CArrayFixFlat<TDataTypeWithPriority>& aDataTypeArray) const
	{
	TInt count=aDataTypeArray.Count();
	for (TInt ii=0;ii<count;ii++)
		{
		const TDataTypeWithPriority& type=aDataTypeArray[ii];
		if (type.iDataType==aDataType)
			{
			return type.iPriority;
			}
		else
			{
			TPtrC8 src=type.iDataType.Des8();
			TPtrC8 trg=aDataType.Des8();
			if (src.Match(trg)==0 || trg.Match(src)==0)
				{
				if (type.iPriority == KDataTypePrioritySystem)
					{
					// This is more or less a magic number so don't decrement
					return KDataTypePrioritySystem;
					}
				else
					{
					return (TInt16)(type.iPriority-1);
					}
				}
			}
		}
	return KDataTypePriorityNotSupported;
	}

/**
 * Returns the CApaMaskedBitmap of size aSize for the application associated
 * with this CApaAppData. If there is not a bitmap of exact size aSize then 
 * the icon closest to but smaller than the one asked for is returned, or NULL if
 * none is smaller.
 * 
 * @since Uikon1.2
 */
EXPORT_C CApaMaskedBitmap* CApaAppData::Icon(TSize aSize) const
	{
	__ASSERT_ALWAYS(iIcons, Panic(EPanicNullPointer));
	return iIcons->IconBySize(aSize);
	}

/**
 * Returns a pointer to the small, medium or large application icon for aIconIndex equal to 0, 1 or 2 respectively.
 * Panics if aIconIndex is not one of these three values.
 *
 * This method is superseded by an overload which returns the icon by finding the closest match to a specified size.
 *
 * @deprecated
 */
EXPORT_C CApaMaskedBitmap* CApaAppData::Icon(TInt aIconIndex) const
	{
	__ASSERT_DEBUG(aIconIndex>-1 && aIconIndex<3, Panic(EDPanicBadIconSize)); //only support old behaviour
	TSize sizeExpected;
	switch(aIconIndex)
		{
	case KApaIconIndexSmall:
		sizeExpected=TSize(24,24);
		break;
	case KApaIconIndexMedium:
		sizeExpected=TSize(32,32);
		break;
	case KApaIconIndexLarge:
		sizeExpected=TSize(48,48);
		break;
	default:
		break;
		}
	return Icon(sizeExpected);
	}

EXPORT_C CArrayFixFlat<TSize>* CApaAppData::IconSizesL() const
/** Gets the sizes of icons available for the application. 

@return A pointer to an array of the icon sizes. The caller takes ownership. */
	{
	__ASSERT_ALWAYS(iIcons, Panic(EPanicNullPointer));;
	return iIcons->IconSizesL();
	}

EXPORT_C TApaAppEntry CApaAppData::AppEntry() const
/** Constructs an application entry based on this object.

@return The application entry. */
	{
	return TApaAppEntry(iUidType,*iFullName);
	}


EXPORT_C void CApaAppData::Capability(TDes8& aCapabilityBuf)const
/** Gets the application's capabilities.

@param aCapabilityBuf On return, contains the application's capabilities, 
specified as a TApaAppCapabilityBuf object. */
	{
	TApaAppCapability::CopyCapability(aCapabilityBuf,iCapabilityBuf);
	}

/**
 * Returns a pointer to the array of view data objects current for this application. Does not imply transfer of ownership.
 *
 * @since App-Framework_6.1
 */
EXPORT_C CArrayPtrFlat<CApaAppViewData>* CApaAppData::Views() const
	{
	return iViewDataArray;
	}

/**
 * Returns a pointer to the array of files for which this application claims ownership. Does not imply transfer of ownership.
 *
 * @since App-Framework_6.1
 */
EXPORT_C CDesCArray* CApaAppData::OwnedFiles() const
	{
	return iOwnedFileArray;
	}

TBool CApaAppData::Update(const TDesC& aDefaultAppIconFileName)
// returns true if changes were made to the cached data
	{
	__APA_PROFILE_START(17);
	TBool changed=EFalse;

	// Get app info file entry
	TEntry entry;
	TInt ret;
	if (iRegistrationFile != NULL)
		{
		ret = iFs.Entry(*iRegistrationFile, entry);
		if (ret==KErrNone && entry.iModified!=iTimeStamp)
			{
			// assume registration file may have changed
			changed = ETrue;
			}
		else
			{
			if (iLocalisableResourceFileName)
				{
				// see if localisable resource information might have changed
				TParse parse;
				ret = parse.SetNoWild(KAppResourceFileExtension, iLocalisableResourceFileName, NULL);
				if (ret == KErrNone)
					{
					TFileName resourceFileName(parse.FullName());
					BaflUtils::NearestLanguageFile(iFs, resourceFileName);
					if (resourceFileName.CompareF(*iLocalisableResourceFileName)!=0)
						{
						changed = ETrue;
						}
					else
						{
						ret = iFs.Entry(*iLocalisableResourceFileName, entry);
						if ((ret==KErrNotFound && iLocalisableResourceFileTimeStamp!=TTime(0)) ||
							(ret==KErrNone && entry.iModified!=iLocalisableResourceFileTimeStamp))
							{
							changed = ETrue;
							}
						}
					}
				}
			}
		}
	if (changed)
		{
		// Ignore result, nothing we can do in case failure
		// and the old values should be preserved
        const TInt ignore = StoreApplicationInformation(aDefaultAppIconFileName);
		} //lint !e529 Suppress symbol 'ignore' not subsequently referenced
		
	__APA_PROFILE_END(17);
	return changed;
	}

EXPORT_C TDataTypePriority CApaAppData::DataType(const TDataType& aDataType) const
// returns the priority of the data type
/** If the application supports the specified data type, the function returns 
the priority with which it should be selected for handling it.

If the application does not support the specified data type, 
KDataTypePriorityNotSupported is returned.

Note that the function supports wildcard matching, using "*" and "?". In the case 
of a wildcard match, the priority value returned is reduced by 1, so that in this 
case, the application could never have the maximum priority 
(KDataTypePriorityUserSpecified).

@param aDataType The data type of interest.
@return The priority with which the application should be selected for handling 
the specified data type, or KDataTypePriorityNotSupported if the data type is 
not supported. */
	{
	if (iIndexOfFirstOpenService >= 0)
		{
		__ASSERT_ALWAYS(iServiceArray, Panic(EPanicNullPointer));
		const CArrayFixFlat<TDataTypeWithPriority>& dataTypeArray = 
			(*iServiceArray)[iIndexOfFirstOpenService].DataTypes();
		return DataType(aDataType, dataTypeArray);
		}
	return KDataTypePriorityNotSupported;
	}


EXPORT_C TBool CApaAppData::IsPending() const
	{
	return (iIsPresent==CApaAppData::EPresentPendingUpdate 
		|| iIsPresent==CApaAppData::ENotPresentPendingUpdate);
	}

EXPORT_C TBool CApaAppData::CanUseScreenMode(TInt aScreenMode)
/** Tests whether the specified screen mode is valid for any of 
this application's views. If the app has no views, the function 
assumes that only the default screen mode (at screen mode index 
zero) is allowed. This function is used by CApaAppList to create 
a list of valid applications.

@param aScreenMode The index of the screen mode.
@return True if screen mode is valid, otherwise false. */
	{
	const TInt count=iViewDataArray->Count();
	// If there are no views, assume only the default screen mode is allowed
	TBool ret=(count==0 && aScreenMode==0);
	for(TInt ii=0;ii<count;ii++)
		{
		const CApaAppViewData* data=(*iViewDataArray)[ii];
		if(data->ScreenMode()==aScreenMode)
			{
			ret=ETrue;
			break;
			}
		}
	return ret;
	}

EXPORT_C void CApaAppData::GetIconInfo(TInt& aIconCount, TInt& aDefaultIconsUsed) const
/** Gets icon information for the app

@param aIconCount On return, this contains the number of app icons
@param aDefaultIconsUsed On return, this indicates whether the default icons have been used
*/
	{
	__ASSERT_ALWAYS(iIcons, Panic(EPanicNullPointer));;
	aIconCount = iIcons->Count();
	aDefaultIconsUsed = iIcons->DefaultIconsUsed();
	}

/** Gets the default screen number used by the application.

A device may have more than once screen. This function
returns the number associated with the screen which will
be the default screen used by the application.

@return The default screen number
*/
EXPORT_C TUint CApaAppData::DefaultScreenNumber() const
	{
	return iDefaultScreenNumber;
	}

/** Returns true if app info was provided by a registration file

@return true if app info was provided by a registration file
*/
EXPORT_C TBool CApaAppData::RegistrationFileUsed() const
	{
	return iRegistrationFile != NULL;
	}

/** Returns the full filename of the registration resource file

@return The full path and filename of the registration resource file.
@internalTechnology
*/
EXPORT_C TPtrC CApaAppData::RegistrationFileName() const
	{
	if (iRegistrationFile)
		{
		return *iRegistrationFile;
		}
	else
		{
		return TPtrC(KNullDesC);
		}
	}


/** Returns the full filename of the localisable resource file

@return The full path and filename of the localisable resource file.
@internalTechnology
*/
EXPORT_C TPtrC CApaAppData::LocalisableResourceFileName() const
	{
	if (iLocalisableResourceFileName)
		{
		return *iLocalisableResourceFileName;
		}
	else
		{
		return TPtrC(KNullDesC);
		}
	}


/** Returns the non-native application opaque data

@return The non-native application opaque data.
@internalComponent
*/
EXPORT_C TPtrC8 CApaAppData::OpaqueData() const
	{
	if (iOpaqueData)
		{
		return *iOpaqueData;
		}
	else
		{
		return TPtrC8(KNullDesC8);
		}
	}

EXPORT_C TUid CApaAppData::NonNativeApplicationType() const
/** @internalComponent */
	{
	return iNonNativeApplicationType;
	}

/** Returns the full filename of the file containing application icons

@return The full path and filename of the icon file.
*/
EXPORT_C TPtrC CApaAppData::IconFileName() const
	{
	if (iIconFileName)
		{
		return *iIconFileName;
		}
	else
		{
		return TPtrC(KNullDesC);
		}
	}

/** Returns true if the application provides a non-MBM icon filename.

If this function returns false, this does not necessarily mean
an MBM icon filename is provided.

@return true if the application provides a non-MBM icon filename.
*/
EXPORT_C TBool CApaAppData::NonMbmIconFile() const
	{
	return iNonMbmIconFile;
	}


/** Determines the current language the application is using to display its
user interface.
@return The current language.
*/	
EXPORT_C TLanguage CApaAppData::ApplicationLanguage() const
	{
	return iApplicationLanguage;
	}

/** Returns true if the application implements the specified service.
@param aServiceUid The service UID.
@return true if the application implements the specified service.
@internalComponent
*/
EXPORT_C TBool CApaAppData::ImplementsService(TUid aServiceUid) const
	{
	if (iServiceArray)
		{
		TInt count = iServiceArray->Count();
		for (TInt i = count-1; i >= 0; i--)
			{
			if ((*iServiceArray)[i].Uid() == aServiceUid)
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}
	
/** Checks if the application implements the specified service and if the 
service explicitly supports the datatype. Explicitly means that the datatype is
listed in the service's list of datatype in the registration file and is
not the general datatype associated with the application (aka the Open service).
@param aServiceUid The service UID.
@param aDataType The datattype
@return The priority. KDataTypePriorityNotSupported if the app doesn't support
this service with this datatype.
@internalComponent
*/
TInt CApaAppData::ImplementsServiceWithDataType(TUid aServiceUid, const TDataType& aDataType) const
	{
	TInt result = KDataTypePriorityNotSupported;
	if (iServiceArray)
		{
		TInt count = iServiceArray->Count();
		for (TInt i = count-1; i >= 0; i--)
			{
			// There can be more than one instance of a given service so we iterate
			// through the whole service list even if we have already found a suitable
			// service.
			if ((*iServiceArray)[i].Uid() == aServiceUid)
				{
				const CArrayFixFlat<TDataTypeWithPriority>& datatypes =
					(*iServiceArray)[i].DataTypes();
				TInt priority = DataType(aDataType, datatypes);
				if (priority > result)
					{
					result = priority;
					}
				}
			}
		}
	return result;
	}

EXPORT_C void CApaAppData::SetShortCaptionL(const TDesC& aShortCaption)
	{
	__ASSERT_ALWAYS(iShortCaption, Panic(EPanicNullPointer));
	if(iShortCaption->Compare(aShortCaption) != 0)
		{
		HBufC* newShortCaption=aShortCaption.AllocL();
		delete iShortCaption; // after the AllocL succeeds
		iShortCaption = newShortCaption;
		}
	}

//
// Class CApaAppList
//

EXPORT_C CApaAppList* CApaAppList::NewL(RFs& aFs,CApaAppRegFinder* aAppRegFinder,TBool aLoadMbmIconsOnDemand, TInt aIdlePeriodicDelay)
	{
	CApaAppList* self=new CApaAppList(aFs, aAppRegFinder,aLoadMbmIconsOnDemand, aIdlePeriodicDelay);
	if (!self)
		{
		delete aAppRegFinder;
		User::LeaveNoMemory();
		}
	else
		{
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		}
	return self;
	}

CApaAppList::CApaAppList(RFs& aFs, CApaAppRegFinder* aAppRegFinder,TBool aLoadMbmIconsOnDemand, TInt aIdlePeriodicDelay)
	:iFs(aFs),
	iFlags(0),
	iAppRegFinder(aAppRegFinder),
	iIdlePeriodicDelay(aIdlePeriodicDelay),
	iLoadMbmIconsOnDemand(aLoadMbmIconsOnDemand)
	{
	}
	
void CApaAppList::ConstructL()
	{
	User::LeaveIfError(iFsShareProtected.Connect());
	User::LeaveIfError(iFsShareProtected.ShareProtected());
	User::LeaveIfError(Dll::SetTls(this));
	
	//Start language change monitor.
	iAppLangMonitor = CApaLangChangeMonitor::NewL(*this);
	}
	
EXPORT_C CApaAppList::~CApaAppList()
/** Frees all resources prior to destruction, including the application data list. */
	{
	iValidFirstAppData = NULL;
	iFlags&=~EFirstScanComplete;
	CApaAppData* next = NULL;
	for (CApaAppData* appData=iAppData; appData; appData = next)
		{
		next = appData->iNext;
		delete appData;
		}
	iAppData = NULL;
	iObserver = NULL;
	
	iFsShareProtected.Close();
	
	delete iDefaultIconArray;
	delete iDefaultAppIcon;
	delete iAppRegFinder;
	delete iAppIdler;
	}

EXPORT_C void CApaAppList::StartIdleUpdateL()
/** Updates the list asynchronously, using an idle time active object, 
and no observer. When the update is finished, the resulting app 
list is stored. */
	{

	// find the default icons (.mbm file) for applications, wrt current locale
	TFileName* tempFileName = new(ELeave) TFileName;
	CleanupStack::PushL(tempFileName);
	tempFileName->Append(KDefaultAppIconMbm);
	BaflUtils::NearestLanguageFile(iFs, *tempFileName); 
	delete iDefaultAppIcon;
	iDefaultAppIcon = NULL;	
	iDefaultAppIcon = HBufC::NewL(tempFileName->Size());
	
	TPtr iconptr = iDefaultAppIcon->Des();
	iconptr.Append(*tempFileName);
	CleanupStack::PopAndDestroy(tempFileName); 
	
	// DEF076594 - if changing locale, need to update the default icons here
	// If updating the default icons array fails, the array is left in the state
	// it was in before the call.	
	if(iDefaultIconArray)
		{
		TRAP_IGNORE(UpdateDefaultIconsL());
		}
	
	// start to scan.
	if (iAppIdler)
		{
		delete iAppIdler;
		iAppIdler=NULL;
		}

	// DEF072701
	// When performing the update scan let the idle object have lower priority.
	if (IsFirstScanComplete())
		{
		iAppIdler=CPeriodic::NewL(CActive::EPriorityLow);
		}
	else
		{
		iAppIdler=CPeriodic::NewL(CActive::EPriorityStandard);
		}
	SetPending(iAppData);
	iCurrentApp=TApaAppEntry();
	iScanStage=EFirstStageScan;
	__ASSERT_ALWAYS(iAppRegFinder, Panic(EPanicNullPointer));
	iAppRegFinder->FindAllAppsL();
 
 	// DEF072701
 	// If this is the first scan i.e the boot scan then it may take some time. Thus
 	// the periodic delay value should be used so that this process will stop periodically 
 	// to allow time for other processes.
 	// If this is just a re-scan it should take much less time. Therefore it should just
 	// be completed in one go rather than periodically delayed. Thus the delay value
 	// should be set to 0.
	iAppIdler->Start(KIdleStartDelay, IsFirstScanComplete()? 0 : iIdlePeriodicDelay, TCallBack(IdleUpdateCallbackL, this));
	}

EXPORT_C void CApaAppList::StartIdleUpdateL(MApaAppListObserver* aObserver)
/** Updates the list asynchronously, using an idle time active object 
and an observer. When the update is finished, the resulting app list 
is stored and the observer is notified with an MApaAppListServObserver::EAppListChanged 
message, if the list changed.
@param aObserver Observer to be notified when the update has finished. */
	{
	iObserver=aObserver;
	if (iAppIdler==NULL)
		{
		// if iAppIdler exists then we're already in the middle of an update and
		// may have changed iUpdateCounter without yet reporting the changes
		iOldUpdateCounter=iUpdateCounter;
		}
	
	StartIdleUpdateL();
	}

// Stop scanning applications if and uninstallation has started	
EXPORT_C void CApaAppList::StopScan()
	{
	if (iAppIdler)
		{
		delete iAppIdler;
		iAppIdler=NULL;
		}
	UndoSetPending(iAppData);
	}
	
// Allow scanning when uninstallation is complete
EXPORT_C void CApaAppList::RestartScanL()
	{
	TRAP_IGNORE(PurgeL());
	StartIdleUpdateL();
	}

EXPORT_C void CApaAppList::InitListL(MApaAppListObserver* aObserver)
/** Starts updating the list asynchronously, by calling StartIdleUpdateL().

@param aObserver Observer to be notified when the update has finished. */
	{
	StartIdleUpdateL(aObserver);
	}
	
TInt CApaAppList::IdleUpdateCallbackL(TAny* aObject)
	{
	CApaAppList* self=reinterpret_cast<CApaAppList*>(aObject);
	const TBool moreToCome=self->IdleUpdateL();
	if (moreToCome==EFalse)
		{
		if(self->iScanStage==EFirstStageScan)
			{
			self->iScanStage=EScanFinished;
			self->StopIdlerL();	
			}
		}
	return moreToCome;
	}

void CApaAppList::StopIdlerL()
	{
 
	delete iAppIdler;
	iAppIdler=NULL;
		
	// now that the scan is finished, iDefaultAppIcon is deleted
	delete iDefaultAppIcon;
	iDefaultAppIcon=NULL;
			
	if (iObserver)
		{
		if (iOldUpdateCounter!=iUpdateCounter)
			{
			//NotifyUpdate will notify clients for both list update and scan complete.
			iObserver->NotifyUpdate(MApaAppListServObserver::EAppListChanged);
			}
		else
			{
			//NotifyScanComplete will notify clients for scan complete.
			iObserver->NotifyScanComplete();
			}
			
		iObserver=NULL;
		}
	}

TInt CApaAppList::IdleUpdateL()
// returns ETrue if there is more scanning to be done.
	{
	TBool more=EFalse;
	__ASSERT_ALWAYS(iAppRegFinder, Panic(EPanicNullPointer));
	TRAPD(err,more=iAppRegFinder->NextL(iCurrentApp, iForcedRegistrations));
	if (err!=KErrNone)
		return more;
	TBool hasChanged=EFalse;
	if (more)
		{
		TRAPD(err,UpdateNextAppL(iCurrentApp,hasChanged));
		if (err!=KErrNone)
			{
			SetNotFound(iAppData,hasChanged);
			more=EFalse; // abandon ship
			}
		}
	else
		{
		SetNotFound(iAppData,hasChanged);
		PurgeL();
		}
	//
	if (hasChanged)
		iUpdateCounter++;
	return more;
	}

EXPORT_C TBool CApaAppList::IsIdleUpdateComplete() const
/** Tests whether an asynchronous update of the list is currently in progress.

@return True if no asynchronous update of the list is currently in progress, 
otherwise false. */
	{
	return iAppIdler == NULL;
	}

void CApaAppList::SetPending(CApaAppData* aAppData)
	// set all apps to pending update - we'll find them again as we scan
	{
  	for (; aAppData; aAppData = aAppData->iNext)
		{
		if (aAppData->iIsPresent==CApaAppData::ENotPresent 
			|| aAppData->iIsPresent==CApaAppData::ENotPresentPendingUpdate)
			{
			aAppData->iIsPresent = CApaAppData::ENotPresentPendingUpdate;
			}
		else
			{
			aAppData->iIsPresent = CApaAppData::EPresentPendingUpdate;
			}
		}
	}

void CApaAppList::UndoSetPending(CApaAppData* aAppData)
	// Reset all apps to pevious pending state so they don't get purged
	{
  	for (; aAppData; aAppData = aAppData->iNext)
		{
		if (aAppData->iIsPresent == CApaAppData::EPresentPendingUpdate)
			{
			aAppData->iIsPresent = CApaAppData::EIsPresent;
			}
		}
	}

void CApaAppList::SetNotFound(CApaAppData* aAppData, TBool& aHasChanged)
	// mark any unfound apps not present
	{
	while (aAppData)
		{
		if (aAppData->IsPending())
			{
			aAppData->iIsPresent = CApaAppData::ENotPresent;
			aHasChanged = ETrue;
			}
		aAppData = aAppData->iNext;
		}
	}

void CApaAppList::AddToList( CApaAppData* aAppData )
{
	__ASSERT_DEBUG(aAppData, Panic(EPanicNullPointer));
	aAppData->iNext=iAppData;
	iAppData=aAppData;
}

void CApaAppList::UpdateNextAppL(const TApaAppEntry& aAppEntry,TBool& aHasChanged)
	{
	CApaAppData* appData=AppDataByUid(aAppEntry.iUidType[2]);
	if (appData==NULL)
		{// not in list, so add it at the start
		TRAPD(err,appData=CApaAppData::NewL(aAppEntry, iFs, *iDefaultAppIcon));
		if (err==KErrNone)
			{
			AddToList( appData );
			aHasChanged=ETrue;
			}
		}
	else if (appData->IsPending())
		{ // not found yet during current scan - we may need to override this one
		
		// On a system which scans for registration .RSC files (V2 apps) first, followed by
		// .APP files (V1 apps), it's valid for a V1 app to override a V2 app (if the V2 app
		// has just been removed). If this is the case, assume it's ok to compare the V1 .APP filename,
		// with the V2 .RSC filename as their filenames will never match (which is what we want in this case).
		TPtrC currentFileName;
		if (appData->RegistrationFileUsed())
			{
			currentFileName.Set(*appData->iRegistrationFile);
			}
		else
			{
			currentFileName.Set(*appData->iFullName);
			}
		if (aAppEntry.iFullName.CompareF(currentFileName)!=0)
			{
			delete appData->iSuccessor;
			appData->iSuccessor = NULL;
			appData->iSuccessor = CApaAppEntry::NewL(aAppEntry);


			appData->iIsPresent = CApaAppData::ESuperseded;
			aHasChanged=ETrue;
			}
		else
			{
			if (appData->Update(*iDefaultAppIcon)
				|| appData->iIsPresent==CApaAppData::ENotPresentPendingUpdate) 
				{
				aHasChanged=ETrue; 
				}
			appData->iIsPresent = CApaAppData::EIsPresent;
			}
		}
	
	}

/**
@internalComponent
*/
EXPORT_C CApaAppData* CApaAppList::FindAndAddSpecificAppL(CApaAppRegFinder* aFinder, TUid aAppUid)
	{
//Scans and adds the specified application to the app list if found
	__ASSERT_DEBUG(aFinder, Panic(EPanicNullPointer));
	TBool found = EFalse;
	TApaAppEntry appEntry;
	aFinder->FindAllAppsL();
	while (aFinder->NextL(appEntry, iForcedRegistrations))
		{
		if (appEntry.iUidType[2] == aAppUid)
			{
			found = ETrue;
			break;
			}
		}
	
	CApaAppData* app = NULL;
	if (found)
		{
		// add the app to the list
		TBool hasChanged = EFalse;
		CApaAppData* prevFirstAppInList = iAppData;
		UpdateNextAppL(appEntry, hasChanged);
		if (iAppData != prevFirstAppInList)
			{
			// assume the new app was added to the list
			app = iAppData;
			}
		if (hasChanged)
			iUpdateCounter++;
		}
	return app;
	}

EXPORT_C void CApaAppList::PurgeL()
/** Removes any applications from the list if they are no longer present 
on the phone. It updates applications that have been 
superceded. */
	{
	CApaAppData* appData=iAppData;
	CApaAppData* prev=NULL;
	while (appData)
		{
		CApaAppData* next=appData->iNext;
		if (appData->iIsPresent==CApaAppData::ENotPresent)
			{
			if (prev)
				prev->iNext=next;
			else
				iAppData=next;
			delete appData;
			}
		else if (appData->iIsPresent==CApaAppData::ESuperseded)
			{
			CApaAppData* newApp=NULL;
			TApaAppEntry appEntry;
			appData->iSuccessor->Get(appEntry);
			TRAPD(err,newApp=CApaAppData::NewL(appEntry, iFs,*iDefaultAppIcon));
			if (err==KErrNone)
				{
				// remove the old one and add the new one in its place
				if (prev)
					prev->iNext=newApp;
				else
					iAppData=newApp;
				newApp->iNext = appData->iNext;
				delete appData;
				// increment the iterator
				prev = newApp;
				}
			}
		else
			prev=appData;
		appData=next;
		}
 
	ScanComplete();
	}

EXPORT_C TInt CApaAppList::Count() const
/** Gets the count of applications present in the app list.

@return The number of applications in the list. */

	{
	TInt count=0;
	CApaAppData* appData=iAppData;
	while (appData)
		{
		count++;
		appData=appData->iNext;
		}
	return count;
	}

EXPORT_C CApaAppData* CApaAppList::FirstApp() const
/** Gets a pointer to the first application in the list 
that can use the default screen mode.

@return A pointer to the first application. */
	{
	return FirstApp(0);
	}

EXPORT_C CApaAppData* CApaAppList::FirstApp(TInt aScreenMode) const
/** Gets a pointer to the first application in the list 
that can use the specified screen mode.

@param aScreenMode The index of the screen mode. Specifying 
KIgnoreScreenMode returns the first application in the list, 
regardless of screen mode.
@return A pointer to the first application that can use the 
specified screen mode. */
	{

	CApaAppData* appData=iValidFirstAppData;

	if(aScreenMode!=KIgnoreScreenMode)
		{
		while (appData && !appData->CanUseScreenMode(aScreenMode))
			appData = appData->iNext;
		}

	return appData;
	}

EXPORT_C CApaAppData* CApaAppList::NextApp(const CApaAppData* aApp) const
/** Gets a pointer to the next application after aApp in the list 
that can use the default screen mode.

@param aApp A pointer to an application in the list.
@return A pointer to the next application after aApp in the list 
that can use the default screen mode.
@panic APGRFX 12 aApp is NULL. */
	{
	return NextApp(aApp,0);
	}

EXPORT_C CApaAppData* CApaAppList::NextApp(const CApaAppData* aApp, TInt aScreenMode) const
/** Gets a pointer to the next application after aApp in the list 
that can use the specified screen mode.

@param aApp A pointer to an application in the list. 
@param aScreenMode The index of the screen mode. Specifying 
KIgnoreScreenMode returns the next application in the list, 
regardless of screen mode.
@return A pointer to the next application after aApp in the list 
that can use the specified screen mode.
@panic APGRFX 12 aApp is NULL. */

	{
	__ASSERT_ALWAYS(aApp,Panic(EPanicNoAppDataSupplied));
	//
	CApaAppData* iApp=aApp->iNext; //lint !e613 Possible use of null pointer - Asserted above

	if(aScreenMode!=KIgnoreScreenMode)
		while (iApp && !iApp->CanUseScreenMode(aScreenMode))
			iApp = iApp->iNext;

	return iApp;
	}

EXPORT_C CApaAppData* CApaAppList::AppDataByUid(TUid aAppUid) const
/** Gets a pointer to the application in the list whose third 
UID matches the specified UID.

@param aAppUid An application's third UID. 
@return A pointer to the application, if successful. Null, 
if no match is found or if KNullUid was specified. */
	{
	if (aAppUid==KNullUid)
		return NULL; // never match null UID as it represents an un-UIDed file
	CApaAppData* appData=iAppData;
	while (appData)
		{
		if (appData->AppEntry().iUidType[2]==aAppUid)
			return appData;
		appData=appData->iNext;
		}
	return NULL;
	}

EXPORT_C CApaAppData* CApaAppList::AppDataByFileName(const TDesC& aFullFileName) const
/** Gets a pointer to the application in the list whose application
file name matches the one specified

@param aFullFileName a file name. 
@return A pointer to the application, if successful. Null, 
if no match is found or if KNullDesC was specified.
@internalTechnology
*/
	{
	if (aFullFileName.Length()==0)
		{
		return NULL; // never match null UID as it represents an un-UIDed file
		}
	CApaAppData* appData=iAppData;
	while (appData)
		{
		if (appData->AppEntry().iFullName.CompareF(aFullFileName)==0)
			{
			return appData;
			}
		appData=appData->iNext;
		}
	return NULL;
	}
	
/**
Adds a registration file to the iForcedRegistrations array.

@param aRegistrationFile The function takes ownership of the HBufC.
@internalComponent
*/
EXPORT_C void CApaAppList::AddForcedRegistrationL(HBufC* aRegistrationFile)
	{
	TInt err = iForcedRegistrations.InsertInOrder(aRegistrationFile, TLinearOrder<HBufC>(CompareStrings));
	if (err == KErrAlreadyExists) // We silently ignore attempts to insert duplicates
		{
		delete aRegistrationFile;
		}
	else
		{
		User::LeaveIfError(err);
		}
	} //lint !e818 Suppress pointer parameter 'aRegistrationFile' could be declared as pointing to const
	
EXPORT_C void CApaAppList::ResetForcedRegistrations()
	{
	iForcedRegistrations.ResetAndDestroy();
	}

EXPORT_C TInt CApaAppList::UpdateCounter() const
/** Gets the number of times the application list has been updated. 
It returns zero for a newly created empty list.

@return The number of times the list has been changed since it was 
created. */
	{
	return iUpdateCounter;
	}

EXPORT_C TUid CApaAppList::PreferredDataHandlerL(const TDataType& aDataType) const
/** Finds the preferred application to handle the specified data type.

@param aDataType The data type of interest.
@return The third UID of the application in the list which has the 
highest priority for handling the specified data type. A null UID is 
returned if no application in the list can handle the specified data type. */
	{
	TInt dummy;
	return PreferredDataHandlerL(aDataType, NULL, dummy);
	}
	
EXPORT_C TUid CApaAppList::PreferredDataHandlerL(const TDataType& aDataType, const TUid* aServiceUid, TInt& aPriority) const
/** Finds the preferred application to handle the specified data type.

@param aDataType The data type of interest.
@param aServiceUid The optional service UID.
@param aPriority The priority associated with the returned app.
@return The third UID of the application in the list which has the 
highest priority for handling a combination of the specified data type
and service. A null UID is returned if no application in the list can
handle the combination of specified data type and service.
@internalComponent
*/
	{
	// If there is a service uid we first try to use the service specific list
	// of datatypes
	if (aServiceUid)
		{
		CApaAppData* appData=iAppData;
		aPriority=KDataTypePriorityNotSupported;
		TUid uid={0};
		while (appData)
			{
			TInt priority = appData->ImplementsServiceWithDataType(*aServiceUid, aDataType);
			if (priority > aPriority)
				{
				aPriority=priority;
				uid=appData->AppEntry().iUidType[2];
				}
			appData=appData->iNext;
			}
		if (aPriority != KDataTypePriorityNotSupported)
			{
			return uid;
			}
		}
	
	CApaAppData* appData=iAppData;
	aPriority=KDataTypePriorityNotSupported;
	TUid uid={0};
	while (appData)
		{
		TInt priority=appData->DataType(aDataType);
		if ((priority > aPriority) &&
			(!aServiceUid || (aServiceUid && appData->ImplementsService(*aServiceUid))))
			{
			aPriority=priority;
			uid=appData->AppEntry().iUidType[2];
			}
		appData=appData->iNext;
		}
	return uid;
	}

void CApaAppList::ScanComplete()
	{
	if(!IsFirstScanComplete() && iObserver)
		iObserver->InitialListPopulationComplete();
	iValidFirstAppData = iAppData;
	iFlags|=EFirstScanComplete;
	}

/**
 *
 * Tests whether the first scan for list of Apps has completed.
 *
 * @return   "TBool"
 *            True, if the first scan for list of Apps has completed; false, otherwise.
 * @internalComponent
 */
EXPORT_C TBool CApaAppList::IsFirstScanComplete() const
	{
	return iFlags&EFirstScanComplete;
	}

EXPORT_C TBool CApaAppList::AppScanInProgress() const
/** @internalComponent */
	{
	return (iAppIdler!=NULL) && iAppIdler->IsActive();
	}

/**
@internalComponent
*/
EXPORT_C CBufFlat* CApaAppList::ServiceArrayBufferL(TUid aAppUid) const
	{
	CApaAppData* app = FirstApp(KIgnoreScreenMode);
	while (app && app->AppEntry().iUidType[2] != aAppUid)
		{
		app = NextApp(app, KIgnoreScreenMode);
		}

	if (app)
		{
		if (!app->RegistrationFileUsed())
			{
			User::Leave(KErrNotSupported);
			}
		if (app->iServiceArray)
			{
			CBufFlat* const buf = CBufFlat::NewL(KBufferExpansionGranularity);
			CleanupStack::PushL(buf);
			RBufWriteStream writeStream(*buf);
			writeStream << *(app->iServiceArray);
			writeStream.CommitL();
			writeStream.Release();
			CleanupStack::Pop(buf);
			return buf;
			}
		}
	User::Leave(KErrNotFound);
	return NULL; // to keep compiler happy
	}

/**
@internalComponent
*/
EXPORT_C CBufFlat* CApaAppList::ServiceUidBufferL(TUid aAppUid) const
	{
	CApaAppData* app = FirstApp(KIgnoreScreenMode);
	while (app && app->AppEntry().iUidType[2] != aAppUid)
		{
		app = NextApp(app, KIgnoreScreenMode);
		}

	if (app)
		{
		if (!app->RegistrationFileUsed())
			{
			User::Leave(KErrNotSupported);
			}
		if (app->iServiceArray)
			{
			CArrayFixFlat<TApaAppServiceInfo>& serviceArray = *(app->iServiceArray);
			CArrayFixFlat<TUid>* uidArray = new(ELeave) CArrayFixFlat<TUid>(4);
			CleanupStack::PushL(uidArray);
			for (TInt i = serviceArray.Count()-1; i >= 0; i--)
				{
				uidArray->AppendL(serviceArray[i].Uid());
				}
			CBufFlat* const buf = CBufFlat::NewL(KBufferExpansionGranularity);
			CleanupStack::PushL(buf);
			RBufWriteStream writeStream(*buf);
			writeStream << *uidArray;
			writeStream.CommitL();
			writeStream.Release();
			CleanupStack::Pop(buf);
			CleanupStack::PopAndDestroy(uidArray);
			return buf;
			}
		}
	User::Leave(KErrNotFound);
	return NULL; // to keep compiler happy
	}

/**
@internalComponent
*/
EXPORT_C CBufFlat* CApaAppList::ServiceOpaqueDataBufferL(TUid aAppUid, TUid aServiceUid) const
	{
	CApaAppData* app = FirstApp(KIgnoreScreenMode);
	while (app && app->AppEntry().iUidType[2] != aAppUid)
		{
		app = NextApp(app, KIgnoreScreenMode);
		}

	if (app)
		{
		if (!app->RegistrationFileUsed())
			{
			User::Leave(KErrNotSupported);
			}
		if (app->iServiceArray)
			{
			CArrayFixFlat<TApaAppServiceInfo>* implArray = NULL;
			CArrayFixFlat<TApaAppServiceInfo>& serviceArray = *(app->iServiceArray);
			for (TInt i = serviceArray.Count()-1; i >= 0; i--)
				{
				const TApaAppServiceInfo& infoRef = serviceArray[i];
				if (infoRef.Uid() == aServiceUid)
					{
					if (!implArray)
						{
						implArray = new(ELeave) CArrayFixFlat<TApaAppServiceInfo>(4);
						CleanupStack::PushL(TCleanupItem(CleanupServiceArray, implArray));
						}
					CArrayFixFlat<TDataTypeWithPriority>* dummy =
						new(ELeave) CArrayFixFlat<TDataTypeWithPriority>(1);
					CleanupStack::PushL(dummy);					
					TApaAppServiceInfo info(aServiceUid, dummy, infoRef.OpaqueData().AllocLC());
					implArray->AppendL(info);
					CleanupStack::Pop(const_cast<TDesC8*>(&info.OpaqueData()));
					CleanupStack::Pop(dummy);
					}
				}
			if (implArray)
				{
				CBufFlat* const buf = CBufFlat::NewL(KBufferExpansionGranularity);
				CleanupStack::PushL(buf);
				RBufWriteStream writeStream(*buf);
				writeStream << *implArray;
				writeStream.CommitL();
				writeStream.Release();
				CleanupStack::Pop(buf);
				CleanupStack::PopAndDestroy(implArray);
				return buf;
				}
			}
		}
	User::Leave(KErrNotFound);
	return NULL; // to keep compiler happy
	}

/**
@internalComponent
*/
EXPORT_C CBufFlat* CApaAppList::ServiceImplArrayBufferL(TUid aServiceUid) const
	{
	CApaAppData* app = FirstApp(KIgnoreScreenMode);
	// build an array containing all implementations of the service identified by aServiceUid
	CArrayFixFlat<TApaAppServiceInfo>* implArray = NULL;
	while (app)
		{
		if (app->iServiceArray)
			{
			for (TInt i = app->iServiceArray->Count()-1; i >= 0; i--)
				{
				const TApaAppServiceInfo& infoRef = (*app->iServiceArray)[i];
				if (infoRef.Uid() == aServiceUid)
					{
					if (!implArray)
						{
						implArray = new(ELeave) CArrayFixFlat<TApaAppServiceInfo>(4);
						CleanupStack::PushL(TCleanupItem(CleanupServiceArray, implArray));
						}
					CArrayFixFlat<TDataTypeWithPriority>* datatypes = DataTypeArrayDeepCopyLC(infoRef.DataTypes());
					HBufC8* data = infoRef.OpaqueData().AllocLC();
					TApaAppServiceInfo info(app->AppEntry().iUidType[2], datatypes, data);
					implArray->AppendL(info);
					CleanupStack::Pop(data);
					CleanupStack::Pop(datatypes);
					}
				}
			}
		app = NextApp(app, KIgnoreScreenMode);
		}

	if (implArray)
		{
		CBufFlat* const buf = CBufFlat::NewL(KBufferExpansionGranularity);
		CleanupStack::PushL(buf);
		RBufWriteStream writeStream(*buf);
		writeStream << *(implArray);
		writeStream.CommitL();
		writeStream.Release();
		CleanupStack::Pop(buf);
		CleanupStack::PopAndDestroy(implArray);
		return buf;
		}
	User::Leave(KErrNotFound);
	return NULL; // to keep compiler happy
	}

EXPORT_C CBufFlat* CApaAppList::ServiceImplArrayBufferL(TUid aServiceUid, const TDataType& aDataType) const
	{
	CApaAppData* app = FirstApp(KIgnoreScreenMode);
	// build an array containing all implementations of the service identified by aServiceUid
	CArrayFixFlat<TApaAppServiceInfo>* implArray = NULL;
	while (app)
		{
		if (app->iServiceArray)
			{
			for (TInt i = app->iServiceArray->Count()-1; i >= 0; i--)
				{
				const TApaAppServiceInfo& infoRef = (*app->iServiceArray)[i];
				if (infoRef.Uid() == aServiceUid)
					{
				 	if (KDataTypePriorityNotSupported != app->DataType(aDataType, infoRef.DataTypes()))
						{
						if (!implArray)
							{
							implArray = new(ELeave) CArrayFixFlat<TApaAppServiceInfo>(4);
							CleanupStack::PushL(TCleanupItem(CleanupServiceArray, implArray));
							}
						CArrayFixFlat<TDataTypeWithPriority>* datatypes = DataTypeArrayDeepCopyLC(infoRef.DataTypes());
						HBufC8* data = infoRef.OpaqueData().AllocLC();
						TApaAppServiceInfo info(app->AppEntry().iUidType[2], datatypes, data);
						implArray->AppendL(info);
						CleanupStack::Pop(data);
						CleanupStack::Pop(datatypes);
						}
					}
				}
			}
		app = NextApp(app, KIgnoreScreenMode);
		}

	if (implArray)
		{
		CBufFlat* const buf = CBufFlat::NewL(KBufferExpansionGranularity);
		CleanupStack::PushL(buf);
		RBufWriteStream writeStream(*buf);
		writeStream << *(implArray);
		writeStream.CommitL();
		writeStream.Release();
		CleanupStack::Pop(buf);
		CleanupStack::PopAndDestroy(implArray);
		return buf;
		}
	User::Leave(KErrNotFound);
	return NULL; // to keep compiler happy
	}
	
CArrayFixFlat<TDataTypeWithPriority>* CApaAppList::DataTypeArrayDeepCopyLC(const CArrayFixFlat<TDataTypeWithPriority>& aOriginal) const
	{
	CArrayFixFlat<TDataTypeWithPriority>* result = new(ELeave) CArrayFixFlat<TDataTypeWithPriority>(1);
	CleanupStack::PushL(result);
	TInt ii = 0;
	TInt end = aOriginal.Count();
	while (ii < end)
		{
		result->AppendL(aOriginal[ii]);
		ii++;
		}
	return result;
	}
	
EXPORT_C TInt CApaAppList::CompareStrings(const HBufC& aFirst, const HBufC& aSecond)
	{
	return aFirst.CompareF(aSecond);
	}

EXPORT_C CApaAppList* CApaAppList::Self()
	{ // static
	return static_cast<CApaAppList*>(Dll::Tls());
	}
	
/*EXPORT_C*/ RFs& CApaAppList::ShareProtectedFileServer()
	{
	return iFsShareProtected; //lint !e1536 Exposing low access member - need to consider a redesign or design clarification here(iFsShareProtected should be owned be the one that needs it) but postpone for now as that may require large changes
	}
	
CApaAppIconArray* CApaAppList::LoadDefaultIconsL() const
	{
	__ASSERT_ALWAYS(iDefaultAppIcon, Panic(EPanicNullPointer));
	CApaIconLoader* iconLoader = CApaIconLoader::NewLC(iFs);
	CApaAppIconArray* icons = CApaAppIconArray::NewRealDefaultIconsLC(KNumberOfIconsInDefaultMbm,*iDefaultAppIcon, *iconLoader);
	const TBool badMbmEntryInfo = !icons->LoadIconsL();
	if(badMbmEntryInfo)
		{
		CleanupStack::PopAndDestroy(2,iconLoader);
		return CApaAppIconArray::NewL();
		}
	else
		{
		CleanupStack::Pop(icons);
		CleanupStack::PopAndDestroy(iconLoader);
		}
	return icons;
	}

void CApaAppList::AcquireDefaultIconArrayL() const
	{
	ASSERT(iDefaultIconUsageCount >= 0);
	if (iDefaultIconUsageCount == 0)
		{
		iDefaultIconArray = LoadDefaultIconsL();
		}
	++iDefaultIconUsageCount;
	}

// Should NEVER be called by an object that does not call AcquireDefaultIconArrayL and
// ReleaseDefaultIconArray at the beginning and end of its lifetime respectively
const CApaAppIconArray& CApaAppList::DefaultIconArray() const
	{
	__ASSERT_ALWAYS(iDefaultIconArray, Panic(EPanicNullPointer));
	return *iDefaultIconArray;
	}

void CApaAppList::ReleaseDefaultIconArray() const
	{
	ASSERT(iDefaultIconUsageCount > 0);
	if(0 == --iDefaultIconUsageCount)
		{
		delete iDefaultIconArray;
		iDefaultIconArray = NULL;		
		}
	}
	
// DEF077478 - Required to update the default icons that the app list holds.
void CApaAppList::UpdateDefaultIconsL()
	{
	CApaAppIconArray* iconArray = LoadDefaultIconsL();
	if (iconArray)
		{
		delete iDefaultIconArray;
		iDefaultIconArray = iconArray;
		}
	}


// ApaUtils

EXPORT_C TBool ApaUtils::HandleAsRegistrationFile(const TUidType& aUidType)
	{ // static
	return (aUidType[1].iUid==KUidAppRegistrationFile.iUid ||
		   aUidType[0].iUid==KUidPrefixedNonNativeRegistrationResourceFile);
	}
	
// Returns ETrue if a language change event is received and a re-scan is in progress otherwise EFalse. 
EXPORT_C TBool CApaAppList::IsLanguageChangePending() const
	{
	return (iFlags & ELangChangePending);
	}
//
// Class CApaLangChangeMonitor
//

CApaAppList::CApaLangChangeMonitor::~CApaLangChangeMonitor()
	{	
	Cancel();
	iLangNotifier.Close();
	}

CApaAppList::CApaLangChangeMonitor* CApaAppList::CApaLangChangeMonitor::NewL(CApaAppList& aAppList)
	{ // static	
	CApaLangChangeMonitor* self=new(ELeave) CApaLangChangeMonitor(aAppList);
	self->ConstructL();
	return self;
	}

CApaAppList::CApaLangChangeMonitor::CApaLangChangeMonitor(CApaAppList& aAppList)
	: CActive(EPriorityNormal),
	iAppList(aAppList)
	{	
	iPrevLanguage = User::Language();
	CActiveScheduler::Add(this);
	}

void CApaAppList::CApaLangChangeMonitor::ConstructL()
	{
	User::LeaveIfError(iLangNotifier.Create());
	Start();
	}
 
void CApaAppList::CApaLangChangeMonitor::Start()
	{
	iLangNotifier.Logon(iStatus);
	SetActive();
 	}
 
void CApaAppList::CApaLangChangeMonitor::DoCancel()
	{
 	iLangNotifier.LogonCancel();
	}
 
void CApaAppList::CApaLangChangeMonitor::RunL()
	{
	// Logon to get further events before handling current request.
	TRequestStatus status = iStatus;
	Start();
	
	// if it is a language change event, start a rescan on app-list.
 	if (status.Int() == EChangesLocale && iPrevLanguage != User::Language())
		{		
		iPrevLanguage = User::Language();
		iAppList.iFlags |= CApaAppList::ELangChangePending;
		iAppList.StartIdleUpdateL(iAppList.iObserver);
		}
	}

TInt CApaAppList::CApaLangChangeMonitor::RunError(TInt /*aError*/)
	{
	// Reset ELangChangePending flag if RunL leaves.
	iAppList.iFlags &= ~CApaAppList::ELangChangePending;
	// Reset iPrevLanguage to ELangNone if RunL leaves.
	iPrevLanguage = ELangNone;
	return KErrNone;
	}
