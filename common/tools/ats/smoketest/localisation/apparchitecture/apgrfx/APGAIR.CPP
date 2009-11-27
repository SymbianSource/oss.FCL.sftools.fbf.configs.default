// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "APGAIR.H"
#include "APGAPLST.H"
#include "APGICNFL.H" 
#include "APFDEF.H"

extern void CleanupServiceArray(TAny* aServiceArray);

CApaAppInfoReader::CApaAppInfoReader(RFs& aFs, const TUid aAppUid)
	: iFs(aFs),
	iAppUid(aAppUid),
	iTimeStamp(0),
	iDefaultScreenNumber(0),
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
	iDefaultIconsUsed(EFalse),
#endif	//SYMBIAN_APPARC_APPINFO_CACHE
	iNonMbmIconFile(EFalse),
	iLocalisableResourceFileTimeStamp(0),
	iApplicationLanguage(ELangNone),
	iIndexOfFirstOpenService(-1)
	{
	}

CApaAppInfoReader::~CApaAppInfoReader()
	{
	delete iAppBinaryFullName;
	delete iCaption;
	delete iShortCaption;
	delete iIcons;
	if (iViewDataArray)
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
	}

// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
HBufC* CApaAppInfoReader::AppBinaryFullName()
	{
	HBufC* fileName = iAppBinaryFullName;
	iAppBinaryFullName = NULL; // ownership transferred to caller
	return fileName;
	}

TUidType CApaAppInfoReader::AppBinaryUidType() const
	{
	return iAppBinaryUidType;
	}

TTime CApaAppInfoReader::TimeStamp() const
	{
	return iTimeStamp;
	}

TTime CApaAppInfoReader::IconFileTimeStamp() const
     {
     return iIconFileTimeStamp;
     }

void CApaAppInfoReader::Capability(TDes8& aCapabilityBuf) const
	{
	TApaAppCapabilityBuf buf(iCapability);
	TApaAppCapability::CopyCapability(aCapabilityBuf, buf);
	}

TUint CApaAppInfoReader::DefaultScreenNumber() const
	{
	return iDefaultScreenNumber;
	}

// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
HBufC* CApaAppInfoReader::Caption()
	{
	HBufC* caption = iCaption;
	iCaption = NULL; // ownership transferred to caller
	return caption;
	}

// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
HBufC* CApaAppInfoReader::ShortCaption()
	{
	HBufC* shortCaption = iShortCaption;
	iShortCaption = NULL; // ownership transferred to caller
	return shortCaption;
	}

// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
CApaAppIconArray* CApaAppInfoReader::Icons()
	{
	CApaAppIconArray* icons = iIcons;
	iIcons = NULL; // ownership transferred to caller
	return icons;
	}

#ifndef SYMBIAN_APPARC_APPINFO_CACHE
TBool CApaAppInfoReader::DefaultIconsUsed() const
	{
	return iDefaultIconsUsed;
	}
#else
TInt CApaAppInfoReader::NumOfAppIcons() const
	{
	return iNumOfAppIcons;
	}
#endif // SYMBIAN_APPARC_APPINFO_CACHE

// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
CArrayPtrFlat<CApaAppViewData>* CApaAppInfoReader::Views()
	{
	CArrayPtrFlat<CApaAppViewData>* viewDataArray = iViewDataArray;
	iViewDataArray = NULL; // ownership transferred to caller
	return viewDataArray;
	}
	
// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
CDesCArray* CApaAppInfoReader::OwnedFiles()
	{
	CDesCArray* ownedFileArray = iOwnedFileArray;
	iOwnedFileArray = NULL; // ownership transferred to caller
	return ownedFileArray;
	}

// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
HBufC* CApaAppInfoReader::IconFileName()
	{
	HBufC* iconFileName = iIconFileName;
	iIconFileName = NULL; // ownership transferred to caller
	return iconFileName;
	}

TBool CApaAppInfoReader::NonMbmIconFile() const
	{
	return iNonMbmIconFile;
	}

// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
HBufC* CApaAppInfoReader::LocalisableResourceFileName()
	{
	HBufC* localisableResourceFileName = iLocalisableResourceFileName;
	iLocalisableResourceFileName = NULL; // ownership transferred to caller
	return localisableResourceFileName;
	}

TTime CApaAppInfoReader::LocalisableResourceFileTimeStamp() const
	{
	return iLocalisableResourceFileTimeStamp;
	}
	
TLanguage CApaAppInfoReader::AppLanguage() const
	{
	return iApplicationLanguage;
	}

// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
CArrayFixFlat<TApaAppServiceInfo>* CApaAppInfoReader::ServiceArray(TInt& aIndexOfFirstOpenService)
	{
	CArrayFixFlat<TApaAppServiceInfo>* serviceArray = iServiceArray;
	iServiceArray = NULL;
	aIndexOfFirstOpenService = iIndexOfFirstOpenService;
	return serviceArray;
	}


// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReader derived object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
HBufC8* CApaAppInfoReader::OpaqueData()
	{
	HBufC8* opaqueData = iOpaqueData;
	iOpaqueData = NULL;
	return opaqueData;
	}
