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
// This contains CTestAppLoaderBase which is the base class for all
// the Psd Agx suite test steps
// 
//

#ifndef TEST_APP_LOADER_BASE_H
#define TEST_APP_LOADER_BASE_H

#include <TestExecuteStepBase.h>

class CTestAppLoaderBase : public CTestStep
	{
public:
	CTestAppLoaderBase();
	~CTestAppLoaderBase();

	virtual enum TVerdict	doTestStepPreambleL();
	virtual enum TVerdict	doTestStepPostambleL();
	};

#endif /* TEST_APP_LOADER_BASE_H */
