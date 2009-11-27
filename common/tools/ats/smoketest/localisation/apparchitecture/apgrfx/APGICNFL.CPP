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

#include <e32std.h>
#include <s32file.h>
#include <s32ucmp.h>
#include <bautils.h>
#include <barsread.h>
#include <barsc.h>
#include <barsread2.h>
#include <barsc2.h>
#include <apfdef.h>
#include "APGAIR.H"

#ifdef _DEBUG
#include "APGSTD.H" // panic codes
#endif // _DEBUG

#if defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER)
// the two #defines immediately below are #defined to "nothing" so that only the minimal set of functions that ought to be "removed" because of SYMBIAN_REMOVE_UI_FRAMEWORKS_V1 (but which are needed still because of UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER) are actually exported
#define IMPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER
#define EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER
#endif // UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER

#include "APGICNFL.H"

#if !defined(EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER)
// we're compiling a source file that doesn't define EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER, so give it it's default "value" of "EXPORT_C"
#define EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER EXPORT_C
#endif

//
// Try and reduce the bitmap mask depth to 1bpp (2 colours)
//
LOCAL_D CFbsBitmap* TryCompressMaskL(const CFbsBitmap& aMask)
	{
	CFbsBitmap* newMask=NULL;
	if (aMask.DisplayMode()!=EGray2 && aMask.IsMonochrome())
		{
		newMask=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(newMask);
		const TSize size=aMask.SizeInPixels();
		User::LeaveIfError(newMask->Create(size,EGray2));
		CFbsBitmapDevice* device=CFbsBitmapDevice::NewL(newMask);
		CleanupStack::PushL(device);
		CFbsBitGc* gc=NULL;
		User::LeaveIfError(device->CreateContext(gc));
		CleanupStack::PushL(gc);
		TPoint origin(0,0);
		gc->BitBlt(origin,&aMask);
		CleanupStack::PopAndDestroy(2); // gc, device
		CleanupStack::Pop(newMask);
		}
	return newMask;
	}

//
// Class CApaMaskedBitmap
//

CApaMaskedBitmap::CApaMaskedBitmap()
:CFbsBitmap()
	{}

EXPORT_C CApaMaskedBitmap::~CApaMaskedBitmap()
/** Destructor.

Frees resources owned by the object prior to its destruction. */
	{
	delete iMask;
	}

EXPORT_C CApaMaskedBitmap* CApaMaskedBitmap::NewLC()
/** Creates a default application icon object.

@return A pointer to the new application icon object. */
	{
	CApaMaskedBitmap* self=new(ELeave) CApaMaskedBitmap;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CApaMaskedBitmap* CApaMaskedBitmap::NewL(const CApaMaskedBitmap* aSourceIcon)
/** Creates a new application icon object, making a duplicate copy of an existing 
application icon.

@param aSourceIcon A pointer to an existing application icon.
@return A pointer to the new application icon object. */
	{
	CApaMaskedBitmap* self=new(ELeave) CApaMaskedBitmap;
	CleanupStack::PushL(self);
	self->ConstructL();
	User::LeaveIfError( self->iMask->Duplicate(aSourceIcon->Mask()->Handle()) );
	User::LeaveIfError( self->Duplicate(aSourceIcon->Handle()) );
	CleanupStack::Pop(self);
	return self;
	}

void CApaMaskedBitmap::ConstructL()
	{
	__DECLARE_NAME(_S("CApaMaskedBitmap"));
	if (!iFbs)
		User::Leave(KErrCouldNotConnect);
	iMask=new(ELeave) CFbsBitmap;
	}

EXPORT_C CFbsBitmap* CApaMaskedBitmap::Mask() const
/** Gets the icon's mask.

@return A pointer to the mask bitmap. */
	{
	return iMask;
	}

EXPORT_C void CApaMaskedBitmap::InternalizeL(RReadStream& aStream)
/** Internalizes the application icon from the read stream.

@param aStream The read stream. */
	{
	__ASSERT_DEBUG(iMask, Panic(EPanicNullPointer));
	CFbsBitmap::InternalizeL(aStream);
	aStream >> *iMask;

	// Try to reduce the colour depth of the bitmap mask
	CFbsBitmap* tempMask;
	tempMask = TryCompressMaskL(*iMask);
	// tempMask = NULL if iMask could not be reduced
	if (tempMask != NULL)
		{
		delete iMask;
		iMask = tempMask;
		}
	}

void CApaMaskedBitmap::SetRomBitmapL(TUint8* aRomPointer)
	{
	__ASSERT_DEBUG(iMask, Panic(EPanicNullPointer));
	TInt bitmapSize = 0;
	CFbsBitmap::SetRomBitmapL(reinterpret_cast<CBitwiseBitmap*>(aRomPointer),bitmapSize);

	aRomPointer += bitmapSize;

	iMask->SetRomBitmapL(reinterpret_cast<CBitwiseBitmap*>(aRomPointer),bitmapSize);
	}

EXPORT_C void CApaMaskedBitmap::ExternalizeL(RWriteStream& aStream) const
/** Externalises the application icon to the specified stream.

@param aStream The write stream. */
	{
	__ASSERT_DEBUG(iMask, Panic(EPanicNullPointer));
	CFbsBitmap::ExternalizeL(aStream);
	aStream << *iMask;
	}


EXPORT_C void CApaMaskedBitmap::SetMaskBitmap(CFbsBitmap* aMask)
/**
Sets the icon's mask

@publishedAll
@released
@param     aMask A pointer to the mask bitmap       
*/
	{
	delete iMask;
	iMask = aMask;
	}



#if (defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER))



//
// Class CApaAIFCaption
//

CApaAIFCaption::~CApaAIFCaption()
	{
	delete iCaption;
	}

CApaAIFCaption* CApaAIFCaption::NewLC(TLanguage aLanguage,const TDesC& aCaption)
	// static
	{
	CApaAIFCaption* self=new(ELeave) CApaAIFCaption;
	CleanupStack::PushL(self);
	self->ConstructL(aLanguage,aCaption);
	return self;
	}

void CApaAIFCaption::ConstructL(TLanguage aLanguage,const TDesC& aCaption)
	{
	iLanguage=aLanguage;
    HBufC* newCaption=aCaption.AllocL();
	delete(iCaption); // after the AllocL succeeds
	iCaption=newCaption;
	}

TLanguage CApaAIFCaption::Language() const
	{
	return iLanguage;
	}

TApaAppCaption CApaAIFCaption::Caption() const
	{
	return *iCaption;
	}

CApaAIFCaption::CApaAIFCaption()
	{
	}

void CApaAIFCaption::InternalizeL(RReadStream& aStream)
	{
	HBufC* newCaption=HBufC::NewL(aStream,KApaMaxAppCaption);
	delete(iCaption);
	iCaption=newCaption;
	iLanguage=(TLanguage)aStream.ReadInt16L();
	}

void CApaAIFCaption::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iCaption;
	aStream.WriteInt16L(iLanguage);
	}


//
// Class CApaAIFViewData
//

CApaAIFViewData::~CApaAIFViewData()
	{
	if (iCaptionArray)
		iCaptionArray->ResetAndDestroy();
	delete iCaptionArray;
	if (iIconArray)
		iIconArray->ResetAndDestroy();
	delete iIconArray;
	delete iIconIndexArray;
	}

CApaAIFViewData::CApaAIFViewData()
	{}

void CApaAIFViewData::ConstructL()
	{
	iCaptionArray=new(ELeave) CArrayPtrFlat<CApaAIFCaption>(4);
	iIconArray=new(ELeave) CArrayPtrFlat<CApaMaskedBitmap>(4);
	}

CApaAIFViewData* CApaAIFViewData::NewLC()
	// static
	{
	CApaAIFViewData* self=new(ELeave) CApaAIFViewData;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CApaAIFViewData* CApaAIFViewData::NewLC(const CApaAIFViewData& aSourceData)
	{
	CApaAIFViewData* self = CApaAIFViewData::NewLC();
	self->ConstructL(aSourceData);
	return self;
	}

void CApaAIFViewData::ConstructL(const CApaAIFViewData& aSourceData)
	{
	// Make a copy of aSourceData in this object
	// N.B. this is not an atomic operation and if the function leaves
	// there is no guarantee that this object will be in a valid state.
	// Should only be called from CApaAIFViewData::NewLC(const CApaAIFViewData& aSourceData)
	//
	// Screen mode
	iScreenMode = aSourceData.iScreenMode;
	// Icon array
	TInt count = (aSourceData.iIconArray ? aSourceData.iIconArray->Count() : 0);
	TInt i;
	for (i=0; i < count; i++)
		{
		// Get the icon from array and duplicate it
		const CApaMaskedBitmap& srcIcon = (*aSourceData.iIconArray->At(i));
		CApaMaskedBitmap* icon = CApaMaskedBitmap::NewLC();
		User::LeaveIfError(icon->Duplicate(srcIcon.Handle()));

		CFbsBitmap* mask = new(ELeave) CFbsBitmap();
		CleanupStack::PushL(mask);
		User::LeaveIfError(mask->Duplicate(srcIcon.Mask()->Handle()));
		icon->SetMaskBitmap(mask);
		CleanupStack::Pop(mask);

		iIconArray->AppendL(icon);
		CleanupStack::Pop(icon);
		}
	// Caption array
	count = (aSourceData.iCaptionArray ? aSourceData.iCaptionArray->Count() : 0);
	for (i=0; i < count; i++)
		{
		const CApaAIFCaption& srcCaption = (*aSourceData.iCaptionArray->At(i));
		CApaAIFCaption* caption = CApaAIFCaption::NewLC(srcCaption.Language(), srcCaption.Caption());
		iCaptionArray->AppendL(caption);
		CleanupStack::Pop(caption);
		}
	// View UID
	iViewUid = aSourceData.iViewUid;
	// Icon index array
	count = (aSourceData.iIconIndexArray ? aSourceData.iIconIndexArray->Count() : 0);
	for (i=0; i < count; i++)
		{
		AddIconIndexL(aSourceData.iIconIndexArray->At(i));
		}
	}

void CApaAIFViewData::LoadIconsL(const TDesC& aFileName, TUint aMbmOffset)
	{
	// This function is called when loading the AIF version 2 format
	// When loading the RSC block, any view icons will have been added to iIconIndexArray as MBM indexes.
	// This function loads the view icons from the MBM using these indexes.
	__ASSERT_DEBUG(iIconArray->Count()==0,Panic(EDPanicArrayNotEmpty));
	const TInt iconCount = (iIconIndexArray ? iIconIndexArray->Count() : 0);
	for (TInt i=0; i < iconCount; i++)
		{
		const TInt mbmIndex = iIconIndexArray->At(i) * 2;	// Each icon is a mask and bitmap == 2 items in MBM
		CApaMaskedBitmap* icon = CApaMaskedBitmap::NewLC();
		CFbsBitmap* mask = new(ELeave) CFbsBitmap();
		CleanupStack::PushL(mask);
		User::LeaveIfError(icon->Load(aFileName, mbmIndex, ETrue, aMbmOffset));
		User::LeaveIfError(mask->Load(aFileName, mbmIndex + 1, ETrue, aMbmOffset));
		icon->SetMaskBitmap(mask);
		CleanupStack::Pop(mask);
		iIconArray->AppendL(icon);
		CleanupStack::Pop(icon);
		}
	}

void CApaAIFViewData::InternalizeL(RReadStream& aStream)
	{
	// Uid
	iViewUid.iUid=aStream.ReadInt32L();
	// Screen mode
	iScreenMode=aStream.ReadInt32L();
	// Captions
	if (iCaptionArray)
		iCaptionArray->ResetAndDestroy();
	else
		iCaptionArray=new(ELeave) CArrayPtrFlat<CApaAIFCaption>(1);
	TCardinality card;
	aStream>>card;
	const TInt captionCount(card);
	CApaAIFCaption* caption=NULL;
	for (TInt ii=0;ii<captionCount;ii++)
		{
		caption=new(ELeave) CApaAIFCaption();
		CleanupStack::PushL(caption);
		aStream >> *caption;
		iCaptionArray->AppendL(caption);
		CleanupStack::Pop(caption);
		}
	// Icons
	if (iIconArray)
		iIconArray->ResetAndDestroy();
	else
		iIconArray=new(ELeave) CArrayPtrFlat<CApaMaskedBitmap>(2);
	aStream>>card;
	const TInt iconCount(card);
	CApaMaskedBitmap* icon=NULL;
	for (TInt jj=0;jj<iconCount;jj++)
		{
		icon=CApaMaskedBitmap::NewLC();
		aStream >> *icon;
		iIconArray->AppendL(icon);
		CleanupStack::Pop(icon);
		}
	}

void CApaAIFViewData::ExternalizeL(RWriteStream& aStream) const
	{
	// Uid
	aStream.WriteInt32L(iViewUid.iUid);
	// Screen Mode
	aStream.WriteInt32L(iScreenMode);
	// Captions
	const TInt captionCount=iCaptionArray->Count();
	aStream<<TCardinality(captionCount);
	for (TInt ii=0;ii<captionCount;ii++)
		{
		aStream << *((*iCaptionArray)[ii]);
		}
	// Icons
	const TInt iconCount=iIconArray->Count();
	aStream<<TCardinality(iconCount);
	for (TInt jj=0;jj<iconCount;jj++)
		{
		aStream << *((*iIconArray)[jj]);
		}
	}

void CApaAIFViewData::SetScreenMode(TInt aScreenMode)
	{
	iScreenMode=aScreenMode;
	}

void CApaAIFViewData::AddCaptionL(TLanguage aLanguage,const TDesC& aCaption)
	{
	CApaAIFCaption* caption=CApaAIFCaption::NewLC(aLanguage,aCaption);
	iCaptionArray->AppendL(caption);
	CleanupStack::Pop(caption);
	}

void CApaAIFViewData::AddIconL(CApaMaskedBitmap& aIcon)
	{
	CApaMaskedBitmap* icon=CApaMaskedBitmap::NewL(&aIcon);
	CleanupStack::PushL(icon);
	iIconArray->AppendL(icon);
	CleanupStack::Pop(icon);
	}

void CApaAIFViewData::AddIconIndexL(TInt aIndex)
	{
	if (!iIconIndexArray)
		{
		iIconIndexArray = new(ELeave) CArrayFixFlat<TInt>(4);
		}
	iIconIndexArray->AppendL(aIndex);
	}

void CApaAIFViewData::SetViewUid(TUid aUid)
	{
	iViewUid=aUid;
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER TApaAppCaption CApaAIFViewData::CaptionL(TLanguage aLanguage) const
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	RArray<TLanguage> downgradePath;
	CleanupClosePushL(downgradePath);
	BaflUtils::GetDowngradePathL(fs,aLanguage,downgradePath);
	TApaAppCaption result(KNullDesC);
	const TInt languagecount=downgradePath.Count();
	const TInt count=iCaptionArray->Count();
	for (TInt j=0; j<languagecount; j++)
		{
		for (TInt ii=0;ii<count;ii++)
			{
			const CApaAIFCaption& caption=*(*iCaptionArray)[ii];
			if (caption.Language()==downgradePath[j])
				{
				result=caption.Caption();
				j=languagecount; // to exit outer loop
				break;
				}
			}
		}
	CleanupStack::PopAndDestroy(2, &fs);
	return result;
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER CApaMaskedBitmap* CApaAIFViewData::IconByIndexL(TInt aIndex) const
	{
	return (*iIconArray)[aIndex];
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER TInt CApaAIFViewData::NumberOfIcons() const
	{
	return (iIconIndexArray ? iIconIndexArray->Count() : iIconArray->Count());
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER TUid CApaAIFViewData::ViewUid() const
	{
	return iViewUid;
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER TInt CApaAIFViewData::ScreenMode() const
	{
	return iScreenMode;
	}

//
// Class CApaAppInfoFile
//

CApaAppInfoFile::CApaAppInfoFile(RFs& aFs)
:iFs(aFs)
	{}

void CApaAppInfoFile::TCaptionHeader::InternalizeL(RReadStream& aStream)
	{
	aStream >> iCaption;
	iLanguage=(TLanguage)aStream.ReadInt16L();
	}

void CApaAppInfoFile::TCaptionHeader::ExternalizeL(RWriteStream& aStream) const
	{
	aStream	<< iCaption;
	aStream.WriteInt16L(iLanguage);
	}

void CApaAppInfoFile::TIconHeader::InternalizeL(RReadStream& aStream)
	{
	aStream >> iIcon;
	iIconSideInPixels=aStream.ReadInt16L();
	}

void CApaAppInfoFile::TIconHeader::ExternalizeL(RWriteStream& aStream) const
	{
	aStream	<< iIcon;
	aStream.WriteInt16L(iIconSideInPixels);
	}

void CApaAppInfoFile::TDataTypeHeader::InternalizeL(RReadStream& aStream)
	{
	aStream >> iDataType;
	iPriority=aStream.ReadInt16L();
	}

void CApaAppInfoFile::TDataTypeHeader::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iDataType;
	aStream.WriteInt16L(iPriority);
	}

void CApaAppInfoFile::TViewDataHeader::InternalizeL(RReadStream& aStream)
	{
	aStream >> iViewData;
	}

void CApaAppInfoFile::TViewDataHeader::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iViewData;
	}

void CApaAppInfoFile::TFileOwnershipInfoHeader::InternalizeL(RReadStream& aStream)
	{
	aStream >> iOwnedFileName;
	}

void CApaAppInfoFile::TFileOwnershipInfoHeader::ExternalizeL(RWriteStream& aStream) const
	{
	aStream	<< iOwnedFileName;
	}

CApaAppInfoFile::~CApaAppInfoFile()
	{
	if (iCaptionHeaderArray)
		{
		const TInt maxIndex=iCaptionHeaderArray->Count() - 1;
		for(TInt i=maxIndex; i>=0; i--)
			{
			const TSwizzle<HBufC>& caption = iCaptionHeaderArray->At(i).iCaption; 
			if (caption.IsPtr())
				{
				delete caption.AsPtr();
				iCaptionHeaderArray->Delete(i);
				}
			}
		}
	delete iCaptionHeaderArray;
	delete iIconHeaderArray;
	if (iDataTypeHeaderArray)
		{
		const TInt maxIndex=iDataTypeHeaderArray->Count() - 1;
		for(TInt i=maxIndex; i>=0; i--)
			{
			const TSwizzle<TDataType>& dataType = iDataTypeHeaderArray->At(i).iDataType;
			if (dataType.IsPtr())
				{
				delete dataType.AsPtr();
				iDataTypeHeaderArray->Delete(i);
				}
			}
		}
	delete iDataTypeHeaderArray;
	if (iViewDataHeaderArray)
		{
		const TInt maxIndex=iViewDataHeaderArray->Count() - 1;
		for(TInt i=maxIndex; i>=0; i--)
			{
			const TSwizzle<CApaAIFViewData>& viewData = iViewDataHeaderArray->At(i).iViewData;
			if (viewData.IsPtr())
				{
				delete viewData.AsPtr();
				iViewDataHeaderArray->Delete(i);
				}
			}
		}
	delete iViewDataHeaderArray;
	if (iFileOwnershipInfoHeaderArray)
		{
		const TInt maxIndex=iFileOwnershipInfoHeaderArray->Count() - 1;
		for(TInt i=maxIndex; i>=0; i--)
			{
			const TSwizzle<HBufC>& fileName = iFileOwnershipInfoHeaderArray->At(i).iOwnedFileName;
			if (fileName.IsPtr())
				{
				delete fileName.AsPtr();
				iFileOwnershipInfoHeaderArray->Delete(i);
				}
			}
		}
	delete iFileOwnershipInfoHeaderArray;
	delete iStore;
	}

void CApaAppInfoFile::ConstructL()
	{
	iCaptionHeaderArray=new(ELeave) CArrayFixFlat<TCaptionHeader>(4);
	iIconHeaderArray=new(ELeave) CArrayFixFlat<TIconHeader>(4);
	iDataTypeHeaderArray=new(ELeave) CArrayFixFlat<TDataTypeHeader>(4);
	iViewDataHeaderArray=new(ELeave) CArrayFixFlat<TViewDataHeader>(4);
	iFileOwnershipInfoHeaderArray=new(ELeave) CArrayFixFlat<TFileOwnershipInfoHeader>(4);
	}

//
// Class CApaAppInfoFileReader
//

CApaAppInfoFileReader::CApaAppInfoFileReader(RFs& aFs)
	:CApaAppInfoFile(aFs)
	{}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER CApaAppInfoFileReader::~CApaAppInfoFileReader()
/** Destructor.

Destroys all resources owned by the aif file reader. */
	{
	delete iDefaultCaption;
	delete iAifFileName;
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER CApaAppInfoFileReader* CApaAppInfoFileReader::NewLC(RFs& aFs, const TDesC& aFileName,TUid aUid)
/** Allocates and constructs the aif file reader and internalises the contents 
of the specified aif file.

@param aFs Connected session with the file server.
@param aFileName The full path and (language-neutral) filename of the aif file.
@param aApplicationUid The application's UID. If you specify a value other 
than KNullUid, the aif file must have this value as its third (most derived) 
UID.
@leave KErrCorrupt The UID specified is incorrect, or if KNullUid was specified, 
the aif file's second UID was not KUidAppInfoFile.
@return Pointer to the newly constructed and initialised aif file reader. The 
object is left on the cleanup stack. */
	{
	CApaAppInfoFileReader* self=new(ELeave) CApaAppInfoFileReader(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName,aUid);
	return self;
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER CApaAppInfoFileReader* CApaAppInfoFileReader::NewL(RFs& aFs, const TDesC& aFileName,TUid aUid)
/** Allocates and constructs the aif file reader and internalises the contents 
of the specified aif file.

@param aFs Connected session with the file server.
@param aFileName The full path and (language-neutral) filename of the aif file.
@param aApplicationUid The application's UID. If you specify a value other 
than KNullUid, the aif file must have this as its third (most derived) UID.
@leave KErrCorrupt The UID specified is incorrect, or if KNullUid was specified, 
the aif file's second UID was not KUidAppInfoFile.
@return Pointer to the newly constructed and initialised aif file reader. The 
object is popped from the cleanup stack. */
	{
	CApaAppInfoFileReader* self=CApaAppInfoFileReader::NewLC(aFs, aFileName,aUid);
	CleanupStack::Pop(self);
	return self;
	}

void CApaAppInfoFileReader::ConstructL(const TDesC& aFileName,TUid aMostDerivedUid)
	{
	__DECLARE_NAME(_S("CApaAppInfoFileReader"));
	CApaAppInfoFile::ConstructL();
	TParse parse;
	User::LeaveIfError(parse.Set(aFileName,NULL,NULL));
	iDefaultCaption=parse.Name().AllocL();
	TFileName localFileName;
	localFileName.Copy(aFileName);
	BaflUtils::NearestLanguageFile(iFs,localFileName);
	iRomPointer=iFs.IsFileInRom(localFileName);

	iAifFileName = localFileName.AllocL();

	// Get the file type
	TEntry entry;
	User::LeaveIfError(iFs.Entry(localFileName, entry));
	if (!entry.IsTypeValid())
		{
		// Invalid UID
		User::Leave(KErrCorrupt);
		}

	// Determine the type of AIF file
	const TUidType& type = entry.iType;
	if ((
#if defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER)
		type[1] == KUidInterimFormatFileForJavaMIDletInstaller
#endif
		) && (aMostDerivedUid == KNullUid || type[2] == aMostDerivedUid))
		{
		// Read old AIF format file
		CFileStore* store = CFileStore::OpenL(iFs,localFileName,EFileRead|EFileShareReadersOnly);
		iStore = store;
		RStoreReadStream inStream;
		inStream.OpenLC(*store,store->Root());
		InternalizeL(inStream);
		CleanupStack::PopAndDestroy(&inStream);
		}
	else
		{
		// it's the wrong file!!!!!
		User::Leave(KErrCorrupt);
		}
	}

void CApaAppInfoFileReader::LoadAifFileVersionTwoL(const TDesC& aFileName, TUid aMostDerivedUid)
	{
	// Version 2 AIF format:
	//
	//  +--------------------- - - -------- - - ---+
	//  |  UID  | RSC len | RSC block |  MBM block |
	//  +--------------------- - - -------- - - ---+
	//
	// UID = 4 bytes (incl. checksum)
	// RSC len = 4 bytes
	// RSC block = RSC len bytes + padding to make MBM block 4-byte aligned
	// MBM block = remainder of file, 4-byte aligned
	//
	// Read 16-bit block for length of RSC file
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);
	TInt aifHeaderOffset = 16;	// skip over UID and checksum
	User::LeaveIfError(file.Seek(ESeekStart, aifHeaderOffset));
	TBuf8<4> buffer;
	User::LeaveIfError(file.Read(buffer));
	const TInt rscLength = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));
	CleanupStack::PopAndDestroy(&file);

	// Open RSC file using offset
	RResourceFile resFile;
	resFile.OpenL(iFs, aFileName, aifHeaderOffset + 4, rscLength);
	CleanupClosePushL(resFile);

	// Read the first resource (AIF_DATA struct)
	HBufC8* resource = resFile.AllocReadLC(1);

	TResourceReader reader;
	reader.SetBuffer(resource);

	// Read app UID
	const TInt uid = reader.ReadInt32();
	if ((aMostDerivedUid != KNullUid) && (uid != aMostDerivedUid.iUid))
		{
		User::Leave(KErrCorrupt);
		}
	// Num icons
	const TInt numIcons = reader.ReadInt16();

	// Caption array
	const TInt captionArraySize = reader.ReadInt16();
	TInt i;
	for (i=0; i < captionArraySize; i++)
		{
		TCaptionHeader header;
		header.iLanguage = static_cast<TLanguage>(reader.ReadInt16());
		TPtrC caption = reader.ReadTPtrC();
		header.iCaption = caption.AllocLC();
		iCaptionHeaderArray->AppendL(header);
		CleanupStack::Pop(header.iCaption);
		}

	// Read capability flags
	iCapability.iAppIsHidden = reader.ReadInt8();
	iCapability.iEmbeddability = static_cast<TApaAppCapability::TEmbeddability>(reader.ReadInt8());
	iCapability.iSupportsNewFile = reader.ReadInt8();
	iCapability.iLaunchInBackground = reader.ReadInt8();

	// Group name
	iCapability.iGroupName = reader.ReadTPtrC();

	// Datatype list
	const TInt dataTypeArraySize = reader.ReadInt16();
	for (i=0; i < dataTypeArraySize; i++)
		{
		TDataTypeHeader dataTypeHeader;
		dataTypeHeader.iPriority = (TDataTypePriority)reader.ReadInt16();
		TPtrC8 dataTypePtr = reader.ReadTPtrC8();
		TDataType* dataType = new(ELeave) TDataType(dataTypePtr);
		CleanupStack::PushL(dataType);
		dataTypeHeader.iDataType = dataType;
		iDataTypeHeaderArray->AppendL(dataTypeHeader);
		CleanupStack::Pop(dataType);
		}

	// Read MBM block
	iMbmOffset = aifHeaderOffset + 4 + rscLength;
	// Calculate any padding after the RSC (MBM block must be 4-byte aligned)
	TInt mbmPadding = (rscLength % 4);
	if (mbmPadding)
		mbmPadding = 4 - mbmPadding;
	iMbmOffset += mbmPadding;
	TInt iconIndex = 0;
	if (fileSize > iMbmOffset)
		{
		// Populate the icon array
		AddIconHeadersL(aFileName, iMbmOffset, numIcons);
		iconIndex += numIcons;
		}

	// View list
	const TInt viewListArraySize = reader.ReadInt16();
	for (i=0; i < viewListArraySize; i++)
		{
		CApaAIFViewData* viewData = CApaAIFViewData::NewLC();
		viewData->SetViewUid(TUid::Uid(reader.ReadInt32()));	// Read UID
		viewData->SetScreenMode(reader.ReadInt32());			// Read screenmode
		TInt numViewIcons = reader.ReadInt16();					// Read num icons
		TInt j = 0;
		for ( ; j < numViewIcons; j++)
			{
			viewData->AddIconIndexL(iconIndex);
			iconIndex++;
			}
		// Read caption array
		const TInt viewCaptArraySize = reader.ReadInt16();
		for (j=0; j < viewCaptArraySize; j++)
			{
			TLanguage langCode = static_cast<TLanguage>(reader.ReadInt16());
			TPtrC caption = reader.ReadTPtrC();
			viewData->AddCaptionL(langCode, caption);
			}
		TViewDataHeader header;
		header.iViewData = viewData;
		iViewDataHeaderArray->AppendL(header);
		CleanupStack::Pop(viewData);
		}

	// File ownership list
	const TInt fileOwnershipArraySize = reader.ReadInt16();
	for (i=0; i < fileOwnershipArraySize; i++)
		{
		// Filename
		TFileOwnershipInfoHeader ownership;
		TPtrC fileNamePtr = reader.ReadTPtrC();
		ownership.iOwnedFileName = fileNamePtr.AllocLC();
		iFileOwnershipInfoHeaderArray->AppendL(ownership);
		CleanupStack::Pop(ownership.iOwnedFileName);
		}
	CleanupStack::PopAndDestroy(2, &resFile);
	}

void CApaAppInfoFileReader::AddIconHeadersL(const TDesC& aFileName, TInt32 aFileOffset, TInt aNumIcons)
	{
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bmp);
	for (TInt i=0; i < aNumIcons; i++)
		{
		User::LeaveIfError(bmp->Load(aFileName, i * 2, ETrue, aFileOffset));
		TSize iconSize = bmp->SizeInPixels();
		TIconHeader header;
		header.iIcon = NULL;
		header.iIconMain = NULL;
		header.iIconMask = NULL;
		header.iIconSideInPixels = iconSize.iWidth;
		iIconHeaderArray->AppendL(header);
		}
	CleanupStack::PopAndDestroy(bmp);
	}

void CApaAppInfoFileReader::InternalizeL(RReadStream& aStream)
	{
	aStream>> *iCaptionHeaderArray;		// Internalizes the headers only
	aStream>> *iIconHeaderArray;
	aStream>> iCapability;
	TInt version=EAifVersionOriginal;
	TRAP_IGNORE(version=aStream.ReadInt32L());
	if (version<EAifVersionAddsDataType)
		return;
	aStream>> *iDataTypeHeaderArray;
	if (version<EAifVersionAddsViewData)
		return;
	aStream>> *iViewDataHeaderArray;
	if (version<EAifVersionAddsFileOwnershipInfo)
		return;
	aStream>> *iFileOwnershipInfoHeaderArray;
#if defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER)
	if (version<EAifVersionAddsJavaMIDletInfo)
		return;
	iJavaMIDletInfo_AmsAuthId=aStream.ReadUint32L();
	iJavaMIDletInfo_MIDlet=aStream.ReadInt32L();
#endif
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER TApaAppCaption CApaAppInfoFileReader::CaptionL(TLanguage aLanguage)
// Find language id and return matching caption, loaded from file.
// Does not allocate storage for the caption so there's no need to delete the array elements.
/** Searches for the most appropriate caption in the aif file for the language 
specified.

If a caption in the specified language is not found, the function searches 
for the best alternative using the language downgrade path.

If there are no captions at all in the aif file, a default caption is used 
(the aif filename).

@param aLanguage The language of the required caption.
@return The application's caption. */
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	RArray<TLanguage> downgradePath;
	CleanupClosePushL(downgradePath);
	BaflUtils::GetDowngradePathL(fs,aLanguage,downgradePath);
	TApaAppCaption result(*iDefaultCaption);
	const TInt count=iCaptionHeaderArray->Count();
	if (count>0)
		{
		const TInt languageCount=downgradePath.Count();
		TInt captionIndex=KErrNotFound;
		for (TInt j=0; j<languageCount; j++)
			{
			for (TInt i=0;i<count;i++)
				{		
				if ((*iCaptionHeaderArray)[i].iLanguage==downgradePath[j])
					{
					captionIndex=i;
					j=languageCount;//to exit outer loop
					break;
					}
				}
			}
		TCaptionHeader pCaptionHeader;
		if (captionIndex!=KErrNotFound)
			{
			pCaptionHeader=(*iCaptionHeaderArray)[captionIndex];
			}
		else		// use caption for the first language specified in the aif file
					// since we don't have anything more suitable
			{ 		
			pCaptionHeader=(*iCaptionHeaderArray)[0];
			}

		if (iStore)
			{
			RStoreReadStream inStream;
			inStream.OpenLC(*iStore,pCaptionHeader.iCaption.AsId());
			inStream >> result;
			CleanupStack::PopAndDestroy(&inStream);
			}
		else
			{
			__ASSERT_DEBUG(pCaptionHeader.iCaption.IsPtr(), Panic(EPanicNoCaption));
			result = *(pCaptionHeader.iCaption);
			}
		downgradePath.Reset();
		}
	CleanupStack::PopAndDestroy(2, &fs);
	return result;
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileReader::StretchDrawL(CFbsBitmap* aSource,CFbsBitmap* aTarget,TSize aSizeInPixels)
/** Creates a new bitmap based on another, stretched or compressed to the specified 
size.

@param aSource The source bitmap to copy.
@param aTarget The target bitmap. On return, this is set to be a copy of aSource, 
but stretched or compressed to the specified size.
@param aSizeInPixels The required size for the target bitmap. */
	{
	User::LeaveIfError(aTarget->Create(aSizeInPixels,aSource->DisplayMode()));
	CFbsBitmapDevice* bitDev=CFbsBitmapDevice::NewL(aTarget);
	CleanupStack::PushL(bitDev);
	CFbsBitGc* gc;
	User::LeaveIfError(bitDev->CreateContext((CGraphicsContext*&)gc));
	gc->DrawBitmap(TRect(aSizeInPixels),aSource);
	delete gc;
	CleanupStack::PopAndDestroy(bitDev);
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER TInt CApaAppInfoFileReader::NumberOfBitmaps() const
/** Returns the number of icons in the aif file.

@return The number of icons in the aif file. */
	{
	return iIconHeaderArray->Count();
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER CApaMaskedBitmap* CApaAppInfoFileReader::CreateMaskedBitmapByIndexLC(TInt aIndex)
	{
	const TInt count=iIconHeaderArray->Count();
	if (count<=aIndex) // panic?
		User::Leave(KErrNotFound);
	//
	CApaMaskedBitmap* icon=CApaMaskedBitmap::NewLC();
	if (iStore)
		{
		RStoreReadStream inStream;
		inStream.OpenLC(*iStore,(*iIconHeaderArray)[aIndex].iIcon.AsId());
		MStreamBuf* src=inStream.Source();
		__ASSERT_DEBUG(src, Panic(EPanicNullPointer));
		const TUid uid = TUid::Uid(inStream.ReadInt32L());
		src->SeekL(MStreamBuf::ERead,-4);
		if (uid==KCBitwiseBitmapUid)
			{
			TUint32* fileOffsetPtr=(TUint32*) src;
			fileOffsetPtr+=4;
			icon->SetRomBitmapL(iRomPointer+(*fileOffsetPtr));
			}
		else
			{
			inStream >> *icon;
			}
		CleanupStack::PopAndDestroy(&inStream);
		}
	else
		{
		CFbsBitmap* mask = new(ELeave) CFbsBitmap();
		CleanupStack::PushL(mask);
		TInt mbmIndex = aIndex * 2;		// we need to map 'icon index' to 'mbm index' (bmp + mask)
		__ASSERT_DEBUG(iAifFileName, Panic(EPanicNullPointer));
		User::LeaveIfError(icon->Load(*iAifFileName, mbmIndex, ETrue, iMbmOffset));
		User::LeaveIfError(mask->Load(*iAifFileName, mbmIndex + 1, ETrue, iMbmOffset));
		icon->SetMaskBitmap(mask);
		CleanupStack::Pop(mask);
		}
	//
	return icon;
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER CApaMaskedBitmap* CApaAppInfoFileReader::CreateMaskedBitmapL(TInt aIconSideInPixels)
//
// Use CreateMaskedBitmapByIndexLC. To access a bitmap by size use RApaLsSession APIs.
//
/** Creates a new masked bitmap based on the one in the aif file whose size is 
closest to the size specified.

If all bitmaps in the file are larger than the size specified, the new bitmap 
is based on the smallest and is shrunk to the specified size.

@deprecated
@param aIconSideInPixels The maximum width and height in pixels of the bitmap. 
The new bitmap will be this size, or smaller.
@leave KErrNotFound There are no bitmaps in the file.
@return Pointer to the new bitmap. The caller takes ownership. */
	{
	__ASSERT_DEBUG(iIconHeaderArray, Panic(EPanicNullPointer));
	const TInt count=iIconHeaderArray->Count();
	if (count==0)
		User::Leave(KErrNotFound);
	//
	TInt closest=0;
	TInt smallest=0;
	TInt minSoFar=KMaxTInt;
	TInt smallestSoFar=KMaxTInt;
	TInt delta=0;
	for (TInt i=0;i<count;i++)
		{
		TIconHeader* pIconHeader=(&(*iIconHeaderArray)[i]);
		delta = aIconSideInPixels-pIconHeader->iIconSideInPixels; // Abs()
		if (pIconHeader->iIconSideInPixels<smallestSoFar)
			{ // the smallest is used if they're all too big
			smallest = i;
			smallestSoFar = pIconHeader->iIconSideInPixels;
			}
		if (delta>=0 && delta<minSoFar)
			{
			minSoFar=delta;
			closest=i;
			if (delta==0)
				break;
			}
		}
	//
	if (minSoFar==KMaxTInt)
		closest = smallest;
	//
	CApaMaskedBitmap* icon = CreateMaskedBitmapByIndexLC(closest);
	//
	if (minSoFar==KMaxTInt)
		{// all the icons are too big - squash down the smallest
		CApaMaskedBitmap* newIcon=CApaMaskedBitmap::NewLC();
		TSize iconSize(aIconSideInPixels,aIconSideInPixels);
		StretchDrawL(icon,newIcon,iconSize);
		StretchDrawL(icon->Mask(),newIcon->Mask(),iconSize);
		CleanupStack::Pop(newIcon);
		CleanupStack::PopAndDestroy(icon);
		return newIcon;
		}
	CleanupStack::Pop(icon);
	return icon;
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileReader::Capability(TDes8& aInfo)const
/** Gets the capability information read from the aif file.

@param aInfo A TApaAppCapabilityBuf object. On return, this contains the application's 
capabilities. */
	{
	TApaAppCapabilityBuf buf(iCapability);
	TApaAppCapability::CopyCapability(aInfo,buf);
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileReader::DataTypesSupportedL(CArrayFix<TDataTypeWithPriority>& aTypeList) const
/** Gets the MIME types supported by the application, and their priorities. 

@param aTypeList An empty array. On return, this contains the MIME type priorities 
read from the aif file. MIME type priorities determine which application is 
launched to display a particular document.
@panic APGRFX 29 The aTypeList array is not empty when calling the function. 
Debug builds only.
@panic APGRFX 33 The internal array holding the MIME type information is uninitialised. 
Debug builds only. */
	{
	__ASSERT_DEBUG(aTypeList.Count()==0,Panic(EDPanicArrayNotEmpty));
	__ASSERT_DEBUG(iDataTypeHeaderArray,Panic(EDPanicInvalidDataTypeArray));
	TInt count=iDataTypeHeaderArray->Count();
	for (TInt i=0; i<count; i++)
		{
		const TDataTypeHeader& header=(*iDataTypeHeaderArray)[i];
		if (iStore)
			{
			RStoreReadStream inStream;
			inStream.OpenLC(*iStore,header.iDataType.AsId());
			TDataType dataType;
			inStream >> dataType;
			aTypeList.AppendL(TDataTypeWithPriority(dataType,header.iPriority));
			CleanupStack::PopAndDestroy(&inStream);
			}
		else
			{
			aTypeList.AppendL(TDataTypeWithPriority(*(header.iDataType), header.iPriority));
			}
		}
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileReader::GetViewsL(CArrayPtr<CApaAIFViewData>& aViewList) const
/** Gets the application view data contained in the aif file.

The view data is an array of view UIDs and their associated captions and icons.

@internalComponent
@param aViewList An empty array. On return, this contains the view data read 
from the aif file. If the file does not contain any application view data, 
this array will be empty on return.
@panic APGRFX 29 The aViewList array is not empty when calling the function. 
Debug builds only.
@panic APGRFX 34 The internal array holding the view data is uninitialised. 
Debug builds only. */
	{
	__ASSERT_DEBUG(aViewList.Count()==0,Panic(EDPanicArrayNotEmpty));
	__ASSERT_DEBUG(iViewDataHeaderArray,Panic(EDPanicInvalidViewArray));
	TInt count=iViewDataHeaderArray->Count();
	for (TInt i=0; i<count; i++)
		{
		const TViewDataHeader& header=(*iViewDataHeaderArray)[i];
		if (iStore)
			{
			CApaAIFViewData* viewData=CApaAIFViewData::NewLC();
			RStoreReadStream inStream;
			inStream.OpenLC(*iStore,header.iViewData.AsId());
			inStream >> *viewData;
			aViewList.AppendL(viewData);
			CleanupStack::PopAndDestroy(&inStream);
			CleanupStack::Pop(viewData);
			}
		else
			{
			// Load any icons for the view
			// Have to create a copy here as the old AIF format (above) gives external ownership
			// of CApaAIFViewData items in aViewList.
			CApaAIFViewData* headerViewData = header.iViewData.AsPtr();
			CApaAIFViewData* viewData = CApaAIFViewData::NewLC(*headerViewData);
			viewData->LoadIconsL(*iAifFileName, iMbmOffset);
			aViewList.AppendL(viewData);
			CleanupStack::Pop(viewData);
			}
		}
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileReader::GetOwnedFilesL(CDesCArray& aOwnedFilesList) const
/** Gets a list of the files that the application has claimed ownership of in the 
aif file.

@param aOwnedFilesList An empty array. On return, this contains the names 
of the files identified as being owned by the application.
@panic APGRFX 29 The aOwnedFilesList array is not empty when calling the function. 
Debug builds only.
@panic APGRFX 35 The internal array holding the owned files list is uninitialised. 
Debug builds only. */
	{
	__ASSERT_DEBUG(aOwnedFilesList.Count()==0,Panic(EDPanicArrayNotEmpty));
	__ASSERT_DEBUG(iFileOwnershipInfoHeaderArray,Panic(EDPanicInvalidFileOwnershipArray));
	const TInt count=iFileOwnershipInfoHeaderArray->Count();
	for (TInt ii=0; ii<count; ii++)
		{
		const TFileOwnershipInfoHeader& header=(*iFileOwnershipInfoHeaderArray)[ii];
		if (iStore)
			{
			TFileName fileName;
			RStoreReadStream inStream;
			inStream.OpenLC(*iStore,header.iOwnedFileName.AsId());
			inStream >> fileName;
			aOwnedFilesList.AppendL(fileName);
			CleanupStack::PopAndDestroy(&inStream);
			}
		else
			{
			aOwnedFilesList.AppendL(*(header.iOwnedFileName));
			}
		}
	}


//
// Class CApaAppInfoFileWriter
//

CApaAppInfoFileWriter::CApaAppInfoFileWriter(RFs& aFs)
	:CApaAppInfoFile(aFs)
	{}

EXPORT_C CApaAppInfoFileWriter::~CApaAppInfoFileWriter()
// Must delete caption and icon pointers that have been set in AddCaptionL and AddIconL
	{
	if (iCaptionHeaderArray)
		{
		const TInt maxIndex=iCaptionHeaderArray->Count() - 1;
		for(TInt i=maxIndex; i>=0; i--)
			{
			const TCaptionHeader& pCaption=(*iCaptionHeaderArray)[i];
			if (pCaption.iCaption.IsPtr())
				{
				delete pCaption.iCaption.AsPtr();
				iCaptionHeaderArray->Delete(i);
				}
			}
		}
	if (iIconHeaderArray)
		{
		const TInt maxIndex=iIconHeaderArray->Count() - 1;
		for(TInt i=maxIndex; i>=0; i--)
			{
			const TIconHeader& pIcon=(*iIconHeaderArray)[i];
			if (pIcon.iIcon.IsPtr())
				{
				delete pIcon.iIcon.AsPtr();
				iIconHeaderArray->Delete(i);
				}
			}
		}
	if (iDataTypeHeaderArray)
		{
		const TInt maxIndex=iDataTypeHeaderArray->Count() - 1;
		for(TInt i=maxIndex; i>=0; i--)
			{
			const TDataTypeHeader& pData=(*iDataTypeHeaderArray)[i];
			if (pData.iDataType.IsPtr())
				{
				delete pData.iDataType.AsPtr();
				iDataTypeHeaderArray->Delete(i);
				}
			}
		}
	if (iViewDataHeaderArray)
		{
		const TInt maxIndex=iViewDataHeaderArray->Count() - 1;
		for(TInt i=maxIndex; i>=0; i--)
			{
			const TViewDataHeader& pView=(*iViewDataHeaderArray)[i];
			if (pView.iViewData.IsPtr())
				{
				delete pView.iViewData.AsPtr();
				iViewDataHeaderArray->Delete(i);
				}
			}
		}
	if (iFileOwnershipInfoHeaderArray)
		{
		const TInt maxIndex=iFileOwnershipInfoHeaderArray->Count() - 1;
		for(TInt i=maxIndex; i>=0; i--)
			{
			const TFileOwnershipInfoHeader& pView=(*iFileOwnershipInfoHeaderArray)[i];
			if (pView.iOwnedFileName.IsPtr())
				{
				delete pView.iOwnedFileName.AsPtr();
				iFileOwnershipInfoHeaderArray->Delete(i);
				}
			}
		}
	delete iMap;		// Must delete before iStore
	}

// CApaResourceFileWriterBase::MDataSink

void CApaResourceFileWriterBase::MDataSink::WriteBufferL(const TDesC8& aBuffer)
	{
	if (aBuffer.Length()>0)
		{
		if (iBufferSinkForCompressedUnicodeFormat!=NULL)
			{
			WriteInCompressedUnicodeFormatL(aBuffer.Length(), aBuffer, EFalse);
			}
		else
			{
			DoWriteBufferL(aBuffer);
			iNumberOfBytesWhenUncompressed+=aBuffer.Length();
			}
		}
	}

void CApaResourceFileWriterBase::MDataSink::WriteCompressedUnicodeRunL(TInt aNumberOfBytesWhenUncompressed, const TDesC8& aTextAsCompressedUnicode)
	{
	WriteInCompressedUnicodeFormatL(aNumberOfBytesWhenUncompressed, aTextAsCompressedUnicode, ETrue);
	}

void CApaResourceFileWriterBase::MDataSink::WriteInCompressedUnicodeFormatL(TInt aNumberOfBytesWhenUncompressed, const TDesC8& aData, TBool aCompressedUnicode)
	{
	__ASSERT_DEBUG(iBufferSinkForCompressedUnicodeFormat!=NULL, Panic(EPanicWrongResourceFormat1));
	const TInt dataLength=aData.Length();
	__ASSERT_DEBUG(((dataLength==0) && ((aNumberOfBytesWhenUncompressed==0) || (aNumberOfBytesWhenUncompressed==1))) ||
				   ((dataLength>0) && (aNumberOfBytesWhenUncompressed>0)), Panic(EPanicBadCompressedUnicodeRun));
	if (dataLength>0)
		{
		if ((iNumberOfBytesWhenUncompressed==0) && (iBufferSinkForCompressedUnicodeFormat->iNumberOfBytesWhenUncompressed==0))
			{
			if (aCompressedUnicode)
				{
				iFlags|=EFlag_InCompressedUnicodeRun;
				}
			else
				{
				iFlags&=~EFlag_InCompressedUnicodeRun;
				}
			}
		else if ((iFlags&EFlag_InCompressedUnicodeRun)^(aCompressedUnicode? EFlag_InCompressedUnicodeRun: 0)) // if we're changing the state of the EFlag_InCompressedUnicodeRun flag
			{
			FlushL(EFalse);
			iFlags^=EFlag_InCompressedUnicodeRun; // toggle the EFlag_InCompressedUnicodeRun flag
			}
		static_cast<MDataSink*>(iBufferSinkForCompressedUnicodeFormat)->DoWriteBufferL(aData);
		}
	iBufferSinkForCompressedUnicodeFormat->iNumberOfBytesWhenUncompressed+=aNumberOfBytesWhenUncompressed;
	}

TInt CApaResourceFileWriterBase::MDataSink::NumberOfBytesWhenUncompressed() const
	{
	TInt numberOfBytesWhenUncompressed=iNumberOfBytesWhenUncompressed;
	if (iBufferSinkForCompressedUnicodeFormat!=NULL)
		{
		numberOfBytesWhenUncompressed+=iBufferSinkForCompressedUnicodeFormat->iNumberOfBytesWhenUncompressed;
		}
	return numberOfBytesWhenUncompressed;
	}

CApaResourceFileWriterBase::MDataSink::MDataSink(RBufferSink* aBufferSinkForCompressedUnicodeFormat)
	:iNumberOfBytesWhenUncompressed(0),
	 iFlags(0),
	 iBufferSinkForCompressedUnicodeFormat(aBufferSinkForCompressedUnicodeFormat)
	{
	}

void CApaResourceFileWriterBase::MDataSink::FlushL(TBool aFinalFlush)
	{
	if (iBufferSinkForCompressedUnicodeFormat!=NULL)
		{
		RBuf8 run;
		CleanupClosePushL(run);
		TInt numberOfBytesInRunWhenUncompressed=0;
		iBufferSinkForCompressedUnicodeFormat->FlushAndGetAndResetL(numberOfBytesInRunWhenUncompressed, run);
		if (numberOfBytesInRunWhenUncompressed>0)
			{
			if ((iNumberOfBytesWhenUncompressed==0) && ((iFlags&EFlag_InCompressedUnicodeRun)==0))
				{
				WriteRunLengthL(0); // insert initial zero-length run-length as we're not starting with compressed Unicode
				}
			__ASSERT_DEBUG(run.Length()>0, Panic(EPanicBadRunLength));
			WriteRunLengthL(run.Length());
			DoWriteBufferL(run);
			iNumberOfBytesWhenUncompressed+=numberOfBytesInRunWhenUncompressed;
			}
		CleanupStack::PopAndDestroy(&run);
		if (aFinalFlush && (iNumberOfBytesWhenUncompressed==0))
			{
			WriteRunLengthL(0); // write a zero-length run-length as the resource is completely empty
			}
		}
	}

void CApaResourceFileWriterBase::MDataSink::Reset(TInt& aNumberOfBytesWhenUncompressed)
	{
	aNumberOfBytesWhenUncompressed=iNumberOfBytesWhenUncompressed;
	iNumberOfBytesWhenUncompressed=0;
	iFlags=0;
	}

void CApaResourceFileWriterBase::MDataSink::WriteRunLengthL(TInt aRunLength)
	{
	__ASSERT_DEBUG((aRunLength&~0x7fff)==0, Panic(EPanicBadRunLengthParameter));
	__ASSERT_DEBUG(CompressedUnicodeFormat(), Panic(EPanicWrongResourceFormat2));
	TBuf8<2> buffer;
	if (aRunLength&~0x7f)
		{
		buffer.Append((aRunLength>>8)|0x80);
		}
	buffer.Append(aRunLength&0xff);
	DoWriteBufferL(buffer); // must call DoWriteBufferL here (not WriteBufferL) so that iNumberOfBytesWhenUncompressed is not altered
	}

// CApaResourceFileWriterBase::RBufferSink

CApaResourceFileWriterBase::RBufferSink::RBufferSink(RBufferSink* aBufferSinkForCompressedUnicodeFormat)
	:MDataSink(aBufferSinkForCompressedUnicodeFormat)
	{
	}

void CApaResourceFileWriterBase::RBufferSink::ConstructLC()
	{
	iBuffer.CreateL(20);
	CleanupClosePushL(*this);
	}

void CApaResourceFileWriterBase::RBufferSink::Close()
	{
	iBuffer.Close();
	}

void CApaResourceFileWriterBase::RBufferSink::FlushAndGetAndResetL(TInt& aNumberOfBytesWhenUncompressed, RBuf8& aBuffer)
	{
	FlushL(ETrue);
	iBuffer.Swap(aBuffer);
	Reset(aNumberOfBytesWhenUncompressed);
	}

void CApaResourceFileWriterBase::RBufferSink::DoWriteBufferL(const TDesC8& aBuffer)
	{
	if (iBuffer.MaxLength()-iBuffer.Length()<aBuffer.Length())
		{
		iBuffer.ReAllocL(iBuffer.Length()+aBuffer.Length()+20);
		}
	iBuffer.Append(aBuffer);
	}

// CApaResourceFileWriterBase

CApaResourceFileWriterBase::CApaResourceFileWriterBase()
	{
	}

void CApaResourceFileWriterBase::DoGenerateFileContentsL(RBuf8& aBuffer, TUid aUid2, TUid aUid3) const
	{
	TInt mainResourceSizeInBytesWhenUncompressed=0;
	RBuf8 mainResourceInFormatContainingCompressedUnicode;
	MainResourceInCompiledFormatLC(mainResourceSizeInBytesWhenUncompressed, mainResourceInFormatContainingCompressedUnicode, ETrue);
	TInt temp=0;
	RBuf8 mainResourceInFormatNotContainingCompressedUnicode;
	MainResourceInCompiledFormatLC(temp, mainResourceInFormatNotContainingCompressedUnicode, EFalse);
	__ASSERT_DEBUG(temp==mainResourceSizeInBytesWhenUncompressed, Panic(EPanicInconsistentResourceSizeInBytes));
	TBool mainResourceInCompressedUnicodeFormat=ETrue;
	TPtrC8 mainResourceInSmallestFormat(mainResourceInFormatContainingCompressedUnicode);
	if (mainResourceInSmallestFormat.Length()>=mainResourceInFormatNotContainingCompressedUnicode.Length())
		{
		mainResourceInCompressedUnicodeFormat=EFalse;
		mainResourceInSmallestFormat.Set(mainResourceInFormatNotContainingCompressedUnicode);
		}
	TBool secondResourceInCompressedUnicodeFormat=EFalse;
	const TDesC8* const secondResource=SecondResourceL(secondResourceInCompressedUnicodeFormat);
	RBufferSink bufferSink(NULL);
	bufferSink.ConstructLC();

	WriteUidTypeL(bufferSink, aUid2, aUid3);
	WriteUint8L(bufferSink, 0); // flags
	WriteLittleEndianUint16L(bufferSink, (secondResource==NULL)? mainResourceSizeInBytesWhenUncompressed: Max(mainResourceSizeInBytesWhenUncompressed, secondResource->Length())); // the size in bytes of the largest resource in the file when uncompressed
	TUint bitArray=0; // bit-array (one per top-level resource) indicating whether each top-level resource contains compressed unicode or not
	if (mainResourceInCompressedUnicodeFormat)
		{
		bitArray|=0x01;
		}
	if (secondResourceInCompressedUnicodeFormat)
		{
		bitArray|=0x02;
		}
	WriteUint8L(bufferSink, bitArray);
	WriteBufferL(bufferSink, mainResourceInSmallestFormat);
	if (secondResource!=NULL)
		{
		WriteBufferL(bufferSink, *secondResource);
		}
	TInt filePosition=16+1+2+1;
	WriteLittleEndianUint16L(bufferSink, filePosition);
	filePosition+=mainResourceInSmallestFormat.Length();
	WriteLittleEndianUint16L(bufferSink, filePosition);
	if (secondResource!=NULL)
		{
		filePosition+=secondResource->Length();
		WriteLittleEndianUint16L(bufferSink, filePosition);
		}
	TInt notUsed;
	bufferSink.FlushAndGetAndResetL(notUsed, aBuffer);

	CleanupStack::PopAndDestroy(3, &mainResourceInFormatContainingCompressedUnicode);
	}

void CApaResourceFileWriterBase::WriteUidTypeL(MDataSink& aDataSink, TUid aUid2, TUid aUid3) const
	{
	aDataSink.WriteBufferL(TCheckedUid(TUidType(TUid::Uid(0x101f4a6b), aUid2, aUid3)).Des());
	}

void CApaResourceFileWriterBase::WriteTextL(MDataSink& aDataSink, const TDesC& aText) const
	{
	// LTEXT

	WriteUint8L(aDataSink, aText.Length());
	if (!aDataSink.CompressedUnicodeFormat())
		{
		if ((aDataSink.NumberOfBytesWhenUncompressed()%2)!=0)
			{
			WriteUint8L(aDataSink, 0xab);
			}
		const TInt lengthOfTextInBytes=aText.Size();
		WriteBufferL(aDataSink, TPtrC8(reinterpret_cast<const TUint8*>(aText.Ptr()), lengthOfTextInBytes));
		}
	else
		{
		TInt numberOfBytesWhenUncompressed=aText.Size();
		if ((aDataSink.NumberOfBytesWhenUncompressed()%2)!=0)
			{
			++numberOfBytesWhenUncompressed; // for the padding-byte 0xab when it's uncompressed
			}
		HBufC8* const textAsCompressedUnicode=AsCompressedUnicodeLC(aText);
		aDataSink.WriteCompressedUnicodeRunL(numberOfBytesWhenUncompressed, *textAsCompressedUnicode);
		CleanupStack::PopAndDestroy(textAsCompressedUnicode);
		}
	}

void CApaResourceFileWriterBase::WriteText8L(MDataSink& aDataSink, const TDesC8& aText8) const
	{
	// LTEXT8

	WriteUint8L(aDataSink, aText8.Length());
	WriteBufferL(aDataSink, aText8);
	}

void CApaResourceFileWriterBase::WriteUint8L(MDataSink& aDataSink, TUint aUint8) const
	{
	TBuf8<1> buffer;
	buffer.Append(aUint8&0xff);
	aDataSink.WriteBufferL(buffer);
	}

void CApaResourceFileWriterBase::WriteLittleEndianUint16L(MDataSink& aDataSink, TUint aUint16) const
	{
	TBuf8<2> buffer;
	buffer.Append(aUint16&0xff);
	buffer.Append((aUint16>>8)&0xff);
	aDataSink.WriteBufferL(buffer);
	}

void CApaResourceFileWriterBase::WriteLittleEndianUint32L(MDataSink& aDataSink, TUint aUint32) const
	{
	TBuf8<4> buffer;
	buffer.Append(aUint32&0xff);
	buffer.Append((aUint32>>8)&0xff);
	buffer.Append((aUint32>>16)&0xff);
	buffer.Append((aUint32>>24)&0xff);
	aDataSink.WriteBufferL(buffer);
	}

void CApaResourceFileWriterBase::WriteBufferL(MDataSink& aDataSink, const TDesC8& aBuffer) const
	{
	aDataSink.WriteBufferL(aBuffer);
	}

HBufC8* CApaResourceFileWriterBase::AsCompressedUnicodeLC(const TDesC& aUncompressedUnicode)
	{
	TUnicodeCompressor unicodeCompressor;
	TMemoryUnicodeSource decompressedUnicode1(aUncompressedUnicode.Ptr());
	TMemoryUnicodeSource decompressedUnicode2(aUncompressedUnicode.Ptr());
	
	// Create a buffer big enough to hold all the compressed output
	TInt compressedUnicodeSizeInBytes = TUnicodeCompressor::CompressedSizeL(decompressedUnicode1, aUncompressedUnicode.Length());
	HBufC8* const compressedUnicodeBuffer=HBufC8::NewLC(compressedUnicodeSizeInBytes);
	TUint8* const compressedUnicodeBuffer_asBytePointer=const_cast<TUint8*>(compressedUnicodeBuffer->Ptr());
	
	// Compress the Unicode string
	TInt numberOfInputElementsConsumed = 0;
	TInt numberOfOutputBytes = 0;
	unicodeCompressor.CompressL(compressedUnicodeBuffer_asBytePointer, decompressedUnicode2, compressedUnicodeSizeInBytes, aUncompressedUnicode.Length(), &numberOfOutputBytes, &numberOfInputElementsConsumed);
	TInt temp = 0;
	unicodeCompressor.FlushL(compressedUnicodeBuffer_asBytePointer, compressedUnicodeSizeInBytes, temp);
	numberOfOutputBytes+=temp;
	TPtr8 compressedUnicodeBuffer_asWritable(compressedUnicodeBuffer->Des());
	compressedUnicodeBuffer_asWritable.SetLength(numberOfOutputBytes);
	return compressedUnicodeBuffer;
	}

void CApaResourceFileWriterBase::MainResourceInCompiledFormatLC(TInt& aMainResourceSizeInBytesWhenUncompressed, RBuf8& aBuffer, TBool aCompressedUnicodeFormat) const
	{
	CleanupClosePushL(aBuffer);
	RBufferSink bufferSinkForCompressedUnicodeFormat(NULL);
	if (aCompressedUnicodeFormat)
		{
		bufferSinkForCompressedUnicodeFormat.ConstructLC();
		}
	RBufferSink bufferSink(aCompressedUnicodeFormat? &bufferSinkForCompressedUnicodeFormat: NULL);
	bufferSink.ConstructLC();
	MainResourceInCompiledFormatL(bufferSink);
	bufferSink.FlushAndGetAndResetL(aMainResourceSizeInBytesWhenUncompressed, aBuffer);
	CleanupStack::PopAndDestroy(&bufferSink);
	if (aCompressedUnicodeFormat)
		{
		CleanupStack::PopAndDestroy(&bufferSinkForCompressedUnicodeFormat);
		}
	}

// CApaRegistrationResourceFileWriter

/** 
Creates a new CApaRegistrationResourceFileWriter instance.

@param aAppUid The UID of the application.
@param aAppFile The name and extension of the file to generate.
@param aAttributes The attributes of the application. See the TApaAppCapability class for more details.
@return A pointer to the new CApaRegistrationResourceFileWriter instance.

@publishedPartner
@released
*/
EXPORT_C CApaRegistrationResourceFileWriter* CApaRegistrationResourceFileWriter::NewL(TUid aAppUid, const TDesC& aAppFile, TUint aAttributes)
// aAppFile does not need to have the drive set
	{ // static
	CApaRegistrationResourceFileWriter* const self=new(ELeave) CApaRegistrationResourceFileWriter(aAppUid, aAttributes);
	CleanupStack::PushL(self);
	self->ConstructL(aAppFile);
	CleanupStack::Pop(self);
	return self;
	}

/** 
The destructor for the CApaRegistrationResourceFileWriter class.

@publishedPartner
@released
*/
EXPORT_C CApaRegistrationResourceFileWriter::~CApaRegistrationResourceFileWriter()
	{
	delete iAppFile;
	delete iLocalisableResourceFile;
	delete iGroupName;
	delete iOpaqueData;

	TInt i;
	for (i=iDataTypeList.Count()-1; i>=0; --i)
		{
		delete iDataTypeList[i].iType;
		}
	iDataTypeList.Close();

	for (i=iFileOwnershipList.Count()-1; i>=0; --i)
		{
		delete iFileOwnershipList[i].iFileName;
		}
	iFileOwnershipList.Close();
	}

TUid CApaRegistrationResourceFileWriter::AppUid() const
/** @internalComponent */
	{
	return iAppUid;
	}

void CApaRegistrationResourceFileWriter::GenerateFileContentsL(RBuf8& aBuffer) const
/** @internalComponent */
	{
	DoGenerateFileContentsL(aBuffer, KUidAppRegistrationFile, iAppUid);
	}

void CApaRegistrationResourceFileWriter::SetLocalisableResourceFileL(const TDesC& aLocalisableResourceFile)
/** @internalComponent */
	{
	HBufC* const localisableResourceFile=aLocalisableResourceFile.AllocL();
	delete iLocalisableResourceFile;
	iLocalisableResourceFile=localisableResourceFile;
	}

/** 
Sets or clears the hidden attribute. The usual purpose of the hidden attribute is to 
decide if the application should appear in the task list or not but this could vary between
products.

@param aAppIsHidden The value of the hidden flag.

@publishedPartner
@released
*/
EXPORT_C void CApaRegistrationResourceFileWriter::SetAppIsHiddenL(TBool aAppIsHidden)
	{
	iAppIsHidden=aAppIsHidden? 1: 0;
	}

/** 
Sets the embeddability attribute. See the TApaAppCapability::TEmbeddability class for more details.

@param aEmbeddability The value of the embeddability flags.

@publishedPartner
@released
*/
EXPORT_C void CApaRegistrationResourceFileWriter::SetEmbeddabilityL(TApaAppCapability::TEmbeddability aEmbeddability)
	{
	iEmbeddability=aEmbeddability;
	}

/** 
Specifies if the application supports the creation of a new file or not.

@param aSupportsNewFile ETrue to specify that the application supports the creation of a new file.

@publishedPartner
@released
*/
EXPORT_C void CApaRegistrationResourceFileWriter::SetSupportsNewFileL(TBool aSupportsNewFile)
	{
	iSupportsNewFile=aSupportsNewFile? 1: 0;
	}

/** 
Specifies if the application must be launched in the background.

@param aLaunchInBackground ETrue if the application must be launched in the background, EFalse otherwise.

@publishedPartner
@released
*/
EXPORT_C void CApaRegistrationResourceFileWriter::SetLaunchInBackgroundL(TBool aLaunchInBackground)
	{
	iLaunchInBackground=aLaunchInBackground;
	}

/** 
Sets the name of the application group.

@param aGroupName The name of the application group.

@publishedPartner
@released
*/
EXPORT_C void CApaRegistrationResourceFileWriter::SetGroupNameL(const TDesC& aGroupName)
	{
	HBufC* const groupName=aGroupName.AllocL();
	delete iGroupName;
	iGroupName=groupName;
	}

/** 
Sets the default screen number. This can be used to specify the preferred screen on devices
that support more than one screen.

@param aDefaultScreenNumber The name of the default screen.

@publishedPartner
@released
*/
EXPORT_C void CApaRegistrationResourceFileWriter::SetDefaultScreenNumberL(TInt aDefaultScreenNumber)
	{
	iDefaultScreenNumber=aDefaultScreenNumber;
	}

/** 
Sets the opaque data. The opaque data is some data that is specific to the type of application.

@publishedPartner
@released
*/
EXPORT_C void CApaRegistrationResourceFileWriter::SetOpaqueDataL(const TDesC8& aOpaqueData)
	{
	HBufC8* const opaqueData=aOpaqueData.AllocL();
	delete iOpaqueData;
	iOpaqueData=opaqueData;
	}

/** 
Adds a datatype to the list of datatypes that the application can handle.

@param aPriority The priority.
@param aType The datatype.

@publishedPartner
@released
*/
EXPORT_C void CApaRegistrationResourceFileWriter::AddDataTypeL(TInt aPriority, const TDesC8& aType)
	{
	SDataType dataType;
	dataType.iPriority=aPriority;
	dataType.iType=aType.AllocLC();
	iDataTypeList.AppendL(dataType);
	CleanupStack::Pop(dataType.iType);
	}

/** 
Adds a file to the list of files owned by the CApaRegistrationResourceFileWriter instances. These files
are deleted if an error occurs whil registering the new applications.

@param aFileName The name of the file.

@publishedPartner
@released
*/
EXPORT_C void CApaRegistrationResourceFileWriter::AddFileOwnershipInfoL(const TDesC& aFileName)
	{
	SFileOwnershipInfo fileOwnershipInfo;
	fileOwnershipInfo.iFileName=aFileName.AllocLC();
	iFileOwnershipList.AppendL(fileOwnershipInfo);
	CleanupStack::Pop(fileOwnershipInfo.iFileName);
	}

CApaRegistrationResourceFileWriter::CApaRegistrationResourceFileWriter(TUid aAppUid, TUint aAttributes)
	:iAppUid(aAppUid),
	 iAppFile(NULL),
	 iAttributes(aAttributes),
	 iLocalisableResourceFile(NULL),
	 iAppIsHidden(0),
	 iEmbeddability(TApaAppCapability::ENotEmbeddable),
	 iSupportsNewFile(0),
	 iLaunchInBackground(0),
	 iGroupName(NULL),
	 iDefaultScreenNumber(0),
	 iOpaqueData(NULL)
	{
	}

void CApaRegistrationResourceFileWriter::ConstructL(const TDesC& aAppFile)
	{
	iAppFile=aAppFile.AllocL();
	iLocalisableResourceFile=NULL;
	}

void CApaRegistrationResourceFileWriter::WriteDataTypeL(MDataSink& aDataSink, const SDataType& aDataType) const
	{
	// DATATYPE

	// LONG priority
	WriteLittleEndianUint32L(aDataSink, aDataType.iPriority);

	// LTEXT8 type(KMaxDataTypeLength)
	WriteText8L(aDataSink, *aDataType.iType);
	}

void CApaRegistrationResourceFileWriter::WriteFileOwnershipInfoL(MDataSink& aDataSink, const SFileOwnershipInfo& aFileOwnershipInfo) const
	{
	// FILE_OWNERSHIP_INFO

	// LTEXT file_name(KMaxFileNameLength)
	WriteTextL(aDataSink, *aFileOwnershipInfo.iFileName);
	}

void CApaRegistrationResourceFileWriter::MainResourceInCompiledFormatL(MDataSink& aDataSink) const
	{
	// APP_REGISTRATION_INFO

	// LONG reserved_long = 0
	WriteLittleEndianUint32L(aDataSink, 0);

	// LLINK reserved_llink = 0
	WriteLittleEndianUint32L(aDataSink, 0);
	__ASSERT_DEBUG(iAppFile, Panic(EPanicNullPointer));
	// LTEXT app_file(KMaxFileNameLength) = ""
	WriteTextL(aDataSink, *iAppFile);

	// LONG attributes = 0
	WriteLittleEndianUint32L(aDataSink, iAttributes);

	// LTEXT localisable_resource_file(KMaxFileNameLength) = ""
	TPtrC localisableResourceFile(KNullDesC);
	if (iLocalisableResourceFile!=NULL)
		{
		localisableResourceFile.Set(*iLocalisableResourceFile);
		}
	WriteTextL(aDataSink, localisableResourceFile);

	// LONG localisable_resource_id = 1
	WriteLittleEndianUint32L(aDataSink, 1);

	// BYTE hidden = KAppNotHidden
	WriteUint8L(aDataSink, iAppIsHidden);

	// BYTE embeddability = KAppNotEmbeddable
	WriteUint8L(aDataSink, iEmbeddability);

	// BYTE newfile = KAppDoesNotSupportNewFile
	WriteUint8L(aDataSink, iSupportsNewFile);

	// BYTE launch = KAppLaunchInForeground
	WriteUint8L(aDataSink, iLaunchInBackground);

	// LTEXT group_name(KAppMaxGroupName) = ""
	TPtrC groupName(KNullDesC);
	if (iGroupName!=NULL)
		{
		groupName.Set(*iGroupName);
		}
	WriteTextL(aDataSink, groupName);

	// BYTE default_screen_number = 0
	WriteUint8L(aDataSink, iDefaultScreenNumber);

	// LEN WORD STRUCT datatype_list[]
	TInt i;
	const TInt numberOfDataTypes=iDataTypeList.Count();
	WriteLittleEndianUint16L(aDataSink, numberOfDataTypes);
	for (i=0; i<numberOfDataTypes; ++i)
		{
		WriteDataTypeL(aDataSink, iDataTypeList[i]);
		}

	// LEN WORD STRUCT file_ownership_list[]
	const TInt numberOfOwnershipItems=iFileOwnershipList.Count();
	WriteLittleEndianUint16L(aDataSink, numberOfOwnershipItems);
	for (i=0; i<numberOfOwnershipItems; ++i)
		{
		WriteFileOwnershipInfoL(aDataSink, iFileOwnershipList[i]);
		}

	// LEN WORD STRUCT service_list[]
	WriteLittleEndianUint16L(aDataSink, 0);

	// LLINK opaque_data = 0
	WriteLittleEndianUint32L(aDataSink, (iOpaqueData!=NULL)? 2: 0);
	}

const TDesC8* CApaRegistrationResourceFileWriter::SecondResourceL(TBool& aSecondResourceInCompressedUnicodeFormat) const
	{
	aSecondResourceInCompressedUnicodeFormat=EFalse;
	return iOpaqueData;
	}

// CApaLocalisableResourceFileWriter

EXPORT_C CApaLocalisableResourceFileWriter* CApaLocalisableResourceFileWriter::NewL(const TDesC& aShortCaption, const TDesC& aCaption, TInt aNumberOfIcons, const TDesC& aGroupName)
	{ // static
	CApaLocalisableResourceFileWriter* const self=new(ELeave) CApaLocalisableResourceFileWriter(aNumberOfIcons);
	CleanupStack::PushL(self);
	self->ConstructL(aShortCaption, aCaption, aGroupName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CApaLocalisableResourceFileWriter::~CApaLocalisableResourceFileWriter()
	{
	delete iShortCaption;
	delete iCaptionAndIcon.iCaption;
	delete iCaptionAndIcon.iIconFile;
	delete iGroupName;
	}

void CApaLocalisableResourceFileWriter::GenerateFileContentsL(RBuf8& aBuffer) const
/** @internalComponent */
	{
	DoGenerateFileContentsL(aBuffer, TUid::Null(), TUid::Null());
	}

void CApaLocalisableResourceFileWriter::SetIconFileL(const TDesC& aIconFile)
/** @internalComponent */
	{
	HBufC* const iconFile=aIconFile.AllocL();
	delete iCaptionAndIcon.iIconFile;
	iCaptionAndIcon.iIconFile=iconFile;
	}

CApaLocalisableResourceFileWriter::CApaLocalisableResourceFileWriter(TInt aNumberOfIcons)
	:iShortCaption(NULL),
	 iGroupName(NULL)
	{
	iCaptionAndIcon.iCaption=NULL;
	iCaptionAndIcon.iNumberOfIcons=aNumberOfIcons;
	iCaptionAndIcon.iIconFile=NULL;
	}

void CApaLocalisableResourceFileWriter::ConstructL(const TDesC& aShortCaption, const TDesC& aCaption, const TDesC& aGroupName)
	{
	iShortCaption=aShortCaption.AllocL();
	iCaptionAndIcon.iCaption=aCaption.AllocL();
	iCaptionAndIcon.iIconFile=NULL;
	iGroupName=aGroupName.AllocL();
	}

void CApaLocalisableResourceFileWriter::WriteCaptionAndIconInfoL(MDataSink& aDataSink, const SCaptionAndIconInfo& aCaptionAndIcon) const
	{
	// CAPTION_AND_ICON_INFO

	// LONG reserved_long = 0
	WriteLittleEndianUint32L(aDataSink, 0);

	// LLINK reserved_llink = 0
	WriteLittleEndianUint32L(aDataSink, 0);

	// LTEXT caption(KMaxCaption) = ""
	WriteTextL(aDataSink, *aCaptionAndIcon.iCaption);

	// WORD number_of_icons = 0
	WriteLittleEndianUint16L(aDataSink, aCaptionAndIcon.iNumberOfIcons);

	// LTEXT icon_file(KMaxFileNameLength) = ""
	TPtrC iconFile(KNullDesC);
	if (aCaptionAndIcon.iIconFile!=NULL)
		{
		iconFile.Set(*aCaptionAndIcon.iIconFile);
		}
	WriteTextL(aDataSink, iconFile);
	}

void CApaLocalisableResourceFileWriter::MainResourceInCompiledFormatL(MDataSink& aDataSink) const
	{
	// LOCALISABLE_APP_INFO

	// LONG reserved_long = 0
	WriteLittleEndianUint32L(aDataSink, 0);

	// LLINK reserved_llink = 0
	WriteLittleEndianUint32L(aDataSink, 0);
	__ASSERT_DEBUG(iShortCaption, Panic(EPanicNullPointer));
	// LTEXT short_caption(KMaxCaption) = ""
	WriteTextL(aDataSink, *iShortCaption);

	// STRUCT caption_and_icon
	WriteCaptionAndIconInfoL(aDataSink, iCaptionAndIcon);

	// LEN WORD STRUCT view_list[]
	WriteLittleEndianUint16L(aDataSink, 0);
	__ASSERT_DEBUG(iGroupName, Panic(EPanicNullPointer));
	//	LTEXT group_name(KAppMaxGroupName) = ""
	WriteTextL(aDataSink, *iGroupName);
	}

const TDesC8* CApaLocalisableResourceFileWriter::SecondResourceL(TBool& aSecondResourceInCompressedUnicodeFormat) const
	{
	aSecondResourceInCompressedUnicodeFormat=EFalse;
	return NULL;
	}

#if defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER)

// ForJavaMIDletInstaller

EXPORT_C void ForJavaMIDletInstaller::CheckInterimFormatFileNotCorruptL(RFile& aInterimFormatFile)
	{ // static
	RFile temporaryDuplicateOfFile; // this is needed as CFileStore::FromLC takes ownership of the file that it's passed, and closes the RFile object passed in - we still want to use it, so we'll create this temporary duplicate RFile object (it also handily means we don't have to rewind the RFile to the start when we copy it below)
	User::LeaveIfError(temporaryDuplicateOfFile.Duplicate(aInterimFormatFile));
	CFileStore* const fileStore=CFileStore::FromLC(temporaryDuplicateOfFile); // CFileStore::FromLC takes *immediate* ownership of the file it's passed (i.e. before doing anything that can leave)
	if (fileStore->Type()[1]!=KUidInterimFormatFileForJavaMIDletInstaller)
		{
		User::Leave(KErrCorrupt);
		}
	RStoreReadStream rootStream;
	rootStream.OpenLC(*fileStore,fileStore->Root());
	RFs unconnectedFs; // should not actually be used, so we don't need to use a real, connected RFs
	CApaAppInfoFileReader* const appInfoFileReader=new(ELeave) CApaAppInfoFileReader(unconnectedFs);
	CleanupStack::PushL(appInfoFileReader);
	appInfoFileReader->CApaAppInfoFile::ConstructL();
	appInfoFileReader->InternalizeL(rootStream); // will leave if the file is corrupt/badly formed
	CleanupStack::PopAndDestroy(3, fileStore);
	}

EXPORT_C void ForJavaMIDletInstaller::GetJavaMIDletInfoL(RFs& aFs, const TDesC& aFileName,TUint32& aJavaMIDletInfo_AmsAuthId,TUint32& aJavaMIDletInfo_MIDlet)
	{ // static
	CApaAppInfoFileReader* const appInfoFileReader=CApaAppInfoFileReader::NewLC(aFs, aFileName);
	aJavaMIDletInfo_AmsAuthId=appInfoFileReader->iJavaMIDletInfo_AmsAuthId;
	aJavaMIDletInfo_MIDlet=appInfoFileReader->iJavaMIDletInfo_MIDlet;
	CleanupStack::PopAndDestroy(appInfoFileReader);
	}

EXPORT_C CApaAppInfoFileWriter* ForJavaMIDletInstaller::NewInterimFormatFileWriterLC(RFs& aFs,const TDesC& aFileName,TUid aApplicationUid,TUint32 aJavaMIDletInfo_AmsAuthId,TInt aJavaMIDletInfo_MIDlet)
	{ // static
	CApaAppInfoFileWriter* self=new(ELeave) CApaAppInfoFileWriter(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName,aApplicationUid,KUidInterimFormatFileForJavaMIDletInstaller,aJavaMIDletInfo_AmsAuthId,aJavaMIDletInfo_MIDlet);
	return self;
	}

#endif // UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER CApaAppInfoFileWriter* CApaAppInfoFileWriter::NewLC(RFs& aFs, const TDesC& aFileName,TUid aUid)
/** Allocates and constructs an aif file writer. It leaves if the path of the 
specified aif file does not exist.

@param aFs Connected session with the file server.
@param aFileName The full path and (language-neutral) filename of the aif file.
@param aUid The application's third (most derived) UID. 
@return Pointer to the newly constructed and initialised aif file writer. 
The object is left on the cleanup stack. */
	{
	CApaAppInfoFileWriter* self=new(ELeave) CApaAppInfoFileWriter(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName,aUid);
	return self;
	}

void CApaAppInfoFileWriter::ConstructL(const TDesC& aFileName,TUid aUid
#if defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER)
									   ,TUid aMiddleUid/*=KUidAppInfoFile*/,TUint32 aJavaMIDletInfo_AmsAuthId/*=0*/,TInt aJavaMIDletInfo_MIDlet/*=0*/
#endif
									   )
	{
	__DECLARE_NAME(_S("CApaAppInfoFileWriter"));
	CApaAppInfoFile::ConstructL();
	TInt ret=iFs.MkDirAll(aFileName);
    if (ret!=KErrAlreadyExists)
        User::LeaveIfError(ret);
	CFileStore* store=CDirectFileStore::ReplaceL(iFs,aFileName,EFileRead|EFileWrite);
	iStore = store;
#if defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER)
	iJavaMIDletInfo_AmsAuthId=aJavaMIDletInfo_AmsAuthId;
	iJavaMIDletInfo_MIDlet=aJavaMIDletInfo_MIDlet;
#else
	const TUid aMiddleUid=KUidAppInfoFile;
#endif
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid,aMiddleUid,aUid));
	iMap=CStoreMap::NewL(*store);	// Ready for Storing components
	}

void CApaAppInfoFileWriter::ExternalizeL(RWriteStream& aStream) const
	{
	__ASSERT_DEBUG(iCaptionHeaderArray, Panic(EPanicNullPointer));
	aStream<< *iCaptionHeaderArray;
	aStream<< *iIconHeaderArray;
	aStream<< iCapability;
	aStream.WriteInt32L(
#if defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER)
						EAifVersionAddsJavaMIDletInfo
#else
						EAifVersionAddsFileOwnershipInfo
#endif
						);
	aStream<< *iDataTypeHeaderArray;
	aStream<< *iViewDataHeaderArray;
	aStream << *iFileOwnershipInfoHeaderArray;
#if defined(UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER)
	aStream.WriteUint32L(iJavaMIDletInfo_AmsAuthId);
	aStream.WriteInt32L(iJavaMIDletInfo_MIDlet);
#endif
	}

EXPORT_C void CApaAppInfoFileWriter::StoreL()
	{
	__ASSERT_DEBUG(iMap, Panic(EPanicNullPointer));
	__ASSERT_DEBUG(iStore, Panic(EPanicNullPointer));
	RStoreWriteStream outStream(*iMap);
	TStreamId id=outStream.CreateLC(*iStore);
	ExternalizeL(outStream);
	outStream.CommitL();
	CleanupStack::PopAndDestroy(&outStream);
	iMap->Reset();
	iStore->SetRootL(id);
	iStore->CommitL();
	}

EXPORT_C void CApaAppInfoFileWriter::AddCaptionL(TLanguage aLanguage,const TDesC& aCaption)
/** Appends a caption in the specified language to the aif file.

@param aLanguage The language of the specified caption.
@param aCaption The new caption.
@leave KErrBadName The caption's name is longer than KApaMaxAppCaption characters. */

// Write caption out to a new stream and add to iCaptionHeaderArray
	{
	if (aCaption.Length()>KApaMaxAppCaption)
		User::Leave(KErrBadName);
	TCaptionHeader captionHeader;
	captionHeader.iLanguage=aLanguage;
	captionHeader.iCaption=aCaption.AllocL();
	CleanupStack::PushL(captionHeader.iCaption.AsPtr());
	RStoreWriteStream outStream;
	__ASSERT_DEBUG(iStore, Panic(EPanicNullPointer));
	TStreamId id=outStream.CreateLC(*iStore);
	outStream << *captionHeader.iCaption;
	outStream.CommitL();
	CleanupStack::PopAndDestroy(&outStream);
	iMap->BindL(captionHeader.iCaption,id);
	iCaptionHeaderArray->AppendL(captionHeader);
	CleanupStack::Pop(captionHeader.iCaption);
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileWriter::AddIconL(const TDesC& aIconFileName)
/** Loads the icon's bitmap and mask from the specified mbm file 
and writes the icon to the aif file. Takes ownership of the icon.

@param aIconFileName Full path and filename of the icon file (with a .mbm 
extension) containing the icon bitmap at identifier 0 and its mask at 1. */

// Icon file must contain 2 bitmaps: icon and mask, and they are assumed to be the same size.
	{
	CApaMaskedBitmap* icon=CApaMaskedBitmap::NewLC();
	User::LeaveIfError(icon->Load(aIconFileName,0));
	User::LeaveIfError((icon->Mask())->Load(aIconFileName,1));
	CleanupStack::Pop(icon);
	AddIconL(*icon);
	}

EXPORT_C void CApaAppInfoFileWriter::AddIconL(CApaMaskedBitmap& aIcon)
/** Writes the specified pre-loaded icon to the aif file, and takes ownership of it.

@param aIcon The new icon. */

// Guaranteed to take ownership of the aIcon
// Will delete aIcon if it leaves
	{
	CleanupStack::PushL(&aIcon);
	TIconHeader iconHeader;
	iconHeader.iIcon = &aIcon;
	iconHeader.iIconSideInPixels = aIcon.SizeInPixels().iWidth;
	RStoreWriteStream outStream;
	__ASSERT_DEBUG(iStore, Panic(EPanicNullPointer));
	TStreamId id=outStream.CreateLC(*iStore);
	outStream << *iconHeader.iIcon;
	outStream.CommitL();
	CleanupStack::PopAndDestroy(&outStream);
	iMap->BindL(iconHeader.iIcon,id);
	iIconHeaderArray->AppendL(iconHeader);
	CleanupStack::Pop(&aIcon);
	}

EXPORT_C TInt CApaAppInfoFileWriter::SetCapability(const TDesC8& aInfo)
/** Sets the application's capability information. Note that the 
information is not written to the aif file until StoreL() is called.
@param aInfo A TApaAppCapabilityBuf object.
@return KErrNone always. */
	{
	// check that aInfo is of a permissable length
	TApaAppCapabilityBuf buf;
	TApaAppCapability::CopyCapability(buf,aInfo);
	iCapability = buf();
	return KErrNone;
	}

EXPORT_C void CApaAppInfoFileWriter::AddDataTypeL(const TDataTypeWithPriority& aTypePriority)
	{
	__ASSERT_DEBUG(aTypePriority.iDataType.Des8().Length()>0,Panic(EDPanicBadDataType));
	__ASSERT_DEBUG(aTypePriority.iPriority<KDataTypePriorityUserSpecified,Panic(EDPanicBadDataType));
	TDataTypeHeader header;
	TDataType* dataType=new(ELeave) TDataType(aTypePriority.iDataType);
	CleanupStack::PushL(dataType);
	header.iDataType=dataType;
	RStoreWriteStream outStream;
	__ASSERT_DEBUG(iStore, Panic(EPanicNullPointer));
	TStreamId id=outStream.CreateLC(*iStore);
	outStream << *header.iDataType;
	outStream.CommitL();
	CleanupStack::PopAndDestroy(&outStream);
	__ASSERT_DEBUG(iMap, Panic(EPanicNullPointer));
	iMap->BindL(header.iDataType,id);
	header.iPriority=aTypePriority.iPriority;
	iDataTypeHeaderArray->AppendL(header);
	CleanupStack::Pop(dataType);
	}

//
// Adds a view with id aViewId to the data to be stored in the AIF. Icons and captions can be added to the
// view before it is stored - view data is written to an inline store so all the information for the view must
// be added and then the call to store the data made.
//
EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileWriter::AddViewL(TUid aViewId)
// Write view data out to a new stream and add to iViewDataHeaderArray
	{
	AddViewL(aViewId,0);
	}

EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileWriter::AddViewL(TUid aViewId,TInt aScreenMode)
	{
	TViewDataHeader viewDataHeader;
	CApaAIFViewData* viewData=CApaAIFViewData::NewLC();
	viewData->SetViewUid(aViewId);
	viewData->SetScreenMode(aScreenMode);
	viewDataHeader.iViewData=viewData;
	__ASSERT_DEBUG(iViewDataHeaderArray, Panic(EPanicNullPointer));
	iViewDataHeaderArray->AppendL(viewDataHeader);
	CleanupStack::Pop(viewData);
	}

//
// Stores the view data in the AIF, writing the store inline.
//
EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileWriter::StoreViewL(TUid aViewId)
	{
	__ASSERT_DEBUG(iViewDataHeaderArray, Panic(EPanicNullPointer));
	const TInt count=iViewDataHeaderArray->Count();
	for (TInt ii=0;ii<count;ii++)
		{
		const TViewDataHeader& pData=(*iViewDataHeaderArray)[ii];
		if (pData.iViewData.AsPtr()->ViewUid() == aViewId)
			{
			RStoreWriteStream outStream;
			__ASSERT_DEBUG(iStore, Panic(EPanicNullPointer));
			TStreamId id=outStream.CreateLC(*iStore);
			outStream << *pData.iViewData;
			outStream.CommitL();
			CleanupStack::PopAndDestroy(&outStream);
			iMap->BindL(pData.iViewData,id);
			return;
			}
		}
	}

//
// Adds the icon aIcon to the view data with id aViewId which will later be stored in the AIF.
//
EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileWriter::AddViewIconL(CApaMaskedBitmap& aIcon,TUid aViewId)
	{
	CApaAIFViewData* viewData=NULL;
	__ASSERT_DEBUG(iViewDataHeaderArray, Panic(EPanicNullPointer));
	const TInt count=iViewDataHeaderArray->Count();
	for (TInt ii=0;ii<count;ii++)
		{
		const TViewDataHeader& pData=(*iViewDataHeaderArray)[ii];
		if (pData.iViewData.AsPtr()->ViewUid() == aViewId)
			{
			viewData=pData.iViewData.AsPtr();
			}
		}
	if (viewData)
		viewData->AddIconL(aIcon);
	}

//
// Adds the caption aCaption in langauge aLanguage to the view data with id aViewId which will later be stored in the AIF.
//
EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileWriter::AddViewCaptionL(TLanguage aLanguage,const TDesC& aCaption,TUid aViewId)
	{
	CApaAIFViewData* viewData=NULL;
	const TInt count=iViewDataHeaderArray->Count();
	for (TInt ii=0;ii<count;ii++)
		{
		const TViewDataHeader& pData=(*iViewDataHeaderArray)[ii];
		if (pData.iViewData.AsPtr()->ViewUid() == aViewId)
			{
			viewData=pData.iViewData.AsPtr();
			}
		}
	if (viewData)
		viewData->AddCaptionL(aLanguage,aCaption);
	}

//
// Adds the name aOwnedFileName to the list of files described as being owned by this application to later be stored in the AIF.
//
EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void CApaAppInfoFileWriter::AddOwnedFileL(const TDesC& aOwnedFileName)
	{
	if (aOwnedFileName.Length()>KMaxFileName)
		User::Leave(KErrBadName);
	TFileOwnershipInfoHeader fileOwnershipInfoHeader;
	fileOwnershipInfoHeader.iOwnedFileName=aOwnedFileName.AllocL();
	CleanupStack::PushL(fileOwnershipInfoHeader.iOwnedFileName.AsPtr());
	RStoreWriteStream outStream;
	__ASSERT_DEBUG(iStore, Panic(EPanicNullPointer));
	TStreamId id=outStream.CreateLC(*iStore);
	outStream << *fileOwnershipInfoHeader.iOwnedFileName;
	outStream.CommitL();
	CleanupStack::PopAndDestroy(&outStream);
	__ASSERT_DEBUG(iMap, Panic(EPanicNullPointer));
	iMap->BindL(fileOwnershipInfoHeader.iOwnedFileName,id);
	__ASSERT_DEBUG(iFileOwnershipInfoHeaderArray, Panic(EPanicNullPointer));
	iFileOwnershipInfoHeaderArray->AppendL(fileOwnershipInfoHeader);
	CleanupStack::Pop(fileOwnershipInfoHeader.iOwnedFileName);
	}

CApaAppCaptionFileReader::CApaAppCaptionFileReader(RFs& aFs,const TDesC& aAppFileName)
: iFs(aFs)
	{
	__ASSERT_DEBUG((aAppFileName.Length()+KApparcExtraLengthOfCaptionFileName<KMaxFileName),Panic(EFilenameTooLong));
	TParse appFileNameParse;
	appFileNameParse.Set(aAppFileName,NULL,NULL);
	iCaptionFileName=appFileNameParse.DriveAndPath(); //extract path name 
	iCaptionFileName.Append(appFileNameParse.Name()); //add app name without extension
	iCaptionFileName.Append(KApaCaptionFileSuffix); //add suffix
	iCaptionFileName.Append(KAppResourceFileExtension); //add extension
	BaflUtils::NearestLanguageFile(aFs,iCaptionFileName);
	}

void CApaAppCaptionFileReader::GetCaptionsL(TApaAppCaption& aCaption,TApaAppCaption& aShortCaption)
//reads the caption and shortcaption of the current language from the appropriate caption file
	{	
	CResourceFile* file = CResourceFile::NewL(iFs, iCaptionFileName, 0, 0);
	CleanupStack::PushL(file);
	RResourceReader reader;
	reader.OpenL(file, 1);
	CleanupClosePushL(reader);
	aCaption = reader.ReadTPtrCL();
	aShortCaption = reader.ReadTPtrCL();
	CleanupStack::PopAndDestroy(&reader);
	CleanupStack::PopAndDestroy(file);

	if ((!aShortCaption.Length())&&(aCaption.Length()))
		aShortCaption=aCaption;
	else if ((!aCaption.Length())&&(aShortCaption.Length()))
		aCaption=aShortCaption;	
	}

//
// Sets the drive of aAifName to the current locale drive.
// Assumes that aAifName is already a valid full filename.
//
EXPORT_C_NOT_NEEDED_FOR_JAVA_MIDLET_INSTALLER void AppInfoFileUtils::GetAifFileName(const RFs& aFs,TDes& aAifName)
	{ // static
	TFileName aifName(aAifName);
	BaflUtils::NearestLanguageFile(aFs,aifName);
	aAifName=aifName;
	}

//
// Sets aAifName to the full path of the relevant .AIF file.
// Note that this may be on a different drive to the .APP file if the system drive has been
// set to point to a new locale that has been downloaded.
//
void AppInfoFileUtils::GetAifFileNameL(const RFs& aFs,const TDesC& aFullName,TDes& aAifName)
	{ // static
	TParse parse;
	User::LeaveIfError(parse.Set(KAppInfoFileExtension,&aFullName,NULL));
	aAifName=parse.FullName();
	AppInfoFileUtils::GetAifFileName(aFs,aAifName);
	}
	
#endif // UI_FRAMEWORKS_V1_REMNANT_FOR_JAVA_MIDLET_INSTALLER
