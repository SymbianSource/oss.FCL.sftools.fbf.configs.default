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
// TestAppLoaderKillProcess.h
// This contains CTestAppLoaderKillProcess
// 
//

#ifndef TEST_APP_LOADER_KILL_PROCESS_H
#define TEST_APP_LOADER_KILL_PROCESS_H

// User includes
#include "TestAppLoaderBase.h"

class CTestAppLoaderKillProcess : public CTestAppLoaderBase
	{
public:
	CTestAppLoaderKillProcess();

	virtual enum TVerdict doTestStepL( void );
	};

#endif /* TEST_APP_LOADER_KILL_PROCESS_H */
