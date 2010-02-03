// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestMessSetDefaultSc.cpp
// This contains TestMessSetDefaultSc which sets an SMS
// SC as default SC.
// 
//

#if (!defined __TEST_MESS_SET_DEFAULT_SC_H__)
#define __TEST_MESS_SET_DEFAULT_SC_H__

#include "TestMessBase.h"

class CTestMessSetDefaultSc : public CTestMessBase
{
public:
	CTestMessSetDefaultSc();

	virtual	TVerdict	doTestStepL();
};

#endif /* __TEST_MESS_SET_DEFAULT_SC_H__ */
