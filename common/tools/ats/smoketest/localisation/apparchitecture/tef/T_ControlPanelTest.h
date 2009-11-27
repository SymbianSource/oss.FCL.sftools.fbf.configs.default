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
 
//#if (!defined __T_CONTROLPANEL_STEP_H)
//#define __T_CONTROLPANEL_STEP_H

#ifndef		__T_CONTROLPANEL_STEP_H
#define		__T_CONTROLPANEL_STEP_H

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
#include <apsserv.h>
#include "appfwk_test_utils.h"
#include "..\apserv\apsclsv.h" // so I can start the server


_LIT(KT_ControlPanelTest, "T_ControlPanelTest");

//!  A CT_ControlPanelTestStep test class. 
/*!
Tests Application Apparc base classes and utility functions.\n
*/

class CT_ControlPanelTestStep : public CTestStep
	{
public:
	CT_ControlPanelTestStep();
	~CT_ControlPanelTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void DoStepTests();
	void DoStepTestsInCallbackL();
	void testControlsL();
	void loadDllL();
	void RemoveFilesFromCDrive();
	TInt ControlIndexInList(CApaSystemControlList* aList, TBool aIsNewPath);
	void testControls1L();
	void testControls2L();
	void testControls3L();
private:
	RFs iFs;
	CApaSystemControlList* iControlList;
	CApaSystemControl* iControl;
	TInt iControlCount;
	TInt iControlCount1;
	TInt iControlCount2;
	TInt iControlCount3;
	TInt iIndex;
	RSmlTestUtils iTestServ;
	};


class CT_ControlPanelTestCallBack : public CBase
	{
public:
	CT_ControlPanelTestCallBack(CT_ControlPanelTestStep* aTestStep);
	~CT_ControlPanelTestCallBack();
	static TInt CallBack(TAny* /*aThis*/);
	CT_ControlPanelTestStep* iTestStep;

private:
	};

#endif
