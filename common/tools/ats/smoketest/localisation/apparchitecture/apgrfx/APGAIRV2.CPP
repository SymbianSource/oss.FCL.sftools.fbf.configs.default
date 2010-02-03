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

#include "APGAIR.H"
#include <apgaplst.h>
#include <bautils.h>
#include "APGICNFL.H" 
#include "APFDEF.H"
#include "apprivate.h"
#include <barsc2.h>
#include <barsread2.h>
#include <e32uid.h>
#include "APGSTD.H"

#include "apsecutils.h"

const TUint KResourceOffsetMask = 0xFFFFF000;


_LIT(KAppBinaryPathAndExtension,"\\sys\\bin\\.exe");
_LIT(KV1AppPathAndExtension, "\\system\\apps\\.app");
const TInt KAppRegistrationInfoResourceId = 1;

#ifndef SYMBIAN_APPARC_APPINFO_CACHE
CApaAppInfoReaderV2* CApaAppInfoReaderV2::NewL(RFs& aFs, const TDesC& aRegistrationFileName, const TUid aAppUid,const TDesC& aDefaultAppIconFileName)
	{
	CApaAppInfoReaderV2* self = new(ELeave) CApaAppInfoReaderV2(aFs, aRegistrationFileName, aAppUid,aDefaultAppIconFileName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CApaAppInfoReaderV2::CApaAppInfoReaderV2(RFs& aFs, const TDesC& aRegistrationFileName, const TUid aAppUid,const TDesC& aDefaultAppIconFileName)
	: CApaAppInfoReader(aFs, aAppUid),
	iRegistrationFileName(aRegistrationFileName),iDefaultAppIconFileName(aDefaultAppIconFileName)
	{
	}
#else
CApaAppInfoReaderV2* CApaAppInfoReaderV2::NewL(RFs& aFs, const TDesC& aRegistrationFileName, const TUid aAppUid)
	{
	CApaAppInfoReaderV2* self = new(ELeave) CApaAppInfoReaderV2(aFs, aRegistrationFileName, aAppUid);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CApaAppInfoReaderV2::CApaAppInfoReaderV2(RFs& aFs, const TDesC& aRegistrationFileName, const TUid aAppUid)
	: CApaAppInfoReader(aFs, aAppUid),
	iRegistrationFileName(aRegistrationFileName)
	{
	}
#endif // SYMBIAN_APPARC_APPINFO_CACHE

void CApaAppInfoReaderV2::ConstructL()
	{
	iIconLoader = CApaIconLoader::NewL(iFs);
	}

CApaAppInfoReaderV2::~CApaAppInfoReaderV2()
	{
	delete iIconLoader;
	}

#ifdef SYMBIAN_APPARC_APPINFO_CACHE
// The behaviour of the following function is a little non-standard, as it
// transfers ownership of the pointer owned by a CApaAppInfoReaderV2 object
// to the caller. This means that this function is only designed to be called once.
// Doing things this way provides a small performance optimisation by enabling the caller
// to delete it's stored pointer, and replace it with one returned by this function,
// instead of having to copy the object (copying could be expensive for the methods
// of this class that need to return arrays).
CApaIconLoader* CApaAppInfoReaderV2::IconLoader()
	{
	CApaIconLoader* iconLoader = iIconLoader;
	iIconLoader = NULL; // ownership transferred to caller
	return iconLoader;
	}
#endif // SYMBIAN_APPARC_APPINFO_CACHE

// this method reads the minimum information required to register an app
// if this fails (Leaves), we say the read has been unsuccessful
void CApaAppInfoReaderV2::ReadMandatoryInfoL(CResourceFile*& aRegistrationFile, RResourceReader& aResourceReader)
	{
	TEntry entry;
	User::LeaveIfError(iFs.Entry(iRegistrationFileName, entry));
	iTimeStamp = entry.iModified;
	
	TUint fileOffset = 0;
	TInt fileLength = 0;
	TUid middleUid(KUidApp);

	// in the case of a non-native application, the resource file
	// has been padded with the application type uid
	
	if (TParsePtrC(iRegistrationFileName).Path().CompareF(KLitPathForNonNativeResourceAndIconFiles)==0)
		{
		fileOffset=sizeof(TCheckedUid);
		fileLength=entry.iSize-fileOffset;
		middleUid=entry[1];
		if (middleUid.iUid==KNullUid.iUid)
			{
			User::Leave(KErrCorrupt);
			}
		}

	aRegistrationFile = CResourceFile::NewL(iFs, iRegistrationFileName, fileOffset, fileLength);
	aResourceReader.OpenL(aRegistrationFile, KAppRegistrationInfoResourceId);

	aResourceReader.ReadUint32L(); // skip over LONG reserved_long
	aResourceReader.ReadUint32L(); // skip over LLINK reserved_llink

	// read LTEXT app_file
	const TPtrC appFile(aResourceReader.ReadTPtrCL());
	TParse parse; // this object gets used for 2 purposes: first to check that a TParsePtrC can be created over "appFile" without it panicking, and second to construct iAppBinaryFullName
	User::LeaveIfError(parse.SetNoWild(appFile, NULL, NULL)); // do this before creating a TParsePtrC, since TParsePtrC's constructor panics if it fails (which would provide an easy way for malware to kill the Apparc server)
	const TParsePtrC appFileParser(appFile);

	// read LONG attributes
	iCapability.iAttributes = aResourceReader.ReadUint32L();

	if (!appFileParser.NamePresent())
		{
		User::Leave(KErrCorrupt);
		}
	const TPtrC appNameWithoutExtension(appFileParser.Name());
	const TPtrC registrationFileDrive(TParsePtrC(iRegistrationFileName).Drive());
	TUid firstUid(TUid::Null());
	if (iCapability.iAttributes & TApaAppCapability::ENonNative)
		{
		if (!appFileParser.PathPresent() || !appFileParser.ExtPresent())
			{
			User::Leave(KErrCorrupt);
			}
		const TPtrC appFilePath(appFileParser.Path());
		const TPtrC appFileNameAndExt(appFileParser.NameAndExt());
		TPtrC appFileDrive(registrationFileDrive);
		if (appFileParser.DrivePresent())
			{
			appFileDrive.Set(appFileParser.Drive());
			}
		User::LeaveIfError(parse.SetNoWild(appFileDrive, &appFilePath, &appFileNameAndExt));
		// keep firstUid as TUid::Null()
		}
	else if (iCapability.iAttributes & TApaAppCapability::EBuiltAsDll)
		{
		// legacy dll-style app
		User::LeaveIfError(parse.SetNoWild(registrationFileDrive, &KV1AppPathAndExtension, &appNameWithoutExtension));
		User::LeaveIfError(parse.AddDir(appNameWithoutExtension));
		firstUid = KDynamicLibraryUid;
		}
	else
		{
		// exe-style app
		User::LeaveIfError(parse.SetNoWild(registrationFileDrive, &KAppBinaryPathAndExtension, &appNameWithoutExtension));
		firstUid = KExecutableImageUid;
		}
	iAppBinaryFullName = parse.FullName().AllocL();

	// set the TUidType for the app binary
	// cannot read the TEntry info from the app binary because it's in \sys\bin
	iAppBinaryUidType = TUidType(firstUid, middleUid, iAppUid);
	}

// reads as much info as it can
// at least captions and icons must be setup on return from this method (using defaults if necessary)
TBool CApaAppInfoReaderV2::Read()
	{
	CResourceFile* registrationFile = NULL;
	RResourceReader resourceReader;
	TRAPD(ret, ReadMandatoryInfoL(registrationFile, resourceReader));
	if (ret != KErrNone)
		{
		resourceReader.Close();
		delete registrationFile;
		return EFalse; // might have read something, but failed to setup enough info to make it worthwhile trying to read any more
		}

	CResourceFile* localisableFile = NULL;
	TUint localisableResourceId = 1; // only initialising this here to keep the compiler happy, as it's concerned that the variable might be used without having been initialised. The variable should be initialised later, before it's used
	TRAP(ret, ReadNonLocalisableInfoL(resourceReader, localisableFile, localisableResourceId));

	if (ret == KErrNone)
		{
		TRAP(ret, ReadNonLocalisableOptionalInfoL(resourceReader, registrationFile, localisableFile));
		}

	TBool useDefaultIcons=ETrue;

	if (ret == KErrNone && localisableFile)
		{
		TRAP(ret, ReadLocalisableInfoL(localisableFile, localisableResourceId, useDefaultIcons));
		}
	delete localisableFile;
	resourceReader.Close();
	delete registrationFile;

	// if anything went wrong, we tell the caller that the read was unsuccessful. Some
	// of the members of this class may not contain complete data, but this doesn't matter
	// because the caller shouldn't try to access the data if the read was unsuccessful
	TBool readSuccessful = (ret == KErrNone);

	if (useDefaultIcons)
		{
		delete iIcons;
		iIcons = NULL;
		TRAP_IGNORE(iIcons = CApaAppIconArray::NewDefaultIconsL());
		}

	return readSuccessful;
	}

HBufC* CApaAppInfoReaderV2::FullIconFileNameL(const TDesC& aIconFileName) const
	{
	HBufC* filename = NULL;
	if (aIconFileName.Length() == 0)
		{
		return NULL;
		}
	/*
	 * aIconFileName may contain a valid string in some format (for eg. URI format) other than path to a regular file on disk
	 * and that can be a mbm or non-mbm file. Such a filename will be reported as invalid filename by iFs.IsValidName() method. 
	 * aIconFileName will be returned since it is a valid string. 
	 */	
	if(!iFs.IsValidName(aIconFileName))
		{
		filename = aIconFileName.AllocL();
		return filename;
		}
	
	TParsePtrC parsePtr(aIconFileName);
	if (parsePtr.IsWild()
		|| !parsePtr.PathPresent()
		|| !parsePtr.NamePresent())
		{
		return NULL;
		}
	// check for fully qualified icon filename
	if (parsePtr.DrivePresent() && BaflUtils::FileExists(iFs, aIconFileName))
		{
		filename = aIconFileName.AllocL();
		}
	else
		{
		// check for icon file on same drive as localisable resource file
		TParse parse;
		TPtrC localisableResourceFileDrive = TParsePtrC(*iLocalisableResourceFileName).Drive();
		TInt ret = parse.SetNoWild(localisableResourceFileDrive, &aIconFileName, NULL);
		if (ret == KErrNone && BaflUtils::FileExists(iFs, parse.FullName()))
			{
			filename = parse.FullName().AllocL();
			}
		else
			{
			TPtrC registrationFileDrive = TParsePtrC(iRegistrationFileName).Drive();
			if (TInt(TDriveUnit(registrationFileDrive)) != TInt(TDriveUnit(localisableResourceFileDrive)))
				{
				// check for icon file on same drive as registration file
				ret = parse.SetNoWild(registrationFileDrive, &aIconFileName, NULL);
				if (ret == KErrNone && BaflUtils::FileExists(iFs, parse.FullName()))
					{
					filename = parse.FullName().AllocL();
					}
				}
			}
		}
	return filename;
	}

void CApaAppInfoReaderV2::ReadLocalisableInfoL(const CResourceFile* aResourceFile, TUint aResourceId, TBool& aUseDefaultIcons)
	{
	RResourceReader resourceReader;
	resourceReader.OpenLC(aResourceFile, aResourceId);

	resourceReader.ReadUint32L(); // skip over LONG reserved_long
	resourceReader.ReadUint32L(); // skip over LLINK reserved_llink

	// read LTEXT short_caption
	iShortCaption = resourceReader.ReadHBufCL();

	resourceReader.ReadUint32L(); // skip over LONG reserved_long
	resourceReader.ReadUint32L(); // skip over LLINK reserved_llink

	// read LTEXT caption
	iCaption = resourceReader.ReadHBufCL();

	// read WORD number_of_icons
	const TInt numOfIcons = resourceReader.ReadInt16L();
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	iNumOfAppIcons = numOfIcons;
#endif // SYMBIAN_APPARC_APPINFO_CACHE

	// read LTEXT icon_file
	TPtrC iconFile = resourceReader.ReadTPtrCL();
	
	iIconFileName = FullIconFileNameL(iconFile);
	if (iIconFileName)
		{
		TEntry entry;
		TInt ret = iFs.Entry(*iIconFileName, entry);
		if (KErrNone == ret)
			{
			iIconFileTimeStamp = entry.iModified;
			}
			
		aUseDefaultIcons = EFalse;
		if (iFs.IsValidName(*iIconFileName))
			{
			if(FileIsMbmWithGenericExtensionL(*iIconFileName))
				{
				if (numOfIcons > 0)
					{
					CApaAppIconArray* iconArray = CApaAppIconArray::NewAppIconsL(numOfIcons, *iIconFileName, *iIconLoader);
					delete iIcons;
					iIcons = iconArray;
					}
				else
					{
					aUseDefaultIcons = ETrue;		
					}
				}
			else
				{
				iNonMbmIconFile = ETrue;
				}
			}
		//If the filename is not a valid name then the file is treated as a non-mbm file.
		else
			{
			iNonMbmIconFile = ETrue;
			}
		}
	// read LEN WORD STRUCT view_list[]
	const TInt numOfViews = resourceReader.ReadInt16L();
	if (numOfViews > 0)
		{
		iViewDataArray = new(ELeave) CArrayPtrFlat<CApaAppViewData>(1);
		}
	for(TInt view = 0; view < numOfViews; ++view)
		{
		CApaAppViewData* viewData=CApaAppViewData::NewLC();
		resourceReader.ReadUint32L(); // skip over LONG reserved_long
		resourceReader.ReadUint32L(); // skip over LLINK reserved_llink

		// read LONG uid
		const TUid viewUid = {resourceReader.ReadInt32L()};
		viewData->SetUid(viewUid);
		// read LONG screen_mode
		const TInt screenMode = {resourceReader.ReadInt32L()};
		viewData->SetScreenMode(screenMode);

		resourceReader.ReadUint32L(); // skip over LONG reserved_long
		resourceReader.ReadUint32L(); // skip over LLINK reserved_llink

		// read LTEXT caption
		TPtrC viewCaption = resourceReader.ReadTPtrCL();
		viewData->SetCaptionL(viewCaption);
		// read WORD number_of_icons
		const TInt numOfViewIcons = resourceReader.ReadInt16L();
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
		viewData->SetNumOfViewIcons(numOfViewIcons);
#endif // SYMBIAN_APPARC_APPINFO_CACHE
		// read LTEXT icon_file
		TPtrC viewIconFile = resourceReader.ReadTPtrCL();
		HBufC* const fullViewIconFileName = FullIconFileNameL(viewIconFile);
		if (fullViewIconFileName)
			{
			CleanupStack::PushL(fullViewIconFileName);
			viewIconFile.Set(*fullViewIconFileName);
			viewData->SetIconFileNameL(viewIconFile);
			
			if (iFs.IsValidName(viewIconFile))
				{
				if(!FileIsMbmWithGenericExtensionL(viewIconFile))
					{
					viewData->SetNonMbmIconFile(ETrue);
					}
				}
			//If the filename is not a valid name then the file is treated as a non-mbm file.
			else
				{
				viewData->SetNonMbmIconFile(ETrue);
				}
			}
		else
			{
			viewIconFile.Set(KNullDesC);
			if (numOfViewIcons > 0 && iIconFileName)
				{
				viewIconFile.Set(*iIconFileName); // default to app icon filename
				}
			}
		if (numOfViewIcons > 0 && iFs.IsValidName(viewIconFile) && (FileIsMbmWithGenericExtensionL(viewIconFile)))
			{
			CApaAppIconArray* iconArray = CApaAppIconArray::NewViewIconsL(numOfViewIcons, viewIconFile, *iIconLoader);
			viewData->SetIconArray(iconArray);
			iconArray = NULL;
			}
		if (fullViewIconFileName)
			{
			CleanupStack::PopAndDestroy(fullViewIconFileName);
			}
		iViewDataArray->AppendL(viewData);
		CleanupStack::Pop(viewData);
		}
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
	iIconLoader->LoadAllIconsL();
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	// Read LTEXT group_name
	// If a localised group name has been specified, it overrides
	// The group name (if any), specified by APP_REGISTRATION_INFO

	TApaAppGroupName groupName;
	TRAPD(ret, (groupName = resourceReader.ReadTPtrCL()));
	if (ret != KErrNone)
		{
		 if (ret != KErrEof)
			 {
	         User::Leave(ret);
			 }
		}
	else
		{
	     if (groupName.Length() > 0)
			 {
			 iCapability.iGroupName = groupName;
			 }
		}

	CleanupStack::PopAndDestroy(&resourceReader);
	}


TBool CApaAppInfoReaderV2::FileIsMbm(const TDesC& aFileName)
	{ // static
	if (aFileName.Length() > 0)
		{
		TParsePtrC parsePtr(aFileName);
		_LIT(KFileExtensionMBM, ".MBM");
		if (parsePtr.Ext().CompareF(KFileExtensionMBM) == 0)
			{
			return ETrue;
			}
		}
	return EFalse;
	}




/*An MBM file may have a generic icon extension. In this case, as a way to check whether the file is an MBM one, 
it is necessary to read the content of the fist four 32bit words of it and find out whether these words correspond to 
KWriteonceFileStoreUid, KMultiBitmapFileImageUid, zero and KMultiBitmapFileImageChecksum respectively (defined in graphics/gditools/bmconv/bmconv.h).
So the file is opened and the first 4 32 bit words are extracted and compared with the header information of standard MBM file.
If they match, the function returns ETrue, else it returns EFalse */
TBool CApaAppInfoReaderV2::FileIsMbmWithGenericExtensionL(const TDesC& aFileName) const 
      { 
      if (aFileName.Length() > 0) 
            { 
            //open a file in Share mode - this will allow other methods to access it too
            RFile file;
            User::LeaveIfError(file.Open(iFs,aFileName,EFileShareReadersOnly));
            //this is done beacuse the file can also be accessed by applist at the same time
            //buffer stores the 16 bytes of the file
            CleanupClosePushL(file);
            TBuf8<16> buffer;
            User::LeaveIfError(file.Read(buffer,16));
            CleanupStack::PopAndDestroy();//file
            //we use a constant pointer to the buffer to read header info
        	TPtrC8 filePointer(buffer);
        	
            /*The first 16 bytes of an MBM file are the same for any generic MBM file.
            These are :
            KWriteOnceFileStoreUid = 0x10000037(Emulator MBM file) 0x10000041(ROM image)	
            KMultiBitMapFileImageUid = 0x10000042(Emulator MBM file) 	0x00000001(ROM image)
            Zero = 0x00000000(Emulator MBM file) 0x0000000C(ROM image)
            checksum = 0x47396439(Emulator MBM file) 0x10000040(ROM image)
            The first 16 bytes of the given file is compared with these standard values to ascertain it is MBM file*/
        	if((filePointer[3]==0x10)&&(filePointer[2]==0x00)&&(filePointer[1]==0x00)&&(filePointer[0]==0x37))
        		{//KWriteOnceFileStoreUid = 0x10000037
        		if((filePointer[7]==0x10)&&(filePointer[6]==0x00)&&(filePointer[5]==0x00)&&(filePointer[4]==0x42))
        			{//KMultiBitMapFileImageUid = 0x10000042
        			if((filePointer[11]==0x00)&&(filePointer[10]==0x00)&&(filePointer[9]==0x00)&&(filePointer[8]==0x00))
        				{//Zero = 0x00000000)
        				if((filePointer[15]==0x47)&&(filePointer[14]==0x39)&&(filePointer[13]==0x64)&&(filePointer[12]==0x39))
        					{//checksum = 0x47396439
        					return ETrue;
        					}
        				}
        			}
        		}
        	//Else Check for ROM Image MBM file's header
        	else if((filePointer[3]==0x10)&&(filePointer[2]==0x00)&&(filePointer[1]==0x00)&&(filePointer[0]==0x41))
        		{//KWriteOnceFileStoreUid = 0x10000041
        		if((filePointer[7]==0x00)&&(filePointer[6]==0x00)&&(filePointer[5]==0x00)&&(filePointer[4]==0x01))
        			{//KMultiBitMapFileImageUid = 0x00000001
        			if((filePointer[11]==0x00)&&(filePointer[10]==0x00)&&(filePointer[9]==0x00)&&(filePointer[8]==0x0C))
        				{//Zero = 0x0000000C)
        				if((filePointer[15]==0x10)&&(filePointer[14]==0x00)&&(filePointer[13]==0x00)&&(filePointer[12]==0x40))
        					{//checksum = 0x10000040
        					return ETrue;
        					}
        				}
        			}
        		}
        	}
      return EFalse;
      }

HBufC8* CApaAppInfoReaderV2::ReadOpaqueDataL(TUint aResourceId, const CResourceFile* aRegistrationFile, CResourceFile* aLocalisableResourceFile)
	{ // static
	HBufC8* opaqueData = NULL;
	if (aResourceId == 0)
		{
		opaqueData = HBufC8::NewL(0);
		}
	else
		{
		if (aResourceId & KResourceOffsetMask)
			{
			// expecting opaque data to be in the localisable resource file
			if (aLocalisableResourceFile)
				{
				aLocalisableResourceFile->ConfirmSignatureL();
				opaqueData = aLocalisableResourceFile->AllocReadLC(aResourceId);
				CleanupStack::Pop(opaqueData);
				}
			else
				{
				opaqueData = HBufC8::NewL(0);
				}
			}
		else
			{
			// expecting opaque data to be in the registration file
			__ASSERT_ALWAYS(aRegistrationFile, Panic(EPanicNullPointer));
			opaqueData = aRegistrationFile->AllocReadLC(aResourceId); //lint !e613 Suppress ossible use of null pointer
			CleanupStack::Pop(opaqueData);
			}
		}
	return opaqueData;
	}

void CApaAppInfoReaderV2::ReadNonLocalisableOptionalInfoL(RResourceReader& aResourceReader, const CResourceFile* aRegistrationFile, CResourceFile* aLocalisableResourceFile)
	{
	// read LEN WORD STRUCT service_list[]
	TInt serviceCount = 0;
	// service information was not present in the first release of the registration file
	// APP_REGISTRATION_INFO resource struct
	// this method must not leave if the registration file doesn't contain service information, so the
	// following call to ReadInt16L is trapped to ensure this method doesn't leave just because
	// there is no more information in the resource to read (KErrEof)
	TRAPD(err, serviceCount = aResourceReader.ReadInt16L());
	if (err != KErrNone)
		{
		if (err != KErrEof)
			{
			User::Leave(err);
			}
		return; // end of resource reached
		}
	else
		{
		if ((!iServiceArray) && (serviceCount > 0))
			{
			iServiceArray = new(ELeave) CArrayFixFlat<TApaAppServiceInfo>(4);
			}
		while (serviceCount--)
			{
			const TUid serviceUid = {aResourceReader.ReadUint32L()};
			
			if ((serviceUid == KOpenServiceUid) && (iOpenServiceIsLegacy))
				{
				__ASSERT_DEBUG(iIndexOfFirstOpenService == 0, User::Invariant());
				// If we found an Open service in the SERVICE_INFO declaration
				// then we must ignore the legacy one
				(*iServiceArray)[0].Release();
				iServiceArray->Delete(0);
				iOpenServiceIsLegacy = EFalse;
				iIndexOfFirstOpenService = -1;
				}
			
			CArrayFixFlat<TDataTypeWithPriority>* datatypes = new(ELeave) CArrayFixFlat<TDataTypeWithPriority>(5);
			CleanupStack::PushL(datatypes);
			ReadDataTypesL(aResourceReader, *datatypes);
			
			const TUint resourceId = aResourceReader.ReadUint32L();
			HBufC8* opaqueData = NULL;
			opaqueData = ReadOpaqueDataL(resourceId, aRegistrationFile, aLocalisableResourceFile);
			CleanupStack::PushL(opaqueData);
			
			TApaAppServiceInfo serviceInfo(serviceUid, datatypes, opaqueData); // takes ownership of opaqueData
			iServiceArray->AppendL(serviceInfo);
			CleanupStack::Pop(opaqueData);
			CleanupStack::Pop(datatypes);
			if ((serviceUid == KOpenServiceUid) && (iIndexOfFirstOpenService < 0))
				{
				iIndexOfFirstOpenService = iServiceArray->Count() - 1;
				}
			}
		// read LLINK opaque_data
		const TUint resourceId = aResourceReader.ReadUint32L();
		delete iOpaqueData;
		iOpaqueData = NULL;
		iOpaqueData = ReadOpaqueDataL(resourceId, aRegistrationFile, aLocalisableResourceFile);
		}
	}

void CApaAppInfoReaderV2::ReadNonLocalisableInfoL(RResourceReader& aResourceReader, CResourceFile*& aLocalisableResourceFile, TUint& aLocalisableResourceId)
	{
	__ASSERT_DEBUG(aLocalisableResourceFile == NULL, User::Invariant());

	// read LTEXT localisable_resource_file
	TPtrC localisableResourceFileName(aResourceReader.ReadTPtrCL());
	if (localisableResourceFileName.Length() > 0 && iFs.IsValidName(localisableResourceFileName))
		{
		// determine the language specific name of the localisable resource file
		TParse parse;
		TParsePtrC parsePtr(iRegistrationFileName);
		User::LeaveIfError(parse.SetNoWild(parsePtr.Drive(), &KAppResourceFileExtension, &localisableResourceFileName));
		TFileName resourceFileName(parse.FullName());
		BaflUtils::NearestLanguageFile(iFs, resourceFileName, iApplicationLanguage);
		iLocalisableResourceFileName = resourceFileName.AllocL();

		TEntry entry;
		User::LeaveIfError(iFs.Entry(*iLocalisableResourceFileName, entry));
		iLocalisableResourceFileTimeStamp = entry.iModified;
	

		// open the localisable resource file	
		aLocalisableResourceFile = CResourceFile::NewL(iFs, resourceFileName, 0, 0);
		}

	// read LONG localisable_resource_id
	aLocalisableResourceId = aResourceReader.ReadUint32L();
	if (aLocalisableResourceFile && (aLocalisableResourceId & KResourceOffsetMask))
		{
		aLocalisableResourceFile->ConfirmSignatureL();
		}

	iCapability.iAppIsHidden=aResourceReader.ReadInt8L();
	iCapability.iEmbeddability = static_cast<TApaAppCapability::TEmbeddability>(aResourceReader.ReadInt8L());
	iCapability.iSupportsNewFile=aResourceReader.ReadInt8L();
	iCapability.iLaunchInBackground = aResourceReader.ReadInt8L();
	iCapability.iGroupName = aResourceReader.ReadTPtrCL();

	// read BYTE default_screen_number
	iDefaultScreenNumber = aResourceReader.ReadUint8L();
 	
	//read the datatypes
	CArrayFixFlat<TDataTypeWithPriority>* datatypes = new(ELeave) CArrayFixFlat<TDataTypeWithPriority>(5);
	CleanupStack::PushL(datatypes);
	ReadDataTypesL(aResourceReader, *datatypes);
	//dataTypes is deleted if 
	// A. There are no legacy datatypes
	// B. Control panel plugin apps are not allowed to register MIME types.If they happen to have any, these datatypes should be ignored.
	if ((iCapability.iAttributes & TApaAppCapability::EControlPanelItem) || (datatypes->Count() == 0))
		{
		CleanupStack::PopAndDestroy(datatypes);
		}
	else
		{
		__ASSERT_DEBUG(!iServiceArray, User::Invariant());
		iServiceArray = new(ELeave) CArrayFixFlat<TApaAppServiceInfo>(4);
		HBufC8* opaqueData = HBufC8::NewL(0);
		CleanupStack::PushL(opaqueData);
		TApaAppServiceInfo serviceInfo(KOpenServiceUid, datatypes, opaqueData);
		iServiceArray->AppendL(serviceInfo);
		CleanupStack::Pop(opaqueData);
		CleanupStack::Pop(datatypes);
		iIndexOfFirstOpenService = 0;
		iOpenServiceIsLegacy = ETrue;
		}

	// read LEN WORD STRUCT file_ownership_list[]
	const TInt fileOwnershipArraySize = aResourceReader.ReadInt16L();
	if (fileOwnershipArraySize > 0)
		{
		iOwnedFileArray = new(ELeave) CDesCArraySeg(1);
		}
	for (TInt i=0; i < fileOwnershipArraySize; i++)
		{
		TPtrC fileNamePtr = aResourceReader.ReadTPtrCL();
		iOwnedFileArray->AppendL(fileNamePtr);
		}
	}
	
void CApaAppInfoReaderV2::ReadDataTypesL(RResourceReader& aResourceReader,
	CArrayFixFlat<TDataTypeWithPriority>& aDatatypes)
	{
	// read LEN WORD STRUCT datatype_list[]
	const TInt dataTypeArraySize = aResourceReader.ReadInt16L();
	if (dataTypeArraySize <= 0)
		{
		return;
		}
	
	for (TInt i=0; i < dataTypeArraySize; i++)
		{
		TDataTypePriority priority = static_cast<TDataTypePriority>(aResourceReader.ReadInt32L());
        
		//Check for data priority of UnTrusted apps however the trusted apps will not have any restrictions 
		//over the data priority.	
		//If an untrusted app has write device data capability (i.e. still has priority = KDataTypePrioritySystem),
		//do not restrict to KDataTypeUnTrustedPriorityThreshold
		if (priority > KDataTypeUnTrustedPriorityThreshold || priority == KDataTypePrioritySystem )
		    {
		    ReadAppSecurityInfo();

            if (priority == KDataTypePrioritySystem)
                {
                // Check that the app has capability WriteDeviceData
                if (!iHasWriteDeviceDataCap)
                    {
                    priority = KDataTypePriorityNormal;
                    }
                }
            else
                {
                //data priority for UnTrusted apps would be capped if it is greater than the threshold priority i.e, KMaxTInt16.
                TInt match=iRegistrationFileName.MatchF(KLitPathForUntrustedRegistrationResourceFiles);
                if (match != KErrNotFound && !iIsSidTrusted) 
                    {
                    //if registration file is in import directory and 
                    //its sid is in unprotected range - downgrade the priority
                    priority = KDataTypeUnTrustedPriorityThreshold;	
                    }
                }
		    }
		TPtrC8 dataTypePtr = aResourceReader.ReadTPtrC8L();
		TDataType dataType(dataTypePtr);
		TDataTypeWithPriority dataTypeWithPriority(dataType, priority);
		aDatatypes.AppendL(dataTypeWithPriority);
		}
	}

// This method can be used to check whether app has a WriteDeviceCap 
// and its sid is trusted
void CApaAppInfoReaderV2::ReadAppSecurityInfo()
    {
    if (!iSecurityInfoHasBeenRead)
        {
        __ASSERT_DEBUG( iAppBinaryFullName, Panic(EPanicNullPointer) );
        const TInt err = CApaSecurityUtils::CheckAppSecurity( *iAppBinaryFullName, 
                                        iHasWriteDeviceDataCap,
                                        iIsSidTrusted);
        if ( KErrNone != err )
            {
            iHasWriteDeviceDataCap = EFalse;
            iIsSidTrusted = EFalse;
            }
        iSecurityInfoHasBeenRead = ETrue;   
        }
    }

//
// Class CApaIconLoader
//

CApaIconLoader* CApaIconLoader::NewL(RFs& aFs)
	{
	CApaIconLoader* self = NewLC(aFs);
	CleanupStack::Pop(self);
	return self;
	}

CApaIconLoader* CApaIconLoader::NewLC(RFs& aFs)
	{
	CApaIconLoader* self = new(ELeave) CApaIconLoader(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CApaIconLoader::CApaIconLoader(RFs& aFs) :
		iFs(aFs)
	{
	}

void CApaIconLoader::ConstructL()
	{
	} //lint !e1762 Suppress member function could be made const


CApaIconLoader::~CApaIconLoader()
	{
	TInt arrayCount = iIconIndexArray.Count();
	while (--arrayCount >= 0)
		{
		delete (iIconIndexArray[arrayCount].iFileName);
		}
	iIconIndexArray.Close();
	iIconArrays.Close();
	}

void CApaIconLoader::AddIconArrayL(const CApaAppIconArray& aIcons)
	{
	iIconArrays.AppendL(&aIcons);
	}

void CApaIconLoader::LoadAllIconsL()
	{
	const TInt iconArraysCount = iIconArrays.Count();
	CleanupClosePushL(iIconArrays);
	for (TInt ii = 0; ii <iconArraysCount; ++ii)
		{
		iIconArrays[ii]->LoadIconsL();
		}
	CleanupStack::PopAndDestroy(&iIconArrays);
	}

// given an mbm filename, returns the next index to read from the file
// always set aUseCache to EFalse on first call in a sequence of calls
TInt CApaIconLoader::IconIndexL(const TDesC& aFileName, TBool& aUseCache)
	{
	if (aUseCache)
		{
		TInt ret = iIconIndexArray[iCachedArrayIndex].iIndex++;
		return ret;
		}
	else
		{
		aUseCache = ETrue;
		}

	// if filename in array, get the next index
	TInt ret = 0;
	const TInt arrayCount = iIconIndexArray.Count();
	TInt arrayIndex;
	for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
		{
		__ASSERT_DEBUG(iIconIndexArray[arrayIndex].iFileName, Panic(EDPanicInvalidIconIndexArray));
		if (iIconIndexArray[arrayIndex].iFileName->CompareF(aFileName) == 0)
			{
			ret = iIconIndexArray[arrayIndex].iIndex++;
			iCachedArrayIndex = arrayIndex;
			break;
			}
		}

	if (arrayIndex >= arrayCount)
		{
		// filename not found, add it to array
		TKeyValuePair keyValuePair;
		keyValuePair.iFileName = aFileName.AllocL();
		keyValuePair.iIndex = 0;
		ret = keyValuePair.iIndex++;
		User::LeaveIfError(iIconIndexArray.Append(keyValuePair));
		iCachedArrayIndex = arrayCount;
		}
	return ret;
	}
	


// returns EFalse if there was an error obtaining aMbmFileName's entry information,
// otherwise returns ETrue.
// Leaves if an error occurs while trying to populate aIcons or sort icons
TBool CApaIconLoader::LoadIconsL(TInt aNumOfIcons, const TDesC& aMbmFileName, CArrayPtr<CApaMaskedBitmap>& aIcons)
	{
	TEntry entry;
	TInt error=iFs.Entry(aMbmFileName,entry);
	if (error!=KErrNone)
		{
		return EFalse;
		}
	TInt iconIndex;
	TInt fileIndex;
	TBool useCache = EFalse;

	
	// create link to CApaAppList which owns this object as it has a separate share protected RFs
	// which can be used to load bitmap icons 
	CApaAppList* const appList = CApaAppList::Self();
	
	RFile mbmFile;
	CleanupClosePushL(mbmFile);
	User::LeaveIfError(mbmFile.Open(appList->ShareProtectedFileServer(), aMbmFileName, EFileShareReadersOnly));

	for(iconIndex=0; iconIndex<aNumOfIcons; ++iconIndex)	
		{		
		CApaMaskedBitmap* bitmap = CApaMaskedBitmap::NewLC();
		fileIndex = IconIndexL(aMbmFileName, useCache);
		User::LeaveIfError(bitmap->Load(mbmFile, 2*fileIndex));
		User::LeaveIfError((bitmap->Mask())->Load(mbmFile,2*fileIndex+1));		
		aIcons.AppendL(bitmap);
		CleanupStack::Pop(bitmap);		
		}
	
	CleanupStack::PopAndDestroy(&mbmFile); // close mbmFile	

	// now sort them in ascending order of size
	TInt numberOfIcons = aIcons.Count();
	// create a new array that we can sort by area, populated from aIcons
	RPointerArray<CApaMaskedBitmap> ptrArray(numberOfIcons);
	CleanupClosePushL(ptrArray);
	TLinearOrder<CApaMaskedBitmap> order(CApaIconLoader::CompareIcons);
	for (iconIndex=0; iconIndex<numberOfIcons; iconIndex++)
		{
		User::LeaveIfError(ptrArray.InsertInOrderAllowRepeats(aIcons[iconIndex], order));
		}
	// copy the sorted icon pointers back into aIcons - must not Leave inside the loop below
	for (iconIndex=0; iconIndex<numberOfIcons; iconIndex++)
		{
		aIcons[iconIndex]=ptrArray[iconIndex];
		}
	CleanupStack::PopAndDestroy(&ptrArray);

	return ETrue;
	}

TInt CApaIconLoader::CompareIcons(const CApaMaskedBitmap& aFirst, const CApaMaskedBitmap& aSecond)
	{ // static
	TSize sizeFirst = aFirst.SizeInPixels();
	TInt areaFirst = sizeFirst.iWidth*sizeFirst.iHeight;
	TSize sizeSecond = aSecond.SizeInPixels();
	TInt areaSecond = sizeSecond.iWidth*sizeSecond.iHeight;
	return areaFirst - areaSecond;
	}

//
// Class CApaAppIconArray
//

CApaAppIconArray* CApaAppIconArray::NewL()
	{
	return new(ELeave) CApaAppIconArray();
	}

/* public factory functions */
CApaAppIconArray* CApaAppIconArray::NewAppIconsL(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader)
	{
	return NewL(aNumOfIcons,aMbmFileName,aIconLoader,ETrue);
	}

CApaAppIconArray* CApaAppIconArray::NewViewIconsL(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader)
	{
	return NewL(aNumOfIcons,aMbmFileName,aIconLoader,EFalse);
	}

CApaAppIconArray* CApaAppIconArray::NewDefaultIconsL()
	{
	CApaAppIconArray* self = NewL();
	CleanupStack::PushL(self);
	self->GetDefaultIconsL();
	CleanupStack::Pop(self);
	return self;
	}

CApaAppIconArray* CApaAppIconArray::NewRealDefaultIconsLC(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader)
	{
	return NewLC(aNumOfIcons,aMbmFileName,aIconLoader,EFalse);
	}

/* real NewL and NewLC, private */
CApaAppIconArray* CApaAppIconArray::NewL(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader, TBool aFallbackToDefaultIcons)
	{
	CApaAppIconArray* self = NewLC(aNumOfIcons,aMbmFileName,aIconLoader,aFallbackToDefaultIcons);
	CleanupStack::Pop(self);
	return self;
	}

CApaAppIconArray* CApaAppIconArray::NewLC(TInt aNumOfIcons, const TDesC& aMbmFileName, CApaIconLoader& aIconLoader, TBool aFallbackToDefaultIcons)
	{
	CApaAppIconArray* self = new(ELeave) CApaAppIconArray(aNumOfIcons,aIconLoader,aFallbackToDefaultIcons);
	CleanupStack::PushL(self);
	self->ConstructL(aMbmFileName);
	return self;
	}

CApaAppIconArray::CApaAppIconArray()
	{
	}

CApaAppIconArray::CApaAppIconArray(TInt aNumOfIcons, CApaIconLoader& aIconLoader, TBool aFallbackToDefaultIcons) :
		iNumOfIcons(aNumOfIcons),
		iIconLoader(&aIconLoader),
		iFallbackToDefaultIcons(aFallbackToDefaultIcons)
	{
	}

void CApaAppIconArray::ConstructL(const TDesC& aMbmFileName)
	{
	iMbmFileName = aMbmFileName.AllocL();
	ASSERT(iIconLoader);
	iIconLoader->AddIconArrayL(*this);
	}

CApaAppIconArray::~CApaAppIconArray()
	{
	if(iDefaultIconsUsed)
		{
		ASSERT(iDefaultIconsProvider);
		if(iDefaultIconsProvider)
			iDefaultIconsProvider->ReleaseDefaultIconArray();
		}
	delete iMbmFileName;
	if (iIcons)
		{
		iIcons->ResetAndDestroy();
		delete iIcons;
		}
	iDefaultIconsProvider = NULL;
	iIconLoader = NULL;
	}

// returns EFalse if there was an error obtaining iMbmFileName's entry information,
// otherwise returns ETrue or leaves with a system-wide error code
TBool CApaAppIconArray::LoadIconsL()
	{
	ASSERT(iIconLoader);
	ASSERT(!iDefaultIconsUsed);
	ASSERT(!iIcons);
	iIcons = new(ELeave) CArrayPtrFlat<CApaMaskedBitmap>(5);
	TBool badMbmEntryInfo = EFalse;
	TRAPD(err,badMbmEntryInfo = !iIconLoader->LoadIconsL(iNumOfIcons,*iMbmFileName,*iIcons));

	// We'll be called by the iconLoader at most once, and after we've been called,
	// it might be destroyed. So we shouldn't keep a pointer to it anymore.
	iIconLoader = NULL;

	if ((badMbmEntryInfo || err != KErrNone) && iFallbackToDefaultIcons)
		{
		GetDefaultIconsL();
		}
	else if (badMbmEntryInfo)
		{
		return EFalse;
		}
	else if (err != KErrNone)
		{
		User::Leave(err);
		}
	return ETrue;
	}

CApaMaskedBitmap* CApaAppIconArray::IconBySize(const TSize& aSize) const
	{
	if(iDefaultIconsUsed)
		{
		return DefaultIcons().IconBySize(aSize);
		}
	else if(iIcons == NULL && iIconLoader == NULL)
		{
		// Then we've been created with the default constructor, just pretend
		// there's no icon of the right size
		return NULL;
		}
	else if(iIcons == NULL)
		{
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
		// Arriving here means the the icons need loading but havn't been loaded yet.
		// when we have lazy loading, this will be a trigger condition for loading them
		ASSERT(iIcons);
		return NULL;
		}
	else
		{
#else
		// Arriving here means the the icons need loading but havn't been loaded yet.
		TRAPD(err, iIconLoader->LoadAllIconsL());
		if (err != KErrNone)
			{
			return NULL;
			}
		}
#endif // SYMBIAN_APPARC_APPINFO_CACHE

	const TInt count = iIcons->Count();
	CApaMaskedBitmap* bmp=NULL;
	for (TInt ii=0;ii<count;ii++)
		{
		if ((*iIcons)[ii]->SizeInPixels()==aSize)
			bmp=(*iIcons)[ii];
		}
	if (!bmp)
		{// if the exact size has not been found get one with nearest but smaller area
		TInt differenceOfClosest=KMaxTInt;
		TInt indexOfClosest=KMaxTInt;
		TInt reqArea=aSize.iHeight*aSize.iWidth;
		TSize bmpSize;
		TInt difference;
		for (TInt jj=0;jj<count;jj++)
			{
			bmpSize=(*iIcons)[jj]->SizeInPixels();
			difference=reqArea-bmpSize.iHeight*bmpSize.iWidth;
			if (difference<differenceOfClosest && difference>=0)
				{
				differenceOfClosest=difference;
				indexOfClosest=jj;
				}
			}
		if (indexOfClosest<KMaxTInt)
			bmp=(*iIcons)[indexOfClosest];
		}
	return bmp;
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
	}
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	}

CArrayFixFlat<TSize>* CApaAppIconArray::IconSizesL() const
	{
	if(iDefaultIconsUsed)
		{
		return DefaultIcons().IconSizesL();
		}
	else if(iIcons == NULL && iIconLoader == NULL)
		{
		return new(ELeave)CArrayFixFlat<TSize>(1);
		}
	else if(iIcons == NULL)
		{
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
		// Arriving here means the the icons need loading but havn't been loaded yet.
		// when we have lazy loading, this will be a trigger condition for loading them
		ASSERT(iIcons);
		return new(ELeave)CArrayFixFlat<TSize>(1);
		}
	else
		{
#else
		// Arriving here means the the icons need loading but havn't been loaded yet.
		TRAPD(err, iIconLoader->LoadAllIconsL());
		if (err != KErrNone)
			{
			return new(ELeave)CArrayFixFlat<TSize>(1);
			}
		}
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	const TInt count=iIcons->Count();
	CArrayFixFlat<TSize>* arrayOfSizes=new(ELeave)CArrayFixFlat<TSize>(1);
	CleanupStack::PushL(arrayOfSizes);
	for(TInt ii=0;ii<count;ii++)
		{
		arrayOfSizes->AppendL((*iIcons)[ii]->SizeInPixels());
		}
	CleanupStack::Pop(arrayOfSizes);
	return arrayOfSizes;
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
		}
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	}

TInt CApaAppIconArray::Count() const
	{
	if(iDefaultIconsUsed)
		{
		return DefaultIcons().Count();
		}
	else if(iIcons == NULL && iIconLoader == NULL)
		{
		return 0;
		}
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
	else if(iIcons == NULL)
		{
		// Arriving here means the the icons need loading but havn't been loaded yet.
		// when we have lazy loading, this will be a trigger condition for loading them
		ASSERT(iIcons);
		return 0;
		}
	else
		{
		return iIcons->Count();
		}
#else
	else if(iIcons == NULL)
		{
		// Arriving here means the the icons need loading but havn't been loaded yet.
		TRAPD(err, iIconLoader->LoadAllIconsL());
		if (err != KErrNone)
			{
			return 0;
			}
		}
	return iIcons->Count();
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	}

TBool CApaAppIconArray::DefaultIconsUsed() const
	{
	return iDefaultIconsUsed;
	}

CApaMaskedBitmap* CApaAppIconArray::operator[](TInt aIndex) const
	{
	if(iDefaultIconsUsed)
		{
		return DefaultIcons()[aIndex];
		}
	else if(iIcons == NULL && iIconLoader == NULL)
		{
		Panic(EPanicIndexOutOfRange);
		return NULL;
		}
#ifndef SYMBIAN_APPARC_APPINFO_CACHE
	else if(iIcons == NULL)
		{
		// Arriving here means the the icons need loading but havn't been loaded yet.
		// when we have lazy loading, this will be a trigger condition for loading them
		ASSERT(iIcons);
		return NULL;
		}
	else
		{
		return (*iIcons)[aIndex];
		}
#else
	else if(iIcons == NULL)
		{
		// Arriving here means the the icons need loading but havn't been loaded yet.
		TRAPD(err, iIconLoader->LoadAllIconsL());
		if (err != KErrNone)
			{
			return NULL;
			}
		}
	return (*iIcons)[aIndex];
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	}

#ifdef SYMBIAN_APPARC_APPINFO_CACHE
TBool CApaAppIconArray::AreAppIconsLoaded() const
	{
	if (iDefaultIconsUsed)
		{
		return ETrue;
		}
	if (iNumOfIcons > 0 && !iIcons)
		{
		return EFalse;	
		}
	return ETrue;
	}

TBool CApaAppIconArray::AreViewIconsLoaded() const
	{
	if (iNumOfIcons > 0 && !iIcons )
		{
		return EFalse;
		}
	return ETrue;	
	}
#endif // SYMBIAN_APPARC_APPINFO_CACHE

void CApaAppIconArray::GetDefaultIconsL()
	{
	ASSERT(!iDefaultIconsUsed);
	iDefaultIconsProvider = CApaAppList::Self();
	ASSERT(iDefaultIconsProvider);
	iDefaultIconsProvider->AcquireDefaultIconArrayL();
	iDefaultIconsUsed = ETrue;
	delete iMbmFileName;
	iMbmFileName = NULL;
	if (iIcons)
		{
	 	iIcons->ResetAndDestroy();
		delete iIcons;
		iIcons = NULL;
		}
	}

const CApaAppIconArray& CApaAppIconArray::DefaultIcons() const
	{
	ASSERT(iDefaultIconsUsed && iDefaultIconsProvider);
	return iDefaultIconsProvider->DefaultIconArray();
	}
