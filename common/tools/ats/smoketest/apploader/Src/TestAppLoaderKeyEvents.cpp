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
// This contains CTestAppLoaderKeyEvents
// 
//

// USER includes
#include "TestAppLoaderKeyEvents.h"

// EPOC includes
#include <w32std.h>

/*@{*/
const TInt KDelayMultiplier		= 1000;

/// Parameters
_LIT(KKey,						"key%d");
_LIT(KDelay,					"delay%d");

/// Key codes
_LIT(KKeyNull,					"EKeyNull");
_LIT(KKeyDevice0,				"EKeyDevice0");
_LIT(KKeyDevice1,				"EKeyDevice1");
/*@}*/

// constructor
CTestAppLoaderKeyEvents::CTestAppLoaderKeyEvents()
:	CTestAppLoaderBase()
	{
	SetTestStepName(_L("KeyEvents"));
	}

TBool CTestAppLoaderKeyEvents::GetKeyCodeFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TKeyCode& aResult, TStdScanCode& aScanCode)
	{
	TPtrC	str;
	TBool	ret=GetStringFromConfig(aSectName, aKeyName, str);

	if ( ret )
		{
		if ( str==KKeyNull )
			{
			aResult=EKeyNull;
			aScanCode=EStdKeyNull;
			}
		else if ( str==KKeyDevice0 )
			{
			aResult=EKeyDevice0;
			aScanCode=EStdKeyDevice0;
			}
		else if ( str==KKeyDevice1 )
			{
			aResult=EKeyDevice1;
			aScanCode=EStdKeyDevice1;
			}
		}

	return ret;
	}

// Each test step must supply a implementation for doTestStepL
enum TVerdict CTestAppLoaderKeyEvents::doTestStepL( void )
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("TEST-> KEY EVENTS"));

	RWsSession							ws;
	User::LeaveIfError(ws.Connect());
	CleanupClosePushL(ws);
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								index=0;
	TBool								moreData=ETrue;
	TKeyCode							keyCode;
	TStdScanCode						scanCode;
	TInt								delay;
	while ( moreData )
		{
		tempStore.Format(KKey(), ++index);
		if ( GetKeyCodeFromConfig(ConfigSection(), tempStore, keyCode, scanCode) )
			{
			TKeyEvent	event = {keyCode, scanCode, 0, 0};
			ws.SimulateKeyEvent(event);
			ws.Flush();

			tempStore.Format(KDelay(), index);
			if ( GetIntFromConfig(ConfigSection(), tempStore, delay) )
				{
				User::After(delay*KDelayMultiplier);
				}
			}
		else
			{
			moreData=EFalse;
			}
		}
	CleanupStack::PopAndDestroy(&ws);

	// test steps return a result
	return TestStepResult();
	}
