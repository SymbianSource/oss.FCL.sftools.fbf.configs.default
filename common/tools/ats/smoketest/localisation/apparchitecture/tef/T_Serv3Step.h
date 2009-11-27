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

#if (!defined __T_SERV3_STEP_H__)
#define __T_SERV3_STEP_H__

#include "ApparcTestServer.h"
#include "appfwk_test_utils.h"

//!  A CT_Serv3Step test class.
/*!
  This class tests wrapper to Application Architecture Server.
*/
class CT_Serv3Step :public CTestStep
	{
public:
	CT_Serv3Step();
	~CT_Serv3Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	void DoSyncFolderRecognizerTest1L(RApaLsSession& aLs);
	void DoSyncFolderRecognizerTest2L(RApaLsSession& aLs);
	void DoSyncFolderRecognizerTest3L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest1L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest2L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest3L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest4L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest5L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest6L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest7L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest8L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest9L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest10L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest11L(RApaLsSession& aLs);
	void DoSyncFolderRecognizerTest4L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest13L(RApaLsSession& aLs);
	void DoAsyncFolderRecognizerTest14L(RApaLsSession& aLs);
	TInt DoServComTestL();
	void testBooleanTruer(TBool aCondition, const TText8* aFile, TInt aLine);
	
private:
	void CreateDirL(RSmlTestUtils* aPtrUtils, TPtr aDirName);
	void DeleteDirL(RSmlTestUtils* aPtrUtils, TPtr aDirName);
	void DeleteFilesL(RSmlTestUtils* aPtrUtils, TPtr aDirName, TDesC& aSuffix, TInt aFileNumber);
	void CreateFilesL(RFs& aFS, RFile& aFile, TPtr aDirName, TDesC& aSuffix, TInt aFileNumber, const TDesC8& aFileContent, TTime aTime);
	CActiveScheduler* iActiveScheduler;
	};

//////////////////////////////////////////////////////////////////////////////
//	CActiveTest2::ActiveTest class definition
//
class CActiveTest2 : public CActive
	{
	public:
		CActiveTest2();
		~CActiveTest2();
		void StartL();

	protected:
		void DoCancel();
		void RunL();
	};

_LIT(KT_Serv3Step,"T_Serv3");

#endif
