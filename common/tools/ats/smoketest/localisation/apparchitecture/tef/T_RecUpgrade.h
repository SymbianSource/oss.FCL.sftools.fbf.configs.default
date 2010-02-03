// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef		__T_RECUPGRADE_H
#define		__T_RECUPGRADE_H

#include 	<testexecutestepbase.h>

_LIT(KT_RecUpgradeStep, "T_RecUpgrade");

class CT_RecUpgradeStep : public CTestStep
	{
public:
	CT_RecUpgradeStep();
	~CT_RecUpgradeStep();
	
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void TestUpgradeRecognizerL(RApaLsSession& aApparcServer);
	TBool DoRecognizeL(RApaLsSession& aApparcServer, const TDataType& aDataType);
	void CopyRecognizerFileL();
	};

#endif
