// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CONSOLEALARMALERTSESSION_H__
#define __CONSOLEALARMALERTSESSION_H__

// System includes
#include <e32base.h>

// User includes
#include "asaltdefs.h"
#include "asshdalarm.h"

// Type definitions

// Constants

// Classes referenced
class CConsoleAlarmAlertConsole;
class CConsoleAlarmAlertLEDFlasher;
class CConsoleAlarmAlertServer;
// Enumerations


struct TAgnAlarmInfo
	/**
	Copy of System-Wide Unique Agenda Entry Reference defined in agmalarm.h
	*/
	{
	TUid iAlarmCategory;
	TUint32 iAgnUniqueId;
	TFileName iFileName;
	};

///////////////////////////////////////////////////////////////////////////////////////
// ----> CConsoleAlarmAlertSession (header)
///////////////////////////////////////////////////////////////////////////////////////
class CConsoleAlarmAlertSession : public CSession2
	{
///////////////////////////////////////////////////////////////////////////////////////
public:										// STATIC CONSTRUCT / DESTRUCT
///////////////////////////////////////////////////////////////////////////////////////
	static CConsoleAlarmAlertSession*		NewL(CConsoleAlarmAlertServer* aServer);
	~CConsoleAlarmAlertSession();

///////////////////////////////////////////////////////////////////////////////////////
private:									// INTERNAL CONSTRUCT
///////////////////////////////////////////////////////////////////////////////////////
	CConsoleAlarmAlertSession(CConsoleAlarmAlertServer* aServer);
	void									ConstructL();

///////////////////////////////////////////////////////////////////////////////////////
public:										// ACCESS
///////////////////////////////////////////////////////////////////////////////////////

	TTime									DeferTime() const;

	inline const TASShdAlarm& 				Alarm() const { return iAlarm; }

	inline TInt								AlarmDataSize() const { return iAlarmAssociatedDataSize; }

	inline const TAgnAlarmInfo&				AlarmData() const { return iAlarmData; }

	void									Notify(TASAltAlertServerResponse aFlag);

	void									Notify(TASAltAlertServerResponse aFlag, const TTime& aDeferTime);

	TAny *									InstructionSet();

///////////////////////////////////////////////////////////////////////////////////////
private:									// FROM CSession2
///////////////////////////////////////////////////////////////////////////////////////
	void									ServiceL(const RMessage2& aMessage);

///////////////////////////////////////////////////////////////////////////////////////
private:									// CMD - DEBUG ONLY
///////////////////////////////////////////////////////////////////////////////////////
	TBool									CmdL();

///////////////////////////////////////////////////////////////////////////////////////
private:									// INTERNAL METHODS
///////////////////////////////////////////////////////////////////////////////////////

	void									SetAlarmL(const RMessage2& aMessage);

	void									GetUserTimeL(const RMessage2& aMessage);

	void									SetDeferTimeL(const RMessage2& aMessage);

	static TBool							SoundPlaybackCallbackL(TAny* aSelf);

///////////////////////////////////////////////////////////////////////////////////////
private:									// MEMBER DATA
///////////////////////////////////////////////////////////////////////////////////////

	TTime									iDeferTime;

	TASShdAlarm								iAlarm;

	TInt									iAlarmAssociatedDataSize;

	TAgnAlarmInfo							iAlarmData;

	TBool									iLoggedOn;

	TBool									iHasMessage;

	TBool									iMsgComplete;

	RMessage2								iMsgPtr;

	CPeriodic*								iSoundCallbackTimer;

	CConsoleAlarmAlertLEDFlasher*			iSoundLEDFlasher;

	CConsoleAlarmAlertConsole*				iConsole;

	CConsoleAlarmAlertServer*				iServer;

	TAny*									iInstructionSet;

	TBool									iInstructionSetFound;
	};

#endif
