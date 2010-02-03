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
// This contains CTestMessSharedData
// 
//

#include "TestMessSharedData.h"

#include <mtclbase.h>
#include <msvids.h>
#include <e32std.h>
#include <imapcmds.h>

_LIT(KName,			"name");

CTestMessSharedData* CTestMessSharedData::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestMessSharedData*	sharedData = new (ELeave) CTestMessSharedData();
	CleanupStack::PushL(sharedData);
	sharedData->ConstructL();
	CleanupStack::Pop(sharedData);
	return sharedData;
	}

CTestMessSharedData::CTestMessSharedData()
:	CBase()
,	iSession(NULL)
,	iMtmRegistry(NULL)
,	iList(NULL)
,	iAnyPostStepToDo(NULL)
,	iDeleteTestStep(EFalse)
	{
	}

void CTestMessSharedData::ConstructL()
	{
	iSession=CMsvSession::OpenSyncL(*this);
	iMtmRegistry=CClientMtmRegistry::NewL(*iSession);
	}

CTestMessSharedData::~CTestMessSharedData()
	{
	while ( iList != NULL )
		{
		CMsgEntry*	entry=iList;
		iList=iList->iNext;
		delete entry;
		}
	delete iMtmRegistry;
	iMtmRegistry=NULL;
	delete iSession;
	iSession=NULL;
	}

void CTestMessSharedData::DoProcessL()
	{
	if ( iAnyPostStepToDo != NULL )
		{
		if (iDeleteTestStep)
			{
			iAnyPostStepToDo->DoCancelProcessL();
			iDeleteTestStep=EFalse;
			}
		else
			{
			iAnyPostStepToDo->DoProcessL(*iMtmRegistry);
			}
		iAnyPostStepToDo=NULL;
		}
	}

void CTestMessSharedData::IMAP4ConnectAndSyncCompleteAfterDisconnectL(TUid aMtmId, TMsvId aMsvId, CTestStep& aStep)
	{
	iList=CMsgIMAP4ConnectAndSyncComplete::NewL(aMtmId, aMsvId, aStep, iList);
	iAnyPostStepToDo=iList;
	}

TInt CTestMessSharedData::DeleteEntryL(CTestStep& aStep)
	{
	TInt	ret=KErrNone;
	TPtrC	name;

	if ( !aStep.GetStringFromConfig(aStep.ConfigSection(), KName, name) )
		{
		aStep.ERR_PRINTF1(_L("Name not found"));
		aStep.SetTestStepResult(EFail);
		ret=KErrArgument;
		}
	else
		{
		aStep.INFO_PRINTF2(_L("Name : %S"), &name);

		CMsgEntry*	msgParent=NULL;
		CMsgEntry*	msgEntry=iList;
		TBool		found=EFalse;

		while ( (msgEntry!=NULL) && !found )
			{
			if ( msgEntry->iName.Compare(name) == 0 )
				{
				if ( msgParent!=NULL )
					{
					msgParent->iNext=msgEntry->iNext;
					}
				else
					{
					iList=msgEntry->iNext;
					}
				iDeleteTestStep=ETrue;
				iAnyPostStepToDo=msgEntry;
				found=ETrue;
				}
			else
				{
				msgParent=msgEntry;
				msgEntry=msgEntry->iNext;
				}
			}

		if ( !found )
			{
			aStep.ERR_PRINTF1(_L("Message Entry not found"));
			aStep.SetTestStepResult(EFail);
			ret=KErrNotFound;
			}
		}

	return ret;
	}

CTestMessSharedData::CMsgEntry::CMsgEntry(CMsgEntry* aList)
:	CActive(EPriorityStandard)
,	iNext(aList)
,	iReadyToBeDeleted(EFalse)
	{
	}

void CTestMessSharedData::CMsgEntry::ConstructL(CTestStep& aStep)
	{
	TPtrC	name;

	//	Read in name tag
	if ( !aStep.GetStringFromConfig(aStep.ConfigSection(), KName, name) )
		{
		aStep.ERR_PRINTF1(_L("Name not found"));
		aStep.SetTestStepResult(EFail);
		}
	else
		{
		aStep.INFO_PRINTF2(_L("Name : %S"), &name);
		iName.Copy(name);
		}
	}

CTestMessSharedData::CMsgEntry::~CMsgEntry()
	{
	}

void CTestMessSharedData::CMsgEntry::DoCancel()
	{
	}

void CTestMessSharedData::CMsgEntry::RunL()
	{
	iReadyToBeDeleted=ETrue;
	}

CTestMessSharedData::CMsgIMAP4ConnectAndSyncComplete* CTestMessSharedData::CMsgIMAP4ConnectAndSyncComplete::NewL(TUid aMtmId, TMsvId aMsvId, CTestStep& aStep, CMsgEntry* aList)
	{
	CMsgIMAP4ConnectAndSyncComplete*	self= new (ELeave) CMsgIMAP4ConnectAndSyncComplete(aMtmId, aMsvId, aList);
	CleanupStack::PushL(self);
	self->ConstructL(aStep);
	CleanupStack::Pop(self);
	return self;
	}

CTestMessSharedData::CMsgIMAP4ConnectAndSyncComplete::CMsgIMAP4ConnectAndSyncComplete(TUid aMtmId, TMsvId aMsvId, CMsgEntry* aList)
:	CMsgEntry(aList)
,	iMtmId(aMtmId)
,	iMsvId(aMsvId)
,	iBaseMtm(NULL)
,	iOperation(NULL)
	{
	}

CTestMessSharedData::CMsgIMAP4ConnectAndSyncComplete::~CMsgIMAP4ConnectAndSyncComplete()
	{
	if ( iOperation != NULL )
		{
		delete iOperation;
		iOperation=NULL;
		}
	delete iBaseMtm;
	iBaseMtm=NULL;
	}

void CTestMessSharedData::CMsgIMAP4ConnectAndSyncComplete::RunL()
	{
	CMsgEntry::RunL();
	delete iOperation;
	iOperation=NULL;
	delete iBaseMtm;
	iBaseMtm=NULL;

	}

void CTestMessSharedData::CMsgIMAP4ConnectAndSyncComplete::DoProcessL(CClientMtmRegistry& aMtmRegistry)
	{
	iBaseMtm=aMtmRegistry.NewMtmL(iMtmId);
	CActiveScheduler::Add(this);

	iBaseMtm->SwitchCurrentEntryL(iMsvId);
	CMsvEntrySelection*	selection=new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(selection);
	selection->Reset();
	selection->AppendL(iMsvId);
	selection->AppendL(iMsvId);	// remote inbox

	TPckg<MMsvImapConnectionObserver*>	parameter(this);
	iOperation=iBaseMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndSyncCompleteAfterDisconnect, *selection, parameter, iStatus);
	CleanupStack::PopAndDestroy(selection);
	SetActive();
	}

void CTestMessSharedData::CMsgIMAP4ConnectAndSyncComplete::DoCancelProcessL()
	{
	if ( iOperation != NULL )
		{
		iOperation->Cancel();
		}
	}

void CTestMessSharedData::CMsgIMAP4ConnectAndSyncComplete::HandleImapConnectionEvent(TImapConnectionEvent aConnectionEvent)
	{
	switch (aConnectionEvent)
		{
	/** Connecting to server. */
	case EConnectingToServer:
		break;
	/** Synchronising folder list. */
	case ESynchronisingFolderList:
		break;
	/** Synchronising inbox. */
	case ESynchronisingInbox:
		break;
	/** Synchronising folders. */
	case ESynchronisingFolders:
		break;
	/** Synchronisation complete. */
	case ESynchronisationComplete:
		break;
	/** Disconnecting. */
	case EDisconnecting:
		break;
	/** Connection completed. */
	case EConnectionCompleted:
		break;
		}
	}
