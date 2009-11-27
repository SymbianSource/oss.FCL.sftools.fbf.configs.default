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
// AppArc recognizer and application scanning
// 
//

#if !defined(__APSSCAN_H__)
#define __APSSCAN_H__

#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif

class MApaFsChangeObserver
/** An interface for notifying a file system observer when
a change has been made to the file system.
This interface is intended for use only by CApaFsMonitor.

@internalComponent
*/
	{
public:
	virtual void FsChanged()=0;
	};

class CApaFsMonitor : public CBase, MApaFsChangeObserver
/** Monitors changes in the file system.

If a change is detected, the callback function supplied by the user of the class 
is called after a 0.25 second delay. If there are further changes, the callback 
is not called again until 3 seconds have elapsed. It uses RFs::NotifyChange() 
to request notifications.

@see RFs::NotifyChange()

@publishedPartner 
@released */
	{
public:
	IMPORT_C ~CApaFsMonitor();
	IMPORT_C static CApaFsMonitor* NewL(RFs& aFs, const TDesC& aLocation, TCallBack aCallBack);
	IMPORT_C void SetBlocked(TBool aIsBlocked);
	IMPORT_C void Start(TNotifyType aNotifyType);
	IMPORT_C TNotifyType NotifyType() const;
	IMPORT_C void AddLocationL(const TDesC& aLocation);
	IMPORT_C void Cancel();
	TBool AnyNotificationImpending() const;
private:

	CApaFsMonitor(RFs& aFs, TCallBack aCallBack);
	static TInt TimerCallBack(TAny* aObject);
	void DoStart();

	// from MApaFsChangeObserver
	void FsChanged();
	class CApaFsNotifier;
private:
	RFs& iFs;
	TNotifyType iNotifyType;
	TCallBack iCallBack;
	CPeriodic* iFsTimer;
	TBool iFsHasChanged;
	TBool iIsBlocked;
	RPointerArray<CApaFsNotifier> iNotifiers;
	};

#if !defined(__WINC__)
class REComSession;
/** 
CApaEComMonitor

A low priority (EPriorityIdle) active object which monitors changes 
in the Ecom plugin.

@see REComSession::NotifyOnChange()

@internalAll 
*/ 

class CApaEComMonitor : public CActive
	{
public:
	 ~CApaEComMonitor();
	 static CApaEComMonitor* NewL(TCallBack aCallBack);
	 void Start();
private:
	CApaEComMonitor(TCallBack aCallBack);
	void ConstructL ();
	static TInt TimerCallBack(TAny* aObject);
	void DoStart();
	void DoCancel();
	void RunL();
private:
	TCallBack iCallBack;
	CPeriodic* iEComTimer;
	REComSession* iEComSession;
	TBool iEComHasChanged;
	};

#endif


#endif
