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

#if (!defined __T_MRU_STEP_H__)
#define __T_MRU_STEP_H__

#include "ApparcTestServer.h"

//!  A CT_MruStep test class. 

/**  Tests MRU list. */

class CT_MruStep : public CTestStep
	{
public:
	CT_MruStep();
	~CT_MruStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void TestMRUL();
//private:
	void DoTestsInScheldulerLoopL();

	void setup();
private:
	RFs iFs;
	};



class CT_MruTestCallBackWrapper : public CBase
	{
public:
	CT_MruTestCallBackWrapper(CT_MruStep* aTestStep);
	~CT_MruTestCallBackWrapper();
	static TInt CallBack(TAny* aThis);
private:
	CT_MruStep* iTestStep;
	};

_LIT(KT_MruStep,"T_Mru");

#endif
