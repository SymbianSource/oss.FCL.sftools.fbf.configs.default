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

#if (!defined __T_SERV2_STEP_H__)
#define __T_SERV2_STEP_H__

#include "ApparcTestServer.h"

//!  A CT_Serv2Step test class. 

/**  This class tests wrapper to Application Architecture Server. */

class CT_Serv2Step :public CTestStep
	{
public:
	CT_Serv2Step();
	~CT_Serv2Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
//private:
	void DoInterrogationTestsL(RApaLsSession& aLs);
	void DoEnquiryTestsL(RApaLsSession& aLs);
	void DoStartAppTestsL(RApaLsSession& aLs);
	void StartAppTests1L(RApaLsSession& aLs);
    void StartAppTests4L(RApaLsSession& aLs);
	void DoAppInfoTests(RApaLsSession& aLs);
	void AppInfoTest1(RApaLsSession& aLs);
	void AppInfoTest3(RApaLsSession& aLs);
	void AppInfoTest4(RApaLsSession& aLs);
	void AppInfoTest5(RApaLsSession& aLs);
	void DoInstallationTestL (RApaLsSession ls);
	void DoNumDefIconsTestL(RApaLsSession& aSession);
	void DoCheckServerIconLoadingBySizeL(RApaLsSession& aLs);
	void LaunchAppThruCreateDocL(RApaLsSession& aLs);
	TFileName SearchAndReturnCompleteFileName(const TDesC& aFileName);
	TBool ComparePaths(const TDesC& aFilename, const TDesC& aPath);
	TInt DoServComTestL();

	void DoGetAllApps(RApaLsSession& aLs);
	void DoGetEmbeddableApps(RApaLsSession& aLs);
	void DoEmbeddableAppCount(RApaLsSession& aLs);
	void DoGetAppCapability(RApaLsSession& aLs);
	void DoGetFilteredApps1(RApaLsSession& aLs);
	void DoGetFilteredApps2(RApaLsSession& aLs);
	void DoGetFilteredApps3(RApaLsSession& aLs);
	void DoGetFilteredApps4(RApaLsSession& aLs);
	void DoGetFilteredApps5(RApaLsSession& aLs);
	void DoGetFilteredApps6(RApaLsSession& aLs);
	void DoTestEmbeddabilityFilter1();
	void DoTestEmbeddabilityFilter2();
	void DoTestEmbeddabilityFilter3();
	void DoTestEmbeddabilityFilter4();
	void DoTestEmbeddabilityFilter5();
	void DoRecognizeDataL(RApaLsSession& aLs);
	void TestRecognitionCacheL(RApaLsSession& aLs);
	};

_LIT(KT_Serv2Step,"T_Serv2");
#endif
