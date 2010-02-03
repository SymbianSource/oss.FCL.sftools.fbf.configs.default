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
// This contains CTestTimewSetAlarm
// 
//

#ifndef TEST_TIMEW_SET_ALARM_H
#define TEST_TIMEW_SET_ALARM_H

#include "TestTimewBase.h"
#include <asshdalarm.h>

class CTestTimewSetAlarm : public CTestTimewBase
{
public:
	CTestTimewSetAlarm();
	virtual ~CTestTimewSetAlarm();

public:
	virtual enum TVerdict doTestStepL( void );

private:
	void	SetAlarmL( void );
	TASShdAlarm iAlarmData;
};

#endif /* TEST_TIMEW_SET_ALARM_H */
