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
// This contains CTestMessProcessExistingMsg
// Base class for processing an existing message
// 
//



/**
 @file TestMessProcessExistingMsg.cpp
*/

#include "TestMessProcessExistingMsg.h"

// EPOC includes
#include <mtclreg.h>
#include <miuthdr.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <miutmsg.h>

/**
 * Constructor.
 * @return - None
 * 
*/	
CTestMessProcessExistingMsg::CTestMessProcessExistingMsg()
:	CTestMessBase(ETrue)
	{
	}

/**
 * Override of base class pure virtual function
 * @return - TVerdict code
 * @leave - KErrNoMemory - memory allocatior failure
 * @leave - KErrBadLibraryEntryPoint  - Malformed MTM
 * @leave - KErrNotFound - Not found the mtmUid and StepStepResult set to Fail
 * 
*/	
TVerdict CTestMessProcessExistingMsg::doTestStepL()
	{
	if ( TestStepResult() == EPass )
		{
		CClientMtmRegistry*	clientMtmRegistry=CClientMtmRegistry::NewL(*iSession);
		CleanupStack::PushL(clientMtmRegistry);

		iMtmID=EntryL().Entry().iMtm;

		CBaseMtm*	baseMtm=clientMtmRegistry->NewMtmL(iMtmID);
		CleanupStack::PushL(baseMtm);

		// switch the context
		TMsvId		msgId=EntryL().EntryId();
		PrintEntryL(EntryL());
		baseMtm->SwitchCurrentEntryL(msgId);
		baseMtm->LoadMessageL();

		ProcessMessageL(*baseMtm, msgId);

		if ( TestStepResult() == EPass )
			{
			PostProcessMessageL();
			}

		CleanupStack::PopAndDestroy(2, clientMtmRegistry);
		}
	
	return TestStepResult();
	}

/**
 * Nothing done for open
 * @return None
 * @leave KErrNone and TestStepResult set to EFail
 * 
*/
void CTestMessProcessExistingMsg::PostProcessMessageL()
	{
	}
