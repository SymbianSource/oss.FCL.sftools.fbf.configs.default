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
// Test CApaCommandLine Apis\n
// Test CApaCommandLine API by setting launching information and
// checking setter and getter function.\n
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include "T_CmdlnStep.h"
#include "testableapalssession.h"

 /**
   @SYMTestCaseID T-CmdlnStep-testSecureCmdLinesL
  
   @SYMPREQ 280 - File Handle Support
  
   @SYMTestCaseDesc Tests CApaCommandLine Apis. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
  
   
   @SYMTestActions .
   Observe the operation that is executed on each command.\n
   Test the following accessor functions:\n
   CApaCommandLine::ExecutableName()\n
   CApaCommandLine::Command()\n
   CApaCommandLine::DocumentName()\n
   CApaCommandLine::TailEnd()\n
   CApaCommandLine::ServerId()\n
   CApaCommandLine::SessionHandle()\n
   CApaCommandLine::SubSessionHandle()\n
   Check whether each of these getter functions return the individual components.\n\n
   Test the following setter functions:\n
   CApaCommandLine::SetExecutableNameL()\n
   CApaCommandLine::SetCommandL()\n
   CApaCommandLine::SetDocumentNameL()\n
   CApaCommandLine::SetTailEndL()\n
   CApaCommandLine::SetServerId()\n
   CApaCommandLine::SetSessionHandle()\n
   CApaCommandLine::SetSubSessionHandle()\n
   Use the setter functions to set the individual components. Use the
   getter methods to verify values set to the individual components.\n
   API Calls:\n	
   CApaCommandLine::NewL(const RMessagePtr2& aMessage)\n 
   CApaCommandLine::ExecutableName() const\n
   CApaCommandLine::Command() const\n
   CApaCommandLine::DocumentName() const\n
   CApaCommandLine::TailEnd() const\n
   CApaCommandLine::TailEnd() const\n
   CApaCommandLine::ServerId() const\n
   CApaCommandLine::SessionHandle() const\n
   CApaCommandLine::SubSessionHandle() const\n
   CApaCommandLine::EnviromentSlotsReaderL()\n
   CApaCommandLine::SetExecutableNameL(const TDesC& aAppName)\n
   CApaCommandLine::SetCommandL(TApaCommand aCommand)\n
   CApaCommandLine::SetDocumentNameL(const TDesC& aDocName)\n
   CApaCommandLine::SetTailEndL(const TDesC8& aTailEnd)\n
   CApaCommandLine::SetServerId(TInt aServerId)\n
   CApaCommandLine::SetSessionHandle(TInt aSessionHandle)\n
   CApaCommandLine::SetSubSessionHandle(TInt aSubSessionHandle)\n
   CApaCommandLine::SetProcessEnvironmentL(RProcess& aProcess) const\n
  
   
  
   @SYMTestExpectedResults Tests checks results against desired results.
    
 */
 void CT_CmdlnStep::testSecureCmdLinesL()
 	{
 	RTestableApaLsSession ls;
 	TEST(KErrNone == ls.Connect());
 	CleanupClosePushL(ls);
 	
 	testPositiveInputsL();
	testInitializationL();
	testBoundaryConditionsL();
	//DONT_CHECK since type store is updated
    HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, testRecAppLaunchL(ls), NO_CLEANUP);
    
    CleanupStack::PopAndDestroy(&ls);
 	}

/**
   Auxiliary function for TestCaseID T-CmdlnStep-testSecureCmdLinesL
  
  
   @SYMPREQ 280 - File Handle Support
  
    Call the Set APIs below with valid inputs
    CApaCommandLine::SetExecutableNameL(appName);
    CApaCommandLine::SetDocumentNameL(docName);
    CApaCommandLine::SetTailEndL(tailEnd);
    CApaCommandLine::SetFileByHandleL(aFile);
    CApaCommandLine::SetCommandL(command);
    
    Compares the values set by the above APIs with those obtained by the GET APIs mentioned below
    CApaCommandLine::ExecutableName());
    CApaCommandLine::DocumentName());
    CApaCommandLine::TailEnd());
    CApaCommandLine::GetFileByHandleL();
    CApaCommandLine::Command());
	
  
 */
void CT_CmdlnStep::testPositiveInputsL()
	{
	TPtrC appName;
	TPtrC docName;
	TApaCommand command = EApaCommandRun;
	TPtrC8  tailEnd;
	TEntry dirEntry;
	RFs fSession;
	RFile file;
	
	INFO_PRINTF1(_L("Tests the Set and Get command line APIs  for valid inputs"));	
	CApaCommandLine* cmdLine=CApaCommandLine::NewLC(); // start with an empty command line
	
	appName.Set(KTAppName);
	docName.Set(KTDocName);
	tailEnd.Set(KTTailEnd);
	
	User::LeaveIfError(fSession.Connect());
	//User::LeaveIfError(fSession.ShareProtected());	
	INFO_PRINTF1(_L("Check for the existence of C:\\System\\Data Directory"));	
	TInt ret = fSession.Entry(KTempDir,dirEntry);	
	if(ret==KErrNotFound)
	{
		INFO_PRINTF1(_L("Create C:\\System\\Data Directory if not existing already"));
		fSession.MkDir(KTempDir); /* Create Data Directory as it does not exist  */				
	}	
	User::LeaveIfError(file.Replace(fSession, KTDocName, EFileStreamText|EFileShareAny));
	User::LeaveIfError(file.Write(KTWriteData));//write some test data
	
	INFO_PRINTF1(_L("The Set command line APIs  are invoked with valid arguments"));	
	// Set the member variables using the Set functions
	
	TRAP(ret,
		{ 
		cmdLine->SetExecutableNameL(appName);
		cmdLine->SetDocumentNameL(docName);
		cmdLine->SetTailEndL(tailEnd);
		cmdLine->SetFileByHandleL(file);
		cmdLine->SetCommandL(command);
		} )
		
	TEST(KErrNone==ret);
	
	if(KErrNone==ret)
		{
		INFO_PRINTF1(_L("The Get command line APIs  are invoked and their return values are verified"));	
		//Test the values returned by the Get Functions
		TEST(appName==cmdLine->ExecutableName());
		TEST(docName==cmdLine->DocumentName());
		TEST(tailEnd==cmdLine->TailEnd());	
	
		RFile tempFile;
		TBuf8<10> readData;
		TRAP(ret,cmdLine->GetFileByHandleL(tempFile));
		TEST(KErrNone==ret);
		if(KErrNone==ret)
			{
			tempFile.Read(0,readData);//file still open so rewind file pointer
			TEST(readData==KTWriteData);
			}
		}
	
	file.Close();
	fSession.Close();	
	//fSession.Delete(KTDocName);
	CleanupStack::PopAndDestroy(cmdLine); 

	}


/**
    Auxiliary function for TestCaseID T-CmdlnStep-testSecureCmdLinesL
  
    @SYMPREQ 280 - File Handle Support
  
    Tests the initial values of the member variables 
  
    Following are the SET APIs tested 
    CApaCommandLine::SetExecutableNameL(appName);
    CApaCommandLine::SetDocumentNameL(docName);
    CApaCommandLine::SetTailEndL(tailEnd);
    CApaCommandLine::SetSessionHandle(TInt aSessionHandle);
    CApaCommandLine::SetSubSessionHandle(TInt aSubSessionHandle);
    CApaCommandLine::SetServerIdL(TInt serverId);
    CApaCommandLine::SetCommandL(command);
    
    Compares the values set with those obtained by the GET APIs
    CApaCommandLine::ExecutableName();
    CApaCommandLine::DocumentName();
    CApaCommandLine::TailEnd();
    CApaCommandLine::ServerId();
    CApaCommandLine::SessionHandle();
    CApaCommandLine::SubSessionHandle();
    CApaCommandLine::Command();
  
    Following Scenarios are tested
   
    1. Do not set the values and check the values returned by the GET APIs
    2. Set Application Name and check all the values returned by the GET APIs
    3. Set AppName,DocName  and check all the values returned by the GET APIs
    4. Set AppName,DocName,Tailend  and check all the values returned by the GET APIs
    5. Set AppName,DocName,Tailend,Serverid and check all the values returned by the GET APIs  
    6. Set AppName,DocName,Tailend,Serverid,sessionId and check all the values returned by the GET APIs  
    7. Set AppName,DocName,Tailend,Serverid,sessionId and subsessionId and check all the values returned by the GET APIs  
 */

void CT_CmdlnStep::testInitializationL()
	{
	
	TPtrC appName(KNullDesC);
	TPtrC docName(KNullDesC);
	TApaCommand cmdType = EApaCommandRun;//CApaCommandLine's initial value
	TPtrC8  tailEnd(KNullDesC8);
	RFs fSession;
	RFile file;
	TEntry dirEntry;
		
	CApaCommandLine* cmdLine=CApaCommandLine::NewLC(); // start with an empty command line

	for(TInt index = 0; index < TEST_SET_APIS-1; index++)
    	{
    	switch(index)
    		{
    		case TEST_INITIALIZATION:
    			INFO_PRINTF1(_L("Tests the initial values of the member data"));
    			break;
    		case TEST_APP:
    			INFO_PRINTF1(_L("Setting Application Name to \"app name\"."));
				appName.Set(KTAppName);
				cmdLine->SetExecutableNameL(appName);
    			break;
    		case TEST_DOC:
    			INFO_PRINTF1(_L("Setting document name to \"doc name\"."));
				docName.Set(KTDocName);
				cmdLine->SetDocumentNameL(docName);
    			break;
    		case TEST_TAILEND:
    			INFO_PRINTF1(_L("Setting tail end to \"tail end \"."));
				tailEnd.Set(KTTailEnd);
				cmdLine->SetTailEndL(tailEnd);
    			break;
    		case TEST_COMMAND:
    			INFO_PRINTF1(_L("Setting tail end to \"command type \"."));
    			cmdType=EApaCommandOpen;//any update to the command value 
    			cmdLine->SetCommandL(cmdType);
    			break;
    		case TEST_FILE_HANDLE:
    			{
    			INFO_PRINTF1(_L("Setting file handle name to \"handle name\"."));
    			User::LeaveIfError(fSession.Connect());
				User::LeaveIfError(fSession.ShareProtected());	
				INFO_PRINTF1(_L("Check for the existence of C:\\System\\Data Directory"));	
				TInt ret = fSession.Entry(KTempDir,dirEntry);	
				if(ret==KErrNotFound)
					{
					INFO_PRINTF1(_L("Create C:\\System\\Data Directory if not existing already"));	
					}	
				User::LeaveIfError(file.Replace(fSession, KTDocName, EFileStreamText|EFileShareAny));
    			TRAP(ret,cmdLine->SetFileByHandleL(file));
    			TEST(KErrNone==ret);
    			break;
    			}
    		default:
    			break;
    		}
    		
    		INFO_PRINTF1(_L("The Get command line APIs  are invoked and their return values are verified"));	
    
    		//Test the values returned by the Get Functions
    		TEST(appName==cmdLine->ExecutableName());
			TEST(docName==cmdLine->DocumentName());
			TEST(tailEnd==cmdLine->TailEnd());
			TEST(cmdType==cmdLine->Command());
			
			RFile testFile;
			TRAPD(ret,cmdLine->GetFileByHandleL(testFile));
			TEST(KErrNone==ret);
			if(KErrNone==ret)
				{	//this test relies on TEST_FILE_HANDLE being the last case in the case statement 
					if(index==TEST_FILE_HANDLE)
						{//not equal when initialised
						TEST(file.SubSessionHandle()!=testFile.SubSessionHandle());	
						}
						else
						{//equal when uninitialised
						TEST(file.SubSessionHandle()==testFile.SubSessionHandle());
						}
				}			
       	}
    	
    file.Close();
	fSession.Close();	
	//fSession.Delete(KTDocName);
    CleanupStack::PopAndDestroy(cmdLine); 
    
    }

/**
    Auxiliary function for TestCaseID T-CmdlnStep-testSecureCmdLinesL
  
    @SYMPREQ 280 - File Handle Support
  
    Tests CApaCommandLine Apis. 
   
    Tests the Boundary values of the member variables like Null descriptors and zero for integer member variables
    Also test negative values for the integer member variables
    
    CApaCommandLine::SetExecutableNameL(appName);
    CApaCommandLine::SetDocumentNameL(docName);
    CApaCommandLine::SetTailEndL(tailEnd);
    CApaCommandLine::SetCommandL(command);
    
    Compares the values set with those obtained by the GET APIs
    CApaCommandLine::ExecutableName());
    CApaCommandLine::DocumentName());
    CApaCommandLine::TailEnd());
    CApaCommandLine::Command());
  
    Following Scenarios are tested
   
    1. Set AppName,DocName and Tailend to Null descriptors and check the values returned by the corresponding
       GET APIs
 */

void CT_CmdlnStep::testBoundaryConditionsL()
	{
		
	TPtrC appName(KNullDesC);
	TPtrC docName(KNullDesC);
	TPtrC8 tailEnd;
	
	CApaCommandLine* cmdLine = CApaCommandLine::NewLC(); // start with an empty command line
	
	INFO_PRINTF1(_L("TEST APIS WITH BOUNDARY CONDITIONS\n"));
	
	//Set the member variables 
	cmdLine->SetExecutableNameL(appName);
	cmdLine->SetDocumentNameL(docName);
	tailEnd.Set(KTNullTail);
	cmdLine->SetTailEndL(tailEnd);
				
	// Check the values returned by the Get Functions		
	TEST(appName==cmdLine->ExecutableName());
	TEST(docName==cmdLine->DocumentName());
	TEST(tailEnd==cmdLine->TailEnd());
	
	CleanupStack::PopAndDestroy(cmdLine); 
    
    }

/**
   Auxiliary function for TestCaseID T-CmdlnStep-testSecureCmdLinesL
  
   @SYMPREQ 280 - File Handle Support
  
   Tests CApaCommandLine Apis. 
  
   The functions tests positive and negative test scenarios for API 
   TInt RApaLsSession::StartDocument(const TDesC& aFileName, RFs& aFSession, RFile& aFile, TThreadId& aThreadId) 
    
   The test case opens a document "temp.test" with datatype "test/plain" which is recognized by TESTREC.
   T_EnvSlots.exe is application mapped to the this data type using InsertDataMappingL function.
   When the file is opened using StartDocument the application is launched which verifies the environment slots 
   of the launched application to those set.
   
   
 */
 
void CT_CmdlnStep::testRecAppLaunchL(RApaLsSession& aLs)
	{

	TThreadId startAppThreadID;
	RFs fSession;
	RFile file;
	TDataType dataType(KTDataType);
	TEntry dirEntry;
		
	User::LeaveIfError(fSession.Connect());
	User::LeaveIfError(fSession.ShareProtected());
	
	INFO_PRINTF1(_L("Check for the existence of C:\\System\\Data Directory"));	
	TInt ret = fSession.Entry(KTempDir,dirEntry);
	
	if(ret==KErrNotFound)
	{
		INFO_PRINTF1(_L("Create C:\\System\\Data Directory if not existing already"));	
	}
	
	User::LeaveIfError(file.Replace(fSession, KTDocName, EFileStreamText|EFileShareAny));
	User::LeaveIfError(file.Write(KTWriteData));
		
	//Create A Global mutex to control the access to the logfile
	
	RMutex fileAccess;
	fileAccess.CreateGlobal(KTLogFileAccess);
	
	INFO_PRINTF1(_L("Start Valid Document of Data type : test"));
	
	TUid uid;
	uid.iUid = (0x102032AB);
	ret = aLs.InsertDataMapping(dataType, KDataTypePriorityHigh, uid);
	TEST(ret==KErrNone);
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("InsertDataMapping API call successful"));
		}
	else 
		{
		INFO_PRINTF1(_L("InsertDataMapping API call failed"));
		}
	
	ret = aLs.StartDocument(file,startAppThreadID);
	TEST(ret==KErrNone);
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("Start Document API call successful"));
		}
	else 
		{
		INFO_PRINTF1(_L("Start Document API call failed"));
		}
		
	// Make the process sleep in order to launch T_EnvSlots
	User::After(KOneSecond);
	
	// Wait for the Mutex for log file access
	fileAccess.Wait();
	TBufC<KMaxFilePath> filePath(KEnvFilePath);
	RFile logFile;
	
	INFO_PRINTF1(_L("File Access Mutex obtained"));
	
	//Open the logfile in the readmode	
	User::LeaveIfError(logFile.Open(fSession,filePath,EFileRead));
	
	INFO_PRINTF1(_L("Env Log File Open successful"));
				
	TBuf8<8> expected(KTPass);
	TBuf8<8> obtained;
	TBuf8<8> failReason;		
	
	// Read from the logfile
	User::LeaveIfError(logFile.Read(obtained));
	
	INFO_PRINTF1(_L("Env Log File Read successful"));
		
	TEST(expected==obtained);
	
	if(expected == obtained)
	{
		INFO_PRINTF1(_L("Environment slots verification Passed"));
	}
	else {
		// Read up to the end of the file to log all the failures
		do
		{
			// Read the environment slot whose verification failed 
			logFile.Read(failReason);
			
			if(failReason == KTApp)
			{
				INFO_PRINTF1(_L("Environment slot verification failed for Application Name"));
			}
			else if(failReason == KTDoc)
			{
				INFO_PRINTF1(_L("Environment slot verification failed for Document Name"));
			}
			else if(failReason == KTServer)
			{
				INFO_PRINTF1(_L("Environment slot verification failed for Server Id"));
			}
			else if(failReason == KTCommand)
			{
				INFO_PRINTF1(_L("Environment slot verification failed for Command"));
			}
			
			// Skip the "TestFail" text
			logFile.Read(failReason);
			
			//Exit the loop if end of the file 
			if(failReason.Length() == 0)
				break;
			
		} while(ETrue);
	}
	
	ret = aLs.DeleteDataMapping(dataType);
	TEST(ret==KErrNone);
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("DeleteDataMapping API call successful"));
		}
	else 
		{
		INFO_PRINTF1(_L("DeleteDataMapping API call failed"));
		}
		
	ret = aLs.StartDocument(file,startAppThreadID);
	TEST(ret==KErrNotFound);
	if(ret == KErrNotFound)
		{
		INFO_PRINTF1(_L("Start Document API call with expected failure successful"));
		}
	else 
		{
		INFO_PRINTF1(_L("Start Document API call with expected failure failed"));
		}
	
	logFile.Close();
		
	// Test on Invalid Document Name, Invalid File Session and Invalid File Args removed
	// This test was invalid & panics
		
	file.Close();
	fSession.Close();	
	//fSession.Delete(KTDocName);
	fileAccess.Close();
		
	}

/**
   @SYMTestCaseID CT-CmdlnStep-testCmdLinesAPIsL()
  
   @SYMPREQ

   @SYMTestCaseDesc Command line APIs to be properly tested 
 
   @SYMTestActions CApaCommandLine::NewLC() is called to create a commandline object;CApaCommandLineSetFileByHandleL() is called to set the file session and file handle; RProcess::Create is called to create a new process to launch the exe;
   CApaCommandLine::SetProcessEnvironmentL is called to initialise the parameters of the child process; CApaCommandLine::GetCommandLineFromProcessEnvironment is
   called by the child process to get the commandline object created by the parent process; In order to be able to read the file, CApaCommandLine::GetFileByHandleL is called by the child process
   to get the file handle created by the parent process. 
   
   API Calls:static CApaCommandLine* NewLC();
 			 void SetProcessEnvironmentL(RProcess& aProcess) const;
 			 static TInt GetCommandLineFromProcessEnvironment(CApaCommandLine*& aCommandLine);
 			 void SetFileByHandleL(const RFile& aFile);
			 void GetFileByHandleL(RFile& aFile) const;
   
   @SYMTestExpectedResults Returns an integer value (99) if no exception occurs else return system wide errors.
    
 */	
void CT_CmdlnStep::testCmdLinesAPIsL()
	{
	INFO_PRINTF1(_L("Start test testCmdLinesAPIsL ..."));
	RFs fs;
	RFile file;	
	RProcess process; 
	TInt ret(0);
	_LIT(KFileName, "z:\\system\\data\\one_byte.txt"); //the file to be read by the process
	_LIT(KExeName, "TCmdLineExe"); //exe that is created for this test case
	TRequestStatus status;
   
	ret = fs.Connect();
	TEST(ret==KErrNone);	
	CleanupClosePushL(fs);
	ret = fs.ShareProtected();		
	TEST(ret==KErrNone);	
	ret = file.Open(fs, KFileName, EFileRead|EFileShareAny); //open for read
	TEST(ret==KErrNone);
	CleanupClosePushL(file);
	if(ret == KErrNone)		
		{
		CApaCommandLine* cmdLine=CApaCommandLine::NewLC();
		ret = process.Create(KExeName, KNullDesC);
		CleanupClosePushL(process);
		TEST(ret==KErrNone);
		if(ret==KErrNone)
			{
			TRAP(ret,cmdLine->SetFileByHandleL(file));
			TEST(ret==KErrNone);
			if(ret==KErrNone)
				{
				TRAP(ret,cmdLine->SetProcessEnvironmentL(process));	
				TEST(ret==KErrNone);
				if(ret==KErrNone)
					{
					status = KRequestPending;
					process.Logon(status);
					process.Resume();
					User::WaitForRequest(status); 	
					}
				TEST(status.Int()==KFileHandleTestPassed);
				}
			}	
		
		CleanupStack::PopAndDestroy(&process);
		CleanupStack::PopAndDestroy(cmdLine);
		}
	CleanupStack::PopAndDestroy(2);
	TEST(ret==KErrNone);	
	}


	
CT_CmdlnStep::~CT_CmdlnStep()
/**
   Destructor
 */
	{
	}

CT_CmdlnStep::CT_CmdlnStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_CmdlnStep);
	}

TVerdict CT_CmdlnStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_CmdlnStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}


// need to also test CApaCommandLine - SetServerNotRequiredL/SetServerRequiredL/ServerRequired

TVerdict CT_CmdlnStep::doTestStepL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
{
	INFO_PRINTF1(_L("Test setting full command lines"));

	// run the testcode (inside an alloc heaven harness)

	__UHEAP_MARK;
	TRAPD(ret,testSecureCmdLinesL());
	TEST(ret==KErrNone);

	TRAP(ret,testCmdLinesAPIsL());
	TEST(ret==KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("Test completed!"));
	return TestStepResult();
}
