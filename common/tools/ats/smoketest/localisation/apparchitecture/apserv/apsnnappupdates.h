// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef APSNNAPPUPDATES_H
#define APSNNAPPUPDATES_H

#include <e32base.h>
#include <f32file.h>
	


// classes referenced
class CApaAppData;
class CApaAppListServer;
class RApsUpdateLog;
class RWriteStream;

/**
This class keeps track of a resource file or icon file, 
whether it is in its final location or a temporary directory.

The NonNativeApps updates system makes heavy use of moving files
between temporary and final locations, in an attempt to provide
atomicity of update lists.

@internalComponent
*/
NONSHARABLE_CLASS(TFileDetails)
	{
public:
	TFileDetails();
	const TDesC& Path();
	void SetPath(const TDesC& aPath);
	TBool Exists() const;
	void CloseHandle();
	
	void OpenL(RFs& aFs, const TFileName& aFileName);
	void CreateTemporaryL(RFs& aFs, const TFileName& aDir);
	void GetDuplicateHandleL(RFile& aFile);

	void RenameToRealL(RFs& aFs);
	void RenameToTemporaryL(RFs& aFs, const TFileName& aDir);
	TInt Delete(RFs& aFs);
	TInt DeleteTemporary(RFs& aFs);
	TInt RestoreReal(RFs& aFs);

	void ExternalizeL(RWriteStream& aStream);
	void ExternalizeContinuationL(RWriteStream& aStream, TInt aTag);
	TInt ExternalizeContinuation(RWriteStream& aStream, TInt aTag);
	void InternalizeL(RReadStream& aStream, TInt& aPosition);
	void PostInternalizeL(RFs& aFs);

private:
	TInt RenameToReal(RFs& aFs);

private:
	enum TState
		{
		EStateInvalid = 0,
		EStateNull,
		EStateTemporary,
		EStateReal
		};
	TState iState;
	TFileName iPath;
	TFileName iTempPath;
	RFile iHandle;
	};

/**
Abstract base class for objects that represent updates to the non-native applications
registry. Subclasses must implement the DoPerformUpdateL and DoRollbackUpdate methods.

- DoPerformUpdateL will only ever be called once or not at all
- DoPerformUpdateL will be called first, if at all
- DoRollbackUpdate will not be called before DoPerformUpdateL
- DoRollbackUpdate will be called if DoPerformUpdateL fails on this update or any subsequent one
- DoRollbackUpdate will not be called twice
- The object may be deleted at any time.

If the device is turned off / crashes partway through performing updates, 
the following will be called upon reboot:

- The subclass' NewL, with dummy values instead of class-specific data (if any)
- InternalizePerformLogL() will be called immediately afterwards
- InternalizeRollbackLogL() iff a rollback action for this update is in the log
- PostInternalizeL() will be called after the log has been read in entirely
- DoRollbackUpdaetL() may then be called if, after replaying the log, the class' state is
                      EPerforming, EPerformed or ERollingBack

@internalComponent
*/
NONSHARABLE_CLASS(CApsNonNativeApplicationsUpdate) : public CBase
	{
public: // Types
	enum TState
		{
		ENew,
		ENeedsInternalizing,
		EPerforming,
		EPerformed,
		ERollingBack,
		ERolledBack
		};
	
	enum TLogActionType
		{
		EInvalidAction = 0,
		ENewUpdate,				// Indicates the record of an update's initial state
		EPerformUpdate,			// Indicates the start of a Perform-update log
		ERollbackUpdate,        // Indicates the start of a Rollback-update log
		EContinuationOfUpdate,  // Specifies that the current update's log has not finished
		EChangeOfUpdateState,   // Similar to EContinuationOfUpdate, but used by this superclass
		EEndOfUpdate            // Specifies that the current update's log has finished
		};

	enum TLogUpdateType
		{
		EInvalidUpdate = 0,
		ERegisterApplication,   // Indicates the start of a RegisterApplication update
		EDeregisterApplication, // Indicates the start of a DeregisterApplication update
#ifdef _DEBUG
		EFail,                  // Indicates the start of a Forced-Fail update
		EPanic,                 // Indicates the start of a Forced-Panic update
		ERollbackPanic          // Indicates the start of a Forced-Panic-On-Rollback update
#endif
		};

public:
	~CApsNonNativeApplicationsUpdate();

protected:
	CApsNonNativeApplicationsUpdate(RFs& aFs, TUid aUid, TState aState, TLogUpdateType aType);

public: 
	CApsNonNativeApplicationsUpdate* Previous() const;
	CApsNonNativeApplicationsUpdate* Next() const;
	TUid Uid();
	void PerformUpdateL(RApsUpdateLog& aUpdateLog);
	void RollbackUpdateL(RApsUpdateLog& aUpdateLog);
	void InternalizeStateChangeL(RReadStream& aStream, TInt& aPosition);

	void InternalizeNewUpdateL(RReadStream& aStream, TInt& aPosition);
	void InternalizePerformUpdateL(RReadStream& aStream, TInt& aPosition);
	void InternalizeRollbackUpdateL(RReadStream& aStream, TInt& aPosition);

protected:
	TFileName TemporaryFilePathL(TDriveName& aDrive);
	void StateChangeL(TState aState, RWriteStream& aStream);

private:
	void SharedInternalizeLoopL(RReadStream& aStream, TInt& aPosition);

	virtual void DoPerformUpdateL(RApsUpdateLog& aUpdateLog) = 0;
	virtual void DoRollbackUpdate(RApsUpdateLog& aUpdateLog) = 0;

	virtual void ExternalizeL(RWriteStream& aStream);
	virtual void InternalizeL(RReadStream& aStream, TInt& aPosition);
	virtual void InternalizeUpdateContinuationL(RReadStream& aStream, TInt& aPosition);
	virtual void PostInternalizeL();


protected:
	TState iState;
	RFs& iFs;

private:
	friend class CApsNonNativeApplicationsUpdateList; // so it can link up iPrevious/iNext
	const TLogUpdateType iType;
	CApsNonNativeApplicationsUpdate* iPrevious;
	CApsNonNativeApplicationsUpdate* iNext;
	const TUid iUid;
	};

/**
Implementation of CApsNonNativeApplicationsUpdate that performs the registration of a
non-native application.

The Write/Copy methods of this class should be used to prepare the update,
they will write resource and icon files to a temporary directory.

When this update is executed, the files will be moved to the locations set with the
Set*FileTargetLocation functions.

@internalComponent
*/
NONSHARABLE_CLASS(CApsRegisterNonNativeApplication) : public CApsNonNativeApplicationsUpdate
	{
public:
	static CApsRegisterNonNativeApplication* NewL(RFs& aFs, TUid aUid, const TDriveName& aDrive, TState aState = ENew);
	~CApsRegisterNonNativeApplication();

private:
	CApsRegisterNonNativeApplication(RFs& aFs, TUid aUid, const TDriveName& aDrive, TState aState);

public:
	void SetResourceFileTargetLocation(const TDesC& aLocation);
	void WriteResourceFileL(const TDesC8& aData, const TDesC8* aDataPrefix);

	void SetLocalisableResourceFileTargetLocation(const TDesC& aLocation);
	void WriteLocalisableResourceFileL( const TDesC8& aData, const TDesC8* aDataPrefix);

	void SetIconFileTargetLocation(const TDesC& aLocation);
	void CopyIconFileL(RFile& aSourceFile);

private:	
	void NewTemporaryFileL(TFileDetails& aFile);
	void WriteResourceFileL(TFileDetails& aFile, const TDesC8& aData, const TDesC8* aDataPrefix);

	// from CApsNonNativeApplicationsUpdate
	void DoPerformUpdateL(RApsUpdateLog& aUpdateLog);
	void DoRollbackUpdate(RApsUpdateLog& aUpdateLog);
	void ExternalizeL(RWriteStream& aStream);
	void InternalizeL(RReadStream& aStream, TInt& aPosition);
	void InternalizeUpdateContinuationL(RReadStream& aStream, TInt& aPosition);
	void PostInternalizeL();


private:
	enum TLogContinuationType 
		{
		EInvalidContinuation = 0,
		EResourceFileUpdate,
		ELocalisableResourceFileUpdate,
		EIconFileUpdate
		};
	TDriveName iDrive;
	TFileDetails iResourceFile;
	TFileDetails iLocalisableResourceFile;
	TFileDetails iIconFile;
	};

/**
Implementation of CApsNonNativeApplicationsUpdate that performs the deregistration of a
non-native application.

@internalComponent
*/
NONSHARABLE_CLASS(CApsDeregisterNonNativeApplication) : public CApsNonNativeApplicationsUpdate
	{
public:
	static CApsDeregisterNonNativeApplication* NewL(RFs& aFs, CApaAppListServer& aServ, TUid aUid, TState aState = ENew);
	~CApsDeregisterNonNativeApplication();

private:
	CApsDeregisterNonNativeApplication(RFs& aFs, CApaAppListServer& aServ, TUid aUid, TState aState);

private:
	void RenameToTemporaryL(TFileDetails& aFile, RApsUpdateLog& aUpdateLog);
	CApaAppData* FindAppDataLC(RApsUpdateLog& aUpdateLog);

	// from CApsNonNativeApplicationsUpdate
	void DoPerformUpdateL(RApsUpdateLog& aUpdateLog);
	void DoRollbackUpdate(RApsUpdateLog& aUpdateLog);
	void ExternalizeL(RWriteStream& aStream);
	void InternalizeL(RReadStream& aStream, TInt& aPosition);
	void InternalizeUpdateContinuationL(RReadStream& aStream, TInt& aPosition);
	void PostInternalizeL();

private:
	enum TLogContinuationType 
		{
		EInvalidContinuation = 0,
		EResourceFileUpdate,
		ELocalisableResourceFileUpdate,
		EIconFileUpdate
		};
	CApaAppListServer& iServ;
	TFileDetails iResourceFile;
	TFileDetails iLocalisableResourceFile;
	TFileDetails iIconFile;
	};


#ifdef _DEBUG
/**
CApsAlwaysFailUpdate

A dummy CNonNativeApplicationsUpdate for testing purpouses.
Always fails with KErrGeneral when performed

@internalComponent
*/
NONSHARABLE_CLASS(CApsAlwaysFailUpdate) : public CApsNonNativeApplicationsUpdate
	{
public:
	CApsAlwaysFailUpdate(RFs& aFs, TState aState = ENew);
private:
	// from CApsNonNativeApplicationsUpdate
	void DoPerformUpdateL(RApsUpdateLog& aUpdateLog);
	void DoRollbackUpdate(RApsUpdateLog&) {}
	};


/**
CApsAlwaysPanicUpdate

A dummy CNonNativeApplicationsUpdate for testing purpouses.
Always panics when performed

@internalComponent
*/
NONSHARABLE_CLASS(CApsAlwaysPanicUpdate) : public CApsNonNativeApplicationsUpdate
	{
public:
	CApsAlwaysPanicUpdate(RFs& aFs, TState aState = ENew);
private:
	// from CApsNonNativeApplicationsUpdate
	void DoPerformUpdateL(RApsUpdateLog& aUpdateLog);
	void DoRollbackUpdate(RApsUpdateLog&) {}
	};


/**
CApsAlwaysPanicOnRollbackUpdate

A dummy CNonNativeApplicationsUpdate for testing purpouses.
Always panics upon rollback

@internalComponent
*/
NONSHARABLE_CLASS(CApsAlwaysPanicOnRollbackUpdate) : public CApsNonNativeApplicationsUpdate
	{
public:
	CApsAlwaysPanicOnRollbackUpdate(RFs& aFs, TState aState = ENew);
private:
	// from CApsNonNativeApplicationsUpdate
	void DoPerformUpdateL(RApsUpdateLog&) {}
	void DoRollbackUpdate(RApsUpdateLog& aUpdateLog);
	};
#endif // _DEBUG


#endif // APSNNAPPUPDATES_H
