// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SmokeTestTimeOutTimer.h
// This contains CTestTimeOutTimer
// 
//

#include "SmokeTestTimeOutTimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EXPORT_C CTestTimeOutTimer::~CTestTimeOutTimer()
	{
	}

EXPORT_C CTestTimeOutTimer* CTestTimeOutTimer::NewL(MTestTimeOutTimerCallback& aTestTimerCallback, TInt aPriority)
	{
	CTestTimeOutTimer*	self=NewLC(aTestTimerCallback, aPriority);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CTestTimeOutTimer* CTestTimeOutTimer::NewLC(MTestTimeOutTimerCallback& aTestTimerCallback, TInt aPriority)
	{
	CTestTimeOutTimer*	self=new(ELeave) CTestTimeOutTimer(aTestTimerCallback, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestTimeOutTimer::CTestTimeOutTimer(MTestTimeOutTimerCallback& aTestTimerCallback, TInt aPriority)
:	CTimer(aPriority)
,	iTestTimerCallback(aTestTimerCallback)
	{
	}

void CTestTimeOutTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CTestTimeOutTimer::RunL()
	{
	iTestTimerCallback.TimerCompleted();
	};
