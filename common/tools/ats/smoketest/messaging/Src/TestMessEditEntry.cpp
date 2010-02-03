// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestMessEditEntry which edits the entry characterstics
// 
//
 
#include "TestMessEditEntry.h"
#include "TestMessEditEntryUtil.h"

/**
 * @name Constant Literals used.
 */
/*@{*/
_LIT(KPtSaveToOutbox,			"SaveToOutbox");
/*@}*/

/// Default value for depth of the count
#define KDefaultDepthCount	1

/**
 * Override of base class pure virtual
 * @param  None
 * @return TVerdict - return TVerdict codes
 * @panic None
 * @leave KErrNoMemory
 *  
*/
void CTestMessEditEntry::ProcessMessageL(CBaseMtm& /*aBaseMtm*/, TMsvId /*aMsgId*/)
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("Edit Entry"));

	if ( TestStepResult() == EPass )
		{
		CTestMessEditEntryUtil*	editUtil=new (ELeave) CTestMessEditEntryUtil(*this, EFalse, EFalse, RMobilePhone::TMobileAddress());
		CleanupStack::PushL(editUtil);

		editUtil->ProcessEntryL(EntryL().EntryId());

		//Read whether the message is saved in outbox
		TBool saveOutbox;
		if( ! GetBoolFromConfig(ConfigSection(), KPtSaveToOutbox, saveOutbox))
			{
			saveOutbox = EFalse;
			}

		INFO_PRINTF2(_L("The message saved in outbox flag is : %S"), &saveOutbox);
		if(saveOutbox)
			{
			//Move the message to outbox
			MoveMessageL(EntryL().EntryId(), KMsvGlobalOutBoxIndexEntryId);
			}
		else
			{
			INFO_PRINTF1(_L("Entry is not moved"));
			}

		CleanupStack::PopAndDestroy(editUtil);
		}
	}

/**
 Move the message the destination folder
 @param aMessageId - TMsvId of the message
 @param aFolderId - TMsvId of the destination folder
 @leave KErrNoMemory
 @leave KErrNotFound
*/
void CTestMessEditEntry::MoveMessageL(TMsvId aMessageId,TMsvId aFolderId)
	{
	// Get CMsvEntry object and the TMsvEntry of the message
	CMsvEntry*		msgvEntry = iSession->GetEntryL(aMessageId);
	CleanupStack::PushL(msgvEntry);
	TMsvEntry entry= msgvEntry->Entry();

	// Get the parent folder Id of the messaage and create the CMsvEntry
	TMsvId			parentId = entry.Parent();
	CMsvEntry*		msvEntry = iSession->GetEntryL(parentId);
	CleanupStack::PushL(msvEntry);

	// Create the active object for handling asynchronous request
	CMoveOperation*	activeOperation = CMoveOperation::NewL();
	CleanupStack::PushL(activeOperation);

	// Move the selected message to given target folder
	CMsvOperation*	op = msvEntry->MoveL(aMessageId, aFolderId, activeOperation->iStatus);
	CleanupStack::PushL(op);

	// Start the Active scheduler 
	activeOperation->SetOperation();
	CActiveScheduler::Start();
	TInt	error = activeOperation->iStatus.Int();
	if(error == KErrNone)
		{
		INFO_PRINTF1(_L("message moved successfully"));
		}
	else
		{
		INFO_PRINTF2(_L("message failed to be moved. Error : %d"),error);
		SetTestStepResult(EFail);
		}
		//Destroy the object
	CleanupStack::PopAndDestroy(4, msgvEntry);
	}

/**
 Constructor for CMoveOperation 
*/
CTestMessEditEntry::CMoveOperation::CMoveOperation()
: CActive(CActive::EPriorityStandard)
	{
	}

/**
 Two phase constructor
 @return CTestMessEditEntry::CMoveOperation* - pointer of CTestMessEditEntry::CMoveOperation
 @leave - KErrNoMemory
 @leave - System wide error codes
*/
CTestMessEditEntry::CMoveOperation* CTestMessEditEntry::CMoveOperation::NewL()
	{
	CMoveOperation* self = new (ELeave) CMoveOperation();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
 Second phase construction
 @leave - sytem wide error codes
*/
void CTestMessEditEntry::CMoveOperation::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

/**
 Sets the operation as active
*/
void CTestMessEditEntry::CMoveOperation::SetOperation()
	{
	SetActive();
	}

/**
 Operation completed event handler.
*/
void CTestMessEditEntry::CMoveOperation::RunL()
	{
	CActiveScheduler::Stop();
	}

/**
 Cancels the active operation
*/
void CTestMessEditEntry::CMoveOperation::DoCancel()
	{
	}
