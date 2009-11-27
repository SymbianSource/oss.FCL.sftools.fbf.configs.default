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

#include "TestSidChecker.h"

#include <e32std.h>
#include <ImplementationProxy.h>
#include <e32property.h>
#include <e32test.h>
#include <e32debug.h>
#include <f32file.h>


TBool E32Dll()
	{
	return (ETrue);
	}

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(0x10281FDE, CTestSidChecker::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

//////////////////////////////
// private class declarations
//////////////////////////////

NONSHARABLE_CLASS(CApFileTestPropertyMonitor) : public CActive
	{
public:
	static CApFileTestPropertyMonitor* NewL(TCallBack aCallBack);
	~CApFileTestPropertyMonitor();
	void Start();

private:
	CApFileTestPropertyMonitor(TCallBack aCallBack);
	void RunL();
	void DoCancel();

private:
	TCallBack iCallBack;
	RProperty iProperty;
	};

NONSHARABLE_CLASS(CApFileTestOneShotTimer) : public CActive
	{
public:
	static CApFileTestOneShotTimer* NewL(TCallBack aCallBack);
	~CApFileTestOneShotTimer();
	void Start(TTimeIntervalMicroSeconds32 aDelay);

private:
	CApFileTestOneShotTimer(TCallBack aCallBack);
	void RunL();
	void DoCancel();

private:
	TCallBack iCallBack;
	RTimer iTimer;
	};

//////////////////////////////
// CApFileTestPropertyMonitor
//////////////////////////////

CApFileTestPropertyMonitor* CApFileTestPropertyMonitor::NewL(TCallBack aCallBack)
	{	
	CApFileTestPropertyMonitor* self = new(ELeave) CApFileTestPropertyMonitor(aCallBack);
	TInt err = self->iProperty.Attach(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey);
	if(err != KErrNone)
		{
		delete self;
		User::Leave(err);
		}
	return self;
	}

CApFileTestPropertyMonitor::CApFileTestPropertyMonitor(TCallBack aCallBack) :
		CActive(EPriorityHigh),
		iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

CApFileTestPropertyMonitor::~CApFileTestPropertyMonitor()
	{
	Cancel();
	iProperty.Close();
	}

void CApFileTestPropertyMonitor::Start()
	{
	iProperty.Subscribe(iStatus);
	SetActive();
	}

void CApFileTestPropertyMonitor::RunL()
	{
	if(iStatus.Int() != KErrNone)
		{
		RDebug::Print(_L("TestSidChecker: Property subcribe failed, error code: %i"), iStatus.Int());
		}
	iCallBack.CallBack();
	}

void CApFileTestPropertyMonitor::DoCancel()
	{
	iProperty.Cancel();
	}

//////////////////////////////
// CApFileTestOneShotTimer
//////////////////////////////

CApFileTestOneShotTimer* CApFileTestOneShotTimer::NewL(TCallBack aCallBack)
	{
	CApFileTestOneShotTimer* self = new(ELeave) CApFileTestOneShotTimer(aCallBack);
	TInt err = self->iTimer.CreateLocal();
	if(err != KErrNone)
		{
		delete self;
		User::Leave(err);
		}
	return self;
	}

CApFileTestOneShotTimer::CApFileTestOneShotTimer(TCallBack aCallBack) :
		CActive(EPriorityHigh),
		iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

CApFileTestOneShotTimer::~CApFileTestOneShotTimer()
	{
	Cancel();
	iTimer.Close();
	}

void CApFileTestOneShotTimer::Start(TTimeIntervalMicroSeconds32 aDelay)
	{
	iTimer.After(iStatus,aDelay);
	SetActive();
	}

void CApFileTestOneShotTimer::RunL()
	{
	if(iStatus.Int() != KErrNone)
		{
		RDebug::Print(_L("TestSidChecker: OneShotTimer Failed, error code: %i"), iStatus.Int());
		}
	iCallBack.CallBack();
	}
	
void CApFileTestOneShotTimer::DoCancel()
	{
	iTimer.Cancel();
	}

//////////////////////////////
// CTestSidChecker
//////////////////////////////

CTestSidChecker* CTestSidChecker::NewL()
	{
	CTestSidChecker* self = new(ELeave) CTestSidChecker();
	return self;
	}

CTestSidChecker::CTestSidChecker()
	{
	}
	
CTestSidChecker::~CTestSidChecker()
	{
	}

TBool CTestSidChecker::AppRegisteredAt(const TUid& aSid, TDriveUnit aDrive)
	{
	TBool ret = EFalse;
	if(aSid == KApFileTestBadApp)
		{
		ret = EFalse;
		}
	else if(aSid == KApFileTestGoodApp)
		{
		#ifdef __WINS__
		TDriveUnit drive(EDriveX);
		#else
		TDriveUnit drive(EDriveX);
		RFs	fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		//The removable media is expected at D: on NAND ROM and at E: on normal ROMs.
		//The following code works on techview but not guaranteed to work on all platforms. 
		TDriveInfo driveInfo;
		TInt error = fs.Drive(driveInfo, EDriveD);
		if(error == KErrNone && ((driveInfo.iDriveAtt & KDriveAttRemovable) != 0))
			{
		 	// Use drive D
		 	drive = EDriveD;
		 	}
		 else
		 	{
			error = fs.Drive(driveInfo, EDriveE);
			if(error == KErrNone && ((driveInfo.iDriveAtt & KDriveAttRemovable) != 0))
				{
			 	// Use drive E
			 	drive = EDriveE;
			 	}
			}
		fs.Close();
		CleanupStack::PopAndDestroy(&fs);
		#endif
		ret = (aDrive == drive);
		}
	else if(aSid == KApFileTestCallBackApp)
		{
		ret = ETrue;
		CallBackAppChecked();
		}
	else if(aSid == KApFileTestForcedRegistration)
		{
		ret = EFalse;
		}
	else if(aSid == KApTriggerRescan)
		{
		ret = ETrue;
		if (!iRescanTriggered)
			{
			iRescanTriggered = ETrue;
			iRescanCallBack.CallBack();
			}
		}
	else if(aSid == KApFileTestAppWithMultipleRegistrationFiles)
		{
		ret = ETrue;
		AppWithMultipleRegistrationFilesChecked();
		}
	else
		{
		Error(_L("TestSidChecker: Unknown sid %x"), aSid.iUid);
		}
	return ret;
	}
	
void CTestSidChecker::SetRescanCallBackL(const TCallBack &aCallBack)
	{
	iRescanCallBack = aCallBack;
	if(aCallBack.iFunction)
		{
		iMonitor = CApFileTestPropertyMonitor::NewL(TCallBack(&PropertyEventCallBack,this));
		iTimer = CApFileTestOneShotTimer::NewL(TCallBack(&TimerEventCallBack,this));
		}
	}

void CTestSidChecker::AppWithMultipleRegistrationFilesChecked()
	{
		TInt value;
		TInt err = RProperty::Get(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, value);
		if(err == KErrNone)
			{
			if(value == ECheckedOnce)
				{
				TInt err = RProperty::Set(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, ECheckedTwice);
				if(err != KErrNone)
					{
					Error(_L("TestSidChecker: Property set failed, error code: %i"), err);
					}
				}
			else if(value == ECheckedTwice)
				{
				TInt err = RProperty::Set(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, ECheckedThrice);
				if(err != KErrNone)
					{
					Error(_L("TestSidChecker: Property set failed, error code: %i"), err);
					}
				}
			else
				{
				TInt err = RProperty::Set(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, ECheckedOnce);
				if(err != KErrNone)
					{
					Error(_L("TestSidChecker: Property set failed, error code: %i"), err);
					}
				}
			}
	}
	
void CTestSidChecker::CallBackAppChecked()
	{
	if(iState == ENotStarted)
		{
		TInt value;
		TInt err = RProperty::Get(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, value);
		if(err == KErrNone)
			{
			if(value == EPluginLoad)
				{
				iMonitor->Start();
				iState = EMonitorStarted;
				}
			else
				{
				Error(_L("TestSidChecker: Bad property value %u"), value);
				}
			}
		else
			{
			// don't fail here, it's probably just a stale reg file.
			}
		}
	else if(iState == EMonitorStarted)
		{
		// do nothing, it's a random scan happening before we start one manually
		}
	else if(iState == EScanStarted)
		{
		iState = EAppChecked;
		TInt err = RProperty::Set(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, EScanOccurred);
		if(err != KErrNone)
			{
			Error(_L("TestSidChecker: Property set failed, error code: %i"), err);
			}
		}
	else
		{
		Error(_L("TestSidChecker: Unexpected state %u"), iState);
		}
	}

TInt CTestSidChecker::PropertyEventCallBack(TAny* aSelf)
	{	
	return STATIC_CAST(CTestSidChecker*,aSelf)->PropertyEvent();
	}

TInt CTestSidChecker::PropertyEvent()
	{
	if(iState == EMonitorStarted)
		{
		TInt value;
		TInt err = RProperty::Get(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, value);
		if(err == KErrNone)
			{
			if(value == ETriggerScan)
				{
				User::After(1000000); // wait a second for the testexecute to subscribe to the property
				iTimer->Start(7 * 1000000); // wait 7 seconds before assuming the callback has failed
				iState = EScanStarted;
				iRescanCallBack.CallBack();
				}
			else
				{
				Error(_L("TestSidChecker: Bad property value %u"), value);
				}
			}
		else
			{
			Error(_L("TestSidChecker: PropertyGetFailed, error code: %i"), err);
			}
		}
	else
		{
		Error(_L("TestSidChecker: Unexpected State %u"), iState);
		}

	return KErrNone;
	}

TInt CTestSidChecker::TimerEventCallBack(TAny* aSelf)
	{
	return STATIC_CAST(CTestSidChecker*,aSelf)->TimerEvent();
	}

TInt CTestSidChecker::TimerEvent()
	{
	if(iState == EScanStarted)
		{
		TInt err = RProperty::Set(KApFileTestPubSubCategory, KApFileTestPubSubCallBackKey, ETimedOut);
		if(err != KErrNone)
			{
			Error(_L("TestSidChecker: Property set failed, error code: %i"), err);
			}
		}
	else if(iState != EAppChecked)
		{
		Error(_L("TestSidChecker: Unexpected state %u"), iState);
		}

	return KErrNone;
	}
	
void CTestSidChecker::Error(TRefByValue<const TDesC> aFmt, TInt aValue)
	{
	// Something rather unexpected happened. Give up, keep quiet.
	RDebug::Print(aFmt,aValue);
	iTimer->Cancel();
	iMonitor->Cancel();
	iState = ECallBackTestError;
	}
