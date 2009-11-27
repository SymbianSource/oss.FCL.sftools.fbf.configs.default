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
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#include <barsread.h>
#include "T_ServicesStep.h"

_LIT(KCompleted, "Completed.");
_LIT8(KLitPlainText,"text/plain");
_LIT8(KLitPriorityText,"text/priority");
_LIT8(KLitCustom1Text, "text/custom1");

/**
   Constructor
 */	
CT_ServicesTestStep::CT_ServicesTestStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_ServicesStep);
	}

/**
   Destructor
 */
CT_ServicesTestStep::~CT_ServicesTestStep()
	{
	iApaLsSession.Close();
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */	
TVerdict CT_ServicesTestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	TInt error = iApaLsSession.Connect();
	TEST(error==KErrNone);
	return TestStepResult();
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CT_ServicesTestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}


TVerdict CT_ServicesTestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparc...T_Services Test Cases Running..."));

	TInt ret;
	TRAP(ret,RunTestCasesL())
		TEST(ret==KErrNone);
	
	INFO_PRINTF1(_L("...T_Services Test Cases Completed."));

	return TestStepResult();
	}

TInt CT_ServicesTestStep::RunTestCasesL()
	{
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery1L(), iApaLsSession.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery2L(), iApaLsSession.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery3L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery4L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery5L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery6(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery7L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery8L(), NO_CLEANUP);
	// The following two APIs InsertDataMappingL() & DeleteDataMappingL(), update the type store on the server side.
	// This update takes place on the server side while the test case is still running, which causes the heap check to fail.
	// To avoid the heap check on the server side, DONT_CHECK macro is used.
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, DONT_CHECK, TestServiceDiscovery9(), NO_CLEANUP );
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery10L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery11L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery12L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery13L(), NO_CLEANUP);

/**	
	#ifndef __SECURE_BIN__
	__UHEAP_MARK;
	TRAP(r,TestServiceDiscovery14L());
	TEST(r==KErrNone);
	__UHEAP_MARKEND;
	#endif
	

	#ifndef __SECURE_BIN__
	__UHEAP_MARK;
	TRAP(r,TestServiceDiscovery15L());
	TEST(r==KErrNone);
	__UHEAP_MARKEND;
	#endif
	
	#ifndef __SECURE_BIN__
	__UHEAP_MARK;
	TRAP(r,TestServiceDiscovery16L());
	TEST(r==KErrNone);
	__UHEAP_MARKEND;
	#endif
*/
	
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery17L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery18L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery19(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery20L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery21L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery22(), iApaLsSession.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery23(), iApaLsSession.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery24(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery25(), iApaLsSession.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery27(), iApaLsSession.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestServiceDiscovery28L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestOpenService1L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, 0, TestCtrlPnlAppL(), NO_CLEANUP);
	// The following two APIs InsertDataMappingL() & DeleteDataMappingL(), update the type store on the server side.
	// This update takes place on the server side while the test case is still running, which causes the heap check to fail.
	// To avoid the heap check on the server side, DONT_CHECK macro is used.
	HEAP_TEST_LS_SESSION(iApaLsSession, 0, DONT_CHECK, TestPrioritySystemL(), NO_CLEANUP );
	return (KErrNone);
	}
	
/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery1L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetServerApps with a service uid that is not provided by any application  
   
   @SYMTestPriority 
   
   @SYMTestStatus Implemented
  
   @SYMTestActions Call GetServerApps with the service uid as parameter.
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetServerApps(TUid aServiceUid) const
   
   @SYMTestExpectedResults Returns  ENoMoreAppsInList  since no application provides a service represented by the uid
    
 */
void CT_ServicesTestStep::TestServiceDiscovery1L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery1 about to start..."));
	const TUid KUidService = {0x01020300}; // service Uid
	TInt ret = iApaLsSession.GetServerApps(KUidService);//initialize the list of apps offering service
	TEST(ret==KErrNone);// test if initalized properly
	TApaAppInfo appInfo;
	// populate the list with all the apps and return the first apps
	ret = iApaLsSession.GetNextApp(appInfo);
	TEST(ret == RApaLsSession::ENoMoreAppsInList);// since no apps implements this service, ENoMoreAppsInList is returned
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery2L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetServerApps with uid that represents a service provided by more that one application 
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions GetServerApps with a particular service uid as parameter, 
   making sure that this service uid is used in at least one application 
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetServerApps(TUid aServiceUid) const
   
   @SYMTestExpectedResults Returns a list of all applications that offers this service
    
 */
void CT_ServicesTestStep::TestServiceDiscovery2L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery2 about to start..."));	
	const TUid KUidService1234 = {0x01020304};//uid of service
	const TUid KUidServerApp1 = {0x10004c56}; //uid of one server app
	const TUid KUidServerApp2 = {0x10004c57};//uid of second server app
	TInt ret = iApaLsSession.GetServerApps(KUidService1234);
	
	TEST(ret==KErrNone);
	TApaAppInfo appInfo;
	ret = iApaLsSession.GetNextApp(appInfo);
	TEST(ret==KErrNone);
	TEST((appInfo.iUid==KUidServerApp2) || (appInfo.iUid==KUidServerApp1));
	ret = iApaLsSession.GetNextApp(appInfo);
	TEST((appInfo.iUid==KUidServerApp2) || (appInfo.iUid==KUidServerApp1));
	ret = iApaLsSession.GetNextApp(appInfo);
	TEST(ret == RApaLsSession::ENoMoreAppsInList);//only one app implements the service, ENoMoreAppsInList is returned
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery3L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetAppServicesLC to get all the services provided by an application 
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetAppServicesLC with the app uid as parameter
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServicesLC(TUid aAppUid) const
   
   @SYMTestExpectedResults Returns an array of TApaAppServiceInfo objects. Each TApaAppServiceInfo contains details about a service uid and corresponding opaque data. 
   The returned data should be the same as that defined in the registration files.
   In the case where the app provides the same services but with different opaque data, it can be checked that the opaque data is different for each service.
   In the case where a service does not have any defined opaque data, it can be checked that the length of the returned descriptor for the opaque data is zero.
    
 */
void CT_ServicesTestStep::TestServiceDiscovery3L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery3 about to start..."));
	const TUid KUidService1234 = {0x01020304};
	const TUid KUidService2345 = {0x02030405};
	const TUid KUidServerApp = {0x10004c56};
	const TUid KUidService5678 = {0x05060708};
	const TUid KUidService = {0x01020333};	

	// get all services for a particular app
	CApaAppServiceInfoArray* array = iApaLsSession.GetAppServicesLC(KUidServerApp);
	TArray<TApaAppServiceInfo> serviceArray(array->Array());
	TInt count = serviceArray.Count();
	TEST(count == 6); // app provides 5 services + 1 legacy open; just to make sure that app provides more than one service

	TPtrC8 opaqueData;
	TResourceReader reader;
	TPtrC16 theText;
	TInt val(0);
	
	_LIT(KFirstService,"Non-localised text for service uid 0x01020304");
	_LIT(KSecondService,"Non-localised text for service uid 0x02030405");
	_LIT(KThirdService,"Non-localised resource A");
	_LIT(KFourthService,"Non-localised resource B");
	
	TEST(serviceArray[0].Uid()==KOpenServiceUid);
	const CArrayFixFlat<TDataTypeWithPriority>& datatypes = serviceArray[0].DataTypes();
	TEST(datatypes.Count()==2);
	TEST(0 == datatypes[0].iDataType.Des8().CompareF(KLitPlainText));
	TEST(0 == datatypes[1].iDataType.Des8().CompareF(KLitPriorityText));
	
	
	TEST(serviceArray[1].Uid()==KUidService1234);
	opaqueData.Set(serviceArray[1].OpaqueData());
	reader.SetBuffer(&opaqueData); // opaqueData is an LTEXT resource
	theText.Set(reader.ReadTPtrC16());
	User::LeaveIfError(val = theText.Compare(KFirstService));
	TEST(val==KErrNone);
	TEST(serviceArray[1].DataTypes().Count()==0);
	
	TEST(serviceArray[2].Uid()==KUidService2345);
	opaqueData.Set(serviceArray[2].OpaqueData());
	reader.SetBuffer(&opaqueData);
	theText.Set(reader.ReadTPtrC16());
	User::LeaveIfError(val=theText.Compare(KSecondService));
	TEST(val==KErrNone);
	const CArrayFixFlat<TDataTypeWithPriority>& datatypes2 = 
		serviceArray[2].DataTypes();
	TEST(datatypes2.Count()==1);
	TEST(0 == datatypes2[0].iDataType.Des8().CompareF(KLitCustom1Text));
	
	TEST(serviceArray[3].Uid()==KUidService5678);
	opaqueData.Set(serviceArray[3].OpaqueData());
	reader.SetBuffer(&opaqueData); // opaqueData is an LTEXT resource
	theText.Set(reader.ReadTPtrC16());
	User::LeaveIfError(val = theText.Compare(KThirdService));
	TEST(val==KErrNone);
	TEST(serviceArray[3].DataTypes().Count()==0);
	
	TEST(serviceArray[4].Uid()==KUidService5678);
	opaqueData.Set(serviceArray[4].OpaqueData());
	reader.SetBuffer(&opaqueData);
	theText.Set(reader.ReadTPtrC16());
	User::LeaveIfError(val=theText.Compare(KFourthService));
	TEST(val==KErrNone);
	TEST(serviceArray[4].DataTypes().Count()==0);
	
	TUid myuid = serviceArray[5].Uid();
	TEST(myuid==KUidService);
	opaqueData.Set(serviceArray[5].OpaqueData());
	TEST(opaqueData.Length()==0);
	TEST(serviceArray[5].DataTypes().Count()==0);
	
	CleanupStack::PopAndDestroy(array); // the servicearray
	array = NULL;
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery4L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetServiceImplementationsLC 
   which gets all the implementation details about a specific service 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetServiceImplementationsLC with the service uid as parameter
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetServiceImplementationsLC(TUid aServiceUid) const
   
   @SYMTestExpectedResults Returns an array of TApaAppServiceInfo objects. 
   The size of the array is equal to the number of apps offering this service. 
   Each TApaAppServiceInfo contain an app uid and  the respective opaquedata. 
   The returned data should be the same as that defined in the registration files.
    
 */
void CT_ServicesTestStep::TestServiceDiscovery4L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery4 about to start..."));
	const TUid KUidService1234 = {0x01020304};
	const TUid KUidServerApp1 = {0x10004c56};
	const TUid KUidServerApp2 = {0x10004c57};
	CApaAppServiceInfoArray* array = iApaLsSession.GetServiceImplementationsLC(KUidService1234);
	TArray<TApaAppServiceInfo> implArray(array->Array());
	TInt count = implArray.Count();	
	TEST(count==2);
	_LIT(KService,"Non-localised text for service uid 0x01020304");
	TPtrC8 opaqueData;
	TResourceReader reader;
	TPtrC16 theText;
	TInt val(0);

	TUid myuid = implArray[0].Uid(); 
	// The services can appear in any order
	if (myuid==KUidServerApp1)
		{
		TEST(myuid==KUidServerApp1);
		myuid = implArray[1].Uid();
		TEST(myuid==KUidServerApp2);
		const CArrayFixFlat<TDataTypeWithPriority>& datatypes = implArray[1].DataTypes();
		TEST(datatypes.Count()==1);
		TEST(0 == datatypes[0].iDataType.Des8().CompareF(KLitCustom1Text));
		}
	else if (myuid==KUidServerApp2)
		{
		TEST(myuid==KUidServerApp2);
		myuid = implArray[1].Uid();
		TEST(myuid==KUidServerApp1);
		const CArrayFixFlat<TDataTypeWithPriority>& datatypes = implArray[0].DataTypes();
		TEST(datatypes.Count()==1);
		TEST(0 == datatypes[0].iDataType.Des8().CompareF(KLitCustom1Text));
		}

	opaqueData.Set(implArray[0].OpaqueData());
	reader.SetBuffer(&opaqueData); // opaqueData is an LTEXT resource
	theText.Set(reader.ReadTPtrC16());
	User::LeaveIfError(val = theText.Compare(KService));
	TEST(val==KErrNone);

	opaqueData.Set(implArray[1].OpaqueData());
	reader.SetBuffer(&opaqueData); // opaqueData is an LTEXT resource
	theText.Set(reader.ReadTPtrC16());
	User::LeaveIfError(val = theText.Compare(KService));
	TEST(val==KErrNone);

	CleanupStack::PopAndDestroy(array);
	array = NULL;
	}
	
/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery5L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetAppServicesL which gets all the service uids of a particular app 
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetAppServicesL with two parameters: the app uid and an array to put in the uid of the services 
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServicesL(TUid aAppUid, CArrayFixFlat<TUid>& aServiceUids) const
   
   @SYMTestExpectedResults Populates the array with the uids of the services offered by this particular app. 
   The returned data should be the same as that defined in the registration files. 
    
 */
void CT_ServicesTestStep::TestServiceDiscovery5L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery5 about to start..."));
	const TUid KUidServerApp = {0x10004c56};
	const TUid KUidService1234 = {0x01020304};
	const TUid KUidService2345 = {0x02030405};
	const TUid KUidService5678 = {0x05060708};
	const TUid KUidService = {0x01020333};
	// get UIDs of all services implemented by an app
	CArrayFixFlat<TUid>* uidArray = new(ELeave) CArrayFixFlat<TUid>(4);
	CleanupStack::PushL(uidArray);
	iApaLsSession.GetAppServicesL(KUidServerApp,*uidArray);
	TInt count = uidArray->Count();
	TUid uid;
	if(count>0)
		{
		TEST(count==6);	// Takes into account the open service
		uid = (*uidArray)[0];
		TEST(uid==KUidService);
		uid = (*uidArray)[1];
		TEST(uid==KUidService5678);
		uid = (*uidArray)[2];
		TEST(uid==KUidService5678);
		uid = (*uidArray)[3];
		TEST(uid==KUidService2345);
		uid = (*uidArray)[4];
		TEST(uid==KUidService1234);
		}
	CleanupStack::PopAndDestroy(uidArray);
	uidArray = NULL;
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery6
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test for the functionality AppForDataTypeAndService in the case where two apps 
   provides have the same datatype definition in their registration file. One app offers the requested 
   service but has its priority assigned to normal while the other app does not offer the requested 
   service and has its priority assigned to high 
  
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call AppForDataTypeAndService with three parameters: the datatype, the service uid 
   and an unitialised app uid
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::AppForDataTypeAndService(const TDataType& aDataType, TUid aServiceUid, TUid& aAppUid) const
   
   @SYMTestExpectedResults Sets the app uid to that of the app with lower priority since the one with higher 
   priority does not offer this service
    
 */
void CT_ServicesTestStep::TestServiceDiscovery6()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery6 about to start..."));
	const TUid KUidServerAppHigh = {0x10004c57};
	const TUid KUidService = {0x02030405};
	const TUid KUidServerAppNormal = {0x10004c56};	
	_LIT8(KLitMimeTypeServerAppData,"text/priority");
	TUid appUid;
	TInt ret(0);
	ret=iApaLsSession.AppForDataTypeAndService(TDataType(KLitMimeTypeServerAppData), KUidService,appUid);
	TEST(ret==KErrNone);
	TEST(appUid != KUidServerAppHigh);
	TEST(appUid == KUidServerAppNormal);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery7L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Given the size of data that is returned from the server is greater than the default 
   1024 bytes of the buffer, test if a larger buffer is assigned in such cases
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetAppServicesLC with the app uid as parameter. The size of the resource data in 
   the registration file should be greater than the default size.
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServicesLC(TUid aAppUid) const
   
   @SYMTestExpectedResults A larger buffer is allocated. The size of the buffer can be tested by checking 
   the size of the returned data.
    
 */
void CT_ServicesTestStep::TestServiceDiscovery7L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery7 about to start..."));
	const TUid KUidServerApp = {0x10004c57};
	// get all services for a particular app
	CApaAppServiceInfoArray* array = iApaLsSession.GetAppServicesLC(KUidServerApp);
	TArray<TApaAppServiceInfo> serviceArray(array->Array());
	TInt count = serviceArray.Count();
	TEST(count == 16);
	TPtrC8 opaqueData;
	TInt size(0);
	for(TInt i=0;i<count;i++)
		{
			opaqueData.Set(serviceArray[i].OpaqueData());
			size +=opaqueData.Length(); 
		}
	TEST(size>=1024);
	CleanupStack::PopAndDestroy(array); // the servicearray	
	array = NULL;
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery8L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test for the functionality of GetAppServiceOpaqueDataLC, particularly in the case 
   where an app offers two services with the same uid but different opaque data
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetAppServiceOpaqueDataLC with two parameters: an app uid and a service uid
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServiceOpaqueDataLC(TUid aAppUid, TUid aServiceUid) const
   
   @SYMTestExpectedResults Returns an array of TApaAppServiceInfo objects. Each TApaAppServiceInfo object 
   contains the same service uid but different opaque data. The returned data should be the same as that 
   defined in the registration files.
    
 */
void CT_ServicesTestStep::TestServiceDiscovery8L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery8 about to start..."));
	const TUid KUidServerApp = {0x10004c56};
	const TUid KUidService = {0x05060708};	
	CApaAppServiceInfoArray* array = iApaLsSession.GetAppServiceOpaqueDataLC(KUidServerApp, KUidService);
	TArray<TApaAppServiceInfo> implArray(array->Array());
	TInt count = implArray.Count();
	_LIT(KFirstService,"Non-localised resource A");
	_LIT(KSecondService,"Non-localised resource B");
	TPtrC8 opaqueData;
	TResourceReader reader;
	TPtrC16 theText;
		if(count>0)
		{
		TEST(count==2);
		
		TEST(implArray[0].Uid()==KUidService);
		opaqueData.Set(implArray[0].OpaqueData());
		reader.SetBuffer(&opaqueData); // opaqueData is an LTEXT resource
		theText.Set(reader.ReadTPtrC16());
		TInt val(0);
		User::LeaveIfError(val=theText.Compare(KSecondService));
		TEST(val==KErrNone);
		TEST(implArray[1].Uid()==KUidService);
		opaqueData.Set(implArray[1].OpaqueData());
		reader.SetBuffer(&opaqueData); // opaqueData is an LTEXT resource
		theText.Set(reader.ReadTPtrC16());
		User::LeaveIfError(val=theText.Compare(KFirstService));
		TEST(val==KErrNone);	
		}
	
	CleanupStack::PopAndDestroy(array);
	array = NULL;
	}
	
/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery9
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of AppForDataTypeAndService in the cases where 
   (a) the datatype is not defined in the registration file 
   (b) the data type is defined in the registration file
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions (a)Call InsertDataMapping to make a mapping between the datatype and app. The parameters 
   used are: the datatype object, the priority assigned and the app uid Call AppForDataTypeAndService with 
   the three parameters: the data type, the service uid and an unitialised uid for the returning value. 
   Call DeleteDataMapping with the datatype object as parameter to delete this mapping
   (b) Call AppForDataTypeAndService with the three parameters: the data type, the service uid and an 
   unitialised uid for the returning value.
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::AppForDataTypeAndService(const TDataType& aDataType, TUid aServiceUid, TUid& aAppUid) const
   
   @SYMTestExpectedResults (a) Returns the app uid of the app that has this datatype and service requested. 
   It can be checked that the app uid corresponds to the app to which the mapping has been inserted.
   (b) Returns the uid of the app that has this datatype and service uid defined in its registration file.
    
 */
void CT_ServicesTestStep::TestServiceDiscovery9()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery9 about to start..."));
	const TUid KUidService1234 = {0x01020304};
	const TUid KUidServerApp = {0x10004c56};

	TInt ret(0);
	// Start AppForDataTypeAndService test cases
	// make sure our data mapping is removed before executing the following test cases
	_LIT8(KLitMimeTypeServerApp,"serverapp/data");
	_LIT8(KLitMimeTypeServerAppData,"text/plain");
	ret = iApaLsSession.InsertDataMapping(TDataType(KLitMimeTypeServerApp),
	 	1, KUidServerApp, KUidService1234); // need to remove magic number
	TEST(ret==KErrNone);
	// Waits till the data type mappings are restored from the data store ini file
	iApaLsSession.WaitForTypeStoreUpdate();
	TUid appUid;
	ret=iApaLsSession.AppForDataTypeAndService(TDataType(KLitMimeTypeServerApp), KUidService1234,appUid);
	TEST(ret==KErrNone);
	// KUidServerApp2 handles KLitMimeTypeJpeg with a higher priority than KUidServerApp,
	// but doesn't implement service KUidService1234, so KUidServerApp should be returned
	TEST(appUid == KUidServerApp);
	ret = iApaLsSession.DeleteDataMapping(TDataType(KLitMimeTypeServerApp),KUidService1234);
	TEST(ret==KErrNone);
	// Waits till the data type mappings are restored from the data store ini file
	iApaLsSession.WaitForTypeStoreUpdate();
	ret=iApaLsSession.AppForDataTypeAndService(TDataType(KLitMimeTypeServerAppData), KUidService1234,appUid);
	TEST(ret==KErrNone);
	TEST(appUid == KUidServerApp);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery10L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetAppServicesLC on an app that does not provide any service
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetAppServicesLC with the app uid as parameter
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServicesLC(TUid aAppUid) const
   
   @SYMTestExpectedResults Returns KErrNotFound
    
 */
void CT_ServicesTestStep::TestServiceDiscovery10L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery10 about to start..."));
	const TUid KUidServerApp = {0x01023450};
	TRAPD(r, iApaLsSession.GetAppServicesLC(KUidServerApp));
	TEST(r==KErrNotFound);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery11L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetServiceImplementationsLC using a service uid that is 
   not defined in any registration file
   
   @SYMTestPriority high
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetServiceImplementationsLC using a service uid not defined in any registration file
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetServiceImplementationsLC(TUid aServiceUid) const
   
   @SYMTestExpectedResults Returns KErrNotFound
    
 */
void CT_ServicesTestStep::TestServiceDiscovery11L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery11 about to start..."));
	const TUid KUidService1230 = {0x01020300};
	TRAPD(r, iApaLsSession.GetServiceImplementationsLC(KUidService1230));
	TEST(r==KErrNotFound);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery12L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetAppServicesL for an app that does not exist
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetAppServicesL using an app uid that is not used by any app
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServicesL(TUid aAppUid, CArrayFixFlat<TUid>& aServiceUids) const
   
   @SYMTestExpectedResults Returns KErrNotFound
    
 */
void CT_ServicesTestStep::TestServiceDiscovery12L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery12 about to start..."));
	const TUid KUidServerApp = {0x01020300};
	CArrayFixFlat<TUid>* uidArray = new(ELeave) CArrayFixFlat<TUid>(4);
	CleanupStack::PushL(uidArray);
	TRAPD(r,iApaLsSession.GetAppServicesL(KUidServerApp,*uidArray));
	CleanupStack::PopAndDestroy(uidArray);
	TEST(r==KErrNotFound);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery13L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetAppServiceOpaqueDataLC on an app that does 
   not provide a particular service
   
   @SYMTestPriority 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetAppServicesL using an existing app uid and service uid that is not 
   defined in its registration file
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServiceOpaqueDataLC(TUid aAppUid, TUid aServiceUid) const
   
   @SYMTestExpectedResults Returns KErrNotFound
    
 */
 void CT_ServicesTestStep::TestServiceDiscovery13L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery13 about to start..."));	
	const TUid KUidServerApp = {0x01023456};
	const TUid KUidService = {0x05060708};	
	TRAPD(r,iApaLsSession.GetAppServiceOpaqueDataLC(KUidServerApp, KUidService));
	TEST(r==KErrNotFound);
	}





/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery17L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetAppServicesLC on a app that has a localised 
   resource defined in the registration file
  
   @SYMTestStatus Implemented

   @SYMTestPriority  High
   
   @SYMTestActions  Call GetAppServicesLC with the app uid as parameter
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServicesLC(TUid aAppUid) const
   
   @SYMTestExpectedResults Returns an array of TApaAppServiceInfo objects. 
   Each TApaAppServiceInfo contains details about a service uid and corresponding opaque data. 
   The returned data should be the same as that defined in the registration files.
    
 */
void CT_ServicesTestStep::TestServiceDiscovery17L() 
	{
	INFO_PRINTF1(_L("TestServiceDiscovery17 about to start..."));
	const TUid KUidService1235 = {0x01020305};
	const TUid KUidServerApp = {0x10004c55};
	CApaAppServiceInfoArray* array = iApaLsSession.GetAppServicesLC(KUidServerApp);
	TArray<TApaAppServiceInfo> serviceArray(array->Array());
	TInt count = serviceArray.Count();
	TEST(count == 1);
	TPtrC8 opaqueData;
	TResourceReader reader;
	TPtrC16 theText;
	TInt val(0);
	_LIT(KSecondService,"Localised text for service UID 0x01020305");
	TEST(serviceArray[0].Uid()==KUidService1235);
	opaqueData.Set(serviceArray[0].OpaqueData());
	reader.SetBuffer(&opaqueData);
	theText.Set(reader.ReadTPtrC16());
	User::LeaveIfError(val=theText.Compare(KSecondService));
	TEST(val==KErrNone);
	CleanupStack::PopAndDestroy(array); // the servicearray
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery18L
  
   @SYMPREQ 538
  
   @SYMTestCaseDesc Test the functionality of GetAppServicesLC on a app that has a localised resource
   defined but the link to the resource file not specified in the registration file
  
   @SYMTestStatus Implemented
   
   @SYMTestPriority  High	
   
   @SYMTestActions  Call GetAppServicesLC with the app uid as parameter
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServicesLC(TUid aAppUid) const
   
   @SYMTestExpectedResults Returns KErrNone
    
 */
void CT_ServicesTestStep::TestServiceDiscovery18L() 
	{
	INFO_PRINTF1(_L("TestServiceDiscovery18 about to start..."));	
	const TUid KUidServerApp = {0x10004c54};
	TRAPD(r, CleanupStack::PopAndDestroy(iApaLsSession.GetAppServicesLC(KUidServerApp)));
	TEST(r==KErrNone);
	}
	
/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery19
  
   @SYMPREQ 538
  
   @SYMTestPriority  High

   @SYMTestCaseDesc Test the functionality of AppForDataTypeAndService in the cases where
   (a) the datatype requested is undefined for a particular service uid
   (b) the service uid is not defined for any app
  
   @SYMTestStatus Implemented
   
   @SYMTestActions  Call AppForDataTypeAndService with the three parameters: the data type, 
   the service uid and an unitialised uid for the returning value.
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::AppForDataTypeAndService(const TDataType& aDataType, TUid aServiceUid, TUid& aAppUid) const
   
   @SYMTestExpectedResults In both cases, the app uid value is set to null
    
 */
void CT_ServicesTestStep::TestServiceDiscovery19()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery19 about to start..."));	
	_LIT8(KLitMimeTypeServerAppData,"text/rubbish");
	_LIT8(KLitMimeTypeServerApp,"serverapp/data");
	TUid appUid=KNullUid;
	TInt ret(0);
	const TUid KUidService = {0x01020304};
	const TUid KUidServiceUnused = {0x01020300};
	ret=iApaLsSession.AppForDataTypeAndService(TDataType(KLitMimeTypeServerAppData), KUidService,appUid);
	TEST(ret==KErrNone);
	TEST(appUid == KNullUid);
	ret=iApaLsSession.AppForDataTypeAndService(TDataType(KLitMimeTypeServerApp), KUidServiceUnused,appUid);
	TEST(ret==KErrNone);
	TEST(appUid == KNullUid);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery20L
  
   @SYMPREQ 538
  
   @SYMTestPriority  High

   @SYMTestCaseDesc Test the functionality of GetAppServicesLC to get all the services 
   provided by an application that does not exist
  
   @SYMTestStatus Implemented
   
   @SYMTestActions  Call GetAppServicesLC with the app uid as parameter
 
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServicesLC(TUid aAppUid) const
   
   @SYMTestExpectedResults Returns KErrNotFound
    
 */	
void CT_ServicesTestStep::TestServiceDiscovery20L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery20 about to start..."));
	const TUid KUidServerApp = {0x10001111};
	// get all services for a particular app
	TRAPD(r,iApaLsSession.GetAppServicesLC(KUidServerApp));
	TEST(r==KErrNotFound);			
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery21L
  
   @SYMPREQ 538
  
   @SYMTestPriority  High
  
   @SYMTestCaseDesc Test the functionality of GetAppServiceOpaqueDataLC on an app that does not exist
   
   @SYMTestStatus Implemented
   
   @SYMTestActions  Call GetAppServiceOpaqueDataLC with two parameters: an app uid and a service uid
 
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppServiceOpaqueDataLC(TUid aAppUid, TUid aServiceUid) const
   
   @SYMTestExpectedResults Returns KErrNotFound
    
 */	
void CT_ServicesTestStep::TestServiceDiscovery21L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery21L about to start..."));
	const TUid KUidServerApp = {0x10004c58};
	const TUid KUidService = {0x01020304};
	// get all services for a particular app
	TRAPD(r,iApaLsSession.GetAppServiceOpaqueDataLC(KUidServerApp,KUidService));
	TEST(r==KErrNotFound);	
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery22
  
   @SYMPREQ 538
  
   @SYMTestPriority  High
  
   @SYMTestStatus Implemented
   
   @SYMTestCaseDesc Test the functionality ofAppForDocumentAndService given an existing file name and service uid
   
   @SYMTestActions  Call AppForDocumentAndService with two parameters: a filename, a service uid, an
   app uid and the datatype requested
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::AppForDocumentAndService(const TDesC& aFileName, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType) const
   
   @SYMTestExpectedResults Returns KErrNone.It returns the first app that can offer this service and initialise the 
   datatype that describes the datatype in the document
    
 */	
void CT_ServicesTestStep::TestServiceDiscovery22()
	{
	// AppForDocumentAndService test cases
	INFO_PRINTF1(_L("TestServiceDiscovery22 about to start..."));
	TInt ret(0);
	TUid appUid=KNullUid;
	const TUid KUidService = {0x01020303};
	TDataType dataType;
	const TUid TUidExpected = {0x10004c54};
	_LIT8(KLitMimeTypeServerData,"text/plain");
    ret = iApaLsSession.AppForDocumentAndService(_L("z:\\system\\data\\one_byte.txt"), KUidService, appUid, dataType);	
	TEST(ret==KErrNone);
	TEST(dataType==TDataType(KLitMimeTypeServerData));
	TEST(appUid==TUidExpected);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery23
  
   @SYMPREQ 538
  
   @SYMTestPriority  High
  
   @SYMTestStatus Implemented

   @SYMTestCaseDesc Test the functionality ofAppForDocumentAndService given an existing file name and undefined service uid
   
   @SYMTestActions  Call AppForDocumentAndService with two parameters: a filename, a service uid, an
   app uid and the datatype requested
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::AppForDocumentAndService(const TDesC& aFileName, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType) const
   
   @SYMTestExpectedResults Returns KErrNone.The value returned for appuid is null.The datatype is set to the one that can desribe the
   datatype in the document 
    
 */	
void CT_ServicesTestStep::TestServiceDiscovery23()
	{
	// AppForDocumentAndService test cases
	INFO_PRINTF1(_L("TestServiceDiscovery23 about to start..."));
	TInt ret(0);
	TUid appUid=KNullUid;
	const TUid KUidService = {0x01120304};
    TDataType dataType;
    _LIT8(KLitMimeTypeServerData,"text/plain");
    ret = iApaLsSession.AppForDocumentAndService(_L("z:\\system\\data\\one_byte.txt"), KUidService, appUid, dataType);	
	TEST(ret==KErrNone);
	TEST(appUid==KNullUid);
	TEST(dataType==TDataType(KLitMimeTypeServerData));
	}


/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery24
  
   @SYMPREQ 538
  
   @SYMTestPriority  High
  
   @SYMTestStatus Implemented

   @SYMTestCaseDesc Test the functionality ofAppForDocumentAndService given an undefined file name
   
   @SYMTestActions  Call AppForDocumentAndService with two parameters: a filename, a service uid, an
   app uid and the datatype requested
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::AppForDocumentAndService(const TDesC& aFileName, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType) const
   
   @SYMTestExpectedResults Returns KErrNone.The value returned for appuid is null.The returned value for the datatpe
   is null.
    
 */	
void CT_ServicesTestStep::TestServiceDiscovery24()
	{
	// AppForDocumentAndService test cases
	INFO_PRINTF1(_L("TestServiceDiscovery24 about to start..."));
	TInt ret(0);
	TUid appUid=KNullUid;
	const TUid KUidService = {0x01020304};
    TDataType dataType;
    ret = iApaLsSession.AppForDocumentAndService(_L(""), KUidService, appUid, dataType);	
	TEST(ret==KErrNone);
	TEST(appUid==KNullUid);
	TEST(dataType.Uid()==KNullUid);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery25
  
   @SYMPREQ 538
   
   @SYMTestPriority  High
  
   @SYMTestStatus Implemented
  
   @SYMTestCaseDesc Test the functionality ofAppForDocumentAndService given a file name, a file, service uid and file server 
   
   @SYMTestActions  Call AppForDocumentAndService with two parameters: a filename, a service uid, an
   app uid and the datatype requested
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::AppForDocumentAndService(const TDesC& aFileName, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType) const
   
   @SYMTestExpectedResults Returns KErrNone.The value returned for appuid is that of the app that provides this services.If more than
   one app provides this service, it returns the first app in the list. The returned value for the datatype is that
   that describes the datatype in the document
    
 */	

void CT_ServicesTestStep::TestServiceDiscovery25()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery25 about to start..."));
	const TUid KUidService = {0x01020303};
	TInt ret(0);
	TUid appUid=KNullUid;
	const TUid TUidExpected = {0x10004c54};
	_LIT8(KLitMimeTypeServerData,"text/plain");
	TDataType dataType;
	RFile file;
	_LIT(KOneByteFileName, "z:\\system\\data\\one_byte.txt");
	RFs fs;
	ret = fs.Connect();
	ret = fs.ShareProtected();
	ret = file.Open(fs, KOneByteFileName, EFileRead|EFileShareAny);
	TEST(ret==KErrNone);
	ret = iApaLsSession.AppForDocumentAndService(file, KUidService, appUid, dataType);
	TEST(appUid==TUidExpected);
	TEST(dataType==TDataType(KLitMimeTypeServerData));
	TEST(ret==KErrNone);
	fs.Close();
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery26
  
   @SYMPREQ 538
   
   @SYMTestPriority  High
  
   @SYMTestStatus Implemented
  
   @SYMTestCaseDesc Test the functionality ofAppForDocumentAndService given a file, service uid,file server and undefined file name 
   
   @SYMTestActions  Call AppForDocumentAndService with two parameters: a filename, a service uid, an
   app uid and the datatype requested
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::AppForDocumentAndService(const TDesC& aFileName, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType) const
   
   @SYMTestExpectedResults Returns KErrBadHandle. The app uid is set to null, the uid of datatype is set to null
    
 */	
void CT_ServicesTestStep::TestServiceDiscovery26()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery26 about to start..."));
	const TUid KUidService = {0x01020304};
	TInt ret(0);
	TUid appUid=KNullUid;
	TDataType dataType;
	RFile file;
	_LIT(KOneByteFileName, "z:\\system\\data\\imaginary_byte.txt");
	RFs fs;
	ret = fs.Connect();
	ret = fs.ShareProtected();
	ret = file.Open(fs, KOneByteFileName, EFileRead|EFileShareAny);
	TEST(ret==KErrNotFound);
	ret = iApaLsSession.AppForDocumentAndService(file, KUidService, appUid, dataType);
	TEST(appUid==KNullUid);
	TEST(dataType.Des8().Length()==0);
	TEST(ret==KErrBadHandle);	
	fs.Close();

	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServiceDiscovery27
  
   @SYMPREQ 538

   @SYMTestPriority  High
  
   @SYMTestStatus Implemented
  
   @SYMTestCaseDesc Test the functionality ofAppForDocumentAndService given a file name, a file, file server and a service uid that the app 
   does not provide  * 
   
   @SYMTestActions  Call AppForDocumentAndService with two parameters: a filename, a service uid, an
   app uid and the datatype requested
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::AppForDocumentAndService(const TDesC& aFileName, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType) const
   
   @SYMTestExpectedResults Returns KErrNone.The value returned for app uid is null.The datatype is set to that that 
   describes the datatype in the document 
    
 */	
void CT_ServicesTestStep::TestServiceDiscovery27()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery27 about to start..."));
	const TUid KUidService = {0x01000300};
	TInt ret(0);
	TUid appUid=KNullUid;
	TDataType dataType;
	RFile file;
	_LIT(KOneByteFileName, "z:\\system\\data\\one_byte.txt");
	RFs fs;
	ret = fs.Connect();
	ret = fs.ShareProtected();
	ret = file.Open(fs, KOneByteFileName, EFileRead|EFileShareAny);
	TEST(ret==KErrNone);
	ret = iApaLsSession.AppForDocumentAndService(file, KUidService, appUid, dataType);
	TEST(appUid==KNullUid);
	_LIT8(KLitMimeTypeServerData,"text/plain");
	TEST(dataType==TDataType(KLitMimeTypeServerData));
	TEST(ret==KErrNone);	
	fs.Close();
	}

/**
 * @SYMTestCaseID T_ServicesTestStep_TestServiceDiscovery28L
 *
 * @SYMPREQ 538
 *
 * @SYMTestCaseDesc Test the functionality of GetServiceImplementationsLC 
 * which gets all the implementation details about a specific service 
 * @SYMTestPriority 
 *
 * @SYMTestStatus Implemented
 * 
 * @SYMTestActions Call GetServiceImplementationsLC with the service uid as parameter
 * on z: drive.\n
 * API Calls:\n	
 * RApaLsSession::GetServiceImplementationsLC(TUid aServiceUid, const TDataType& aDataType) const
 * 
 * @SYMTestExpectedResults Returns an array of TApaAppServiceInfo objects. 
 * The size of the array is equal to the number of apps offering the specified service that
 * also handles the speficied datatype. 
 * Each TApaAppServiceInfo contain an app uid and  the respective opaquedata. 
 * The returned data should be the same as that defined in the registration files.
 *  
 */
void CT_ServicesTestStep::TestServiceDiscovery28L()
	{
	INFO_PRINTF1(_L("TestServiceDiscovery28 about to start..."));
	const TUid KUidService1234 = {0x01020304};
	//const TUid KUidServerApp1 = {0x10004c56};
	const TUid KUidServerApp2 = {0x10004c57};
	
	TDataType datatype(KLitCustom1Text);
	CApaAppServiceInfoArray* array = iApaLsSession.GetServiceImplementationsLC(KUidService1234, datatype);
	TArray<TApaAppServiceInfo> implArray(array->Array());
	TInt count = implArray.Count();	
	TEST(count==1);
	_LIT(KService,"Non-localised text for service uid 0x01020304");
	TPtrC8 opaqueData;
	TResourceReader reader;
	TPtrC16 theText;
	TInt val(0);
	TUid myuid = implArray[0].Uid(); 
	TEST(myuid==KUidServerApp2);
	const CArrayFixFlat<TDataTypeWithPriority>& datatypes = implArray[0].DataTypes();
	TEST(datatypes.Count()==1);
	TEST(0 == datatypes[0].iDataType.Des8().CompareF(KLitCustom1Text));
	opaqueData.Set(implArray[0].OpaqueData());
	reader.SetBuffer(&opaqueData); // opaqueData is an LTEXT resource
	theText.Set(reader.ReadTPtrC16());
	User::LeaveIfError(val = theText.Compare(KService));
	TEST(val==KErrNone);
	CleanupStack::PopAndDestroy(array);
	array = NULL;
	}

/**
 * @SYMTestCaseID T_ServicesTestStep_TestOpenService1
 *
 * @SYMPREQ
 *
 * @SYMTestCaseDesc This test checks that the Open service and legacy datatypes
 * functions return the same results. See defect INC053978 as well.
 * 
 * @SYMTestPriority High 
 *
 * @SYMTestStatus Implemented
 * 
 * @SYMTestActions \n
 * API Calls:\n
 * 
 * @SYMTestExpectedResults 
 *  
 */
void CT_ServicesTestStep::TestOpenService1L()
	{
	INFO_PRINTF1(_L("TestOpenService1L about to start..."));
		
	// First try using the legacy method
	_LIT8(KLitDummyopen1,"text/dummyopen1");
	TUid expectedAppUid1={0x10208200};
	TUid uid1;
	TInt error1 = iApaLsSession.AppForDataType(TDataType(KLitDummyopen1), uid1);
	TEST(error1==KErrNone);
	TEST(uid1==expectedAppUid1);
	
	// Then try with legacy API but new Open service in reg file
	_LIT8(KLitDummyopen2,"text/dummyopen2");
	TUid expectedAppUid2={0x10208201};
	TUid uid2;
	TInt error2 = iApaLsSession.AppForDataType(TDataType(KLitDummyopen2), uid2);
	TEST(error2==KErrNone);
	TEST(uid2==expectedAppUid2);
	
	// Try with new API and old registration file format
	TUid uid3;
	TInt error3 = iApaLsSession.AppForDataTypeAndService(TDataType(KLitDummyopen1), 
		KOpenServiceUid, uid3);
	TEST(error3==KErrNone);
	TEST(uid3==expectedAppUid1);
	
	// Try with new API and new registration file format
	TUid uid4;
	TInt error4 = iApaLsSession.AppForDataTypeAndService(TDataType(KLitDummyopen2), 
		KOpenServiceUid, uid4);
	TEST(error4==KErrNone);
	TEST(uid4==expectedAppUid2);
	
	INFO_PRINTF1(KCompleted);
	}

//Test that the datatype list in the registration file is skipped
void CT_ServicesTestStep::TestCtrlPnlAppL()
	{
	INFO_PRINTF1(_L("TestCtrlPnlAppL about to start..."));
	TUid appUid={0x10207f79};
	TUid serviceUid = {0x10101010};
	CDesCArray* filesArray = new(ELeave) CDesCArraySeg(1);
	CleanupStack::PushL(filesArray);
	TInt ret = iApaLsSession.GetAppOwnedFiles(*filesArray,appUid);
	TEST(ret==KErrNone);
	TInt count(0);
	count = filesArray->Count();
	TEST(count == 1); 
	CApaAppServiceInfoArray* array = iApaLsSession.GetAppServicesLC(appUid);
	TArray<TApaAppServiceInfo> serviceArray(array->Array());
	count = serviceArray.Count();
	TEST(count == 1); 
	TEST(serviceArray[0].Uid()==serviceUid);
	CleanupStack::PopAndDestroy(2);
	filesArray = NULL;
	array = NULL;
	}
	
/**
 * @SYMTestCaseID T_ServicesTestStep_TestPrioritySystemL
 *
 * @SYMPREQ
 *
 * @SYMTestCaseDesc Checks that the system priority for data handlers requires
 * WriteDeviceData capability
 * 
 * @SYMTestPriority High 
 *
 * @SYMTestStatus Implemented
 * 
 * @SYMTestActions \n
 * API Calls:\n
 * 
 * @SYMTestExpectedResults 
 *  
 */
void CT_ServicesTestStep::TestPrioritySystemL()
	{
	INFO_PRINTF1(_L("TestPrioritySystemL about to start..."));
	// App 0x10207f7b has system priority and WriteDeviceData so it should
	// be returned and not 0x10207f7f which has only high priority
	const TUid KUidService1 = {0x10207f7d};
	TUid appUid=KNullUid;
	const TUid TUidExpected1 = {0x10207f7b};
	_LIT8(KLitMimeTypeServerData,"text/custom1");
	TInt ret=iApaLsSession.AppForDataTypeAndService(TDataType(KLitMimeTypeServerData), KUidService1,appUid);
	TEST(ret==KErrNone);
	TEST(appUid == TUidExpected1);
	
	// App 0x10207f7e has system priority but not WriteDeviceData so it should
	// be 0x10207f7f that is returned  that has high priority
	const TUid KUidService2 = {0x10207f7e};
	const TUid TUidExpected2 = {0x10207f7f};
	ret=iApaLsSession.AppForDataTypeAndService(TDataType(KLitMimeTypeServerData), KUidService2,appUid);
	TEST(ret==KErrNone);
	TEST(appUid == TUidExpected2);
	
	// System priority handler take precedence over user mappings
	const TUid KUidServerApp = {0x10004c56};
	ret = iApaLsSession.InsertDataMapping(TDataType(KLitMimeTypeServerData),
	 	1, KUidServerApp, KUidService1);
	TEST(ret==KErrNone);
	// Waits till the data type mappings are restored from the data store ini file
	iApaLsSession.WaitForTypeStoreUpdate();
	ret = iApaLsSession.AppForDataTypeAndService(TDataType(KLitMimeTypeServerData), KUidService1,appUid);
	TEST(ret==KErrNone);
	TEST(appUid == TUidExpected1);
	ret = iApaLsSession.DeleteDataMapping(TDataType(KLitMimeTypeServerData),KUidService1);
	TEST(ret==KErrNone);
	// Waits till the data type mappings are restored from the data store ini file
	iApaLsSession.WaitForTypeStoreUpdate();
	}
