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


#if (!defined __T_groupNametest_ver1_H__)
#define __T_groupNametest_ver1_H__

#include "ApparcTestServer.h"

//  A CT_GroupNameStep_ver1 test class. 

class CT_GroupNameStep_ver1 : public CTestStep
	{
public:
	CT_GroupNameStep_ver1();
	~CT_GroupNameStep_ver1();
	virtual TVerdict doTestStepL();
	void DoTestGroupNameL(RApaLsSession& aLs);

private:
	};

_LIT(KT_GroupNameStep_ver1, "T_GroupName_ver1");


#endif
