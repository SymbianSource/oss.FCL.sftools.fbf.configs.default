// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <basched.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikon.hrh>
#include <eikapp.h>
#include <eikdoc.h>
#include <eiklabel.h>
#include <eikfctry.h>
#include <coecntrl.h>
#include <barsread.h>
#include <eiktbar.h>
#include <eikmenub.h>

#include "appfwk_test_AppUi.h"
#include "ApparcTestServer.h"
#include "t_foreground.h"
#include "T_ProStep.h"
#include "T_OOMStep.h"
#include "T_File1Step.h"
#include "T_File2Step.h"
#include "T_File3Step.h"
#include "T_BackupStep.h"
#include "T_MdrStep.h"
#include "T_Serv2Step.h"
#include "T_Serv3Step.h"
#include "T_MruStep.h"
#include "T_CmdlnStep.h"
#include "T_WgnamStep.h"
#include "T_ExeStep.h"
#include "T_NotifStep.h"
#include "T_CaptionStep.h"
#include "T_LocaleStep.h"
#include "T_RApaLsSessionStep.h"
#include "T_ServicesStep.h"
#include "T_ServiceRegistryStep.h"
#include "T_StartAppStep.h"
#include "T_StartDocStep.h"
#include "T_AutoMMCReaderOpen.h"
#include "T_ControlPanelTest.h"
#include "T_RuleBasedLaunchingStep.h"
#include "T_Capability1.h"
#include "T_Capability2.h"
#include "T_DataTypeMappingWithSid1.h"
#include "T_groupNametest.h"
#include "T_groupNametest_ver1.h"
#include "T_groupNametest_ver2.h"
#include "T_WindowChainingStep.h"
#include "T_AppList.h"
#include "T_SysStartApparcStep.h"
#include "T_ProcStep.h"
#include "T_DataMappingPersistenceA.h"
#include "T_DataMappingPersistenceB.h"
#include "T_DataMappingPersistenceC.h"
#include "T_NonNativeAppsStep.h"
#include "T_IntegritySupportStep.h"
#include "T_IntegritySupportRebootStep.h"
#include "T_ApsScan.h"
#include "T_EndTaskStep.h"
#include "T_RecUpgrade.h"
#ifdef SYMBIAN_APPARC_APPINFO_CACHE
#include "T_AppListFileBootStep.h"
#include "T_AppListFileUpdateStep.h"
#endif //SYMBIAN_APPARC_APPINFO_CACHE
#include "t_largestackstep.h"
#include "t_drivenotification.h"
#include "t_mimecontentpolicystep.h"
#include "t_servicebasestep.h"

CApparctestServer* CApparctestServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{
	CApparctestServer * server = new (ELeave) CApparctestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	TParsePtrC serverName(RProcess().FileName());	
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}



CTestStep* CApparctestServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// User::After(TTimeIntervalMicroSeconds32(5000000));
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created 
	if(aStepName == KTestAppListStep)
		{
		testStep = new CTestAppListStep();
		}
	else if(aStepName == KTestForegroundStep)
		{
		testStep = new CTestForegroundStep();
		}
	else if(aStepName == KT_ProStep)
		{
		testStep = new CT_ProStep();
		}
	else if(aStepName == KT_OOMStep)
		{
		testStep = new CT_OOMStep();
		}	
	else if(aStepName == KT_File1Step)
		{
		testStep = new CT_File1Step();
		}
	else if(aStepName == KT_File2Step)
		{
		testStep = new CT_File2Step();
		}
	else if(aStepName == KT_File3Step)
		{
		testStep = new CT_File3Step();
		}
	else if(aStepName == KT_BackupStep)
		{
		testStep = new CT_BackupStep();
		}
	else if(aStepName == KT_MdrStep)
		{
		testStep = new CT_MdrStep();
		}
	else if(aStepName == KT_Serv2Step)
		{
		testStep = new CT_Serv2Step();
		}
	else if(aStepName == KT_Serv3Step)
		{
		testStep = new CT_Serv3Step();
		}
	else if(aStepName == KT_MruStep)
		{
		testStep = new CT_MruStep();
		}
	else if(aStepName == KT_CmdlnStep)
		{
		testStep = new CT_CmdlnStep();
		}
	else if(aStepName == KT_RApaLsSessionStep)
		{
		testStep = new CT_RApaLsSessionTestStep();
		}
	else if(aStepName == KT_NotifStep)
		{
		testStep = new CT_NotifStep();
		}
	else if(aStepName == KT_WgnamStep)
		{
		testStep = new CT_WgnamStep();
		}
	else if(aStepName == KT_ExeStep)
		{
		testStep = new CT_ExeStep();
		}
	else if(aStepName == KT_CaptionStep)
		{
		testStep = new CT_CaptionStep();
		}
	else if(aStepName == KT_LocaleStep)
		{
		testStep = new CT_LocaleStep();
		}
	else if(aStepName == KT_ServicesStep)
		{
		testStep = new CT_ServicesTestStep();
		}
	else if(aStepName == KT_StartAppStep)
		{
		testStep = new CT_StartAppTestStep();
		}
	else if(aStepName == KT_ServiceRegistryStep)
		{
		testStep = new CT_ServiceRegistryTestStep();
		}
	else if(aStepName == KT_StartDocStep)
		{
		testStep = new CT_StartDocStep();
		}
	else if(aStepName == KTestAutoMMCReaderOpenStep)
		{
		testStep = new CTestAutoMMCReaderStep();
		}
	else if(aStepName == KT_ControlPanelTest)
		{
		testStep = new CT_ControlPanelTestStep();
		}
	else if(aStepName == KT_RuleBasedLaunchingStep)
		{
		testStep = new CTRuleBasedLaunchingStep();
		}
	else if(aStepName == KT_GroupNameStep)
		{
		testStep = new CT_GroupNameStep();
		}
	else if(aStepName == KT_GroupNameStep_ver1)
		{
		testStep = new CT_GroupNameStep_ver1();
		}
	else if(aStepName == KT_GroupNameStep_ver2)
		{
		testStep = new CT_GroupNameStep_ver2();
		}
	else if(aStepName == KT_Capability1)
		{
		testStep = new CT_Capability1();
		}
	else if(aStepName == KT_Capability2)
		{
		testStep = new CT_Capability2();
		}
    else if(aStepName == KT_DataTypeMappingWithSid1)
        {
        testStep = new CT_DataTypeMappingWithSid1();
        }
	else if (aStepName == KT_WindowChainingStep)
		{
		testStep = new CT_WindowChainingStep();
		}
	else if( (aStepName == KT_SysStartApparcStep) ||
             (aStepName == KApparcStartupT_TestStartApp1L) ||
             (aStepName == KApparcStartupT_TestStartApp2L) ||
             (aStepName == KApparcStartupT_TestStartApp3L) ||
             (aStepName == KApparcStartupT_TestStartApp4L) ||
             (aStepName == KApparcStartupT_TestStartApp5L) ||
             (aStepName == KApparcStartupT_TestStartApp6L) ||
             (aStepName == KApparcStartupT_TestGetAllApps) ||
             (aStepName == KApparcStartupT_TestInsertDataTypeL) ||
             (aStepName == KApparcStartupT_TestAppForDataTypeL) ||
             (aStepName == KApparcStartupT_TestDeleteDataTypeL) ||
             (aStepName == KApparcStartupT_TestServiceDiscovery) ||
             (aStepName == KApparcStartupT_TestGetAppInfo) ||
             (aStepName == KApparcStartupT_TestAppCount) ||
             (aStepName == KApparcStartupT_TestCreateDoc) ||
             (aStepName == KApparcStartupT_TestLocalisedCaptionL) )
		{
		testStep = new CT_SysStartApparcStep();
		}
	else if (aStepName == KT_ProcStep)
		{
		testStep = new CT_ProcStep();
		}
	else if (aStepName == KT_DataMappingPersistenceAStep)
		{
		testStep = new CT_DataMappingPersistenceATestStep();
		}
	else if (aStepName == KT_DataMappingPersistenceBStep)
		{
		testStep = new CT_DataMappingPersistenceBTestStep();
		}
	else if (aStepName == _L("T_DataMappingPersistenceC"))
		{
		testStep = new CT_DataMappingPersistenceCTestStep();
		}
	else if (aStepName == _L("T_NonNativeApps"))
		{
		testStep = new CT_NonNativeAppsStep();
		}
	else if (aStepName == KT_IntegritySupportStep)
		{
		testStep = new CT_IntegritySupportTestStep();
		}
	else if (aStepName == KT_IntegritySupportReboot1Step)
		{
		testStep = new CT_IntegritySupportReboot1TestStep();
		}
	else if (aStepName == KT_IntegritySupportReboot2Step)
		{
		testStep = new CT_IntegritySupportReboot2TestStep();
		}
	else if (aStepName == KT_ApsScanStep)
		{
		testStep = new CT_ApsScanStep();
		}
	else if (aStepName == KT_EndTaskStep)
		{
		testStep = new CTEndTaskStep();
		}	
	#ifdef SYMBIAN_APPARC_APPINFO_CACHE
	else if (aStepName == KT_AppListFileUpdate)
		{
		testStep = new CT_AppListFileUpdateStep();
		}
	else if (aStepName == KT_AppListFile_Boot1)
		{ 		
		testStep = new CT_AppListFileBootStep(1);
		}
	else if (aStepName == KT_AppListFile_Boot2)
		{
		testStep = new CT_AppListFileBootStep(2);
		}
	else if (aStepName == KT_AppListFile_Boot3)
		{
		testStep = new CT_AppListFileBootStep(3);
		}
	#endif //SYMBIAN_APPARC_APPINFO_CACHE
	else if (aStepName == KT_LargeStackStep)
		{
		testStep = new CT_LargeStackStep();
		}
	else if (aStepName == KT_DriveNotificationStep)
		{
		testStep = new CT_DriveNotificationStep();
		}
	else if (aStepName == KT_MimeContentPolicyStep)
		{
		testStep = new CT_MimeContentPolicyStep();
		}
	else if (aStepName == KT_ServiceBaseStep)
		{
		testStep = new CT_ServiceBaseStep();
		}
	else if (aStepName == KT_RecUpgradeStep)
		{
		testStep = new CT_RecUpgradeStep();
		}
	
	return testStep;
	}


LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CApparctestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CApparctestServer::NewL());;
	
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }
