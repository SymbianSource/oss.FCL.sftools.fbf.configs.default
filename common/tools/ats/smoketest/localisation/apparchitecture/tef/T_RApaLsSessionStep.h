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

#if (!defined __T_RAPALSSESSION_STEP_H__)
#define __T_RAPALSSESSION_STEP_H__

_LIT(KT_RApaLsSessionStep,"T_RApaLsSession");

#include "ApparcTestServer.h"

#include <coecntrl.h>
#include <coeccntx.h>
#include <coemain.h>
#include <coeview.h>
#include <eikdialg.h>
#include <eikdoc.h>
#include <eikapp.h>
#include <eiksrvc.h>
#include <eikconso.h>
#include <eikenv.h>

#include <apaid.h>
#include "..\apparc\apadll.h"
#include <apgaplst.h>
#include <apgicnfl.h>
#include <apgdoor.h>
#include <apfrec.h>
#include <apfctlf.h>
#include <apgctl.h>
#include <apgaplst.h>
#include <apaflrec.h>
#include "testableapalssession.h"
#include <apacmdln.h>
#include <apsserv.h>
#include <datastor.h>
#include "appfwk_test_AppUi.h"
#include "TAppEmbedUids.h"


class CT_RApaLsSessionTestStep : public CTestStep
	{
public:
	CT_RApaLsSessionTestStep();
	~CT_RApaLsSessionTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void RunTestCases();

private:
	void ConstructL();
	void setup();
	void RunTestCasesL();
	void TestAppListInvalidSetupL();
	TBool ComparePaths(const TDesC& aFilename, const TDesC& aPath);
	
	void TestAppListInvalidL();
	
	void IconLoadingTestCasesL();
	void TestServerIconLoadingBySizeSetupL();
	void TestServerIconSizeCountL(CArrayFixFlat<TSize>& aTestIconSizeArray);
	void TestServerIconLoadingBySize1L(CArrayFixFlat<TSize>& aTestIconSizeArray);
	void TestServerIconLoadingBySize2L(CArrayFixFlat<TSize>& aTestIconSizeArray);
	void TestServerIconLoadingBySize3L(CArrayFixFlat<TSize>& aTestIconSizeArray);
	
	void AppInfoTestCasesL();
	void TestGetAppCapabilityL();
	void TestRecognizeSpecificData();
	void TestSetGetAcceptedConfidenceL();
	void TestSetGetMaxDataBufSizeL();
	void TestGetSupportedDataTypesL();
	void TestGetAppIconL();
	void TestGetAppIcon1L();
	void TestGetAppIcon2L();
	void TestGetAppIconForMBMIconsL();
	void TestGetAppIconForSVGIconsL();

	void TestGetAppViewsL();
	void TestGetAppViewsFailureL();
	
	void TestAppListInfoDataInterrogationSetupL();
	void TestAppInfo1();
	void TestAppInfo2();
	void TestAppInfo3();
	void TestAppInfo4();

	void TestAppListRecognizeDataL();
	void TestAppListRecognizeDataBufferOnlyL();
	void TestAppListInstallationL();
	void TestAppListInstallation1L();
	void TestAppFolderNonRomDrivesL();

	void EmbeddedAppsTestCases();
	void DoGetAllApps();
	void DoGetEmbeddableApps();
	void DoEmbeddableAppCount();
	void DoGetAppCapability();
	void DoGetFilteredApps1();
	void DoGetFilteredApps2();
	void DoGetFilteredApps3();
	void DoGetFilteredApps4();
	void DoGetFilteredApps5();
	void DoGetFilteredApps6();
	void DoTestEmbeddabilityFilter1();
	void DoTestEmbeddabilityFilter2();
	void DoTestEmbeddabilityFilter3();
	void DoTestEmbeddabilityFilter4();
	void DoTestEmbeddabilityFilter5();
	void DoNumDefIconsTestL();
	void TestMatchesSecurityPolicy();
	void TestNotifyOnDataMappingChangeL();
	void TestDataPriorityForUnTrustedApps();
	void TestDataPriorityForUnTrustedAppsRegFile();
	void TestIconLoaderAndIconArrayMemoryLeaksL();
private:
	RFs iFs;
	RTestableApaLsSession iLs;
	};

#endif
