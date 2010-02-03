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
// This contains CTestAppLoaderBase which is the base class for all the AppLoader TestCase DLL
// 
//

// USER includes
#include "TestAppLoaderBase.h"

CTestAppLoaderBase::CTestAppLoaderBase()
	{
	}

// destructor
CTestAppLoaderBase::~CTestAppLoaderBase()
	{
	}

enum TVerdict CTestAppLoaderBase::doTestStepPreambleL()
	{
	TVerdict	ret = CTestStep::doTestStepPreambleL();
	return ret;
	}

enum TVerdict CTestAppLoaderBase::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}
