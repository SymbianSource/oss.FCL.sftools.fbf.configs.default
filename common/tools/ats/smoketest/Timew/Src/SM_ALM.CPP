// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "almmod.h"
#include "sm_std.h"
#include <ecom.h>

//
//	class CAlmModel
//

CAlmModel::CAlmModel()
//
//	Constructor
//
	{
	}

CAlmModel::~CAlmModel()
//
//	Destructor
//
	{
	Stop();
	iClockAlarms.ResetAndDestroy();
	iClockAlarms.Close();
	iReviewAlarmIds.Close();
	iNextAlarmIds.Close();
	iAlmSvr.Close();
 	REComSession::FinalClose();
	}

void CAlmModel::ConstructL(MTimewModelObserver* aObserver,TInt aResponderPriority)
//
//	Construct the alarm engine components
//
	{
	SetTimewModelObserver(aObserver);
	//
	iReviewAlarmIds = RArray<TAlarmId>(KArrayOfAlarmIdsGranularity);
	iNextAlarmIds = RArray<TAlarmId>(KArrayOfAlarmIdsGranularity);
	//
	const TInt r = iAlmSvr.Connect();
	User::LeaveIfError(r);
	//
	TInt i;
	iClockAlarms = RPointerArray<TASShdAlarm>(KMaxClockAlarms);
	for(i=0; i<KMaxClockAlarms; i++)
		{
		TASShdAlarm* blankAlarm = new(ELeave) TASShdAlarm();
		CleanupStack::PushL(blankAlarm);
		User::LeaveIfError(iClockAlarms.Append(blankAlarm));
		CleanupStack::Pop(blankAlarm);
		}

	// Identify any clock alarms in the alarm server and put them
	// into the slots.

	// Fetch
	RArray<TAlarmId> clockIds(KMaxClockAlarms);
	CleanupClosePushL(clockIds);
	iAlmSvr.GetAlarmIdListForCategoryL(KASCliCategoryClock, clockIds);

	// Put them in the slots - we can only display KMaxClockAlarms alarms
	// so any beyond that are ignored.
	TInt slotsUsedSoFar = 0;
	const TInt count = Min(KMaxClockAlarms, clockIds.Count());
	for(i=0; i<count; i++)
		{
		TASShdAlarm* alarm = iClockAlarms[slotsUsedSoFar];
		User::LeaveIfError(iAlmSvr.GetAlarmDetails(clockIds[i], *alarm));

		// Only show alarms which haven't yet notified
		if	(alarm->State() != EAlarmStateNotified)
			{
			// If we don't do this, then the alarm slot is treated
			// as available.
			++slotsUsedSoFar;
			alarm->ClientFlags().Set(EClockAlarmFlagsIsSet);
			}
		}
	CleanupStack::PopAndDestroy(&clockIds);
	//
	CAlmResponder* pR=new(ELeave) CAlmResponder(iAlmSvr,*this,aResponderPriority);
	SetResponderActive(pR);
	}

CAlmModel* CAlmModel::NewL(MTimewModelObserver* aObserver,TInt aResponderPriority)
//
//	Create a new alarm engine
//
	{
	CAlmModel* model=new(ELeave) CAlmModel;
	CleanupStack::PushL(model);
	model->ConstructL(aObserver,aResponderPriority);
	CleanupStack::Pop(model);
	return(model);
	}

void CAlmModel::ProcessResponder(TInt aStatus)
//
//	Interpret responder status
//
	{
	// FIX
	if (aStatus!=KErrCancel && aStatus!=KErrNone)
		Notify(aStatus);
	else
		Notify(0);
	}

TInt CAlmModel::AlarmQuietPeriodMinutes() const
//
//	Return the time that sound is delayed until
//
	{
	TTime timeNow;
	timeNow.HomeTime();
	TTime timeQuiet=AlarmQuietPeriodEnd();
	TInt quietMinutes;
	if (timeQuiet<=timeNow || timeQuiet==Time::NullTTime())
		quietMinutes=0;
	else
		{
		TTimeIntervalMinutes minutes;
		timeQuiet.MinutesFrom(timeNow,minutes);
		quietMinutes=minutes.Int()+1;
		}
	return(quietMinutes);
	}

TInt CAlmModel::AlarmQuietPeriodSet(TInt aMinutes)
//
//	Delay alarm sounds for a given number of minutes
//
	{
	TInt ret = KErrNone;
	//
	if	(!aMinutes)
		ret = iAlmSvr.CancelAlarmSilence();
	else
		ret = iAlmSvr.SetAlarmSoundsSilentFor(TTimeIntervalMinutes(aMinutes));
	//
	return ret;
	}

TInt CAlmModel::ClockAlarmEnable(TInt aAlarmIndex, TTimeWClockAlarmState aClockAlarmState)
//
//	Enable or disable a clock alarm
//
	{
	__ASSERT_DEBUG(aAlarmIndex < KMaxClockAlarms, Panic(EModelOutOfRange));
	//
	TAlarmStatus newStatus;
	switch(aClockAlarmState)
		{
	case ETimeWClockAlarmStateNotSet:
		return ClockAlarmDelete(aAlarmIndex);

	case ETimeWClockAlarmStateSet:
		newStatus = EAlarmStatusEnabled;
		break;

	default:
	case ETimeWClockAlarmStateDisabled:
		newStatus = EAlarmStatusDisabled;
		break;
		}

	TASShdAlarm* alarm = iClockAlarms[aAlarmIndex];
	__ASSERT_ALWAYS(alarm->Id() != KNullAlarmId, Fault(EModelfTryingToChangeStateOfNullClockAlarm));
	TInt error = iAlmSvr.SetAlarmStatus(alarm->Id(), newStatus);
	if	(error != KErrNone)
		return error;

	// Refetch alarm from server (will update status)
	error = iAlmSvr.GetAlarmDetails(alarm->Id(), *alarm);
	if	(error == KErrNone)
		alarm->ClientFlags().Set(EClockAlarmFlagsIsSet);
	return error;
	}

TInt CAlmModel::ClockAlarmSet(TInt aAlarmIndex, const TASShdAlarm& aInfo)
//
//	Set a clock alarm
//
	{
	__ASSERT_DEBUG(aAlarmIndex < KMaxClockAlarms, Panic(EModelOutOfRange));
	//
	TInt error = KErrNone;
	TASShdAlarm* alarm = iClockAlarms[aAlarmIndex];
	if	(alarm->Id() != KNullAlarmId)
		{
		// There is already an alarm set in this slot. Delete
		// the old, replace it with the new.
		error = iAlmSvr.AlarmDelete(alarm->Id());
		if	(error != KErrNone)
			return error;
		alarm->ClientFlags().Clear(EClockAlarmFlagsIsSet);
		}

	// Add alarm (will also update cached alarm details)
	*alarm = aInfo;
	alarm->Category() = KASCliCategoryClock;
	error = iAlmSvr.AlarmAdd(*alarm);
	if	(error == KErrNone)
		alarm->ClientFlags().Set(EClockAlarmFlagsIsSet);
	return error;
	}

TInt CAlmModel::ClockAlarmInfo(TInt aAlarmIndex,TASShdAlarm& aInfo)
//
//	Get information about a clock alarm
//
	{
	__ASSERT_DEBUG(aAlarmIndex < KMaxClockAlarms, Panic(EModelOutOfRange));
	//
	TInt error = KErrNone;
	aInfo = *(iClockAlarms[aAlarmIndex]);
	if	(aInfo.Id() != KNullAlarmId && aInfo.ClientFlags().IsSet(EClockAlarmFlagsIsSet))
		{
		// Preserve flags
		const TBitFlags16 flags = aInfo.ClientFlags();

		// Fetch alarm details
		error = iAlmSvr.GetAlarmDetails(aInfo.Id(), aInfo);

		// Restore flags
		if	(error == KErrNone)
			{
			// Has the alarm already notified?
			if	(aInfo.State() != EAlarmStateQueued && aInfo.State() != EAlarmStateNotifying)
				{
				// Treat this slot as free
				TASShdAlarm* alarm = iClockAlarms[aAlarmIndex];
				alarm->Reset();
				aInfo = *alarm;
				}
			else
				aInfo.ClientFlags() = flags;
			}
		else if (error == KErrNotFound)
			{
			// Alarm has been deleted
			TASShdAlarm* alarm = iClockAlarms[aAlarmIndex];
			alarm->Reset();
			aInfo = *alarm;
			}
		}
	return error;
	}

TInt CAlmModel::ClockAlarmDelete(TInt aClockAlarmNum)
	{
	__ASSERT_DEBUG(aClockAlarmNum < KMaxClockAlarms, Panic(EModelOutOfRange));
	TASShdAlarm* alarm = iClockAlarms[aClockAlarmNum];
	__ASSERT_ALWAYS(alarm->Id() != KNullAlarmId, Fault(EModelfTryingToChangeStateOfNullClockAlarm));
	const TInt error = iAlmSvr.AlarmDelete(alarm->Id());
	if	(error == KErrNone || error == KErrNotFound)
		{
		// Update cached representation of deleted alarm
		alarm->Reset();
		}
	return error;
	}

TTimeWClockAlarmState CAlmModel::ClockAlarmState(TInt aClockAlarmNum) const
	{
	__ASSERT_DEBUG(aClockAlarmNum < KMaxClockAlarms, Panic(EModelOutOfRange));
	//
	const TASShdAlarm* alarm = iClockAlarms[aClockAlarmNum];
	if	(alarm->ClientFlags().IsSet(EClockAlarmFlagsIsSet))
		{
		switch(alarm->Status())
			{
		case EAlarmStatusEnabled:
			return ETimeWClockAlarmStateSet;
		case EAlarmStatusDisabled:
			return ETimeWClockAlarmStateDisabled;
			}
		}
	return ETimeWClockAlarmStateNotSet;
	}

void CAlmModel::NextAlarmResetListL()
//
//	Fetch a list of ids of all the alarms that are pending
//	within the alarm server.
//
	{
	iAlmSvr.GetAlarmIdListByStateL(EAlarmStateQueued, iNextAlarmIds);
	}

void CAlmModel::NextAlarmClearList()
	{
	iNextAlarmIds.Reset();
	}

TInt CAlmModel::NextAlarmNumber() const
	{
	return iAlmSvr.NumberOfAlarmsActiveInQueue();
	}

TInt CAlmModel::NextAlarmDelete(TInt aIndex)
//
//	Delete one of the next alarms
//
	{
	TInt ret = KErrNotFound;
	if	(aIndex < iNextAlarmIds.Count())
		{
		const TAlarmId id = iNextAlarmIds[aIndex];
		ret = iAlmSvr.AlarmDelete(id);
		}
	return ret;
	}

TBool CAlmModel::NextAlarmCanDelete(TInt aIndex) const
//
//	Determine whether can delete the specified alarm.
//
	{
	__ASSERT_DEBUG(aIndex < iNextAlarmIds.Count(), Panic(EModelOutOfRange));
	//
	const TAlarmId id = iNextAlarmIds[aIndex];
	//
	TASShdAlarm alarm;
	if	(iAlmSvr.GetAlarmDetails(id, alarm) == KErrNone)
		{
		return (
			alarm.Category() == KASCliCategoryClock ||
			alarm.State() == EAlarmStateSnoozed ||
			!alarm.HasOwningSession()
			);
		}
	return EFalse;
	}

TInt CAlmModel::NextAlarm(TFullName& aOwner, TASShdAlarm& aInfo) const
//
//	Get information about the next alarm
//
	{
	aOwner = KNullDesC;
	//
	TAlarmId id = KNullAlarmId;
	TInt ret = iAlmSvr.GetNextDueAlarmId(id);
	if	(ret == KErrNone)
		{
		if	(id == KNullAlarmId)
			{
			aInfo.Reset();
			ret = KErrNotFound;
			}
		else
			{
			ret = iAlmSvr.GetAlarmDetails(id, aInfo);
			if	(ret == KErrNone)
				ret = iAlmSvr.GetAlarmOwner(id, aOwner);
			}
		}
	return ret;
	}

TInt CAlmModel::NextAlarmInfo(TInt aIndex, TFullName& aOwner, TASShdAlarm& aInfo) const
//
//	Get info about the next alarm
//
	{
	TInt ret = KErrNotFound;
	if	(aIndex < iNextAlarmIds.Count())
		{
		const TAlarmId id = iNextAlarmIds[aIndex];
		ret = iAlmSvr.GetAlarmDetails(id, aInfo);
		if	(ret == KErrNone)
			{
			ret = iAlmSvr.GetAlarmOwner(id, aOwner);
			}
		}
	return ret;
	}

void CAlmModel::ReviewAlarmResetListL()
	{
	iAlmSvr.GetAlarmIdListByStateL(EAlarmStateNotified, iReviewAlarmIds);
	}

void CAlmModel::ReviewAlarmClearList()
	{
	iReviewAlarmIds.Reset();
	}

TInt CAlmModel::ReviewAlarmNumber() const
	{
	return iAlmSvr.AlarmCountByState(EAlarmStateNotified);
	}

TInt CAlmModel::ReviewAlarmInfo(TInt aIndex, TASShdAlarm& aInfo) const
//
//	Get information about a past alarm
//
	{
	TInt ret = KErrNotFound;
	if	(aIndex < iReviewAlarmIds.Count())
		{
		const TAlarmId id = iReviewAlarmIds[aIndex];
		ret = iAlmSvr.GetAlarmDetails(id, aInfo);
		}
	return ret;
	}

void CAlmModel::AlarmSoundToggle()
	{
	TTimeWAlarmSoundState soundState = AlarmSoundState();
	if	(soundState == ETimeWAlarmSoundOn)
		AlarmSoundStateSet(EFalse);
	else
		AlarmSoundStateSet(ETrue);
	}

TInt CAlmModel::AlarmSoundStateSet(TBool aSoundOn)
	{
	TAlarmGlobalSoundState state = EAlarmGlobalSoundStateOn;
	if	(!aSoundOn)
		state = EAlarmGlobalSoundStateOff;

	return iAlmSvr.SetAlarmSoundState(state);
	}

TTimeWAlarmSoundState CAlmModel::AlarmSoundState() const
	{
	// First check whether we're in a silent period
	if	(iAlmSvr.AlarmSoundsTemporarilySilenced())
		return ETimeWAlarmQuietPeriod;

	// We're not, so check global sound state and map
	// to TimeW settings
	TAlarmGlobalSoundState state = EAlarmGlobalSoundStateOff;
	if	(iAlmSvr.GetAlarmSoundState(state) == KErrNone)
		{
		if	(state == EAlarmGlobalSoundStateOn)
			return ETimeWAlarmSoundOn;
		}
	return ETimeWAlarmSoundOff;
	}

TTime CAlmModel::AlarmQuietPeriodEnd() const
	{
	TTime endTime;
	if	(iAlmSvr.GetAlarmSoundsSilentUntil(endTime) == KErrNone)
		return endTime;
	//
	endTime.HomeTime();
	return endTime;
	}

TInt CAlmModel::OrphanAlarmNumber() const
	{
	// Get the alarms
	RArray<TAlarmId> clockIds(KMaxClockAlarms);
	CleanupClosePushL(clockIds);
	iAlmSvr.GetAlarmIdListForCategoryL(KASCliCategoryClock, clockIds);

	//which are orphaned?
	TInt count = clockIds.Count();
	TInt orphaned = 0;
	for	(TInt ii=0; ii<count; ii++)
		{
		TASShdAlarm alarm;
		iAlmSvr.GetAlarmDetails(clockIds[ii], alarm);
		if	(alarm.HasBecomeOrphaned())
			orphaned++;
		}
	CleanupStack::PopAndDestroy(&clockIds);

	return orphaned;
	}
