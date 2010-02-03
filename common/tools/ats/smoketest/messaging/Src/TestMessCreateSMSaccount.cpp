// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestMessCreateSmsAccount which creates an SMS
// account folder for GSM or CDMA
// 
//

#include "TestMessCreateSmsAccount.h"
#include "TestMess.h"
#include "Configuration.cfg"

// EPOC includes
#include <msvids.h>
#include <miutset.h>
#include <msvstore.h>

#include "TestMessCreateGsmSmsUtil.h"
//Usage of Macro for CDMA mtm
#if (defined CDMA_API_ENABLED)
#include "TestMessCreateCdmaSmsUtil.h"
#endif

#include <csmsaccount.h>
#include <MsvScheduleSettings.h>
#include <MsvOffPeakTime.h>
#include <MsvSendErrorAction.h>
#include <MsvSysAgentAction.h>

/*@{*/
_LIT(KSCName,				"scname");
_LIT(KSCNumber,				"scnumber");

_LIT(KPtGSM,				"GSM");
#if (defined CDMA_API_ENABLED)
_LIT(KPtMessageBearerType,	"MessageBearerType");
_LIT(KPtCDMA,				"CDMA");		
#endif
/*@}*/

CTestMessCreateSmsAccount::CTestMessCreateSmsAccount()
:	CTestMessBase(EFalse)
	{
	SetTestStepName(_L("CreateSmsAccount"));
	}

/**
 Creates the SMS account folder for GSM or CDMA. For CDMA mtm,
 it creates the respective utility class for creating the account
 @return TVerdict
*/
TVerdict CTestMessCreateSmsAccount::doTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("Create SMS account"));

	TPtrC	ptrSCName;
	TBool	returnValue =GetStringFromConfig(ConfigSection(), KSCName, ptrSCName);
	INFO_PRINTF2(_L("SC Name = %S"), &ptrSCName);

	TPtrC	ptrSCNumber;
	returnValue =GetStringFromConfig(ConfigSection(), KSCNumber, ptrSCNumber);
	INFO_PRINTF2(_L("SC Number = %S"), &ptrSCNumber);

	CSmsAccount*			smsAccount = CSmsAccount::NewLC();
	CMsvScheduleSettings*	scheduleSetting = CMsvScheduleSettings::NewLC();
	CMsvOffPeakTimes*		offPeakTimes=new (ELeave) CMsvOffPeakTimes();
	CleanupStack::PushL(offPeakTimes);
	CMsvSendErrorActions*	errorActions=CMsvSendErrorActions::NewLC();
	CMsvSysAgentActions*	sysAgentActions=new (ELeave) CMsvSysAgentActions();
	CleanupStack::PushL(sysAgentActions);
	CSmsSettings*		smsSettings = CSmsSettings::NewL();
	CleanupStack::PushL(smsSettings);
	SetEntryL(KMsvRootIndexEntryId);

	EntryL().SetSortTypeL(SelectionOrdering());
	CMsvEntrySelection*	selection=EntryL().ChildrenWithTypeL(KUidMsvServiceEntry);
	CleanupStack::PushL(selection);
	TInt				count=selection->Count();
	TBool				found=EFalse;
	for (TInt i=count; i>0 && !found; )
		{
		SetEntryL(selection->At(--i));

		// Only one Sms Service allowed
		if	(	EntryL().Entry().iMtm == KUidMsgTypeSMS &&
				EntryL().Entry().iType == KUidMsvServiceEntry
			)
			{
			found=ETrue;
			}
		}

	if ( !found )
		{
		smsAccount->InitialiseDefaultSettingsL(*smsSettings);
		smsAccount->InitialiseDefaultSettingsL(*scheduleSetting, *offPeakTimes, *errorActions, *sysAgentActions);

		TPtrC	messageBearerType;
		messageBearerType.Set(KPtGSM);
#if (defined CDMA_API_ENABLED)
		GetStringFromConfig(ConfigSection(), KPtMessageBearerType, messageBearerType);
		INFO_PRINTF2(_L("The message bearer type is : %S"), &messageBearerType);
#endif

		CTestMessCreateSmsUtilBase*	createSmsUtil = NULL;
		//Creates the util class for GSM or CDMA
		if ( messageBearerType.Compare(KPtGSM)==0 )
			{
			createSmsUtil = new (ELeave) CTestMessCreateGsmSmsUtil(*this);
			}
#if (defined CDMA_API_ENABLED)
		else if( messageBearerType.Compare(KPtCDMA)==0 )
			{
			createSmsUtil = new (ELeave) CTestMessCreateCdmaSmsUtil(*this);
			}
#endif
		else
			{
			WARN_PRINTF1(_L("Unknown bearer type using Gsm"));
			createSmsUtil= new (ELeave) CTestMessCreateGsmSmsUtil(*this);
			}
		CleanupStack::PushL(createSmsUtil);

		//Changes the new account settings
		createSmsUtil->SetSmsAccountSettings(*smsSettings);
		CleanupStack::PopAndDestroy(createSmsUtil);

		smsAccount->SaveSettingsL(*smsSettings);
		smsAccount->SaveSettingsL(*scheduleSetting, *offPeakTimes, *errorActions, *sysAgentActions);
		}

	if ( TestStepResult()==EPass )
		{
		smsAccount->LoadSettingsL(*smsSettings);
		smsSettings->AddServiceCenterL(ptrSCName, ptrSCNumber);
		smsAccount->SaveSettingsL(*smsSettings);
		}

	CleanupStack::PopAndDestroy(2, smsSettings);
	CleanupStack::PopAndDestroy(5, smsAccount);

	return TestStepResult();
	}
