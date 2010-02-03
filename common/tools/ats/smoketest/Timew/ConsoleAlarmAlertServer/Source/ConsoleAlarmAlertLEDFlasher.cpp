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

#include "consolealarmalertledflasher.h"

// System includes
#include <hal.h>

// User includes
#include "asaltdefs.h"

// Type definitions

// Constants
const TInt KFlasherPeriodicity = 500000;

// Enumerations

// Classes referenced


///////////////////////////////////////////////////////////////////////////////////////
// ----> CConsoleAlarmAlertLEDFlasher (source)
///////////////////////////////////////////////////////////////////////////////////////

//*************************************************************************************
CConsoleAlarmAlertLEDFlasher::CConsoleAlarmAlertLEDFlasher()
:	CTimer(CActive::EPriorityIdle)
	{
	CActiveScheduler::Add(this);
	}


//*************************************************************************************
CConsoleAlarmAlertLEDFlasher* CConsoleAlarmAlertLEDFlasher::NewL()
	{
	CConsoleAlarmAlertLEDFlasher* self = new(ELeave) CConsoleAlarmAlertLEDFlasher();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
/**
 * Start the flasher, if not busy
 */
TInt CConsoleAlarmAlertLEDFlasher::Start()
	{
	Stop();
	After(KFlasherPeriodicity);
	return(SetRedLedOn(ETrue));
	}


//*************************************************************************************
/**
 * Stop the LED flashing
 */
void CConsoleAlarmAlertLEDFlasher::Stop()
	{
	Cancel();
	SetRedLedOn(EFalse);
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
/**
 * @see CActive
 */
void CConsoleAlarmAlertLEDFlasher::RunL()
	{
	if (iStatus!=KErrNone)
		{
		SetRedLedOn(EFalse);
		return;
		}
	SetRedLedOn(!iLedOn);
	After(KFlasherPeriodicity);
	}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


//*************************************************************************************
/**
 * Turn the LED on or off
 */
TInt CConsoleAlarmAlertLEDFlasher::SetRedLedOn(TBool aOn)
	{
	const TInt KClearAllLeds = 0;
	const TInt KLedMaskRed1 = 1;

	iLedOn = aOn;
	if (iLedOn)
		{
		return HAL::Set(HAL::ELEDmask, KLedMaskRed1);
		}
	else
		{
		return HAL::Set(HAL::ELEDmask, KClearAllLeds);
		}
	}
