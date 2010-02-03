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
 @test
 @internalComponent - Internal Symbian test code
*/

#if (!defined __T_SYSSTARTAPPARCTESTCASES_H__)
#define __T_SYSSTARTAPPARCTESTCASES_H__

#include <apgcli.h>

/**
  Literals which determine:
  1. CApparcFuncInSystart - message logged if a panic occurs in a test case
  2. z:\\apparctest\\TestCaseConfig.txt - Input configuration file which determines
     which test case will be run.

*/
_LIT(KApparcFuncInSystartPanic, "CApparcFuncInSystart");
_LIT(KApparcConfigFile, "z:\\apparctest\\TestCaseConfig.txt");

enum TSystartPanic
	{
	ESystartRApaLsSessionErr=10,
	ESystartRunTestCaseErr,
	ESystartWriteResultsErr,
	ESystartRFsErr,
	ESystartFindFileErr
	};


/**
  Class containing the apparc test cases
*/
class CSysStartApparcTestCase : public CBase
	{
public:
	CSysStartApparcTestCase();
	~CSysStartApparcTestCase();
	void doTestCasesL();

private:
	void RunTestCasesL();
	void TestStartApp1L();
	void TestStartApp2L();
	void TestStartApp3L();
	void TestStartApp4L();
	void TestStartApp5L();
	void TestStartApp6L();
	void TestGetAllApps();
	void TestInsertDataTypeL();
	void TestAppForDataTypeL();
	void TestDeleteDataTypeL();
	void TestServiceDiscovery();
	void TestGetAppInfo();
	void TestAppCount();
	void TestCreateDoc();

	void WriteResultFileL();
	TInt ReadConfigFileL();

private:
	TInt iStartApp1LRes1;
	TInt iStartApp1LRes2;

	RApaLsSession iApaLsSession;
	};

#endif
