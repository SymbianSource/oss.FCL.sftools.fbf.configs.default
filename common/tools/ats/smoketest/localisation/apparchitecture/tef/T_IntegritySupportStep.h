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

 
#ifndef __T_INTEGRITYSUPPORT_STEP_H__
#define __T_INTEGRITYSUPPORT_STEP_H__

#include "t_abstractnonnativeappsstep.h"

_LIT(KT_IntegritySupportStep,"T_IntegritySupport");

class CT_IntegritySupportTestStep : public CT_AbstractNonNativeAppsTestStep
	{
public:
	CT_IntegritySupportTestStep();
	~CT_IntegritySupportTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	TInt RunTestCasesL();
	void TestNormalInstallation1L();
	void TestManualRollback1L();
	void TestRollbackOnError1L();
	void TestRollbackOnSessionCloseL();
	void TestNormalRemoval1L();
	void TestManualRollback2L();
	void TestRollbackWithoutPrepareL();
	void TestRegisterWithoutPrepareL();
	void TestCallingPrepareTwiceL();
	void TestCallingPrepareFromTwoSessionsL();
	void TestReregisterApplicationL();
	void TestDoubleInstallFailsL();
	void TestRollbackOnFailedUpdateStepL();
	};
	
#endif
