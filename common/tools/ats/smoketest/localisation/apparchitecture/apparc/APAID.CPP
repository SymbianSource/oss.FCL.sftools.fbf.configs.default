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

#include <apaid.h>
#include <s32strm.h>
#include "APASTD.H"


/////////////////////////////
// TApaAppIdentifier
/////////////////////////////

EXPORT_C TApaAppIdentifier::TApaAppIdentifier()
/** Constructs an empty application identifier.

The data is not initialised. */
	{}

EXPORT_C TApaAppIdentifier::TApaAppIdentifier(TUid aAppUid,const TFileName& aDllName)
	: iAppUid(aAppUid),
	iFullName(aDllName)
/** Constructs an application identifier from the specified application 
DLL file name and extension, and the specified application UID.
@param aAppUid The application specific UID.
@param aDllName The filename and extension of the application DLL. */
	{}

EXPORT_C void TApaAppIdentifier::ExternalizeL(RWriteStream& aStream)const
/** Externalises the application identifier to a write stream.

@param aStream The write stream. */
	{
	aStream<< iAppUid;
	aStream<< iFullName;
	}

EXPORT_C void TApaAppIdentifier::InternalizeL(RReadStream& aStream)
/** Internalises the application identifier from a read stream.

@param aStream The read stream. */
	{
	aStream>> iAppUid;
	aStream>> iFullName;
	}

/////////////////////////////
// TApaAppEntry
/////////////////////////////

EXPORT_C TApaAppEntry::TApaAppEntry()
	: iUidType(TUidType()),
	iFullName(KNullDesC)
/** Constructs an empty application entry object.

The full path name is empty, and the triplet of UIDs forming the UID type 
are set to null UIDs. */
	{}


EXPORT_C TApaAppEntry::TApaAppEntry(const TUidType& aAppUidType,const TFileName& aDllName)
	: iUidType(aAppUidType),
	iFullName(aDllName)
/** Constructs an application entry object from the specified application 
DLL full path name and UID type. 
@param aAppUidType UID type.
@param aDllName Application DLL full path name. */
	{}


EXPORT_C void TApaAppEntry::ExternalizeL(RWriteStream& aStream)const
/** Externalises the application entry to a write stream.

@param aStream The write stream. */
	{
	aStream<< iUidType[0];
	aStream<< iUidType[1];
	aStream<< iUidType[2];
	aStream<< iFullName;
	}


EXPORT_C void TApaAppEntry::InternalizeL(RReadStream& aStream)
/** Internalises the application entry from a read stream.

@param aStream The read stream. */
	{
	TUid uid1;
	TUid uid2;
	TUid uid3;
	aStream>> uid1;
	aStream>> uid2;
	aStream>> uid3;
	iUidType = TUidType(uid1,uid2,uid3);
	aStream>> iFullName;
	}


/////////////////////////////
// TApaAppInfo
/////////////////////////////

EXPORT_C TApaAppInfo::TApaAppInfo()
	: iUid(TUid()),
	iFullName(KNullDesC),
	iCaption(KNullDesC),
	iShortCaption(KNullDesC)
/** Constructs an empty application information object.

The full path name is empty, the captions are empty and the application specific 
UID is set to the null UID. */
	{}


EXPORT_C TApaAppInfo::TApaAppInfo(TUid aAppUid,const TFileName& aDllName,const TApaAppCaption& aCaption)
	: iUid(aAppUid),
	iFullName(aDllName),
	iCaption(aCaption),
	iShortCaption(aCaption)
/** Constructs an application information object from the specified full DLL path 
name, UID and full length caption.

@param aAppUid The application specific UID. 
@param aDllName The full path name of the application DLL. 
@param aCaption The application caption. */
	{}

EXPORT_C TApaAppInfo::TApaAppInfo(TUid aAppUid,const TFileName& aDllName,const TApaAppCaption& aCaption,const TApaAppCaption& aShortCaption)
	: iUid(aAppUid),
	iFullName(aDllName),
	iCaption(aCaption),
	iShortCaption(aShortCaption)
/** Constructs an application information object from the specified full DLL path 
name, UID, caption and short caption.

@param aAppUid The application specific UID. 
@param aDllName The full path name of the application DLL. 
@param aCaption The application caption. 
@param aShortCaption The application short caption. */
	{}

EXPORT_C void TApaAppInfo::ExternalizeL(RWriteStream& aStream)const
/** Externalises the application information to a write stream.

@param aStream The write stream. */
	{
	aStream<< iUid;
	aStream<< iFullName;
	aStream<< iCaption;
	aStream<< iShortCaption;
	}

EXPORT_C void TApaAppInfo::InternalizeL(RReadStream& aStream)
/** Internalises the application information from a read stream.

@param aStream The read stream. */
	{
	aStream>> iUid;
	aStream>> iFullName;
	aStream>> iCaption;
	aStream>> iShortCaption;
	}


/////////////////////////////
// TApaAppViewInfo
/////////////////////////////

EXPORT_C TApaAppViewInfo::TApaAppViewInfo()
	: iUid(KNullUid),
	iViewCaption(KNullDesC)
/** Constructs an empty object.

Specifically, it sets the view UID to KNullUid and empties the application 
caption, i.e. sets it to KNullDesC. */
	{}


EXPORT_C TApaAppViewInfo::TApaAppViewInfo(TUid aAppUid,const TApaAppCaption& aViewCaption, TInt aScreenMode)
	: iUid(aAppUid),
	iViewCaption(aViewCaption),
	iScreenMode(aScreenMode)
	{}


EXPORT_C void TApaAppViewInfo::ExternalizeL(RWriteStream& aStream)const
/** Externalises the application view information to a write stream.

@param aStream The write stream. */
	{
	aStream<< iUid;
	aStream<< iViewCaption;
	aStream.WriteInt32L(iScreenMode);
	}


EXPORT_C void TApaAppViewInfo::InternalizeL(RReadStream& aStream)
/** Internalises the application view information from a read stream.

@param aStream The read stream. */
	{
	aStream>> iUid;
	aStream>> iViewCaption;
	iScreenMode=aStream.ReadInt32L();
	}


///////////////////////////////////////
// class TApaAppCapability
///////////////////////////////////////

EXPORT_C void TApaAppCapability::InternalizeL(RReadStream& aStream)
	{
	DoInternalizeL(aStream, iLaunchInBackground, iGroupName);
	}
	
EXPORT_C void TApaAppCapability::Internalize7_0L(RReadStream& aStream)
/**
@deprecated
*/
	{
	TBool dummyBool;
	TApaAppGroupName dummyGroupName;
	DoInternalizeL(aStream,dummyBool,dummyGroupName);
	}

void TApaAppCapability::DoInternalizeL(RReadStream& aStream, TBool& aLaunchInBackground, TApaAppGroupName& aGroupName)
/** Internalises the application capabilities from a read stream.

@param aStream The read stream. */
	{
	TInt version = aStream.ReadInt32L();
	iEmbeddability = TEmbeddability(aStream.ReadInt32L());
	iSupportsNewFile = aStream.ReadInt32L();
	iAppIsHidden = aStream.ReadInt32L();

	// initialise values of members which may not be in the stream
	aLaunchInBackground = EFalse;
	aGroupName.Zero();

	if (version==1)
		return;

	// Calypso extension to allow apps to be launched in the background
	aLaunchInBackground = aStream.ReadInt32L();
	if (version==2)
		return;

	aStream >> aGroupName;
	if (version == 3)
		return;

	iAttributes = aStream.ReadUint32L();
	if (version == 4)
		return;
	
	Panic(EDPanicInvalidVersionNumber);
	}

EXPORT_C void TApaAppCapability::ExternalizeL(RWriteStream& aStream) const
/** Externalises the application capabilities to a write stream.

@param aStream The write stream. */
	{
	aStream.WriteInt32L(EVersion);
	aStream.WriteInt32L(iEmbeddability);
	aStream.WriteInt32L(iSupportsNewFile);
	aStream.WriteInt32L(iAppIsHidden);
	aStream.WriteInt32L(iLaunchInBackground);
	aStream << iGroupName;
	aStream.WriteUint32L(iAttributes);
	}
	
const TInt KOldVersion=1;
	
EXPORT_C void TApaAppCapability::Externalize7_0L(RWriteStream& aStream) const
/** Externalises the application capabilities to a write stream.

@param aStream The write stream. 
@deprecated*/
	{
	aStream.WriteInt32L(KOldVersion);
	aStream.WriteInt32L(iEmbeddability);
	aStream.WriteInt32L(iSupportsNewFile);
	aStream.WriteInt32L(iAppIsHidden);
	}
	
EXPORT_C void TApaAppCapability::CopyCapability(TDes8& aDest,const TDesC8& aSource)
/** A utility function that can copy capability information from one descriptor 
to another.

@param aDest Target descriptor.
@param aSource Source descriptor. */
	{
	TInt maxLen=aDest.MaxLength();
	aDest.FillZ(maxLen); // zero fill in case aSource is shorter
	aDest.Copy(aSource.Left(Min(aSource.Length(),maxLen)));
	aDest.SetLength(maxLen);
	}


///////////////////////////////////////
// class TApaEmbeddabilityFilter
///////////////////////////////////////
EXPORT_C TApaEmbeddabilityFilter::TApaEmbeddabilityFilter()
	: iEmbeddabilityFlags(0)
/** Constructs an empty embeddability filter. */
	{
	}

EXPORT_C void TApaEmbeddabilityFilter::AddEmbeddability(TApaAppCapability::TEmbeddability aEmbeddability)
/** Adds aEmbeddability to the filter.

@param aEmbeddability TEmbeddability value to add to the filter. */
	{
	__ASSERT_ALWAYS(aEmbeddability >= 0 && static_cast<TUint>(aEmbeddability) < (sizeof(TUint)*8), Panic(EPanicEmbeddabilityOutOfRange));
	iEmbeddabilityFlags |= (1 << aEmbeddability);
	}

EXPORT_C TBool TApaEmbeddabilityFilter::MatchesEmbeddability(TApaAppCapability::TEmbeddability aEmbeddability) const
/** Compares aEmbeddability with the filter.

@param aEmbeddability TEmbeddability value to compare.
@return True, if aEmbeddability is included in the filter; false, otherwise. */
	{
	__ASSERT_ALWAYS(aEmbeddability >= 0 && static_cast<TUint>(aEmbeddability) < (sizeof(TUint)*8), Panic(EPanicEmbeddabilityOutOfRange));
	TUint embeddabilityFlag = (1 << aEmbeddability);
	if (embeddabilityFlag & iEmbeddabilityFlags)
		{
		return ETrue;
		}
	return EFalse;
	}

//
// CApaAppFinder
//

/** Constructor for CApaAppFinder */
EXPORT_C CApaAppFinder::CApaAppFinder()
	{
	}

/* Reserved for future use */
EXPORT_C void CApaAppFinder::CApaAppFinder_Reserved1()
	{
	}

/* Reserved for future use */
EXPORT_C void CApaAppFinder::CApaAppFinder_Reserved2()
	{
	}
	
//
// CApaAppServiceInfoArray
//

EXPORT_C CApaAppServiceInfoArray::CApaAppServiceInfoArray()
	{
	}
	
EXPORT_C void CApaAppServiceInfoArray::CApaAppServiceInfoArray_Reserved1()
	{
	}

EXPORT_C void CApaAppServiceInfoArray::CApaAppServiceInfoArray_Reserved2()
	{
	}
