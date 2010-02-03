// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestAgendaCase which is the base class for all the Agenda TestCase DLL
// 
//

// EPOC includes
#include <e32base.h>
#include <Uri16.h>
#include <UriUtils.h>
#include <TestExecuteLog.h>
#include "TestAgendabase.h"
#include "TestAgendaServerLaunchProgress.h"

#include <calsession.h>



_LIT(KDef,		"default");
_LIT(KFileName,		"filenameSecure");

void CTestAgendaPicture::NewPictureL(TPictureHeader& /*aHeader*/,const CStreamStore& /*aDeferredPictureStore*/)const
/**
Creates a new Picture object for use by agenda model, currently the implementation is empty as our code
does not use any the functionality.
*/
	{
	//empty implementation
	}

CTestAgendaPicture::CTestAgendaPicture()
/**
constructor, currently the implementation is empty
*/
	{
	//empty implementation
	}

CTestAgendaPicture::~CTestAgendaPicture()
/**
destructor, currently the implementation is empty
*/
	{
	//empty implementation
	}

CTestAgendaBase::CTestAgendaBase()
: CTestStep()
, iSchedular(NULL)
, iEnteredBlocked(EFalse)
, iExitedBlocked(EFalse)
, iSessionConnected(EFalse)
, iCalEntryViewBase(NULL)
, iStore(NULL)
, iCalSession(NULL)
	{
	}

// destructor
CTestAgendaBase::~CTestAgendaBase()
	{
	delete iSchedular;
	iSchedular=NULL;
	
	}

enum TVerdict CTestAgendaBase::doTestStepPreambleL()
	{
	TVerdict	ret = CTestStep::doTestStepPreambleL();
	
	INFO_PRINTF1(_L("doTestStepPreambleL"));
	iSchedular=new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iSchedular);
	
	iCalSession = CCalSession::NewL();

	return ret;
	}

enum TVerdict CTestAgendaBase::doTestStepPostambleL()
	{
	
	delete iCalEntryViewBase;
	iCalEntryViewBase=NULL;
	
	delete iSchedular;
	iSchedular=NULL;
	CActiveScheduler::Install(NULL);
	

	return CTestStep::doTestStepPostambleL();
	}

// Create a new database
void CTestAgendaBase::CreateDatabaseL( void )
	{
	TPtrC	ptrFileName;
	TBool	returnValue =GetStringFromConfig(ConfigSection(), KFileName, ptrFileName);
	if ( !returnValue )
		returnValue=GetStringFromConfig(KDef, KFileName, ptrFileName);
	
	if (returnValue)
		{
		INFO_PRINTF2(_L("filename = %S"), &ptrFileName);
		}
	else
		{
		User::Leave(KErrNotFound);
		}

	//	Connect file server
	User::LeaveIfError(iFs.Connect());
	iSessionConnected=ETrue;
	
	TInt r;
	if (iCalSession)
	    {
		TRAP(r, iCalSession->CreateCalFileL(ptrFileName));
	    }
	    
	if (r == KErrAlreadyExists)
	    {
		TRAP_IGNORE(iCalSession->DeleteCalFileL(ptrFileName));
		TRAP(r, iCalSession->CreateCalFileL(ptrFileName));
	    }

        
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		ERR_PRINTF2(_L("Creating Agenda file failed with error %d"), r);
		SetTestStepResult(EFail);
		}
	}

void CTestAgendaBase::OpenDatabaseL( void )
	{
	TPtrC	ptrFileName;
	TBool	returnValue =GetStringFromConfig(ConfigSection(), KFileName, ptrFileName);
	if ( !returnValue )
		returnValue=GetStringFromConfig(KDef, KFileName, ptrFileName);
	

	if (returnValue)
		{
		INFO_PRINTF2(_L("filename = %S"), &ptrFileName);
		}
	else
		{
		User::Leave(KErrNotFound);
		}

	//	Connect file server
	User::LeaveIfError(iFs.Connect());
	iSessionConnected=ETrue;

    
	if (iCalSession)
	    {
		iCalSession->OpenL(ptrFileName);
	    }	
    
	//open the database by creating an object of CTestAgendaServerLaunchProgress
	CTestAgendaServerLaunchProgress*	progress = CTestAgendaServerLaunchProgress::NewL(this);
	CleanupStack::PushL(progress);

	//kick start the launching process
	progress->Start();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(progress);

	}

void CTestAgendaBase::CleanupDatabase( void )
	{
	if (iCalEntryViewBase)
	    {
		delete iCalEntryViewBase;
		iCalEntryViewBase = NULL;
	    }

/*	if (iCalSession)
		{
		delete iCalSession;
		iCalSession=NULL;
		}*/


	if (iSessionConnected)
		{
		iFs.Close();
		iSessionConnected=EFalse;
		}
	}
	
void CTestAgendaBase::SetCalEntryView(CCalEntryView* calEntryViewPtr)
    {
	iCalEntryViewBase = calEntryViewPtr;
    }

CCalSession& CTestAgendaBase::CalSession()
    {
	if (iCalSession)
	    {
		return *iCalSession;
	    }
	else
	    {
	    iCalSession = CCalSession::NewL();
	    
	    return *iCalSession;	
	    }
    }
