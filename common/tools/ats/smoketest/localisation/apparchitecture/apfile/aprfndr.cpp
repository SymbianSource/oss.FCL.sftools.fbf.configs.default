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

#include "aprfndr.h"
#include "APFDEF.H"
#include "APFSTD.H"
#include "../apgrfx/APGAIR.H"
#include <apsserv.h>
#include "../apgrfx/apprivate.h"
#include <apsidchecker.h>

/////////////////////////////
// CApaAppRegFinder
/////////////////////////////

EXPORT_C CApaAppRegFinder* CApaAppRegFinder::NewL(const RFs& aFs)
	{
	CApaAppRegFinder* self=NewLC(aFs);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CApaAppRegFinder* CApaAppRegFinder::NewLC(const RFs& aFs)
	{
	CApaAppRegFinder* self=new (ELeave) CApaAppRegFinder(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CApaAppRegFinder::CApaAppRegFinder(const RFs& aFs)
	: iFs(aFs),
	  iListOfFolders(2),
	  iSidCheckerMap(CApaAppListServer::Self()?CApaAppListServer::Self()->RescanCallBack():TCallBack(NULL,NULL))
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	  ,iRemovableMediaDriveScan(EFalse)
#endif // SYMBIAN_APPARC_APPINFO_CACHE

	{
	}

#ifdef SYMBIAN_APPARC_APPINFO_CACHE
CApaAppRegFinder::~CApaAppRegFinder()
	{
	iListOfFolders.ResetAndDestroy();
	iListOfFolders.Close();
	iListOfDrives.Close();
	iSidCheckerMap.Close();
	delete iFileList; // should already be deleted and NULL at this point
	}
#else
CApaAppRegFinder::~CApaAppRegFinder()
	{
	iListOfFolders.ResetAndDestroy();
	iListOfFolders.Close();
	if (iListOfDrives)
		{
		iListOfDrives->Close();
		delete iListOfDrives;
		}
	iSidCheckerMap.Close();
	
	delete iFileList; // should already be deleted and NULL at this point
	}
#endif // SYMBIAN_APPARC_APPINFO_CACHE

void CApaAppRegFinder::ConstructL()
	{
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	BuildDriveListL();
#else
	GetDriveListL();
#endif // SYMBIAN_APPARC_APPINFO_CACHE
	GetFolderListL();
	}

#ifndef SYMBIAN_APPARC_APPINFO_CACHE
void CApaAppRegFinder::GetDriveListL()
	{
	TDriveList driveList;
	User::LeaveIfError(iFs.DriveList(driveList));
	iListOfDrives = new(ELeave) RArray<TDriveUnitInfo>(4);
	BuildDriveListL(iFs, driveList, *iListOfDrives);
	}
#endif // SYMBIAN_APPARC_APPINFO_CACHE

_LIT(KAppRegRscSearchPath,"\\private\\10003a3f\\apps\\");
_LIT(KAppRegRscImportSearchPath,"\\private\\10003a3f\\import\\apps\\");
_LIT(KAppRegRscImportNonNativeSearchPath,"\\private\\10003a3f\\import\\apps\\NonNative\\Resource\\");

void CApaAppRegFinder::GetFolderListL()
	{
	TPtrC ptr(KAppRegRscSearchPath);
	HBufC* searchPath = ptr.AllocLC();
	User::LeaveIfError(iListOfFolders.Append(searchPath));
	CleanupStack::Pop(searchPath);
	searchPath = NULL;

	ptr.Set(KAppRegRscImportSearchPath);
	searchPath = ptr.AllocLC();
	User::LeaveIfError(iListOfFolders.Append(searchPath));
	CleanupStack::Pop(searchPath);

	ptr.Set(KAppRegRscImportNonNativeSearchPath);
	searchPath = ptr.AllocLC();
	User::LeaveIfError(iListOfFolders.Append(searchPath));
	CleanupStack::Pop(searchPath);
	}

#ifndef SYMBIAN_APPARC_APPINFO_CACHE
void CApaAppRegFinder::BuildDriveListL(const RFs& aFs, const TDriveList& aSourceDriveList, RArray<TDriveUnitInfo>& aDestDriveList)
	{ // static
	TInt drive(EDriveY);
	TDriveUnitInfo driveUnitInfo;
	while(drive >= EDriveA)
		{
		if (aSourceDriveList[drive]!=KDriveAbsent)
			{
			TDriveInfo driveInfo;
			TInt ret = aFs.Drive(driveInfo,drive);	
			if (ret==KErrNone)
				{
				if (driveInfo.iType!=EMediaNotPresent && driveInfo.iType!=EMediaRemote)
					{
					driveUnitInfo.iDriveUnit=drive;
					driveUnitInfo.iDriveAtt=driveInfo.iDriveAtt;
					User::LeaveIfError(aDestDriveList.Append(driveUnitInfo));
					}
				}
			}
		--drive;
		}
	driveUnitInfo.iDriveUnit=EDriveZ;
	driveUnitInfo.iDriveAtt=KDriveAttRom;	
	User::LeaveIfError(aDestDriveList.Append(driveUnitInfo));
	}

EXPORT_C void CApaAppRegFinder::FindAllAppsL()
	{
	delete iFileList;
	iFileList = NULL;
	if(iListOfDrives)
		{
		iListOfDrives->Close();
		delete iListOfDrives;
		iListOfDrives = NULL;
		}
	GetDriveListL();
	iCurrentIndexInDriveList=-1;
	iCurrentIndexInFolderList=0;
	iScanStage=EScanStageNonImportROM;
	iLastChkedApp = KNullUid;
	}

#else
void CApaAppRegFinder::BuildDriveListL()
	{
	TDriveList driveList;
	User::LeaveIfError(iFs.DriveList(driveList));

	TInt drive(EDriveY);
	TDriveUnitInfo driveUnitInfo;
	while(drive >= EDriveA)
		{
		if (driveList[drive]!=KDriveAbsent)
			{
			TDriveInfo driveInfo;
			TInt ret = iFs.Drive(driveInfo,drive);
			if (ret==KErrNone)
				{
				if (iRemovableMediaDriveScan)
					{// iListOfDrives should have list of all the removable drives irrespective of presence of the media
					if ((driveInfo.iType != EMediaRemote) && (driveInfo.iDriveAtt & KDriveAttRemovable))
						{
						driveUnitInfo.iDriveUnit=drive;
						driveUnitInfo.iDriveAtt=driveInfo.iDriveAtt;
						User::LeaveIfError(iListOfDrives.Append(driveUnitInfo));
						}
					}
				else
					{
					if (driveInfo.iType!=EMediaNotPresent && driveInfo.iType!=EMediaRemote)
						{
						driveUnitInfo.iDriveUnit=drive;
						driveUnitInfo.iDriveAtt=driveInfo.iDriveAtt;
						User::LeaveIfError(iListOfDrives.Append(driveUnitInfo));
						}
					}
				}
			}
		--drive;
		}
	if (!iRemovableMediaDriveScan)
		{
		driveUnitInfo.iDriveUnit=EDriveZ;
		driveUnitInfo.iDriveAtt=KDriveAttRom;
		User::LeaveIfError(iListOfDrives.Append(driveUnitInfo));
		}
	}

EXPORT_C void CApaAppRegFinder::FindAllRemovableMediaAppsL()
	{
	iRemovableMediaDriveScan = ETrue;	
	FindAllAppsL();
	}

EXPORT_C const RArray<TDriveUnitInfo>& CApaAppRegFinder::DriveList() const
	{
	return (iListOfDrives);
	}

EXPORT_C void CApaAppRegFinder::FindAllAppsL()
	{
	delete iFileList;
	iFileList = NULL;
	iListOfDrives.Reset();
	BuildDriveListL();
	iCurrentIndexInDriveList=-1;
	iCurrentIndexInFolderList=0;
	iScanStage = EScanStageNonImportROM;	
	iRemovableMediaDriveScan = EFalse;
	iLastChkedApp = KNullUid;
	}
#endif // SYMBIAN_APPARC_APPINFO_CACHE

/** Scans the applications in following order:
1. non-import y-a,z but only on ROM drives
2. import on all non-ROM drives in the order y-a,z
3. import on all ROM drives in the order y-a,z

Upgrades of apparc server (via Software Install) are currently disallowed. 
This means it is not possible to install registration files to \private\10003a3f\apps 
(on any drive), so there is no need to scan \private\10003a3f\apps on non ROM based drives.
*/
EXPORT_C TBool CApaAppRegFinder::NextL(TApaAppEntry& aAppRegistrationEntry, const RPointerArray<HBufC>& aForcedRegistrations)
	{
	TBool entryFound = EFalse;
	while (DoNextL(aAppRegistrationEntry, aForcedRegistrations, entryFound))
		{
		//do nothing. the point is to keep calling DoNextL
		}
	return entryFound;
	}

TBool CApaAppRegFinder::DoNextL(TApaAppEntry& aAppRegistrationEntry, const RPointerArray<HBufC>& aForcedRegistrations, TBool& aEntryFound)
	{
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	__ASSERT_ALWAYS(iListOfDrives.Count(),Panic(EPanicFindAllAppsNotCalled));
#else
	__ASSERT_ALWAYS(iListOfDrives,Panic(EPanicFindAllAppsNotCalled));
#endif // SYMBIAN_APPARC_APPINFO_CACHE

	if (!iFileList)
		{// scan the next drive/dir
		iFileIndex = 0;
		while (GetNextDriveAndFolder())
			{
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
			if (GetFileListL((iListOfDrives[iCurrentIndexInDriveList]).iDriveUnit, *iListOfFolders[iCurrentIndexInFolderList])==KErrNone)
#else
			if (GetFileListL(((*iListOfDrives)[iCurrentIndexInDriveList]).iDriveUnit, *iListOfFolders[iCurrentIndexInFolderList])==KErrNone)
#endif // SYMBIAN_APPARC_APPINFO_CACHE
				{
				break;
				}
			}
		if (!iFileList)
			{
			aEntryFound = EFalse;
			return EFalse; // no more drives to scan
			}
		}
	// scan the file list
	while (iFileIndex<iFileList->Count())
		{
		const TEntry& entry=(*iFileList)[iFileIndex++];
		if (!ApaUtils::HandleAsRegistrationFile(entry.iType))
			{
			continue; // only interested in app registration resource files
			}
			
		TParse parse;
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
		const TDriveName driveName = (iListOfDrives[iCurrentIndexInDriveList]).iDriveUnit.Name();
#else
		const TDriveName driveName = ((*iListOfDrives)[iCurrentIndexInDriveList]).iDriveUnit.Name();
#endif // SYMBIAN_APPARC_APPINFO_CACHE
		parse.Set(entry.iName, iListOfFolders[iCurrentIndexInFolderList], &driveName);

#ifdef SYMBIAN_APPARC_APPINFO_CACHE
		if (iListOfDrives[iCurrentIndexInDriveList].iDriveAtt&KDriveAttRemovable)
#else
		if ((*iListOfDrives)[iCurrentIndexInDriveList].iDriveAtt&KDriveAttRemovable)
#endif // SYMBIAN_APPARC_APPINFO_CACHE
			{
			if (entry[2] != iLastChkedApp) //Check for validity only if it has not yet been checked
				{
				TUid appTypeUid;
				if(entry[0].iUid == KUidPrefixedNonNativeRegistrationResourceFile)
					{
					appTypeUid = entry[1];
					}
				else
					{
					appTypeUid = KNullUid;
					}
				
				// Get the CAppSidChecker for this type of executable
				TBool validRegistration = ETrue;
				#ifdef SYMBIAN_APPARC_APPINFO_CACHE
				TRAPD(err, validRegistration = iSidCheckerMap.FindSidCheckerL(appTypeUid).AppRegisteredAt(entry[2], (iListOfDrives)[iCurrentIndexInDriveList].iDriveUnit));
				#else
				TRAPD(err, validRegistration = iSidCheckerMap.FindSidCheckerL(appTypeUid).AppRegisteredAt(entry[2], (*iListOfDrives)[iCurrentIndexInDriveList].iDriveUnit));
				#endif // SYMBIAN_APPARC_APPINFO_CACHE
				iLastChkedApp = entry[2];
				if(err == KErrNone)
					{
					if(!validRegistration)
						{
						// Check if this registration file should be included
						// despite not being officially reported as a valid registration
						HBufC* fullName = parse.FullName().AllocLC();
						TInt pos = aForcedRegistrations.FindInOrder(fullName, TLinearOrder<HBufC>(CApaAppList::CompareStrings));
						CleanupStack::PopAndDestroy(fullName);
						if (pos == KErrNotFound)
							{
							continue;
							}
						}
					}
				else if(err != KErrNotFound)
					{
					User::Leave(err);
					}
				}
			else
				{
				continue;
				}
			}
		aAppRegistrationEntry.iUidType = entry.iType;
		aAppRegistrationEntry.iFullName = parse.FullName();
		aEntryFound = ETrue; // we found one
		return EFalse;
		}
	// current Drive has been exhausted
	delete iFileList;
	iFileList = NULL;
	return ETrue; // scan the next drive
	}
	
TBool CApaAppRegFinder::GetNextDriveAndFolder()	
	{
	TBool found=ETrue;
	TBool checkNextFolder=ETrue;
	TDriveUnitInfo driveUnitInfo;
	while (checkNextFolder)
		{
		// move to next folder
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
		if (++iCurrentIndexInDriveList>=iListOfDrives.Count())
#else
		if (++iCurrentIndexInDriveList>=iListOfDrives->Count())
#endif // SYMBIAN_APPARC_APPINFO_CACHE
			{
			iCurrentIndexInDriveList=0;
			++iScanStage;
			}
		
		//Get drive info
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
		driveUnitInfo=iListOfDrives[iCurrentIndexInDriveList];
#else
		driveUnitInfo=(*iListOfDrives)[iCurrentIndexInDriveList];
#endif // SYMBIAN_APPARC_APPINFO_CACHE
		//check folder
		switch (iScanStage)
			{
			case EScanStageNonImportROM:
				if (driveUnitInfo.iDriveAtt==KDriveAttRom)
					{
					iCurrentIndexInFolderList=0;
					checkNextFolder=EFalse;
					}
				break;
			case EScanStageImportNonROM:
				if (driveUnitInfo.iDriveAtt!=KDriveAttRom)
					{
					iCurrentIndexInFolderList=1;
					checkNextFolder=EFalse;
					}
				break;
			case EScanStageImportNonNativeResourceNonROM:
				if (driveUnitInfo.iDriveAtt!=KDriveAttRom)
					{
					iCurrentIndexInFolderList=2;
					checkNextFolder=EFalse;
					}
				break;
			case EScanStageImportROM:
				if (driveUnitInfo.iDriveAtt==KDriveAttRom)
					{
					iCurrentIndexInFolderList=1;
					checkNextFolder=EFalse;
					}
				break;
			case EScanStageImportNonNativeResourceROM:
				if (driveUnitInfo.iDriveAtt==KDriveAttRom)
					{
					iCurrentIndexInFolderList=2;
					checkNextFolder=EFalse;
					}
				break;
			case EScanStageComplete:
				checkNextFolder=EFalse;
				found=EFalse;
				break;
			default:
				break;	
			}
		}
	return found;
	}	
	
_LIT(KAppRegRscSearchAnyFile,"*");

TInt CApaAppRegFinder::GetFileListL(TDriveUnit aDriveUnit, const TDesC& aPathName)
	{
	const TDriveName driveName=aDriveUnit.Name();
	TParse parse;
	TInt error = parse.Set(KAppRegRscSearchAnyFile,&aPathName,&driveName);
	User::LeaveIfError(error);
	TInt ret = iFs.GetDir(parse.FullName(), KEntryAttAllowUid, ESortNone, iFileList);
	CheckErrorL(ret);
	return ret;
	}

void CApaAppRegFinder::CheckErrorL(TInt aError)
// static
	{
	if (aError!=KErrNone && aError!=KErrNotFound && aError!=KErrPathNotFound && aError!=KErrNotReady
		&& aError!=KErrDisMounted && aError!=KErrCorrupt && aError!=KErrNotSupported && aError!=KErrBadName && aError!=KErrLocked)
		{
		User::Leave(aError);
		}
	}
