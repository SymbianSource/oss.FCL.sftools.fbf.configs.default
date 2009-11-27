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
 @internalComponent - Internal Symbian test code	 
*/

#ifndef		__T_APSSCAN_H
#define		__T_APSSCAN_H

#include "appfwk_test_AppUi.h"

_LIT(KT_ApsScanStep, "T_ApsScan");

class CT_ApsScanStep : public CTestStep
	{
public:
	CT_ApsScanStep();
	~CT_ApsScanStep();
	
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void TestDynInstallRecognizerL(RApaLsSession& aApparcServer);
	TBool DoRecognizeL(RApaLsSession& aApparcServer, const TDataType& aDataType);
	void CopyRecognizerFileL();
	};

#endif
