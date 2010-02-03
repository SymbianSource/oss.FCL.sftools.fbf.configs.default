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

#ifndef __TIMEWMOD_H__
#define __TIMEWMOD_H__

// System includes
#include <e32base.h>

// Constants
const TInt KMaxClockAlarms = 8;

/**
 * From the old T32Alm.h file. Encapsulates the idea of
 * a "Not set" alarm, which the new AlarmServer doesn't
 * support.
 */
enum TTimeWClockAlarmState
	{
	ETimeWClockAlarmStateNotSet = 0,
	ETimeWClockAlarmStateSet,
	ETimeWClockAlarmStateDisabled
	};

/**
 *
 */
enum TTimeWAlarmSoundState
	{
	ETimeWAlarmSoundOn = 0,
	ETimeWAlarmSoundOff,
	ETimeWAlarmQuietPeriod
	};

/**
 *
 */
enum TInternalClockAlarmFlags
	{
	EClockAlarmFlagsIsSet		= 1
	};

enum TModelPanic
	{
	EModelWorld = 0,
	EModelObserver,
	EModelOutOfRange,
	EModel
	};
enum TModelFault
	{
	EModelfIdleObj = 0,
	EModelfPriorityInvalid,
	EModelfIdleArgument,
	EModelfNoNotifier,
	EModelfOutOfRange,
	EModelfTime,
	EModelfTryingToChangeStateOfNullClockAlarm,
	EModelfTryingToGetInfoAboutNullClockAlarm,
	EModelfTryingToOrphanClockAlarm
	};

GLREF_C void Panic(TModelPanic aPanic);
GLREF_C void Fault(TModelFault aFault);

class MTimewModelObserver
	{
public:
	virtual void HandleUpdate(TInt aNotification)=0;
	};

class CIntermediateObserver : public CBase, public MTimewModelObserver
	{
public:
	CIntermediateObserver();
	virtual void HandleUpdate(TInt aNotification);
	void SetTimewModelObserver(MTimewModelObserver* aObserver);
protected:
	void Notify(TInt aNotification);
private:
	MTimewModelObserver* iObserver;
	};

class CIdleObserver : public CIntermediateObserver
	{
public:
	CIdleObserver();
	~CIdleObserver();
	void ConstructL(MTimewModelObserver* aObserver,TInt aPriority);
public:
	void CancelIdleRefresh();
	virtual void HandleUpdate(TInt aNotification);
protected:
	virtual void DoIdleCancel();
	virtual void DoIdleNotify();
private:
	void StartIdleRefresh();
	static TInt IdleCallBack(TAny* aPtr);
private:
	TInt iNotification;
	CIdle* iIdle;
	};

class CModelResponder;
class CModelBase : public CBase
	{
protected:
	CModelBase();

public:
	~CModelBase();

public:
	void Stop();
	void Start();
	void SetTimewModelObserver(MTimewModelObserver* aObserver);

protected:
	void Notify(TInt aNotification);
	virtual void ProcessResponder(TInt aStatus)=0;
	void SetResponderActive(CModelResponder* aResponder);

private:
	void DoObserverNotify(TInt aNotification);

private:
	TBool iDoingViewNotify;
	MTimewModelObserver* iObserver;
	CModelResponder* iResponder;
	friend class CModelResponder;
	};

#endif
