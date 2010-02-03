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

#if (!defined __T_BACKUP_STEP_H__)
#define __T_BACKUP_STEP_H__
#include "ApparcTestServer.h"
#include <babackup.h>

class CT_BackupStep;
struct CBackupTestThreadParams
		{
			TInt64 seed;
			CT_BackupStep* Step;
		};



//!  A CT_BackupStep test class. 
/*!
  Performs Tests on Backup server
*/
class CT_BackupStep :public CTestStep 
	{
public:
	CT_BackupStep();
	~CT_BackupStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	TInt DoBackupMultipleTestThreadEntryL(/*TAny* aPtr*/);	

//private:
	void doMainL();
	void SetupSchedulerL();
	void CloseScheduler();
	void doTestsL();
	void testMultipleClientsL();
	void testRestartAppsL();
	TRequestStatus testExtraRestartLockedOutL();
	void testGetsCallbackOnBackupWrapperKillOtherL();
	void testGetsCallbackOnBackupWrapperKillL();
	void testGetsCallbackOnBackupStartInSecondWrapperL();
	void testGetsCallbackOnBackupEndL();
	void testGetsCallbackOnBackupStartL();
	void FlushSchedulerQueueL();
	static TInt BackupMultipleTestThreadEntry(TAny* aPtr);
	
private:
	CBaBackupSessionWrapper* gWrapper;
	CActiveScheduler *testScheduler;
	};

_LIT(KT_BackupStep,"T_Backup");

//**********************************
// CTestActive
//**********************************

class CTestActive : public CActive
	{
public:
	CTestActive();
	~CTestActive();
	void StartL();
	void StartL(TRequestStatus& aRequestStatus);
protected:
	void DoCancel();
	void RunL();

	TRequestStatus* iReport;
	};

// Class to test the n-th recieved event from the backup server is the same as the event
// the object is initialised with


class CTestBackupOperationObserver : public CTestActive, public MBackupOperationObserver
	{
public:
	virtual void HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes);
	CTestBackupOperationObserver(const TBackupOperationAttributes& aEvent, CTestExecuteLogger& aLogger);
	CTestBackupOperationObserver(const TBackupOperationAttributes& aEvent, TInt aCount, CTestExecuteLogger& aLogger);
	void SetReadyToGo();/*{iCount=KOneOfOurEvents;};*/
	inline CTestExecuteLogger& Logger() {return iLogger;};
private:
	TBackupOperationAttributes iEvent;
	TInt iCount;
	CTestExecuteLogger iLogger;
	};

class CThreadLaunchCounter : public CActive
	{
public:
	CThreadLaunchCounter();
	~CThreadLaunchCounter();
	void Start();
protected:
	void DoCancel(){};
	void RunL();
private:
	TInt gCount;
	};

#endif
