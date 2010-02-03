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

 
#ifndef __T_INTEGRITYREBOOTSUPPORT_STEP_H__
#define __T_INTEGRITYREBOOTSUPPORT_STEP_H__

#include "t_abstractnonnativeappsstep.h"

_LIT(KT_IntegritySupportReboot1Step,"T_IntegritySupportReboot1");

class CT_IntegritySupportReboot1TestStep : public CT_AbstractNonNativeAppsTestStep
	{
public:
	CT_IntegritySupportReboot1TestStep();
	~CT_IntegritySupportReboot1TestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	TInt RunTestCasesL();
	void TestSetupL();
	};

_LIT(KT_IntegritySupportReboot2Step,"T_IntegritySupportReboot2");

class CT_IntegritySupportReboot2TestStep : public CT_AbstractNonNativeAppsTestStep
	{
public:
	CT_IntegritySupportReboot2TestStep();
	~CT_IntegritySupportReboot2TestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	TInt RunTestCasesL();
	void TestClosingSessionL();
	void TestAppsAreIgnoredL();
	void TestUpdatesL();
	void TestRollbackL();
	};

#endif
