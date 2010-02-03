// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "apgnotif.h"
#include "APGSTD.H"

EXPORT_C CApaAppListNotifier::~CApaAppListNotifier()
/** The destructor calls Cancel() and closes the session with the server. 

@see CActive::Cancel() */
	{
	Cancel();
	iLsSession.Close();
	}


EXPORT_C CApaAppListNotifier* CApaAppListNotifier::NewL(MApaAppListServObserver* aObserver, TPriority aPriority)
/** Allocates and constructs an application list change notifier. 

It creates a session with the application architecture server (RApaLsSession), 
issues the notification request to the server and adds itself to the active scheduler.

@param aObserver Observer whose HandleAppListEvent() function is called when an 
application is added or deleted.
@param aPriority The active object priority.
@return The application list change notifier. */
	{
	__ASSERT_DEBUG(aObserver!=NULL,Panic(EDPanicInvalidObserver));
	CApaAppListNotifier* self=new(ELeave) CApaAppListNotifier(*aObserver, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CApaAppListNotifier::CApaAppListNotifier(MApaAppListServObserver& aObserver, TPriority aPriority)
	:CActive(aPriority),
	iObserver(aObserver)
	{	
	}

void CApaAppListNotifier::ConstructL()
	{
	User::LeaveIfError(iLsSession.Connect());
	iLsSession.SetNotify(EFalse, iStatus);
	CActiveScheduler::Add(this);
	SetActive();
	}

void CApaAppListNotifier::DoCancel()
	{
	iLsSession.CancelNotify();
	}

void CApaAppListNotifier::RunL()
	{
	TInt status=iStatus.Int();
	User::LeaveIfError(status);
	iLsSession.SetNotify(EFalse, iStatus); // requeue before handling in case the handling changes things
	SetActive();
	iObserver.HandleAppListEvent(status);
	}

//
// MApaAppListServObserver
//
/** Constructor for MApaAppListServObserver */
EXPORT_C MApaAppListServObserver::MApaAppListServObserver()
	{
	}

/** Reserved for future use */	
EXPORT_C void MApaAppListServObserver::MApaAppListServObserver_Reserved1()
	{
	}

/** Reserved for future use */	
EXPORT_C void MApaAppListServObserver::MApaAppListServObserver_Reserved2()
	{
	}
