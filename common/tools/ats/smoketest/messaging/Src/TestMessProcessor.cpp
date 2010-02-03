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
// This contains CTestMessProcessor which an active object utility
// used for send/recieving or any time consuming 
// 
//

#include "TestMessProcessor.h"
#include "TestMessBase.h"

CTestMessProcessor* CTestMessProcessor::NewL(CTestStep& aTestStep, CMsvSession& aSession)
	{
	CTestMessProcessor*	self=new(ELeave) CTestMessProcessor(aTestStep, aSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTestMessProcessor::CTestMessProcessor(CTestStep& aTestStep, CMsvSession& aSession)
:	CActive(EPriorityStandard)
,	iState(EStatePrepareToSendMsg)
,	iTestStep(aTestStep)
,	iSession(aSession)
,	iEntry(NULL)
,	iMsgId(KMsvRootIndexEntryId)
,	iClient(NULL)
,	iMsvOperation(NULL)
,	iCancelSms(EFalse)
	{
	}

CTestMessProcessor::~CTestMessProcessor()
	{
	delete iEntry;
	iEntry=NULL;
	delete iMsvOperation;
	iMsvOperation=NULL;
	}

void CTestMessProcessor::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

void CTestMessProcessor::DoCancel()
	{
	iTestStep.INFO_PRINTF1(_L("Cancel operation"));
	iMsvOperation->Cancel();
	}

void CTestMessProcessor::DisplayProgress()
	{
	// display the current progress
	switch(iState)
		{
	case EStatePrepareToSendMsg:
		{
		TMsvLocalOperationProgress			temp;
		TPckgC<TMsvLocalOperationProgress>	progress(temp);

		progress.Set(iMsvOperation->ProgressL());
		iTestStep.INFO_PRINTF1(_L("   Progress"));
		iTestStep.INFO_PRINTF2(_L("       Operation              :%d"), progress().iType);
		iTestStep.INFO_PRINTF2(_L("       Total Number Of Entries:%d"), progress().iTotalNumberOfEntries);
		iTestStep.INFO_PRINTF2(_L("       Number Completed       :%d"), progress().iNumberCompleted);
		iTestStep.INFO_PRINTF2(_L("       Number Failed          :%d"), progress().iNumberFailed);
		iTestStep.INFO_PRINTF2(_L("       Number Remaining       :%d"), progress().iNumberRemaining);
		iTestStep.INFO_PRINTF2(_L("       Error                  :%d"), progress().iError);
		iTestStep.INFO_PRINTF2(_L("       Id                     :%x"), progress().iId);
		}
		break;
	case EStateRequest:
		iClient->DisplayProgressL(EFalse, iMsvOperation);
		break;
	default:
		break;
		}
	}

void CTestMessProcessor::SendEmailMessageL(TMsvId aMsgId, MTestMessProcessorClient* aClient)
	{
	iMsgId=aMsgId;
	iClient=aClient;
	iState=EStatePrepareToSendMsg;

	delete iEntry;
	iEntry=NULL;
	iEntry=iSession.GetEntryL(iMsgId);
	iMsgEntry=iEntry->Entry();
	iMsgEntry.SetSendingState(KMsvSendStateWaiting);
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iEntry->ChangeL(iMsgEntry, iStatus);
	SetActive();
	}

/**
 Sends the sms message. It sets the fields of the enty.
 @param aMsgId - TMsvId of the message
 @param aClient - Pointer to the MTestMessProcessClient
 @param aCancelSms - Bool value for the Cancel sms flag
*/
void CTestMessProcessor::SendSmsMessageL(TMsvId aMsgId, MTestMessProcessorClient* aClient, TBool aCancelSms)
	{
	iMsgId=aMsgId;
	iClient=aClient;
	iState=EStatePrepareToSendMsg;
	iCancelSms = aCancelSms;

	delete iEntry;
	iEntry=NULL;
	iEntry=iSession.GetEntryL(iMsgId);
	iMsgEntry=iEntry->Entry();
	iMsgEntry.SetConnected(ETrue);
	iMsgEntry.SetFailed(EFalse);
	iMsgEntry.SetSendingState(KMsvSendStateWaiting);
	iMsgEntry.iError = KErrNone;
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iEntry->ChangeL(iMsgEntry, iStatus);
	SetActive();
	}

void CTestMessProcessor::SendMmsMessageL(TMsvId aMsgId, MTestMessProcessorClient* aClient)
	{
	iMsgId=aMsgId;
	iClient=aClient;
	iState=EStatePrepareToSendMsg;

	delete iEntry;
	iEntry=NULL;
	iEntry=iSession.GetEntryL(iMsgId);
	iMsgEntry=iEntry->Entry();
	iMsgEntry.SetSendingState(KMsvSendStateWaiting);
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iEntry->ChangeL(iMsgEntry, iStatus);
	SetActive();
	}

void CTestMessProcessor::MtmRequestL(TMsvId aAccountId, MTestMessProcessorClient* aClient)
	{
	iMsgId=aAccountId;
	iClient=aClient;
	iState=EStateRequest;

	delete iEntry;
	iEntry=NULL;
	iEntry=iSession.GetEntryL(iMsgId);
	iStatus = KRequestPending;
	delete iMsvOperation;
	iMsvOperation=NULL;
	TRAPD(err, iMsvOperation=iClient->CreateOperationL(iStatus));
	if ( iTestStep.TestStepResult() == EPass && err==KErrNone )
		{
		SetActive();
		}
	}

void CTestMessProcessor::RunL()
	{
	switch(iState)
		{
	case EStatePrepareToSendMsg:
		switch (iStatus.Int())
			{
		case KErrNone:
			{
			delete iMsvOperation;
			iMsvOperation=NULL;
			delete iEntry;
			iEntry=NULL;
			iEntry=iSession.GetEntryL(iMsgId);
			iMsgEntry=iEntry->Entry();
			iMsgEntry.SetSendingState(KMsvSendStateWaiting);
			iEntry->ChangeL(iMsgEntry);
			if ( iMsgEntry.SendingState() == KMsvSendStateWaiting )
				{
				iState=EStateRequest;
				iStatus = KRequestPending;
				TRAPD(err, iMsvOperation=iClient->CreateOperationL(iStatus));
				if ( iTestStep.TestStepResult() == EPass && err==KErrNone )
					{
					SetActive();
					}
				else
					{
					CActiveScheduler::Stop();
					}
				}
			else
				{
				iTestStep.ERR_PRINTF2(_L("Sending State not Waiting :%d"), iMsgEntry.SendingState());
				iTestStep.SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
			break;
		case KErrCancel:
			{
			if(iCancelSms)
				{
				iTestStep.INFO_PRINTF1(_L("Sending of the mesage Cancelled successfully"));
				}
			else
				{
				iTestStep.ERR_PRINTF2(_L("Message failed to Send, Error = %d"), iStatus.Int());
				iTestStep.SetTestStepResult(EFail);
				}
			CActiveScheduler::Stop();
			break;
			}
		default:
			iTestStep.ERR_PRINTF2(_L("Unknown Event :%d"), iStatus.Int());
			iTestStep.SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		break;
	case EStateRequest:
		switch (iStatus.Int())
			{
		case KErrNone:
			{
			TInt	error=iClient->DisplayProgressL(ETrue, iMsvOperation);
			if ( error != KErrNone )
				{
				iTestStep.ERR_PRINTF2(_L("Progress Error:%d"), error);
				iTestStep.SetTestStepResult(EFail);
				}
			delete iMsvOperation;
			iMsvOperation=NULL;

			//	Is there another operation
			if ( iClient->MoreOperations() )
				{
				iStatus = KRequestPending;
				TRAPD(err, iMsvOperation=iClient->CreateOperationL(iStatus));
				if ( iTestStep.TestStepResult() == EPass && err==KErrNone )
					{
					SetActive();
					}
				else
					{
					CActiveScheduler::Stop();
					}
				}
			else
				{
				CActiveScheduler::Stop();
				}
			}
			break;
		case KErrServerTerminated:
			{
			TInt	error=iClient->DisplayProgressL(ETrue, iMsvOperation);
			if ( error != KErrServerTerminated )
				{
				iTestStep.ERR_PRINTF2(_L("Progress Error:%d"), error);
				iTestStep.SetTestStepResult(EFail);
				}
			delete iMsvOperation;
			iMsvOperation=NULL;
			CActiveScheduler::Stop();
			}
		default:
			iTestStep.ERR_PRINTF2(_L("Unknown Event :%d"), iStatus.Int());
			iTestStep.SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		break;
	default:
		iTestStep.ERR_PRINTF1(_L("No handler found in CTestMessProcessor class"));
		iTestStep.SetTestStepResult(EFail);
		CActiveScheduler::Stop();
		}
	}
