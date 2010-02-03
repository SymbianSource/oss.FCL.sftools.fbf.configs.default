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
// This contains CTestTimeOutTimer
// 
//

#if (!defined __SMOKETEST_TIMEOUT_TIMER_H_)
#define __SMOKETEST_TIMEOUT_TIMER_H_

#include <e32base.h>

/**
 * Multimedia test timer completion callback
 */
class MTestTimeOutTimerCallback
	{
public:
	/**
	* Method from which CTestTimeOutTimer informs the user of the timer completion.
	*/
	virtual void TimerCompleted() = 0;
	};

/**
 * Multimedia test timer active object.
 *
 * On timer completion MTestTimeOutTimerCallback::TimerCompleted is called
 * to inform the user off the completion
 */
class CTestTimeOutTimer : public CTimer  
{
public:
	/**
	* Destructor
	*/
	IMPORT_C virtual ~CTestTimeOutTimer();

	/**
	* Two phase constructor that allocates and constructs
	* a new Multimedia timer a timer completion callback and priority.
	*
	* \param aTestTimerCallback object to inform on timer completion.
	* \param aPriority priority of active object.
	* \return New Multimedia test timer active object.
	*/
	IMPORT_C static CTestTimeOutTimer*	NewL(MTestTimeOutTimerCallback& aTestTimerCallback, TInt aPriority=EPriorityStandard);

	/**
	* Two phase constructor that allocates and constructs
	* a new Multimedia timer a timer completion callback and priority.
	*
	* \param aTestTimerCallback object to inform on timer completion.
	* \param aPriority priority of active object.
	* \return New Multimedia test timer active object.
	*/
	IMPORT_C static CTestTimeOutTimer*	NewLC(MTestTimeOutTimerCallback& aTestTimerCallback, TInt aPriority=EPriorityStandard);

	/**
	* Active object RunL implementation.
	*
	* Calls the MTestTimeOutTimerCallback::TimerCompleted to inform user that the timer has completed.
	*/
	void	RunL();

protected:
	/**
	* Protected constructor with timer completion callback and priority.
	*
	* Called by two phase constructor.
	*
	* \param aTestTimerCallback object to inform on timer completion.
	* \param aPriority priority of active object.
	*/
	CTestTimeOutTimer(MTestTimeOutTimerCallback& aTestTimerCallback, TInt aPriority);

private:
	/**
	* This is internal and not intended for use.
	*
	* Second phase of two phase constructor.
	*/
	void	ConstructL();

private:
	/**
	* This is internal and not intended for use.
	*/
	MTestTimeOutTimerCallback&	iTestTimerCallback;
};

#endif /* __SMOKETEST_TIMEOUT_TIMER_H_ */
