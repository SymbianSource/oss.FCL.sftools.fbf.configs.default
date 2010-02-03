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

#ifndef __CONSOLEALARMALERTCONSOLE_H__
#define __CONSOLEALARMALERTCONSOLE_H__

// System includes
#include <e32base.h>

// User includes
#include "asaltdefs.h"

// Type definitions

// Constants

// Classes referenced
class CConsoleAlarmAlertSession;

// Enumerations


///////////////////////////////////////////////////////////////////////////////////////
// ----> CConsoleAlarmAlertConsole (header)
///////////////////////////////////////////////////////////////////////////////////////
class CConsoleAlarmAlertConsole : public CActive
	{
///////////////////////////////////////////////////////////////////////////////////////
public:										// CONSTRUCT / DESTRUCT
///////////////////////////////////////////////////////////////////////////////////////
	CConsoleAlarmAlertConsole(CConsoleAlarmAlertSession& aSession);
	~CConsoleAlarmAlertConsole();

///////////////////////////////////////////////////////////////////////////////////////
public:										// ACCESS
///////////////////////////////////////////////////////////////////////////////////////
	void									GetKey();
	void									CancelKey();
	void									DecCounter();
	void									UpdateDisplay();
	TInt									GetTimeInterval() const;
	void									SetVisibility(TInt aVis);
	void									SetAlertState(TInt aFlags);

///////////////////////////////////////////////////////////////////////////////////////
protected:									// FROM CActive
///////////////////////////////////////////////////////////////////////////////////////
	void									RunL();
	void									DoCancel();

///////////////////////////////////////////////////////////////////////////////////////
protected:									// INTERNAL METHODS
///////////////////////////////////////////////////////////////////////////////////////
	void									Hide();
	void									CreateWin();
	void									DoDeferTime(TASAltAlertServerResponse aResponse);

	static TBool							CountDownCallBack(TAny* aSelf);

///////////////////////////////////////////////////////////////////////////////////////
private:									// MEMBER DATA
///////////////////////////////////////////////////////////////////////////////////////

	TInt									iAlertState;

	TInt									iTimeInterval;

	CPeriodic*								iCountDown;

	CConsoleBase*							iWin;

	CConsoleAlarmAlertSession&				iSession;

	friend class							CConsoleAlarmAlertSession;

	TInt									iInstructionIndex;
	};

#endif
