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
// This contains CTestSuiteContacts
// 
//

#ifndef TEST_CONTACTS_ADD_ENTRIES_H
#define TEST_CONTACTS_ADD_ENTRIES_H

#include "TestContactsBase.h"

class CTestContactsAddEntries : public CTestContactsBase
{
public:
	CTestContactsAddEntries();
	virtual ~CTestContactsAddEntries();

	virtual enum TVerdict doTestStepL( void );

private:
	void	AddEntriesL( void );
};

#endif TEST_CONTACTS_ADD_ENTRIES_H
