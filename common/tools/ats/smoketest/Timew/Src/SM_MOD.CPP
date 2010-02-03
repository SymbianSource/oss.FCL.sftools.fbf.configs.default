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

#include "sm_std.h"

CModelBase::CModelBase()
//
//	Protected constructor
//
	{
	}

CModelBase::~CModelBase()
//
//	destructor
//
	{
	delete(iResponder);
	}

void CModelBase::Notify(TInt aNotification)
//
//	Funnel for all calls to the notifier
//
	{
	if (!iDoingViewNotify && iObserver)
		DoObserverNotify(aNotification);
	}

void CModelBase::DoObserverNotify(TInt aNotification)
//
//	Single function that actually calls the view notifier
//
	{
	__ASSERT_DEBUG(iObserver,Fault(EModelfNoNotifier));
	iDoingViewNotify=ETrue;
	iObserver->HandleUpdate(aNotification);
	iDoingViewNotify=EFalse;
	}

void CModelBase::SetResponderActive(CModelResponder* aResponder)
//
//	Set the engines responder
//
	{
	iResponder=aResponder;
	if (iResponder)
		CActiveScheduler::Add(iResponder);
	}

void CModelBase::Start()
//
//	Start the responder
//	The engine requires that an active scheduler be installed
//
	{
	iResponder->Start();
	}

void CModelBase::Stop()
//
//	Stop the responder and the engine
//
	{
	if (iResponder)
		iResponder->Cancel();
	}

void CModelBase::SetTimewModelObserver(MTimewModelObserver *aObserver)
//
//	Register the observer of the engine
//
	{
	iObserver=aObserver;
	}
