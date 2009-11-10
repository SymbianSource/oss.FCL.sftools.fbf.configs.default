// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// @file CT_LbsClientStep.h
// This contains the header file for LBS Client Test Step Base
// 
//

#ifndef __CT_LBS_CLIENT_STEP_H__
#define __CT_LBS_CLIENT_STEP_H__

#include <testexecutestepbase.h>
#include <lbs.h>
#include <lbsnetsimtest.h>

#include "ctlbsclientserver.h"

//todo: is this a sensible place to put this as it's used by many teststeps?
const TTimeIntervalMicroSeconds32 delay10Seconds = 10000000;


class CT_LbsClientStep : public CTestStep
{

public:
	virtual ~CT_LbsClientStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsClientStep(CT_LbsClientServer& aParent);


	// Carryout a notify position update.
	// Must have a connect server, and a open positioner
	// also the pos info array is appened to
	TInt DoNotifyUpdateL(TPositionInfoBase& aPosInfo);
	
	TInt DoLastKnownPosL(TPositionInfoBase& aPosInfo);

	TInt OpenNetSim();
	void SendResetAssistanceData(TLbsAssistanceDataGroup aMask);
	void CloseNetSim();

	CT_LbsClientServer& iParent;

	RPositionServer iServer;
	RPositioner iPositioner;

	/** Net sim required for tests which invoke assistance data via AGPS module. */
	RLbsNetSimTest iNetSim;
	
	/** Reference position used to setup Net sim and later verification. */
	TPosition iRefPos;

		
private:
	/**
	Position call types - used by GetLocation() to determine which position update function to call

	*/
	enum _TPositionUpdateType
		{
		/** EDoNotifyPosUpdate - call NotifyPositionUpdate  */
		EDoNotifyPosUpdate = 0,
		/** EDoLastKnownPos - call GetLastKnownPosition */
		EDoLastKnownPos = 0x01
		};

	TInt GetLocationL(TPositionInfoBase& aPosInfo, TUint32 aNotifyCallType);
	
};

#endif // __CT_LBS_CLIENT_STEP_H__
