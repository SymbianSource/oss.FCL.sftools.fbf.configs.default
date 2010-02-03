// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTSIDCHECKER_H__
#define __TESTSIDCHECKER_H__

#include <ApSidChecker.h>
#include <e32property.h>

const TUid  KApFileTestAppType              = {0x10281FDB};
const TUid  KApFileTestBadApp               = {0x10281FDC};
const TUid  KApFileTestGoodApp              = {0x10281FDD};
const TUid  KApFileTestAppTypeSidChecker    = {0x10281FDE};
const TUid  KApFileTestAppTypeSidCheckerDll = {0x10281FDF};
const TUid  KApFileTestPubSubCategory       = {0x101F289C}; // apparc test server sid
const TUid  KApFileTestCallBackApp          = {0x10281FE0};
const TUid	KApFileTestForcedRegistration	= {0x102825B5};
const TUid	KApTriggerRescan				= {0x102825B6};
// randomly picked, to try to avoid anything else in the apparctestserver using pub/sub
const TUint KApFileTestPubSubCallBackKey    =  0x0000d3f9;
const TUid KApFileTestAppWithMultipleRegistrationFiles    = {0xA000D107};

class CApFileTestPropertyMonitor;
class CApFileTestOneShotTimer;

NONSHARABLE_CLASS(CTestSidChecker) : public CAppSidChecker
/** Test Sid checker.

This is an implementation of CAppSidChecker for checking native application Sids

This instance of the CAppSidChecker is used to verify that a native application
installed and can be launched.

@internalComponent */
	{
public:
	enum TPropertyValue
		{
		EPropertyDefault,
		EPluginLoad,
		ETriggerScan,
		EScanOccurred,
		ETimedOut,
		EOtherError,
		ECheckedOnce,
		ECheckedTwice,
		ECheckedThrice
		};
	static CTestSidChecker* NewL();
	~CTestSidChecker();

public:
	virtual TBool AppRegisteredAt(const TUid& aSid, TDriveUnit aDrive);
	virtual void SetRescanCallBackL(const TCallBack &aCallback);

private:
	CTestSidChecker();
	void CallBackAppChecked();
	void AppWithMultipleRegistrationFilesChecked();
	static TInt PropertyEventCallBack(TAny* aSelf);
	TInt PropertyEvent();
	static TInt TimerEventCallBack(TAny* aSelf);
	TInt TimerEvent();
	void Error(TRefByValue<const TDesC> aFmt, TInt aValue);

private:
	enum TState
		{
		ENotStarted,
		EMonitorStarted,
		EScanStarted,
		EAppChecked,
		ECallBackTestError
		};
	TCallBack iRescanCallBack;
	CApFileTestPropertyMonitor* iMonitor;
	CApFileTestOneShotTimer* iTimer;
	TState iState;
	TBool iRescanTriggered;
	};

#endif
