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
// This contains CTestTimewBase which is the base class for all
// the Psd Agx suite test steps
// 
//

#ifndef TEST_TIMEW_BASE_H
#define TEST_TIMEW_BASE_H

#include "almmod.h"
#include <TestExecuteStepBase.h>


class CTestTimewBase : public CTestStep
{
public:
	CTestTimewBase();
	~CTestTimewBase();
	virtual enum TVerdict	doTestStepPreambleL();
	virtual enum TVerdict	doTestStepPostambleL();	CAlmModel*				Model();

private:
	CActiveScheduler*	iSchedular;

protected:	CAlmModel*			iModel;};

#endif /* TEST_TIMEW_BASE_H */
