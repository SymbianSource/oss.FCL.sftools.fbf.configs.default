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
// This contains CTestContactsBase
// 
//

#ifndef TEST_CONTACTS_BASE_H
#define TEST_CONTACTS_BASE_H


#include <cntdb.h>
#include <TestExecuteStepBase.h>
#include <cntitem.h>

#include "TestContact.h"

const TInt KMaxScriptLineLength = 200;

class CTestContactsBase : public CTestStep, protected MTestContactInstrumentation
{
public:
	CTestContactsBase();
	~CTestContactsBase();
	virtual enum TVerdict	doTestStepPreambleL();
	virtual enum TVerdict	doTestStepPostambleL();

protected:
	void				CreateDatabase();
	void				OpenDatabase();
	void				CleanupDatabase();
	void				RaiseInstrumentationEventNotificationL(TInt aEventTag);
	CContactIdArray*	SearchEntriesL();

protected:
	CContactDatabase*			iDatabase;
	CContactItemFieldDef*		iFieldDef;

private:
	CActiveScheduler*			iSchedular;
};

#endif /* TEST_CONTACTS_BASE_H */
