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


#include "apsnnappupdates.h"

#include <bautils.h>
#include <f32file.h>
#include <s32file.h>

#include "APAID.H"
#include "APGAPLST.H"
#include "APSSERV.H"
#include "APSSTD.H"
#include "../apfile/aprfndr.h"
#include "../apgrfx/apprivate.h"
#include "apsnnapps.h"


/**************************************************************************************************************
 * TFilePositionReset
 **************************************************************************************************************/

/**
This class is used to ensure that a RFile's read/write position is reset to its original value
if a leave occurs.

@internalComponent
*/
NONSHARABLE_CLASS(TFilePositionReset)
	{
public:
	TFilePositionReset(RFile& aFile) :
	 		iFile(aFile),
 			iOriginalFilePosition(-1)
 		{
	 	}

	void RewindToStartLC()
		{
		TInt originalFilePosition=0;
		User::LeaveIfError(iFile.Seek(ESeekCurrent, originalFilePosition)); // retrieves the current file-position
		TInt newFilePosition=0;
		User::LeaveIfError(iFile.Seek(ESeekStart, newFilePosition)); // RFile::Seek(ESeekStart,..) will not modify the TInt, but needs a reference.
		iOriginalFilePosition=originalFilePosition;
		CleanupStack::PushL(TCleanupItem(&StaticReset, this));
		}

	TInt Reset()
		{
		__ASSERT_DEBUG(iOriginalFilePosition>=0, Panic(ENonNativeAppsNegativeStoredFilePosition));
		const TInt error = iFile.Seek(ESeekStart, iOriginalFilePosition);
		iOriginalFilePosition=-1;
		return error;
		}

private:
	static void StaticReset(TAny* aThis)
		{
		// ignore the error code from this, we can't do anything about it
		static_cast<TFilePositionReset*>(aThis)->Reset();
		}

private:
	RFile& iFile;
	TInt iOriginalFilePosition;
	};


/**************************************************************************************************************
 * TFileDetails
 **************************************************************************************************************/

TFileDetails::TFileDetails() :
		iState(EStateNull) 
	{
	}

/*
Returns the real path, (aka the target location) of this file
*/
const TDesC& TFileDetails::Path()
	{
	__ASSERT_DEBUG(iPath != KNullDesC, Panic(ENonNativeAppsTFileDetailsPathNotSet));
	return iPath;
	}


/*
Sets the real path, (aka the target location) of this file
*/
void TFileDetails::SetPath(const TDesC& aPath)
	{
	__ASSERT_DEBUG(iPath == KNullDesC, Panic(ENonNativeAppsTFileDetailsPathAlreadySet));
	iPath = aPath;
	}


/*
Determines whether this TFileDetails has a real path set.
If thie object doesn't have a real path set, it is assumed to not be in use
*/
TBool TFileDetails::Exists() const
	{
	return (iPath != KNullDesC);
	}


/*
Releases whatever file handle is currently held, whether it is on the real file,
temporary file, or none at all
*/
void TFileDetails::CloseHandle()
	{
	iHandle.Close();
	}


/*
Opens a handle on a real file
*/
void TFileDetails::OpenL(RFs& aFs, const TFileName& aFileName)
	{
	__ASSERT_DEBUG(iState == EStateNull, Panic(ENonNativeAppsTFileDetailsOpenInBadState));
	__ASSERT_DEBUG(iPath == KNullDesC, Panic(ENonNativeAppsTFileDetailsOpenWithRealPathSet));
	__ASSERT_DEBUG(iTempPath == KNullDesC, Panic(ENonNativeAppsTFileDetailsOpenWithTempPathSet));

	User::LeaveIfError(iHandle.Open(aFs, aFileName, EFileShareExclusive|EFileStream|EFileWrite));
	iPath = aFileName;
	iState = EStateReal;
	}

/*
Creates a new, temporary file to write new data into
Used in the preparation of a register-application update to create new resource
and icon files before they are moved into their target locations.
*/
void TFileDetails::CreateTemporaryL(RFs& aFs, const TFileName& aDir)
	{
	__ASSERT_DEBUG(iState == EStateNull, Panic(ENonNativeAppsTFileDetailsCreateTempInBadState));
	__ASSERT_DEBUG(iTempPath == KNullDesC, Panic(ENonNativeAppsTFileDetailsCreateTempWithTempPathSet));
	RFile file;
	CleanupClosePushL(file);
	// TODO: remove this hack if/when RFile::Temp is fixed by base
	TInt tempErr = KErrAlreadyExists;
	/* RFile::Temp is a bit dodgy, at least on Winscw with the proxy FS */
	for(TInt tries = 0; tempErr == KErrAlreadyExists && tries < 50; ++tries)
		{
		tempErr = file.Temp(aFs, aDir, iTempPath, EFileShareExclusive|EFileStream|EFileWrite);
		}
	User::LeaveIfError(tempErr);
	CleanupStack::Pop(&file);
	iHandle = file;
	iState = EStateTemporary;
	}

/*
Makes aFile a duplicates of the file handle owned by this class.
Used to write to a file created by CreateTemporaryL()
*/
void TFileDetails::GetDuplicateHandleL(RFile& aFile)
	{
	User::LeaveIfError(aFile.Duplicate(iHandle));
	}


/*
RenameToRealL

Used to perform a register-appliction update
This uses the same code as the non-leaving
RenameToReal().
*/
void TFileDetails::RenameToRealL(RFs& aFs)
	{
	User::LeaveIfError(RenameToReal(aFs));
	}

/*
RenameToReal

Used by RenameToRealL() and RestoreReal()
@see RenameToRealL
@see RestoreReal
*/
TInt TFileDetails::RenameToReal(RFs& aFs)
	{
	__ASSERT_DEBUG(iState == EStateTemporary, Panic(ENonNativeAppsTFileDetailsRenameToRealInBadState));
	__ASSERT_DEBUG(iTempPath != KNullDesC, Panic(ENonNativeAppsTFileDetailsRenameToRealWithoutTempPath));
	__ASSERT_DEBUG(iPath != KNullDesC, Panic(ENonNativeAppsTFileDetailsRenameToRealWithoutRealPath));
	TParse parse;
	parse.SetNoWild(iPath,NULL,NULL);
	if(!BaflUtils::FolderExists(aFs,parse.DriveAndPath())) 
		{
		TInt err = aFs.MkDirAll(parse.DriveAndPath());
		if(err != KErrNone)
			{
			return err;
			}
		}
	//Check if file already exists, if it exists delete it because we might be trying to register an upgrade
	if(BaflUtils::FileExists(aFs, iPath))
		{
		TInt err = BaflUtils::DeleteFile(aFs, iPath);
		if(KErrNone != err)
			{
			return err;
			}
		}
	TInt err = iHandle.Rename(iPath);
	if(err != KErrNone)
		{
		return err;
		}
	iTempPath.Zero();
	iState = EStateReal;
	return KErrNone;
	}

/*
RenameToTemporaryL

Used to perform a deregister-application update
*/
void TFileDetails::RenameToTemporaryL(RFs& aFs, const TFileName& aDir)
	{
	__ASSERT_DEBUG(iState == EStateReal, Panic(ENonNativeAppsTFileDetailsRenameToTempInBadState));
	__ASSERT_DEBUG(iPath != KNullDesC, Panic(ENonNativeAppsTFileDetailsRenameToTempWithoutRealPathSet));
	__ASSERT_DEBUG(iTempPath == KNullDesC, Panic(ENonNativeAppsTFileDetailsRenameToTempWithTempPathSet));
	/* create a temp file and delete it to get an unused filename */
	RFile file;
	CleanupClosePushL(file);
	// TODO: remove this hack if/when RFile::Temp is fixed by base
	TInt tempErr = KErrAlreadyExists;
	/* RFile::Temp is a bit dodgy, at least on Winscw with the proxy FS */
	for(TInt tries = 0; tempErr == KErrAlreadyExists && tries < 50; ++tries)
		{
		tempErr = file.Temp(aFs, aDir, iTempPath, EFileShareExclusive|EFileStream|EFileWrite);
		}
	User::LeaveIfError(tempErr);
	CleanupStack::PopAndDestroy(&file); // close the file handle
	const TInt err = aFs.Delete(iTempPath);
	if(err != KErrNone && err != KErrNotFound) 
		{
		User::Leave(err);
		}

	User::LeaveIfError(iHandle.Rename(iTempPath));
	iState = EStateTemporary;
	}


/*
Delete

Whatever the state, real or temporary, delete it.
Used in the rollback phase of a register-application update.

This function releases the handle (if any) and sets the state back to EStateNull
*/
TInt TFileDetails::Delete(RFs& aFs)
	{
	__ASSERT_DEBUG(iState == EStateNull || iState == EStateReal || iState == EStateTemporary,
			Panic(ENonNativeAppsTFileDetailsDeleteInBadState));
	if(iState == EStateNull)
		{
		return KErrNone;
		}

	iHandle.Close();
	TInt err;
	if(iState == EStateReal)
		{
		err = aFs.Delete(iPath);
		}
	else if(iState == EStateTemporary)
		{
		err = aFs.Delete(iTempPath);
		}
	else
		{
		err = KErrGeneral;
		ASSERT(EFalse);
		}
	iPath.Zero();
	iTempPath.Zero();
	iState = EStateNull;
	return err;
	}


/*
DeleteTemporary

Check that the state is EStateTemporary, and delete it.
This function is used by the destructor of a deregister-application update

This function releases the handle (if any) and sets the state back to EStateNull
*/
TInt TFileDetails::DeleteTemporary(RFs& aFs)
	{
	__ASSERT_DEBUG(iState == EStateNull || iState == EStateTemporary,
			Panic(ENonNativeAppsTFileDetailsDeleteTemporaryInBadState));
	if(iState == EStateNull || iState == EStateTemporary)
		{
		return Delete(aFs);
		}
	return KErrGeneral;
	}


/*
RestoreReal

Whatever the state, real or temporary, try to move it back to the real location.
Used in the rollback phase of a deregister-application update.

This function releases the handle (if any) and sets the state back to EStateNull
*/
TInt TFileDetails::RestoreReal(RFs& aFs)
	{
	__ASSERT_DEBUG(iState == EStateNull || iState == EStateReal || iState == EStateTemporary,
		Panic(ENonNativeAppsTFileDetailsRestoreRealInBadState));
	if(iState == EStateNull)
		{
		return KErrNone;
		}

	TInt err = KErrNone;
	if(iState == EStateReal)
		{
		iHandle.Close();
		}
	else if(iState == EStateTemporary)
		{
		err = RenameToReal(aFs);
		iHandle.Close();
		}
	else
		{
		ASSERT(EFalse);
		}
	iPath.Zero();
	iTempPath.Zero();
	iState = EStateNull;
	return err;
	}


void TFileDetails::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteUint8L(iState);
	aStream.WriteUint32L(iPath.Length());
	aStream.WriteL(iPath);
	aStream.WriteUint32L(iTempPath.Length());
	aStream.WriteL(iTempPath);
	}

void TFileDetails::ExternalizeContinuationL(RWriteStream& aStream, TInt aTag)
	{
	aStream.WriteInt8L(CApsNonNativeApplicationsUpdate::EContinuationOfUpdate);
	aStream.WriteInt8L(aTag);
	ExternalizeL(aStream);
	aStream.CommitL(); // beta stopping point
	}

TInt TFileDetails::ExternalizeContinuation(RWriteStream& aStream, TInt aTag)
	{
	TRAPD(err,ExternalizeContinuationL(aStream,aTag));
	return err;
	}

void TFileDetails::InternalizeL(RReadStream& aStream, TInt& aPosition)
	{
	TState state = static_cast<TState>(aStream.ReadUint8L());
	TUint pathLen = aStream.ReadUint32L();
	if(pathLen > KMaxFileName)
		{
		User::Leave(KErrCorrupt);
		}
	TFileName path;
	aStream.ReadL(path, pathLen);
	TUint tempPathLen = aStream.ReadUint32L();
	if(tempPathLen > KMaxFileName)
		{
		User::Leave(KErrCorrupt);
		}
	TFileName tempPath;
	aStream.ReadL(tempPath, tempPathLen);

	/* all dangerous operations are now out of the way */
	iState = state;
	iPath = path;
	iTempPath = tempPath;
	// state, length, path, length, temppath
	aPosition += sizeof(TUint8) +  sizeof(TUint32) + (pathLen * sizeof(TText)) +  sizeof(TUint32) + (tempPathLen * sizeof(TText));
	}

/*
Works out what file we had a handle on when the log was written, and try to regain it.
*/	
void TFileDetails::PostInternalizeL(RFs& aFs)
	{
	TInt err;
	switch(iState)
		{
	case EStateNull:
		if(iTempPath != KNullDesC)
			{
			User::Leave(KErrCorrupt);
			}
		break;
	case EStateTemporary:
		if(iTempPath == KNullDesC)
			{
			User::Leave(KErrCorrupt);
			}
		err = iHandle.Open(aFs, iTempPath, EFileShareExclusive|EFileStream|EFileWrite);
		if(err == KErrNotFound || err == KErrPathNotFound)
			{
			iTempPath.Zero();
			iPath.Zero();
			iState = EStateNull;
			}
		else 
			{
			User::LeaveIfError(err);
			}
		break;
	case EStateReal:
		if(iPath == KNullDesC)
			{
			User::Leave(KErrCorrupt);
			}
		err = iHandle.Open(aFs, iPath, EFileShareExclusive|EFileStream|EFileWrite);
		if(err == KErrNotFound || err == KErrPathNotFound)
			{
			iTempPath.Zero();
			iPath.Zero();
			iState = EStateNull;
			}
		else 
			{
			User::LeaveIfError(err);
			}
		break;
	default:
		User::Leave(KErrCorrupt);
		break;
		}
	}

/**************************************************************************************************************
 * CApsNonNativeApplicationsUpdate
 **************************************************************************************************************/

CApsNonNativeApplicationsUpdate::CApsNonNativeApplicationsUpdate(RFs& aFs, TUid aUid, TState aState, TLogUpdateType aType) :
		iState(aState),
		iFs(aFs),
		iType(aType),
		iUid(aUid)
	{
	}

CApsNonNativeApplicationsUpdate::~CApsNonNativeApplicationsUpdate()
	{
	}

CApsNonNativeApplicationsUpdate* CApsNonNativeApplicationsUpdate::Previous() const
	{
	return iPrevious;
	}

CApsNonNativeApplicationsUpdate* CApsNonNativeApplicationsUpdate::Next() const
	{
	return iNext;
	}

TUid CApsNonNativeApplicationsUpdate::Uid() 
	{
	return iUid;
	}

void CApsNonNativeApplicationsUpdate::PerformUpdateL(RApsUpdateLog& aUpdateLog)
	{
	ASSERT(iState == ENew);
	RWriteStream& stream = aUpdateLog.LogWriteStream();

	/* NewUpdateSection */
	stream.WriteInt8L(ENewUpdate);
	stream.WriteInt8L(iType);
	stream.WriteInt32L(iUid.iUid);
	ExternalizeL(stream);
	stream.CommitL(); // alpha stopping point

	/* PerformUpdateSection */
	stream.WriteInt8L(EPerformUpdate);
	stream.WriteInt8L(iType);
	stream.WriteInt32L(iUid.iUid);     // beta stopping points
	StateChangeL(EPerforming, stream); // ...
	DoPerformUpdateL(aUpdateLog);      // ...
	StateChangeL(EPerformed, stream);  // ...
	stream.WriteInt8L(EEndOfUpdate);
	stream.CommitL();                  // alpha stopping point
	}

void CApsNonNativeApplicationsUpdate::RollbackUpdateL(RApsUpdateLog& aUpdateLog)
	{
	ASSERT(iState == EPerforming || iState == EPerformed || iState == ERollingBack);
	RWriteStream& stream = aUpdateLog.LogWriteStream();
	stream.WriteInt8L(ERollbackUpdate);
	stream.WriteInt8L(iType);
	stream.WriteInt32L(iUid.iUid);
	stream.CommitL();                   // beta stopping point
 	// even if we're already in ERollingBack because we crashed and are reattempting it,
 	// it's harmless to write a duplicate statechange to the log
	StateChangeL(ERollingBack, stream); // beta stopping points
	DoRollbackUpdate(aUpdateLog);       // ...
	StateChangeL(ERolledBack, stream);  // ...
	stream.WriteInt8L(EEndOfUpdate);
	stream.CommitL();                   // alpha stopping point
	}

TFileName CApsNonNativeApplicationsUpdate::TemporaryFilePathL(TDriveName& aDrive)
	{
	TFileName dir(aDrive);
	
	// this is safe since KMaxFileName >= (KMaxDriveName + length(KLitPathForTemporaryNonNativeResourceAndIconFiles))
	dir.Append(KLitPathForTemporaryNonNativeResourceAndIconFiles);

	if(!BaflUtils::FolderExists(iFs,dir)) 
		{
		User::LeaveIfError(iFs.MkDirAll(dir));
		}	
	return dir;
	}

void CApsNonNativeApplicationsUpdate::StateChangeL(TState aState, RWriteStream& aStream)
	{
	iState = aState;
	aStream.WriteInt8L(EChangeOfUpdateState);
	aStream.WriteInt8L(iState);
	aStream.CommitL(); // beta stopping point
	}

void CApsNonNativeApplicationsUpdate::InternalizeStateChangeL(RReadStream& aStream, TInt& aPosition)
	{
	TState state = static_cast<TState>(aStream.ReadInt8L());
	++aPosition;
	iState = state;
	}

void CApsNonNativeApplicationsUpdate::InternalizeNewUpdateL(RReadStream& aStream, TInt& aPosition)
	{
	__ASSERT_DEBUG(iState == ENeedsInternalizing, Panic(ENonNativeAppsNonNativeApplicationsUpdateInternalizeNewUpdateInBadState));
	/*
	internalize all member variables to how they were before any updates were performed.
	*/
	InternalizeL(aStream, aPosition);
	
	iState = ENew; /* the update is now OK to add to the list */
	}

void CApsNonNativeApplicationsUpdate::InternalizePerformUpdateL(RReadStream& aStream, TInt& aPosition)
	{
	/*
	internalize any state changes and update-specific continuations, until EEndOfUpdate
	*/
	SharedInternalizeLoopL(aStream, aPosition);
	}

void CApsNonNativeApplicationsUpdate::InternalizeRollbackUpdateL(RReadStream& aStream, TInt& aPosition)
	{
	/*
	internalize any state changes and update-specific continuations, until EEndOfUpdate
	*/
	SharedInternalizeLoopL(aStream, aPosition);
	}

void CApsNonNativeApplicationsUpdate::SharedInternalizeLoopL(RReadStream& aStream, TInt& aPosition)
	{
	while (ETrue)
		{
		TInt pos = aPosition;
		TLogActionType action = static_cast<TLogActionType>(aStream.ReadInt8L());
		pos += sizeof(TInt8);
		/* we are at a beta stopping point at the start of each loop iteration,
		   so don't update aPosition until we have reached the end of the loop.
		   give the funcitons a copy to stop them updating aPosition direclty and
		   then leaving */
		switch(action)
			{
		case EChangeOfUpdateState:
			InternalizeStateChangeL(aStream, pos);
			break;
		case EContinuationOfUpdate:
			InternalizeUpdateContinuationL(aStream, pos);
			break;
		case EEndOfUpdate:
			aPosition = pos;
			return;
		default:
			User::Leave(KErrCorrupt);
			break;
			}
		aPosition = pos;
		}
	}

/*
 Default implemenation of a virtual function. does nothing.

 In this method, subclasses should write out all important, subclass-specific data.
 It will be called before DoPerformUpdateL.
 */
void CApsNonNativeApplicationsUpdate::ExternalizeL(RWriteStream& /*aStream*/)
	{
	}

/*
 Default implemenation of a virtual function. does nothing.

 In this method, subclasses read in any subclass-specific data that was written out
 by ExternalizeL().
 */
void CApsNonNativeApplicationsUpdate::InternalizeL(RReadStream& /*aStream*/, TInt& /*aPosition*/)
	{
	}

/*
 Default implemenation of a virtual function. does nothing.

 In this method, subclasses should handle any EContinuationOfUpdate messages that
 they might have written to the log during DoPerformUpdateL or DoRollbackUpdate
 */
void CApsNonNativeApplicationsUpdate::InternalizeUpdateContinuationL(RReadStream& /*aStream*/, TInt& /*aPosition*/)
	{
	}

/*
 Default implemenation of a virtual function. does nothing.

 In this method, subclasses should do any internal initialization that is dependent on
 data that may be changed by InternalizeUpdateContinuationL().
 
 InternalizeUpdateContinuationL() may be called many times over and its implementation
 may change the same member variable many times.
 This function will only be called once, and it will be called after the entire log has
 been read and InternalizeUpdateContinuationL() has been called for the last time.
 */
void CApsNonNativeApplicationsUpdate::PostInternalizeL()
	{
	}

/**************************************************************************************************************
 * CApsRegisterNonNativeApplication
 **************************************************************************************************************/

CApsRegisterNonNativeApplication* CApsRegisterNonNativeApplication::NewL(RFs& aFs, TUid aUid, const TDriveName& aDrive, TState aState)
	{
	return new(ELeave) CApsRegisterNonNativeApplication(aFs, aUid, aDrive, aState);
	}

CApsRegisterNonNativeApplication::CApsRegisterNonNativeApplication(RFs& aFs, TUid aUid, const TDriveName& aDrive, TState aState) :
		CApsNonNativeApplicationsUpdate(aFs, aUid, aState, ERegisterApplication),
		iDrive(aDrive)
	{
	}

void CApsRegisterNonNativeApplication::SetResourceFileTargetLocation(const TDesC& aLocation)
	{
	iResourceFile.SetPath(aLocation);
	}

void CApsRegisterNonNativeApplication::SetLocalisableResourceFileTargetLocation(const TDesC& aLocation)
	{
	iLocalisableResourceFile.SetPath(aLocation);
	}

void CApsRegisterNonNativeApplication::SetIconFileTargetLocation(const TDesC& aLocation)
	{
	iIconFile.SetPath(aLocation);
	}

void CApsRegisterNonNativeApplication::WriteResourceFileL(const TDesC8& aData, const TDesC8* aDataPrefix) 
	{
	WriteResourceFileL(iResourceFile, aData, aDataPrefix);
	}

void CApsRegisterNonNativeApplication::WriteLocalisableResourceFileL(const TDesC8& aData, const TDesC8* aDataPrefix)
	{
	WriteResourceFileL(iLocalisableResourceFile, aData, aDataPrefix);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Create a new file in a designated temporary-files directory
*/
void CApsRegisterNonNativeApplication::NewTemporaryFileL(TFileDetails& aFile)
	{
	TFileName path(TemporaryFilePathL(iDrive));
	aFile.CreateTemporaryL(iFs,path);
	}

/**
Writes a resource file to a new temporary file
 */
void CApsRegisterNonNativeApplication::WriteResourceFileL(TFileDetails& aFile, const TDesC8& aData, const TDesC8* aDataPrefix)
	{
	// create temp file, put stuff on the cleanup stack
	NewTemporaryFileL(aFile);
	RFileWriteStream targetStream;
	CleanupClosePushL(targetStream);

	// the stream takes ownership of the file handle and closes it, so make a copy
	RFile targetFile;
	aFile.GetDuplicateHandleL(targetFile);
	targetStream.Attach(targetFile); // will take ownership of the handle and set targetFile to a Null handle

	if (aDataPrefix!=NULL)
		{
		targetStream.WriteL(*aDataPrefix);
		}
	targetStream.WriteL(aData);
	targetStream.CommitL();

	CleanupStack::PopAndDestroy(&targetStream);
	}

/**
This function will copy the file provided to a new temporary file.
Upon success or failure, the read/write position of aSourceFile should remain unchanged.
 */
void CApsRegisterNonNativeApplication::CopyIconFileL(RFile& aSourceFile)
	{
	NewTemporaryFileL(iIconFile);

	// RFile*Stream::Attach will take ownership of the handle we give it, but we want to hold
	// on to the handles we have. To achieve this, we give Attach a copy of the file handle,
	// for both streams.
	RFileWriteStream targetStream;
	CleanupClosePushL(targetStream);
	RFile targetFile;
	iIconFile.GetDuplicateHandleL(targetFile);
	targetStream.Attach(targetFile);

	// rewind to start, but restore original position if a leave occurs
	TFilePositionReset filePositionReset(aSourceFile);
	filePositionReset.RewindToStartLC();

	RFileReadStream sourceStream;
	CleanupClosePushL(sourceStream);
	RFile sourceFile;
	User::LeaveIfError(sourceFile.Duplicate(aSourceFile));
	sourceStream.Attach(sourceFile);

	targetStream.WriteL(sourceStream);
	targetStream.CommitL();
	CleanupStack::PopAndDestroy(&sourceStream);

	// perform these seperately so we can leave if Reset fails
	CleanupStack::Pop(&filePositionReset);
	User::LeaveIfError(filePositionReset.Reset());

	CleanupStack::PopAndDestroy(&targetStream);
	}

void CApsRegisterNonNativeApplication::DoPerformUpdateL(RApsUpdateLog& aUpdateLog)
	{
	HBufC* hDrive = iDrive.AllocL();
	TInt err = aUpdateLog.DrivesAffected().InsertInOrder(hDrive, TLinearOrder<HBufC>(CApaAppList::CompareStrings));
	if(err != KErrNone)
		{
		delete hDrive;
		}

	if ((err != KErrNone) &&
	    (err != KErrAlreadyExists)) // We silently ignore attempts to insert duplicates
		{
		User::Leave(err);
		}
	
	RWriteStream& stream = aUpdateLog.LogWriteStream();

	iResourceFile.RenameToRealL(iFs);

	/* make a note of what's changed */
	iResourceFile.ExternalizeContinuationL(stream, EResourceFileUpdate);

	if(iLocalisableResourceFile.Exists()) 
		{
		iLocalisableResourceFile.RenameToRealL(iFs);
		iResourceFile.ExternalizeContinuationL(stream, ELocalisableResourceFileUpdate);
		}

	if(iIconFile.Exists())
		{
		iIconFile.RenameToRealL(iFs);
		iResourceFile.ExternalizeContinuationL(stream, EIconFileUpdate);
		}
	
	HBufC* hPath = iResourceFile.Path().AllocL();
	err = aUpdateLog.NewRegistrationFiles().InsertInOrder(hPath, TLinearOrder<HBufC>(CApaAppList::CompareStrings));
	if(err != KErrNone)
		{
		delete hPath;
		}
	
	if ((err != KErrNone) &&
	    (err != KErrAlreadyExists)) // We silently ignore attempts to insert duplicates
		{
		User::Leave(err);
		}
	}

void CApsRegisterNonNativeApplication::DoRollbackUpdate(RApsUpdateLog& aUpdateLog)
	{
	RWriteStream& stream = aUpdateLog.LogWriteStream();

	/* TFileDetails::Delete() is safe to call whatever state the object is in.

	   if it was never opened, it'll do nothing, otherwise it'll delete whichever
	   file (real/temp) it has a handle on.
	
	   ignore the return codes since we can't do anything about it if they fail */
	iResourceFile.Delete(iFs);
	iResourceFile.ExternalizeContinuation(stream, EResourceFileUpdate);

	iLocalisableResourceFile.Delete(iFs);
	iResourceFile.ExternalizeContinuation(stream, ELocalisableResourceFileUpdate);

	iIconFile.Delete(iFs);
	iResourceFile.ExternalizeContinuation(stream, EIconFileUpdate);
	}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CApsRegisterNonNativeApplication::ExternalizeL(RWriteStream& aStream)
	{
	/* write our initial state to the log stream */
	aStream.WriteUint32L(iDrive.Length());
	aStream.WriteL(iDrive);
	iResourceFile.ExternalizeL(aStream);
	iLocalisableResourceFile.ExternalizeL(aStream);
	iIconFile.ExternalizeL(aStream);
	}

void CApsRegisterNonNativeApplication::InternalizeL(RReadStream& aStream, TInt& aPosition)
	{
	// we can update aPosition whenever we like in this function since we're protected
	// from it being left at a bad value by the copy taken by CApsNonNativeApplicationsUpdateList::InternalizeActionL
	TUint driveLen = aStream.ReadUint32L();
	aPosition += sizeof(TUint32);
	if(driveLen > KMaxDriveName)
		{
		User::Leave(KErrCorrupt);
		}
	iDrive.Zero();
	aStream.ReadL(iDrive, driveLen);
	aPosition += driveLen * sizeof(TText);

	iResourceFile.InternalizeL(aStream, aPosition);
	iLocalisableResourceFile.InternalizeL(aStream, aPosition);
	iIconFile.InternalizeL(aStream, aPosition);
	}
	
void CApsRegisterNonNativeApplication::InternalizeUpdateContinuationL(RReadStream& aStream, TInt& aPosition)
	{
	// we can update aPosition whenever we like in this function since we're protected
	// from it being left at a bad value by the copy taken by CApsNonNativeApplicationsUpdate::SharedInternalizeLoopL

	TLogContinuationType type = static_cast<TLogContinuationType>(aStream.ReadInt8L());
	aPosition += sizeof(TInt8);

	switch(type)
		{
	case EResourceFileUpdate:
		iResourceFile.InternalizeL(aStream, aPosition);
		break;
	case ELocalisableResourceFileUpdate:
		iLocalisableResourceFile.InternalizeL(aStream, aPosition);
		break;
	case EIconFileUpdate:
		iIconFile.InternalizeL(aStream, aPosition);
		break;
	default:
		User::Leave(KErrCorrupt);
		break;
		}
	}

void CApsRegisterNonNativeApplication::PostInternalizeL()
	{
	iResourceFile.PostInternalizeL(iFs);
	iLocalisableResourceFile.PostInternalizeL(iFs);
	iIconFile.PostInternalizeL(iFs);
	}
	
CApsRegisterNonNativeApplication::~CApsRegisterNonNativeApplication()
	{
	/*
	if we performed the update successfully, just close the handles and leave
	the reg files in place.

	in all other cases, just delete everything that still exists
	 */
	if(iState == EPerformed) 
		{
		iResourceFile.CloseHandle();
		iLocalisableResourceFile.CloseHandle();
		iIconFile.CloseHandle();
		}
	else
		{
		iResourceFile.Delete(iFs);
		iLocalisableResourceFile.Delete(iFs);
		iIconFile.Delete(iFs);
		}
	}


/**************************************************************************************************************
 * CApsDeregisterNonNativeApplication
 **************************************************************************************************************/

CApsDeregisterNonNativeApplication* CApsDeregisterNonNativeApplication::NewL(RFs& aFs, CApaAppListServer& aServ, TUid aUid, TState aState)
	{
	return new(ELeave)CApsDeregisterNonNativeApplication(aFs, aServ, aUid, aState);
	}

CApsDeregisterNonNativeApplication::CApsDeregisterNonNativeApplication(RFs& aFs, CApaAppListServer& aServ, TUid aUid, TState aState) :
		CApsNonNativeApplicationsUpdate(aFs, aUid, aState, EDeregisterApplication),
		iServ(aServ)
	{
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


CApaAppData* CApsDeregisterNonNativeApplication::FindAppDataLC(RApsUpdateLog& aUpdateLog)
	{
	/* search back through the list for a an update concerning the same Uid */
	CApsNonNativeApplicationsUpdate* prev = Previous();
	while(prev != NULL)
		{
		if(prev->Uid() == Uid())
			break;
		prev = prev->Previous();
		}

	/* if none found, use apparc's app list as a shortcut */
	CApaAppData* appData = NULL;
	if(prev == NULL)
		{
		appData = iServ.AppList().AppDataByUid(Uid());

		if(appData != NULL) 
			{
			// create a new AppData so we can put it on the cleanup stack.
			// if the NewL fails for whatever reson, fall back to the search code below

			// this is a bit tricky because we need to create the appdata from a
			// TApaAppEntry for the reg file, but using appData->AppEntry() will give us
			// one for the dll.
			TApaAppEntry entry;
			entry.iFullName = appData->RegistrationFileName();
			TEntry fsEntry;

			User::LeaveIfError(iFs.Entry(entry.iFullName,fsEntry));
			entry.iUidType = fsEntry.iType;

			appData = NULL;
			#ifdef SYMBIAN_APPARC_APPINFO_CACHE
 				TRAP_IGNORE(appData = CApaAppData::NewL(entry,iFs));
 			#else
 				{
 				// find the default icons (.mbm file) for applications, wrt current locale
 				TFileName* tempFileName = NULL;
 				tempFileName->Append(KDefaultAppIconMbm);
 				BaflUtils::NearestLanguageFile(iFs, *tempFileName); 
 				HBufC* defaultAppIcon = tempFileName->AllocL();
 				TRAP_IGNORE(appData = CApaAppData::NewL(entry,iFs,*defaultAppIcon));
 				}
 			#endif //SYMBIAN_APPARC_APPINFO_CACHE
			if(appData != NULL)
				{
				CleanupStack::PushL(appData);
				return appData;
				}
			}
		}

	/*
	 * If the appData wasn't found or we can't trust the app list because of previous updates,
	 * attempt to find and load the appData manually
	 */
	CApaAppRegFinder* regFinder = CApaAppRegFinder::NewLC(iFs);
	TBool found = EFalse;
	TApaAppEntry appEntry;

	regFinder->FindAllAppsL();
	RPointerArray<HBufC>& forcedRegs = aUpdateLog.NewRegistrationFiles();

	while(regFinder->NextL(appEntry,forcedRegs))
		{
		if (appEntry.iUidType[2] == Uid())
			{
			found = ETrue;
			break;
			}
		}
	CleanupStack::PopAndDestroy(regFinder);

	if(found)
		{
		#ifdef SYMBIAN_APPARC_APPINFO_CACHE
 			appData = CApaAppData::NewL(appEntry, iFs);
 		#else
			{
 			// find the default icons (.mbm file) for applications, wrt current locale
 			TFileName* tempFileName = NULL;
 			tempFileName->Append(KDefaultAppIconMbm);
 			BaflUtils::NearestLanguageFile(iFs, *tempFileName); 
 			HBufC* defaultAppIcon = tempFileName->AllocL();
 			appData = CApaAppData::NewL(appEntry, iFs,*defaultAppIcon);	
 			}
 		#endif //SYMBIAN_APPARC_APPINFO_CACHE 
		CleanupStack::PushL(appData);
		return appData;
		}

	CleanupStack::PushL(static_cast<CApaAppData*>(NULL));
	return NULL;
	}
		
void CApsDeregisterNonNativeApplication::RenameToTemporaryL(TFileDetails& aFile, RApsUpdateLog& aUpdateLog)
	{
	/* get the drive */
	TParse parse;
	parse.SetNoWild(aFile.Path(), NULL, NULL);
	if(!parse.DrivePresent())
		{
		// should really never happen
		User::Leave(KErrPathNotFound);
		}
	TDriveName drive(parse.Drive());

	HBufC* hDrive = drive.AllocL();
	TInt err = aUpdateLog.DrivesAffected().InsertInOrder(hDrive, TLinearOrder<HBufC>(CApaAppList::CompareStrings));
	if(err != KErrNone)
		{
		delete hDrive;
		}
	
	TFileName path(TemporaryFilePathL(drive));

	aFile.RenameToTemporaryL(iFs,path);
	}

void CApsDeregisterNonNativeApplication::DoPerformUpdateL(RApsUpdateLog& aUpdateLog)
	{
	CApaAppData* appData = FindAppDataLC(aUpdateLog);
	if(appData == NULL)
		{
		/*
		 * App not found.
		 * Behavioural backwards compatibility says we shouldn't raise an error here,
		 * instead just let it go and carry on.
		 */
		CleanupStack::Pop(appData);
		return;
		}

	/*
	 for each of the 3 files, open a handle on the real file if it exists
	 the first one (iResourceFile) is mandatory
	 */
	TRAPD(err,iResourceFile.OpenL(iFs, appData->RegistrationFileName()));
	if(err != KErrNone && err != KErrNotFound)
		{
		User::Leave(err);
		}

	if(appData->LocalisableResourceFileName() != KNullDesC)
		{
		TRAP(err,iLocalisableResourceFile.OpenL(iFs, appData->LocalisableResourceFileName()));
		if(err != KErrNone && err != KErrNotFound)
			{
			User::Leave(err);
			}
		}

	if(appData->IconFileName() != KNullDesC)
		{
		TRAP(err,iIconFile.OpenL(iFs, appData->IconFileName()));
		if(err != KErrNone && err != KErrNotFound)
			{
			User::Leave(err);
			}
		}

	CleanupStack::PopAndDestroy(appData);

	/*
	 perform the actual updates
	 */
	RWriteStream& stream = aUpdateLog.LogWriteStream();

	if(iIconFile.Exists())
		{
		RenameToTemporaryL(iIconFile, aUpdateLog);
		iIconFile.ExternalizeContinuationL(stream, EIconFileUpdate);
		}
	
	if(iLocalisableResourceFile.Exists())
		{
		RenameToTemporaryL(iLocalisableResourceFile, aUpdateLog);
		iLocalisableResourceFile.ExternalizeContinuationL(stream, ELocalisableResourceFileUpdate);
		}

	RenameToTemporaryL(iResourceFile, aUpdateLog);
	iResourceFile.ExternalizeContinuationL(stream, EResourceFileUpdate);
	}

void CApsDeregisterNonNativeApplication::DoRollbackUpdate(RApsUpdateLog& aUpdateLog)
	{
	RWriteStream& stream = aUpdateLog.LogWriteStream();

	iResourceFile.RestoreReal(iFs);
	iResourceFile.ExternalizeContinuation(stream, EResourceFileUpdate);

	iLocalisableResourceFile.RestoreReal(iFs);
	iLocalisableResourceFile.ExternalizeContinuation(stream, ELocalisableResourceFileUpdate);
	
	iIconFile.RestoreReal(iFs);
	iIconFile.ExternalizeContinuation(stream, EIconFileUpdate);
	}

void CApsDeregisterNonNativeApplication::ExternalizeL(RWriteStream& aStream)
	{
	iResourceFile.ExternalizeL(aStream);
	iLocalisableResourceFile.ExternalizeL(aStream);
	iIconFile.ExternalizeL(aStream);
	}

void CApsDeregisterNonNativeApplication::InternalizeL(RReadStream& aStream, TInt& aPosition)
	{
	// we can update aPosition whenever we like in this function since we're protected
	// from it being left at a bad value by the copy taken by CApsNonNativeApplicationsUpdateList::InternalizeActionL
	iResourceFile.InternalizeL(aStream, aPosition);
	iLocalisableResourceFile.InternalizeL(aStream, aPosition);
	iIconFile.InternalizeL(aStream, aPosition);
	}

void CApsDeregisterNonNativeApplication::InternalizeUpdateContinuationL(RReadStream& aStream, TInt& aPosition)
	{
	// we can update aPosition whenever we like in this function since we're protected
	// from it being left at a bad value by the copy taken by CApsNonNativeApplicationsUpdate::SharedInternalizeLoopL

	TLogContinuationType type = static_cast<TLogContinuationType>(aStream.ReadInt8L());
	aPosition += sizeof(TInt8);

	switch(type)
		{
	case EResourceFileUpdate:
		iResourceFile.InternalizeL(aStream, aPosition);
		break;
	case ELocalisableResourceFileUpdate:
		iLocalisableResourceFile.InternalizeL(aStream, aPosition);
		break;
	case EIconFileUpdate:
		iIconFile.InternalizeL(aStream, aPosition);
		break;
	default:
		User::Leave(KErrCorrupt);
		break;
		}
	}

void CApsDeregisterNonNativeApplication::PostInternalizeL()
	{
	iResourceFile.PostInternalizeL(iFs);
	iLocalisableResourceFile.PostInternalizeL(iFs);
	iIconFile.PostInternalizeL(iFs);
	}

CApsDeregisterNonNativeApplication::~CApsDeregisterNonNativeApplication()
	{
	/*
	if we performed the update successfully, we need to delete the temporary

	in all other cases, just delete everything that still exists
	 */
	if(iState == EPerformed) 
		{
		// in this state we know that Perform has completed successfully.
		iResourceFile.DeleteTemporary(iFs);
		iLocalisableResourceFile.DeleteTemporary(iFs);
		iIconFile.DeleteTemporary(iFs);
		}
	else
		{
		// Hopefully these are now (back) in their target locations.
		// if they're not, there's nothing we can do now, so abandon them either way
		iResourceFile.CloseHandle();
		iLocalisableResourceFile.CloseHandle();
		iIconFile.CloseHandle();
		}
	}

#ifdef _DEBUG
/**************************************************************************************************************
 * CApsAlwaysFailUpdate
 **************************************************************************************************************/

CApsAlwaysFailUpdate::CApsAlwaysFailUpdate(RFs& aFs, TState aState) :
		CApsNonNativeApplicationsUpdate(aFs,TUid::Uid(0xDEADBEEF),aState,EFail)
	{
	}

void CApsAlwaysFailUpdate::DoPerformUpdateL(RApsUpdateLog& /*aUpdateLog*/)
	{
	User::Leave(KErrGeneral);
	}

/**************************************************************************************************************
 * CApsAlwaysPanicUpdate
 **************************************************************************************************************/

CApsAlwaysPanicUpdate::CApsAlwaysPanicUpdate(RFs& aFs, TState aState) :
		CApsNonNativeApplicationsUpdate(aFs,TUid::Uid(0xCAFEBABE),aState,EPanic)
	{
	}

void CApsAlwaysPanicUpdate::DoPerformUpdateL(RApsUpdateLog& /*aUpdateLog*/)
	{
	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);
	Panic(ENonNativeAppsTestHandlingPanicDuringUpdate);
	User::SetJustInTime(jit);
	}

/**************************************************************************************************************
 * CApsAlwaysPanicOnRollbackUpdate
 **************************************************************************************************************/

CApsAlwaysPanicOnRollbackUpdate::CApsAlwaysPanicOnRollbackUpdate(RFs& aFs, TState aState) :
		CApsNonNativeApplicationsUpdate(aFs,TUid::Uid(0x1337D00D),aState,ERollbackPanic)
	{
	}

void CApsAlwaysPanicOnRollbackUpdate::DoRollbackUpdate(RApsUpdateLog& /*aUpdateLog*/)
	{
	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);
	Panic(ENonNativeAppsTestHandlingPanicDuringRollback);
	User::SetJustInTime(jit);
	}

#endif // _DEBUG
