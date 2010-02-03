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

//	class CAlmModel
inline TAlarmSoundState CAlmModel::AlarmSoundState() const
	{
	return(iAlmSvr.AlarmSoundState());
	}
inline void CAlmModel::AlarmSoundStateSet(TBool aSoundOn)
	{
	iAlmSvr.SetAlarmSoundOn(aSoundOn);
	}
inline void CAlmModel::AlarmSoundToggle()
	{
	iAlmSvr.SetAlarmSoundOn(iAlmSvr.AlarmSoundState()==EAlarmSoundOff);
	}
inline TTime CAlmModel::AlarmQuietPeriodEnd() const
	{
	return(iAlmSvr.QuietPeriodUntil());
	}
inline void CAlmModel::AlarmQuietPeriodCancel()
	{
	iAlmSvr.QuietPeriodCancel();
	}
inline TInt CAlmModel::ClockAlarmDelete(TInt aClockAlarmNum)
	{
	__ASSERT_DEBUG(aClockAlarmNum<KMaxClockAlarms,Panic(EModelOutOfRange));
	return(iAlmSvr.AlarmDelete(aClockAlarmNum));
	}
inline TAlarmSetState CAlmModel::ClockAlarmState(TInt aClockAlarmNum) const
	{
	__ASSERT_DEBUG(aClockAlarmNum<KMaxClockAlarms,Panic(EModelOutOfRange));
	return(iAlmSvr.AlarmState(aClockAlarmNum));
	}
inline void CAlmModel::NextAlarmResetListL()
	{
	iAlmSvr.AlarmArrayPopulateL(*iNextAlarms,RAlarmServer::EArrayNext);
	}
inline void CAlmModel::NextAlarmClearList()
	{
	iNextAlarms->Reset();
	}
inline TInt CAlmModel::NextAlarmNumber() const
	{
	return(iAlmSvr.AlarmCount(RAlarmServer::ECountNext));
	}
inline void CAlmModel::ReviewAlarmResetListL()
	{
	iAlmSvr.AlarmArrayPopulateL(*iReviewAlarms,RAlarmServer::EArrayReview);
	}
inline void CAlmModel::ReviewAlarmClearList()
	{
	iReviewAlarms->Reset();
	}
inline TInt CAlmModel::ReviewAlarmNumber() const
	{
	return(iAlmSvr.AlarmCount(RAlarmServer::ECountReview));
	}
inline TInt CAlmModel::OrphanAlarmNumber() const
	{
	return(iAlmSvr.AlarmCount(RAlarmServer::ECountOrphan));
	}
inline TInt CAlmModel::UnacknowledgedAlarmNumber() const
	{
	return(iAlmSvr.AlarmCount(RAlarmServer::ECountUnacknowledged));
	}
	
