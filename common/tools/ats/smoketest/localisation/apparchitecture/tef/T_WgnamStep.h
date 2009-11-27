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

#if (!defined __T_WGNAM_STEP_H__)
#define __T_WGNAM_STEP_H__
#include <w32std.h>
#include "ApparcTestServer.h"

//!  A CT_WgnamStep test class. 
/*!
  Test accessing window group name.
*/
class CT_WgnamStep :  public CTestStep
	{
public:
	CT_WgnamStep();
	~CT_WgnamStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
//private:
	void testWgNamesL();
	void PrintWindowGroupName(const TPtrC& aWndGroupName);
private:
	RWsSession iWsSession;
	};

_LIT(KT_WgnamStep,"T_Wgnam");

#endif
