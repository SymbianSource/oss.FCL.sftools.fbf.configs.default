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
#include "almmod.h"

//
//	class CModelResponder
//

CModelResponder::CModelResponder(CModelBase& aModel,TInt aPriority)
//
//	Base class constructor
//
	: CActive(aPriority), iModel(&aModel)
	{
	}

void CModelResponder::NotifyModel(TInt aStatus)
//
//	Funnel all response to the engine
//
	{
	iModel->ProcessResponder(aStatus);
	}

void CModelResponder::RunL()
//
//	Handle changes in the servers
//
	{
//	Reset the status request before notifiying the view
//	so that it can catch anything untoward that the view does
	TInt v=iStatus.Int();
	Start();
	NotifyModel(v);
	}

void CModelResponder::Start()
//
//	Start the response
//
	{
	if (!IsActive())
		{
		SetActive();
		DoStart();
		}
	}

//
//	class CAlmResponder
//

CAlmResponder::CAlmResponder(RASCliSession& aSvr,CAlmModel& aModel,TInt aResponderPriority)
//
//	Protected constructor
//
	: CModelResponder(aModel,aResponderPriority),iAlmSvr(&aSvr)
	{
	}

void CAlmResponder::DoStart()
//
//	Start a repsonse
//
	{
	iAlmSvr->NotifyChange(iStatus, iAlarmId);
	}

void CAlmResponder::DoCancel()
//
//	Stop the response
//
	{
	if	(iStatus == KRequestPending)
		iAlmSvr->NotifyChangeCancel();
	}
