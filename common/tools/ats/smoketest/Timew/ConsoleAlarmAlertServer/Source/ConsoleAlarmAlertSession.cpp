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

#include "consolealarmalertsession.h"

// System includes

#include "e32svr.h"
// User includes
#include "asaltdefs.h"
//#include "..\..\Test\ConsoleANTestClient.h"
//
#include "consolealarmalertsession.h"
#include "consolealarmalertconsole.h"
#include "consolealarmalertledflasher.h"
#include "consolealarmalertserver.h" //for inter-session communication

// Type definitions

// Constants
const TInt KSlot0 = 0;
const TInt KSlot1 = 1;
const TInt KSlot2 = 2;

// Enumerations

// Classes referenced


///////////////////////////////////////////////////////////////////////////////////////
// ----> CConsoleAlarmAlertSession (source)
///////////////////////////////////////////////////////////////////////////////////////

//*************************************************************************************
CConsoleAlarmAlertSession::CConsoleAlarmAlertSession(CConsoleAlarmAlertServer* aServer)
:	iServer(aServer), iInstructionSet(NULL), iInstructionSetFound(EFalse)
	{
	}


//*************************************************************************************
CConsoleAlarmAlertSession::~CConsoleAlarmAlertSession()
	{
	CConsoleAlarmAlertSession* session = iServer->WaitingSession();
	session = NULL;
	delete iSoundLEDFlasher;
	delete iSoundCallbackTimer;
	delete iConsole;
	}


//*************************************************************************************
void CConsoleAlarmAlertSession::ConstructL()
	{
	iSoundLEDFlasher = CConsoleAlarmAlertLEDFlasher::NewL();
	iSoundCallbackTimer = CPeriodic::NewL(CActive::EPriorityIdle);
	iConsole = new(ELeave) CConsoleAlarmAlertConsole(*this);
	}


//*************************************************************************************
CConsoleAlarmAlertSession* CConsoleAlarmAlertSession::NewL(CConsoleAlarmAlertServer* aServer)
	{
	CConsoleAlarmAlertSession* self = new(ELeave) CConsoleAlarmAlertSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
TTime CConsoleAlarmAlertSession::DeferTime() const
	{
	return iDeferTime;
	}


//*************************************************************************************
void CConsoleAlarmAlertSession::Notify(TASAltAlertServerResponse aFlag)
	{
	if	(iHasMessage)
		{
		iMsgPtr.Complete(aFlag);
		iHasMessage = EFalse;
		}
	}


//*************************************************************************************
void CConsoleAlarmAlertSession::Notify(TASAltAlertServerResponse aFlag, const TTime& aDeferTime)
	{
	if	(iHasMessage)
		{
		TPckgC<TTime> time(aDeferTime);
		TRAPD(err, iMsgPtr.WriteL(KSlot0, time));
		__ASSERT_DEBUG(err == KErrNone, User::Invariant());
		Notify(aFlag);
		}
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
void CConsoleAlarmAlertSession::ServiceL(const RMessage2& aMessage)
	{
	iMsgComplete = ETrue;
	const TInt function = aMessage.Function();

	//see if the server holds a set of instructions to be followed,
	//rather than waiting for user input.
	if(!iInstructionSetFound) //go look for it now.
		{
		//we will enter into this code only once
		const CServer2* tmpServ = Server();
		const CConsoleAlarmAlertServer* specific = reinterpret_cast<const CConsoleAlarmAlertServer*>(tmpServ);
		iInstructionSet = const_cast<CConsoleAlarmAlertServer*>(specific)->ResponseArray();
		iInstructionSetFound = ETrue;
		}

	//
	switch(function)
		{
    case EASAltOpCodeNotify:
    	{
		__ASSERT_ALWAYS(!iHasMessage, User::Invariant());
		iMsgComplete = EFalse;
		iMsgPtr = aMessage;
		iHasMessage = ETrue;
		CConsoleAlarmAlertSession* session = iServer->WaitingSession();
		session = this;
		break;
		}

    case EASAltOpCodeNotifyCancel:
		if	(iHasMessage)
			{
			iHasMessage=EFalse;
			iMsgPtr.Complete(KErrCancel);
			}
		break;

	case EASAltOpCodeVisible:
		{
		TBool isVisible = aMessage.Int0();
		iConsole->SetVisibility(isVisible);
		iServer->SetNotifying(isVisible);
		break;
		}

    case EASAltOpCodeSetState:
		iConsole->SetAlertState(aMessage.Int0());
		break;

    case EASAltOpCodeSetAlarm:
		SetAlarmL(aMessage);
		iConsole->iTimeInterval=0;
		iConsole->UpdateDisplay();
		break;

	case EASAltOpCodeGetUserTime:
		GetUserTimeL(aMessage);
		break;

	case EASAltOpCodeSetDeferTime:
		SetDeferTimeL(aMessage);
		iConsole->UpdateDisplay();
		break;

    case EASAltOpCodeLogon:
		__ASSERT_ALWAYS(!iLoggedOn, User::Invariant());
		iMsgComplete=EFalse;
		iLoggedOn=ETrue;
		break;

	case EASAltOpCodeStartPlayingSound:
		{
		iSoundLEDFlasher->Start();
		TCallBack callback(SoundPlaybackCallbackL, this);
		iSoundCallbackTimer->Start(TTimeIntervalMicroSeconds32(0), TTimeIntervalMicroSeconds32(3000000), callback);
		break;
		}

	case EASAltOpCodeStopPlayingSound:
		iSoundLEDFlasher->Stop();
		iSoundCallbackTimer->Cancel();
		break;

#if 0
//Test code
	case EASAltTestOpCodeAcknowledgeAlarm:
		if (iServer->WaitingSession())
			iServer->WaitingSession()->Notify(EASAltAlertServerResponseClear);
		break;

	case EASAltTestOpCodeGetAttachment:
		{
		if (iServer->Attachment() != NULL)
			{
			const TDes8& data = iServer->Attachment()->Des();
			const TInt maxLength = static_cast<TInt>(aMessage.Int0());
			if	(data.Size() > maxLength)
				User::Panic(_L("Client's buffer is too short"), KErrOverflow);
			else
				{
				if (data.Length() == 0)
					{
					_LIT8(KEmpty,"Empty");
					aMessage.WriteL(KSlot1, KEmpty);
					}
				else
					{
					aMessage.WriteL(KSlot1, data);
					}
				iServer->SetAttachment(NULL);
				}
			}
		}
		break;

	case EASAltTestOpCodeGetIsNotifying:
		{
		TPckgC<TBool> ret = iServer->IsNotifying();
		aMessage.WriteL(KSlot0, ret);
		break;
		}
#endif
	default:
		User::Invariant();
		break;
		}

	if	(iMsgComplete)
		aMessage.Complete(KErrNone);
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
TBool CConsoleAlarmAlertSession::CmdL()
	{
	return KErrNone;
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
void CConsoleAlarmAlertSession::SetAlarmL(const RMessage2& aMessage)
	{
	TPckg<TASShdAlarm> pA(iAlarm);
	aMessage.ReadL(KSlot0, pA);
	if	(iAlarm.HasAssociatedData())
		{
		iAlarmAssociatedDataSize = aMessage.GetDesLength(2);
		TPckg<TAgnAlarmInfo> pB(iAlarmData);
		aMessage.ReadL(KSlot2, pB);

		//Storing the data in the server for the test session to read..
		HBufC8* data = HBufC8::NewLC(iAlarmAssociatedDataSize);
		TPtr8 pData(data->Des());
		aMessage.ReadL(KSlot2, pData);
		iServer->SetAttachment(data);  //Server takes an ownership
		CleanupStack::Pop(data);
		}
	else
		iAlarmAssociatedDataSize = 0;
	}


//*************************************************************************************
void CConsoleAlarmAlertSession::GetUserTimeL(const RMessage2& aMessage)
	{
	TTime time;
	time.HomeTime();
	time+=TTimeIntervalMinutes(iConsole->GetTimeInterval());
	TPtrC8 pTime((TUint8 *)&time,sizeof(TTime));
	aMessage.WriteL(KSlot0,pTime);
	}


//*************************************************************************************
void CConsoleAlarmAlertSession::SetDeferTimeL(const RMessage2& aMessage)
	{
	TPckg<TTime> pTime(iDeferTime);
	aMessage.ReadL(KSlot0, pTime);
	}


//*************************************************************************************
TBool CConsoleAlarmAlertSession::SoundPlaybackCallbackL(TAny* /*aSelf*/)
	{
//	_LIT(KPlayingSound, "Playing sound");
//	User::InfoPrint(KPlayingSound);
	return ETrue;
	}

//**************************************************************************************
TAny * CConsoleAlarmAlertSession::InstructionSet()
	{
	return iInstructionSet;
	}
