// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Non-Native application registration functionality for the AppArc server session
// 
//


#include "apsnnapps.h"

#include <bautils.h>
#include <f32file.h>
#include <s32file.h>

#include "APSCLSV.H"
#include "APSSERV.H"
#include "APSSTD.H"
#include "../apgrfx/apprivate.h"
#include "apsnnappupdates.h"

/**
KNonNativeApplicationsUpdateLog
@internalComponent
*/
_LIT(KNonNativeApplicationsUpdateLog, ":\\private\\10003a3f\\UpdatedAppsList.bin");

/**************************************************************************************************************
 * CApsNonNativeApplicationsUpdateList
 **************************************************************************************************************/

CApsNonNativeApplicationsUpdateList* CApsNonNativeApplicationsUpdateList::NewL(RFs& aFs)
	{
	CApsNonNativeApplicationsUpdateList* self = new(ELeave) CApsNonNativeApplicationsUpdateList(aFs);
	return self;
	}

void CApsNonNativeApplicationsUpdateList::RecoverFromUpdateLogL(RFs& aFs, RFile& aFile)
	{
	CleanupClosePushL(aFile);
	CApsNonNativeApplicationsUpdateList* self = new(ELeave) CApsNonNativeApplicationsUpdateList(aFs);
	CleanupStack::PushL(self);
	self->RecoverFromUpdateLogL(aFile); // consumes aFile
	CleanupStack::PopAndDestroy(self);
	CleanupStack::Pop(&aFile);
	}


CApsNonNativeApplicationsUpdateList::CApsNonNativeApplicationsUpdateList(RFs& aFs) :
		iFs(aFs)
	{
	}

CApsNonNativeApplicationsUpdateList::~CApsNonNativeApplicationsUpdateList()
	{
	CApsNonNativeApplicationsUpdate* update = iUpdateListHead;
	while(update != NULL)
		{
		CApsNonNativeApplicationsUpdate* next = update->Next();
		delete update;
		update = next;
		}
	}

void CApsNonNativeApplicationsUpdateList::RecoverFromUpdateLogL(RFile& aFile)
	{
	RFileReadStream readStream;
	CleanupClosePushL(readStream);
	RFile logFileDupe;
	User::LeaveIfError(logFileDupe.Duplicate(aFile));
	readStream.Attach(logFileDupe); // Attach takes ownership of & Nulls the handle passed to it
	TInt pos = 0;
	InternalizeL(readStream,pos);
	CleanupStack::PopAndDestroy(&readStream);

	/* Great, we've read in all our performed updates and deleted all the ones that were fully
	   rolled back.
	   The next step is to roll back the rest of the updates. We'll need to create an
	   update log as it was before the reboot, so that further rollbacks we successfully perform
	   can also be logged in it */
	
	// pos should point to the end of the last compelte action recorded in the file
	// remove any invalid or half-written actions by truncating to pos
	User::LeaveIfError(aFile.SetSize(pos));
	RApsUpdateLog updateLog(iFs);
	updateLog.Open(aFile,pos); // consumes aFile
	CleanupClosePushL(updateLog);

	if(iEEndOfUpdateRequiredToFixLog)
		{
		updateLog.LogWriteStream().WriteInt8L(CApsNonNativeApplicationsUpdate::EEndOfUpdate);
		updateLog.LogWriteStream().CommitL(); // puts us back to an alpha stopping point
		}

	if(iLogReplayCurrentUpdate != NULL)
		{
		switch(iLogReplayCurrentUpdate->iState)
			{
		case CApsNonNativeApplicationsUpdate::ENew:
			/* current update was never performed. Start rollback at the previous one,
			   which will be in EPerformed */
			iLogReplayCurrentUpdate = iLogReplayCurrentUpdate->Previous();
			ASSERT(iLogReplayCurrentUpdate == NULL || iLogReplayCurrentUpdate->iState == CApsNonNativeApplicationsUpdate::EPerformed);
			break;
		case CApsNonNativeApplicationsUpdate::EPerforming:
		case CApsNonNativeApplicationsUpdate::EPerformed:
		case CApsNonNativeApplicationsUpdate::ERollingBack:
			break;
		case CApsNonNativeApplicationsUpdate::ERolledBack:
			User::Leave(KErrCorrupt);
			break;
			}
		Rollback(iLogReplayCurrentUpdate, updateLog);
		}
	CleanupStack::PopAndDestroy(&updateLog);
	}

void CApsNonNativeApplicationsUpdateList::InternalizeL(RReadStream& aStream, TInt& aPosition)
	{
	TInt err = KErrNone;
	while(err == KErrNone)
		{
		TRAP(err,InternalizeActionL(aStream,aPosition));
		if(err == KErrNone || err == KErrCorrupt || err == KErrEof)
			{
			// We anticipate that the last update we try to read may be incomplete or corrupt.
			// if we get either of these, jsut stop reading in the list of updates.
			}
		else
			{
			// something really unexpected, e.g. KErrNoMemory
			User::Leave(err);
			}
		}
	CApsNonNativeApplicationsUpdate* update = iUpdateListHead;
	while(update) 
		{
		update->PostInternalizeL();
		update = update->Next();
		}
	}

void CApsNonNativeApplicationsUpdateList::InternalizeActionL(RReadStream& aStream, TInt& aPosition)
	{
	TInt pos = aPosition;

	CApsNonNativeApplicationsUpdate::TLogActionType action =
		static_cast<CApsNonNativeApplicationsUpdate::TLogActionType>(aStream.ReadInt8L());
	CApsNonNativeApplicationsUpdate::TLogUpdateType type =
		static_cast<CApsNonNativeApplicationsUpdate::TLogUpdateType>(aStream.ReadInt8L());
	TUid appUid;
	appUid.iUid = aStream.ReadInt32L();
	pos += sizeof(TInt8) + sizeof(TInt8) + sizeof(TUint32);

	switch(action)
		{
	case CApsNonNativeApplicationsUpdate::ENewUpdate:
		// for ENewUpdate, this is not a valid stopping point,
		// so we don't want to change aPosition unless it completes successfully.
		InternalizeNewUpdateL(aStream, pos, type, appUid);
		aPosition = pos;
		break;
	case CApsNonNativeApplicationsUpdate::EPerformUpdate:
		// for EPerformUpdate, this is a beta stopping point, and
		// InternalizePerformUpdateL will only ever update aPosition to
		// another stopping point, so we can give it the real aPosition.
		// Since it's a beta stopping point, we need to set iEEndofUpdateRequiredToFixLog
		iEEndOfUpdateRequiredToFixLog = ETrue;
		aPosition = pos;
		InternalizePerformUpdateL(aStream, aPosition, type, appUid);
		iEEndOfUpdateRequiredToFixLog = EFalse;
		break;
	case CApsNonNativeApplicationsUpdate::ERollbackUpdate:
		// this action has the same behaviour as EPerformUpdate
		iEEndOfUpdateRequiredToFixLog = ETrue;
		aPosition = pos;
		InternalizeRollbackUpdateL(aStream, aPosition, type, appUid);
		iEEndOfUpdateRequiredToFixLog = EFalse;
		break;
	default:
		User::Leave(KErrCorrupt);
		break;
		}
	}

void CApsNonNativeApplicationsUpdateList::InternalizeNewUpdateL(RReadStream& aStream, TInt& aPosition, CApsNonNativeApplicationsUpdate::TLogUpdateType aType, TUid aUid)
	{
	// check that this is the first update or the previous update was performed completely
	if(iLogReplayCurrentUpdate !=  NULL && (iLogReplayCurrentUpdate->iState != CApsNonNativeApplicationsUpdate::EPerformed || iLogReplayCurrentUpdate->Next() != NULL))
		{
		User::Leave(KErrCorrupt);
		}

	/*
	 create the update object, depending on type
	 */
	CApsNonNativeApplicationsUpdate* update = NULL;
	switch(aType) 
		{
	case CApsNonNativeApplicationsUpdate::ERegisterApplication:
		update = CApsRegisterNonNativeApplication::NewL(iFs, aUid, TDriveName(), CApsNonNativeApplicationsUpdate::ENeedsInternalizing);
		break;
	case CApsNonNativeApplicationsUpdate::EDeregisterApplication:
		update = CApsDeregisterNonNativeApplication::NewL(iFs, *CApaAppListServer::Self(), aUid, CApsNonNativeApplicationsUpdate::ENeedsInternalizing);
		break;
#ifdef _DEBUG
	case CApsNonNativeApplicationsUpdate::EFail:
		update = new(ELeave) CApsAlwaysFailUpdate(iFs, CApsNonNativeApplicationsUpdate::ENeedsInternalizing);
		break;
	case CApsNonNativeApplicationsUpdate::EPanic:
		update = new(ELeave) CApsAlwaysPanicUpdate(iFs, CApsNonNativeApplicationsUpdate::ENeedsInternalizing);
		break;
	case CApsNonNativeApplicationsUpdate::ERollbackPanic:
		update = new(ELeave) CApsAlwaysPanicOnRollbackUpdate(iFs, CApsNonNativeApplicationsUpdate::ENeedsInternalizing);
		break;
#endif
	default:
		User::Leave(KErrCorrupt);
		break;
		}
	CleanupStack::PushL(update);
	update->InternalizeNewUpdateL(aStream, aPosition);
	CleanupStack::Pop(update);
	/* alpha stopping point.
	   update->iState should be ENew now.
	 */
	Append(update);
	iLogReplayCurrentUpdate = update;
	}

void CApsNonNativeApplicationsUpdateList::InternalizePerformUpdateL(RReadStream& aStream, TInt& aPosition, CApsNonNativeApplicationsUpdate::TLogUpdateType aType, TUid aUid)
	{
	if(iLogReplayCurrentUpdate == NULL || iLogReplayCurrentUpdate->iState != CApsNonNativeApplicationsUpdate::ENew)
		{
		User::Leave(KErrCorrupt);
		}
	if(iLogReplayCurrentUpdate->Uid() != aUid || iLogReplayCurrentUpdate->iType != aType)
		{
		User::Leave(KErrCorrupt);
		}
	/*
	 tell the update to read in its log
	 */
	iLogReplayCurrentUpdate->InternalizePerformUpdateL(aStream, aPosition);
	// the function above will return iff EEndOfUpdate is reached
	}


void CApsNonNativeApplicationsUpdateList::InternalizeRollbackUpdateL(RReadStream& aStream, TInt& aPosition, CApsNonNativeApplicationsUpdate::TLogUpdateType aType, TUid aUid)
	{
	if(iLogReplayCurrentUpdate == NULL)
		{
		User::Leave(KErrCorrupt);
		}
	if( iLogReplayCurrentUpdate->iState != CApsNonNativeApplicationsUpdate::EPerforming
	 && iLogReplayCurrentUpdate->iState != CApsNonNativeApplicationsUpdate::EPerformed
	 && iLogReplayCurrentUpdate->iState != CApsNonNativeApplicationsUpdate::ERollingBack)
		{
		User::Leave(KErrCorrupt);
		}
	if(iLogReplayCurrentUpdate->iType != aType || iLogReplayCurrentUpdate->Uid() != aUid)
		{
		User::Leave(KErrCorrupt);
		}
	
	iLogReplayCurrentUpdate->InternalizeRollbackUpdateL(aStream, aPosition);
	// the function above will return iff EEndOfUpdate is reached

	if(iLogReplayCurrentUpdate->iState == CApsNonNativeApplicationsUpdate::ERolledBack)
		{
		iLogReplayCurrentUpdate = iLogReplayCurrentUpdate->Previous();
		}
	}

void CApsNonNativeApplicationsUpdateList::Append(CApsNonNativeApplicationsUpdate* aUpdate)
	{
	if(iUpdateListHead == NULL)
		{
		iUpdateListHead = aUpdate;
		return;
		}

	CApsNonNativeApplicationsUpdate* update = iUpdateListHead;
	while(update->iNext != NULL)
		{
		update = update->iNext;
		}
	update->iNext = aUpdate;
	aUpdate->iPrevious = update;
	}

void CApsNonNativeApplicationsUpdateList::ExecuteL()
	{
	RApsUpdateLog updateLog(iFs);
	updateLog.OpenL();
	CleanupClosePushL(updateLog);
	CApsNonNativeApplicationsUpdate* update = iUpdateListHead;
	while(update != NULL)
		{
		TRAPD(err,update->PerformUpdateL(updateLog));
		if(err == KErrNone)
			{
			update = update->Next();
			}
		else
			{
			// call rollback on everything, including the one that failed
			// since it may have to clean up half-finished stuff
			Rollback(update, updateLog);
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(&updateLog);
	}

void CApsNonNativeApplicationsUpdateList::Rollback(CApsNonNativeApplicationsUpdate* aStartPoint, RApsUpdateLog& aUpdateLog)
	{
	CApsNonNativeApplicationsUpdate* update = aStartPoint;

	while(update != NULL)
		{
		TRAP_IGNORE(update->RollbackUpdateL(aUpdateLog));
		update = update->Previous();
		}
	}

/**
@internalComponent
*/
RApsUpdateLog::RApsUpdateLog(RFs& aFs) :
		iFs(aFs)
	{
	TChar drive = RFs::GetSystemDriveChar();
	iLogFileName.Append(drive);
	iLogFileName.Append(KNonNativeApplicationsUpdateLog);
	}

/**
RApsUpdateLog::OpenL

Opens a write stream to a log file that keeps track of what updates have been performed.

@internalComponent
*/
void RApsUpdateLog::OpenL()
	{
	User::LeaveIfError(iLogWriteStream.Replace(iFs,iLogFileName,EFileShareExclusive|EFileStream|EFileWrite));
	}

/**
RApsUpdateLog::Open

Opens a write stream to a log file that keeps track of what updates have been performed.

@internalComponent
*/
void RApsUpdateLog::Open(RFile& aFile, TUint aSeekPos)
	{
	iLogWriteStream.Attach(aFile, aSeekPos);
	}

/**
RApsUpdateLog::Close

Cleans up the list of new Registration files created during this set of updates
and deletes the log files created.

@internalComponent
*/
void RApsUpdateLog::Close()
	{
	_LIT(KLitPathForTemporaryFiles, "\\private\\10003a3f\\temp\\");
	const TInt count = iDrivesAffected.Count();
	CFileMan* fileman = NULL;
	TRAPD(err, fileman = CFileMan::NewL(iFs));
	if (err == KErrNone) 
		{
		for(TInt i = 0; i < count; ++i)
			{
			TFileName dir(*(iDrivesAffected[i]));
			dir.Append(KLitPathForTemporaryNonNativeResourceAndIconFiles);
			fileman->RmDir(dir); // recursive
			iFs.RmDir(KLitPathForTemporaryFiles); // non-recursive
			}
		delete fileman;
		}
	
	iFilesRegistered.ResetAndDestroy();
	iDrivesAffected.ResetAndDestroy();
	iLogWriteStream.Close();
	iFs.Delete(iLogFileName);
	}


/**
CApsNonNativeApplicationsUpdateList::RApsUpdateLog holds a list of registration files that have
been written during this transaction. This list is used by the Deregistration
command as the forced-updates list.

This stops them from being incorrectly hidden from the scan performed to find
the registration file to delete.

@internalComponent
*/
RPointerArray<HBufC>& RApsUpdateLog::NewRegistrationFiles()
	{
	return iFilesRegistered;
	}


/**
CApsNonNativeApplicationsUpdateList::RApsUpdateLog holds a list of drives that have been used
in the transaction. This is then used to clean up the temporary file directories

@internalComponent
*/
RPointerArray<HBufC>& RApsUpdateLog::DrivesAffected()
	{
	return iDrivesAffected;
	}

/**
Returns the stream that updates should write their recovery log data to
@internalComponent
*/
RWriteStream& RApsUpdateLog::LogWriteStream()
	{
	return iLogWriteStream;
	}

/**************************************************************************************************************
 * CApsNonNativeApplicationsManager
 **************************************************************************************************************/

void CApsNonNativeApplicationsManager::NotifyScanComplete()
	{
	//See if the session is intrested in scan complete notification.
	if (!iNotifyOnScanCompleteMsg.IsNull())
		{
		iNotifyOnScanCompleteMsg.Complete(KErrNone);
		}
	} //lint !e1762 Suppress member function could be made const


CApsNonNativeApplicationsManager* CApsNonNativeApplicationsManager::NewL(CApaAppListServer& aServ, RFs& aFs)
	{
	CApsNonNativeApplicationsManager* self = new(ELeave)CApsNonNativeApplicationsManager(aServ, aFs);
	return self;
	}

CApsNonNativeApplicationsManager::~CApsNonNativeApplicationsManager()
	{
	delete iUpdateList;
	}

CApsNonNativeApplicationsManager::CApsNonNativeApplicationsManager(CApaAppListServer& aServ, RFs& aFs) :
		iServ(aServ),
		iFs(aFs)
	{
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CApsNonNativeApplicationsManager::PrepareNonNativeApplicationsUpdatesL()
	{
	if (iServ.AppList().AppScanInProgress())
		{
		iServ.AppList().StopScan(ETrue); //Stop updating applist now, we anyways have to update it when we are commiting these changes
		}
	if (iNonNativeApplicationsUpdateAppsLock) 
		{
		User::Leave(KErrInUse);
		}

	ASSERT(iUpdateList == NULL);
	
	iUpdateList = CApsNonNativeApplicationsUpdateList::NewL(iFs);
	iNonNativeApplicationsUpdateAppsLock = ETrue;
	}


void CApsNonNativeApplicationsManager::CheckForUpdateAppsLockL()
	{
	if (!iNonNativeApplicationsUpdateAppsLock)
		{
		// PrepareNonNativeApplicationsUpdatesL hasn't been called successfully
		User::Leave(KErrNotReady);
		}	
	}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 void CApsNonNativeApplicationsManager::RegisterNonNativeApplicationL(const RMessage2& aMessage)
	{
	CheckForUpdateAppsLockL();
	CleanupStack::PushL(TCleanupItem(&StaticAbortNonNativeApplicationsUpdates,this));
	DoRegisterNonNativeApplicationL(aMessage);
	CleanupStack::Pop(this);
	}


void CApsNonNativeApplicationsManager::DoRegisterNonNativeApplicationL(const RMessage2& aMessage)
	{
	// an RBuf for creating the file names in.
	// create it here so it's below ipcParameter0 on the cleanup stack
	RBuf target;
	CleanupClosePushL(target);

	// read in the resource file from aMessage
	RBuf8 ipcParameter0(HBufC8::NewL(aMessage.GetDesLengthL(0)));
	CleanupClosePushL(ipcParameter0);
	aMessage.ReadL(0, ipcParameter0);
	if (ipcParameter0.Length()<=sizeof(SNonNativeApplicationInfo)+sizeof(TCheckedUid))
		{
		User::Leave(KErrArgument);
		}

	// get drivename
	const SNonNativeApplicationInfo nonNativeApplicationInfo = *reinterpret_cast<const SNonNativeApplicationInfo*>(ipcParameter0.Ptr());
	TDriveName driveName = TDriveUnit(nonNativeApplicationInfo.iDrive).Name();

	// get uid
	const TUid applicationUid(TCheckedUid(ipcParameter0.Mid(sizeof(SNonNativeApplicationInfo), sizeof(TCheckedUid))).UidType()[2]);
	if (applicationUid==TUid::Null())
		{
		User::Leave(KErrArgument);
		}

	// construct resource file name
	target.CreateL(driveName, KMaxFileName);
	target.Append(KLitPathForNonNativeResourceAndIconFiles);
	const TInt startOfFileName=target.Length(); // take note of this so we can chop off the file name later
	target.AppendFormat(KLitFormatForRegistrationResourceFile, applicationUid.iUid);

	// prepare registrationResourceFileDataPrefix
	const TUidType uidPrefix(TUid::Uid(KUidPrefixedNonNativeRegistrationResourceFile), nonNativeApplicationInfo.iApplicationType, applicationUid);
	const TBufC8<sizeof(TCheckedUid)> registrationResourceFileDataPrefix(TCheckedUid(uidPrefix).Des());

	// create the object that will actually perform the update
	CApsRegisterNonNativeApplication* updateObject = CApsRegisterNonNativeApplication::NewL(iFs, applicationUid, driveName);
	CleanupStack::PushL(updateObject);

	// write resource file to temp location
	updateObject->SetResourceFileTargetLocation(target);
	updateObject->WriteResourceFileL(ipcParameter0.Mid(sizeof(SNonNativeApplicationInfo)), &registrationResourceFileDataPrefix);
	
	if (aMessage.Int1()!=0) // if there is a localisable resource-file
		{
		RBuf8 ipcParameter1(HBufC8::NewL(User::LeaveIfError(aMessage.GetDesLength(1))));
		CleanupClosePushL(ipcParameter1);
		aMessage.ReadL(1, ipcParameter1);
		
		// construct localisable resoure file name
		target.SetLength(startOfFileName);
		target.AppendFormat(KLitFormatForLocalisableResourceFile, applicationUid.iUid);

		updateObject->SetLocalisableResourceFileTargetLocation(target);
		// write localisable resource file to temp location
		updateObject->WriteLocalisableResourceFileL(ipcParameter1, NULL);
		CleanupStack::PopAndDestroy(&ipcParameter1);

		if (aMessage.Int2()!=0) // if there is an icon-file
			{
			RFile iconFile;
			CleanupClosePushL(iconFile);
			User::LeaveIfError(iconFile.AdoptFromClient(aMessage, 2, 3));
			RBuf iconFileName;
			CleanupClosePushL(iconFileName);
			iconFileName.CreateL(KMaxFileName);
			User::LeaveIfError(iconFile.Name(iconFileName));

			if (startOfFileName+iconFileName.Length()>KMaxFileName)
				{
				User::Leave(KErrArgument);
				}
			target.SetLength(startOfFileName);
			target.Append(iconFileName);
			updateObject->SetIconFileTargetLocation(target);
			updateObject->CopyIconFileL(iconFile);

			CleanupStack::PopAndDestroy(2, &iconFile); // iconFileName, iconFile
			}
		}

	CleanupStack::Pop(updateObject);
	iUpdateList->Append(updateObject);

	CleanupStack::PopAndDestroy(2, &target); // ipcParameter0, target
	}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void CApsNonNativeApplicationsManager::DeregisterNonNativeApplicationL(const RMessage2& aMessage)
	{
	CheckForUpdateAppsLockL();
	CleanupStack::PushL(TCleanupItem(&StaticAbortNonNativeApplicationsUpdates,this));
	const TUid applicationUid(TUid::Uid(aMessage.Int0()));
	iUpdateList->Append(CApsDeregisterNonNativeApplication::NewL(iFs, iServ, applicationUid));
	CleanupStack::Pop(this);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CApsNonNativeApplicationsManager::ForceFailInNonNativeApplicationsUpdatesL()
	{
	CheckForUpdateAppsLockL();
	CleanupStack::PushL(TCleanupItem(&StaticAbortNonNativeApplicationsUpdates,this));
	iUpdateList->Append(new(ELeave) CApsAlwaysFailUpdate(iFs));
	CleanupStack::Pop(this);
	}

void CApsNonNativeApplicationsManager::ForcePanicInNonNativeApplicationsUpdatesL()
	{
	CheckForUpdateAppsLockL();
	CleanupStack::PushL(TCleanupItem(&StaticAbortNonNativeApplicationsUpdates,this));
	iUpdateList->Append(new(ELeave) CApsAlwaysPanicUpdate(iFs));
	CleanupStack::Pop(this);
	}

void CApsNonNativeApplicationsManager::ForcePanicInNonNativeApplicationsRollbackL()
	{
	CheckForUpdateAppsLockL();
	CleanupStack::PushL(TCleanupItem(&StaticAbortNonNativeApplicationsUpdates,this));
	iUpdateList->Append(new(ELeave) CApsAlwaysPanicOnRollbackUpdate(iFs));
	CleanupStack::Pop(this);
	}

#endif // _DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CApsNonNativeApplicationsManager::CommitNonNativeApplicationsUpdatesL(const RMessage2& aMessage)
	{
	CheckForUpdateAppsLockL();

	TRAPD(err,iUpdateList->ExecuteL());

	// Failure or success, we can't be hanging on to the lock after this function completes
	delete iUpdateList;
	iUpdateList = NULL;
	iNonNativeApplicationsUpdateAppsLock = EFalse;
	
	if (KErrNone != err)
		{
		if(iServ.AppList().AppListUpdatePending())
			{
			// Trigger a rescan
			iServ.UpdateApps();	
			}
		User::Leave(err); // bail out at this point if things didn't go to plan
		}
	
	if(aMessage.Int0()==(TInt) ETrue)	
		//The request is completed without waiting till completion application list preparation. 
		aMessage.Complete(KErrNone); 
	else	
		//The request is not completed till completion application list preparation.		
		iNotifyOnScanCompleteMsg=aMessage;
	
	// Trigger a rescan
	iServ.UpdateApps();
	}

/**
This function has the word "Rollback" in it because it used to actually roll back the
updates that had been preformed but whose effects were hidden from the public APIs until commit.

The real updates no longer happen until commit-time, so there's no real rolling back to be done here.
The real rolling back is done automatically by iUpdateList, during the commit call, if neccessary.
*/
void CApsNonNativeApplicationsManager::RollbackNonNativeApplicationsUpdates()
	{
	AbortNonNativeApplicationsUpdates();
	}

void CApsNonNativeApplicationsManager::AbortNonNativeApplicationsUpdates()
	{
	if (!iNonNativeApplicationsUpdateAppsLock)
		{
		// It is always ok to call this function but if we haven't called PrepareNonNativeApplicationsUpdatesL()
		// there isn't anything to do
		return;
		}
	if(iServ.AppList().AppListUpdatePending())
		{
		// Trigger a rescan
		iServ.UpdateApps();	
		}
	delete iUpdateList;
	iUpdateList = NULL;
	iNonNativeApplicationsUpdateAppsLock = EFalse;
	}

void CApsNonNativeApplicationsManager::StaticAbortNonNativeApplicationsUpdates(TAny* aSelf)
	{
	static_cast<CApsNonNativeApplicationsManager*>(aSelf)->AbortNonNativeApplicationsUpdates();
	}

void CApsNonNativeApplicationsManager::RecoverFromUpdateLogL(RFs& aFs)
	{
	TChar drive = RFs::GetSystemDriveChar();
	TFileName logPath;
	logPath.Append(drive);
	logPath.Append(KNonNativeApplicationsUpdateLog);
	RFile logFile;
	TInt err = logFile.Open(aFs,logPath,EFileShareExclusive|EFileStream|EFileWrite);
	if(err == KErrNotFound || err == KErrPathNotFound)
		{
		// no log file, nothing to do.
		return;
		}
	User::LeaveIfError(err);
	// this function takes over ownership of logFile
	TRAP(err,CApsNonNativeApplicationsUpdateList::RecoverFromUpdateLogL(aFs, logFile));
	if(err != KErrNone)
		{
		// there's nothing we can do, just delete it and move on
		aFs.Delete(logPath);
		}
	
	}
