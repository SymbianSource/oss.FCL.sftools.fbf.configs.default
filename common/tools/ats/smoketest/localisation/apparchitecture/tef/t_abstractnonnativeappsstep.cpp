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
 @internalComponent - Internal Symbian test code  
*/

#include "t_abstractnonnativeappsstep.h"

#include <apgicnfl.h>

CT_AbstractNonNativeAppsTestStep::CT_AbstractNonNativeAppsTestStep()
	{
	}

CT_AbstractNonNativeAppsTestStep::~CT_AbstractNonNativeAppsTestStep()
	{
	}

void CT_AbstractNonNativeAppsTestStep::doAbstractNonNativeAppsTestStepPreambleL()
	{
	TInt error = iSession.Connect();
	TEST(error==KErrNone);
	error = iUtils.Connect();
	TEST(error==KErrNone);
	}

void CT_AbstractNonNativeAppsTestStep::CleanupAndReset(RTestableApaLsSession& aSession, const TTestAppDetails& aApp)
	{
	// make sure we're starting from a clean sheet
	CompleteDeregisterL(aSession, aApp);
	aSession.DeregisterNonNativeApplicationTypeL(TUid::Uid(KApplicationType));
	aSession.RegisterNonNativeApplicationTypeL(TUid::Uid(KApplicationType), KLitNativeExecutable);
	}

void CT_AbstractNonNativeAppsTestStep::CallRegisterL(RTestableApaLsSession& aSession, const TTestAppDetails& aApp)
	{
	CApaRegistrationResourceFileWriter* const registrationResourceFileWriter =
		CApaRegistrationResourceFileWriter::NewL(TUid::Uid(aApp.iAppUid), aApp.iLogicalExecutable, TApaAppCapability::ENonNative);
	CleanupStack::PushL(registrationResourceFileWriter);
	aSession.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, *registrationResourceFileWriter, NULL, NULL);
	CleanupStack::PopAndDestroy();
	}

void CT_AbstractNonNativeAppsTestStep::CompleteRegisterL(RTestableApaLsSession& aSession, const TTestAppDetails& aApp)
	{
	aSession.PrepareNonNativeApplicationsUpdatesL();
	CallRegisterL(aSession, aApp);
	aSession.CommitNonNativeApplicationsUpdatesL();
	}

void CT_AbstractNonNativeAppsTestStep::CallDeregisterL(RTestableApaLsSession& aSession, const TTestAppDetails& aApp)
	{
	aSession.DeregisterNonNativeApplicationL(TUid::Uid(aApp.iAppUid));
	}

void CT_AbstractNonNativeAppsTestStep::CompleteDeregisterL(RTestableApaLsSession& aSession, const TTestAppDetails& aApp)
	{
	aSession.PrepareNonNativeApplicationsUpdatesL();
	CallDeregisterL(aSession, aApp);
	aSession.CommitNonNativeApplicationsUpdatesL();
	}

TBool CT_AbstractNonNativeAppsTestStep::FilePresent(const TDesC& aFile)
	{
	TBool present = EFalse;
	TInt err = iUtils.IsFilePresent(aFile, present);
	TEST(err == KErrNone);
	return present;
	}

TBool CT_AbstractNonNativeAppsTestStep::FileAbsent(const TDesC& aFile)
	{
	TBool present = ETrue;
	TInt err = iUtils.IsFilePresent(aFile, present);
	TEST(err == KErrNone);
	return !present;
	}

TBool CT_AbstractNonNativeAppsTestStep::AppPresent(RTestableApaLsSession& aSession, const TTestAppDetails& aApp)
	{
	TApaAppInfo info;
	TInt err = aSession.GetAppInfo(info,TUid::Uid(aApp.iAppUid));
	TEST(err == KErrNone);
	TUid typeUid;
	err = aSession.GetAppType(typeUid, TUid::Uid(aApp.iAppUid));
	TEST(err == KErrNone);
	
	TBool test1 = (info.iUid.iUid == aApp.iAppUid) && (typeUid == TUid::Uid(KApplicationType));
	TBool test2 = FilePresent(aApp.iRegistrationFile);
	TBool test3 = FileAbsent(KLitUpdatedAppsList);
	return test1 && test2 && test3;
	}

TBool CT_AbstractNonNativeAppsTestStep::AppAbsent(RTestableApaLsSession& aSession, const TTestAppDetails& aApp)
	{
	TApaAppInfo info;
	TInt err = aSession.GetAppInfo(info,TUid::Uid(aApp.iAppUid));
	TBool test1 = (err == KErrNotFound);
	TBool test2 = FileAbsent(aApp.iRegistrationFile);
	TBool test3 = FileAbsent(KLitUpdatedAppsList);
	return test1 && test2 && test3;
	}
