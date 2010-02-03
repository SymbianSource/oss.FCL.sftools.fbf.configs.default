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
// This contains CTestMessTimer
// 
//

#if (!defined __TEST_MESS_TIMER_H_)
#define __TEST_MESS_TIMER_H_

#include <e32base.h>

class CTestMessProcessor;

class CTestMessTimer : public CTimer  
{
public:
	virtual ~CTestMessTimer();
	static CTestMessTimer*	NewL(TTimeIntervalMicroSeconds32 aInterval, CTestMessProcessor& aProcessor);

	void	RunL();

	void	IssueRequest();

protected:
	CTestMessTimer(TTimeIntervalMicroSeconds32 aInterval, CTestMessProcessor& aProcessor);
	void	ConstructL();

private:
	TTimeIntervalMicroSeconds32	iInterval;
	CTestMessProcessor&			iProcessor;
};

#endif /* __TEST_MESS_TIMER_H_ */
