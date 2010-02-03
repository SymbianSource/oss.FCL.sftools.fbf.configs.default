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
// SmokeTestTestActive.h
// This contains CTestActive
// 
//

#include "SmokeTestActive.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EXPORT_C CTestActive::~CTestActive()
	{
	}

EXPORT_C CTestActive* CTestActive::NewL(MTestActiveCallback& aTestActiveCallback, TInt aPriority)
	{
	CTestActive*	self=NewLC(aTestActiveCallback, aPriority);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CTestActive* CTestActive::NewLC(MTestActiveCallback& aTestActiveCallback, TInt aPriority)
	{
	CTestActive*	self=new(ELeave) CTestActive(aTestActiveCallback, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C void CTestActive::Activate()
	{
	SetActive();
	}

CTestActive::CTestActive(MTestActiveCallback& aTestActiveCallback, TInt aPriority)
:	CActive(aPriority)
,	iTestActiveCallback(aTestActiveCallback)
	{
	}

void CTestActive::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

void CTestActive::RunL()
	{
	iTestActiveCallback.RunL();
	}

void CTestActive::DoCancel()
	{
	iTestActiveCallback.DoCancel();
	}
