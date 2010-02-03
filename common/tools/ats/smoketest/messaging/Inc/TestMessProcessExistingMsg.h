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
// TestMessProcessExistingMsg.cpp
// This contains CTestMessProcessExistingMsg
// Base class for processing an existing message
// 
//

#if (!defined __TEST_MESS_PROCESS_EXISTING_MSG__)
#define __TEST_MESS_PROCESS_EXISTING_MSG__

#include "TestMessBase.h"

#include <mtclbase.h>

class CTestMessProcessExistingMsg : public CTestMessBase
{
public:
	CTestMessProcessExistingMsg();

	virtual	TVerdict		doTestStepL();

protected:
	virtual	void			ProcessMessageL(CBaseMtm& aBaseMtm, TMsvId aMsgId) = 0;
	virtual	void			PostProcessMessageL();
	TUid					MtmID() const { return iMtmID; }

private:
	TUid			iMtmID;
};

#endif /* __TEST_MESS_PROCESS_EXISTING_MSG__ */
