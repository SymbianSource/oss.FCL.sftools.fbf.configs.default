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

 
#if (!defined __T_DATAMAPPINGPERSISTENCEB_STEP_H__)
#define __T_DATAMAPPINGPERSISTENCEB_STEP_H__

#include "appfwk_test_AppUi.h"
#include "T_RApaLsSessionStep.h"

_LIT(KT_DataMappingPersistenceBStep,"T_DataMappingPersistenceB");

class CT_DataMappingPersistenceBTestStep : public CTestStep
	{
public:
	CT_DataMappingPersistenceBTestStep();
	~CT_DataMappingPersistenceBTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	TInt RunTestCasesL();
	void TestDeleteDataMappingL();

private:
	RTestableApaLsSession iSession;

	};
	
#endif
