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


#if (!defined __T_STARTDOCSTEP_H__)
#define __T_STARTDOCSTEP_H__

#include <TestExecuteStepBase.h>

class CT_StartDocStep : public CTestStep
	{
public:
	virtual TVerdict doTestStepL();
private:
	void TestStartDocL(RApaLsSession& aLs);
	};

_LIT(KT_StartDocStep, "T_StartDoc");
#endif
