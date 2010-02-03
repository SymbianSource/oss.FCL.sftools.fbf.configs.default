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
 @test
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __T_NONNATIVEAPPSTEP_H__)
#define __T_NONNATIVEAPPSTEP_H__

#include <APASERVERAPP.H>
#include <EIKSERVERAPP.H>

#include "ApparcTestServer.h"

// RTstFileArray

class RTstFileArray
	{
public:
	RTstFileArray(TInt aGranularity);
	void Close();
	void AppendL(const TDesC& aFullFileName, const TDesC8& aExpectedDataType);
	inline TInt Count() const {return iFileArray.Count();}
	void Get(TPtrC& aFullFileName, TPtrC8& aExpectedDataType, TInt aIndex) const;
private:
	struct SFile
		{
		HBufC* iFullFileName;
		HBufC8* iExpectedDataType;
		};
private:
	RArray<SFile> iFileArray;
	};


// RTstAppService

class RTstAppService : public RApaAppServiceBase
	{
public:
	static TInt DoTestL(TUid aServiceUid, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath);
private:
	RTstAppService(TUid aServiceUid);
	void ConnectL();
	void ReceiveTestResult(TRequestStatus& aRequestStatus, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath);
	// from RApaAppServiceBase
	virtual TUid ServiceUid() const;
private:
	const TUid iServiceUid;
	};



// CT_NonNativeApps test class. 

class CT_NonNativeAppsStep : public CTestStep
	{
public:
	CT_NonNativeAppsStep();
	~CT_NonNativeAppsStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	HBufC8* OpenFileAndReadBufferLC(RApaLsSession& aApparcServer, RFs& aFileServer, const TDesC& aFullFileName);
	void OpenFileAndSetFilePositionToSomewhereUnusualLC(RFile& aFile, TInt& aFilePosition, RFs& aFileServer, const TDesC& aFullFileName);
	void TestFilePosition(RFile& aFile, TInt aFilePosition);
	void TestResultsOfAppForDocumentAndServiceL(TUid aServiceUid, TInt aError, const TUid& aAppUid, const TDataType& aDataType, const TDesC8& aExpectedDataType);
	void CheckPropertyUpdateAndResetL(const TDesC& aExpectedNewValue);
	void CheckApplicationLaunchesOkayL(RApaLsSession& aApparcServer, const CApaCommandLine& aCommandLine, TAny* aOption);
	void CheckApplicationFailsToLaunchL(RApaLsSession& aApparcServer, const CApaCommandLine& aCommandLine, TAny* aOption);	
	void WaitForApplistUpdate(RApaLsSession& aApparcServer);
private:
	void TestAppForDocumentL(RApaLsSession& aApparcServer, RFs& aFileServer, const RTstFileArray& aFileArray);
	void TestAppForDocumentAndServiceL(RApaLsSession& aApparcServer, RFs& aFileServer, const RTstFileArray& aFileArray);
	void TestRecognizingL(RApaLsSession& aApparcServer, RFs& aFileServer, const RTstFileArray& aFileArray);
	void TestStartDocumentL(RApaLsSession& aApparcServer, RFs& aFileServer, const RTstFileArray& aFileArray);
	void TestRegisterNonNativeApplicationL(RApaLsSession& aApparcServer, RFs& aFileServer);
	void TestGetAppIconForNonNativeL(RApaLsSession& aApparcServer, RFs& aFileServer, CApaRegistrationResourceFileWriter* const aRegistrationResourceFileWriter, CApaLocalisableResourceFileWriter* const aLocalisableResourceFileWriter);	
	void TestCommitNNAppUpdatesL(RApaLsSession& aApparcServer);	
	void TestForceCommitNNAppUpdatesL(RApaLsSession& aApparcServer);
	void TestForceCommitL(RApaLsSession& aApparcServer, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriter, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriterB, CApaCommandLine& aNextCommandLine);
	void TestRollbackRegistrationL(RApaLsSession& aApparcServer, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriter, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriterB, CApaCommandLine& aNextCommandLine);
	void TestRollbackDeregistrationL(RApaLsSession& aApparcServer, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriter, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriterB, CApaCommandLine& aNextCommandLine);	

private:

	};


_LIT(KT_NonNativeAppsStep,"T_NonNativeApps");

#endif
