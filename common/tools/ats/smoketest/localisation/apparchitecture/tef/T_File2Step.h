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
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __T_FILE2_STEP_H__)
#define __T_FILE2_STEP_H__
#include "ApparcTestServer.h"

//!  A CT_File2Step test class. 

/**   A more elaborate class description. */

class CT_File2Step : public CTestStep
	{
public:
	CT_File2Step();
	~CT_File2Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void testFindAllAppsRegL();
	};

_LIT(KT_File2Step,"T_File2");
#endif
