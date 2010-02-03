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

#if (!defined __T_SERVICES_STEP_H__)
#define __T_SERVICES_STEP_H__

#include "testableapalssession.h"
#include "appfwk_test_AppUi.h"

_LIT(KT_ServicesStep,"T_Services");

class CT_ServicesTestStep : public CTestStep
	{
public:
	CT_ServicesTestStep();
	~CT_ServicesTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	TInt RunTestCasesL();
	void TestServiceDiscovery1L();
	void TestServiceDiscovery2L();
	void TestServiceDiscovery3L();
	void TestServiceDiscovery4L();
	void TestServiceDiscovery5L();
	void TestServiceDiscovery6();
	void TestServiceDiscovery7L();
	void TestServiceDiscovery8L();
	void TestServiceDiscovery9();
	void TestServiceDiscovery10L();
	void TestServiceDiscovery11L();
	void TestServiceDiscovery12L();
	void TestServiceDiscovery13L();

	void TestServiceDiscovery17L();
	void TestServiceDiscovery18L();
	void TestServiceDiscovery19();
	void TestServiceDiscovery20L();
	void TestServiceDiscovery21L();
	void TestServiceDiscovery22();
	void TestServiceDiscovery23();
	void TestServiceDiscovery24();
	void TestServiceDiscovery25();
	void TestServiceDiscovery26();
	void TestServiceDiscovery27();
	void TestServiceDiscovery28L();
	void TestOpenService1L();
	void TestCtrlPnlAppL();
	void TestPrioritySystemL();
			
private:
	RTestableApaLsSession iApaLsSession;
	};

#endif
