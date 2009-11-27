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

#if (!defined __T_PROC_STEP_H__)
#define __T_PROC_STEP_H__

#include <apacmdln.h>
#include <apgcli.h>
#include "ApparcTestServer.h"
#include <apparc.h>

_LIT(KT_ProcStep,"T_Proc");
_LIT(KParentExe, "ParentProcess");
_LIT(KChildOneExe, "ChildI");
_LIT(KChildTwoExe, "ChildII");
_LIT(KChildThreeExe, "ChildIII");
_LIT(KT_ParentProcessID,"ParentProcessID");
_LIT(KFilePath,"c:\\logs\\TestExecute\\Proc.txt");
_LIT8(KTResultPass, "PASS");
_LIT8(KTResultFail, "FAIL");


//!  A CT_ProcStep test class.

/**  Checks for child process existence when its parent terminates. */

class CT_ProcStep : public CTestStep
	{
public:
	CT_ProcStep();
	~CT_ProcStep();
	virtual TVerdict doTestStepL();
private:
	void testChildExistsL(void);
	void testChildSetToTerminateL(void);
	void testChildSetToRemainL(void);
	void testTwoChildsOneToTerminateAndOtherToRemainL(void);
	void testParentWithoutAChildL(void);
	void testAllChildsSetToTerminateL(void);
	void testNoChildSetToTerminateL(void);
	void testIdAvailableToChildL(void);
	void testIdNotAvailableToChildL(void);
	};


//!  A support class for CT_ProcStep test class.

/**  An active object which monitors the current status of child when its parent terminates. */

class CChildProcess : public CActive
	{
public:
	static CChildProcess* NewL(RProcess& aParentProcess,TProcessId aChildProcessId);
	void RunL();
	void DoCancel();
	~CChildProcess();
private:
	void ConstructL();
	CChildProcess(RProcess& aParentProcess,TProcessId aChildProcessId);
private:
	TProcessId iChildProcessId;
	RProcess iParentProcess;
	RProcess iChildProcess;
	TExitType iExitType;
	TInt iCount;
	};

#endif
