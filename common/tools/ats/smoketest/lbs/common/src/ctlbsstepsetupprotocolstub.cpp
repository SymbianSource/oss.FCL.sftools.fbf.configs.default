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
// @file ctlbsstep_setupprotocolstub.cpp
// This is the class implementation for the Set Autonomous Moder Test Step
// 
//



/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepsetupprotocolstub.h"
#include <lbsadmin.h>

CT_LbsStep_SetupProtocolStub::CT_LbsStep_SetupProtocolStub(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetupProtocolStub);
	}

/**
Static Constructor
*/
CT_LbsStep_SetupProtocolStub* CT_LbsStep_SetupProtocolStub::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetupProtocolStub(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
@pre 	
@return Test verdict
@post	GPSMode set to autonomous
*/
TVerdict CT_LbsStep_SetupProtocolStub::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetupProtocolStub::doTestStepL()"));

	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	TUid uid;

	TInt err = adminApi->Set(KLbsSettingHomeProtocolModule, TUid::Uid(0x10282293));	
	User::LeaveIfError(err);
	err = adminApi->Get(KLbsSettingHomeProtocolModule, uid);	
	User::LeaveIfError(err);
	
	if(uid.iUid != 0x10282293)
		{
		INFO_PRINTF1(_L("Admin setting didn't work"));
		SetTestStepResult(EFail);
		}					
		
	CleanupStack::PopAndDestroy(adminApi);

	// Alow he setting to be propagated
	User::After(2000000);

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetupProtocolStub::doTestStepL()"));

	return TestStepResult();
	}
