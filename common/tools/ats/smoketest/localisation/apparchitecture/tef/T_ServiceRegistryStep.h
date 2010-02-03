// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code
*/

#if (!defined __T_SERVICEREGISTRY_STEP_H__)
#define __T_SERVICEREGISTRY_STEP_H__

#include "appfwk_test_AppUi.h"
#include <apgnotif.h>

class RApaLsSession;

_LIT(KT_ServiceRegistryStep,"T_ServiceRegistry");

class CT_ServiceRegistryTestStep : public CTestStep
	{
public:
	CT_ServiceRegistryTestStep();
	~CT_ServiceRegistryTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	TInt RunTestCasesL();
	void TestAssociation1L();
	void TestAppForDataTypeAndServiceL(RApaLsSession& aLs);
    void TestUpdateOfServiceRegistryOnAppUninstallationL(RApaLsSession& aLs);   
    CActiveScheduler* iScheduler;    
    };
    
class CTestApplistChangeObserver : public CBase, public MApaAppListServObserver
    {
public: // from MApaAppListServObserver
    void HandleAppListEvent(TInt aEvent);
public:
    CApaAppListNotifier* iNotifier;
    };
	
#endif
