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
// TestAppLoaderKeyEvents.h
// This contains CTestAppLoaderKeyEvents
// 
//

#ifndef TEST_APP_LOADER_KEY_EVENTS_H
#define TEST_APP_LOADER_KEY_EVENTS_H

// User includes
#include "TestAppLoaderBase.h"

class CTestAppLoaderKeyEvents : public CTestAppLoaderBase
	{
public:
	CTestAppLoaderKeyEvents();
	
	TBool GetKeyCodeFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TKeyCode& aResult, TStdScanCode& aScanCode);

	virtual enum TVerdict doTestStepL( void );
	};

#endif /* TEST_APP_LOADER_KEY_EVENTS_H */
