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
//

#ifndef APSNNAPPS_H
#define APSNNAPPS_H

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>

#include "apsnnappupdates.h"

// classes referenced
class CApaAppListServer;
class CApsNonNativeApplicationsUpdateList;

_LIT(KLitPathForTemporaryNonNativeResourceAndIconFiles, "\\private\\10003a3f\\temp\\NonNativeUpdates\\");

/**
@internalComponent
*/
NONSHARABLE_CLASS(CApsNonNativeApplicationsManager) : public CBase
	{
public:
	static CApsNonNativeApplicationsManager* NewL(CApaAppListServer& aServ, RFs& aFs);
	~CApsNonNativeApplicationsManager();

private:
	CApsNonNativeApplicationsManager(CApaAppListServer& aServ, RFs& aFs);

public:
	void NotifyScanComplete();
	void PrepareNonNativeApplicationsUpdatesL();
	void RegisterNonNativeApplicationL(const RMessage2& aMessage);
	void DeregisterNonNativeApplicationL(const RMessage2& aMessage);
#ifdef _DEBUG
	void ForceFailInNonNativeApplicationsUpdatesL();
	void ForcePanicInNonNativeApplicationsUpdatesL();
	void ForcePanicInNonNativeApplicationsRollbackL();
#endif
	void CommitNonNativeApplicationsUpdatesL(const RMessage2& aMessage);
	void RollbackNonNativeApplicationsUpdates();
	static void RecoverFromUpdateLogL(RFs& aFs);

private:
	void DoRegisterNonNativeApplicationL(const RMessage2& aMessage);
	void AbortNonNativeApplicationsUpdates();
	static void StaticAbortNonNativeApplicationsUpdates(TAny* aSelf);
	void CheckForUpdateAppsLockL();

private:
	CApaAppListServer& iServ;
	RFs& iFs;
	RMessage2 iNotifyOnScanCompleteMsg;
	TBool iNonNativeApplicationsUpdateAppsLock;
	CApsNonNativeApplicationsUpdateList* iUpdateList;
	};

class RApsUpdateLog;

/**
This class holds a list of updates to the non-native apps registry that should
be performed as one transaction, along with code to execute / rollback the updates.



Update list structure:

These are the possible valid types of list:
(* means "can occur 0 or more times", ? means "can occur 0 or 1 times")

EPerformed*, EPerforming?, ENew*
EPerformed*, ERollingBack?, ERolledBack*, ENew*

The list can be destructed in any of these states:

EPerformed*, ERolledBack*, ENew*

@internalComponent
*/
NONSHARABLE_CLASS(CApsNonNativeApplicationsUpdateList) : public CBase
	{
public:
	static CApsNonNativeApplicationsUpdateList* NewL(RFs& aFs);
	static void RecoverFromUpdateLogL(RFs& aFs, RFile& aFile);
	~CApsNonNativeApplicationsUpdateList();

private:
	CApsNonNativeApplicationsUpdateList(RFs& aFs);
	void RecoverFromUpdateLogL(RFile& aFile);

public:
	void Append(CApsNonNativeApplicationsUpdate* aUpdate);
	void ExecuteL();

private:
	void Rollback(CApsNonNativeApplicationsUpdate* aStartPoint, RApsUpdateLog& aUpdateLog);
	void SetLogWriteStream(RWriteStream* aStream);
	void InternalizeL(RReadStream& aStream, TInt& aPosition);
	void InternalizeActionL(RReadStream& aStream, int& aPosition);
	void InternalizeNewUpdateL(RReadStream& aStream, TInt& aPosition, CApsNonNativeApplicationsUpdate::TLogUpdateType aType, TUid aUid);
	void InternalizePerformUpdateL(RReadStream& aStream, TInt& aPosition, CApsNonNativeApplicationsUpdate::TLogUpdateType aType, TUid aUid);
	void InternalizeRollbackUpdateL(RReadStream& aStream, TInt& aPosition, CApsNonNativeApplicationsUpdate::TLogUpdateType aType, TUid aUid);

private:
	RFs& iFs;
	CApsNonNativeApplicationsUpdate* iUpdateListHead;
	CApsNonNativeApplicationsUpdate* iLogReplayCurrentUpdate;
	TBool iEEndOfUpdateRequiredToFixLog; // for recovering from the log. This is ETrue if we've reached a beta stopping point.
	};


/*
This class opens & owns the recovery log file

Recovery log structure:

As updates are performed, they are written to a log. The log's format on disk is documented here.

When we read the log in, there are certain points that are considered to be "valid stopping points"
These points are at the end of every useful section of data. When we keep track of the stream-position
during internalization, we make sure that the position we record is always at one of these points, never
between them.
This means that we can then truncate the log to the last of these points and then append new operations to the
log.
There are two types of stopping points, i've called them alpha and beta. At an alpha stopping point we
can start appending new operations to the log immediately. At a beta stopping point, we must append a
EEndOfUpdate marker first.


These points are denoted in the structure below with "--- stopping point ---"

LogStructure =
{
	(NewUpdateSection,
	PerformUpdateSection)*
	RollbackUpdateSection*
}

NewUpdateSection = 
{
	Int8  ENewUpdate
	Int8  TLogUpdateType
	Int32 TUid
	... dependent on update type ...
	--- alpha stopping point ---
}

PerformUpdateSection =
{
	Int8  EPerformUpdate
	Int8  TLogUpdateType
	Int32 TUid
	--- beta stopping point ---
	(UpdateContinuationSection | UpdateStateChangeSection)*
	Int8  EEndOfUpdate
	--- alpha stopping point ---
}

RollbackUpdateSection =
{
	Int8  ERollbackUpdate
	Int8  TLogUpdateType
	Int32 TUid
	--- beta stopping point ---
	(UpdateContinuationSection | UpdateStateChangeSection)*
	Int8  EEndOfUpdate
	--- alpha stopping point ---
}

UpdateContinuationSection =
{
	Int8 EUpdateContinuation
	... dependent on update type ...
	--- beta stopping point ---
}

UpdateStateChangeSection =
{
	Int8 EChangeOfUpdateState
	Int8 TState
	--- beta stopping point ---
}

@internalComponent
*/
NONSHARABLE_CLASS(RApsUpdateLog) 
	{
public:
	RApsUpdateLog(RFs& aFs);
	void OpenL();
	void Open(RFile& aFile, TUint aSeekPos);
	void Close();
	RPointerArray<HBufC>& NewRegistrationFiles();
	RPointerArray<HBufC>& DrivesAffected();
	RWriteStream& LogWriteStream();
private:
	RFs& iFs;
	// RPointerArray neccessary for use with CApaAppRegFinder
	RPointerArray<HBufC> iFilesRegistered;
	RPointerArray<HBufC> iDrivesAffected;
	RFileWriteStream iLogWriteStream;
	TFileName iLogFileName;
	};

#endif // APSNNAPPS_H
