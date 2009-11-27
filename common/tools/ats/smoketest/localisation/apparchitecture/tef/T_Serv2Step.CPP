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
// Test wrapper to Application Architecture Server.
// Tests Application information, Data recognition and application
// launching capabilities of the application architecture server.\n
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
#include "testableapalssession.h"
#include <apacmdln.h>
#include <apsserv.h>
#include <apfrec.h>
#include <datastor.h>
#include <apgicnfl.h>
#include <apasvst.h>
#include "tstapp.h"
//
#include <e32test.h>
#include "T_Serv2Step.h"

#include <coemain.h>
#include <eikenv.h>
#include "TAppEmbedUids.h"
#include "appfwk_test_utils.h"

// Constants
const TInt KOneSecondDelay = 1000000;


/**
  This function accepts a filename with full path and searches all available
  drives for the presence of file.
  The method returns Filename if a match is found. 
*/
TFileName CT_Serv2Step::SearchAndReturnCompleteFileName(const TDesC& aFileName)
	{
	RFs fs;
	TInt ret = fs.Connect();
	TEST(ret==KErrNone);

	TParsePtrC parse(aFileName);
	TFindFile findFile(fs);
	ret = findFile.FindByDir(parse.NameAndExt(), parse.Path());
	if (ret!=KErrNone)
		{
		TBuf<64> buffer;
		buffer.Format(_L("Error while finding the file, %d"), ret);
		}
	fs.Close();
	return findFile.File();
	}


/**
  @SYMTestCaseID T-Serv2Step-AppInfoTest1
  
  @SYMPREQ
  
  @SYMTestCaseDesc Test RApaLsSession::GetNextApp() 
  
  @SYMTestPriority High 
  
  @SYMTestStatus Implemented
  
  @SYMTestActions Call RApaLsSession::GetAllApps() to initialize the process of
  getting all applications in the cached list. Traverse the application list
  by calling RApaLsSession::GetNextApp() to search for application tstapp.
  Test the application info obtained from GetNextApp() to ensure the application
  info obtained is of tstapp. Complete traversal of the list to ensure that at
  the end of the list, the enum ENoMoreAppInList is returned by GetNextApp().\n
  API Calls:\n	
  RApaLsSession::GetAllApps() const\n
  RApaLsSession::GetNextApp(TApaAppInfo& aInfo) const\n
  
  @SYMTestExpectedResults The test compares the results obtained with expected values.
   
 */
void CT_Serv2Step::AppInfoTest1(RApaLsSession& aLs)
	{
	_LIT(KLitAppPath,"z:\\sys\\bin\\tstapp.exe");
	TFullName appPath(KLitAppPath);
	//Search for TestApp
	TApaAppInfo info;
	TInt ret = aLs.GetAllApps();
	TEST(ret==KErrNone);
	
	do
		{
		ret=aLs.GetNextApp(info);
		} 
		while (ret==KErrNone && info.iUid!=KUidTestApp);
	
	TEST(ret==KErrNone);
	info.iFullName.LowerCase();	
	TEST(info.iFullName.Compare(appPath) == 0);

	//Make sure ENoMoreAppsInList is returned at the end of list.
	do
		{
 		ret=aLs.GetNextApp(info);
 		}
 		while (ret != RApaLsSession::ENoMoreAppsInList);
 
 	ret=aLs.GetNextApp(info);
 	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	}


/**
   @SYMTestCaseID T-Serv2Step-DoCheckServerIconLoadingBySizeL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test loading of Icon by size. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Allocate a flat dynamic buffer which would hold available icon
   sizes for tstapp. Call RApaLsSession::GetAppIconSizes() to store the sizes
   of icons available for the app tstapp to the allocated flat dynamic buffer.
   Call RApaLsSession::GetAppIcon() by passing each of the icon sizes stored.
   Verify whether icon is retrieved each time by GetAppIcon().
   Test calling RApaLsSession::GetAppIcon() by specifying reduced size than
   the actual icon size. Since no match would be found, KErrNotFound should be
   returned. Test GetAppIcon() again by specifying an increased size than the
   actual icon size. Observe whether the GetAppIcon() in this case gets the
   closest icon available.\n
   API Calls:\n	
   RApaLsSession::GetAppIcon(TUid aAppUid, TSize aSize, CApaMaskedBitmap& aAppBitmap) const\n
   
   @SYMTestExpectedResults The results obtained are compared with the expected values to verify
   functionality of RApaLsSession::GetAppIcon().
    
 */
void CT_Serv2Step::DoCheckServerIconLoadingBySizeL(RApaLsSession& aLs)
	{
	CArrayFixFlat<TSize>* array=new(ELeave) CArrayFixFlat<TSize>(3);
	CleanupStack::PushL(array);
	TInt ret=KErrNone;
	ret=aLs.GetAppIconSizes(KUidTestApp,*array);
	if(ret==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST(ret==KErrNone);
	TEST(array->Count());
	for(TInt i=0;i<array->Count();i++)
		{
		CApaMaskedBitmap* iconBySize=CApaMaskedBitmap::NewLC();
		ret=aLs.GetAppIcon(KUidTestApp,(*array)[i],*iconBySize); //by Size
		if(ret==KErrNoMemory)
			User::Leave(KErrNoMemory);
		TEST(ret==KErrNone);
		TEST(iconBySize->SizeInPixels()==(*array)[i]);
		CleanupStack::PopAndDestroy(); //iconBySize
		}

	// now try non exact sizes
	CApaMaskedBitmap* iconBySize=CApaMaskedBitmap::NewLC();
	ret=aLs.GetAppIcon(KUidTestApp,((*array)[0])+TSize(-5,-5),*iconBySize); //by Size
	if(ret==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST(ret==KErrNotFound);
	CleanupStack::PopAndDestroy(); //iconBySize

	for(TInt ii=0;ii<array->Count();ii++)
		{
		CApaMaskedBitmap* iconBySize=CApaMaskedBitmap::NewLC();
		ret=aLs.GetAppIcon(KUidTestApp,((*array)[ii])+TSize(5,5),*iconBySize); //by Size
		if(ret==KErrNoMemory)
			User::Leave(KErrNoMemory);
		TEST(ret==KErrNone);
		TEST(iconBySize->SizeInPixels()==(*array)[ii]); // should get size below always!!!
		CleanupStack::PopAndDestroy(); //iconBySize
		}
	CleanupStack::PopAndDestroy(); //array
	}


/**
   @SYMTestCaseID T-Serv2Step-AppInfoTest3
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test RApaLsSession::GetNextApp() 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call RApaLsSession::GetAllApps() to initialize the process of
   getting all applications in the cached list. Traverse the application list
   by calling RApaLsSession::GetNextApp() to search for application tstapp.
   Test the application information obtained from GetNextApp() to ensure that
   information retrieved is of tstapp.\n
   API Calls:\n	
   RApaLsSession::GetAllApps() const\n
   RApaLsSession::GetNextApp(TApaAppInfo& aInfo) const\n
   
   @SYMTestExpectedResults The test compares the results obtained with expected values.
    
 */
void CT_Serv2Step::AppInfoTest3(RApaLsSession& aLs)
	{
	_LIT(KLitAppPath,"z:\\sys\\bin\\tstapp.exe");
	TFullName appPath(KLitAppPath);
	// get the first app...
	TApaAppInfo info;
	TInt ret = aLs.GetAllApps();
	TEST(ret==KErrNone);

	do
		{
		aLs.GetNextApp(info);
		}
		while (info.iUid!=KUidTestApp);
		
	TEST(ret==KErrNone);
	TEST(info.iUid==KUidTestApp);
	info.iFullName.LowerCase();	
	TEST(info.iFullName.Compare(appPath) == 0);
	}


/**
   @SYMTestCaseID T-Serv2Step-AppInfoTest4
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test RApaLsSession::GetAppInfo(). 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call RApaLsSession::GetAppInfo() by passing the Uid of test
   application tstapp and test the application info obtained from GetAppInfo()
   to ensure the obtained information is of test application tstapp.\n
   API Calls:\n	
   RApaLsSession::GetAppInfo(TApaAppInfo& aInfo,TUid aAppUid) const\n
   
   @SYMTestExpectedResults The test confirms that RApaLsSession::GetAppInfo()
   returns the expected application info.
    
 */
void CT_Serv2Step::AppInfoTest4(RApaLsSession& aLs)
	{
	_LIT(KLitAppPath,"z:\\sys\\bin\\tstapp.exe");
	TFullName appPath(KLitAppPath);
	// Get info for an app that exists
	TApaAppInfo info;
	TInt ret = aLs.GetAppInfo(info,KUidTestApp);
	TEST(ret==KErrNone);
	TEST(info.iUid==KUidTestApp);
	info.iFullName.LowerCase();	
	TEST(info.iFullName.Compare(appPath) == 0);
	}
	

/**
   @SYMTestCaseID T-Serv2Step-AppInfoTest5
 
   @SYMPREQ
  
   @SYMTestCaseDesc Test if RApaLsSession::GetAppInfo() return KErrNotFound. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The method calls RApaLsSession::GetAppInfo() by passing the null Uid
   KNullUid. Observe whether KErrNotFound is returned when Uid that doesn't match
   is passed to GetAppInfo().\n
   API Calls:\n	
   RApaLsSession::GetAppInfo(TApaAppInfo& aInfo,TUid aAppUid) const\n
   
   @SYMTestExpectedResults The test confirms RApaLsSession::GetAppInfo() returns
   KErrNotFound when no match is found for the Uid passed.\n
    
 */
void CT_Serv2Step::AppInfoTest5(RApaLsSession& aLs)
	{
	// get info for a non-existant app
	TApaAppInfo info;
	TInt ret = aLs.GetAppInfo(info,KNullUid);
	TEST(ret==KErrNotFound);
	}
	

/**
  Auxiliary Fn for Test Case ID T-Serv2Step-AppInfoTest1, T-Serv2Step-AppInfoTest3, T-Serv2Step-AppInfoTest4, T-Serv2Step-AppInfoTest5
 
  This function initiates test for retrieving Application Information.
 
*/
void CT_Serv2Step::DoAppInfoTests(RApaLsSession& aLs)
	{
	AppInfoTest1(aLs);
	AppInfoTest3(aLs);
	AppInfoTest4(aLs);
	AppInfoTest5(aLs);
	}


/**
   @SYMTestCaseID T-Serv2Step-StartAppTests1L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test Launching of an application. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Prepare command line information to start an application using
   CApaCommandLine Apis.Call RApaLsSession::StartApp() to start an
   application defined by the command line information.\n
   Test the launching of application for following scenarios:\n
   (1) When Application specified by command line exists.\n
   (2) When Application specified by command line does not exist.\n
   API Calls:\n	
   RApaLsSession::StartApp(const CApaCommandLine& aCommandLine)\n
   
   @SYMTestExpectedResults The test observes that StartApp() returns KErrNone and
   starts the app in the first scenario. In the second case KErrNotFound is
   returned.
    
 */	
void CT_Serv2Step::StartAppTests1L(RApaLsSession& aLs)
{
	CApaCommandLine* cmdLn=CApaCommandLine::NewLC();
	TFileName filename;
	_LIT(KLitExePath,"\\sys\\bin\\texe.exe");
	TFullName exePath(KLitExePath);
	filename = SearchAndReturnCompleteFileName(exePath);
	
	_LIT8(KLitDogfish,"dogfish");
	_LIT(KLitWibble,"wibble");
	cmdLn->SetExecutableNameL(filename);
	cmdLn->SetTailEndL(KLitDogfish);
	TInt ret = aLs.StartApp(*cmdLn); // explicit
	TEST(ret==KErrNone);

	// start a non-existant app
	cmdLn->SetExecutableNameL(KLitWibble);
	ret = aLs.StartApp(*cmdLn);
	TEST(ret==KErrNotFound);
	//
	//
	CleanupStack::PopAndDestroy(); // cmdLn

}

/**
 * @SYMTestCaseID T_Serv2Step_StartAppTests4L
 *
 * @SYMPREQ
 *
 * @SYMTestCaseDesc Test Launching of an application. 
 * 
 * @SYMTestActions Call RApaLsSession::StartDocument() to start an
 * application defined by the command line information.\n
 * Test the launching of application for following scenarios:\n
 * (1) When Application specified by command line is EmbeddableOnly.\n
 * (2) When Application specified by command line is EmbeddableUiNotStandAlone.\n
 * (3) When Application specified by command line is Embeddable or StandAlone.\n
 * API Calls:\n	
 * RApaLsSession::StartDocument(const TDesC &aFileName, TUid aAppUid, TThreadId &aId, TLaunchType aLaunchType=ELaunchNewApp)\n
 * 
 * @SYMTestExpectedResults The test observes that StartDocument() returns KErrNone and
 * starts the app in the third scenario. In the first and second case KErrNotSupported is
 * returned.
 *  
 */	
 
void CT_Serv2Step::StartAppTests4L(RApaLsSession& aLs)
	{
	TInt ret;
	TThreadId id;

	_LIT(KLitSdkDocPath,"\\system\\data\\sdk.txt");
	TFileName filename = SearchAndReturnCompleteFileName(KLitSdkDocPath);
	
	// Test starting a file using an app which can be embeddable only.
	ret=aLs.StartDocument(filename,KUidAppEmbeddableUiNotStandAlone,id);
	TEST(ret==KErrNotSupported);
	
	// Test starting a file using an app which can be embeddable only.
	ret=aLs.StartDocument(filename,KUidAppEmbeddableOnly,id);
	TEST(ret==KErrNotSupported);

	// Test starting a file using an app which can be embeddable or standalone.
	ret=aLs.StartDocument(filename,KUidAppEmbeddable,id);
	TEST(ret==KErrNone);
	}

/**
* Auxiliary Fn for Test Case ID T_Serv2Step_StartAppTests1L, T_Serv2Step_StartAppTests3L,StartAppTests4L(aLs)
*
* This method tests launching of an application from command line by calling
* CT_Serv2Step::StartAppTests1L().
*
*/
void CT_Serv2Step::DoStartAppTestsL(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Checking application launching"));

	// start the test exe
	StartAppTests1L(aLs);
    StartAppTests4L(aLs);
	}

/**
   @SYMTestCaseID T-Serv2Step-DoEnquiryTestsL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests querying of data (MIME) type information. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Test setting and getting the confidence threshold for
   successful data recognition by calling RApaLsSession::SetAcceptedConfidence()
   and RApaLsSession::GetAcceptedConfidence().\n
   Test setting and getting maximum size of the data that can be read
   from a file for the purpose of recognizing the data type by calling
   RApaLsSession::SetMaxDataBufSize() and RApaLsSession::GetMaxDataBufSize().
   Finally test retrieval of all supported data (MIME) types.\n
   API Calls:\n	
   RApaLsSession::SetAcceptedConfidence(TInt aConfidence)\n
   RApaLsSession::GetAcceptedConfidence(TInt& aConfidence) const\n
   RApaLsSession::SetMaxDataBufSize(TInt aBufSize)\n
   RApaLsSession::GetMaxDataBufSize(TInt& aBufSize) const\n
   RApaLsSession::GetSupportedDataTypesL(CDataTypeArray& aDataTypes) const\n
   
   @SYMTestExpectedResults Test results against expected values.
    
 */
void CT_Serv2Step::DoEnquiryTestsL(RApaLsSession& aLs)
	{
	TInt temp=1234;
	TInt ret;
	TSecurityInfo info;
	info.Set(RProcess());
	
	TBool hasCapability=info.iCaps.HasCapability(ECapabilityWriteDeviceData);
	
	if(hasCapability)
		{
		ret=aLs.SetAcceptedConfidence(temp);
		INFO_PRINTF2(_L(" Error= %d \n"),ret);
		TEST(ret==KErrNone);
		}
	else
		{
		ret=aLs.SetAcceptedConfidence(temp);
		INFO_PRINTF2(_L(" Error= %d \n"),ret);
		TEST(ret==KErrPermissionDenied);
		}
	temp=0;
	ret=aLs.GetAcceptedConfidence(temp);
	TEST(ret==KErrNone);
	TEST(temp==1234);
	ret=aLs.SetMaxDataBufSize(temp);
	TEST(ret==KErrNone);
	temp=0;
	ret=aLs.GetMaxDataBufSize(temp);
	CDataTypeArray* dataTypes=new(ELeave) CDataTypeArray(5);
	CleanupStack::PushL(dataTypes);
	TInt err=0;
	TInt failRate=0;
	for (failRate=1;failRate<=KMaxTInt;failRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EFailNext,failRate);
		TRAP(err, ret=aLs.GetSupportedDataTypesL(*dataTypes));
		if (err!=KErrNoMemory)
			{
		 	if (err==KErrNone)
		 		{
		 		err=ret;
		 		}
		 	break;
		 	}
		}
	__UHEAP_RESET;
	TEST(ret==KErrNone);
	TEST(err==KErrNone);
	TEST(dataTypes->Count()>=4);
	CleanupStack::PopAndDestroy(); // dataTypes
	}

/**
   @SYMTestCaseID T-Serv2-DoGetAllApps
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Check that RApaLsSession::GetAllApps() hasn't been
   affected by the new code to filter the app list
   based on embeddability (implemented for CR PHAR-5Q4FX8)
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetAllApps() once, followed by repeated calls
   to GetNextApp until something other than KErrNone is returned
  
   @SYMTestExpectedResults
   GetNextApp should return app info for the following test apps:
   TAppNotEmbeddable.app
   TAppEmbeddable.app
   TAppEmbeddableOnly.app
   TAppEmbeddableUiOrStandAlone.app
   TAppEmbeddableUiNotStandAlone.app
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_Serv2Step::DoGetAllApps(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoGetAllApps"));

	TInt ret = aLs.GetAllApps();
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = aLs.GetNextApp(appInfo)) == KErrNone)
		{
		if (appInfo.iUid == KUidAppNotEmbeddable)
			bitFlags |= 0x01;
		else if (appInfo.iUid == KUidAppEmbeddable)
			bitFlags |= 0x02;
		else if (appInfo.iUid == KUidAppEmbeddableOnly)
			bitFlags |= 0x04;
		else if (appInfo.iUid == KUidAppEmbeddableUiOrStandAlone)
			bitFlags |= 0x08;
		else if (appInfo.iUid == KUidAppEmbeddableUiNotStandAlone)
			bitFlags |= 0x10;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	TEST(bitFlags == 0x1F);
	}
	
	
/* 
   @SYMTestCaseID T-Serv2-LaunchAppThruCreateDocL
  
   @SYMTestCaseDesc
   Check for the functionality of  RApaLsSession::CreateDocument(). 
   
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions 
   Call CreateDocument(const TDesC& aFileName, TUid aAppUid, TThreadId& aThreadId)
   
   @SYMTestExpectedResults
   Verify whether the Application is launched and Document is created for the respective 
   Application UID passed.  
*/

void CT_Serv2Step::LaunchAppThruCreateDocL(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("LaunchAppThruCreateDocL"));
	_LIT(KFileName, "c:\\nofile.txt");
	TThreadId startAppThreadID;
	TInt errCreate=aLs.CreateDocument(KFileName,KUidAppEmbeddable,startAppThreadID);
	TEST(errCreate == KErrNone);
	
	//we need to close the started thread, if appropiate
	if (errCreate==KErrNone)	
		{
		RThread thread;
		User::LeaveIfError(thread.Open(startAppThreadID));
		CleanupClosePushL(thread);

		RProcess process;
		User::LeaveIfError(thread.Process(process));
		CleanupClosePushL(process);

		process.Kill(0);
		
		CleanupStack::PopAndDestroy(&process);
		CleanupStack::PopAndDestroy(&thread);
		}
		
	}
	
/**
   @SYMTestCaseID T-Serv2-DoGetEmbeddableApps
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Check that RApaLsSession::GetEmbeddableApps() hasn't been
   affected by the new code to filter the app list
   based on embeddability (implemented for CR PHAR-5Q4FX8)
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetEmbeddableApps() once, followed by repeated calls
   to GetNextApp until something other than KErrNone is returned
  
   @SYMTestExpectedResults
   GetNextApp should return app info for the following test apps:
   TAppEmbeddable.app
   TAppEmbeddableOnly.app
   and should not return app info for:
   TAppNotEmbeddable.app
   TAppEmbeddableUiOrStandAlone.app
   TAppEmbeddableUiNotStandAlone.app
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_Serv2Step::DoGetEmbeddableApps(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoGetEmbeddableApps"));

	TInt ret = aLs.GetEmbeddableApps();
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = aLs.GetNextApp(appInfo)) == KErrNone)
		{
		if (appInfo.iUid == KUidAppNotEmbeddable)
			bitFlags |= 0x01;
		else if (appInfo.iUid == KUidAppEmbeddable)
			bitFlags |= 0x02;
		else if (appInfo.iUid == KUidAppEmbeddableOnly)
			bitFlags |= 0x04;
		else if (appInfo.iUid == KUidAppEmbeddableUiOrStandAlone)
			bitFlags |= 0x08;
		else if (appInfo.iUid == KUidAppEmbeddableUiNotStandAlone)
			bitFlags |= 0x10;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	TEST(bitFlags == 0x06);
	}

/**
   @SYMTestCaseID T-Serv2-DoEmbeddableAppCount
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   The number of apps returned by GetEmbeddableApps should be the
   same as EmbeddableAppCount returns.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Count the number of apps returned by calling GetEmbeddableApps
   followed by repeated calls to GetNextApp.
   Count the number of apps returned by EmbeddableAppCount.
  
   @SYMTestExpectedResults
   The counts should be equal.
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_Serv2Step::DoEmbeddableAppCount(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoEmbeddableAppCount"));

	TInt ret = aLs.GetEmbeddableApps();
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TInt count1 = 0;
	while ((ret = aLs.GetNextApp(appInfo)) == KErrNone)
		{
		count1 ++;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);

	TInt count2 = 0;
	ret = aLs.EmbeddableAppCount(count2);
	TEST(ret==KErrNone);

	TEST(count1 == count2);
	}

/**
   @SYMTestCaseID T-Serv2-DoGetAppCapability
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Confirm that the application embeddability value that a specific app
   defines in it's AIF file, matches the value returned by
   RApaLsSession::GetAppCapability.
   Check that this is true for all supported embeddability values
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetAppCapability for each of the following test apps:
   TAppNotEmbeddable.app
   TAppEmbeddable.app
   TAppEmbeddableOnly.app
   TAppEmbeddableUiOrStandAlone.app
   TAppEmbeddableUiNotStandAlone.app
  
   @SYMTestExpectedResults
   For each app, GetAppCapability should return the embeddability value
   defined in the application's AIF file.
  
 */
void CT_Serv2Step::DoGetAppCapability(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoGetAppCapability"));

	TApaAppCapabilityBuf capabilityBuf;
	TInt ret;
	const TInt numberOfCapabilities = 5;
	TUid uidArray[numberOfCapabilities];
	uidArray[0] = KUidAppNotEmbeddable;
	uidArray[1] = KUidAppEmbeddable;
	uidArray[2] = KUidAppEmbeddableOnly;
	uidArray[3] = KUidAppEmbeddableUiOrStandAlone;
	uidArray[4] = KUidAppEmbeddableUiNotStandAlone;

	const TApaAppCapability::TEmbeddability embeddabilityArray[numberOfCapabilities]
		= { TApaAppCapability::ENotEmbeddable,
			TApaAppCapability::EEmbeddable, 
			TApaAppCapability::EEmbeddableOnly, 
			TApaAppCapability::EEmbeddableUiOrStandAlone, 
			TApaAppCapability::EEmbeddableUiNotStandAlone };

	for (TInt ii = 0; ii < numberOfCapabilities; ii++)
		{
		ret = aLs.GetAppCapability(capabilityBuf, uidArray[ii]);
		TEST(ret==KErrNone);
		TEST(capabilityBuf().iEmbeddability == embeddabilityArray[ii]);
		}
	}

/**
   @SYMTestCaseID T-Serv2-DoGetFilteredApps1
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Passing an empty filter to RApaLsSession::GetFilteredApps
   should cause no apps to be returned by subsequent calls
   to RApaLsSession::GetNextApp
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetFilteredApps passing a default constructed filter,
   followed by a single call to GetNextApp
  
   @SYMTestExpectedResults
   GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_Serv2Step::DoGetFilteredApps1(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoGetFilteredApps1"));

	TApaEmbeddabilityFilter filter;
	TInt ret = aLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TEST(aLs.GetNextApp(appInfo) == RApaLsSession::ENoMoreAppsInList);
	}

/**
   @SYMTestCaseID T-Serv2-DoGetFilteredApps2
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Passing a filter specifying only ENotEmbeddable
   to RApaLsSession::GetFilteredApps should cause subsequent
   calls to RApaLsSession::GetNextApp to return only
   applications which cannot be embedded
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetFilteredApps passing a filter containing ENotEmbeddable,
   followed by repeated calls to GetNextApp until something other than
   KErrNone is returned
  
   @SYMTestExpectedResults
   GetNextApp should return app info for only the following test app:
   TAppNotEmbeddable.app
   and should not return app info for:
   TAppEmbeddable.app
   TAppEmbeddableOnly.app
   TAppEmbeddableUiOrStandAlone.app
   TAppEmbeddableUiNotStandAlone.app
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_Serv2Step::DoGetFilteredApps2(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoGetFilteredApps2"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::ENotEmbeddable);
	TInt ret = aLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = aLs.GetNextApp(appInfo)) == KErrNone)
		{
		if (appInfo.iUid == KUidAppNotEmbeddable)
			bitFlags |= 0x01;
		else if (appInfo.iUid == KUidAppEmbeddable)
			bitFlags |= 0x02;
		else if (appInfo.iUid == KUidAppEmbeddableOnly)
			bitFlags |= 0x04;
		else if (appInfo.iUid == KUidAppEmbeddableUiOrStandAlone)
			bitFlags |= 0x08;
		else if (appInfo.iUid == KUidAppEmbeddableUiNotStandAlone)
			bitFlags |= 0x10;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	TEST(bitFlags == 0x01);
	}

/**
   @SYMTestCaseID T-Serv2-DoGetFilteredApps3
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   EEmbeddableUiOrStandAlone (value 5) shares a bit flag (bit 1)
   with EEmbeddable (value 1).
   Confirm that a filter of EEmbeddableUiOrStandAlone only returns
   apps that define EEmbeddableUiOrStandAlone
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetFilteredApps passing a filter containing EEmbeddableUiOrStandAlone,
   followed by repeated calls to GetNextApp until something other than
   KErrNone is returned
  
   @SYMTestExpectedResults
   GetNextApp should return app info for only the following test app:
   TAppEmbeddableUiOrStandAlone.app
   and should not return app info for:
   TAppEmbeddable.app
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_Serv2Step::DoGetFilteredApps3(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoGetFilteredApps3"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone);
	TInt ret = aLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = aLs.GetNextApp(appInfo)) == KErrNone)
		{
		if (appInfo.iUid == KUidAppEmbeddable)
			bitFlags |= 0x02;
		else if (appInfo.iUid == KUidAppEmbeddableUiOrStandAlone)
			bitFlags |= 0x08;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	TEST(bitFlags == 0x08);
	}

/**
   @SYMTestCaseID T-Serv2-DoGetFilteredApps4
 
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   A filter specifying two embeddability values should
   include apps of either embeddability
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetFilteredApps passing a filter containing EEmbeddableUiOrStandAlone
   and EEmbeddableUiNotStandAlone followed by repeated calls to GetNextApp
   until something other than KErrNone is returned
  
   @SYMTestExpectedResults
   GetNextApp should return app info for only the following test apps:
   TAppEmbeddableUiOrStandAlone.app
   TAppEmbeddableUiNotStandAlone.app
   and should not return app info for:
   TAppNotEmbeddable.app
   TAppEmbeddable.app
   TAppEmbeddableOnly.app
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_Serv2Step::DoGetFilteredApps4(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoGetFilteredApps4"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone);
	TInt ret = aLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = aLs.GetNextApp(appInfo)) == KErrNone)
		{
		if (appInfo.iUid == KUidAppNotEmbeddable)
			bitFlags |= 0x01;
		else if (appInfo.iUid == KUidAppEmbeddable)
			bitFlags |= 0x02;
		else if (appInfo.iUid == KUidAppEmbeddableOnly)
			bitFlags |= 0x04;
		else if (appInfo.iUid == KUidAppEmbeddableUiOrStandAlone)
			bitFlags |= 0x08;
		else if (appInfo.iUid == KUidAppEmbeddableUiNotStandAlone)
			bitFlags |= 0x10;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	TEST(bitFlags == 0x18);
	}

/**
   @SYMTestCaseID T-Serv2-DoGetFilteredApps5
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Getting apps using a filter specifying all embeddability values
   should return the same number of apps as RApaLsSession::GetAllApps()
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetFilteredApps passing a filter containing all embeddability values,
   and count the number of calls to GetNextApp until something other than
   KErrNone is returned.
   Then call GetAllApps and count the number of calls to GetNextApp until
   something other than KErrNone is returned.
  
   @SYMTestExpectedResults
   The two counts should be equal.
   The last call in each sequence of calls to GetNextApp should return
   RApaLsSession::ENoMoreAppsInList
  
 */
void CT_Serv2Step::DoGetFilteredApps5(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoGetFilteredApps5"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::ENotEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableOnly);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone);
	TInt ret = aLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TInt filteredCount = 0;
	while ((ret = aLs.GetNextApp(appInfo)) == KErrNone)
		{
		filteredCount++;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);

	ret = aLs.GetAllApps();
	TEST(ret==KErrNone);

	TInt allAppsCount = 0;
	while ((ret = aLs.GetNextApp(appInfo)) == KErrNone)
		{
		allAppsCount++;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);

	TEST(filteredCount == allAppsCount);
	}

/**
   @SYMTestCaseID T-Serv2-DoGetFilteredApps6
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   None of the following embeddability test apps support
   screen mode 1, so none of them should be returned even
   if the filter specifies all embeddability types.
   TAppNotEmbeddable.app
   TAppEmbeddable.app
   TAppEmbeddableOnly.app
   TAppEmbeddableUiOrStandAlone.app
   TAppEmbeddableUiNotStandAlone.app
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetFilteredApps passing a filter containing all embeddability values
   and screen mode 1, followed by repeated calls to GetNextApp until something
   other than KErrNone is returned
  
   @SYMTestExpectedResults
   GetNextApp should not return app info for any of the following test apps:
   TAppNotEmbeddable.app
   TAppEmbeddable.app
   TAppEmbeddableOnly.app
   TAppEmbeddableUiOrStandAlone.app
   TAppEmbeddableUiNotStandAlone.app
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_Serv2Step::DoGetFilteredApps6(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("DoGetFilteredApps6"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::ENotEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableOnly);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone);
	const TInt screenMode1 = 1;
	TInt ret = aLs.GetFilteredApps(filter, screenMode1);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = aLs.GetNextApp(appInfo)) == KErrNone)
		{
		if (appInfo.iUid == KUidAppNotEmbeddable)
			bitFlags |= 0x01;
		else if (appInfo.iUid == KUidAppEmbeddable)
			bitFlags |= 0x02;
		else if (appInfo.iUid == KUidAppEmbeddableOnly)
			bitFlags |= 0x04;
		else if (appInfo.iUid == KUidAppEmbeddableUiOrStandAlone)
			bitFlags |= 0x08;
		else if (appInfo.iUid == KUidAppEmbeddableUiNotStandAlone)
			bitFlags |= 0x10;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	TEST(bitFlags == 0);
	}

/**
   @SYMTestCaseID T-Serv2-DoTestEmbeddabilityFilter1
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Ensure default constructed TApaEmbeddabilityFilter object is
   initialized correctly.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Construct a TApaEmbeddabilityFilter object and call
   MatchesEmbeddability using each of the following embeddability values:
   TApaAppCapability::ENotEmbeddable
   TApaAppCapability::EEmbeddable
   TApaAppCapability::EEmbeddableOnly
   TApaAppCapability::EEmbeddableUiOrStandAlone
   TApaAppCapability::EEmbeddableUiNotStandAlone
  
   @SYMTestExpectedResults
   All calls to MatchesEmbeddability should return false.
  
 */
void CT_Serv2Step::DoTestEmbeddabilityFilter1()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter1"));

	TApaEmbeddabilityFilter filter;
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::ENotEmbeddable));
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddable));
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableOnly));
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone));
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone));
	}

/**
   @SYMTestCaseID T-Serv2-DoTestEmbeddabilityFilter2
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Bit 2 is used by both EEmbeddableUiNotStandAlone and EEmbeddableOnly.
   Make sure that setting the filter to only EEmbeddableUiNotStandAlone
   doesn't match EEmbeddableOnly.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Construct a TApaEmbeddabilityFilter object,
   set the filter to only EEmbeddableUiNotStandAloneand
   and call MatchesEmbeddability passing EEmbeddableOnly
  
   @SYMTestExpectedResults
   MatchesEmbeddability should return false.
  
 */
void CT_Serv2Step::DoTestEmbeddabilityFilter2()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter2"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone);
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableOnly));
	}

/**
   @SYMTestCaseID T-Serv2-DoTestEmbeddabilityFilter3
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Bit 2 is used by both EEmbeddableUiNotStandAlone and EEmbeddableOnly.
   Make sure that setting the filter to only EEmbeddableOnly
   doesn't match EEmbeddableUiNotStandAlone.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Construct a TApaEmbeddabilityFilter object,
   set the filter to only EEmbeddableOnly
   and call MatchesEmbeddability passing EEmbeddableUiNotStandAlone
  
   @SYMTestExpectedResults
   MatchesEmbeddability should return false.
  
 */
void CT_Serv2Step::DoTestEmbeddabilityFilter3()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter3"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableOnly);
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone));
	}

/**
   @SYMTestCaseID T-Serv2-DoTestEmbeddabilityFilter4
  
   @SYMPREQ PHAR-5Q4FX8
 
   @SYMTestCaseDesc
   Ensure MatchesEmbeddability returns true when the filter only contains
   the embeddability being matched.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions
   Construct a TApaEmbeddabilityFilter object,
   set the filter to only EEmbeddable
   and call MatchesEmbeddability passing EEmbeddable
  
   @SYMTestExpectedResults
   MatchesEmbeddability should return true.
  
 */
void CT_Serv2Step::DoTestEmbeddabilityFilter4()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter4"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	TEST(filter.MatchesEmbeddability(TApaAppCapability::EEmbeddable));
	}

/**
   @SYMTestCaseID T-Serv2-DoTestEmbeddabilityFilter5
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   Ensure MatchesEmbeddability returns true when the filter contains
   a superset of the embeddability being matched.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Construct a TApaEmbeddabilityFilter object,
   set the filter to both ENotEmbeddable and EEmbeddable
   and call MatchesEmbeddability passing EEmbeddable
  
   @SYMTestExpectedResults
   MatchesEmbeddability should return true.
  
 */
void CT_Serv2Step::DoTestEmbeddabilityFilter5()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter5"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::ENotEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	TEST(filter.MatchesEmbeddability(TApaAppCapability::EEmbeddable));
	}


/**
   @SYMTestCaseID T-Serv2Step-DoInterrogationTestsL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test interrogates the Application Architecture Server. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Test Application Architecture Server's ability to:\n
   (1) Retrieve list of all applications in the cache list by calling 
   RApaLsSession::GetAllApps().The test also calls RApaLsSession::AppCount()
   to retrieve the number of applications in the list.\n
   (2) Retrieve list of all embeddable applications from the cache list
   by calling RApaLsSession::GetEmbeddableApps().The test also calls
   RApaLsSession::EmbeddableAppCount() to retrieve the number of
   embeddable applications in the list.\n
   (3) Retrieve application information.\n
   (4) Retrieve capability of an existing & non-existing application by
   calling RApaLsSession::GetAppCapability().\n
   (5) Check and retrieve Data (MIME) type information of data taken from a
   specified file by calling RApaLsSession::RecognizeSpecificData().
   This is confirmed by taking data from the same file and checking
   the data type information by calling RApaLsSession::RecognizeData().\n
   (6) Retrieve application icon by calling RApaLsSession::GetAppIcon().\n
   API Calls:\n	
   RApaLsSession::GetAllApps() const\n
   RApaLsSession::AppCount(TInt& aCount) const\n
   RApaLsSession::EmbeddableAppCount(TInt& aCount) const\n
   RApaLsSession::GetEmbeddableApps() const\n
   RApaLsSession::GetAppCapability(TDes8& aCapabilityBuf,TUid aAppUid) const\n
   RApaLsSession::RecognizeSpecificData(const TDesC& aName, const TDesC8& aBuffer, const TDataType& aDataType, TBool& aResult) const\n
   RApaLsSession::RecognizeData(const TDesC& aName, const TDesC8& aBuffer, TDataRecognitionResult& aDataType) const\n
   RApaLsSession::GetAppIcon(TUid aAppUid, TInt aSize, CApaMaskedBitmap& aAppBitmap) const\n
   
   @SYMTestExpectedResults All tests compare the results obtained with the desired result.
    
 */
void CT_Serv2Step::DoInterrogationTestsL(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Interogating the server"));

	TInt ret = aLs.GetAllApps(); // SC - test
	TEST(ret==KErrNone);

	// get the full app count
	TInt count=0;
	ret = aLs.AppCount(count);
	TEST(ret==KErrNone);
	TEST(count>=1);

	// get the embedded app count
	count=0;
	ret = aLs.EmbeddableAppCount(count);
	TEST(ret==KErrNone);
	TEST(count>=1);

	// prepare to get a list of embeddable apps
	ret = aLs.GetEmbeddableApps();
	TEST(ret==KErrNone);
	DoAppInfoTests(aLs);

	// Get the capability of an app that exists
	TApaAppCapabilityBuf buf;
	ret = aLs.GetAppCapability(buf, KUidTestApp);
	TEST(ret==KErrNone);
	TApaAppCapability cap=buf();
	TEST(cap.iEmbeddability==TApaAppCapability::EEmbeddable);
	TEST(!cap.iSupportsNewFile);
	TEST(!cap.iAppIsHidden);

	// get the capability of a non-existant app
	ret = aLs.GetAppCapability(buf, KNullUid);
	TEST(ret==KErrNotFound);

	INFO_PRINTF1(_L("Checking data type information"));
	TBool isText;
	ret=aLs.RecognizeSpecificData(_L("a file name.txt"),_L8("Some plain text"),TDataType(_L8("text/plain")),isText);
	TEST(ret==KErrNone);
	TEST(isText);
	TDataRecognitionResult result;
	ret=aLs.RecognizeData(_L("a file name.txt"),_L8("Some plain text"),result);
	TEST(ret==KErrNone);
	TEST(result.iDataType==TDataType(_L8("text/plain")));
	TEST(result.iConfidence==CApaDataRecognizerType::EProbable);
	isText=EFalse;
	ret=aLs.RecognizeSpecificData(_L("a file name.txt"),_L8("Some plain text"),TDataType(_L8("foo/bar")),isText);
	TEST(!isText);

	INFO_PRINTF1(_L("Checking icon loading by index"));
	CApaMaskedBitmap* icon=CApaMaskedBitmap::NewLC();

	TSize lastSize(0,0);
	ret=aLs.GetAppIcon(KUidTestApp,0,*icon);
	TEST(ret==KErrNone);
	TEST(icon->SizeInPixels().iWidth*icon->SizeInPixels().iHeight > lastSize.iWidth*lastSize.iHeight);
	lastSize=icon->SizeInPixels();
		
	ret=aLs.GetAppIcon(KUidTestApp,1,*icon);
	TEST(ret==KErrNone);

	TEST(icon->SizeInPixels().iWidth*icon->SizeInPixels().iHeight == lastSize.iWidth*lastSize.iHeight);
	lastSize=icon->SizeInPixels();

	ret=aLs.GetAppIcon(KUidTestApp,2,*icon);
	TEST(ret==KErrNone);
	TEST((icon->SizeInPixels().iWidth*icon->SizeInPixels().iHeight) > (lastSize.iWidth*lastSize.iHeight));
	lastSize=icon->SizeInPixels();


	TUid uid={9999};
	ret=aLs.GetAppIcon(uid,0,*icon);
	TEST(ret==KErrNotFound);
	CleanupStack::PopAndDestroy(icon);
	}


/**
   @SYMTestCaseID T-Serv2Step-DoInstallationTestL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test updating of Application list on installation of a new application. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Test updating of application list on installation of a new
   application. Copy the tstapp from z: drive to c: drive. Call RApaLsSession::GetAppInfo()
   and test the application information obtained to verify whether the
   application architecture server updates the application to reside in c: drive.
   Delete the copied application and call RApaLsSession::GetAppInfo().
   Verify that the application list updates the tstapp application to reside
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppInfo(TApaAppInfo& aInfo,TUid aAppUid) const\n
   
   @SYMTestExpectedResults The test confirms that the application architecture
   server updates the application list on installation of a new application.
    
 */
void CT_Serv2Step::DoInstallationTestL (RApaLsSession ls)
 	{
 	_LIT(KTestAppDestDir, "C:\\private\\10003a3f\\import\\apps\\" );
 	_LIT(KTestAppSource, "Z:\\private\\10003a3f\\import\\apps\\tstapp_reg.rsc" );
 	_LIT(KTestAppDest, "C:\\private\\10003a3f\\import\\apps\\tstapp_reg.rsc" );

 	_LIT(KTestWaitingForApplistUpdate,"\nWaiting %d microseconds for applist to be updated");
 	const TInt KApplistUpdateTime = 8000000;

 	// Copy App files around and delete them to check whether 
	// the app list updates and stores the cache correctly.
 	RFs	theFs;
 	theFs.Connect();
 
 	// Remove Test app from the file system
 	CFileMan* fileManager = CFileMan::NewL (theFs);
 
 	INFO_PRINTF1(_L("Copying the app to C"));
 	TEST(KErrNone == fileManager->Copy (KTestAppSource, KTestAppDest, CFileMan::ERecurse));
 	
 	INFO_PRINTF2(KTestWaitingForApplistUpdate, KApplistUpdateTime);
 	User::After(KApplistUpdateTime);
 
 	TApaAppInfo aInfo;
 	TEST(KErrNone == ls.GetAppInfo (aInfo, KUidTestApp));

	TParsePtrC parse (aInfo.iFullName);
 	_LIT (KCdrive, "C:");
	INFO_PRINTF1(_L("Comparing App drive location is C:... "));
	TEST(parse.Drive ().CompareF (KCdrive) == 0);
 
 	INFO_PRINTF1(_L("Removing the app from C"));
	TTime tempTime(0);
	fileManager->Attribs(KTestAppDest,0,KEntryAttReadOnly, tempTime, CFileMan::ERecurse);
 	TEST(KErrNone == fileManager->Delete (KTestAppDest, CFileMan::ERecurse));
	INFO_PRINTF1(_L("Removing the app dir from C"));
	TEST(fileManager->RmDir(KTestAppDestDir)==KErrNone);
 	
	INFO_PRINTF2(KTestWaitingForApplistUpdate, KApplistUpdateTime);
	User::After(KApplistUpdateTime);
 
 	// That should put the file in the right place
 	TEST(KErrNone == ls.GetAppInfo( aInfo, KUidTestApp));

 	TParsePtrC parse1 (aInfo.iFullName);
 	_LIT (KZdrive, "Z:");
	INFO_PRINTF1(_L("Comparing App drive location is Z:... "));
 	TEST((parse1.Drive().CompareF(KZdrive)) == 0);
 	
 	delete fileManager;
 	theFs.Close();
 }

//
// Tests the new RApaLsSession::NumberOfOwnDefinedIcons function
// See TeamTrack 'DEF037571 - Provide a better alternative to CApaAppInfoFileReader::NumberOfBitmaps'
//
void CT_Serv2Step::DoNumDefIconsTestL(RApaLsSession& aSession)
	{
	INFO_PRINTF1(_L("Testing RApaLsSession::NumberOfOwnDefinedIcons"));
	TInt err = KErrNone;

	CArrayFixFlat<TSize>* array = new(ELeave) CArrayFixFlat<TSize>(4);
	CleanupStack::PushL(array);

	// Test the GetAppIconSizes function, check 3 icons are returned (default icons)
	INFO_PRINTF1(_L("Testing GetAppIconSizes"));
	TUid appUid;
	appUid.iUid = 0x12008ACE;	// SimpleApparcTestApp UID
	err = aSession.GetAppIconSizes(appUid, *array);
	TEST(err == KErrNone);
	TEST(array->Count() == 3);

	// Test the NumberOfOwnDefinedIcons function, check count is zero (no own defined icons)
	INFO_PRINTF1(_L("Testing NumberOfOwnDefinedIcons (SimpleApparcTestApp)"));
	TInt iconCount = -1;
	err = aSession.NumberOfOwnDefinedIcons(appUid, iconCount);
	TEST(err == KErrNone);
	TEST(iconCount == 0);

	// Test NumberOfOwnDefinedIcons again, this time with Contacts app UID (icons defined)
	INFO_PRINTF1(_L("Testing NumberOfOwnDefinedIcons (Contacts app)"));
	iconCount = -1;
	appUid.iUid = 0x10003a74;	// Contacts app UID

	err = aSession.NumberOfOwnDefinedIcons(appUid, iconCount);
	TEST(err == KErrNone);
	TEST(iconCount == 3);

	INFO_PRINTF1(_L("NumberOfOwnDefinedIcons test complete"));

	CleanupStack::PopAndDestroy(array);
	}

/** Test RecognizeData() */	
void CT_Serv2Step::DoRecognizeDataL(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Testing RApaLsSession::RecognizeData"));
	
	_LIT(KExistingFile, "Z:\\ApparcTest\\apparctest_t_Serv2.script");
	_LIT(KNonExistingFile, "x:\\non-existing directory\\non-existing file.any");
	_LIT(KWeirdFileName, "\\c:\\  : weird...txt");
	_LIT8(KFileContents, "just some random noice, we don't really care about the result");
	_LIT8(KTextPlainMimeType, "text/plain");
	
	RFs fs;
	TEST(KErrNone == fs.Connect());
	CleanupClosePushL(fs);
	
	// make sure KExistingFile really exists and that KNonExistingFile doesn't
	TTime modificationTime;
	TEST(KErrNone == fs.Modified(KExistingFile, modificationTime));
	TEST(KErrNone != fs.Modified(KNonExistingFile, modificationTime));

	// test with existing file (result will be cached)
	TDataRecognitionResult recData;
	TEST(KErrNone == aLs.RecognizeData(KExistingFile, KFileContents, recData));
	TEST(0 ==        recData.iDataType.Des8().Compare(KTextPlainMimeType));
	
	// test to get result from the cache (not possible to know if we succeed though)
	TEST(KErrNone == aLs.RecognizeData(KExistingFile, KFileContents, recData));
	TEST(0 ==        recData.iDataType.Des8().Compare(KTextPlainMimeType));
	
	//test with non-existing file (result won't be cached)	
	TEST(KErrNone == aLs.RecognizeData(KNonExistingFile, KFileContents, recData));
	TEST(0 ==        recData.iDataType.Des8().Compare(KTextPlainMimeType));
	
	//test with empty file name (result won't be cached)
	TEST(KErrNone == aLs.RecognizeData(KNullDesC, KFileContents, recData));
	TEST(0 ==        recData.iDataType.Des8().Compare(KTextPlainMimeType));

	//test with weird file name (result won't be cached)
	TEST(KErrNone == aLs.RecognizeData(KWeirdFileName, KFileContents, recData));
	TEST(0 ==        recData.iDataType.Des8().Compare(KTextPlainMimeType));
	
	CleanupStack::PopAndDestroy(&fs);
	}

/**
   @SYMTestCaseID APPFWK-APPARC-0074
  
   @SYMDEF DEF099084: Apparc server's MIME type recognition cache can become out of date   
  
   @SYMTestCaseDesc Test checks whether the MIME type recognition cache maintained by apparc server is
					flushed when it receives the ECom change notification., i.e., either when a new ecom plugin is installed 
					or an existing ecom plugin is uninstalled.
   
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Calls RecognizeData() to test the MIME type returned, 
   when
   1. When there is no existing "mime/mmr1" recognizer installed.
   2. "mime/mmr1" recognizer with EPossible confidence is installed.
   3. "mime/mmr1" recognizer is uninstalled.
   It tests whether the mime type recognition cache maintained by the apparc server gets flushed
   when any new ecom plugin is installed or when an existing ecom plugin is uninstalled.
   
   @SYMTestExpectedResults RecognizeData() returns CApaDataRecognizerType::ENotRecognized when "mime/mmr1" recognizer is not
   installed.  RecognizeData() returns CApaDataRecognizerType::EPossible when "mime/mmr1" recognizer with EPossible confidence is installed.
   RecognizeData() returns CApaDataRecognizerType::ENotRecognized when "mime/mmr1" recognizer with EPossible confidence is uninstalled.
 */
 
void CT_Serv2Step::TestRecognitionCacheL(RApaLsSession& aLs)
	{
	INFO_PRINTF1(_L("Testing fix for DEF099084: Apparc server's MIME type recognition cache can become out of date "));
	INFO_PRINTF1(_L("Testing MIME type recognition cache ..."));

	_LIT(KLitDocPath, "Z:\\system\\data\\TApsRecogAppTest.mmr");
	_LIT8(KMimeMmrType,"mime/mmr1");

	TDataRecognitionResult recogResult;
	TBuf8<255> buffer;
	buffer.FillZ(255);
	recogResult.Reset();
		
	// Since recognizer for "mime\mmr1" mime type is not yet installed, call to RecognizeData()
	// returns CApaDataRecognizerType::ENotRecognized.
	INFO_PRINTF1(_L("MIME type recognition cache is not used..."));
	User::LeaveIfError(aLs.RecognizeData(KLitDocPath, buffer, recogResult));
	TEST(recogResult.iConfidence == CApaDataRecognizerType::ENotRecognized);

	INFO_PRINTF1(_L("Installs new mime/mmr1 recognizer plugin file "));
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	// Ensure c:\\resource\\plugins is created, otherwise leave from this teststep
	_LIT(KRecogRscTargetDir,"C:\\resource\\plugins\\");
	TInt err = fs.CreateDirectoryL(KRecogRscTargetDir);
	TESTEL((err == KErrNone || err == KErrAlreadyExists), err);
	
	// Ensure c:\\sys\\bin is created, otherwise leave from this teststep
	_LIT(KDllRecogTargetDir,"C:\\sys\\bin\\");
	err = fs.CreateDirectoryL(KDllRecogTargetDir);
	TESTEL((err == KErrNone || err == KErrAlreadyExists), err);

	// Copy recmime_1.rsc from z:\\ to c:\\resource\\plugins, to trigger the rescan for plugins.
	_LIT(KRecogRscSourcePath, "Z:\\system\\data\\recmime_1.rsc");
	_LIT(KRecogRscTargetPath,"C:\\resource\\plugins\\recmime_1.rsc");

	err = fs.CopyFileL(KRecogRscSourcePath, KRecogRscTargetPath);
	TEST(err == KErrNone);

	// Copy recmime_*.dll from z:\\ to c:\\sys\\bin.
	_LIT(KRecogDllTargetPath, "C:\\sys\\bin\\recmime_1.dll");

	#ifdef __WINS__ 
		_LIT(KRecogDllSourcePathWinscw, "Z:\\system\\data\\recmime_winscw.dll");
		err = fs.CopyFileL(KRecogDllSourcePathWinscw, KRecogDllTargetPath);
	#else
		_LIT(KRecogDllSourcePathArmv5, "Z:\\system\\data\\recmime_armv5.dll");
		err = fs.CopyFileL(KRecogDllSourcePathArmv5, KRecogDllTargetPath);
	#endif
	TEST(err == KErrNone);

	INFO_PRINTF2(_L("Waits %d seconds for new mime/mmr1 recognizer with EPossible confidence to be installed."), (4 * KOneSecondDelay)/1000000);	
	User::After(4 * KOneSecondDelay);
	
	// Without fix for DEF099084: Apparc server's MIME type recognition cache can become out of date 
	// RApaLsSession::RecognizeData() would return CApaDataRecognizerType::ENotRecognized (ie., Mime Recognition cache
	// would not have been flushed when new recognizer is installed).
	INFO_PRINTF1(_L("MIME type recognition cache is flushed when a new recogizer is installed"));
	recogResult.Reset();
	User::LeaveIfError(aLs.RecognizeData(KLitDocPath, buffer, recogResult));
	TEST(recogResult.iDataType.Des8().CompareF(KMimeMmrType) == 0);
	TEST(recogResult.iConfidence == CApaDataRecognizerType::EPossible);
		
	// Second call to RecognizeData() utilizes the Mime type Recognition cache.
	INFO_PRINTF1(_L("MIME type recognition cache is used..."));
	recogResult.Reset();
	User::LeaveIfError(aLs.RecognizeData(KLitDocPath, buffer, recogResult));
	TEST(recogResult.iDataType.Des8().CompareF(KMimeMmrType) == 0);
	TEST(recogResult.iConfidence == CApaDataRecognizerType::EPossible);
	
	// Delete the plugin resource file of the recognizer with EPossible confidence which was installed during the test from z:.
	TInt fileExists = EFalse;
	fs.IsFilePresent(KRecogRscTargetPath, fileExists);
	if (fileExists)
		{
		fs.ChangeFilePermissionL(KRecogRscTargetPath); // clear readonly attribute inherited from z:
		err = fs.DeleteFileL(KRecogRscTargetPath);
		if(err == KErrNone) 
			INFO_PRINTF1(_L("Removed temporary file recmime_1.rsc"));
		else 
			INFO_PRINTF1(_L("Failed to remove temporary file recmime_1.rsc"));	
		}

	INFO_PRINTF2(_L("Waits %d seconds for the data recognizer list maintained by EMIME to be updated"), (10 * KOneSecondDelay)/1000000);	
 	User::After(10 * KOneSecondDelay);
	
	// Since "mime/mmr1" recognizer is uninstalled, mime/mmr1 mime type is not recognized. 
	INFO_PRINTF1(_L("MIME type recognition cache is flushed when a existing recogizer is uninstalled"));
	recogResult.Reset();
	User::LeaveIfError(aLs.RecognizeData(KLitDocPath, buffer, recogResult));
	TEST(recogResult.iConfidence == CApaDataRecognizerType::ENotRecognized);
	
	CleanupStack::PopAndDestroy(&fs);
	}

/**
  Auxiliary Fn for the entire Test Step
 
  This method initiates all the tests.
*/
TInt CT_Serv2Step::DoServComTestL()
	{
	INFO_PRINTF1(_L("Testing Server Com"));
	
	
	RFs theServFs;
	TInt ret = theServFs.Connect();
	TEST(ret==KErrNone);
	CleanupClosePushL(theServFs);

	// Connect to the server
    RTestableApaLsSession ls;
	ret = ls.ConnectWithoutWaitingForListPopulation();
	TEST(ret==KErrNone);
	CleanupClosePushL(ls);
	
	
	INFO_PRINTF1(_L("Waiting applist to be generated.\n"));
	TRequestStatus requeststatus;
	ls.RegisterListPopulationCompleteObserver(requeststatus);
	
	//Testing the CancelListPopulationCompleteObserver
	TEST(ls.CancelListPopulationCompleteObserver() == KErrNone);
	User::WaitForRequest(requeststatus); 

	ls.RegisterListPopulationCompleteObserver(requeststatus);
	User::WaitForRequest(requeststatus);
	TEST(requeststatus == KErrNone);

	//make sure the RFbsSession doesn't have to allocate buffer since it would change the heap
	CFbsBitmap* bufferAllocator = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bufferAllocator);
	TEST(bufferAllocator->Create(TSize(200,1), EColor16M) == KErrNone);
	CleanupStack::PopAndDestroy(bufferAllocator);
	
	//
	HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, DoInterrogationTestsL(ls), ls.ClearAppInfoArray(); ls.FlushRecognitionCache(); );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoGetAllApps(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoGetEmbeddableApps(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoEmbeddableAppCount(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoGetAppCapability(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoGetFilteredApps1(ls), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoGetFilteredApps2(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoGetFilteredApps3(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoGetFilteredApps4(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoGetFilteredApps5(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoGetFilteredApps6(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoTestEmbeddabilityFilter1(), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoTestEmbeddabilityFilter2(), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoTestEmbeddabilityFilter3(), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoTestEmbeddabilityFilter4(), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoTestEmbeddabilityFilter5(), ls.ClearAppInfoArray() );
	//DONT_CHECK due to file system changes
	HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, TestRecognitionCacheL(ls), ls.FlushRecognitionCache());
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoRecognizeDataL(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, LaunchAppThruCreateDocL(ls), ls.FlushRecognitionCache() );


	INFO_PRINTF1(_L("Checking icon loading by size - including OOM tests\n"));
	ret = KErrNoMemory;
	for(TInt fail=0; ret == KErrNoMemory; fail++)
		{
		__UHEAP_FAILNEXT(fail);
		__UHEAP_MARK;
		TRAP(ret, DoCheckServerIconLoadingBySizeL(ls));
		__UHEAP_SETFAIL(RHeap::ENone,0);// turn failure off
		__UHEAP_MARKEND;
		TEST((ret==KErrNoMemory || ret==KErrNone));
		}

	//DONT_CHECK due to app is started and that app opens up another RApaLsSession (which changes the server's heap)
	HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, DoStartAppTestsL(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoEnquiryTestsL(ls), NO_CLEANUP);
	//DONT_CHECK due to changes to file system
	HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, DoInstallationTestL(ls), ls.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, DoNumDefIconsTestL(ls), ls.ClearAppInfoArray() );
	
	//
	// close the server session and we have done some type store reloading
	CleanupStack::PopAndDestroy(2, &theServFs);

	return KErrNone;
	}

CT_Serv2Step::~CT_Serv2Step()
/**
   Destructor
 */
	{
	}

CT_Serv2Step::CT_Serv2Step()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_Serv2Step);
	}

TVerdict CT_Serv2Step::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_Serv2Step::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}

TVerdict CT_Serv2Step::doTestStepL()
/**
   Override of base class virtual.
   @return - TVerdict code
 */
{
	FbsStartup();
	TInt ret = RFbsSession::Connect();
	TEST(ret==KErrNone);

	// set up the directory structure
	RFs theFs;
	ret = theFs.Connect();
	TEST(ret==KErrNone);
	CleanupClosePushL(theFs);
	
	// run the testcode (inside an alloc heaven harness)
	INFO_PRINTF1(_L("About to test server in process"));
	TRAP(ret,DoServComTestL());
	INFO_PRINTF2(_L("  DoServComTestL completes with %d"), ret);
	TEST(ret==KErrNone);

	// Cleanup
	CleanupStack::PopAndDestroy(&theFs);

	INFO_PRINTF1(_L("Test Completed!"));	
	return TestStepResult();

}
