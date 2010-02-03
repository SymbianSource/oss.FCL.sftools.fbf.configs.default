// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



/**
 @file 
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __T_OOM_STEP_H__)
#define __T_OOM_STEP_H__

#include "ApparcTestServer.h"

//!  A CT_OOMStep test class. 

/**  Performs the Out of Memory Tests. */

class CT_OOMStep : public CTestStep
	{
public:
	CT_OOMStep();
	~CT_OOMStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void DoStepTests();

//private:
	CApaDocument* CreateTestDocL(CApaProcess* aProcess);
	void CreateTestDocFileL();
	void TestOOMConstructionL();
	void TestDoorOOML();
	void DoTestsInCallBackL();
	void setup();
	
private:
	RFs iFs;
	};

class CT_OOMStepCallBack : public CBase
	{
public:
	CT_OOMStepCallBack(CT_OOMStep* aTestStep);
	~CT_OOMStepCallBack();
	static TInt CallBack(TAny* aThis);
	CT_OOMStep* iTestStep;
	};



_LIT(KT_OOMStep,"T_OOM");

#endif
