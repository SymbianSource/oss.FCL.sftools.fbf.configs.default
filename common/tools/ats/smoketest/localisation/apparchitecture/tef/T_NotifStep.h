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
 @test
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __T_NOTIF_STEP_H__)
#define __T_NOTIF_STEP_H__
#include <apgnotif.h>
#include "ApparcTestServer.h"
#include "testableapalssession.h"

//!  A CT_NotifStep test class. 

/**  Checks for notifications when application list changes. */

class CT_NotifStep :public CTestStep
	{
public:
	CT_NotifStep();
	~CT_NotifStep();
	virtual TVerdict doTestStepL();
	void TestAppNotificationL();
	void TestIconFileNotificationL();
	
private:
	void CreateAppL(const TDesC& aAppName);
	void DeleteAppL(const TDesC& aAppName);
	void ModifyIconFileTimeStamp();
	
private:
	CActiveScheduler* iScheduler;
	RFs	iFs;
 	RApaLsSession iSession;
	RSmlTestUtils iUtils;
	};

class CTestObserver : public CBase, public MApaAppListServObserver
	{
public: // from MApaAppListServObserver
	void HandleAppListEvent(TInt aEvent);
	
public:
	TInt iNotified;
	CApaAppListNotifier* iNotifier;
	};

_LIT(KT_NotifStep,"T_Notif");

#endif
