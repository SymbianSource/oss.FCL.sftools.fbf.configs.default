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
//

#include <apacmdln.h>
#include "apstart.h"
#include "apststd.h"

const TInt KAppArcAppListInitialCompletionTimeout = 10000000; //10s

CApaAppStart::CApaAppStart()
	{
	}

CApaAppStart::~CApaAppStart()
	{
	iApaLsSession.Close();
	}

/**
Used to create an instance of @c CApaAppStart class

@return An instance of @c CApaAppStart
*/
EXPORT_C CApaAppStart* CApaAppStart::NewL()
	{
 	CApaAppStart* self = new(ELeave) CApaAppStart();
	return self;
	}

/**
Wait for apparc server to complete initial population of its app list. If list 
population doesn't complete within @c KAppArcAppListInitialCompletionTimeout, 
this function will leave with KErrTimedOut.

@panic If Apsexe.exe isn't started.
@leave KErrTimedOut if apparc doesn't complete the initial list population within KAppArcAppListInitialCompletionTimeout.
@leave With system-wide error-codes for generic errors.
*/
void CApaAppStart::WaitForApparcToInitialiseL()
	{
	// Make sure we have a usable session...
	if (iApaLsSession.Handle()==KNullHandle)
		{
		const TInt err = iApaLsSession.Connect(); 
		if(err != KErrNone)
			{
			Panic(EApsexeNotRunning);
			}
		}
	
	//...and a timer
	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer); 
	
	// Request apparc to notify us when the initial list population is complete
	TRequestStatus apparcStatus;
	iApaLsSession.RegisterListPopulationCompleteObserver(apparcStatus);

	// Request a timeout.
	TRequestStatus timerStatus;
	timer.After(timerStatus, TTimeIntervalMicroSeconds32(KAppArcAppListInitialCompletionTimeout));
	
	// As soon as either request completes, cancel the other
	User::WaitForRequest(timerStatus, apparcStatus);
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		// Timeout
		User::LeaveIfError(iApaLsSession.CancelListPopulationCompleteObserver());
		User::WaitForRequest(apparcStatus);
		User::Leave(KErrTimedOut);
		}
		
	CleanupStack::PopAndDestroy(&timer);
	}

/** 
Set up the CApaCommandLine object which will be used to start the app.
*/
void CApaAppStart::SetupCommandLineL(CApaCommandLine& aCmdLine,
					const TDesC& aFileName,
					const TDesC8& aArgs,
					TBool aViewLess,
					TBool aStartInBackground)
	{
	
	aCmdLine.SetExecutableNameL(aFileName);
	aCmdLine.SetTailEndL(aArgs) ;
	
	// Define how the app will be launched 		
	if (!aStartInBackground && !aViewLess)
		{
		aCmdLine.SetCommandL(EApaCommandRun);
		}
	else if (aStartInBackground && !aViewLess)
		{
		aCmdLine.SetCommandL(EApaCommandBackground);
		}
	else if (!aStartInBackground && aViewLess)
		{
		aCmdLine.SetCommandL(EApaCommandRunWithoutViews);
		}
	else 
		{
		aCmdLine.SetCommandL(EApaCommandBackgroundAndWithoutViews);
		}
	}

/**
Start an application as defined by the specified command line information. Feedback about 
when the application is actually up and running is given via @c aRequestStatusForRendezvous.

Rule-based launching and non-native applications are supported only after 
@c WaitForApparcToInitialiseL has been called. 
@see struct INIT_APPARC.

@param aFileName The full filename of the application.
@param aArgs The arguments passed to the application.
@param aViewLess Indicates whether the application has a view.
@param aStartInBackground Indicates whether the application should start in background.
@param aThreadId The id of the main thread for the application.
@param aRequestStatusForRendezvous To be used for deferred return of the application startup status.

@leave With a system-wide error-code e.g. if @c aFileName doesn't exists or if memory couldn't be allcoated.
*/
void CApaAppStart::StartAppL(const TDesC& aFileName, 
					const TDesC& aArgs, 
					TBool aViewLess, 
					TBool aStartInBackground, 
					TThreadId& aThreadId, 
					TRequestStatus& aRequestStatusForRendezvous)
	{
	DoStartAppL(aFileName, aArgs, aViewLess, aStartInBackground, aThreadId, &aRequestStatusForRendezvous);
	}


/**
Start an application as defined by the specified command line information. No feedback is provided 
about when the application is actually up and running.

Rule-based launching and non-native applications are supported only after 
@c WaitForApparcToInitialiseL has been called. 
@see struct INIT_APPARC.

@param aFileName The full filename of the application.
@param aArgs The arguments passed to the application.
@param aViewLess Indicates whether the application has a view.
@param aStartInBackground Indicates whether the application should start in background.
@param aThreadId The id of the main thread for the application.

@leave With a system-wide error-code e.g. if @c aFileName doesn't exists or if memory couldn't be allcoated.
*/
void CApaAppStart::StartAppL(const TDesC& aFileName, 
					const TDesC& aArgs, 
					TBool aViewLess, 
					TBool aStartInBackground, 
					TThreadId& aThreadId)
	{
	DoStartAppL(aFileName, aArgs, aViewLess, aStartInBackground, aThreadId, NULL);
	}


void CApaAppStart::DoStartAppL(const TDesC &aFileName, 
					const TDesC& aArgs, 
					TBool aViewLess, 
					TBool aStartInBackground, 
					TThreadId& aThreadId, 
					TRequestStatus* aRequestStatusForRendezvous)
	{
	// Create an 8-bit variant of aArgs
	RBuf writableArgs;
	writableArgs.CreateL(aArgs);
	CleanupClosePushL(writableArgs);
	TPtr8 args8 = writableArgs.Collapse();
	
	CApaCommandLine* const cmdLine = CApaCommandLine::NewLC();
	SetupCommandLineL(*cmdLine, aFileName, args8, aViewLess, aStartInBackground) ;

	User::LeaveIfError(iApaLsSession.StartApp(*cmdLine, aThreadId, aRequestStatusForRendezvous));
	
	CleanupStack::PopAndDestroy(cmdLine);
	CleanupStack::PopAndDestroy(&writableArgs);
	}
