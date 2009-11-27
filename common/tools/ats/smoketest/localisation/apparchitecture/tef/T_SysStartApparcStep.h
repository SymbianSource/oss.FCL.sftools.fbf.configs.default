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

#if (!defined __T_SYSSTARTAPPARCSTEP_H__)
#define __T_SYSSTARTAPPARCSTEP_H__

#include "ApparcTestServer.h"
#include "tssaac\SysStartApparcTestCases.h"  // KApparcResultFile

#define KTestResultPass 1


/**
  Literal used as name for test case.
*/
_LIT(KT_SysStartApparcStep,"T_SysStartApparc");
_LIT(KApparcStartupT_TestStartApp1L,"T_TestStartApp1L");
_LIT(KApparcStartupT_TestStartApp2L,"T_TestStartApp2L");
_LIT(KApparcStartupT_TestStartApp3L,"T_TestStartApp3L");
_LIT(KApparcStartupT_TestStartApp4L,"T_TestStartApp4L");
_LIT(KApparcStartupT_TestStartApp5L,"T_TestStartApp5L");
_LIT(KApparcStartupT_TestStartApp6L,"T_TestStartApp6L");
_LIT(KApparcStartupT_TestGetAllApps,"T_TestGetAllApps");
_LIT(KApparcStartupT_TestInsertDataTypeL,"T_TestInsertDataTypeL");
_LIT(KApparcStartupT_TestAppForDataTypeL,"T_TestAppForDataTypeL");
_LIT(KApparcStartupT_TestDeleteDataTypeL,"T_TestDeleteDataTypeL");
_LIT(KApparcStartupT_TestServiceDiscovery,"T_TestServiceDiscovery");
_LIT(KApparcStartupT_TestGetAppInfo,"T_TestGetAppInfo");
_LIT(KApparcStartupT_TestAppCount,"T_TestAppCount");
_LIT(KApparcStartupT_TestCreateDoc,"T_TestCreateDoc");
_LIT(KApparcStartupT_TestLocalisedCaptionL,"T_TestLocalisedCaptionL");


/**
  CT_SysStartApparcStep encapsulates the functionality required
  for the T_SysStartApparcTest SysStart test case.
*/
class CT_SysStartApparcStep : public CTestStep
	{
public:
	CT_SysStartApparcStep();
	~CT_SysStartApparcStep();
	virtual TVerdict doTestStepL();
	void CheckApparcPanicResultsL();
	void CheckApparcResultsL();
	TInt ReadConfigFileL();
	TInt iTestcase;
	};

#endif
