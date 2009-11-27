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
// The following test case is used to test writable APIs in RApaLsSession and to test whether
// apparctestserver process has the capability to access those APIs.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#if !defined(__T_CAPABILITY1_H__)
#define __T_CAPABILITY1_H__

#include "ApparcTestServer.h"
#include "appfwk_test_AppUi.h"

class RApaLsSession;

//! CT_Capability1
/** 
  This class is used to test writable apis in RApaLsSession
*/
class CT_Capability1 : public CTestStep
	{
public:
	CT_Capability1();
	virtual TVerdict doTestStepL();
	void ExecuteL(RApaLsSession& aLs);
private:
	~CT_Capability1();
private:
	};

_LIT(KT_Capability1,"T_Capability1");

#endif
