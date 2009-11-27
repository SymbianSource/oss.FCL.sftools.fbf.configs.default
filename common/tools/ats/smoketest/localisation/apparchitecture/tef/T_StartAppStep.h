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
 @test
 @internalComponent - Internal Symbian test code 
*/

#if (!defined __T_DATATYPEMAPPINGS_STEP_H__)
#define __T_DATATYPEMAPPINGS_STEP_H__

#include "testableapalssession.h"
#include "appfwk_test_AppUi.h"

_LIT(KT_StartAppStep,"T_StartApp");

class CT_StartAppTestStep : public CTestStep
	{
public:
	CT_StartAppTestStep();
	~CT_StartAppTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	void RunTestCasesL();
	void TestStartApp1L();
	void TestStartApp2L();
	void TestStartApp3L();
	void TestStartApp4L();
	void TestStartApp5L();
	void TestStartApp6L();
	void TestStartApp7L();
	void TestStartApp8L();
	void TestStartApp9L();
	void TestInsertDataTypeL();
	void TestAppForDataTypeL();
	void TestDeleteDataTypeL();
	void TestIpcFuzzL();
	
private:
	TFileName SearchAndReturnCompleteFileName(const TDesC& aFileName);
	
private:
	RTestableApaLsSession iApaLsSession;
	TFileName iStartAppTestsFilename;
	TUid iStartAppTestsUid;
	};

#endif
