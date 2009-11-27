// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The following test case is used to test whether untrusted applications 
// are able to override MIME types mappings using InsertDataMappingIfHigher API.
// It is to ensure that priority checking mechanism (downgrading priority of 
// untrusted apps) works as was designed.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#if !defined(__T_DATATYPEMAPPINGWITHSID1_H__)
#define __T_DATATYPEMAPPINGWITHSID1_H__

#include "ApparcTestServer.h"
#include "appfwk_test_AppUi.h"

class RApaLsSession;

//! CT_DataTypeMappingWithSid1
/** 
  This class is used to test priority checks in InsertDataMappingIfHigher API
*/
class CT_DataTypeMappingWithSid1 : public CTestStep
	{
public:
	CT_DataTypeMappingWithSid1();
	virtual TVerdict doTestStepL();
	void ExecuteL(RApaLsSession& aLs);
private:
	~CT_DataTypeMappingWithSid1();
private:
	};

_LIT(KT_DataTypeMappingWithSid1, "T_DataTypeMappingWithSid1");

#endif // __T_DATATYPEMAPPINGWITHSID1_H__
