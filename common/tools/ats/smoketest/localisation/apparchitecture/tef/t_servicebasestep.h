// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __T_SERVICEBASESTEP_H__)
#define __T_SERVICEBASESTEP_H__

#include <apaserverapp.h>
#include <apgcli.h>
#include "apparctestserver.h"


// RTstServiceApp

class RTstServiceApp : public RApaAppServiceBase
	{
public:
	static TInt DoTestTransferSessionL(TUid aServiceUid, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath);
	static TInt DoTestTransferSessionAndBackL(TUid aServiceUid, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath);
	static TInt DoTestTransferWithUnconnectedSessionL(TUid aServiceUid, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath);
	RTstServiceApp(TUid aServiceUid);
	void ConnectL();
	void ReceiveTestResult(TRequestStatus& aRequestStatus, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath);
	// from RApaAppServiceBase
	virtual TUid ServiceUid() const;
private:
	const TUid iServiceUid;
	};



// CT_ServiceBaseStep test class. 

class CT_ServiceBaseStep : public CTestStep
	{
public:
	CT_ServiceBaseStep();
	~CT_ServiceBaseStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	void TestServiceAppL(RApaLsSession& aApparcServer, RFs& aFileServer);
	void TestPanicWithWrongInstanceL();
	void OpenFileLC(RFile& aFile, RFs& aFileServer, const TDesC& aFullFileName);
	void ClosePanicWindowL();
	void TestTransferBackL(RApaLsSession& aApparcServer, RFs& aFileServer);
	void TestTransferWithUnconnectedSessionL();
	};

_LIT(KT_ServiceBaseStep,"T_ServiceBase");
	
#endif
