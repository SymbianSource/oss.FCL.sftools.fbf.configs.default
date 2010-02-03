// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

 
#ifndef T_ABSTRACT_NONNATIVEAPPS_STEP_H
#define T_ABSTRACT_NONNATIVEAPPS_STEP_H

#include <testexecutestepbase.h>
#include  <appfwk_test_utils.h>
#include "testableapalssession.h"

const TInt KApplicationType = 0x102081A0;
_LIT(KLitNativeExecutable, "z:\\sys\\bin\\dummy.exe");
_LIT(KLitUpdatedAppsList, "c:\\private\\10003a3f\\UpdatedAppsList.bin");

class CT_AbstractNonNativeAppsTestStep : public CTestStep
	{
public:
	CT_AbstractNonNativeAppsTestStep();
	~CT_AbstractNonNativeAppsTestStep();

public:
	struct TTestAppDetails 
		{
		inline TTestAppDetails(TInt aAppUid, const TDesC& aLogicalExecutable, const TDesC& aRegistrationFile) :
				iAppUid(aAppUid),
				iLogicalExecutable(aLogicalExecutable),
				iRegistrationFile(aRegistrationFile)
			{
			}
		TInt iAppUid;
		const TDesC& iLogicalExecutable;
		const TDesC& iRegistrationFile;
		};

protected:
	void doAbstractNonNativeAppsTestStepPreambleL();
	static void CleanupAndReset(RTestableApaLsSession& aSession, const TTestAppDetails& aApp);
	static void CallRegisterL(RTestableApaLsSession& aSession, const TTestAppDetails& aApp);
	static void CompleteRegisterL(RTestableApaLsSession& aSession, const TTestAppDetails& aApp);
	static void CallDeregisterL(RTestableApaLsSession& aSession, const TTestAppDetails& aApp);
	static void CompleteDeregisterL(RTestableApaLsSession& aSession, const TTestAppDetails& aApp);
	TBool FilePresent(const TDesC& aFile);
	TBool FileAbsent(const TDesC& aFile);
	TBool AppPresent(RTestableApaLsSession& aSession, const TTestAppDetails& aApp);
	TBool AppAbsent(RTestableApaLsSession& aSession, const TTestAppDetails& aApp);

protected:
	RTestableApaLsSession iSession;
	RSmlTestUtils iUtils;
	};
	
#endif // T_ABSTRACT_NONNATIVEAPPS_STEP_H
