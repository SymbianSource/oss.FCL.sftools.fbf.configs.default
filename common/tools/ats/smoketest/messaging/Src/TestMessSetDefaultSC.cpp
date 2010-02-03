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
// This contains TestMessSetDefaultSc which sets an SMS
// SC as default SC.
// 
//

#include "TestMessSetDefaultSC.h"

// EPOC includes
#include <msvids.h>
#include <miutset.h>
#include <smutset.h>
#include <etelmm.h>
#include <msvstore.h>
#include <csmsaccount.h>

_LIT(KSCName,		"scname");

CTestMessSetDefaultSc::CTestMessSetDefaultSc()
:	CTestMessBase(EFalse)
	{
	SetTestStepName(_L("SetSmsSc"));
	}

TVerdict CTestMessSetDefaultSc::doTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("Set Default Service Center"));

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

		//	Log child
		if	(	EntryL().Entry().iMtm == KUidMsgTypeSMS &&
				EntryL().Entry().iType == KUidMsvServiceEntry
			)
			{
			found=ETrue;
			}
		}

	if ( !found )
		{
		ERR_PRINTF1(_L("No SMS Account found"));
		SetTestStepResult(EFail);
		}

	TName	network;
//IMK	TBool	returnValue=(GetPhoneSmsNetworkL(network)==KErrNone);
TBool	returnValue=false; //IMK
	TPtrC	ptrSCName(network);

	if ( !returnValue )
		{
		returnValue=GetStringFromConfig(ConfigSection(), KSCName, ptrSCName);
		}
	if ( !returnValue )
		{
		ERR_PRINTF1(_L("No SC specified in script/ini file"));
		SetTestStepResult(EFail);
		}

	INFO_PRINTF2(_L("SC Name = %S"), &ptrSCName);

	if ( TestStepResult()==EPass )
		{
		CSmsAccount*	smsAccount = CSmsAccount::NewLC();
		smsAccount->LoadSettingsL(*smsSettings);
		const TInt	numSCAddr = smsSettings->ServiceCenterCount();

		if( !numSCAddr )
			{
			ERR_PRINTF1(_L("No SC found"));
			SetTestStepResult(EFail);
			}

		TBool	scFound	= EFalse;
		for( TInt index=0; (index<numSCAddr) && (!scFound);index++ )
			{
			CSmsServiceCenter&	scAddr = smsSettings->GetServiceCenter(index);
			if( (scAddr.Name()).CompareF(ptrSCName) == 0)
				{
				smsSettings->SetDefaultServiceCenter(index);
				smsAccount->SaveSettingsL(*smsSettings);
				scFound = ETrue;
				}
			}

		if ( !scFound )
			{
			ERR_PRINTF2(_L("%S NOT Found"), &ptrSCName);
			SetTestStepResult(EFail);
			}

		CleanupStack::PopAndDestroy(smsAccount);
		}
	CleanupStack::PopAndDestroy(2, smsSettings);

	return TestStepResult();
	}
