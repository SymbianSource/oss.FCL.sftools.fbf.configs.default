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
// Test wrapper to Application Architecture Server.
// Tests Application information, Data recognition and application
// launching capabilities of the application architecture server.\n
// 
//



/**
 @file
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
#include "T_Serv3Step.h"

#include <coemain.h>
#include <eikenv.h>
#include "TAppEmbedUids.h"
#include "appfwk_test_utils.h"


//Literals for directory and file naming
_LIT(KMimeDir,"c:\\system\\data\\Mimepath\\");
_LIT(KMimeFile,"c:\\system\\data\\Mimepath\\mime_content");
_LIT(KEmptyDir,"c:\\system\\data\\Testpath\\Empty\\");
_LIT8(KTextContent, "Never hit your mother with a shovel.\
                     It leaves a dark impression on her mind.");
_LIT(KSuffixText, ".txt");
_LIT(KSuffixHtml, ".html");
_LIT8(KHtmlContent, "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0\
                     Transitional//EN'><HTML><HEAD><TITLE>SYMBIAN Centre\
                      - 5 Day Forecast for London, United Kingdom</TITLE>\
                      <META http-equiv=Content-Type content=text/html;\
                      charset=iso-8859-1><META content='Symbian Five Day Forecast'\
                      name=type><META content='The Symbian five day forecast'\
                      name=description>");
_LIT(KSuffixUnknown, ".canyoutell");

const TInt KFixedNumWidth = 3;

CT_Serv3Step::~CT_Serv3Step()
/**
 * Destructor
 */
	{
	delete iActiveScheduler;
	}

CT_Serv3Step::CT_Serv3Step()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_Serv3Step);

	//Set up active scheduler
	iActiveScheduler= new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	}

/**
 @SYMTestCaseID T_Serv3Step_DoSyncRecognizerTest1L

 @SYMPREQ 699

 @SYMREQ  3897

 @SYMTestCaseDesc Test PREQ699.1.0 Validate Synchronous RecognizeFilesL function and CDataRecognitionResultArray\n

 @SYMTestPriority High

 @SYMTestStatus Implemented

 @SYMTestActions. Calls RecognizeFilesL() with a folder containing two files of zero and one byte sizes. Validates
 the returned number of files and file types. Tests CDataRecognitionResultArray by using the object.
 API Calls:\n
 RApaLsSession::RecognizeFilesL(const TDesC& aPath, CDataRecognitionResultArray& aResult);\n
 CDataRecognitionResultArray::CDataRecognitionResultArray();
 CDataRecognitionResultArray::Count() const;\n
 CDataRecognitionResultArray::const TFileName& Path() const\n
 CDataRecognitionResultArray::GetDataRecognitionResultL(TDataRecognitionResult& aResult, const TUint aIndex) const;\n
 CDataRecognitionResultArray::GetFileNameL(TFileName& aFileName, const TUint aIndex) const\n

 @SYMTestExpectedResults KErrNone and CDataRecognitionResultArray returned containing two entries, of MIME type "text/plain" \n

 */
void CT_Serv3Step::DoSyncFolderRecognizerTest1L(RApaLsSession& aLs)
	{
	//Synchronous Folder File Recognition Test Group 1

	_LIT(KStandardDir,"z:\\system\\data\\Testpath\\");

 	// Copy App files around and delete them to check whether
	// the app list updates and stores the cache correctly.
	CDataRecognitionResultArray* result = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result);

	INFO_PRINTF1(_L("Synchronous Folder File Recognition Test PREQ699.1.0"));
	TInt ret=aLs.RecognizeFilesL(KStandardDir,*result);
	TEST(ret==KErrNone);
	User::LeaveIfError(ret);

	// Test the CDataRecognitionResultArray	object
	TFileName path = result->Path(); 	//test: CDataRecognitionResultArray::Path()
	TFullName standardDir(KStandardDir);
	//Test the path created is correct
	TEST(path==standardDir);

	TUint count = result->Count(); //test: CDataRecognitionResultArray::Count()
	TEST(count==2); //two files in directory

	//get each result from the test directory. Check the MIME values against their known types
	for(TUint i=0; i<count;i++)
		{
		//test: CDataRecognitionResultArray::GetDataRecognitionResultL
		TDataRecognitionResult res;
		result->GetDataRecognitionResultL(res, i);
		//test: correct MIME type is returned - "text/plain"
		TEST(res.iDataType.Des8().Compare(TDataType(_L8("text/plain")).Des8()) == 0);
		
		//test: CDataRecognitionResultArray::GetFileNameL
		TFileName fileName;
		result->GetFileNameL(fileName, i);
		TEST(fileName.Compare( (i==0 ? _L("file1.txt") : _L("file2.txt")) ) == 0);	
   		}

   	// Cleanup
	CleanupStack::PopAndDestroy(result);//result
 }

 /**
 @SYMTestCaseID T_Serv3Step_DoSyncRecognizerTest2L

 @SYMPREQ 699

 @SYMREQ  3899

 @SYMTestCaseDesc Test PREQ699.2.0 and PREQ699.2.1  Validate Synchronous RecognizeFilesL function for error handling.\n

 @SYMTestStatus Implemented

 @SYMTestPriority High

 @SYMTestActions. Test 2.0 Calls RecognizeFilesL() (without a data filter then with a data filter) using an unknown folder path.
				Checks returned value is KErrPathNotFound.
				Test 2.1 Calls RecognizeFilesL() (without a data filter then with a data filter) using an empty folder path.
				Checks returned value is KErrNone and that returned array is empty.

 API Calls:\n
 RApaLsSession::RecognizeFilesL(const TDesC& aPath, CDataRecognitionResultArray& aResult);\n
 RApaLsSession::RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult);\n
 CDataRecognitionResultArray::CDataRecognitionResultArray();
 CDataRecognitionResultArray::Count() const;\n

 @SYMTestExpectedResults 	Test 2.0 returned value is KErrPathNotFound.\n
						Test 2.1 returned value is KErrNone and that returned array is empty.\n
 */
 void CT_Serv3Step::DoSyncFolderRecognizerTest2L(RApaLsSession& aLs)
	{
	TBufC<50> emptyDir(KEmptyDir);
    TPtr ptrEmptyDir(emptyDir.Des());

    //Synchronous Folder File Recognition Test Group

   	//**************
    // Test Case 2.0 - Test Recognizer for incorrect folder path.Test both versions of synchronous RecognizeFiles
    //**************
    INFO_PRINTF1(_L("Synchronous Folder File Recognition Test PREQ699.2.0"));
    _LIT(KNotKnownDir,"z:\\system\\data\\Testpath\\NotKnown\\");

	CDataRecognitionResultArray* result1 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result1);

	TInt ret=aLs.RecognizeFilesL(KNotKnownDir,*result1);
	TEST(ret==KErrPathNotFound);

	ret=aLs.RecognizeFilesL(KNotKnownDir,_L8("text/plain"),*result1);
	TEST(ret==KErrPathNotFound);

	 // Cleanup
	CleanupStack::PopAndDestroy(result1);//result1

	// Create a directory with no files
	RSmlTestUtils utils;
	CleanupClosePushL(utils);

	CreateDirL(&utils, ptrEmptyDir);

	//**************
    // Test Case 2.1 -Test Recognizer for empty folder. Test both versions of synchronous RecognizeFiles
    //**************
    INFO_PRINTF1(_L("Synchronous Folder File Recognition Test PREQ699.2.1"));
	CDataRecognitionResultArray* result2 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result2);
	
	ret=aLs.RecognizeFilesL(KEmptyDir,*result2);
	TEST(ret==KErrNone);

	TUint count = result2->Count();
	TEST(count==0);

	INFO_PRINTF1(_L("Test empty folder for filter version parameter"));
	ret=aLs.RecognizeFilesL(KEmptyDir,_L8("text/plain"),*result2);
	TEST(ret==KErrNone);

	count = result2->Count();
	TEST(count==0);

	//Delete empty directory
	DeleteDirL(&utils, ptrEmptyDir);

	// Cleanup
	CleanupStack::PopAndDestroy(result2);//result2
	CleanupStack::PopAndDestroy(&utils);//utils
 }

 /**
 @SYMTestCaseID T_Serv3Step_DoSyncRecognizerTest3L

 @SYMPREQ 699

 @SYMREQ  3899

 @SYMTestCaseDesc Validate Synchronous RecognizeFilesL data filter function using various filters.\n
 		Test 3.0 Validate Synchronous RecognizeFilesL data filter function and CDataRecognitionResultArray\n
 		Test 3.1 Validate Synchronous RecognizeFilesL functions with wildcard/MIME type validity.\n
 		Test 3.2 Validate Synchronous RecognizeFilesL functions with irregular strings.\n

 @SYMTestPriority High

 @SYMTestStatus Implemented

 @SYMTestActions. Call RecognizeFilesL with three different data filters: i) standard= "text/plain"; ii) wildcard: "text*"; iii) non-valid \n
 				For each call, check the results are correct: number of files identified and the MIME types.

 API Calls:\n
 RApaLsSession::RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult);\n
 CDataRecognitionResultArray::CDataRecognitionResultArray();\n
 CDataRecognitionResultArray::Count() const;\n
 CDataRecognitionResultArray::const TFileName& Path() const\n
 CDataRecognitionResultArray::GetDataRecognitionResultL(TDataRecognitionResult& aResult, const TUint aIndex) const;\n
 CDataRecognitionResultArray::GetFileNameL(TFileName& aFileName, const TUint aIndex) const\n

 @SYMTestExpectedResults\n
 	Test 3.0 KErrNone and CDataRecognitionResultArray returned containing four entries, of MIME type "text/plain" \n
 	Test 3.1 KErrNone and CDataRecognitionResultArray returned containing four entries, of MIME type "text/plain" \n
 	Test 3.2 KErrNone and CDataRecognitionResultArray returned containing zero entries\n

 */
 void CT_Serv3Step::DoSyncFolderRecognizerTest3L(RApaLsSession& aLs)
	{

  	 //**************
    // Test Case 3.0 - Test Recognizer filter version with datatype filter type validity.
    // Also test CDataRecognitionResultArray::GetDataRecognitionResultL();
    //			 CDataRecognitionResultArray::
    //***************
    //Synchronous Folder File Recognition Test Group 3;
	_LIT(KFilterTestDir,"z:\\system\\data\\Testpath\\FilterTests\\");

	CDataRecognitionResultArray* result3 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result3);

    INFO_PRINTF1(_L("Synchronous Folder File Recognition Test PREQ699.3.0"));
	TInt ret=aLs.RecognizeFilesL(KFilterTestDir,_L8("text/plain"),*result3);
	TEST(ret==KErrNone);
	TUint count = result3->Count();
	TEST(count==4);

	//get each result from the test directory. Check the MIME values against known types
	for(TUint i=0; i<count;i++)
		{
		TDataRecognitionResult res;
		result3->GetDataRecognitionResultL(res, i);  //the result
		TFileName fileName;
		result3->GetFileNameL(fileName, i);       //the file we are looking at

		//test for correct MIME type result
		TEST(res.iDataType.Des8().Compare(TDataType(_L8("text/plain")).Des8())==0);//report an error if not text/plain
   		}

   	// Cleanup
	CleanupStack::PopAndDestroy(result3);//result

  	//*****************
    // Test Case 3.1 - Test Recognizer filter version for wildcard/MIME type validity.
    // Also test CDataRecognitionResultArray
    //*********
    INFO_PRINTF1(_L("Synchronous Folder File Recognition Test PREQ699.3.1"));
	CDataRecognitionResultArray* result4 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result4);
	ret=aLs.RecognizeFilesL(KFilterTestDir,_L8("text*"),*result4);
	TEST(ret==KErrNone);
	count = result3->Count();
	TEST(count==4);

	//get each result from the test directory. Check the MIME values against some known types
	for(TUint j=0; j<count;j++)
		{
		TDataRecognitionResult res;
		result4->GetDataRecognitionResultL(res, j);  //the result
		TFileName fileName;
		result4->GetFileNameL(fileName, j);       //the file we are looking at

		//test for correct MIME type result
		TEST(res.iDataType.Des8().Compare(TDataType(_L8("text/plain")).Des8())==0);//report an error if not text
   		}

   	// Cleanup
	CleanupStack::PopAndDestroy(result4);//result

    //*****************
    // Test Case 3.2 -Test Recognizer filter version for with irregular strings.
    //*********
    INFO_PRINTF1(_L("Synchronous Folder File Recognition Test PREQ699.3.2"));
	CDataRecognitionResultArray* result5 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result5);
	ret=aLs.RecognizeFilesL(KFilterTestDir,_L8("&*/?|=+-"),*result5);
	TEST(ret==KErrNone);
	count = result5->Count();
	TEST(count==0);

	ret=aLs.RecognizeFilesL(KFilterTestDir,_L8("'/"),*result5);
	TEST(ret==KErrNone);
	count = result5->Count();
	TEST(count==0);

   	// Cleanup
	CleanupStack::PopAndDestroy(result5);//result
 }

 /**
 @SYMTestCaseID T_Serv3Step_DoAsyncRecognizerTest1L

 @SYMPREQ 699

 @SYMREQ  Non-Specific

 @SYMTestCaseDesc Test PREQ699.4.0 Validate Asynchronous RecognizeFilesL (non-filtered) function and CDataRecognitionResultArray\n

 @SYMTestPriority High

 @SYMTestStatus Implemented

 @SYMTestActions. Calls RecognizeFilesL() with a folder containing two files of zero and one byte sizes. Uses a Test Active Object
 				within the request. Validates the returned number of files and file types. Tests CDataRecognitionResultArray by using the object.\n

 API Calls:\n
 RApaLsSession::RecognizeFilesL(const TDesC& aPath, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus);\n
 CDataRecognitionResultArray::CDataRecognitionResultArray();
 CDataRecognitionResultArray::Count() const;\n
 CDataRecognitionResultArray::const TFileName& Path() const\n
 CDataRecognitionResultArray::GetDataRecognitionResultL(TDataRecognitionResult& aResult, const TUint aIndex) const;\n
 CDataRecognitionResultArray::GetFileNameL(TFileName& aFileName, const TUint aIndex) const\n

 @SYMTestExpectedResults status= KErrNone and CDataRecognitionResultArray returned containing two entries, of MIME type "text/plain" \n

 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest1L(RApaLsSession& aLs)
	{

	//Asynchronous folder file recognition Test Group 4

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result);

	//Check results and Stop AS
	_LIT(KStandardDir,"z:\\system\\data\\Testpath\\");

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.4.0"));
	aLs.RecognizeFilesL(KStandardDir,*result,active->iStatus);
	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result->Count();

		TEST(count==2); //two files in directory

		//get each result from the test directory. Check the MIME values against some known types
		for(TUint i=0; i<count;i++)
			{
			TDataRecognitionResult res;
			result->GetDataRecognitionResultL(res, i);  // the result
			//test MIME type result = "text/plain"
			TEST(res.iDataType.Des8().Compare(TDataType(_L8("text/plain")).Des8())==0);
			
			TFileName fileName;
			result->GetFileNameL(fileName, i);       //the file we are looking at
			TEST(fileName.Compare( (i==0 ? _L("file1.txt") : _L("file2.txt")) ) == 0);
	   		}
		}

	// Cleanup
	CleanupStack::PopAndDestroy(result);//result
	CleanupStack::PopAndDestroy(active);//active
 }

 /**
 @SYMTestCaseID T_Serv3Step_DoAsyncRecognizerTest2L

 @SYMPREQ 699

 @SYMREQ  3899

 @SYMTestCaseDesc Test PREQ699.5.0 Validate Asynchronous RecognizeFilesL (filtered) function and CDataRecognitionResultArray\n

 @SYMTestPriority High

 @SYMTestStatus Implemented

 @SYMTestActions. Call RecognizeFilesL() with a Data Type Filter (aDataType ="Text/Plain") and a Test Active Object.
 				Test that the number and data type of files returned is correct.
 				Tests CDataRecognitionResultArray by using the object.\n

 API Calls:\n
 RApaLsSession::RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus);\n
 CDataRecognitionResultArray::CDataRecognitionResultArray();
 CDataRecognitionResultArray::Count() const;\n
 CDataRecognitionResultArray::GetDataRecognitionResultL(TDataRecognitionResult& aResult, const TUint aIndex) const;\n
 CDataRecognitionResultArray::GetFileNameL(TFileName& aFileName, const TUint aIndex) const\n

 @SYMTestExpectedResults status= KErrNone and CDataRecognitionResultArray returned containing four entries, of MIME type "text/plain" \n


 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest2L(RApaLsSession& aLs)
	{

    //Asynchronous folder file recognition Test Group 5

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result);

	_LIT(KFilterDir,"z:\\system\\data\\Testpath\\FilterTests\\");

    INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.5.0"));
    //Test RecognizeFilesL with a MIME type filter "text/plain"
 	aLs.RecognizeFilesL(KFilterDir,_L8("text/plain"), *result,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result->Count();

		TEST(count==4); //four files in directory

		//get each result from the test directory. Check the MIME values against some known types
		for(TUint i=0; i<count;i++)
			{
			TDataRecognitionResult res;
			result->GetDataRecognitionResultL(res, i);  // the result
			TFileName fileName;
			result->GetFileNameL(fileName, i);       // the file we are looking at

			//test MIME type result = "text/plain"
			TEST(res.iDataType.Des8().Compare(TDataType(_L8("text/plain")).Des8())==0);
	   		}
		}

	// Cleanup
	CleanupStack::PopAndDestroy(result);//result
	CleanupStack::PopAndDestroy(active);//active
 }

 /**
 @SYMTestCaseID T_Serv3Step_DoAsyncRecognizerTest3L

 @SYMPREQ 699

 @SYMREQ  3899

 @SYMTestCaseDesc Test PREQ699.6.0 Validate Asynchronous RecognizeFilesL function (with data filter and wildcard) \n

 @SYMTestPriority High

 @SYMTestStatus Implemented

 @SYMTestActions. Call RecognizeFilesL() with a Data Type Filter and wildcard (aDataType ="Text/ *") and a Test Active Object.
 				Test that the number and data type of files returned is correct.
 				Tests CDataRecognitionResultArray by using the object.\n

 API Calls:\n
 RApaLsSession::RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus);\n
 CDataRecognitionResultArray::CDataRecognitionResultArray();
 CDataRecognitionResultArray::Count() const;\n
 CDataRecognitionResultArray::GetDataRecognitionResultL(TDataRecognitionResult& aResult, const TUint aIndex) const;\n
 CDataRecognitionResultArray::GetFileNameL(TFileName& aFileName, const TUint aIndex) const\n

 @SYMTestExpectedResults status= KErrNone and CDataRecognitionResultArray returned containing four entries, of MIME type "text/plain" \n


 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest3L(RApaLsSession& aLs)
	{

	// Asynchronous folder file recognition Test Group 6

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result);

	_LIT(KFilterDir,"z:\\system\\data\\Testpath\\FilterTests\\");

    INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.6.0"));
	aLs.RecognizeFilesL(KFilterDir,_L8("text*"), *result,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);//Test no error returned
	if (active->iStatus==KErrNone)
		{
		TUint count = result->Count();

		TEST(count==4); //four files in directory

		//get each result from the test directory. Check the MIME values against some known types
		for(TUint i=0; i<count;i++)
			{
			TDataRecognitionResult res;
			result->GetDataRecognitionResultL(res, i);  // the result
			TFileName fileName;
			result->GetFileNameL(fileName, i);       //the file we are looking at

			//test MIME type result="text/plain"
			TEST(res.iDataType.Des8().Compare(TDataType(_L8("text/plain")).Des8())==0);
	   		}
		}

	// Cleanup
	CleanupStack::PopAndDestroy(result);//result
	CleanupStack::PopAndDestroy(active);//active
 }

  /**
 @SYMTestCaseID T_Serv3Step_DoAsyncRecognizerTest4L

 @SYMPREQ 699

 @SYMREQ  3899

 @SYMTestCaseDesc Test PREQ699.7.0 Validate Asynchronous RecognizeFilesL function (with unknown and irregular data filters).

 @SYMTestPriority High

 @SYMTestStatus Implemented

 @SYMTestActions. Call RecognizeFilesL() with Data Type Filters of an irregular nature and a Test Active Object.
 				Test that no files are returned and status= KErrNone.\n

 API Calls:\n
 RApaLsSession::RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus);\n
 CDataRecognitionResultArray::CDataRecognitionResultArray();
 CDataRecognitionResultArray::Count() const;\n

 @SYMTestExpectedResults status= KErrNone and CDataRecognitionResultArray returned containing zero entries \n

 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest4L(RApaLsSession& aLs)
	{

    //Asynchronous folder file recognition Test Group 7

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	INFO_PRINTF1(_L("Testing Recognition of folder filtered with irregular strings"));

	CDataRecognitionResultArray* result = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result);

	_LIT(KFilterDir,"z:\\system\\data\\Testpath\\FilterTests\\");

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.7.0"));
	aLs.RecognizeFilesL(KFilterDir,_L8("&* /?|=+-"),*result, active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result->Count();
		TEST(count==0); //no files should be found
		}

	// Cleanup
	CleanupStack::PopAndDestroy(result);//result
	CleanupStack::PopAndDestroy(active);//active

 }
 /**
 @SYMTestCaseID T_Serv3Step_DoAsyncRecognizerTest5L

 @SYMPREQ 699

 @SYMREQ  Non-Specific

 @SYMTestCaseDesc Test PREQ699.8.0 Validate Asynchronous RecognizeFilesL function with large directory to test Cache upperlimit. \n

 @SYMTestPriority High

 @SYMTestStatus Implemented

 @SYMTestActions. Call RecognizeFilesL() with a folder containing a large number of files and a Test Active Object.
 				Test that the number of files returned is correct.
 				Tests CDataRecognitionResultArray by using the object.\n

 API Calls:\n
 RApaLsSession::RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus);\n
 CDataRecognitionResultArray::CDataRecognitionResultArray();
 CDataRecognitionResultArray::Count() const;\n

 @SYMTestExpectedResults status= KErrNone and CDataRecognitionResultArray returned containing 550 entries. \n

 */

void CT_Serv3Step::DoAsyncFolderRecognizerTest5L(RApaLsSession& aLs)
	{
    _LIT(KLargeDir,"c:\\system\\data\\Testpath\\largeDir\\");
    _LIT(KLargeDirRoot, "c:\\system\\data\\Testpath\\largeDir\\focus");

	TBufC<50> bigDir(KLargeDir);
    TPtr ptrBigDir (bigDir.Des());
	TBufC<50> bigDirRoot(KLargeDirRoot);
    TPtr ptrBigDirRoot (bigDirRoot.Des());

	// Asynchronous folder file recognition Test Group 8

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result);

	//File server and utils
	RFs	theFS;
 	theFS.Connect();
 	RFile file;

	RSmlTestUtils utils;
	CleanupClosePushL(utils);

	CreateDirL(&utils, ptrBigDir);

	CreateFilesL(theFS, file, ptrBigDirRoot, (TDesC&)KSuffixText, 550, KTextContent, 0);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.8.0"));
	//Check results and Stop AS
	aLs.RecognizeFilesL(KLargeDir,*result,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result->Count();
		TEST(count==550); //550 files in directory - 500 files is thought to be cache capacity
		}

	DeleteFilesL(&utils, ptrBigDirRoot, (TDesC&)KSuffixText, 550);

	DeleteDirL(&utils, ptrBigDir);

	// Cleanup
	CleanupStack::PopAndDestroy(&utils);//utils
	CleanupStack::PopAndDestroy(result);//result
	CleanupStack::PopAndDestroy(active);//active
 }

 /**
 @SYMTestCaseID 	T_Serv3Step_DoAsyncRecognizerTest6L

 @SYMPREQ 699

 @SYMREQ  3898 3899

 @SYMTestCaseDesc	Test PREQ699.9.0, PREQ699.9.1, PREQ699.9.2 Validates cache functionality with changed file mime type

 @SYMTestPriority 	High

 @SYMTestStatus 	Implemented

 @SYMTestActions	Recognize a directory with a filter and then change the mime type of one file.
				Recognize the directory with a filter again and ensure that the file type change has been registered.

 API Calls:\n
 				RApaLsSession::RecognizeFilesL(const TDesC& aPath,
 												const TDesC8& aDataType,
 												CDataRecognitionResultArray& aResult,
												TRequestStatus& aStatus);
 				CDataRecognitionResultArray::CDataRecognitionResultArray();
 				CDataRecognitionResultArray::Count() const;

 @SYMTestExpectedResults\n
 				status = KErrNone for file creation
  				9.0 count  = 1 	CDataRecognitionResultArray::Count returns one entry of MIME type "text/plain"
				status = KErrNone for file content changed to html
				9.1 count  = 0 CDataRecognitionResultArray::Count returns no entries of MIME type "text/plain"
				9.2 count  = 0 CDataRecognitionResultArray::Count returns no entries of MIME type "text/plain"

 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest6L(RApaLsSession& aLs)
	{

	TBufC<50> mimeDir(KMimeDir);
    TPtr ptrMimeDir(mimeDir.Des());

	TBufC<50> mimeFile(KMimeFile);
    TPtr ptrMimeFile(mimeFile.Des());

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	//Asynchronous folder file recognition Test Group 9

	//Declare result arrays for storing seperate recognition results
	CDataRecognitionResultArray* result1 = new(ELeave) CDataRecognitionResultArray();
	CDataRecognitionResultArray* result2 = new(ELeave) CDataRecognitionResultArray();
	CDataRecognitionResultArray* result3 = new(ELeave) CDataRecognitionResultArray();

	CleanupStack::PushL(result1);
	CleanupStack::PushL(result2);
	CleanupStack::PushL(result3);

	//File server and utils
	RFs	theFS;
 	theFS.Connect();
 	RFile file;

	RSmlTestUtils utils;
	CleanupClosePushL(utils);

	//Create directory for test files
	CreateDirL(&utils, ptrMimeDir);

	//Timestamp details for files
	TTime time;
	time.UniversalTime();

	TTime aLaterTime(time);
	TTimeIntervalMinutes aInterval(1);

	aLaterTime+=(aInterval);

	// Create file mime_content000.txt containing text
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixText, 1, KTextContent, time);
    
	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.9.0"));
    //Test RecognizeFilesL with a MIME type filter "text/plain"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/plain"), *result1 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result1->Count();
		TEST(count==1);
		}

	//Change file content to html
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixText, 1, KHtmlContent, aLaterTime);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.9.1"));
    //Test RecognizeFilesL with a MIME type filter "text/plain"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/plain"), *result2 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result2->Count();
		TEST(count==0);
		}


	//Delete file and recognise again
	DeleteFilesL(&utils, ptrMimeFile, (TDesC&)KSuffixText, 1);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.9.2"));
 	aLs.RecognizeFilesL(KMimeDir, *result3,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result3->Count();
		TEST(count==0);
		}


	//Remove test directory
	DeleteDirL(&utils, ptrMimeDir);

	// Cleanup
	CleanupStack::PopAndDestroy(&utils); //utils
	CleanupStack::PopAndDestroy(result3);//result1
	CleanupStack::PopAndDestroy(result2);//result2
	CleanupStack::PopAndDestroy(result1);//result3
	CleanupStack::PopAndDestroy(active); //active

 }

 /**
 @SYMTestCaseID	T_Serv3Step_DoAsyncRecognizerTest7L

 @SYMPREQ 699

 @SYMREQ  3898 3899

 @SYMTestCaseDesc 	Test PREQ699.10.0 Validates cache functionality with changed mime type and filter (1)

 @SYMTestPriority 	High

 @SYMTestStatus 	Implemented

 @SYMTestActions	Recognize a directory with a filter. Change the mime type of one file in the
				directory such that it is now described by the filter.  Recognize the
				directory again with the same filter and ensure that the file type change has
				been registered.

 API Calls:\n
 				RApaLsSession::RecognizeFilesL(const TDesC& aPath,
 												const TDesC8& aDataType,
 												CDataRecognitionResultArray& aResult,
												TRequestStatus& aStatus);
 				CDataRecognitionResultArray::CDataRecognitionResultArray();
 				CDataRecognitionResultArray::Count() const;

 @SYMTestExpectedResults\n
 				status = KErrNone for file creation
  				count  = 3 	CDataRecognitionResultArray::Count returns three entries of MIME type "text/html"
				status = KErrNone for file content changed to html
				count  = 4 CDataRecognitionResultArray::Count returns four entries of MIME type "text/html"
 */
 void CT_Serv3Step::DoAsyncFolderRecognizerTest7L(RApaLsSession& aLs)
	{
	TBufC<50> mimeDir(KMimeDir);
    TPtr ptrMimeDir (mimeDir.Des());

	TBufC<50> mimeFile(KMimeFile);
    TPtr ptrMimeFile(mimeFile.Des());

	//Asynchronous folder file recognition Test Group 10

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result1 = new(ELeave) CDataRecognitionResultArray();
	CDataRecognitionResultArray* result2 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result1);
	CleanupStack::PushL(result2);


	RFs	theFS;
 	theFS.Connect();
 	RFile file;

	RSmlTestUtils utils;
	CleanupClosePushL(utils);

	//Create directory for test files
	CreateDirL(&utils, ptrMimeDir);

	//Timestamp details for files
	TTime time;
	time.UniversalTime();

	TTime aLaterTime(time);
	TTimeIntervalMinutes aInterval(1);
	aLaterTime+=(aInterval);

	// Create files mime_content000.canyoutell - mime_content003.canyoutell
	// 000 with text content and 001-003 with html content
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 4, KHtmlContent, time);
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 1, KTextContent, time);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.10.0"));
    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/html"), *result1 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result1->Count();
		TEST(count == 3);
		}


	//Change file content to html and update the timestamp
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 1, KHtmlContent, aLaterTime);

    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/html"), *result2 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result2->Count();
		TEST(count == 4);
		}


	DeleteFilesL(&utils, ptrMimeFile, (TDesC&)KSuffixUnknown, 4);

	//Remove test directory
	DeleteDirL(&utils, ptrMimeDir);

	// Cleanup
	CleanupStack::PopAndDestroy(&utils);//utils
	CleanupStack::PopAndDestroy(result2);//result1
	CleanupStack::PopAndDestroy(result1);//result2
	CleanupStack::PopAndDestroy(active);//active

 }

 /**
 @SYMTestCaseID	T_Serv3Step_DoAsyncRecognizerTest8L

 @SYMPREQ 699

 @SYMREQ  3898 3899

 @SYMTestCaseDesc	Test PREQ699.11.0 Validates cache functionality with changed file mime type and filter (2)

 @SYMTestPriority 	High

 @SYMTestStatus 	Implemented

 @SYMTestActions	Recognize a directory with a filter. Change the mime type of one file in the
				directory such that it is now not described by the filter.  Recognize the
				directory again with the same filter and ensure that the file type change has
				been registered.

 API Calls:\n
 				RApaLsSession::RecognizeFilesL(const TDesC& aPath,
 												const TDesC8& aDataType,
 												CDataRecognitionResultArray& aResult,
												TRequestStatus& aStatus);
 				CDataRecognitionResultArray::CDataRecognitionResultArray();
 				CDataRecognitionResultArray::Count() const;

 @SYMTestExpectedResults\n
 				status = KErrNone for file creation
  				count  = 4 	CDataRecognitionResultArray::Count returns four entries of MIME type "text/html"
				status = KErrNone for file content changed to html
				count  = 3 CDataRecognitionResultArray::Count returns three entries of MIME type "text/html"
 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest8L(RApaLsSession& aLs)
	{
	TBufC<50> mimeDir(KMimeDir);
    TPtr ptrMimeDir (mimeDir.Des());

	TBufC<50> mimeFile(KMimeFile);
    TPtr ptrMimeFile(mimeFile.Des());

	//Asynchronous folder file recognition Test Group 11

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result1 = new(ELeave) CDataRecognitionResultArray();
	CDataRecognitionResultArray* result2 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result1);
	CleanupStack::PushL(result2);


	RFs	theFS;
 	theFS.Connect();
 	RFile file;

	RSmlTestUtils utils;
	CleanupClosePushL(utils);

	//Create directory for test files
	CreateDirL(&utils, ptrMimeDir);

	//Timestamp details for files
	TTime time;
	time.UniversalTime();

	TTime aLaterTime(time);
	TTimeIntervalMinutes aInterval(1);
	aLaterTime+=(aInterval);

	// Create files mime_content000.canyoutell - mime_content003.canyoutell with html content
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 4, KHtmlContent, time);

    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/html"), *result1 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result1->Count();
		TEST(count == 4);
		}


	//Change file content to text
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 1, KTextContent, aLaterTime);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.11.0"));
    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/html"), *result2 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result2->Count();
		TEST(count == 3);
		}

	DeleteFilesL(&utils, ptrMimeFile, (TDesC&)KSuffixUnknown, 4);

	//Remove test directory
	DeleteDirL(&utils, ptrMimeDir);

	// Cleanup
	CleanupStack::PopAndDestroy(&utils);//utils
	CleanupStack::PopAndDestroy(result2);//result1
	CleanupStack::PopAndDestroy(result1);//result2
	CleanupStack::PopAndDestroy(active);//active

 }

 /**
 @SYMTestCaseID	T_Serv3Step_DoAsyncRecognizerTest9L

 @SYMPREQ 699

 @SYMREQ  3898 3899

 @SYMTestCaseDesc	Test PREQ699.12.0 Validates cache functionality with older timestamp(1)

 @SYMTestPriority 	High

 @SYMTestStatus 	Implemented

 @SYMTestActions	Recognize a directory. Change the timestamp on one file to be older and alter
				the mime type. Repeat the directory recognition and ensure that the mime type
				has been updated..

 API Calls:\n
 				RApaLsSession::RecognizeFilesL(const TDesC& aPath,
 												const TDesC8& aDataType,
 												CDataRecognitionResultArray& aResult,
												TRequestStatus& aStatus);
 				CDataRecognitionResultArray::CDataRecognitionResultArray();
 				CDataRecognitionResultArray::Count() const;

 @SYMTestExpectedResults\n
 				status = KErrNone for html file creation
  				count  = 1 	CDataRecognitionResultArray::Count returns one entry of MIME type "text/html"
				status = KErrNone for file content changed to text
				count  = 0 CDataRecognitionResultArray::Count returns zero entries of MIME type "text/html"
 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest9L(RApaLsSession& aLs)
	{
	TBufC<50> mimeDir(KMimeDir);
    TPtr ptrMimeDir (mimeDir.Des());

	TBufC<50> mimeFile(KMimeFile);
    TPtr ptrMimeFile(mimeFile.Des());

	//Asynchronous folder file recognition Test Group 12

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result1 = new(ELeave) CDataRecognitionResultArray();
	CDataRecognitionResultArray* result2 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result1);
	CleanupStack::PushL(result2);

	RFs	theFS;
 	theFS.Connect();
 	RFile file;

	RSmlTestUtils utils;
	CleanupClosePushL(utils);

	//Create directory for test files
	CreateDirL(&utils, ptrMimeDir);

	//Timestamp details for files
	TTime time;
	time.UniversalTime();

	TTime aLaterTime(time);
	TTimeIntervalMinutes aInterval(1);
	aLaterTime+=(aInterval);

	//Set the directory timestamp to be 'new'
	theFS.SetModified(ptrMimeDir, aLaterTime);

	// Create file mime_content000.canyoutell with html content and 'new' time
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 1, KHtmlContent, aLaterTime);

    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/html"), *result1 ,active->iStatus);
	active->StartL();//set active
 	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result1->Count();
		TEST(count == 1);
		}


	//Change file content to text and set the timestamp to 'old'
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 1, KTextContent, time);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.12.0"));
    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/html"), *result2 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result2->Count();
		TEST(count == 0);
		}

	//Delete file
	utils.DeleteFileL(_L("c:\\system\\data\\Mimepath\\mime_content000.canyoutell"));

	//Remove test directory
	DeleteDirL(&utils, ptrMimeDir);

	// Cleanup
	CleanupStack::PopAndDestroy(&utils);//utils
	CleanupStack::PopAndDestroy(result2);//result1
	CleanupStack::PopAndDestroy(result1);//result2
	CleanupStack::PopAndDestroy(active);//active

 }

/**
 @SYMTestCaseID	T_Serv3Step_DoAsyncRecognizerTest10L

 @SYMPREQ 699

 @SYMREQ  3898 3899

 @SYMTestCaseDesc	Test PREQ699.13.0 Validates cache functionality with older timestamp.

 @SYMTestPriority 	High

 @SYMTestStatus 	Implemented

 @SYMTestActions	Recognize a directory. Change the timestamp on one file to be older and alter
				the mime type. Change the timestamp on the directory to be newer. Repeat the
				directory recognition and ensure that the mime type has been updated..

 API Calls:\n
 				RApaLsSession::RecognizeFilesL(const TDesC& aPath,
 												const TDesC8& aDataType,
 												CDataRecognitionResultArray& aResult,
												TRequestStatus& aStatus);
 				CDataRecognitionResultArray::CDataRecognitionResultArray();
 				CDataRecognitionResultArray::Count() const;

 @SYMTestExpectedResults\n
 				status = KErrNone for html file creation
  				count  = 1 	CDataRecognitionResultArray::Count returns one entry of MIME type "text/html"
				status = KErrNone for file content changed to text
				count  = 0 CDataRecognitionResultArray::Count returns zero entries of MIME type "text/html"
 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest10L(RApaLsSession& aLs)
	{
	TBufC<50> mimeDir(KMimeDir);
    TPtr ptrMimeDir (mimeDir.Des());

	TBufC<50> mimeFile(KMimeFile);
    TPtr ptrMimeFile(mimeFile.Des());

	//Asynchronous folder file recognition Test Group 13

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result1 = new(ELeave) CDataRecognitionResultArray();
	CDataRecognitionResultArray* result2 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result1);
	CleanupStack::PushL(result2);

	RFs	theFS;
 	theFS.Connect();
 	RFile file;

	RSmlTestUtils utils;
	CleanupClosePushL(utils);

	//Create directory for test files
	CreateDirL(&utils, ptrMimeDir);

	//Timestamp details for files
	TTime time;
	time.UniversalTime();

	TTime aLaterTime(time);
	TTimeIntervalMinutes aInterval(1);
	aLaterTime+=(aInterval);

	//Set the directory timestamp to be 'old'
	theFS.SetModified(ptrMimeDir, time);

	// Create file mime_content000.canyoutell with html content and 'old' time
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 1, KHtmlContent, time);

    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/html"), *result1 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result1->Count();
		TEST(count == 1);
		}


	//Change file content to text and set the timestamp to 'new'
	//Change dir timestamp to 'old'
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 1, KTextContent, aLaterTime);

	//Set the directory timestamp to be 'new'
	theFS.SetModified(ptrMimeDir, aLaterTime);


	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.13.0"));
    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/html"), *result2 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result2->Count();
		TEST(count == 0);
		}

	//Delete file
	utils.DeleteFileL(_L("c:\\system\\data\\Mimepath\\mime_content000.canyoutell"));

	//Remove test directory
	DeleteDirL(&utils, ptrMimeDir);

	// Cleanup
	CleanupStack::PopAndDestroy(&utils);//utils
	CleanupStack::PopAndDestroy(result2);//result1
	CleanupStack::PopAndDestroy(result1);//result2
	CleanupStack::PopAndDestroy(active);//active

 }

/**
 @SYMTestCaseID	T_Serv3Step_DoAsyncRecognizerTest11L

 @SYMPREQ 699

 @SYMREQ  3989 3899

 @SYMTestCaseDesc	Test PREQ699.14.0 Validates cache functionality with substituted directory

 @SYMTestPriority 	High

 @SYMTestStatus 	Implemented

 @SYMTestActions	Recognize a directory. Substitute a directory of the same name with
 				different contents and repeat the recognition. Ensure that the file
 				contents are correctly listed.

 API Calls:\n
 				RApaLsSession::RecognizeFilesL(const TDesC& aPath,
 												const TDesC8& aDataType,
 												CDataRecognitionResultArray& aResult,
												TRequestStatus& aStatus);
 				CDataRecognitionResultArray::CDataRecognitionResultArray();
 				CDataRecognitionResultArray::Count() const;

 @SYMTestExpectedResults\n
 				status = KErrNone for file creation
  				count  = 4 	CDataRecognitionResultArray::Count returns four entries of MIME type "text/html"
				status = KErrNone for file content changed to text
				count  = 2 CDataRecognitionResultArray::Count returns two entries of MIME type "text/html"
 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest11L(RApaLsSession& aLs)
	{
	_LIT(KMimeDir1,"c:\\system\\data\\Mimepath1\\");
	_LIT(KMimeDir2,"c:\\system\\data\\Mimepath2\\");

	_LIT(KMimeDir1Html,"c:\\system\\data\\Mimepath1\\dir1_html");
	_LIT(KMimeDir1Text,"c:\\system\\data\\Mimepath1\\dir1_text");
	_LIT(KMimeDir2Html,"c:\\system\\data\\Mimepath2\\dir2_html");
	_LIT(KMimeDir2Text,"c:\\system\\data\\Mimepath2\\dir2_text");

	//After directory renaming
	_LIT(KRenameHtml,"c:\\system\\data\\Mimepath1\\dir2_html");
	_LIT(KRenameText,"c:\\system\\data\\Mimepath1\\dir2_text");

	TBufC<50> mimeDir1(KMimeDir1);
    TPtr ptrMimeDir1 (mimeDir1.Des());

	TBufC<50> mimeDir2(KMimeDir2);
    TPtr ptrMimeDir2 (mimeDir2.Des());

	TBufC<50> mimeDir1Html(KMimeDir1Html);
    TPtr ptrFile1x4(mimeDir1Html.Des());

	TBufC<50> mimeDir1Text(KMimeDir1Text);
    TPtr ptrFile1x2(mimeDir1Text.Des());

	TBufC<50> mimeDir2Html(KMimeDir2Html);
    TPtr ptrFile2x2(mimeDir2Html.Des());

	TBufC<50> mimeDir2Text(KMimeDir2Text);
    TPtr ptrFile2x4(mimeDir2Text.Des());

	TBufC<50> renameText(KRenameText);
    TPtr ptrRenameText(renameText.Des());

	TBufC<50> renameHtml(KRenameHtml);
    TPtr ptrRenameHtml(renameHtml.Des());

	//"Asynchronous folder file recognition Test Group 14

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result1 = new(ELeave) CDataRecognitionResultArray();
	CDataRecognitionResultArray* result2 = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result1);
	CleanupStack::PushL(result2);

	RFs	theFS;
 	theFS.Connect();
 	RFile file;

	RSmlTestUtils utils;
	CleanupClosePushL(utils);

	//Create directories for test files
	CreateDirL(&utils, ptrMimeDir1);
	CreateDirL(&utils, ptrMimeDir2);

	//Timestamp details for files
	TTime time;
	time.UniversalTime();

	TTime aLaterTime(time);
	TTimeIntervalMinutes aInterval(1);
	aLaterTime+=(aInterval);

	// Fill the first directory with files
	// Create files dir1_html000.html - dir1_html003.html with html content
	// Create files dir1_text000.txt - dir1_text001.txt with text content
	CreateFilesL(theFS, file, ptrFile1x4, (TDesC&)KSuffixHtml, 4, KHtmlContent, time);
	CreateFilesL(theFS, file, ptrFile1x2, (TDesC&)KSuffixText, 2, KTextContent, time);

	// Fill the second directory with files
	// Create files dir2_text000.txt - dir2_text003.txt with text content
	// Create files dir2_html001.html - dir2_html002.html with html content
	CreateFilesL(theFS, file, ptrFile2x4, (TDesC&)KSuffixText, 4, KTextContent, time);
	CreateFilesL(theFS, file, ptrFile2x2, (TDesC&)KSuffixHtml, 2, KHtmlContent, time);

    //Test RecognizeFilesL on first directory with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir1,_L8("text/html"), *result1 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result1->Count();
		TEST(count == 4);
		}


	//Delete first directory (KMimeDir1) and rename second directory to be the
	//first (KMimeDir2 -> KMimeDir1)
	DeleteFilesL(&utils, ptrFile1x4, (TDesC&)KSuffixHtml, 4);
	DeleteFilesL(&utils, ptrFile1x2, (TDesC&)KSuffixText, 2);
	DeleteDirL(&utils, ptrMimeDir1);
 	utils.RenameDirectoryL(mimeDir2, mimeDir1);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.14.0"));
    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir1,_L8("text/html"), *result2 ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result2->Count();
		TEST(count == 2);
		}

	//Bin out all the remaining created files
	DeleteFilesL(&utils, ptrRenameText, (TDesC&)KSuffixText, 4);
	DeleteFilesL(&utils, ptrRenameHtml, (TDesC&)KSuffixHtml, 2);

	//Remove test directory
	DeleteDirL(&utils, ptrMimeDir1);

	// Cleanup
	CleanupStack::PopAndDestroy(4, active); // utils, result2, result1, active

 }

/**
 @SYMTestCaseID	T_Serv3Step_DoSyncRecognizerTest4L

 @SYMPREQ 699

 @SYMREQ  3898

 @SYMTestCaseDesc	Test PREQ699.15.0, PREQ699.15.1 Validates cache performance with a typical directory

 @SYMTestPriority 	High

 @SYMTestStatus 	Implemented

 @SYMTestActions	1st Recognition - Recognize a directory that has significant file content
 					with the cache enabled and log the time taken.
					15.0 Repeat the recognition on the directory, it should be faster than
					the first recognition.
					15.1 Alter a number of files and repeat, again logging the time. The time
					taken should be faster than the 1st recognition, but slower than test 15.0


 API Calls:\n
 				RApaLsSession::RecognizeFilesL(const TDesC& aPath,
 												const TDesC8& aDataType,
 												CDataRecognitionResultArray& aResult,
												TRequestStatus& aStatus);
 				CDataRecognitionResultArray::CDataRecognitionResultArray();
 				CDataRecognitionResultArray::Count() const;

 @SYMTestExpectedResults\n
 				status = KErrNone for file creation
				15.0 Time taken faster than 1st recognition
				15.1 Time taken faster than 1st recognition, but slower than 15.0
 */
void CT_Serv3Step::DoSyncFolderRecognizerTest4L(RApaLsSession& aLs)
{
	//Synchronous folder file recognition Test Group 15
	_LIT(KTestAppSource, "Z:\\System\\data\\" );
 	_LIT(KTestAppDest, "C:\\System\\data\\TestPath\\" );
 	_LIT(KEmpty, "");

 	_LIT(KTestWaitingForApplistUpdate,"\nWaiting %d microseconds for applist to be updated");
 	const TInt KApplistUpdateTime = 8000000;

	TBufC<128> fileDir(KEmpty);
    TPtr ptrFileDir(fileDir.Des());

	CDataRecognitionResultArray* result1 = new(ELeave) CDataRecognitionResultArray();
	CDataRecognitionResultArray* result2 = new(ELeave) CDataRecognitionResultArray();
	CDataRecognitionResultArray* result3 = new(ELeave) CDataRecognitionResultArray();

	CleanupStack::PushL(result1);
	CleanupStack::PushL(result2);
	CleanupStack::PushL(result3);

 	RFs	theFS;
 	theFS.Connect();
 	RFile file;

 	// Remove Test app from the file system
 	CFileMan* fileManager = CFileMan::NewL (theFS);

 	INFO_PRINTF1(_L("Copying the app to C"));
 	TEST(KErrNone == fileManager->Copy (KTestAppSource, KTestAppDest, CFileMan::EOverWrite ));

 	INFO_PRINTF2(KTestWaitingForApplistUpdate, KApplistUpdateTime);
 	User::After(KApplistUpdateTime);

	TTime time;
	TTime timeLater;

	//1st recognition
	time.HomeTime();
	TInt ret=aLs.RecognizeFilesL(KTestAppDest,*result1);
	timeLater.HomeTime();
	TEST(ret==KErrNone);
	User::LeaveIfError(ret);

	TTimeIntervalMicroSeconds delayNoCache = timeLater.MicroSecondsFrom(time);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.15.0"));
	time.HomeTime();
	ret=aLs.RecognizeFilesL(KTestAppDest,*result2);
	timeLater.HomeTime();
	TEST(ret==KErrNone);
	User::LeaveIfError(ret);

	TTimeIntervalMicroSeconds delayCache = timeLater.MicroSecondsFrom(time);

	//Modify the timestamps on a few files
	TInt count = result1->Count();
	for(TInt i=1; i < (count/2); i++)
		{
		TDataRecognitionResult res;
		result1->GetDataRecognitionResultL(res, i);
		TFileName fileName;
		result1->GetFileNameL(fileName, i);

		ptrFileDir.Copy(KTestAppDest);
		ptrFileDir.Append(fileName);

   		time.HomeTime();
   		theFS.SetModified(ptrFileDir, time);
   		}

 	INFO_PRINTF2(KTestWaitingForApplistUpdate, KApplistUpdateTime);
 	User::After(KApplistUpdateTime);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.15.1"));
	time.HomeTime();
	ret=aLs.RecognizeFilesL(KTestAppDest,*result3);
	timeLater.HomeTime();
	TEST(ret==KErrNone);
	User::LeaveIfError(ret);

	TTimeIntervalMicroSeconds delayCache2 = timeLater.MicroSecondsFrom(time);
#ifndef __WINS__ 
	// On Emulator, these time related checks cannot always be ensured.
	TEST(delayCache2 > delayCache);
	TEST(delayCache2 < delayNoCache);
#endif

   	// Cleanup
   	delete fileManager;
	CleanupStack::PopAndDestroy(3, result1); // result3, result2, result1
}

/**
 @SYMTestCaseID	T_Serv3Step_DoAsyncRecognizerTest13L

 @SYMPREQ 699

 @SYMREQ  3899

 @SYMTestCaseDesc	Test PREQ699.17.0 Validates performance of recognition.

 @SYMTestPriority 	High

 @SYMTestStatus 	Not Implemented

 @SYMTestActions	Call RecognizeFilesL() without a filter on a directory
 				containing a text file named text.canyoutell

 API Calls:\n
 				RApaLsSession::RecognizeFilesL(const TDesC& aPath,
 												const TDesC8& aDataType,
 												CDataRecognitionResultArray& aResult,
												TRequestStatus& aStatus);
 				CDataRecognitionResultArray::CDataRecognitionResultArray();
 				CDataRecognitionResultArray::Count() const;

 @SYMTestExpectedResults\n
 				status = KErrNone for file creation
				count = 1 returned
 */
void CT_Serv3Step::DoAsyncFolderRecognizerTest13L(RApaLsSession& aLs)
{
	//Asynchronous folder file recognition Test Group 17
	TBufC<50> mimeDir(KMimeDir);
    TPtr ptrMimeDir (mimeDir.Des());

	TBufC<350> mimeFile(KMimeFile);
    TPtr ptrMimeFile(mimeFile.Des());

	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result);

	RFs	theFS;
 	theFS.Connect();
 	RFile file;

	RSmlTestUtils utils;
	CleanupClosePushL(utils);

	//Create directory for test files
	CreateDirL(&utils, ptrMimeDir);

	//Timestamp details for files
	TTime time;
	time.UniversalTime();

	// Create file mime_content000.canyoutell  with text content
	CreateFilesL(theFS, file, ptrMimeFile, (TDesC&)KSuffixUnknown, 1, KHtmlContent, time);

	INFO_PRINTF1(_L("Asynchronous folder file Recognition Test PREQ699.17.0"));
    //Test RecognizeFilesL with a MIME type filter "text/html"
 	aLs.RecognizeFilesL(KMimeDir,_L8("text/html"), *result ,active->iStatus);

	active->StartL();//set active
	CActiveScheduler::Start();

	TEST(active->iStatus==KErrNone);
	if (active->iStatus==KErrNone)
		{
		TUint count = result->Count();
		TEST(count == 1);
		}

	DeleteFilesL(&utils, ptrMimeFile, (TDesC&)KSuffixUnknown, 1);

	//Remove test directory
	DeleteDirL(&utils, ptrMimeDir);

	// Cleanup
	CleanupStack::PopAndDestroy(&utils);//utils
	CleanupStack::PopAndDestroy(result);//result2
	CleanupStack::PopAndDestroy(active);//active

}
/*

@SYMTestCaseID 		APPFWK-APPARC-0098 

@SYMDEF 			INC125788   

@SYMTestCaseDesc 	The test verify that RApaLsSession::CancelRecognizeFiles() complete TRequestStatus with KErrCancel 
					that was passed to RApaLsSession::RecognizeFilesL() when cancelling the outstanding async recognition request. 

@SYMTestPriority 	High   

@SYMTestStatus 		Implemented

@SYMTestActions 	
1.RApaLsSession::RecognizeFilesL(const TDesC& aPath, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus)
is called to get data (MIME) types for files in a specified directory.
2.Outstanding async recognition request is cancelled using RApaLsSession::CancelRecognizeFiles().

@SYMTestExpectedResults
RApaLsSession::CancelRecognizeFiles() complete the TRequestStatus with KErrCancel.
 
*/
void CT_Serv3Step::DoAsyncFolderRecognizerTest14L(RApaLsSession& aLs)
	{
	//Setup Active Object
	CActiveTest2* active = new(ELeave) CActiveTest2();
	CleanupStack::PushL(active);

	CDataRecognitionResultArray* result = new(ELeave) CDataRecognitionResultArray();
	CleanupStack::PushL(result);

	//Check results and Stop AS
	_LIT(KStandardDir,"z:\\system\\data\\Testpath\\");

	INFO_PRINTF1(_L("Asynchronous RecognizeFilesL Test"));
	aLs.RecognizeFilesL(KStandardDir,*result,active->iStatus);
	active->StartL();//set active
	TEST(active->iStatus==KRequestPending);
	INFO_PRINTF2(_L("RecognizeFilesL: iStatus = %d\n"), active->iStatus.Int());
	
	aLs.CancelRecognizeFiles();
	TEST(active->iStatus==KErrCancel);
	INFO_PRINTF2(_L("CancelRecognizeFiles: iStatus = %d\n"), active->iStatus.Int());
	CActiveScheduler::Start();
	
	// Cleanup
	CleanupStack::PopAndDestroy(result);//result
	CleanupStack::PopAndDestroy(active);//active
	}
/**
* Auxiliary Fn for the entire Test Step
*
* This method initiates all the tests.
*
*/
TInt CT_Serv3Step::DoServComTestL()
	{
	INFO_PRINTF1(_L("Testing Server Com"));

	RFs theServFs;
	TInt ret = theServFs.Connect();
	TEST(ret==KErrNone);
	CleanupClosePushL(theServFs);


    RTestableApaLsSession ls;
	ret = ls.Connect();
	TEST(ret==KErrNone);
	CleanupClosePushL(ls);
	
	//wait for CExtrasAppKeyLayout to open up a RApaLsSession so it doesn't mess with our result
	//unsure why it affects this test but not the others
	User::After(10000000);

	HEAP_TEST_LS_SESSION(ls, 0, 0, DoSyncFolderRecognizerTest1L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoSyncFolderRecognizerTest2L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoSyncFolderRecognizerTest3L(ls), ls.FlushRecognitionCache() );
	
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest1L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest2L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest3L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest4L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest5L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest6L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest7L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest8L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest9L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest10L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest11L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoSyncFolderRecognizerTest4L(ls), ls.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(ls, 0, 0, DoAsyncFolderRecognizerTest13L(ls), ls.FlushRecognitionCache() );
	//DONT_CHECK Skips the heap check at server side. This heap imbalance occurs randomly in server side
	// while canceling the outstanding async request.
	HEAP_TEST_LS_SESSION(ls, 0, DONT_CHECK, DoAsyncFolderRecognizerTest14L(ls), ls.FlushRecognitionCache() );
			
	CleanupStack::PopAndDestroy(&ls);

	// close the server session and we have done some type store reloading
	CleanupStack::PopAndDestroy(&theServFs);

	return KErrNone;
	}



TVerdict CT_Serv3Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_Serv3Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

void CT_Serv3Step::CreateDirL(RSmlTestUtils* aPtrUtils, TPtr aDirName)
	{
	User::LeaveIfError(aPtrUtils->Connect());
	aPtrUtils->CreateDirectoryL(aDirName);

 	//Delay required to prevent armv5 failure
  	const TInt KApplistUpdateTime = 800000;
  	User::After(KApplistUpdateTime);
	}

void CT_Serv3Step::CreateFilesL(RFs& aFS, RFile& aFile, TPtr aFileRoot, TDesC& aSuffix, TInt aFileNumber, const TDesC8& aFileContent, TTime aTime = 0)
	{
	if(aTime == 0)
		{
		aTime.UniversalTime();
		}

	for(TInt counter=0; counter<aFileNumber; counter++)
		{
		aFileRoot.AppendNumFixedWidth(counter, EDecimal, KFixedNumWidth);
    	aFileRoot.Append(aSuffix);

		//Creates a file if does not already exist. If file exists,
		//content is overwritten
		TEST(KErrNone == aFile.Replace(aFS, aFileRoot, EFileWrite));

		TEST(KErrNone == aFile.Write(aFileContent));
		TEST(KErrNone == aFile.SetModified(aTime));
   		TEST(KErrNone == aFile.Flush());
    	aFile.Close();

		TInt Length = aFileRoot.Length();
		TInt DeleteLength = aSuffix.Length() + KFixedNumWidth;
		aFileRoot.Delete( (Length - DeleteLength) , DeleteLength);
		}

 	//Delay required to prevent armv5 failure
  	const TInt KApplistUpdateTime = 800000;
  	User::After(KApplistUpdateTime);
	}

void CT_Serv3Step::DeleteDirL(RSmlTestUtils* aPtrUtils, TPtr aDirName)
	{
	aPtrUtils->DeleteDirectoryL(aDirName);
	}

void CT_Serv3Step::DeleteFilesL(RSmlTestUtils* aPtrUtils, TPtr aDirName, TDesC& aSuffix, TInt aFileNumber)
	{
	for(TInt counter=0; counter<aFileNumber; counter++)
		{
		aDirName.AppendNumFixedWidth(counter, EDecimal, KFixedNumWidth);
    	aDirName.Append(aSuffix);
		aPtrUtils->DeleteFileL(aDirName);
		TInt Length = aDirName.Length();
		TInt DeleteLength = aSuffix.Length() + KFixedNumWidth;
		aDirName.Delete( (Length - DeleteLength) , DeleteLength);
		}
	}

TVerdict CT_Serv3Step::doTestStepL()
/**
 * Override of base class virtual.
 * @return - TVerdict code
 */
{
	FbsStartup();
	TInt ret = RFbsSession::Connect();
	TEST(ret==KErrNone);

	// run the testcode (inside an alloc heaven harness)
	INFO_PRINTF1(_L("About to start tests:"));
	TRAP(ret,DoServComTestL());
	INFO_PRINTF2(_L("  DoServComTestL completes with %d"), ret);
	TEST(ret==KErrNone);

	INFO_PRINTF1(_L("Test Completed!"));
	return TestStepResult();

}


/**
*
* Methods for Active Object Test
*
* Used in for Asynchronous Data Recognizer tests
*
*/

CActiveTest2::CActiveTest2()
	:CActive(0)
	{
	CActiveScheduler::Add(this);
	}

CActiveTest2::~CActiveTest2()
	{
	Cancel();
	}

void CActiveTest2::DoCancel()
	{
	TRequestStatus* s=&iStatus;
	User::RequestComplete(s, KErrNone);
	}

void CActiveTest2::StartL()
	{
	SetActive();
	}

void CActiveTest2::RunL()
	{
	//Stop AS
	CActiveScheduler::Stop();
	}
