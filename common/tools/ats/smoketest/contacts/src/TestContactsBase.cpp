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
// This CTestContactsBase
// 
//

// User include
#include "TestContactsBase.h"

// EPOC includes
#include <e32base.h>
#include <Uri16.h>
#include <UriUtils.h>
#include <TestExecuteLog.h>

/*@{*/
/// Literal constants
_LIT(KDef,			"default");
_LIT(KFileName,		"filenameSecure");
_LIT(KKey,			"key");
/*@}*/


// constructor
CTestContactsBase::CTestContactsBase()
:	iDatabase(NULL)
,	iFieldDef(NULL)
,	iSchedular(NULL)
	{
	}

// destructor
CTestContactsBase::~CTestContactsBase()
	{
	delete iFieldDef;
	iFieldDef=NULL;

	delete iSchedular;
	iSchedular=NULL;
	CActiveScheduler::Install(NULL);
	}

enum TVerdict CTestContactsBase::doTestStepPreambleL()
	{
	iSchedular=new (ELeave)CActiveScheduler();
	CActiveScheduler::Install(iSchedular);

	iFieldDef=new(ELeave) CContactItemFieldDef;
	iFieldDef->AppendL(KUidContactFieldFamilyName);
	iFieldDef->AppendL(KUidContactFieldGivenName);
	return CTestStep::doTestStepPreambleL();
	}

enum TVerdict CTestContactsBase::doTestStepPostambleL()
	{
	delete iFieldDef;
	iFieldDef=NULL;

	delete iSchedular;
	iSchedular=NULL;
	CActiveScheduler::Install(NULL);

	return CTestStep::doTestStepPostambleL();
	}

// Create a new database
void CTestContactsBase::CreateDatabase()
	{
	TPtrC	ptrFileName;
	TBool	returnValue =GetStringFromConfig(ConfigSection(), KFileName(), ptrFileName);
	if (!returnValue)
		returnValue=GetStringFromConfig(KDef(), KFileName(), ptrFileName);

	//	Create database
	if (returnValue)
		{
		INFO_PRINTF2(_L("filename = %S"), &ptrFileName);
		TRAPD(r, iDatabase=CContactDatabase::ReplaceL(ptrFileName))
		if (r!=KErrNone)
			{
			ERR_PRINTF2(_L("Creating Contacts file failed. Error %d"), r);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		INFO_PRINTF1(_L("No Contacts file specified. Open default file."));
		TRAPD(r, iDatabase=CContactDatabase::ReplaceL())
		if (r!=KErrNone)
			{
			ERR_PRINTF2(_L("Creating Contacts file failed. Error %d"), r);
			SetTestStepResult(EFail);
			}
		}
	}

void CTestContactsBase::OpenDatabase()
	{
	TPtrC	ptrFileName;
	TBool	returnValue =GetStringFromConfig(ConfigSection(), KFileName(), ptrFileName);
	if (!returnValue)
		returnValue=GetStringFromConfig(KDef(), KFileName(), ptrFileName);

	//	Open database
	if (returnValue)
		{
		INFO_PRINTF2(_L("filename = %S"), &ptrFileName);
		RaiseInstrumentationEventNotificationL(17);
		TRAPD(r, iDatabase=CContactDatabase::OpenL(ptrFileName));
		RaiseInstrumentationEventNotificationL(18);
		if (r!=KErrNone)
			{
			ERR_PRINTF2(_L("Opening Contacts file failed. Error %d"), r);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		INFO_PRINTF1(_L("No Contacts file specified. Open default file."));
		RaiseInstrumentationEventNotificationL(17);
		TRAPD(r, iDatabase=CContactDatabase::OpenL(CContactDatabase::EMultiThread));
		RaiseInstrumentationEventNotificationL(18);
		if (r!=KErrNone)
			{
			ERR_PRINTF2(_L("Opening Contacts file failed. Error %d"), r);
			SetTestStepResult(EFail);
			}
		}
	}

void CTestContactsBase::CleanupDatabase()
	{
	delete iDatabase;
	iDatabase=NULL;
	}

void CTestContactsBase::RaiseInstrumentationEventNotificationL(TInt aEventTag)
	{
	INFO_PRINTF2(_L("RaiseEventNotification %d"), aEventTag);
	}

CContactIdArray* CTestContactsBase::SearchEntriesL()
/**
This function searches in the contacts database and populates an array
with ids' of the contacts which satisfy the search criterion
read from the ini file.
@return - Pointer to the contact ID array
@leave system wide error code
 */
	{
	TPtrC	ptrKey;
	if (!GetStringFromConfig(ConfigSection(), KKey, ptrKey))
		{
		ERR_PRINTF1(_L("No key value for entry"));
		SetTestStepResult(EFail);
		}
	INFO_PRINTF2(_L("Key Value for Entry Search = %S"), &ptrKey);

	// The function searches the fields contained in the field definition
	INFO_PRINTF1(_L("Searching through the database for entry"));

	CContactIdArray*	ret=iDatabase->FindLC(ptrKey, iFieldDef);
	CleanupStack::Pop(ret);
	return ret;
	}
