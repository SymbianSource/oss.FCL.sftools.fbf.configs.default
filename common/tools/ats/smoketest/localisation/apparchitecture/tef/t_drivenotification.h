// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __T_DRIVENOTIFICATION_H__)
#define __T_DRIVENOTIFICATION_H__
#include "ApparcTestServer.h"
#include <apgnotif.h>
#include <apgcli.h>
#include <eikenv.h>

/**  Checks for notifications when application list changes. */

class CT_DriveNotificationStep :public CTestStep
	{
public:
	CT_DriveNotificationStep();
	~CT_DriveNotificationStep();	
	virtual TVerdict doTestStepL();
	void TestDrivesNotificationL();
private:
	TDriveNumber GetMMCDriveL();
	void DeleteRegFileL(TDriveNumber aDriveNumber);
	void CopyRegFileL(TDriveNumber aDriveNumber);
	CActiveScheduler* iScheduler;
	};
	
class CDriveTestObserver : public CBase, public MApaAppListServObserver
	{
public: // from MApaAppListServObserver
	void HandleAppListEvent(TInt aEvent);
public:
	TInt iNotified;
	CApaAppListNotifier* iNotifier;
	};

_LIT(KT_DriveNotificationStep,"T_DriveNotification");
#endif
