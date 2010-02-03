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
// This contains CTestAppLoaderAppStart
// 
//

// USER includes
#include "TestAppLoaderAppStart.h"

// EPOC includes
#include <apacmdln.h>
#include <apgcli.h>

/*@{*/
/// Parameters
_LIT(KProgram,					"program");

/// Error messages
_LIT(KErrCode,					"Error %d");
_LIT(KErrMissingParameter ,		"Misssing Parameter %S");
/*@}*/

// constructor
CTestAppLoaderAppStart::CTestAppLoaderAppStart()
:	CTestAppLoaderBase()
	{
	SetTestStepName(_L("AppStart"));
	}

// Each test step must supply a implementation for doTestStepL
enum TVerdict CTestAppLoaderAppStart::doTestStepL( void )
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("TEST-> APP START"));

	TPtrC	program;
	if ( !GetStringFromConfig(ConfigSection(), KProgram, program) )
		{
		ERR_PRINTF2(KErrMissingParameter, &KProgram());
		SetTestStepResult(EFail);
		}
	else
		{
		RApaLsSession		apaLsSession;
		User::LeaveIfError(apaLsSession.Connect());
		CleanupClosePushL(apaLsSession);

		CApaCommandLine*	cmdLine=CApaCommandLine::NewLC();
		cmdLine->SetExecutableNameL(program);
		cmdLine->SetCommandL(EApaCommandRun);

		TInt	err=apaLsSession.StartApp(*cmdLine);
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(KErrCode, err);
			SetTestStepError(err);
			}
		CleanupStack::PopAndDestroy(2, &apaLsSession);
		}

	// test steps return a result
	return TestStepResult();
	}
