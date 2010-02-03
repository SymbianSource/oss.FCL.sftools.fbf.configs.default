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
// Test Application for verification of environment slots\n
// Calls the EnvironmentSlotsReaderL function and verfies the CApaCommandLine object returned  \n
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#include "T_EnvSlots.H"

/**
  
   Application invoked as part of CommandLine tests
  
   @SYMPREQ 280 File Handle Support
  
   FunctionDesc Tests Environment slots . 
   Acquires a Mutex for log file access
   Invokes EnvironmentSlotsReaderL() which returns an CApaCommandLine Object
   Verifies the values returned by the GET APIs of CApaCommandLine object with those of predefined values
   Writes "Pass" to the logfile if verification passes else "Fail" is written.
  
 */
void testEnvironmentSlotsL()
	{

	TPtrC appName;
	TPtrC docName;
	TApaCommand command = EApaCommandOpen;
	TBool testResult = PASS;
	
		
	//Get the Mutex to access the log file
	RMutex fileAccess;
	fileAccess.OpenGlobal(KTLogFileAccess);
	fileAccess.Wait();
			
	/** Invoke EnvironmenstSlotsReaderL() function which would constuct the CApaCommandLine class object
	from the environment slots and return the pointer to that object */
	//CApaCommandLine* cmdLine = CApaCommandLine::EnvironmentSlotsReaderL(); 
	
	//Method CApaCommandLine::EnvironmentSlotsReaderL has been implemented in a different fashion
	CApaCommandLine* cmdLine;
	CApaCommandLine::GetCommandLineFromProcessEnvironment(cmdLine);

	CleanupStack::PushL(cmdLine);
    				
    appName.Set(KTAppName);
	docName.Set(KTDocName);
	
	RFs fSession;
	fSession.Connect();
	RFile logFile;
	TBufC<KMaxFilePath> testFilePath(KFilePath);
	
	//Open the Log file in Write Mode			
	User::LeaveIfError(logFile.Replace(fSession,testFilePath,EFileWrite));
		
	// Compare the values returned by GET APIs with pre defined values	
	TESTCOND(appName,cmdLine->ExecutableName());
	
	if(appName != cmdLine->ExecutableName())
	{
		logFile.Write(KTFail);
		logFile.Write(KTApp);
	}
		
	TESTCOND(docName,cmdLine->DocumentName());
	if(docName != cmdLine->DocumentName())
	{
		logFile.Write(KTFail);
		logFile.Write(KTDoc);
	}
	
    TESTCOND(command,cmdLine->Command());
	
	if(command != cmdLine->Command())
	{
		logFile.Write(KTFail);
		logFile.Write(KTCommand);
	}
	          
    if(testResult == PASS)
    	{
    	logFile.Write(KTPass);
    	}
  
    	
    //Close the file and the file session
    logFile.Close();
    fSession.Close();
    
    //Signal the Mutex
    fileAccess.Signal();  
    CleanupStack::PopAndDestroy(cmdLine); 

	}


GLDEF_C TInt E32Main()
/**
   EXE Entry Point
   @return - Standard Epoc error code on exit
 */
	{
	
	__UHEAP_MARK;
	
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	
	TRAPD(error,testEnvironmentSlotsL());
	
	__ASSERT_ALWAYS(!error,User::Panic(KTEnvSlots,error));
	
	delete cleanupStack;
	
	__UHEAP_MARKEND;
	
	return KErrNone;
	}
