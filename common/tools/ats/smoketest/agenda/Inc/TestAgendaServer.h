// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestAgendaServer
// 
//

#ifndef TEST_AGENDA_SERVER_H
#define TEST_AGENDA_SERVER_H

#include <TestExecuteServerBase.h>

class  CTestAgendaServer : public CTestServer
{
public:
	static CTestAgendaServer*	NewL();
	virtual CTestStep*			CreateTestStep(const TDesC& aStepName);

private:
	void	ConstructL();
};

#endif /* TEST_AGENDA_SERVER_H */
