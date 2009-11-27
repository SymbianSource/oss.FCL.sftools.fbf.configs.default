// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/


#ifndef		__T_APPLIST_H
#define		__T_APPLIST_H

#include "appfwk_test_AppUi.h"
#include "testableapalssession.h"

_LIT(KTestAppListStep, "T_AppList");

//! CTestAppListStep
/*! 
  This class is used to test applist apis (GetAllApps, GetNextApp, GetAppInfo)
  in RApaLsSession
*/
class CTestAppListStep : public CTestStep
	{
public:
	CTestAppListStep();
	~CTestAppListStep();
	void TestAppList(); 
	virtual TVerdict doTestStepL();
private:
	RTestableApaLsSession iApaLsSession;
	};

#endif
