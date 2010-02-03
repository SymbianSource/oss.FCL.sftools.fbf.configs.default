// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestMessWatcher which starts the message watchers
// 
//

#include "TestMessWatcher.h"

_LIT(KWatcherExe, "z:\\system\\libs\\watcher.exe");

CTestMessWatcher* CTestMessWatcher::NewL(TInt aPriority)
	{
	CTestMessWatcher* self = new (ELeave) CTestMessWatcher(aPriority);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop(self);
	return self;
	}

CTestMessWatcher::~CTestMessWatcher()
	{
	Cancel();
	iTimer.Close();
	if (iConstructed)
		iWatcher.Kill(KErrNone);

	iWatcher.Close();
	}

CTestMessWatcher::CTestMessWatcher(TInt aPriority)
:	CActive(aPriority)
,	iConstructed(EFalse)
	{
	CActiveScheduler::Add(this);
	}

void CTestMessWatcher::ConstructL()
	{
	TInt	ret=iWatcher.Create(KWatcherExe, KNullDesC);
	switch ( ret )
		{
	case KErrNone:
		iWatcher.Resume();
		iConstructed = ETrue;
	case KErrAlreadyExists:
		break;
	default:
		User::Leave(ret);
		}
	}

void CTestMessWatcher::RunL()
	{
	}

void CTestMessWatcher::DoCancel()
	{
	iTimer.Cancel();
	iStatus=KErrCancel;
	}
