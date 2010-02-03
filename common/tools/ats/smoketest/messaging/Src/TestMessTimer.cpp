// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestMessTimer.h
// This contains CTestMessTimer
// EPOC includes
// 
//

#include "TestMessTimer.h"
#include "TestMessProcessor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestMessTimer* CTestMessTimer::NewL(TTimeIntervalMicroSeconds32 aInterval, CTestMessProcessor& aProcessor)
	{
	CTestMessTimer*	self=new(ELeave) CTestMessTimer(aInterval, aProcessor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTestMessTimer::CTestMessTimer(TTimeIntervalMicroSeconds32 aInterval, CTestMessProcessor& aProcessor)
: CTimer(EPriorityStandard)
, iInterval(aInterval)
, iProcessor(aProcessor)
	{
	}

void CTestMessTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}


CTestMessTimer::~CTestMessTimer()
	{
	}

void CTestMessTimer::RunL()
	{
	iProcessor.DisplayProgress();
	IssueRequest();
	};

void CTestMessTimer::IssueRequest()
	{
	After(iInterval);
	}
