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
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code	 
*/

#ifndef		__T_LARGESTACK_H
#define		__T_LARGESTACK_H

#include "appfwk_test_AppUi.h"

_LIT(KT_LargeStackStep, "T_LargeStack");

class RTestApaLsSession : public RApaLsSession
	{
public:
	TInt TestExeRecognizerL(const CApaCommandLine& aCommandLine);
	};

class CT_LargeStackStep : public CTestStep
	{
public:
	CT_LargeStackStep();
	~CT_LargeStackStep();
	
	//from CTestStep
	TVerdict doTestStepL();

private:
	void TestLargeStackL();

private:
	RTestApaLsSession iApaLsSession;
	};

#endif
