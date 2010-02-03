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
// This contains CTestAgendaServerLaunchProgress
// 
//

#include "TestAgendaServerLaunchProgress.h"
#include <calentryview.h>

CTestAgendaServerLaunchProgress* CTestAgendaServerLaunchProgress::NewL(CTestAgendaBase* aAgendaBase)
/**
 * @return - Instance of the agenda progress callback
 */
	{
	CTestAgendaServerLaunchProgress*	progress = new (ELeave) CTestAgendaServerLaunchProgress(aAgendaBase);
	CleanupStack::PushL(progress);
	progress->ConstructL();
	CleanupStack::Pop(progress);
	return progress;
	}

CTestAgendaServerLaunchProgress::CTestAgendaServerLaunchProgress(CTestAgendaBase* aAgendaBase)
:	CActive(EPriorityStandard)
,	iAgendaBase(aAgendaBase)
/**
constructor
*/
	{
	}

void CTestAgendaServerLaunchProgress::ConstructL()
/**
constructor
*/
	{
	//adding this active object into an active scheduler
	CActiveScheduler::Add(this);
	}

CTestAgendaServerLaunchProgress::~CTestAgendaServerLaunchProgress()
/**
destructor
*/
	{
 	}

void CTestAgendaServerLaunchProgress::Completed(TInt aError)
/**
callback function invoked after the file is completely loaded.
@param	TInt aError
*/
	{
	if(aError != KErrNone)
		{
		iAgendaBase ->INFO_PRINTF2(_L("Opening the agenda database file failed with error %D"),aError);
		iAgendaBase ->SetTestStepResult(EFail);
		}
	else
		{
		iAgendaBase ->INFO_PRINTF1(_L("Opening the Agenda database succesful"));
		}
		CActiveScheduler::Stop();
	}

void CTestAgendaServerLaunchProgress::Progress(TInt aPercentageCompleted)
/**
callback function invoked to indicate the progress
@param aPercentageCompleted percent file loaded
*/
	{
	//log the progress percentage of saving the entries
	iAgendaBase ->INFO_PRINTF2(_L("%D%% completed opening the agenda database"),aPercentageCompleted);
	}

TBool CTestAgendaServerLaunchProgress::NotifyProgress()
    {
	return ETrue;
    }
    
void CTestAgendaServerLaunchProgress::RunL()
/**
RunL from CActive class, opens the agenda database file, and builds the indices
@leave system wide error code
*/
	{
	CCalEntryView* calEntryViewPtr = CCalEntryView::NewL(iAgendaBase->CalSession(), *this);
	iAgendaBase->SetCalEntryView(calEntryViewPtr);
	}

void CTestAgendaServerLaunchProgress::DoCancel()
/**
DoCancel of the active object
*/
	{
	iAgendaBase ->INFO_PRINTF1(_L("Inside do cancel of the active object"));
	}


void CTestAgendaServerLaunchProgress::Start(void)
/**
starts the saving process
*/
	{
	TRequestStatus *threadStatus=&iStatus;
	User::RequestComplete(threadStatus,KErrNone);
	SetActive();
	}
	
