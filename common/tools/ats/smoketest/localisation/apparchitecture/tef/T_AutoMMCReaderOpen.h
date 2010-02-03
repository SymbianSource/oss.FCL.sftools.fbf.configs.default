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

#ifndef		__T_AUTOMMCREADEROPEN_H
#define		__T_AUTOMMCREADEROPEN_H

#include "appfwk_test_AppUi.h"

_LIT(KTestAutoMMCReaderOpenStep, "T_AutoMMCReaderOpen");

class CTestAutoMMCReaderStep : public CTestStep
	{
public:
	CTestAutoMMCReaderStep();
	~CTestAutoMMCReaderStep();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void OpenMMCCardReaderDrive();
	};

#endif
