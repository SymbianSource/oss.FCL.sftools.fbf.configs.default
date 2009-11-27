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

#include "APSSCAN.H"
#ifdef USING_ECOM_RECOGS
#include <ecom/ecom.h>
#endif

class CApaFsMonitor::CApaFsNotifier : public CActive
	{
public:
	~CApaFsNotifier();
	static CApaFsNotifier* NewLC(RFs& aFs, const TDesC& aLocation, MApaFsChangeObserver& aObserver);
	void Start(TNotifyType aNotifyType);
private:
	CApaFsNotifier(RFs& aFs, MApaFsChangeObserver& aObserver);
	void DoCancel();
	void RunL();
private:
	RFs& iFs;
	MApaFsChangeObserver& iObserver;
	HBufC* iLocation;
	};

CApaFsMonitor::CApaFsNotifier::~CApaFsNotifier()
	{
	Cancel();
	delete iLocation;
	}

CApaFsMonitor::CApaFsNotifier* CApaFsMonitor::CApaFsNotifier::NewLC(RFs& aFs, const TDesC& aLocation, MApaFsChangeObserver& aObserver)
	{ // static
	CApaFsNotifier* self=new(ELeave) CApaFsNotifier(aFs, aObserver);
	CleanupStack::PushL(self);
	self->iLocation = aLocation.AllocL();
	return self;
	}

CApaFsMonitor::CApaFsNotifier::CApaFsNotifier(RFs& aFs, MApaFsChangeObserver& aObserver)
	: CActive(EPriorityLow), // priority must be higher than that of CApaFsMonitor::iFsTimer, to ensure the RunL of each completed CApaFsNotifier object is executed before the RunL of a completed CApaFsMonitor::iFsTimer
	iFs(aFs),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}


void CApaFsMonitor::CApaFsNotifier::Start(TNotifyType aNotifyType)
	{
	if (iLocation->Length())
		{
		iFs.NotifyChange(aNotifyType, iStatus, *iLocation);
		}
	else
		{
		iFs.NotifyChange(aNotifyType, iStatus);
		}
	SetActive();
	}

void CApaFsMonitor::CApaFsNotifier::DoCancel()
	{
	iFs.NotifyChangeCancel(iStatus);
	}

void CApaFsMonitor::CApaFsNotifier::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver.FsChanged();
		}
	}



// An FsMonitor checks for any changes to the file system
// If a change is detected, a CallBack function is called after a short pause.
// If there are further changes, the CallBack is not called again until after 3 secs have elapsed.

const TInt KApaFsMonitorPause=250000;			// Wait 0.25s before calling CallBack
const TInt KApaFsMonitorPeriod=3000000;			// Don't call CallBack again before 3s have elapsed

//
// Class CApaFsMonitor
//

EXPORT_C CApaFsMonitor::~CApaFsMonitor()
/** Deletes the timer object, file system change notification
active objects and location descriptors. */
	{
	if (iFsTimer)
		{
		iFsTimer->Cancel();
		delete iFsTimer;
		}
	iNotifiers.ResetAndDestroy();
	iNotifiers.Close();
	}

CApaFsMonitor::CApaFsMonitor(RFs& aFs, TCallBack aCallBack)
 	:iFs(aFs),
 	iCallBack(aCallBack)
	{
	}


EXPORT_C CApaFsMonitor* CApaFsMonitor::NewL(RFs& aFs,const TDesC& aLocation, TCallBack aCallBack)
/** Allocates and constructs a file system monitor.

@param aFs A session with the file server.
@param aLocation Optional name of the file or directory to be 
monitored. If the length of the descriptor is zero, the object 
monitors changes to all files and directories in the file system.
Additional locations to monitor may be specified by calling
AddLocationL.
@param aCallBack The callback function.
@return The file system monitor.
@see CApaFsMonitor::AddLocationL() */
	{
	CApaFsMonitor* self=new(ELeave) CApaFsMonitor(aFs, aCallBack);
	CleanupStack::PushL(self);
	self->iFsTimer=CPeriodic::NewL(CActive::EPriorityIdle);
	self->AddLocationL(aLocation);
	CleanupStack::Pop(self);
	return self;
	}

/** Adds an additional file system location to monitor.

@param aLocation Name of the file or directory to be monitored. */
EXPORT_C void CApaFsMonitor::AddLocationL(const TDesC& aLocation)
	{
	CApaFsNotifier* const notifier = CApaFsNotifier::NewLC(iFs, aLocation, *this);
	User::LeaveIfError(iNotifiers.Append(notifier));
	CleanupStack::Pop(notifier);
	}

/** Cancels all file system notification active objects owned by
this monitor.

A call to this function may be followed immediately by a call
to Start() to change the notification type.

@see CApaFsMonitor::Start() */
EXPORT_C void CApaFsMonitor::Cancel()
	{
	const TInt notifierCount = iNotifiers.Count();
	for (TInt ii=0; ii < notifierCount; ii++)
		{
		iNotifiers[ii]->Cancel();
		}
	}

TBool CApaFsMonitor::AnyNotificationImpending() const
	{
	for (TInt i=iNotifiers.Count()-1; i>=0; --i)
		{
		if (iNotifiers[i]->iStatus!=KRequestPending)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

void CApaFsMonitor::FsChanged()
	{
	iFsHasChanged=ETrue;
	if (!iIsBlocked && !iFsTimer->IsActive())
		iFsTimer->Start(KApaFsMonitorPause,KApaFsMonitorPeriod,TCallBack(TimerCallBack,this));
	}

EXPORT_C void CApaFsMonitor::Start(TNotifyType aNotifyType)
/** Sets the type of notification required and starts a file system notification
active object for each location being monitored.

You can either request notification of changes to all files in the file system, 
or to specific files, depending on the aLocation parameter specified in the NewL() 
function.

@param aNotifyType A set of flags that indicate what kinds of change should cause 
notifications. */
	{
	iNotifyType=aNotifyType;

	DoStart();
	}

void CApaFsMonitor::DoStart()
	{
	const TInt notifierCount = iNotifiers.Count();
	for (TInt ii=0; ii < notifierCount; ii++)
		{
		CApaFsNotifier* const notifier = iNotifiers[ii];
		if (!notifier->IsActive())
			{
			notifier->Start(iNotifyType);
			}
		}
	}

EXPORT_C TNotifyType CApaFsMonitor::NotifyType() const
/** Gets the type of notification as passed to Start(). 

If Start() has not yet been called, the type of notification is undefined.

@return Flags that indicate what kinds of change should cause notifications. */
	{
	return iNotifyType;
	}

EXPORT_C void CApaFsMonitor::SetBlocked(TBool aIsBlocked)
/** Blocks or unblocks file system monitoring. 

While in a blocked state, the callback never gets called. If monitoring is 
unblocked using this function, and a change has occurred when in the blocked 
state then the timer is started, causing the callback to be called initially 
after 0.25 seconds, followed by subsequent delays of 3 seconds.

@param aIsBlocked True to block callbacks, false to unblock them. */
	{
	iIsBlocked=aIsBlocked;
	if (iIsBlocked==EFalse)		// If end of a blocked period, notify if a change was detected
		{
		if (iFsHasChanged && !iFsTimer->IsActive())
			{
			iFsTimer->Start(KApaFsMonitorPause,KApaFsMonitorPeriod,TCallBack(TimerCallBack,this));
			}
		}
	}

TInt CApaFsMonitor::TimerCallBack(TAny* aObject)
	{
	CApaFsMonitor* self=(CApaFsMonitor*)aObject;
	if (self->iFsHasChanged && !self->iIsBlocked)
		{
		self->iFsHasChanged=EFalse;
		self->DoStart();
		self->iCallBack.CallBack();		// Should not leave, but if it does, it's OK
		}
	else
		self->iFsTimer->Cancel();
	return KErrNone;
	}


#ifdef USING_ECOM_RECOGS
//
// CApaEComMonitor class
//
CApaEComMonitor::CApaEComMonitor(TCallBack aCallBack)
	: CActive(EPriorityIdle), iCallBack (aCallBack)
	{
	CActiveScheduler::Add(this);
	}

CApaEComMonitor::~CApaEComMonitor()
	{
	Cancel();
	delete iEComTimer;
	if (iEComSession)
		{
		iEComSession->Close();
		}
	}

CApaEComMonitor* CApaEComMonitor::NewL(TCallBack aCallBack)
	{
	CApaEComMonitor* self=new(ELeave) CApaEComMonitor(aCallBack);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CApaEComMonitor::ConstructL()
	{
	iEComSession = &(REComSession::OpenL());
	iEComTimer = CPeriodic::NewL(CActive::EPriorityIdle);
	}

void CApaEComMonitor::DoCancel()
	{
	iEComTimer->Cancel();
	iEComSession->CancelNotifyOnChange(iStatus);
	}

void CApaEComMonitor::Start ()
	{
	DoStart();
	}

void CApaEComMonitor::DoStart()
	{
	iEComSession->NotifyOnChange (iStatus);
	SetActive();
	}

//callback to to call the ecomrecognizer scanning routine upon notification from ECom
TInt CApaEComMonitor::TimerCallBack(TAny* aObject)
	{
	CApaEComMonitor* self=(CApaEComMonitor*)aObject;
	if (self->iEComHasChanged)
		{
		self->iEComHasChanged=EFalse;
		self->DoStart();
		self->iCallBack.CallBack();		// Should not leave, but if it does, it's OK
		}
	else
		self->iEComTimer->Cancel();
	return KErrNone;
	}

const TInt KApaEComMonitorPause=250000;			// Wait 0.25s before calling CallBack
const TInt KApaEComMonitorPeriod=3000000;			// Don't call CallBack again before 3s have elapsed

void CApaEComMonitor::RunL()
	{
	if (iStatus == KErrNone)
		{
		iEComHasChanged = ETrue;
		if  (!iEComTimer->IsActive())
			{
			iEComTimer->Start(KApaEComMonitorPause,KApaEComMonitorPeriod,TCallBack(TimerCallBack,this));
			}
		}
	}

#endif
