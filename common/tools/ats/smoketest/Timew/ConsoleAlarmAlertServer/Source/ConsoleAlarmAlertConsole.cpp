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

#include "consolealarmalertconsole.h"

// System includes
#include <e32twin.h>
#include <e32base.h>
#include <e32svr.h>

// User includes
#include "asaltdefs.h"
#include "consolealarmalertsession.h"

// Type definitions

// Constants

// Enumerations

// Classes referenced


///////////////////////////////////////////////////////////////////////////////////////
// ----> CConsoleAlarmAlertConsole (source)
///////////////////////////////////////////////////////////////////////////////////////

//*************************************************************************************
CConsoleAlarmAlertConsole::CConsoleAlarmAlertConsole(CConsoleAlarmAlertSession& aSession)
:	CActive(CActive::EPriorityLow), iSession(aSession), iInstructionIndex(0)
	{
	CActiveScheduler::Add(this);
	}


//*************************************************************************************
CConsoleAlarmAlertConsole::~CConsoleAlarmAlertConsole()
	{
	Cancel();
	Hide();
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
void CConsoleAlarmAlertConsole::GetKey()
	{
	if	(iWin && !IsActive())
		{
		SetActive();
		iWin->Read(iStatus);
		}		
	}


//*************************************************************************************
void CConsoleAlarmAlertConsole::CancelKey()
	{
	if	(IsActive())
		Cancel();
	}


//*************************************************************************************
void CConsoleAlarmAlertConsole::DecCounter()
	{
	if	(iTimeInterval > 0)
		{
		--iTimeInterval;
		UpdateDisplay();
		}
	}


//*************************************************************************************
void CConsoleAlarmAlertConsole::UpdateDisplay()
	{
	TBuf<180> buf;
	_LIT(KEalwlAlarmSoundOff, "Off");
	_LIT(KEalwlAlarmSoundOn, "On");
	_LIT(KEalwlStatusEnabled, "Are");
	_LIT(KEalwlStatusNotEnabled, "Are Not");
	_LIT(KEalwlStatusIs, "Is");
	_LIT(KEalwlStatusIsNo, "Is No");
	//
	if	(!iWin)
		return;
	iWin->SetCursorPosAbs(TPoint(0,0));

	// MESSAGE
	buf.Format(_L("Message : %S"), &iSession.Alarm().Message());
	iWin->Write(buf);
	iWin->ClearToEndOfLine();

	// DATA
	if	(iSession.Alarm().HasAssociatedData())
		{
		const TAgnAlarmInfo data = iSession.AlarmData();
		const TUid KUidAgendaModelAlarmCategory = { 0x101F4A70 };
		if (data.iAlarmCategory == KUidAgendaModelAlarmCategory)
			{
			_LIT(KEalwlAgendaData,"\ndata: agenda filename \"%S\"");
			if (data.iFileName.Length() < 40)
				buf.Format(KEalwlAgendaData,&data.iFileName);
			else
				{
				TBuf<40> temp = data.iFileName.Left(40);
				buf.Format(KEalwlAgendaData,&temp);
				}
			}
		else
			buf.Format(_L("\nAlarm has %d data bytes"), iSession.AlarmDataSize());
		}
	else
		{
		buf = _L("\nAlarm has no associated data");
		}
	iWin->Write(buf);
	iWin->ClearToEndOfLine();

	// DEFER TIME
	if	(iSession.DeferTime() != Time::NullTTime())
		{
		TDateTime dateTime(iSession.DeferTime().DateTime());
		buf.Format(_L("\nDefer   : %02d:%02d"), dateTime.Hour(), dateTime.Minute());
		}
	else
		buf=_L("\nNULL defer time");
	iWin->Write(buf);
	iWin->ClearToEndOfLine();

	// TIME INTERVAL
	buf.Format(_L("\nCurrent time interval is %d minutes"), GetTimeInterval());
	iWin->Write(buf);
	iWin->ClearToEndOfLine();

	// STATE FLAGS
	buf.Format(_L("\nAlarm sounds are %S"),(iAlertState&EASAltStateFlagsSilentRunning)?&KEalwlAlarmSoundOff:&KEalwlAlarmSoundOn);
	iWin->Write(buf);
	iWin->ClearToEndOfLine();
	buf.Format(_L("\nAlarm sounds %S paused"),(iAlertState&EASAltStateFlagsSoundIsPaused)?&KEalwlStatusEnabled:&KEalwlStatusNotEnabled);
	iWin->Write(buf);
	iWin->ClearToEndOfLine();
	buf.Format(_L("\nAlarm sounds %S deferred"),(iAlertState&EASAltStateFlagsInQuietPeriod)?&KEalwlStatusEnabled:&KEalwlStatusNotEnabled);
	iWin->Write(buf);
	iWin->ClearToEndOfLine();
	buf.Format(_L("\nThere %S more than one unacknowledged alarms"),(iAlertState&EASAltStateFlagsMoreThanOneDueAlarm)?&KEalwlStatusEnabled:&KEalwlStatusNotEnabled);
	iWin->Write(buf);
	iWin->ClearToEndOfLine();
	buf.Format(_L("\nThere %S memory to snooze alarms"),(iAlertState&EASAltStateFlagsNoMemoryForSnoozeAlarm)?&KEalwlStatusIsNo:&KEalwlStatusIs);
	iWin->Write(buf);
	iWin->ClearToEndOfLine();
	buf.Format(_L("\nThere %S an alarm sound to play"),(iAlertState&EASAltStateFlagsAlarmHasNoSoundFileName)?&KEalwlStatusIsNo:&KEalwlStatusIs);
	iWin->Write(buf);
	iWin->ClearToEndOfLine();

	// COMMAND PROMPTS
	iWin->Write(_L("\nENTER = Silence alarm          SPACE = Quiet Period"));
	iWin->ClearToEndOfLine();
	iWin->Write(_L("\nESCAPE = Acknowledge alarm     A = Acknowledge all"));
	iWin->ClearToEndOfLine();
	iWin->Write(_L("\nP = Pause alarm sound          TAB = Snooze alarm"));
	iWin->ClearToEndOfLine();
	iWin->Write(_L("\n1 = 1 min sound interval		 0 = 0 mins sound interval"));
	iWin->ClearToEndOfLine();
	iWin->Write(_L("\n+ = Increase int. and pause    - = Decrease int. and pause"));
	iWin->ClearToEndOfLine();
	iWin->Write(_L("\n! = Kill alarm alert server"));
	iWin->ClearToEndOfLine();
	iWin->Write(_L("\n"));
	iWin->ClearToEndOfLine();
	iWin->Write(_L("\r"));
	//
	if(!iSession.InstructionSet()) //Get user input if instruction set unspecified.
		GetKey();
	else if (iInstructionIndex >= static_cast<RArray<TInt>*>(iSession.InstructionSet())->Count()) //Get user input if program has already iterated thru entire instruction set.
		GetKey();
	else //Use the command in the instruction set, instead of waiting for user input.
		{
			if(!IsActive())
			{
			iStatus = KRequestPending;
			TRequestStatus* ptrStatus = &iStatus;
			User::RequestComplete( ptrStatus, KErrNone );
			SetActive();
			}
		}
	}

//*************************************************************************************
TInt CConsoleAlarmAlertConsole::GetTimeInterval() const
	{
	return(iTimeInterval);
	}


//*************************************************************************************
void CConsoleAlarmAlertConsole::SetVisibility(TInt aVis)
	{
	if	(!aVis)
		Hide();
	else if (!iWin)
		{
		// Create a new window, since cannot set order of console windows
		CreateWin();
		UpdateDisplay();
		}
	}


//*************************************************************************************
void CConsoleAlarmAlertConsole::SetAlertState(TInt aFlags)
	{
	iAlertState = aFlags;
	UpdateDisplay();
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
void CConsoleAlarmAlertConsole::RunL()
	{
	if (iStatus.Int() == KErrCancel)
		return;

	__ASSERT_DEBUG(iWin, User::Invariant());
	TInt key = iWin->KeyCode();
	if(iSession.InstructionSet() && iInstructionIndex < static_cast<RArray<TInt>*>(iSession.InstructionSet())->Count())
		{
		RArray<TInt>* temp = static_cast<RArray<TInt>*>(iSession.InstructionSet());
		key = (*temp)[iInstructionIndex++];
		GetKey();
		}
	else
		{
		GetKey();
		if ((key>=EKeyF1 && key<=EKeyF12) || key==EKeyTab)
			{
			DoDeferTime(EASAltAlertServerResponseSnooze);
			}
		}

	switch (key)
		{
	// ACKNOWLEDGE CURRENT
	case EKeyEscape:
		iSession.Notify(EASAltAlertServerResponseClear);
		return;
	// ACKNOWLEDGE ALL
	case 'a':
	case 'A':
		iSession.Notify(EASAltAlertServerResponseClearAll);
		return;
	// QUIET PERIOD
	case EKeySpace:
		DoDeferTime(EASAltAlertServerResponseQuietPeriod);
		break;
	// PAUSE SOUND
	case 'p':
	case 'P':
		DoDeferTime(EASAltAlertServerResponsePauseSound);
		break;
	// SILENCE ALL SOUND
	case EKeyEnter:
		iSession.Notify(EASAltAlertServerResponseSilence);
		break;
	// INTERVAL 1 MINUTE
	case '1':
		iTimeInterval=1;
		iSession.Notify(EASAltAlertServerResponsePauseSound);
		break;
	// INTERVAL 0 MINUTES
	case '0':
		iTimeInterval=0;
		iSession.Notify(EASAltAlertServerResponsePauseSound);
		break;
	// PAUSE +
	case '=':
	case '+':
		iTimeInterval++;
		iSession.Notify(EASAltAlertServerResponsePauseSound);
		break;
	// PAUSE -
	case '-':
		iTimeInterval--;
		iSession.Notify(EASAltAlertServerResponsePauseSound);
		break;

	// QUIT
	case '!':
		CActiveScheduler::Stop();
		break;
		}

	TBuf<10> buf;
	buf.Format(_L("'%c',"), key);
	iWin->Write(buf);
	}


//*************************************************************************************
void CConsoleAlarmAlertConsole::DoCancel()
	{
	__ASSERT_DEBUG(iWin, User::Invariant());
	iWin->ReadCancel();
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
void CConsoleAlarmAlertConsole::Hide()
	{
	if	(iWin)
		CancelKey(); 
	//
	delete iWin;
	iWin = NULL;
	//
	if	(iCountDown)
		iCountDown->Cancel();
	//
	delete iCountDown;
	iCountDown=NULL;
	}


//*************************************************************************************
void CConsoleAlarmAlertConsole::CreateWin()
	{
	_LIT(KAlarmAlertConsoleCaption, "Test Alarm Alert Server Console");
	//
	__ASSERT_DEBUG(!iWin, User::Invariant());
	__ASSERT_DEBUG(!iCountDown, User::Invariant());
	//
	TRAPD(trapCheck, iWin = Console::NewL(KAlarmAlertConsoleCaption, TSize(KConsFullScreen, KConsFullScreen)));
	if	(trapCheck)
		{
		iWin=NULL;
		return;
		}
	
	TRAP(trapCheck, iCountDown = CPeriodic::NewL(CActive::EPriorityIdle));
	if	(trapCheck)
		{
		iCountDown=NULL;
		return;
		}
	//
	const TTimeIntervalMicroSeconds32 interval(60000000);
	iCountDown->Start(interval, interval, TCallBack(&CountDownCallBack, this));
	}


//*************************************************************************************
void CConsoleAlarmAlertConsole::DoDeferTime(TASAltAlertServerResponse aResponse)
	{
	if	(++iTimeInterval > 60)
		{
		iTimeInterval = 5;
		User::Beep(440, 12);
		}
	//
	TTime now;
	now.HomeTime();
	now += TTimeIntervalMinutes(iTimeInterval);
	//
	iSession.Notify(aResponse, now);
	}


//*************************************************************************************
TBool CConsoleAlarmAlertConsole::CountDownCallBack(TAny* aSelf)
	{
	CConsoleAlarmAlertConsole& self = *reinterpret_cast<CConsoleAlarmAlertConsole*>(aSelf);
	self.DecCounter();
	//
	return EFalse;
	}
