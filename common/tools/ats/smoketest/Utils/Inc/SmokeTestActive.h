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
// This contains CTestActive
// 
//

#if (!defined __SMOKETEST_ACTIVE_H__)
#define __SMOKETEST_ACTIVE_H__

#include <e32base.h>

/**
 * Multimedia test timer completion callback
 */
class MTestActiveCallback
	{
public:
	/**
	* Method from which CTestActive informs the user RunL call
	*/
	virtual void	RunL() = 0;
	/**
	* Method from which CTestActive informs the user DoCancel call
	*/
	virtual void	DoCancel() = 0;
	};

/**
 * Test Active Notification class
 *
 */
class CTestActive : public CActive
{
public:
	/**
	* Destructor
	*/
	IMPORT_C virtual ~CTestActive();

	/**
	* Two phase constructor that allocates and constructs
	* a new Active object whos actions are performed by a callback
	*
	* \param aTestActiveCallback object to inform on RunL.
	* \param aPriority priority of active object.
	* \return New Callback active object.
	*/
	IMPORT_C static CTestActive*	NewL(MTestActiveCallback& aTestActiveCallback, TInt aPriority=EPriorityStandard);

	/**
	* Two phase constructor that allocates and constructs
	* a new Active object whos actions are performed by a callback
	*
	* \param aTestActiveCallback object to inform on RunL.
	* \param aPriority priority of active object.
	* \return New Callback active object.
	*/
	IMPORT_C static CTestActive*	NewLC(MTestActiveCallback& aTestActiveCallback, TInt aPriority=EPriorityStandard);

	/**
	* Activate the object
	*/
	IMPORT_C void					Activate();

	/**
	* Active object RunL implementation.
	*
	* Calls the MTestActiveCallback::RunL to inform user that the RunL has been reached.
	*/
	void	RunL();
	/**
	* Active object DoCancel implementation.
	*
	* Calls the MTestActiveCallback::DoCancel to inform user that the DoCancel has been reached.
	*/
	void	DoCancel();

protected:
	/**
	* Protected constructor with timer completion callback and priority.
	*
	* Called by two phase constructor.
	*
	* \param aTestTimerCallback object to inform on timer completion.
	* \param aPriority priority of active object.
	*/
	CTestActive(MTestActiveCallback& aTestActiveCallback, TInt aPriority);

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
	MTestActiveCallback&	iTestActiveCallback;
};

#endif /* __SMOKETEST_ACTIVE_H__ */
