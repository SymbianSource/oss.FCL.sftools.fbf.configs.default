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
// This contains CTestAppLoaderServer
// 
//

#ifndef TEST_APP_LOADER_SERVER_H
#define TEST_APP_LOADER_SERVER_H

#include <TestExecuteServerBase.h>

class  CTestAppLoaderServer : public CTestServer
{
public:
	static CTestAppLoaderServer*	NewL();
	~CTestAppLoaderServer();
	virtual CTestStep*				CreateTestStep(const TDesC& aStepName);

private:
	CTestAppLoaderServer();
	void	ConstructL();
};

#endif /* TEST_APP_LOADER_SERVER_H */
