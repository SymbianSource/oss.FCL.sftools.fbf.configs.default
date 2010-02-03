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

 
#if !defined(__T_RULEBASEDLAUNCHING_STEP_H__)
#define __T_RULEBASEDLAUNCHING_STEP_H__

#include <apgcli.h>
#include "ApparcTestServer.h"
#include "appfwk_test_AppUi.h"
#include <apgtask.h>

class CTRuleBasedLaunchingStep : public CTestStep
	{
public:
	CTRuleBasedLaunchingStep();
	virtual TVerdict doTestStepL();
	void ExecuteL();
private:
	~CTRuleBasedLaunchingStep();
	TInt AppLaunchedL(RApaLsSession& aLs, const TUid& AppUid);
	TInt AppLaunchedL(RApaLsSession& aLs, const TDesC& aAppFullFileName);
	TInt LaunchAppFromAssociatedDocument(RApaLsSession& aLs, const TUid& aAppUid);
	TInt AppClosed(TUid AppUid);
	void ClosedAllTestApp();
	
	void TestLaunchNonNativeApplicationForRuleBasedL(RApaLsSession& aApparcServer);
	void LaunchAppTests1L(RApaLsSession& aLs);
	void LaunchAppTests2L(RApaLsSession& aLs);
	void LaunchAppTests3L(RApaLsSession& aLs);
	void LaunchAppTests4L(RApaLsSession& aLs);
	void LaunchAppTests5L(RApaLsSession& aLs);
	void LaunchAppTests6L(RApaLsSession& aLs);
private:
	RFs iFs;
	RWsSession iWs; 
	};

_LIT(KT_RuleBasedLaunchingStep,"T_RuleBasedLaunching");

#endif
