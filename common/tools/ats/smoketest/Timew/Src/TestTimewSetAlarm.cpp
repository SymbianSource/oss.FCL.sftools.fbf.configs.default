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

#include <TestExecuteClient.h>

#include "TestTimewSetAlarm.h"


_LIT(KHour,	"hour");
_LIT(KMinute,"minute");
_LIT(KMessage,"message");
_LIT(KBells,"bells");


// constructor
CTestTimewSetAlarm::CTestTimewSetAlarm()
	{
	SetTestStepName(_L("SetAlarm"));
	}

// destructor
CTestTimewSetAlarm::~CTestTimewSetAlarm()
	{
	}

// Each test step must supply a implementation for doTestStepL
enum TVerdict CTestTimewSetAlarm::doTestStepL( void )
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("TEST-> SETTING AN ALARM"));
		TRAPD(r, SetAlarmL());

	if (r!=KErrNone)
		SetTestStepResult(EFail);

	// test steps return a result
	return TestStepResult();
	}

void CTestTimewSetAlarm::SetAlarmL( void )
	{
	TInt	hour;
	TInt	minute;
	TPtrC	ptrMessage;

	TTime alarmTime;
	alarmTime.HomeTime();

	if ( !GetIntFromConfig(ConfigSection(), KHour(), hour) )
		hour=alarmTime.DateTime().Hour();

	if ( !GetIntFromConfig(ConfigSection(), KMinute(), minute) )
		minute=0;

	GetStringFromConfig(ConfigSection(), KMessage(), ptrMessage);


	TDateTime dateTime=alarmTime.DateTime();
	dateTime.SetHour(hour);
	dateTime.SetMinute(minute);
	dateTime.SetSecond(0);
	dateTime.SetMicroSecond(0);
	iAlarmData.NextDueTime()=dateTime;
	iAlarmData.RepeatDefinition()=(TAlarmRepeatDefinition)EAlarmRepeatDefintionRepeatNext24Hours;
	iAlarmData.Message() = ptrMessage;
	iAlarmData.SoundName()=KBells();
	iModel = CAlmModel::NewL(NULL,KPriorityAlarmResponder);
	Model()->ClockAlarmSet(0,iAlarmData);
	delete iModel;
	}
