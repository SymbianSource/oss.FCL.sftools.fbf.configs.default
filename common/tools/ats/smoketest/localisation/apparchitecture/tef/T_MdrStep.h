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

#if (!defined __T_MDR_STEP_H__)
#define __T_MDR_STEP_H__

#include "ApparcTestServer.h"

#include <apamdr.h>
#include <apgdoor.h>

class TTestModelHeaderFactory : public MApaModelHeaderFactory
	{
public:
	CApaModelHeader* NewHeaderL(const CStreamStore& aStore,const CStreamDictionary& aDict,const TApaAppIdentifier& aAppId)const;
	};

class CTestModelHeader : public CApaModelHeader
	{
public:
	void RestoreL(const CStreamStore& aStore,const CStreamDictionary& aDict);
	void StoreL(CStreamStore& aStore,CStreamDictionary& aDict) const;
	void DetachFromStoreL(CPicture::TDetach aDetach);
	TApaAppIdentifier AppId()const;
public:
	TInt iData;
	};
	
//!  A CT-MdrStep test class. 

/**  Class tests CApaDoor and CApaModelDoor Apis. */

class CT_MdrStep:public CTestStep
	{
public:
	CT_MdrStep();
	~CT_MdrStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	void testModelDoorL();
	void testConversionL();
	void testMiscellaneousL();
	void DoTestsInScheldulerLoopL();
	void DoStepTests();

private:
	RFs iFs;
	CApaProcess* iProcess;
	};

class CT_MdrTestCallBackWrapper : public CBase
	{
public:
	CT_MdrTestCallBackWrapper(CT_MdrStep* aTestStep);
	~CT_MdrTestCallBackWrapper();
	static TInt CallBack(TAny* aPtr);
private:
	CT_MdrStep* iTestStep;
	};

_LIT(KT_MdrStep,"T_Mdr");
#endif
