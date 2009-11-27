// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Checks for notifications when application list changes.\n
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <f32file.h>
#include <fbs.h>
#include <apaid.h>
#include <apgaplst.h>
#include <apaflrec.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <apsserv.h>
#include <apfrec.h>
#include <datastor.h>
#include <apgicnfl.h>
#include <apasvst.h>
#include <apgnotif.h>
#include <e32test.h>

#include "appfwk_test_utils.h"
#include "T_NotifStep.h"


/**
 
  Overridden from virtual method MApaAppListServObserver::HandleAppListEvent().
  This method is used to receive notification for change in application list.\n
 
*/
void CTestObserver::HandleAppListEvent(TInt /*aEvent*/)
	{

	iNotified++;
	if (iNotifier)
		iNotifier->Cancel();
	CActiveScheduler::Stop();
	}
	
/**
  Auxiliary Fn for Test Case ID T-NotifStep-TestAppNotificationL
 
  Copy a registration resource file in the path  "c:\private\10003a3f\import\apps" .
*/
void CT_NotifStep::CreateAppL(const TDesC& aAppName)
	{
	TFileName appFullName;
	TFileName appTargetName; 
	CFileMan* fileManager = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileManager);	
	appFullName.Format(_L("z:\\ApparcTest\\%S_reg.RSC"),&aAppName);
	appTargetName.Format(_L("C:\\Private\\10003a3f\\Import\\apps\\%S_reg.Rsc"), &aAppName);
	INFO_PRINTF2(_L("copying the file: %S"), &appTargetName);
	TInt ret = fileManager->Copy (appFullName, appTargetName, CFileMan::ERecurse);
	TEST(ret==KErrNone);
	CleanupStack::PopAndDestroy(fileManager);
	}

/**
	Auxiliary Fn for Test Case ID T-NotifStep-TestAppNotificationL
	Delete a registration resource file from the path  "c:\private\10003a3f\import\apps" .
*/
void CT_NotifStep::DeleteAppL(const TDesC& aAppName)
	{
	TFileName appFullName;
	CFileMan* fileManager = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileManager);
	appFullName.Format(_L("C:\\Private\\10003a3f\\Import\\apps\\%S_reg.Rsc"),&aAppName);
		
	INFO_PRINTF2(_L("Deleting the file: %S"), &appFullName);
	TRequestStatus status;
	TTime tempTime(0); // added tempTime to avoid asynch CFileMan::Attribs request completing with KErrArgument
	TEST(fileManager->Attribs(appFullName,0,KEntryAttReadOnly, tempTime, CFileMan::ERecurse, status)==KErrNone);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	TInt ret = fileManager->Delete(appFullName, CFileMan::ERecurse);
	TEST(ret==KErrNone);
	CleanupStack::PopAndDestroy(fileManager);
	}
 
/**
   @SYMTestCaseID T-NotifStep-TestAppNotificationL
  
   @SYMPREQ REQ1087, PCHY-5L3RDW
  
   @SYMTestCaseDesc Test whether notification is received from CApaAppListNotifier
   whenever application list changes. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The method creates an object of user-defined observer CTestObserver
   and creates a CApaAppListNotifier active object which is associated to the
   observer. It then starts the active scheduler and adds a new application or deletes
   an application from the application list. To ensure that notifications are received on
   change in application list HandleAppListEvent() method which is derived
   from interface class MApaAppListServObserver is observed.\n
   API Calls:\n	
   CApaAppListNotifier::NewL(MApaAppListServObserver* aObserver, TPriority aPriority)\n
   
   @SYMTestExpectedResults On deletion of the app the notification for change in
   the application list is received.
    
 */
void CT_NotifStep::TestAppNotificationL()
	{
	// Force the applist to be updated 
	//To ensure that server has time to count all applications in the system
	RPointerArray<TDesC> dummy;
	User::LeaveIfError(iSession.ForceRegistration(dummy));

	TInt theAppCount = 0;
	TInt theErr1 = iSession.AppCount(theAppCount);
	TEST(theErr1==KErrNone);

	INFO_PRINTF2(_L("The number of applications : %d"), theAppCount);
		
	CTestObserver* obs=new(ELeave) CTestObserver();
	CleanupStack::PushL(obs);
	CApaAppListNotifier* notif=CApaAppListNotifier::NewL(obs,CActive::EPriorityHigh);
	CleanupStack::PushL(notif);
	obs->iNotifier=notif;	
	INFO_PRINTF1(_L("Creating and deleting apps for notification"));
	CreateAppL(_L("AAA"));

	CActiveScheduler::Start();
	
	TInt theAppCount1 = 0;
	theErr1 = iSession.AppCount(theAppCount1);
	TEST((theAppCount1 - 1) == theAppCount);
	INFO_PRINTF2(_L("The number of applications : %d"), theAppCount1);
	CleanupStack::PopAndDestroy(notif);
	
	notif = CApaAppListNotifier::NewL(obs,CActive::EPriorityHigh);
	CleanupStack::PushL(notif);
	obs->iNotifier = notif;
	INFO_PRINTF1(_L("Deleting the application"));
	DeleteAppL(_L("AAA")); 

	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(notif);	
	User::LeaveIfError(iSession.ForceRegistration(dummy));	
	theErr1 = iSession.AppCount(theAppCount1);
	TEST(theErr1==KErrNone);
	TEST(theAppCount1 == theAppCount);
	
	INFO_PRINTF2(_L("The number of applications : %d"), theAppCount1);
	TEST(obs->iNotified>0);
	
	INFO_PRINTF2(_L("Received %d notifications"),obs->iNotified);
	CleanupStack::PopAndDestroy(obs);
	}

void CT_NotifStep::ModifyIconFileTimeStamp()
	{
	_LIT(KMbmIconFile, "c:\\resource\\apps\\tupgradeiconapp.mbm");
	_LIT(KTestIconFile, "c:\\TestUpgradeIcon\\tupgradeiconapp.mbm");
	_LIT(KTestIconFileTempPath, "c:\\TestUpgradeIcon\\");

	// Create KMbmIconFileTempPath
	TInt ret = iUtils.CreateDirectoryL(KTestIconFileTempPath);
	TEST(ret == KErrNone || ret == KErrAlreadyExists);
	INFO_PRINTF1(_L("Copy icon file from C: drive to temp path"));
	ret = iUtils.CopyFileL(KMbmIconFile, KTestIconFileTempPath);
	
	INFO_PRINTF1(_L("Modify timestamp of the icon file"));
	TTime modifiedTime(0);
	modifiedTime.HomeTime();
 	ret = iFs.SetModified(KTestIconFile, modifiedTime);
 	TEST(ret==KErrNone);
 	ret = iUtils.SetReadOnly(KTestIconFile, 0);
	TEST(ret==KErrNone);	
	INFO_PRINTF2(_L("Replace the modified icon file in C: drive (%S)"), &KMbmIconFile);
 	ret = iUtils.CopyFileL(KTestIconFile, KMbmIconFile);
	// wait 5 seconds to finish re-scan
	User::After(5*1000000);
	TEST(ret==KErrNone);
	}

/**
   @SYMTestCaseID	T-NotifStep-TTestIconFileNotificationL
  
   @SYMPREQ 		PDEF102804
  
   @SYMTestCaseDesc Test whether notification is received from CApaAppListNotifier   whenever the application icon file is changed
   
   @SYMTestPriority High 
  
   @SYMTestStatus 	Implemented
   
   @SYMTestActions 	Modify the application icon file in a temporary location. Move icon file to a location specified in the resource file.
   
   @SYMTestExpectedResults A notification is recieved when the updated icon file is replaced to a location specified in the resource file.
*/ 
void CT_NotifStep::TestIconFileNotificationL()
	{
	// Force the applist to be updated 
	//To ensure that server has time to count all applications in the system
	RPointerArray<TDesC> dummy;
	User::LeaveIfError(iSession.ForceRegistration(dummy));

	// Create observer
	CTestObserver* obs = new(ELeave) CTestObserver();
	CleanupStack::PushL(obs);
	CApaAppListNotifier* notif = CApaAppListNotifier::NewL(obs, CActive::EPriorityHigh);
	CleanupStack::PushL(notif);
	obs->iNotifier=notif;

	// Change the timestamp of the icon file
	INFO_PRINTF1(_L("Modifying the icon file...attempt to check it's notified"));
	ModifyIconFileTimeStamp();
	
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(notif);
	// Test if notification is recieved.
	TEST(obs->iNotified > 0);
	if (obs->iNotified > 0)
		{
		INFO_PRINTF2(_L("Received %d notifications"),obs->iNotified);
		}
	
	CleanupStack::PopAndDestroy(obs); //obs
	}

CT_NotifStep::~CT_NotifStep()
/**
   Destructor
 */
	{
	delete iScheduler;
	}

CT_NotifStep::CT_NotifStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_NotifStep);
	}

TVerdict CT_NotifStep::doTestStepL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Testing Apparc...T_Notif"));

	TInt ret = FbsStartup();
	TEST(ret==KErrNone);
	ret=RFbsSession::Connect();
	TEST(ret==KErrNone);
	// start an active scheduler
	iScheduler=new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);

	TEST(KErrNone == iFs.Connect());
	TEST(KErrNone == iSession.Connect());
	TEST(KErrNone == iUtils.Connect());

	// run the testcode (inside an alloc heaven harness)	
	__UHEAP_MARK;
#if defined (__WINSCW__)
	INFO_PRINTF1(_L("T-NotifStep-TTestIconFileNotificationL Test Started..."));
	TRAP(ret,TestIconFileNotificationL());
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("TestIconFileNotificationL() finished with return code '%d'\n"), ret);
#endif
	INFO_PRINTF1(_L("T-NotifStep-TestAppNotificationL Test Started..."));
	TRAP(ret,TestAppNotificationL());
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("TestAppNotificationL() finished with return code '%d'\n"), ret);
	__UHEAP_MARKEND;
	
	iUtils.Close();
	iSession.Close();
	iFs.Close();
	RFbsSession::Disconnect();
	INFO_PRINTF1(_L("T_Notif Completed."));
	return TestStepResult();
	}
