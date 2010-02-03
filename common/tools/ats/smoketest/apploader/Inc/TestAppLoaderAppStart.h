// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestAppLoaderAppStart.h
// This contains CTestAppLoaderAppStart
// 
//

#ifndef TEST_APP_LOADER_APP_START_H
#define TEST_APP_LOADER_APP_START_H

// User includes
#include "TestAppLoaderBase.h"

class CTestAppLoaderAppStart : public CTestAppLoaderBase
	{
public:
	CTestAppLoaderAppStart();

	virtual enum TVerdict doTestStepL( void );
	};

#endif /* TEST_APP_LOADER_APP_START_H */
