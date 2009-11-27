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
// Header File for the test application for environment slots verification
// Declares literals and macros used in T_EnvSlots.CPP .\n
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#ifndef __T_ENVSLOTS_H__
#define __T_ENVSLOTS_H__

#include <e32std.h>
#include <e32base.h>
#include <e32uid.h>
#include <e32cmn.h>
#include <f32file.h>
#include <e32test.h>
#include <apacmdln.h>
#include <apgcli.h>
#include "ApparcTestServer.h"
#include "appfwk_test_AppUi.h"


//!  A Environment slots reader test class. 

/**
  Test Environment slots of the process.
*/
	
_LIT(KTLogFileAccess,"TestLogFile");
_LIT(KFilePath,"c:\\logs\\TestExecute\\EnvSlots.txt");
_LIT(KTAppName,"Z:\\sys\\bin\\T_EnvSlots.exe");
_LIT(KTDocName,"C:\\System\\data\\temp.test");
_LIT(KTEnvSlots,"T_EnvSlots");

_LIT8(KTPass,"TestPass");
_LIT8(KTFail,"TestFail");
_LIT8(KTApp,"ApplName");
_LIT8(KTDoc,"DocuName");
_LIT8(KTCommand,"ECommand");

const TInt PASS = 1;
const TInt FAIL = 0;

#define TESTCOND(a,b) if(a != b)  \
{\
	testResult = FAIL;\
}\

const TInt KMaxFilePath=50;
#endif //__T_ENVSLOTS_H__
