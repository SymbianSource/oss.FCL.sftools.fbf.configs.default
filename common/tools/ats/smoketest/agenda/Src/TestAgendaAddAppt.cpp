// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestAgendaAddAppt
// 
//

#include <TestExecuteClient.h>
#include <calentry.h>
#include <calalarm.h> 


#include "TestAgendaAddAppt.h"

_LIT(KCount,		"count");
_LIT(KYear,			"year%d");
_LIT(KMonth,		"month%d");
_LIT(KDay,			"day%d");
_LIT(KHour,			"hour%d");
_LIT(KMinute,		"min%d");
_LIT(KDuration,		"duration%d");
_LIT(KMessage,		"message%d");
_LIT(KAlarm,		"alarm%d");
_LIT(KAlarmSound,	"alarmsound%d");

// constructor
CTestAgendaAddAppt::CTestAgendaAddAppt()
	{
	SetTestStepName(_L("AddAppt"));
	}

// destructor
CTestAgendaAddAppt::~CTestAgendaAddAppt()
	{
	}

// Each test step must supply a implementation for doTestStepL
enum TVerdict CTestAgendaAddAppt::doTestStepL( void )
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("TEST-> ADDING APPOINTMENT ENTRIES"));

	OpenDatabaseL();
	if ( TestStepResult() == EPass )
		{
		TRAPD(r, AddEntriesL());
		if (r!=KErrNone)
			SetTestStepResult(EFail);
		}
	CleanupDatabase();

	// test steps return a result
	return TestStepResult();
	}
	
// Destroy the RPointerArray
void DestroyRPointerArray(TAny* aPtr)
        {
        RPointerArray<CCalEntry>* self = static_cast<RPointerArray<CCalEntry>*> (aPtr);
        self->ResetAndDestroy();
        }


void CTestAgendaAddAppt::AddEntriesL( void )
	{
	TInt	count=1;
	if ( !GetIntFromConfig(ConfigSection(), KCount, count) )
		count=1;

	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	year;
	TInt	month;
	TInt	day;
	TInt	hour;
	TInt	minute;
	TInt	duration;
	TInt	alarm;
	TPtrC	ptrAlarmSound;
	TPtrC	ptrMessage;
	TBuf<KMaxDateStringLength> dateString;
	_LIT(KDateString,"%*E%*D%X%*N%*Y %1 %2 '%3");

	RPointerArray<CCalEntry> array;
    CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &array));

	for (TInt entry=0; entry<count && TestStepResult() == EPass; )
		{
		TTime	today;
		today.HomeTime();

		tempStore.Format(KYear(), ++entry);
		if ( !GetIntFromConfig(ConfigSection(), tempStore, year) )
			year=today.DateTime().Year();

		tempStore.Format(KMonth(), entry);
		if ( !GetIntFromConfig(ConfigSection(), tempStore, month) )
			month=today.DateTime().Month();

		tempStore.Format(KDay(), entry);
		if ( !GetIntFromConfig(ConfigSection(), tempStore, day) )
			day=today.DateTime().Day();
		else
			--day;

		tempStore.Format(KHour(), entry);
		if ( !GetIntFromConfig(ConfigSection(), tempStore, hour) )
			hour=today.DateTime().Hour();

		tempStore.Format(KMinute(), entry);
		if ( !GetIntFromConfig(ConfigSection(), tempStore, minute) )
			minute=0;

		tempStore.Format(KDuration(), entry);
		if ( !GetIntFromConfig(ConfigSection(), tempStore, duration) )
			duration=30;

		tempStore.Format(KMessage(), entry);
		GetStringFromConfig(ConfigSection(), tempStore, ptrMessage);

		TTime		startTime(TDateTime(year, TMonth(month-1+EJanuary), day, hour, minute,0,0));
		startTime.FormatL(dateString,KDateString);
	  	INFO_PRINTF2(_L("Start date is  %S"), &dateString);

		TTime		endTime = startTime + TTimeIntervalMinutes(duration);
		endTime.FormatL(dateString,KDateString);
	  	INFO_PRINTF2(_L("End date is  %S"), &dateString);

		
		HBufC8* uid = HBufC8::NewLC(255);
		TPtr8 uidP = uid->Des();
		uidP.Append(count);
		
		CCalEntry* calEntry = CCalEntry::NewL(CCalEntry::EAppt, uid, CCalEntry::EMethodNone, 0);
		
		CleanupStack::Pop(); //uid
		CleanupStack::PushL(calEntry);
		
		TCalTime calStartTime, calEndTime;
		
		calStartTime.SetTimeLocalL(startTime);
		calEndTime.SetTimeLocalL(endTime);
		
		calEntry->SetStartAndEndTimeL(calStartTime, calEndTime);
		
		tempStore.Format(KAlarm(), entry);
		if ( GetIntFromConfig(ConfigSection(), tempStore, alarm) )
			{
			TTimeIntervalMinutes	currentTime((hour*60) + minute);
			TTimeIntervalMinutes	alarmTime(currentTime.Int());
			
			CCalAlarm* calAlarm = CCalAlarm::NewL();
			CleanupStack::PushL(calAlarm);
			
			calAlarm->SetTimeOffset(alarmTime);
			
			tempStore.Format(KAlarmSound(), entry);
			if ( GetStringFromConfig(ConfigSection(), tempStore, ptrAlarmSound) )
				calAlarm->SetAlarmSoundNameL(ptrAlarmSound);
			else
				calAlarm->SetAlarmSoundNameL(_L("Bells"));
			
			calEntry->SetAlarmL(calAlarm);
			CleanupStack::PopAndDestroy(); //calAlarm
			}
		//Store in the array
		array.AppendL(calEntry);
		
		CleanupStack::Pop(); //calEntry
		}
		INFO_PRINTF1(_L("About to store appointments now"));
		TInt success(0);
		TRAPD(storeError, iCalEntryViewBase->StoreL(array, success));
		INFO_PRINTF2(_L("Store result is %d"), storeError);
		if (success != count && storeError == KErrNone)
		    {
			SetTestStepResult(EFail);
		    } 
		
		CleanupStack::PopAndDestroy(&array);
		
	}
