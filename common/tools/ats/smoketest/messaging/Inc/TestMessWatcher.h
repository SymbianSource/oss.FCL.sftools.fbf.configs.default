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

#if (!defined __TEST_MESS_WATCHER_H__)
#define __TEST_MESS_WATCHER_H__

#include <e32base.h>
#include <watcher.h>

const TInt	KTestUtilsWatcherStarterMin = 5000000;

class CTestMessWatcher : public CActive
{
public:
	static CTestMessWatcher* NewL(TInt aPriority = EPriorityStandard);
	~CTestMessWatcher();

private:
	CTestMessWatcher(TInt aPriority);
	void	RunL();
	void	DoCancel();
	void	ConstructL();

private:
	RTimer						iTimer;
	RProcess					iWatcher;
	TBool						iConstructed;
	TTimeIntervalMicroSeconds32	iTimeLeft;
};

#endif /* __TEST_MESS_WATCHER_H__ */
