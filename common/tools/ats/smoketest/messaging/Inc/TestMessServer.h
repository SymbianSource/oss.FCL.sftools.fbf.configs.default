// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestMessServer
// 
//

#if (!defined __TEST_MESS_SERVER_H_)
#define __TEST_MESS_SERVER_H_

#include "TestMessWatcher.h"
#include "TestMessSharedData.h"

#include <TestExecuteServerBase.h>

class CMsvSession;

class  CTestMessServer : public CTestServer
{
public:
	static CTestMessServer*	NewL();
	virtual CTestStep*		CreateTestStep(const TDesC& aStepName);
	~CTestMessServer();
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
protected:
	CTestMessServer();
	void	ConstructL(const TDesC& aName);

private:

private:
	CTestMessSharedData*	iSharedData;
};

#endif /* __TEST_MESS_SERVER_H_ */
