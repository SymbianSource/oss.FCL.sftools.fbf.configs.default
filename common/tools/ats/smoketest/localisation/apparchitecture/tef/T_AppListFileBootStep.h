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
 @test
 @internalComponent - Internal Symbian test code  
*/

#ifndef __T_APPLISTFILEBOOTSTEP_H
#define __T_APPLISTFILEBOOTSTEP_H

#include "ApparcTestServer.h"
#include <appfwk_test_utils.h>

_LIT(KT_AppListFile_Boot1,"T_AppListFile_Boot1");
_LIT(KT_AppListFile_Boot2,"T_AppListFile_Boot2");
_LIT(KT_AppListFile_Boot3,"T_AppListFile_Boot3");

class CT_AppListFileBootStep : public CTestStep
	{
public:
	CT_AppListFileBootStep(TInt);
	~CT_AppListFileBootStep();
	virtual TVerdict doTestStepL();

private:
	void DoTestL();
	TBool CheckForFilePresent();
	void DeleteAppListFileL(const TDesC&);
	void SetShortCaptionSettings();
	void TestShortCaptionSettings();

private:
	TInt iBootType;
	RApaLsSession iSession;
	RSmlTestUtils iUtils;
	};

#endif
