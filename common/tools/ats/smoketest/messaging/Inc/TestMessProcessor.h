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
// TestMessProcessor.cpp
// This contains CTestMessProcessor which an active object utility
// used for send/recieving or any time consuming 
// 
//

#if (!defined __TEST_MESS_PROCESSOR_H__)
#define __TEST_MESS_PROCESSOR_H__

//Epoc Include
#include <e32base.h>
#include <msvapi.h>
#include <miuthdr.h>

class CTestStep;

class MTestMessProcessorClient
	{
public:
	virtual TBool			MoreOperations() const = 0;
	virtual CMsvOperation*	CreateOperationL(TRequestStatus& aStatus) = 0;
	virtual TInt			DisplayProgressL(TBool aFinalProgress, CMsvOperation* aMsvOperation) = 0;
	};

/**
 This class is an active object utility used for send/recieving
 or any time consuming
*/
class CTestMessProcessor : public CActive
	{
public:
	virtual ~CTestMessProcessor();
	static CTestMessProcessor*	NewL(CTestStep& aTestStep, CMsvSession& aSession);

	void	RunL();

	void	DisplayProgress();
	void	SendEmailMessageL(TMsvId aMsgId, MTestMessProcessorClient* aClient);

	//Sends the SMS message
	void	SendSmsMessageL(TMsvId aMsgId, MTestMessProcessorClient* aClient, TBool aCancelSms);
	void	SendMmsMessageL(TMsvId aMsgId, MTestMessProcessorClient* aClient);
	void	MtmRequestL(TMsvId aAccountId, MTestMessProcessorClient* aClient);

protected:
	CTestMessProcessor(CTestStep& aTestStep, CMsvSession& aSession /*, MTestInstrumentation& aInstrumentation */);
	void	ConstructL();
	void	DoCancel();

private:
	enum EState
		{
		EStatePrepareToSendMsg,
		EStateRequest,
		};

	EState						iState;
	CTestStep&					iTestStep;
	CMsvSession&				iSession;

	//	Send message parameters saved for RunL
	CMsvEntry*					iEntry;
	TMsvId						iMsgId;
	MTestMessProcessorClient*	iClient;
	TMsvEntry					iMsgEntry;
	CMsvOperation*				iMsvOperation;
	/**
	 Boolean value for the Cancel sms flag
	*/
	TBool						iCancelSms;
};

#endif /* __TEST_MESS_PROCESSOR_H__ */
