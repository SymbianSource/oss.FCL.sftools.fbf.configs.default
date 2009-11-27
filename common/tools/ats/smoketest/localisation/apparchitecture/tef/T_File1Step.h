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

#if (!defined __T_FILE1_STEP_H__)
#define __T_FILE1_STEP_H__
#include "ApparcTestServer.h"

class CApaScanningFileRecognizer;

//!  A CT_File1Step test class. 

/**   A more elaborate class description. */

class CT_File1Step : public CTestStep
	{
public:
	CT_File1Step();
	~CT_File1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void testScanningFileTheRecognizerL();
	void setup();
private:
	RFs iFs;
	CApaScanningFileRecognizer* iRecognizer;
	};

_LIT(KT_File1Step,"T_File1");
#endif
