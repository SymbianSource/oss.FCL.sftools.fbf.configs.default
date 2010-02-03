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
// This contains CTestAppLoaderKillProcess
// 
//

// USER includes
#include "TestAppLoaderKillProcess.h"

// EPOC includes
#include <w32std.h>
#include <coemain.h>
#include <apgtask.h>
#include <apgcli.h>
#include <apgwgnam.H>

/*@{*/
/// Parameters
_LIT(KProgram,					"program");

/// Log messages
_LIT(KLogProcess,				"Process '%S'.");

/// Error messages
_LIT(KErrMissingParameter,		"Misssing Parameter %S");
_LIT(KErrProcessNotFound,		"Process '%S' not found");
/*@}*/

// constructor
CTestAppLoaderKillProcess::CTestAppLoaderKillProcess()
:	CTestAppLoaderBase()
	{
	SetTestStepName(_L("KillProcess"));
	}

// Each test step must supply a implementation for doTestStepL
enum TVerdict CTestAppLoaderKillProcess::doTestStepL( void )
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("TEST-> KILL PROCESS"));

	TPtrC	program;
	if ( !GetStringFromConfig(ConfigSection(), KProgram, program) )
		{
		ERR_PRINTF2(KErrMissingParameter, &KProgram());
		SetTestStepResult(EFail);
		}
	else
		{
	    TFindProcess	findProcess;
	    TFullName		currentProcessName;
	    RProcess		process;
	    TBool			searching=ETrue;
	    while ( (findProcess.Next(currentProcessName)==KErrNone) && searching )
	    	{
	        User::LeaveIfError(process.Open(currentProcessName));
	        CleanupClosePushL(process);

	        HBufC*	fileName = process.FileName().AllocLC();

	        TPtrC	name=*fileName;
	        INFO_PRINTF2(KLogProcess, &name);
	        if ( program.CompareC(name)==0 )
	        	{
	        	searching=EFalse;
	        	process.Kill(0);
	        	}
	        CleanupStack::PopAndDestroy(2, &process);   // cmdLine, fileName, process
			}
	    if ( searching )
	    	{
	        ERR_PRINTF2(KErrProcessNotFound, &program);
			SetTestStepResult(EFail);
	    	}
		}

	// test steps return a result
	return TestStepResult();
	}
