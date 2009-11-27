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

#ifndef __APRFNDR_H__
#define __APRFNDR_H__

#include <apaid.h>
#include <f32file.h>
#include <apsidchecker.h>

class TDriveUnitInfo;

/**
@internalComponent
*/
class CApaAppRegFinder : public CBase
	{
public: // Construction / Destruction
	IMPORT_C static CApaAppRegFinder* NewL(const RFs& aFs);
	IMPORT_C static CApaAppRegFinder* NewLC(const RFs& aFs);
	~CApaAppRegFinder();
private: // Private construction
	CApaAppRegFinder(const RFs& aFs);
	void ConstructL();
public:
	IMPORT_C TBool NextL(TApaAppEntry& aEntry, 	const RPointerArray<HBufC>& aForcedRegistrations); // increments a scan for all apps
	IMPORT_C void FindAllAppsL(); // sets up a scan for all apps
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	IMPORT_C const RArray<TDriveUnitInfo>& DriveList() const;
	IMPORT_C void FindAllRemovableMediaAppsL();	// sets up a scan for all removable media apps
#endif	// SYMBIAN_APPARC_APPINFO_CACHE

private:
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	void BuildDriveListL();
#else
	void GetDriveListL();
	static void BuildDriveListL(const RFs& aFs, const TDriveList& aSourceDriveList, RArray<TDriveUnitInfo>& aDestDriveList);
#endif	// SYMBIAN_APPARC_APPINFO_CACHE
	TInt GetFileListL(TDriveUnit aDriveUnit, const TDesC& aPathName);
	void GetFolderListL();
	static void CheckErrorL(TInt aError);
	TBool DoNextL(TApaAppEntry& aEntry, const RPointerArray<HBufC>& aForcedRegistrations,
		TBool& aEntryFound);
	TBool GetNextDriveAndFolder();

private:
	enum TScanStage
		{
		EScanStageNonImportROM,
		EScanStageImportNonROM, 
		EScanStageImportNonNativeResourceNonROM,
		EScanStageImportROM,
		EScanStageImportNonNativeResourceROM,
		EScanStageComplete
		};
	

private:
	CDir* iFileList;
	TInt iCurrentIndexInDriveList;
	TInt iCurrentIndexInFolderList;
	TInt iFileIndex;
	const RFs& iFs;
	RPointerArray<HBufC> iListOfFolders;
	TInt iScanStage;
	RSidCheckerMap iSidCheckerMap;
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	RArray<TDriveUnitInfo> iListOfDrives;
	TBool iRemovableMediaDriveScan;
#else
	RArray<TDriveUnitInfo>* iListOfDrives;
#endif	// SYMBIAN_APPARC_APPINFO_CACHE
	TUid iLastChkedApp; //Uid of the last app whose sid cheking has been done.
	};
	
class TDriveUnitInfo 
{
public:
 TDriveUnit iDriveUnit; 
 TUint		iDriveAtt;
};
	
#endif // __APRFNDR_H__
