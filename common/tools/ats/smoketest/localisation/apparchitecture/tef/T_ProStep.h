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

#if (!defined __T_PRO_STEP_H__)
#define __T_PRO_STEP_H__

_LIT(KT_ProStep,"T_Pro");

#include "ApparcTestServer.h"

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
#include <apgcli.h>
#include <apacmdln.h>

//!  A CT_ProStep test class. 

/**  Tests Application Apparc base classes and utility functions.\n */

class CT_ProStep : public CTestStep
	{
public:
	CT_ProStep();
	~CT_ProStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void DoStepTests();
//private:
	void DoStepTestsInCallbackL();
	void testAppIdentifierL();
	void doTestGenerateFileName();
	void testDocCreationL();
	void testDocCreation1L();
	CApaDocument* testDocCreation3L();
	void testDocCreation2L();
	void testDocCreation4L(CApaDocument* aDoc);
	void testEmbeddingL();
	void testIniFilesL();
	void testControlsL();
	void setup();
	TInt ControlIndexInList(CApaSystemControlList* aList, TBool aIsNewPath);
	void testControls1L();
	void testControls2L();
	void testControls3L();
	void DoAppListInvalidTestL(RApaLsSession& aLs);
	void MainDocFileNameTestsL();
	void testMainDocFileName1L();
	void testMainDocFileName2L();
	void testMainDocFileName3L();
	void testReadStreamL();
    void testPanicForControlListL();
    void ClosePanicWindowL();
        

private:
	RFs iFs;
	CApaProcess* iProcess;
	CApaSystemControlList* iControlList;
	CApaSystemControl* iControl;

	TInt iControlCount1;
	TInt iControlCount2;
	TInt iControlCount3;

	TInt iAppListCount;
	};


class CT_ProStepCallBack : public CBase
	{
public:
	CT_ProStepCallBack(CT_ProStep* aTestStep);
	~CT_ProStepCallBack();
	static TInt CallBack(TAny* /*aThis*/);
	CT_ProStep* iTestStep;

private:
	};

#endif
