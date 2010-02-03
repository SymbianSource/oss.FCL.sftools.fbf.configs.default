// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalComponent - Internal Symbian test code  
*/


#include <E32STD.H>
#include <E32BASE.H>
#include <E32PROPERTY.H>
#include <APACMDLN.H>
#include <APAID.H>
#include <APGCLI.H>
#include <APGICNFL.H>
#include <APMSTD.H>

const TUint KPropertyCategory=0x101F289C;
const TUint KPropertyKey=0;

_LIT(KLitExtension, ".NNAPP2");

inline void DoTest(TBool aCondition) {__ASSERT_ALWAYS(aCondition, User::Invariant());}

// top-level functions

LOCAL_C void MainL()
	{
	CApaCommandLine* commandLine=NULL;
	User::LeaveIfError(CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine));
	CleanupStack::PushL(commandLine);
	const TPtrC executableName(commandLine->ExecutableName());
	DoTest(TParsePtrC(executableName).Ext().CompareF(KLitExtension)==0);
	User::LeaveIfError(RProperty::Set(TUid::Uid(KPropertyCategory), KPropertyKey, commandLine->DocumentName()));
	CleanupStack::PopAndDestroy(commandLine);
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* const trapCleanup=CTrapCleanup::New();
	if (trapCleanup==NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(error, MainL());
	DoTest(error==KErrNone);
	delete trapCleanup;
	return error;
	}
