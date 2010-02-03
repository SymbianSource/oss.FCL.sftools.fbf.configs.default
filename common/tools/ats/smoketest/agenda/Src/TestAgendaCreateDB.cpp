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
// This contains CTestAgendaCreateDB
// 
//

#include <TestExecuteLog.h>

#include "TestAgendaCreateDB.h"

// constructor
CTestAgendaCreateDB::CTestAgendaCreateDB()
	{
	SetTestStepName(_L("CreateDB"));
	}

// destructor
CTestAgendaCreateDB::~CTestAgendaCreateDB()
	{
	}

// Each test step must supply a implementation for doTestStepL
enum TVerdict CTestAgendaCreateDB::doTestStepL( void )
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("TEST-> CREATING OUR AGENDA DATABASE"));

	CreateDatabaseL();

	// test steps return a result
	return TestStepResult();
	}
