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

#if (!defined __T_CMDLN_STEP_H__)
#define __T_CMDLN_STEP_H__

#include <e32std.h>
#include <e32base.h>
#include <e32uid.h>
#include <e32cmn.h>
#include <f32file.h>
#include <e32test.h>
#include <apacmdln.h>
#include <apgcli.h>
#include "ApparcTestServer.h"
#include <apparc.h>

//!  A CT_CmdlnStep test class. 
/**
  Test CApaCommandLine Apis.
*/
class CT_CmdlnStep : public CTestStep
	{
public:
	CT_CmdlnStep();
	~CT_CmdlnStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

	void testSecureCmdLinesL();
	void testBoundaryConditionsL();
	void testInitializationL();
	void testPositiveInputsL();
	void testRecAppLaunchL(RApaLsSession& aLs);
	void testCmdLinesAPIsL();
	
	};

_LIT(KT_CmdlnStep,"T_Cmdln");


#define TEST_INITIALIZATION   0
#define TEST_APP              1  
#define TEST_DOC              2
#define TEST_TAILEND          3
#define TEST_COMMAND		  4
#define TEST_FILE_HANDLE	  5

#define TEST_SET_APIS         7

_LIT(KTLogFileAccess,"TestLogFile");
_LIT(KEnvFilePath,"c:\\Logs\\TestExecute\\EnvSlots.txt");

_LIT(KTAppName,"Z:\\sys\\bin\\T_EnvSlots.exe");
_LIT(KTDocName,"C:\\System\\data\\temp.test");
_LIT(KTempDir,"C:\\System\\data\\");
_LIT(KTNoDocName,"C:\\Logs\\TestExecute\\NotFound.aaa");

_LIT8(KTTailEnd,"TailEnd");
_LIT8(KTNullTail,"");
_LIT8(KTDataType,"test/plain");
_LIT8(KTWriteData,"Support");
_LIT8(KTPass,"TestPass");
_LIT8(KTFail,"TestFail");
_LIT8(KTApp,"ApplName");
_LIT8(KTDoc,"DocuName");
_LIT8(KTServer,"ServerId");
_LIT8(KTCommand,"ECommand");

const TInt KEnvThirdUidValue = 0x102032AB; 
const TInt KOneSecond = 1000000;
const TInt KMaxFilePath = 50;
const TInt KFileHandleTestPassed = 99;

#endif //__T_CMDLN_STEP_H__
