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

#ifndef __ALMMOD_H__
#define __ALMMOD_H__

// System includes
#include <asclisession.h>

// User includes
#include "timewmod.h"

// Constants
const TInt KArrayOfAlarmIdsGranularity = 64;
const TInt KPriorityAlarmIdleObserver=-0x0010;
const TInt KPriorityAlarmResponder=-0x0005; // Idle observer priority must be lower than this

//	The alarm model gives a general notification only
//
class CAlmIdleObserver : public CIdleObserver
	{
public:
	static CAlmIdleObserver* NewL(MTimewModelObserver* aObserver,TInt aIdlePriority);
	};

class CAlmModel : public CModelBase
	{
public:
	~CAlmModel();
	static CAlmModel* NewL(MTimewModelObserver* aObserver,TInt aResponderPriority);
	virtual void ProcessResponder(TInt aStatus);
public:
	TInt AlarmQuietPeriodMinutes() const;
	TInt AlarmQuietPeriodSet(TInt aMinutes);
	inline void AlarmQuietPeriodCancel();
	TTime AlarmQuietPeriodEnd() const;
	//
	void AlarmSoundToggle();
	TInt AlarmSoundStateSet(TBool aSoundOn);
	TTimeWAlarmSoundState AlarmSoundState() const;
	//
	TInt ClockAlarmDelete(TInt aAlarmIndex);
	TInt ClockAlarmEnable(TInt aAlarmIndex,TTimeWClockAlarmState aClockAlarmState);
	TTimeWClockAlarmState ClockAlarmState(TInt aAlarmIndex) const;
	TInt ClockAlarmSet(TInt aAlarmIndex,const TASShdAlarm& aInfo);
	TInt ClockAlarmInfo(TInt aAlarmIndex,TASShdAlarm& aInfo);
	//
	void NextAlarmResetListL();
	void NextAlarmClearList();
	TInt NextAlarmNumber() const;
	TInt NextAlarmDelete(TInt aIndex);
	TBool NextAlarmCanDelete(TInt aIndex) const;
	TInt NextAlarm(TFullName& aOwner, TASShdAlarm& aInfo) const;
	TInt NextAlarmInfo(TInt aIndex, TFullName& aOwner, TASShdAlarm& aInfo) const;
	//
	TInt OrphanAlarmNumber() const;
	inline TInt UnacknowledgedAlarmNumber() const;
	//
	void ReviewAlarmResetListL();
	void ReviewAlarmClearList();
	TInt ReviewAlarmNumber() const;
	TInt ReviewAlarmInfo(TInt aIndex, TASShdAlarm& aInfo) const;

private:
	CAlmModel();
	void ConstructL(MTimewModelObserver* aObserver,TInt aResponderPriority);

private:

	/**
	 * The list of clock alarms. Note that although there is always 8
	 * alarms in this list, not all will be active.
	 */
	RPointerArray<TASShdAlarm> iClockAlarms;

	/**
	 *
	 */
	RASCliSession iAlmSvr;

	/**
	 *
	 */
	TRequestStatus iStatus;

	/**
	 *
	 */
	RArray<TAlarmId> iNextAlarmIds;

	/**
	 *
	 */
	RArray<TAlarmId> iReviewAlarmIds;
	};


TInt CAlmModel::UnacknowledgedAlarmNumber() const
	{
	TInt count = iAlmSvr.AlarmCountByState(EAlarmStateWaitingToNotify);
	return (count + iAlmSvr.AlarmCountByState(EAlarmStateNotifying));
	}

void CAlmModel::AlarmQuietPeriodCancel()
	{
	iAlmSvr.CancelAlarmSilence();
	}

#endif
