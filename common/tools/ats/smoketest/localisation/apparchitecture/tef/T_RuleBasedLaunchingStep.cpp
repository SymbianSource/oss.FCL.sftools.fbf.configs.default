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
// The following test cases test the funtionality of RuleBased plugins framework.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <apacmdln.h>
#include "..\tef\tRuleBasedApps\RuleBasedUID.H" //The UID'S for the 4 applications used during testing
#include "T_RuleBasedLaunchingStep.h"
#include <APGICNFL.H>
#include "testableapalssession.h"

const TUint KNonNativeApplicationType = 0x10207f90;
const TUint KNonNativeApplication = 0xA0000B6E;

_LIT(KLitNativeExecutable, "z:\\sys\\bin\\TNNAPP2.EXE");
_LIT(KLitLogicalExecutableA, "c:\\A.NNAPP2"); // this file never actually needs to exist
_LIT8(KLitMimeTypeA, "x-epoc/a-nnapp2");

CTRuleBasedLaunchingStep::CTRuleBasedLaunchingStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_RuleBasedLaunchingStep);
	}

CTRuleBasedLaunchingStep::~CTRuleBasedLaunchingStep()
	{
	iWs.Close();
	iFs.Close();
	}

void CTRuleBasedLaunchingStep::ExecuteL()
	{
	RTestableApaLsSession theLs;
	User::LeaveIfError(theLs.Connect());
	CleanupClosePushL(theLs);
	
	//DONT_CHECK since app list is updated
	HEAP_TEST_LS_SESSION(theLs, 0, DONT_CHECK, TestLaunchNonNativeApplicationForRuleBasedL(theLs), NO_CLEANUP);	
	//DONT_CHECK since result is unstable
	HEAP_TEST_LS_SESSION(theLs, 0, DONT_CHECK, LaunchAppTests1L(theLs), theLs.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(theLs, 0, 0, LaunchAppTests2L(theLs), theLs.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(theLs, 0, 0, LaunchAppTests3L(theLs), theLs.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(theLs, 0, 0, LaunchAppTests4L(theLs), theLs.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(theLs, 0, 0, LaunchAppTests5L(theLs), theLs.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(theLs, 0, 0, LaunchAppTests6L(theLs), theLs.FlushRecognitionCache() );
	
	CleanupStack::PopAndDestroy(&theLs);
	}
	
LOCAL_C void DeregisterNonNativeForRuleBased(TAny* aApparcServer)
	{
	RApaLsSession& apparcServer = *static_cast<RApaLsSession*>(aApparcServer);
	TRAP_IGNORE(apparcServer.PrepareNonNativeApplicationsUpdatesL());
	TRAP_IGNORE(apparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KNonNativeApplication)));
	TRAP_IGNORE(apparcServer.CommitNonNativeApplicationsUpdatesL());
	TRAP_IGNORE(apparcServer.DeregisterNonNativeApplicationTypeL(TUid::Uid(KNonNativeApplicationType)));
	}

void CTRuleBasedLaunchingStep::TestLaunchNonNativeApplicationForRuleBasedL(RApaLsSession& aApparcServer)
	{
	INFO_PRINTF1(_L("TestLaunchNonNativeApplicationForRuleBasedL..."));

	// make sure we're starting from a clean sheet
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KNonNativeApplication));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationTypeL(TUid::Uid(KNonNativeApplicationType));
	CleanupStack::PushL(TCleanupItem(DeregisterNonNativeForRuleBased, &aApparcServer));

	aApparcServer.RegisterNonNativeApplicationTypeL(TUid::Uid(KNonNativeApplicationType), KLitNativeExecutable);

	CApaRegistrationResourceFileWriter* const registrationResourceFileWriter = CApaRegistrationResourceFileWriter::NewL(TUid::Uid(KNonNativeApplication), KLitLogicalExecutableA, TApaAppCapability::ENonNative);
	CleanupStack::PushL(registrationResourceFileWriter);
	registrationResourceFileWriter->AddDataTypeL(KDataTypePriorityNormal, KLitMimeTypeA);

	CApaCommandLine* const commandLineForNonNativeApp = CApaCommandLine::NewLC();
	commandLineForNonNativeApp->SetExecutableNameL(KLitLogicalExecutableA);

	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KNonNativeApplicationType), EDriveC, *registrationResourceFileWriter, NULL, NULL);
	aApparcServer.CommitNonNativeApplicationsUpdatesL();

	// Since NonNative application will be launched inspite of Rule saying it should not be launched 
	INFO_PRINTF1(_L("Testing launching of a NonNativeApp..."));
	TEST(aApparcServer.StartApp(*commandLineForNonNativeApp) == KErrCancel);

	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KNonNativeApplication));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();

 	// clean-up
	CleanupStack::PopAndDestroy(3); 
	}

/**
@SYMTestCaseID 		APPFWK-APPARC-0023  
@SYMPREQ 			PREQ1122   
@SYMTestCaseDesc 	Rule Based Launching of Applications     
@SYMTestPriority 	High   
@SYMTestStatus 		Implemented   

@SYMTestActions 		The test utilises 4 test applications that simply display the appication name.
   			Each application is launched and closed in sequence.

@SYMTestExpectedResults The test verifies that each application can be opened and closed normally.
*/
void CTRuleBasedLaunchingStep::LaunchAppTests1L(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Test case 1 started"));
	
	// App1 is launched and closed
	TEST(AppLaunchedL(aLs, KUidApp1) == KErrNone);
	TEST(AppClosed(KUidApp1) == KErrNone);

	// App2 is launched and closed
	TEST(AppLaunchedL(aLs, KUidApp2) == KErrNone);
	TEST(AppClosed(KUidApp2) == KErrNone);

	// App3 is launched and closed
	TEST(AppLaunchedL(aLs, KUidApp3) == KErrNone);
	TEST(AppClosed(KUidApp3) == KErrNone);

	// App4 is launched and closed
	TEST(AppLaunchedL(aLs, KUidApp4) == KErrNone);
	TEST(AppClosed(KUidApp4) == KErrNone);
	
	// Close all apps
	ClosedAllTestApp();
	
	INFO_PRINTF1(_L("Test case 1 finished"));
	}
	
/**
@SYMTestCaseID 		APPFWK-APPARC-0024  
@SYMPREQ 			PREQ1122   
@SYMTestCaseDesc 	Rule Based Launching of Applications     
@SYMTestPriority 	High   
@SYMTestStatus 		Implemented

@SYMTestActions 	The test utilises 3 test applications that simply display the appication name.
   			The test utilises a single plugin that implements 2 rules.
   			Only the first rule is executed(Launching App3 closes App1).

@SYMTestExpectedResults The test verifies that that when an application is launched, 
					  the rule based mechanism is invoked and only the corresponding rule executed.
 					      
*/	
void CTRuleBasedLaunchingStep::LaunchAppTests2L(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Test case 2 started"));

	TEST(AppLaunchedL(aLs, KUidApp1) == KErrNone);
	TEST(AppLaunchedL(aLs, KUidApp2) == KErrNone);
	TEST(AppLaunchedL(aLs, KUidApp3) == KErrNone);

	TApaTaskList taskList(iWs);

	//Wait 2sec for App1 to be closed
	User::After(2 * 1000000);

	// App1 should be closed when App3 is launched
	const TApaTask task1 = taskList.FindApp(KUidApp1);
	TEST(!task1.Exists());
	const TApaTask task2 = taskList.FindApp(KUidApp2);
	TEST(task2.Exists());
	const TApaTask task3 = taskList.FindApp(KUidApp3);
	TEST(task3.Exists());
	
	// Close all apps
	ClosedAllTestApp();
	
	INFO_PRINTF1(_L("Test case 2 finished"));
	}
	
/**
@SYMTestCaseID 		APPFWK-APPARC-0025
@SYMPREQ 			PREQ1122 
@SYMTestCaseDesc 	Rule Based Launching of Applications  
@SYMTestPriority 	High 
@SYMTestStatus 		Implemented

@SYMTestActions 	The test utilises 2 test applications that simply display the appication name.
   			Only the first rule is executed(A running App3 prevents App1 launching).
   			Only the second rule is executed(App1 cannot launch if App3 is running).

@SYMTestExpectedResults The test verifies that multiple rules can be implemented in a single plugin and 
                      only the correct rule is executed.
 					      	
*/		
void CTRuleBasedLaunchingStep::LaunchAppTests3L(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Test case 3 started"));
	
	// App1 cannot be launched if App3 is running	
	TEST(AppLaunchedL(aLs, KUidApp2) == KErrNone);
	TEST(AppLaunchedL(aLs, KUidApp3) == KErrNone);
	TEST(AppLaunchedL(aLs, KUidApp1) == KErrCancel);

	TApaTaskList taskList(iWs);

	const TApaTask task1 = taskList.FindApp(KUidApp1);
	TEST(!task1.Exists());
	const TApaTask task2 = taskList.FindApp(KUidApp2);
	TEST(task2.Exists());
	const TApaTask task3 = taskList.FindApp(KUidApp3);
	TEST(task3.Exists());

	// Close all apps
	ClosedAllTestApp();
	
	INFO_PRINTF1(_L("Test case 3 finished"));
	}
	
/**
@SYMTestCaseID 		APPFWK-APPARC-0026
@SYMPREQ 			PREQ1122 
@SYMTestCaseDesc 	Rule Based Launching of Applications  
@SYMTestPriority 	High 
@SYMTestStatus 		Implemented

@SYMTestActions 	The test utilises 3 test applications that simply display the appication name.
   			The test utilises 2 plugins.
   			Rules from both plugins are executed.
   			Plugin1 (Launching App4 closes App2)
   			Plugin2 (Launching App4 closes App3)
   				
@SYMTestExpectedResults The test verifies that multiple rules can be implemented in multiple plugins and 
                      only the correct rules are executed.
*/		
void CTRuleBasedLaunchingStep::LaunchAppTests4L(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Test case 4 started"));
	
	// App2 and App3 are closed if App4 is launched
	TEST(AppLaunchedL(aLs, KUidApp3) == KErrNone);
	TEST(AppLaunchedL(aLs, KUidApp2) == KErrNone);
	TEST(AppLaunchedL(aLs, KUidApp4) == KErrNone);
	
	TApaTaskList taskList(iWs);
	
	//Wait 2sec for App2 and App3 to be closed 
	User::After(2 * 1000000);

	const TApaTask task2 = taskList.FindApp(KUidApp2);
	TEST(!task2.Exists());
	const TApaTask task3 = taskList.FindApp(KUidApp3);
	TEST(!task3.Exists());
	const TApaTask task4 = taskList.FindApp(KUidApp4);
	TEST(task4.Exists());
	
	// Close all apps
	ClosedAllTestApp();
	
	INFO_PRINTF1(_L("Test case 4 finished"));	
	}

/**
@SYMTestCaseID 		APPFWK-APPARC-0027
@SYMPREQ 			PREQ1122 
@SYMTestCaseDesc 	Rule Based Launching of Applications  
@SYMTestPriority 	High 
@SYMTestStatus 		Implemented

@SYMTestActions 	The test cases is similar to APPFWK-APPARC-0025 but it launching applications 
					via associated documents.

@SYMTestExpectedResults The test verifies that multiple rules can be implemented in a single plugin and 
                      only the correct rule is executed.
 					      	
*/		
void CTRuleBasedLaunchingStep::LaunchAppTests5L(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Test case 5 started"));
	
	// App1 cannot be launched if App3 is running
	TEST(LaunchAppFromAssociatedDocument(aLs, KUidApp2) == KErrNone);
	TEST(LaunchAppFromAssociatedDocument(aLs, KUidApp3) == KErrNone);
	// Wait 2sec for App2 and App3 to be closed
	User::After(2 * 1000000);
	TEST(LaunchAppFromAssociatedDocument(aLs, KUidApp1) == KErrCancel);
		
	TApaTaskList taskList(iWs);
	
	const TApaTask task1 = taskList.FindApp(KUidApp1);
	TEST(!task1.Exists());
	const TApaTask task2 = taskList.FindApp(KUidApp2);
	TEST(task2.Exists());
	const TApaTask task3 = taskList.FindApp(KUidApp3);
	TEST(task3.Exists());
	
	// Close all apps
	ClosedAllTestApp();
	
	INFO_PRINTF1(_L("Test case 5 finished"));
	}

/**
@SYMTestCaseID 		APPFWK-APPARC-0028
@SYMPREQ 			PREQ1122 
@SYMTestCaseDesc 	Rule Based Launching of Applications  
@SYMTestPriority 	High 
@SYMTestStatus 		Implemented

@SYMTestActions 	The test case launches an applications where file name defined whether without extension or 
					disk drive, or as non system path.

@SYMTestExpectedResults Rule based framework has to process this name correctly and apply pre-defined rules.
 					      	
*/		
void CTRuleBasedLaunchingStep::LaunchAppTests6L(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Test case 6 started"));
	
	TPtrC arFileName[] =  {	_L("tRuleBasedApp1.exe"),
							_L("\\sys\\bin\\tRuleBasedApp1.exe"),
							_L("\\nonsys\\bin\\tRuleBasedApp1.exe"),
							_L("\\sys\\bin\\tRuleBasedApp1"),
							_L("tRuleBasedApp1"),
						  };
	TInt sizeOfFileNameArray = sizeof(arFileName) / sizeof(arFileName[0]);
	
	for(TInt ii = 0; ii < sizeOfFileNameArray; ii++)
		{
		TEST(LaunchAppFromAssociatedDocument(aLs, KUidApp3) == KErrNone);
		TEST(AppLaunchedL(aLs, arFileName[ii]) == KErrCancel);

		TApaTaskList taskList(iWs);

		const TApaTask task1 = taskList.FindApp(KUidApp1);
		TEST(!task1.Exists());
		const TApaTask task3 = taskList.FindApp(KUidApp3);
		TEST(task3.Exists());
		if(task1.Exists() || !task3.Exists())
			{	
			INFO_PRINTF2(_L("The name %S is not recognized by rule based plug-ins as a valid"), &arFileName[ii]);
			}
		
		// Close all apps
		ClosedAllTestApp();
		}
		
	INFO_PRINTF1(_L("Test case 6 finished"));
	}
	
/**
The function is used to launch an app whose UID is supplied as its second input parameter.
The function returns KErrNone upon success, KErrGeneral otherwise.
*/
TInt CTRuleBasedLaunchingStep::AppLaunchedL(RApaLsSession& aLs, const TUid& aAppUid)
	{	
	TApaAppInfo info; 
	TFileName fileName;
	aLs.GetAppInfo(info,aAppUid);
	//Wait 0.5sec for function to complete
	User::After(500000);
	fileName = info.iFullName; 
	CApaCommandLine* cmdLn = CApaCommandLine::NewLC();     
	cmdLn->SetExecutableNameL(fileName);
	TInt result = aLs.StartApp(*cmdLn);
	//Wait 0.5sec for App to start
	User::After(500000);
	CleanupStack::PopAndDestroy(cmdLn);
	return result;
	}

/**
The function is used to launch an application using full file name as param.
*/
TInt CTRuleBasedLaunchingStep::AppLaunchedL(RApaLsSession& aLs, const TDesC& aAppFullFileName)
	{	
	CApaCommandLine* cmdLn = CApaCommandLine::NewLC();     
	cmdLn->SetExecutableNameL(aAppFullFileName);
	TInt result = aLs.StartApp(*cmdLn);
	//Wait 0.5sec for App to start
	User::After(500000);
	CleanupStack::PopAndDestroy(cmdLn);
	return result;
	}

/**
The function is used to launch an application via dummy document associated with this application.
*/
TInt CTRuleBasedLaunchingStep::LaunchAppFromAssociatedDocument(RApaLsSession& aLs,const TUid& aAppUid)
	{	
	TDataType theDataType(aAppUid); 
	TThreadId theThreadId;
	TInt result = aLs.StartDocument(KNullDesC, theDataType, theThreadId);
	//Wait 2sec for App to start
	User::After(2 * 1000000);
	return result;
	}

/**
The function is used to close an app whose UID is supplied as the input parameter.
The function returns KerrNone upon success, KErrGeneral othewise.
*/
TInt CTRuleBasedLaunchingStep::AppClosed(TUid AppUid)
	{
	TInt result = KErrNone;
	TApaTaskList taskList(iWs);
	TApaTask task1 = taskList.FindApp(AppUid);
	if(task1.Exists())
		{
		task1.EndTask();
		//Wait 0.5sec for the App to close
		User::After(500000);

		const TApaTask task2 = taskList.FindApp(AppUid);
		if (task2.Exists())
			{
			result = KErrGeneral;
			}
		}
	return result;
	}

/** The function close all test applications */ 
void CTRuleBasedLaunchingStep::ClosedAllTestApp()
	{
	AppClosed(KUidApp1);
	AppClosed(KUidApp2);
	AppClosed(KUidApp3);
	AppClosed(KUidApp4);
	}

TVerdict CTRuleBasedLaunchingStep::doTestStepL()
	{
	INFO_PRINTF1(_L("TRuleBasedLaunchingStep test started...."));
	
	User::LeaveIfError(FbsStartup());
	TInt ret=RFbsSession::Connect();

	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iWs.Connect());
	
	TEST(ret==KErrNone);
	__UHEAP_MARK;			
	TRAPD(err, ExecuteL()); 	
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("execute tests ended with return value '%d'"), err);
	__UHEAP_MARKEND; 			
	
	RFbsSession::Disconnect();
	INFO_PRINTF1(_L(".... TRuleBasedLaunchingStep test finished!!"));
	return TestStepResult();
	}
