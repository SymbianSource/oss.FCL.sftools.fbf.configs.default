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
// REQ3889 is essentially a change to the boot sequence and not a change to the functionality of apparc,
// i.e. once the system has finished startup the functional behaviour should not have changed, which will
// be proved with execution of the existing App-Framework test suite.
// The test cases implemented below are executed during the Critical Static phase of startup to prove the
// restricted functionality during this phase is correct. All calls to the API will panic except the call
// to RApaLsSession::StartApp which has been modified not to panic.
// Note that some of the tests cases used in this test are later executed during the normal
// phone operation phase (non-critical phase) by other tests (see T_StartAppStep.cpp, T_RapaLsSession.cpp, ...)
// As nearly all these test cases panic at the first client-server call and so execution of these test cases
// will never reach the end of the function.
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <apacmdln.h>
#include <s32file.h>

#include "appfwk_test_AppUi.h"
#include "T_SysStartApparcStep.h"
#include "SysStartApparcTestCases.h"
#include "TRApaLsSessionStartAppTest.h"
#include "TRApaLsSessionTestUids.h"
#include "tstapp.h"
#include "tssaac.h"

_LIT(KNonExistingFile, "z:\\system\\data\\not_here_thanks.foo");
_LIT(KLitExePath, "\\sys\\bin\\texe.exe");


/**
   Constructor
 */
CSysStartApparcTestCase::CSysStartApparcTestCase()
	{
	}

/**
   Destructor
 */
CSysStartApparcTestCase::~CSysStartApparcTestCase()
	{
	iApaLsSession.Close();
	}

/**
Connect to the Apparc server and call RunTestCasesL() inside a
trap harness. Panic if an error is returned
*/
void CSysStartApparcTestCase::doTestCasesL()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test Cases Running..."));

	TInt error = iApaLsSession.Connect();
	if(error!=KErrNone)
		{
		RDebug::Print(_L("CSysStartApparcTestCase: Error connecting to RApaLsSession (err=%d)"), error);
		}

	TRAP(error,RunTestCasesL())
	if(error!=KErrNone)
		{
		RDebug::Print(_L("CSysStartApparcTestCase: Error running test cases (err=%d)"), error);
		}

	RDebug::Print(_L("CSysStartApparcTestCase: Test Cases Completed!"));
	}

/**
Call each test case inside a trap harness and panic if an error is returned.
The Panic will be raised in doTestCasesL() function.
The Use __UHEAP macros to test for memory leaks.
Reads in the test case number from file TestCaseConfig.txt
Each test case is assigned a unique number in the batch file sysstart_apparc_run.bat
*/
void CSysStartApparcTestCase::RunTestCasesL()
	{
	__UHEAP_MARK;

	TInt aTestCase=ReadConfigFileL();

	switch(aTestCase)
		{
		case 1:
			TestStartApp1L();
			WriteResultFileL(); //Called if the test case does not panic
			break;

		case 2:
			TestStartApp2L();
			break;

		case 3:
			TestStartApp3L();
			break;

		case 4:
			TestStartApp4L();
			break;

		case 5:
			TestStartApp5L();
			break;

		case 6:
			TestStartApp6L();
			break;

		case 7:
			TestGetAllApps();
			break;

		case 8:
			TestInsertDataTypeL();
			break;

		case 9:
			TestAppForDataTypeL();
			break;

		case 10:
			TestDeleteDataTypeL();
			break;

		case 11:
			TestServiceDiscovery();
			break;

		case 12:
			TestGetAppInfo();
			break;

		case 13:
			TestAppCount();
			break;

		case 14:
			TestCreateDoc();
			break;

		case 29:
		case 0:
		default:
			RDebug::Print(_L("CSysStartApparcTestCase: Unknown Test Case Specified"));
			break;
		}

	__UHEAP_MARKEND;
	}


/**
   @SYMTestCaseID          APPFWK-APPARC-I-0001

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Test Launching of an application using command line information.
                           Test is run during SSA Critical Static phase,

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::StartApp() to start an application defined by the command
   						   line information. Test the launching of application for following scenarios:
                           (1) When Application specified by command line exists.
                           (2) When Application specified by command line does not exist.
                           API Calls:\n
						   RApaLsSession::StartApp(const CApaCommandLine& aCommandLine)

   @SYMTestExpectedResults (1) StartApp() returns KErrNone
                           (2) StartApp() returns KErrNotFound
*/
void CSysStartApparcTestCase::TestStartApp1L()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::StartApp using CApaCommandLine....."));

	// Set filename location
	TPtrC apparcTestFilename(KLitExePath);

	// (1) start application texe.exe from the commandline
	CApaCommandLine* cmdLn=CApaCommandLine::NewLC();
	cmdLn->SetExecutableNameL(apparcTestFilename);
	_LIT8(KLitDogfish,"dogfish");
	cmdLn->SetTailEndL(KLitDogfish);
	iStartApp1LRes1 = iApaLsSession.StartApp(*cmdLn); // explicit

	if(iStartApp1LRes1 == KErrNone)
		{
		RDebug::Print(_L("CSysStartApparcTestCase: TestStartApp1L Launch of Command line app successful"));
		}

	// (2) start a non-existant app
	_LIT(KLitWibble,"wibble");
	cmdLn->SetExecutableNameL(KLitWibble);
	iStartApp1LRes2 = iApaLsSession.StartApp(*cmdLn);
	if(iStartApp1LRes2 == KErrNotFound)
		{
		RDebug::Print(_L("CSysStartApparcTestCase: TestStartApp1L Launch of non-existant app returns KErrNotFound"));
		}

	CleanupStack::PopAndDestroy(cmdLn); // cmdLn
	}

/**
   @SYMTestCaseID          APPFWK-APPARC-0002

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Get the MIME type of the data in the specified document and return the UID of an
   						   application that can handle this type.

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::AppForDocument() to return the UID of an application that can
   						   read/display the specified file. The Apparc server is not ready in CS phase, so the
   						   call has to fail.
                           API Calls:\n
						   RApaLsSession::AppForDocument(const TDesC& aFileName, TUid& aAppUid, TDataType& aDataType) const

   @SYMTestExpectedResults AppForDocument() causes Main Panic KERN-EXEC 0
*/
void CSysStartApparcTestCase::TestStartApp2L()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::AppForDocument using filename and app uid...."));

	TUid retUid = KNullUid;

	TDataType data;

	// Set filename location
	TPtrC apparcTestFilename(KLitTRApaLsSessionStartAppTest);

	// Main Panic KERN-EXEC 0
	iApaLsSession.AppForDocument(apparcTestFilename, retUid, data);

	}

/**
   @SYMTestCaseID          APPFWK-APPARC-0003

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Finds and launches an application to match the specified document.

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::StartDocument() with a document name
                           API Calls:\n
						   RApaLsSession::StartDocument(const TDesC& aFileName, TThreadId& aThreadId, TLaunchType aLaunchType)

   @SYMTestExpectedResults StartDocument() causes Main Panic KERN-EXEC 0
*/
void CSysStartApparcTestCase::TestStartApp3L()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::StartDocument....start the app from the recognized doc given."));

	// Set filename location
	TPtrC apparcTestFilename(KLitTRApaLsSessionStartAppTest);

	//Call to tested function - for info, startAppThreadID is assigned to the app thread id
  	TThreadId startAppThreadID;

	//Main Panic KERN-EXEC 0
	iApaLsSession.StartDocument(apparcTestFilename, startAppThreadID);
	}


/**
   @SYMTestCaseID          APPFWK-APPARC-0004

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Launches an application to suit the specified document by means of a specified MIME type

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::StartDocument() with filename and MIME type.
                           API Calls:\n
						   RApaLsSession::StartDocument(const TDesC& aFileName, const TDataType& aDataType, TThreadId& aThreadId, TLaunchType aLaunchType)

   @SYMTestExpectedResults StartDocument() causes Main Panic KERN-EXEC 0
*/
void CSysStartApparcTestCase::TestStartApp4L()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::StartDocument....known data type"));

	// Set filename location
	TPtrC apparcTestFilename(KLitTRApaLsSessionStartAppTest);

	// KErrNotFound as a return value
	TThreadId startAppThreadID;

	//Main Panic KERN-EXEC 0
	iApaLsSession.StartDocument(apparcTestFilename, TDataType(KLitMimeType_TRApaLsSessionStartAppTest), startAppThreadID);
	}


/**
   @SYMTestCaseID          APPFWK-APPARC-0005

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Attempt to launch an application by means of an unknown MIME type

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::StartDocument() with filename and unknown MIME type.
                           API Calls:\n
						   RApaLsSession::StartDocument(const TDesC& aFileName, const TDataType& aDataType, TThreadId& aThreadId, TLaunchType aLaunchType)

   @SYMTestExpectedResults StartDocument() causes Main Panic KERN-EXEC 0
*/
void CSysStartApparcTestCase::TestStartApp5L()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::StartDocument....unknown data type"));

	// Set filename location
	TPtrC apparcTestFilename(KLitTRApaLsSessionStartAppTest);

	// KErrNotFound as a return value
	TThreadId startAppThreadID;

	//Main Panic KERN-EXEC 0
	iApaLsSession.StartDocument(apparcTestFilename, TDataType(_L8("text/plain/no thanks")), startAppThreadID);
	}

/**
   @SYMTestCaseID          APPFWK-APPARC-0006

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Attempt to launch an application by specifying a non-existent file

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::StartDocument() with a non-existent file
                           API Calls:\n
						   RApaLsSession::StartDocument(const TDesC& aFileName, TThreadId& aThreadId, TLaunchType aLaunchType)

   @SYMTestExpectedResults StartDocument() causes Main Panic KERN-EXEC 0
*/
void CSysStartApparcTestCase::TestStartApp6L()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::StartDocument....non existent file."));

	// Set filename location
	TPtrC apparcTestFilename(KNonExistingFile);

	// KErrNotFound as a return value
	TThreadId startAppThreadID;

	//Main Panic KERN-EXEC 0
	iApaLsSession.StartDocument(apparcTestFilename, startAppThreadID);
	}

/**
   @SYMTestCaseID          APPFWK-APPARC-0007

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Test the functionality of GetAllApps

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::GetAllApps()

   @SYMTestExpectedResults GetAllApps() causes Main Panic KERN-EXEC 0

*/
void CSysStartApparcTestCase::TestGetAllApps()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::GetAllApps....check list of apps"));

	// Main Panic KERN-EXEC 0
	iApaLsSession.GetAllApps();

	}

/**
   @SYMTestCaseID          APPFWK-APPARC-0008

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Tests InsertDataMapping() which links a datatype to the UID of an associated application
   						   for launching assigns adds a low priority.
                           Tests InsertDataMappingIfHigher() which remaps the datatype to the specified UID if the
                           specified priority is higher.
                           The "added" parameter indicates if the mapping was successful.

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::InsertDataMapping() with datatype, application UID and low priority
                           API Calls:\n
						   RApaLsSession::InsertDataMapping(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid)

   @SYMTestExpectedResults InsertDataMapping() causes Main Panic KERN-EXEC 0
*/
void CSysStartApparcTestCase::TestInsertDataTypeL()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::InsertDataMapping....Check data type mapping addition"));
	const TDataTypePriority KPriLow = 1;

	_LIT8(KLitPlainText,"text/plain");

	TSecurityInfo info;
	info.Set(RProcess());

	//Main Panic KERN-EXEC 0
	iApaLsSession.InsertDataMapping(TDataType(KLitPlainText), KPriLow, KUidTestApp);

	}

/**
   @SYMTestCaseID          APPFWK-APPARC-0009

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Tests AppForDataType() which returns the UID of an application which can handle the specified datatype

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::InsertDataMapping() with datatype and application UID (and priority)
                           API Calls:\n
						   RApaLsSession::InsertDataMapping(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid)

   @SYMTestExpectedResults AppForDataType() causes Main Panic KERN-EXEC 0
*/
void CSysStartApparcTestCase::TestAppForDataTypeL()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::AppForDataType....Check an app binding"));

	_LIT8(KLitPlainText,"text/plain");

	TUid retUid = KNullUid;

	//Main Panic KERN-EXEC 0
	iApaLsSession.AppForDataType(TDataType(KLitPlainText), retUid);

	}


/**
   @SYMTestCaseID          APPFWK-APPARC-0010

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Tests DeleteDataMapping() which removes the linking of a datatype to the UID of an associated application

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::DeleteDataMapping() with datatype and service UID
                           API Calls:\n
						   RApaLsSession::DeleteDataMapping(const TDataType& aDataType, TUid aServiceUid)

   @SYMTestExpectedResults DeleteDataMapping() causes Main Panic KERN-EXEC 0
*/
void CSysStartApparcTestCase::TestDeleteDataTypeL()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::DeleteDataMapping....Check data type mapping deletion"));

	_LIT8(KLitPlainText,"text/plain");

	TSecurityInfo info;
	info.Set(RProcess());

	//Main Panic KERN-EXEC 0
	iApaLsSession.DeleteDataMapping(TDataType(KLitPlainText));
 	}


/**
   @SYMTestCaseID          APPFWK-APPARC-0011

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Test the functionality of GetServerApps with the service uid as parameter.

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::GetServerApps() with a service uid that
   						   is not provided by any application
                           API Calls:\n
						   RApaLsSession::GetServerApps(TUid aServiceUid) const

   @SYMTestExpectedResults GetServerApps() causes Main Panic KERN-EXEC 0

*/
void CSysStartApparcTestCase::TestServiceDiscovery()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::GetServerApps....check the service uid"));

	TApaAppInfo appInfo;

	// Uid is not provided by any application
	const TUid KUidNonProvidedService = {0x01020300}; // service Uid

	//Main Panic KERN-EXEC 0
	iApaLsSession.GetServerApps(KUidNonProvidedService);

	}


/**
   @SYMTestCaseID          APPFWK-APPARC-0012

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Test the functionality of GetAppInfo with a Uid as a parameter

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::GetAppInfo() with a Uid that
   						   represents a non-existing app

                           API Calls:\n
						   RApaLsSession::GetAppInfo(TApaAppInfo& aInfo,TUid aAppUid) const

   @SYMTestExpectedResults GetAppInfo() causes Main Panic KERN-EXEC 0

*/
void CSysStartApparcTestCase::TestGetAppInfo()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::GetAppInfo....check the app uid"));

	TApaAppInfo info;

	// non-existing app  causes Main Panic KERN-EXEC 0
	iApaLsSession.GetAppInfo(info, KNullUid);

	}


/**
   @SYMTestCaseID          APPFWK-APPARC-0013

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Test the functionality of AppCount

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call RApaLsSession::AppCount()


   @SYMTestExpectedResults AppCount() causes Main Panic KERN-EXEC 0

*/
void CSysStartApparcTestCase::TestAppCount()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::AppCount()....check list of apps"));

	TInt dummyArg;

	// Main Panic KERN-EXEC 0
	iApaLsSession.AppCount(dummyArg);

	}


/**
   @SYMTestCaseID           APPFWK-APPARC-0014

   @SYMPREQ                 PREQ967

   @SYMREQ                  REQ3889

   @SYMTestCaseDesc         Test the functionality of CreateDocument with a Uid as a parameter

   @SYMTestPriority         High

   @SYMTestStatus           Implemented

   @SYMTestActions          Call RApaLsSession::CreateDocument() with a Uid of an application
   						    When Application specified exists.

                            API Calls:\n
						    RApaLsSession::CreateDocument(const TDesC& aFileName, TUid aAppUid, TThreadId& aThreadId, TLaunchType aLaunchType)

   @SYMTestExpectedResults (1) GetAppInfo() causes Main Panic KERN-EXEC 0

*/
void CSysStartApparcTestCase::TestCreateDoc()
	{
	RDebug::Print(_L("CSysStartApparcTestCase: Test RApaLsSession::CreateDocument....check the app uid"));

	// Set filename location
	TPtrC apparcTestFilename(KLitTRApaLsSessionStartAppTest);

  	TThreadId startAppThreadID;

	//Call to tested function with a existing uid (tstapp)  causes Main Panic KERN-EXEC 0
	iApaLsSession.CreateDocument(apparcTestFilename, KUidTestApp, startAppThreadID);
	}

/**
Reads the input config file created by sysstart_apparc_setup.bat and returns the
test case number (Defined in sysstart_apparc_run.bat)
@return TInt - the test case number
*/
TInt CSysStartApparcTestCase::ReadConfigFileL()
	{
	RFs	fs;
 	User::LeaveIfError(fs.Connect());
 	CleanupClosePushL(fs);

 	RFileReadStream reader;
	User::LeaveIfError(reader.Open(fs, KApparcConfigFile, EFileRead));
	CleanupClosePushL(reader);

	TChar delimChar('=');

	TBuf8<255> aDes;
	reader.ReadL(aDes, delimChar);

	//Read in a two character representation of a number and convert to an integer
	TChar result1(reader.ReadInt8L());
	TChar result0(reader.ReadInt8L());
	TInt aTestCase = result1.GetNumericValue()*10 + result0.GetNumericValue();

	// clean-up
	CleanupStack::PopAndDestroy(2, &fs);

	return aTestCase;

	}

/**
  Write the results to the result file, to be later extracted by
  testexecute test case.
  These contain the results of the individual tests. The order in
  which these results are written in MUST match the order in which
  they are read and tested in CSysStartApparcTestStep::GetApparcResultsL()
  and the expected result tested in CSysStartApparcTestStep::Testxxx must
  match that in the test specification\n

  The format is:
  StartApp1LRes1 count=value1
  StartApp1LRes2 count=value2

  '=' is used as the deliminator.
*/
void CSysStartApparcTestCase::WriteResultFileL()
	{
	RFs	fs;
 	User::LeaveIfError(fs.Connect());
 	CleanupClosePushL(fs);

	RFileWriteStream writer;
	User::LeaveIfError(writer.Replace(fs, KSsaacResultFile, EFileWrite));
	CleanupClosePushL(writer);

	// write data to file
	writer << _L("StartApp1LRes1 count=");
	writer.WriteInt16L(iStartApp1LRes1);
	writer << _L("\n");
	writer << _L("StartApp1LRes2 count=");
	writer.WriteInt16L(iStartApp1LRes2);
	writer << _L("\n");

	writer.CommitL();
	CleanupStack::PopAndDestroy(2, &fs);
	}
