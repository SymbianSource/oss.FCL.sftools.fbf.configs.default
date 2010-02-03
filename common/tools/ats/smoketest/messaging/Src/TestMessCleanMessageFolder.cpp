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
// This contains CTestMessCleanMessageFolder which cleans out all the
// message folders
// 
//

#include "TestMessCleanMessageFolder.h"

// EPOC includes
#include <msvids.h>
#include <TestExecuteClient.h>

#ifdef __MESSAGING_API_V2__
#include <cemailaccounts.h>
#endif

_LIT(KMessageCount, "messagecount");

CTestMessCleanMessageFolder::CTestMessCleanMessageFolder()
:	CTestMessBase(ETrue)
,	iNumberOfMessagesToDelete(-1)		//	Negative value implies delete all
	{
	SetTestStepName(_L("CleanMessageFolder"));
	}

TVerdict CTestMessCleanMessageFolder::doTestStepL()
	{
	INFO_PRINTF1(_L("Clean message folder"));

	if ( GetIntFromConfig(ConfigSection(), KMessageCount, iNumberOfMessagesToDelete) )
		{
		INFO_PRINTF2(_L("Message count read from the ini file is  %D"), iNumberOfMessagesToDelete);
		}
	else
		{
		WARN_PRINTF1(_L("No Count specified in the ini file"));
		WARN_PRINTF1(_L("All the messages will be deleted"));
		}

	if ( TestStepResult() == EPass )
		{
		//delete the contents of the folder
		CleanFolders(EntryL().EntryId());

		if ( iNumberOfMessagesToDelete<=0 )
			{
			INFO_PRINTF1(_L("Messages deleted"));
			}
		else
			{
			ERR_PRINTF2(_L("%d Outstanding entries not deleted."), iNumberOfMessagesToDelete);
			SetTestStepResult(EFail);
			}
		}

	return TestStepResult();
	}

void CTestMessCleanMessageFolder::CleanFolders(const TMsvId aEntryId)
	{
#ifdef __MESSAGING_API_V2__
	if ( aEntryId == KMsvRootIndexEntryId )
		{
		CEmailAccounts*	accounts = CEmailAccounts::NewLC();

		RArray<TPopAccount>	pop3Accounts;
		CleanupClosePushL(pop3Accounts);
		accounts->GetPopAccountsL(pop3Accounts);
		TInt	count = pop3Accounts.Count();
		TInt	index=0;
		for( index = 0; index < count; index++ )
			{
			TPopAccount	id = pop3Accounts[index];
			accounts->DeletePopAccountL( id );
			}

		RArray<TImapAccount>	imap4Accounts;
		CleanupClosePushL(imap4Accounts);
		accounts->GetImapAccountsL(imap4Accounts);
		count = imap4Accounts.Count();
		for( TInt index = 0; index < count; index++ )
			{
			TImapAccount	id = imap4Accounts[index];
			accounts->DeleteImapAccountL( id );
			}

		CleanupStack::PopAndDestroy(3, accounts);
		}
#endif

	SetEntryL(aEntryId);
	EntryL().SetSortTypeL(SelectionOrdering());
	CMsvEntrySelection*	selection=EntryL().ChildrenL();
	CleanupStack::PushL(selection);
	TInt				count=selection->Count();
	TInt				i;
	for (i=count; i>0; )
		{
		TMsvId	childId=selection->At(--i);

		SetEntryL(aEntryId);
		if( iNumberOfMessagesToDelete!=0 )
			{
			TRAPD(err, EntryL().DeleteL(childId));
			switch ( err )
				{
			case KErrNone:
				INFO_PRINTF2(_L("DeletedId      : %x"), childId);
				if ( iNumberOfMessagesToDelete > 0 )
					{
					--iNumberOfMessagesToDelete;
					}
				break;
			case KErrAccessDenied:
				INFO_PRINTF2(_L("Access Denied  : %x"), childId);
				break;
			default:
				ERR_PRINTF3(_L("Deletion Error : %d , Id : %x "), err, childId);
				SetTestStepResult(EFail);
				break;
				}
			}
		}
	CleanupStack::PopAndDestroy(selection);

	EntryL().SetSortTypeL(SelectionOrdering());
	selection=EntryL().ChildrenL();
	CleanupStack::PushL(selection);
	count=selection->Count();
	for (i=count; i>0; )
		{
		TMsvId	childId=selection->At(--i);

		//	Log child
		CleanFolders(childId);

		SetEntryL(aEntryId);

		TRAPD(err, EntryL().DeleteL(childId));
		switch ( err )
			{
		case KErrNone:
			INFO_PRINTF2(_L("DeletedId      : %x"), childId);
			if ( iNumberOfMessagesToDelete > 0 )
				{
				--iNumberOfMessagesToDelete;
				}
			break;
		case KErrAccessDenied:
			INFO_PRINTF2(_L("Access Denied  : %x"), childId);
			break;
		default:
			ERR_PRINTF3(_L("Deletion Error : %d , Id : %x "), err, childId);
			SetTestStepResult(EFail);
			break;
			}
		}
	CleanupStack::PopAndDestroy(selection);
	}
