// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Common utils used by hybrid plugin tests
// 
//



/**
 @file ctlbsactiveyield.cpp
*/

#include <e32base.h>
#include <e32std.h>

#include <testexecutestepbase.h>


void CheckForObserverEventTestsL(TTimeIntervalMicroSeconds32 aTimeout, CTestStep& aStep)
	{
	class CTimeout : public CTimer
		{
		CTestStep& iStep;

		void RunL()  
	    	{
	    	iStep.SetTestStepError(KErrTimedOut);
	    	CActiveScheduler::Stop();
			}

		CTimeout(CTestStep& aTestStep) : CTimer(EPriorityLow), iStep(aTestStep) { ; }
		
	public:
		static CTimeout* NewL(CTestStep& aTestStep)
			{
			CTimeout* p = new (ELeave) CTimeout(aTestStep);
			CleanupStack::PushL(p);
			p->ConstructL();
			CActiveScheduler::Add(p);
			CleanupStack::Pop(p);
			return p;
			}
		};
		
	CTimeout *pTimeout = CTimeout::NewL(aStep);
	pTimeout->After(aTimeout);	
	
	CActiveScheduler::Start();

	if(pTimeout->iStatus == KRequestPending)
		{
		pTimeout->Cancel();
//		User::WaitForRequest(pTimeout->iStatus);
		}
	
	pTimeout->Deque();
	delete pTimeout;
	
	if(EPass != aStep.TestStepResult())
		{
		User::Leave(aStep.TestStepResult());
		}
	}
	

void ReturnToTestStep()
	{
	CActiveScheduler::Stop();
	}
