// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "T_ServiceRegistryStep.h"
#include <ServiceRegistry.h>
#include "testableapalssession.h"
#include "appfwk_test_utils.h"

/**
 * Constructor
 */	
CT_ServiceRegistryTestStep::CT_ServiceRegistryTestStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_ServiceRegistryStep);
	}

/**
 * Destructor
 */
CT_ServiceRegistryTestStep::~CT_ServiceRegistryTestStep()
	{
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */	
TVerdict CT_ServiceRegistryTestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_ServiceRegistryTestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CT_ServiceRegistryTestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparc...T_ServiceRegistry Test Cases Running..."));

    // start an active scheduler
    iScheduler=new(ELeave) CActiveScheduler();
    CActiveScheduler::Install(iScheduler);
    
	__UHEAP_MARK;
	TInt ret;
	TRAP(ret,RunTestCasesL())
	TEST(ret==KErrNone);
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("...T_ServiceRegistry Test Cases Completed."));

	return TestStepResult();
	}

TInt CT_ServiceRegistryTestStep::RunTestCasesL()
	{
	RTestableApaLsSession ls;
	TEST(KErrNone == ls.Connect());
	CleanupClosePushL(ls);

	// Use DONT_CHECK because it complaints of heap unbalance (a CTypeStoreManager object, althought it is not actually leaked,
	//   but reallocated in CApaAppListServer::DoUpdateTypeStoreL(void)). 
	//   This unbalance happens 'randonmly', possibly a timing issue, as the TypeStore refresh is performed in response to a callback from 
	//	 a CApaFsMonitor object, which introduces an extra 0.25 second delay before invoking the callback.
	//	 *** See DEF101056 ****
	HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, TestAssociation1L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, TestAppForDataTypeAndServiceL(ls), NO_CLEANUP);
    HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, TestUpdateOfServiceRegistryOnAppUninstallationL(ls), NO_CLEANUP);
    
	CleanupStack::PopAndDestroy(&ls);	
	return KErrNone;
	}
	
/**
   @SYMTestCaseID T-ServiceRegistryTestStep-TestAssociation1L
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Test the functionality of ServiceRegistry 
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Tests the addition, retrieval and deletions of 
   associations
   
   @SYMTestExpectedResults The association are added, retrieved and deleted
   from the registry
 */
void CT_ServiceRegistryTestStep::TestAssociation1L()
	{
	INFO_PRINTF1(_L("TestAssociation1L"));
	CServiceRegistry* registry = CServiceRegistry::NewL();
	CleanupStack::PushL(registry);
	
	const TUid uid1 = { 0x11111111 };
	const TUid uid2 = { 0x22222222 };
	
	const TUid serviceUid1 = { 0x00001111 };
	const TUid serviceUid2 = { 0x00002222 };

	const TDataType data1(uid1);
	const TDataType data2(uid2);
	
	TUid result;
	
	TInt err = registry->GetDefault(serviceUid1, data1, result);
	TEST(err == KErrNotFound);
	TEST(result == KNullUid);
	
	err = registry->SetDefault(serviceUid1, data1, uid1);
	TEST(err == KErrNone);
	err = registry->SetDefault(serviceUid2, data2, uid2);
	TEST(err == KErrNone);
	
	err = registry->GetDefault(serviceUid1, data1, result);
	TEST(err == KErrNone);
	TEST(result == uid1);
	
	err = registry->GetDefault(serviceUid2, data2, result);
	TEST(err == KErrNone);
	TEST(result == uid2);
	
	err = registry->RemoveEntry(serviceUid1, data1);
	TEST(err == KErrNone);
	
	err = registry->RemoveEntry(serviceUid1, data1);
	TEST(err == KErrNotFound);
	
	err = registry->GetDefault(serviceUid1, data1, result);
	TEST(err == KErrNotFound);
	TEST(result == KNullUid);
	
	err = registry->GetDefault(serviceUid2, data2, result);
	TEST(err == KErrNone);
	TEST(result == uid2);
	
	err = registry->RemoveEntry(serviceUid2, data2);
	TEST(err == KErrNone);
	
	err = registry->GetDefault(serviceUid2, data2, result);
	TEST(err == KErrNotFound);
	TEST(result == KNullUid);
	
	CleanupStack::PopAndDestroy(registry);
	}
	
/**
   @SYMTestCaseID T-ServiceRegistryTestStep-TestAppForDataTypeAndServiceL
  
   @SYMPREQ 
  
   @SYMTestCaseDesc Test the functionality of ServiceRegistry 
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Tests that the RApaLsSession::AppForDataTypeAndService
   function behaves correctly when associations are added to the registry
   
   @SYMTestExpectedResults The correct application is returned
 */
void CT_ServiceRegistryTestStep::TestAppForDataTypeAndServiceL(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("TestAppForDataTypeAndServiceL"));

	const TUid KUidServerApp = {0x10004c56};
	const TUid KUidServerApp2 = {0x10004c58};
	const TUid KUidServerAppDummy = {0x12344268};
	const TUid KUidService01020333 = {0x01020333};
	_LIT8(KLitMimeTypePriority,"text/priority");

	CServiceRegistry* registry = CServiceRegistry::NewL();
	CleanupStack::PushL(registry);

	// Check first the default association (no association in registry)
	TUid appUid;
	TInt err = aLs.AppForDataTypeAndService(TDataType(KLitMimeTypePriority), KUidService01020333,appUid);
	TEST(err == KErrNone);
	TEST(appUid == KUidServerApp);

	// Check after having added an association in the registry
	err = registry->SetDefault(KUidService01020333, TDataType(KLitMimeTypePriority), KUidServerApp2);
	TEST(err == KErrNone);

	err = aLs.AppForDataTypeAndService(TDataType(KLitMimeTypePriority), KUidService01020333,appUid);
	TEST(err == KErrNone);
	TEST(appUid == KUidServerApp2);

	err = registry->RemoveEntry(KUidService01020333, TDataType(KLitMimeTypePriority));
	TEST(err == KErrNone);

	err = aLs.AppForDataTypeAndService(TDataType(KLitMimeTypePriority), KUidService01020333,appUid);
	TEST(err == KErrNone);
	TEST(appUid == KUidServerApp);

	// Check association with app that doesn't exist
	err = registry->SetDefault(KUidService01020333, TDataType(KLitMimeTypePriority), KUidServerAppDummy);
	TEST(err == KErrNone);

	err = aLs.AppForDataTypeAndService(TDataType(KLitMimeTypePriority), KUidService01020333,appUid);
	TEST(err == KErrNone);
	TEST(appUid == KUidServerApp);

	err = registry->RemoveEntry(KUidService01020333, TDataType(KLitMimeTypePriority));
	TEST(err == KErrNone);

	err = aLs.AppForDataTypeAndService(TDataType(KLitMimeTypePriority), KUidService01020333,appUid);
	TEST(err == KErrNone);
	TEST(appUid == KUidServerApp);

	CleanupStack::PopAndDestroy(registry);
	}

/**
   @SYMTestCaseID APPFWK-APPARC-0104
  
   @SYMDEF INC138417 
  
   @SYMTestCaseDesc Tests whether a data mapping of an applicaiton is removed from 
                    service registry if that applicaiton is uninstalled.
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Inserts data mapping for an applicaion and uninstalls that application.
                   Tests whether the inserted data mapping is removed from the service registry. 
   
   @SYMTestExpectedResults The inserted data mapping is removed from the service registry.
 */

void CT_ServiceRegistryTestStep::TestUpdateOfServiceRegistryOnAppUninstallationL(RApaLsSession& aLs)   
    {
    INFO_PRINTF1(_L("TestUpdateOfServiceRegistryOnAppUninstallationL"));
    RSmlTestUtils fs;
    CleanupClosePushL(fs);
    User::LeaveIfError(fs.Connect());

     _LIT(KImportAppsDir,"c:\\private\\10003a3f\\import\\apps\\");
     
     TInt err = fs.CreateDirectoryL(KImportAppsDir);
     TESTEL((err == KErrNone || err == KErrAlreadyExists), err);
     INFO_PRINTF1(_L("c:\\private\\10003a3f\\import\\apps is created successfully or already exists"));

     
     // Copy TestUpdRegAppUninstallation_reg.rsc from z:\ to c:\private\10003a3f\import\apps\.
     _LIT(KAppRscSourcePath,"z:\\system\\data\\TestUpdRegAppUninstallation_reg.rsc");
     _LIT(KAppRscTargetPath,"c:\\private\\10003a3f\\import\\apps\\TestUpdRegAppUninstallation_reg.rsc");
     
     err = fs.CopyFileL(KAppRscSourcePath, KAppRscTargetPath);
     TEST(err == KErrNone);
     INFO_PRINTF1(_L("Successfully copied TestUpdRegAppUninstallati on_reg.rsc from Z:\\system\\data to c:\\private\\10003a3f\\import\\apps"));

     CServiceRegistry* registry = CServiceRegistry::NewL();
     CleanupStack::PushL(registry);
     
     //Wait till the application scanning is completed.
     CTestApplistChangeObserver* obs = new(ELeave) CTestApplistChangeObserver();
     CleanupStack::PushL(obs);
     CApaAppListNotifier* notif = CApaAppListNotifier::NewL(obs, CActive::EPriorityHigh);
     CleanupStack::PushL(notif);
     obs->iNotifier = notif;
     CActiveScheduler::Start();
     INFO_PRINTF1(_L("Applicaiton scanning is completed."));
     CleanupStack::PopAndDestroy(notif);
 
     TUid uid1 = {0x10207f80};
     TUid resultUid={KNullUidValue};    
     _LIT8(KLitMimeType,"mime/updregappuninstall");
     TDataType dataType (KLitMimeType);
     
     //Test whether 0x10207f80 application is in application list.
     TApaAppInfo appInfo;
     TEST(aLs.GetAppInfo(appInfo,uid1)==KErrNone);

     //Set 0x10207f80 as default application for "mime/updregappuninstall" MIME type.
     registry->SetDefault(KOpenServiceUid,dataType, uid1);
     registry->GetDefault(KOpenServiceUid,dataType, resultUid);
     TEST(uid1==resultUid);
 
     TInt fileExists = EFalse;
     fs.IsFilePresent(KAppRscTargetPath, fileExists);
     if (fileExists)
         {
         fs.ChangeFilePermissionL(KAppRscTargetPath); // clear readonly attribute inherited from z:
         err = fs.DeleteFileL(KAppRscTargetPath);
         if(err == KErrNone) 
             INFO_PRINTF1(_L("Removed file TestUpdRegAppUninstallation_reg.rsc"));
         else 
             INFO_PRINTF1(_L("Failed to remove TestUpdRegAppUninstallation_reg.rsc"));    
         }

     //Wait till the application scanning is completed
     notif = CApaAppListNotifier::NewL(obs, CActive::EPriorityHigh);
      CleanupStack::PushL(notif);
      obs->iNotifier = notif;
      CActiveScheduler::Start();
      INFO_PRINTF1(_L("Applicaiton scanning is completed."));
      CleanupStack::PopAndDestroy(notif);
      CleanupStack::PopAndDestroy(obs);    
     
     //Check the application is removed form the application list
     TEST(aLs.GetAppInfo(appInfo,uid1)==KErrNotFound);
     
     //Check data mappings related to uninstalled applicaiton is also removed.
     TEST(registry->GetDefault(KOpenServiceUid,dataType, resultUid)==KErrNotFound);

     CleanupStack::PopAndDestroy(registry);
     CleanupStack::PopAndDestroy(&fs);  
    }

void CTestApplistChangeObserver::HandleAppListEvent(TInt /*aEvent*/)
    {
    CActiveScheduler::Stop();    
    }
