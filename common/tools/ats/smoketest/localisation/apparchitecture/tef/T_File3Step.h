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



/**
 @file
 @internalComponent - Internal Symbian test code  
*/

#ifndef __T_FILE3_STEP_H__
#define __T_FILE3_STEP_H__

#include "ApparcTestServer.h"

#include "testableapalssession.h"
#include <ecom.h>
#include <appfwk_test_utils.h>

class CTestSidChecker;

//!  A CT_File3Step test class. 

/** Test the CAppSidChecker Interface. */

class CT_File3Step : public CTestStep
	{
public:
	CT_File3Step();
	~CT_File3Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void TestAppPresenceL();
	void TestRescanCallBackL();
	void TestForceRegistrationL();
	void TestCorruptedRegFileL();	
	void TestMultipleRegistrationFilesForSameAppL();	
private:
	void PrepareRegFileL(const TUid& aUid, const TDesC& aRscFile, const TDesC& aLogicalExecutable);
	void CleanupRegFilesL();
	void FindRemovableDriveAndSetPathL();
private:
	RTestableApaLsSession iSession;
	REComSession iEComSession;
	CTestSidChecker* iTestSidChecker;
	RSmlTestUtils iUtils;
	TBuf<128> iCallBackAppRsc;
	TBuf<64> iCallBackAppIst;
	TBuf<128> iBadAppRsc;
	TBuf<64> iBadAppIst;
	TBuf<128> iGoodAppRsc;
	TBuf<64> iGoodAppIst;
	TBuf<128> iForcedAppRsc;
	TBuf<64> iForcedAppIst;
	TBuf<128> iTriggerRescanRsc;
	TBuf<64> iTriggerRescanIst;
	TBuf<64> iMmcPrivateDir;
	TBuf<128> iAppRsc1;
	TBuf<128> iAppRsc2;
	TBuf<128> iAppRsc3;
	TBuf<128> iAppIst;
	TDriveUnit iDrive;
	};

_LIT(KT_File3Step,"T_File3");
#endif
