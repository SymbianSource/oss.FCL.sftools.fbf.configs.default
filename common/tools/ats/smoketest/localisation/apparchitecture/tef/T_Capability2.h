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
// The following test case is used to test writable APIs in RApaLsSession and to test whether
// apparctestserver process has the capability to access those APIs.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#if !defined(__T_CAPABILITY2_H__)
#define __T_CAPABILITY2_H__

#include "ApparcTestServer.h"
#include "appfwk_test_AppUi.h"

class RApaLsSession;

//! CT_Capability2
/** 
  This class is used to test writable apis in RApaLsSession
*/
class CT_Capability2 : public CTestStep
	{
public:
	CT_Capability2();
	virtual TVerdict doTestStepL();
	void ExecuteL(RApaLsSession& aLs);
private:
	~CT_Capability2();
private:
	};

_LIT(KT_Capability2,"T_Capability2");

#endif
