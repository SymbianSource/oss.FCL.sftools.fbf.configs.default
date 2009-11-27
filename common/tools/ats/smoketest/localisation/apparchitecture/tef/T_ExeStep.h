// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



/**
 @file
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __T_EXE_STEP_H__)
#define __T_EXE_STEP_H__

#include "ApparcTestServer.h"

//!  A CT_ExeStep test class. 

/**   Tests Retrieving data from main thread. */

class CT_ExeStep : public CTestStep
	{
public:
	CT_ExeStep();
	~CT_ExeStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
//private:
	void DoThings(const TApaCommandLine& aCommandLine);
	};

_LIT(KT_ExeStep,"T_Exe");

#endif
