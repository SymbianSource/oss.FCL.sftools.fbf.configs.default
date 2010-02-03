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


#ifndef TEST_SYSTEM_BASE_H
#define TEST_SYSTEM_BASE_H

#include <eikunder.h>
#include <TestExecuteLog.h>

class CEikUndertaker;

class CSystemTestBase : public MEikUndertakerObserver
	{
public:
	static CSystemTestBase* NewL();
	~CSystemTestBase();

protected:	
	CSystemTestBase();
	void ConstructL();

protected: // From MEikUndertakerObserver
	void HandleThreadExitL(RThread& aThread);
	
protected:
	CEikUndertaker* iUndertaker;
	RTestExecuteLogServ iLogger;
	TInt iExitReason;	
	TExitCategoryName iExitCategory;
	};


#endif // TEST_SYSTEM_BASE_H
