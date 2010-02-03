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
// TestAgendaCreateDB.h
// This contains CTestAgendaAddAppt
// 
//

#ifndef TEST_AGENDA_ADD_APPT_H
#define TEST_AGENDA_ADD_APPT_H

#include "TestAgendaBase.h"

class CTestAgendaAddAppt : public CTestAgendaBase
{
public:
	CTestAgendaAddAppt();
	virtual ~CTestAgendaAddAppt();

public:
	virtual enum TVerdict doTestStepL( void );

private:
	void	AddEntriesL( void );
};

#endif /* TEST_AGENDA_ADD_APPT_H */
