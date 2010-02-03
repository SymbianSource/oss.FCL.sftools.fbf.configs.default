// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <f32file.h>
#include <fbs.h>
#include <s32std.h> 
#include <s32stor.h> 
#include <s32file.h> 

#include <coecntrl.h>
#include <coeccntx.h>
#include <coemain.h>
#include <coeview.h>
#include <eikdialg.h>
#include <eikdoc.h>
#include <eikapp.h>
#include <eiksrvc.h>
#include <eikconso.h>

#include <apaid.h>
#include "..\apparc\apadll.h"
#include <apgaplst.h>
#include <apgicnfl.h>
#include <apgdoor.h>
#include <apfrec.h>
#include <apfctlf.h>
#include <apgctl.h>
#include <apgaplst.h>
#include <apaflrec.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <apsserv.h>
#include <barsread.h>
#include "tstapp.h"
#include <appfwk_test.h>

#include "T_RApaLsSessionStep.h"
#include "appfwk_test_AppUi.h"
#include "TRApaLsSessionStartAppTest.h"
#include "TIconLoaderAndIconArrayForLeaks.h"  

#if !defined(__E32TEST_H__)
#include <e32test.h>
#endif

// Literals & Constants
_LIT(KCompleted, "Completed.");
const TUint KBytesToRead=100;


/****************************************
 Auxiliary functions used by test cases
*****************************************/
void CT_RApaLsSessionTestStep::setup()
	{
#if defined(__EPOC32__)
	
	TFullName filePath=_L("c:\\docs\\tstapp.doc");
	TFullName tempPath=_L("c:\\system\\temp\\");
	// if we're on the rack create the directories we need
	TParse parser;
	parser.Set(filePath,NULL,NULL);
	iFs.MkDirAll(parser.DriveAndPath());
	parser.Set(tempPath,NULL,NULL);
	iFs.MkDirAll(parser.DriveAndPath());
#endif
	}


////////////////////////////
// Might actually be tests......
/////////////////////////////

void CT_RApaLsSessionTestStep::TestAppListInfoDataInterrogationSetupL()
	{
	//
	INFO_PRINTF1(_L("Initialising App List Cache using GetAllApps....."));
	TInt ret = iLs.GetAllApps();
	TEST(ret==KErrNone);
	INFO_PRINTF1(KCompleted);

// get the full app count
	INFO_PRINTF1(_L("Getting AppCount....."));
	TInt count=0;
	ret = iLs.AppCount(count);
	TEST(ret==KErrNone);
	TEST(count>=1);
	INFO_PRINTF1(KCompleted);

// prepare to get a list of embeddable apps
	INFO_PRINTF1(_L("Initialising embeddable app list cache using GetEmbeddableApps....."));
	ret = iLs.GetEmbeddableApps();
	TEST(ret==KErrNone);
	INFO_PRINTF1(KCompleted);

// get the embedded app count
	INFO_PRINTF1(_L("Initialising embeddable app list cache using EmbeddableAppCount....."));
	count=0;
	ret = iLs.EmbeddableAppCount(count);
	TEST(ret==KErrNone);
	TEST(count>=1);
	INFO_PRINTF1(KCompleted);
	}	

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppInfo1
  
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
void CT_RApaLsSessionTestStep::TestAppInfo1()
	{
	
	INFO_PRINTF1(_L("Testing RApaLsSession::ENoMoreAppsInList....attempt to get an app"));
    //Go to end of Applist for next test... 
	TApaAppInfo info;
	TInt ret = iLs.GetAllApps();
	TEST(ret==KErrNone);

	do
		{
 		ret=iLs.GetNextApp(info);
 		}
 	while (ret != RApaLsSession::ENoMoreAppsInList);

	//try to get another app (there arn't any more)
	ret = iLs.GetNextApp(info);
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppInfo2
  
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
void CT_RApaLsSessionTestStep::TestAppInfo2()
	{
	INFO_PRINTF1(_L("Testing RApaLsSession::GetNextApps() return info...."));
	_LIT(KLitAppPath,"z:\\sys\\bin\\tstapp.exe");
	TFullName appPath(KLitAppPath);

	TApaAppInfo info;
	TInt ret = iLs.GetAllApps();
	TEST(ret==KErrNone);

	do
	{
		iLs.GetNextApp(info);
	}
	while (ret==KErrNone && info.iUid!=KUidTestApp);
		
	TEST(ret==KErrNone);
	TEST(info.iUid==KUidTestApp);
	info.iFullName.LowerCase();	
	TEST(info.iFullName.Compare(appPath) == 0);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppInfo3
  
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
void CT_RApaLsSessionTestStep::TestAppInfo3()
	{
	INFO_PRINTF1(_L("Testing RApaLsSession::GetAppInfo()...."));
	_LIT(KLitAppPath,"z:\\sys\\bin\\tstapp.exe");
	TFullName appPath(KLitAppPath);
	// Get info for an app that exists
	TApaAppInfo info;
	TInt ret = iLs.GetAppInfo(info,KUidTestApp);
	TEST(ret==KErrNone);
	TEST(info.iUid==KUidTestApp);
	info.iFullName.LowerCase();	
	TEST(info.iFullName.Compare(appPath) == 0);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppInfo4
  
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
void CT_RApaLsSessionTestStep::TestAppInfo4()
	{
	INFO_PRINTF1(_L("Testing RApaLsSession::GetAppInfo()....non-existent app"));

	TApaAppInfo info;
	TInt ret = iLs.GetAppInfo(info,KNullUid);
	TEST(ret==KErrNotFound);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServerIconSizeCountL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test Get the number of icon sizes for testapp app. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Allocate a flat dynamic buffer which would hold available icon
   sizes for tstapp. Call RApaLsSession::GetAppIconSizes() to store the sizes
   of icons available for the app tstapp to the allocated flat dynamic buffer.
   Check that it is non zero, i.e. that a number of icon sizes has been retreived
   for testapp.\n
   API Calls:\n	
   RApaLsSession::GetAppIconSizes(TUid aAppUid, CArrayFixFlat<TSize>& aArrayToFill) const\n
   
   @SYMTestExpectedResults aTestIconSizeArray->Count() is greater than 0.
    
 */
void CT_RApaLsSessionTestStep::TestServerIconSizeCountL(CArrayFixFlat<TSize>& aTestIconSizeArray)
	{
	INFO_PRINTF1(_L("Test RApaLsSession::GetAppIconSizes..."));

	TInt ret=iLs.GetAppIconSizes(KUidTestApp,aTestIconSizeArray);
	if(ret==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST(ret==KErrNone);
	TEST(aTestIconSizeArray.Count()!=0);
	
	INFO_PRINTF1(KCompleted);
	}



/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServerIconLoadingBySize1L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test loading of Icon by size. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Allocate a flat dynamic buffer which would hold available icon
   sizes for tstapp. Call RApaLsSession::GetAppIconSizes() to store the sizes
   of icons available for the app tstapp to the allocated flat dynamic buffer.
   Call RApaLsSession::GetAppIcon() by passing each of the icon sizes stored.
   Verify whether icon is retrieved each time by GetAppIcon().\n
   API Calls:\n	
   RApaLsSession::GetAppIcon(TUid aAppUid, TSize aSize, CApaMaskedBitmap& aAppBitmap) const\n
   
   @SYMTestExpectedResults The results obtained are compared with the expected values to verify
   functionality of RApaLsSession::GetAppIcon().
    
 */
void CT_RApaLsSessionTestStep::TestServerIconLoadingBySize1L(CArrayFixFlat<TSize>& aTestIconSizeArray)
	{
	INFO_PRINTF1(_L("Test RApaLsSession::GetAppIcon....Compare icon sizes"));

	for(TInt i=0;i<aTestIconSizeArray.Count();i++)
		{
		CApaMaskedBitmap* iconBySize=CApaMaskedBitmap::NewLC();
		TInt ret=iLs.GetAppIcon(KUidTestApp,aTestIconSizeArray[i],*iconBySize); //by Size
		if(ret==KErrNoMemory)
			User::Leave(KErrNoMemory);
		TEST(ret==KErrNone);
		TEST(iconBySize->SizeInPixels()==aTestIconSizeArray[i]);
		CleanupStack::PopAndDestroy(iconBySize);
		}
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServerIconLoadingBySize2L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test loading of Icon by size. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Allocate a flat dynamic buffer which would hold available icon
   sizes for tstapp. Call RApaLsSession::GetAppIconSizes() to store the sizes
   of icons available for the app tstapp to the allocated flat dynamic buffer.
   Call RApaLsSession::GetAppIcon() by passing each of the icon sizes stored.
   Test calling RApaLsSession::GetAppIcon() by specifying reduced size than
   the actual icon size. Since no match would be found, KErrNotFound should be
   returned.\n
   API Calls:\n	
   RApaLsSession::GetAppIcon(TUid aAppUid, TSize aSize, CApaMaskedBitmap& aAppBitmap) const\n
   
   @SYMTestExpectedResults The results obtained are compared with the expected values to verify
   functionality of RApaLsSession::GetAppIcon().
    
 */
void CT_RApaLsSessionTestStep::TestServerIconLoadingBySize2L(CArrayFixFlat<TSize>& aTestIconSizeArray)
	{
	INFO_PRINTF1(_L("Test RApaLsSession::GetAppIcon....Compare icon sizes that don't match"));

	// now try non exact sizes
	CApaMaskedBitmap* iconBySize=CApaMaskedBitmap::NewLC();
	TInt ret=iLs.GetAppIcon(KUidTestApp,(aTestIconSizeArray[0])+TSize(-5,-5),*iconBySize); //by Size
	if(ret==KErrNoMemory)
		User::Leave(KErrNoMemory);
	TEST(ret==KErrNotFound);
	CleanupStack::PopAndDestroy(iconBySize);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestServerIconLoadingBySize3L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test loading of Icon by size. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Allocate a flat dynamic buffer which would hold available icon
   sizes for tstapp. Call RApaLsSession::GetAppIconSizes() to store the sizes
   of icons available for the app tstapp to the allocated flat dynamic buffer.
   Call RApaLsSession::GetAppIcon() by passing each of the icon sizes stored.
   Test GetAppIcon() again by specifying an increased size than the
   actual icon size. Observe whether the GetAppIcon() in this case gets the
   closest icon available.\n
   API Calls:\n	
   RApaLsSession::GetAppIcon(TUid aAppUid, TSize aSize, CApaMaskedBitmap& aAppBitmap) const\n
   
   @SYMTestExpectedResults The results obtained are compared with the expected values to verify
   functionality of RApaLsSession::GetAppIcon().
    
 */
void CT_RApaLsSessionTestStep::TestServerIconLoadingBySize3L(CArrayFixFlat<TSize>& aTestIconSizeArray)
	{
	INFO_PRINTF1(_L("Test RApaLsSession::GetAppIcon....Compare icon sizes that don't match."));
	for(TInt ii=0;ii<aTestIconSizeArray.Count();ii++)
		{
		CApaMaskedBitmap* iconBySize=CApaMaskedBitmap::NewLC();
		TInt ret=iLs.GetAppIcon(KUidTestApp,(aTestIconSizeArray[ii])+TSize(5,5),*iconBySize); //by Size
		if(ret==KErrNoMemory)
			User::Leave(KErrNoMemory);
		TEST(ret==KErrNone);
		TEST(iconBySize->SizeInPixels()==aTestIconSizeArray[ii]); // should get size below always!!!
		CleanupStack::PopAndDestroy(iconBySize);
		}
	INFO_PRINTF1(KCompleted);
	}


/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppListInvalidL()
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test .
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Test RApaLsSession::AppForDocument() to recognize data type &
   applications that can handle the data type of data stored in the specified
   file. The test is performed on files of one byte and zero byte sizes.
   Observe that on both occasions the API returns Uid of an application that
   can handle text \ plain data type.\n
   Test RApaLsSession::AppForDataType() to recognize Uid of an application
   that can handle a specified data type. To do this the test calls
   RApaLsSession::RecognizeData() which gets the data type for data taken
   from a specified file. The test is performed on files of one byte and zero
   byte sizes. The test ensures that RApaLsSession::AppForDataType() doesn't
   return KNullUid for both sizes of the file.\n
   API Calls:\n	
   RApaLsSession::AppForDocument(const TDesC& aFileName, TUid& aUid, TDataType& aDataType) const\n
   RApaLsSession::RecognizeData(const TDesC& aName, const TDesC8& aBuffer, TDataRecognitionResult& aDataType) const\n
   RApaLsSession::AppForDataType(const TDataType& aDataType, TUid& aAppUid) const\n
   
   @SYMTestExpectedResults Test ensures that the data recognition functions identify the data type of
   data in a file and the application capable of handling the specified
   data type.
   
 */
void CT_RApaLsSessionTestStep::TestAppListInvalidL()
	{
	__UHEAP_MARK;
	TestAppListInvalidSetupL();
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("Test RApaLsSession::GetNextApp() never returns RApaLsSession::EAppListInvalid."));

	TApaAppInfo info;
	TInt ret=0;
	while(ret==KErrNone)
		{
		ret=iLs.GetNextApp(info);
		}

	//tests GetNextApp() never returns RApaLsSession::EAppListInvalid
	TEST(ret!=RApaLsSession::EAppListInvalid);
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	INFO_PRINTF1(KCompleted);
	}


void CT_RApaLsSessionTestStep::TestAppListInvalidSetupL()
	{
	INFO_PRINTF1(_L("Setting up Applist invalid test."));
	
	_LIT(KTempAppDir, "C:\\private\\10003a3f\\import\\apps\\");
	_LIT(KTempRegPath, "C:\\private\\10003a3f\\import\\apps\\tstapp_reg.rsc");
	TFullName regPath=_L("z:\\private\\10003a3f\\import\\apps\\tstapp_reg.rsc");
	
	CFileMan* iAppListInvalidTestFileMan = CFileMan::NewL (iFs);
	CleanupStack::PushL(iAppListInvalidTestFileMan);
	
	INFO_PRINTF1(_L("Copy tstapp files to C: drive......."));
	TInt rtn=iFs.MkDir(KTempAppDir);
	TEST(rtn==KErrNone||rtn==KErrAlreadyExists); 
	TEST(iAppListInvalidTestFileMan->Copy(regPath, KTempRegPath)==KErrNone);	//Just to start the idle update.

	INFO_PRINTF1(_L("Get app list......."));
	TInt ret = iLs.GetAllApps();
	TEST(ret==KErrNone);

	INFO_PRINTF1(_L("Remove temp files from C: drive......."));
	TRequestStatus status;
	TTime tempTime(0); // added tempTime to avoid asynch CFileMan::Attribs request completing with KErrArgument
	TEST(iAppListInvalidTestFileMan->Attribs(KTempAppDir,0,KEntryAttReadOnly, tempTime, CFileMan::ERecurse, status)==KErrNone);
	
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	INFO_PRINTF1(_L("Deleting Reg file......."));
	TEST(iAppListInvalidTestFileMan->Delete(KTempRegPath)==KErrNone);	//Just to start the idle update.
	INFO_PRINTF1(_L("Removing App dir......."));
	TEST(iAppListInvalidTestFileMan->RmDir(KTempAppDir)==KErrNone);
	CleanupStack::PopAndDestroy(iAppListInvalidTestFileMan);

	INFO_PRINTF1(KCompleted);
	}



/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppListRecognizeDataL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test data recognition functions of RApaLsSession.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Test RApaLsSession::AppForDocument() to recognize data type &
   applications that can handle the data type of data stored in the specified
   file. The test is performed on files of one byte and zero byte sizes.
   Observe that on both occasions the API returns Uid of an application that
   can handle text \ plain data type.\n
   Test RApaLsSession::AppForDataType() to recognize Uid of an application
   that can handle a specified data type. To do this the test calls
   RApaLsSession::RecognizeData() which gets the data type for data taken
   from a specified file. The test is performed on files of one byte and zero
   byte sizes. The test ensures that RApaLsSession::AppForDataType() doesn't
   return KNullUid for both sizes of the file.\n
   API Calls:\n	
   RApaLsSession::AppForDocument(const TDesC& aFileName, TUid& aUid, TDataType& aDataType) const\n
   RApaLsSession::RecognizeData(const TDesC& aName, const TDesC8& aBuffer, TDataRecognitionResult& aDataType) const\n
   RApaLsSession::AppForDataType(const TDataType& aDataType, TUid& aAppUid) const\n
   
   @SYMTestExpectedResults Test ensures that the data recognition functions identify the data type of
   data in a file and the application capable of handling the specified
   data type.
    
 */
void CT_RApaLsSessionTestStep::TestAppListRecognizeDataL()
	{
	// The following tests will only succeed if the text recogniser and an app which likes text files are both installed
	// The tests check that file recognisers work for zero and non zero length files.
	INFO_PRINTF1(_L("Testing AppForDocument(), RecognizeData() and AppForDataType()"));
	TInt ret;
	TUid appUid=KNullUid;
    TDataType dataType;
	INFO_PRINTF1(_L("Testing 1 byte text file recognition"));
    ret = iLs.AppForDocument(_L("z:\\system\\data\\one_byte.txt"), appUid,dataType);
	TEST(ret==KErrNone);
	TEST(appUid!=KNullUid);
	INFO_PRINTF1(KCompleted);
	
	appUid=KNullUid;
	INFO_PRINTF1(_L("Testing zero byte text file recognition"));
    ret = iLs.AppForDocument(_L("z:\\system\\data\\zero_len.txt"), appUid,dataType);
	TEST(ret==KErrNone);
	TEST(appUid!=KNullUid);
	INFO_PRINTF1(KCompleted);
	
	// this following exihibits a defect which was fixed for Release 6 is fixed.
	// KNullUid was recognized for zero length text files using this method before the fix.
	appUid=KNullUid;
	TBuf8<255> buffer;
    buffer.FillZ(255);
    buffer[0] = 'a';	// We used to fill it entirely with zeroes but an ICL recognizer know interferes 
    					// with this, so we added an 'a'
	TDataRecognitionResult recogResult;

	appUid=KNullUid;
	INFO_PRINTF1(_L("Testing 1 byte text file recognition - lengthy method"));
    ret = iLs.RecognizeData(_L("z:\\system\\data\\one_byte.txt"),buffer,recogResult);
	TEST(ret==KErrNone);
    ret = iLs.AppForDataType(recogResult.iDataType,appUid);
	TEST(ret==KErrNone);
	TEST(appUid!=KNullUid);
	INFO_PRINTF1(KCompleted);

	appUid=KNullUid;
	INFO_PRINTF1(_L("Testing zero byte text file recognition -lengthy method"));
	ret = iLs.RecognizeData(_L("z:\\system\\data\\zero_len.txt"),buffer,recogResult);
	TEST(ret==KErrNone);
    ret = iLs.AppForDataType(recogResult.iDataType,appUid);
	TEST(ret==KErrNone);
	TEST(appUid!=KNullUid);
	INFO_PRINTF1(KCompleted);
	
	// the following attempts to open a file of an unknown mime type with extension .UnrecognisableExtention
	
	INFO_PRINTF1(_L("Testing RecognizeData() with unrecognizable file type..."));
	buffer.FillZ(255);
	_LIT8(KLitUnRecog,"");
	iLs.FlushRecognitionCache();
	recogResult.Reset();
	//First call to RecognizeData() does not utilize the cache
	User::LeaveIfError(iLs.RecognizeData(_L("z:\\system\\data\\FileWithUnknownMimeType.UnrecognisableExtention"),buffer,recogResult));
	TEST(recogResult.iDataType.Des8().CompareF(KLitUnRecog)==0);
	recogResult.Reset();
	//Second call to RecognizeData() utilizes the cache
	User::LeaveIfError(iLs.RecognizeData(_L("z:\\system\\data\\FileWithUnknownMimeType.UnrecognisableExtention"),buffer,recogResult));
	TEST(recogResult.iDataType.Des8().CompareF(KLitUnRecog)==0);
	INFO_PRINTF1(_L("Completed Testing unrecognizable file type..."));
	
   	}
   	
/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppListRecognizeDataBufferOnlyL
  
   @SYMPREQ INC064138
  
   @SYMTestCaseDesc Test data recognition functions of RApaLsSession without a filename.
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions 
   
   @SYMTestExpectedResults Test ensures that the data recognition functions identify the data type of
   data in a file.
 */
void CT_RApaLsSessionTestStep::TestAppListRecognizeDataBufferOnlyL()
	{
	INFO_PRINTF1(_L("Testing recognition without filename"));
	
	TDataRecognitionResult rr;
    _LIT8(KTestBuffer, "Text_TestRecognizer");
    rr.Reset();
    TInt error = iLs.RecognizeData(KNullDesC, KTestBuffer, rr);
    if (rr.iConfidence != CApaDataRecognizerType::ECertain)
        {
        INFO_PRINTF1(_L("Error: confidence should be ECertain"));
        TEST(EFalse);
        }
    
    _LIT8(KAnotherBuffer, "AnotherText");
    rr.Reset();
    error = iLs.RecognizeData(KNullDesC, KAnotherBuffer, rr);
    if (rr.iConfidence != CApaDataRecognizerType::EProbable)
        {
        INFO_PRINTF1(_L("Error: confidence should be EProbable"));        
        TEST(EFalse);
        }  
   
  	INFO_PRINTF1(KCompleted);
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
void CT_RApaLsSessionTestStep::TestSetGetAcceptedConfidenceL()
	{
	INFO_PRINTF1(_L("Test SetAcceptedConfidence...."));
	
	TInt temp=1234;
	TInt ret=iLs.SetAcceptedConfidence(temp);
	TSecurityInfo info;
	info.Set(RProcess());
	
	info.iCaps.HasCapability(ECapabilityWriteDeviceData) ? TEST(ret==KErrNone) :  TEST(ret==KErrPermissionDenied);
	INFO_PRINTF1(KCompleted);
	
	INFO_PRINTF1(_L("Test GetAcceptedConfidence...."));
	temp=0;
	ret=iLs.GetAcceptedConfidence(temp);
	TEST(ret==KErrNone);
	TEST(temp==1234);
	INFO_PRINTF1(KCompleted);
	}

void CT_RApaLsSessionTestStep::TestSetGetMaxDataBufSizeL()
	{
	INFO_PRINTF1(_L("Test SetMaxDataBufSize...."));

	TInt temp=1234;
	TInt ret=iLs.SetMaxDataBufSize(temp);
	TEST(ret==KErrNone);
	INFO_PRINTF1(KCompleted);
	
	INFO_PRINTF1(_L("Test GetMaxDataBufSize...."));
	temp=0;
	ret=iLs.GetMaxDataBufSize(temp);
	TEST(ret==KErrNone);
	TEST(temp==1234);
	INFO_PRINTF1(KCompleted);
	}

void CT_RApaLsSessionTestStep::TestGetSupportedDataTypesL()
{
	INFO_PRINTF1(_L("Test GetSupportedDataTypes...."));

	CDataTypeArray* dataTypes=new(ELeave) CDataTypeArray(5);
	CleanupStack::PushL(dataTypes);
	
	TInt ret=iLs.GetSupportedDataTypesL(*dataTypes);
	TEST(ret==KErrNone);
	TEST(dataTypes->Count()>=4);
	CleanupStack::PopAndDestroy(); // dataTypes

	INFO_PRINTF1(KCompleted);
	}


/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoGetAllApps
  
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
   TAppNotEmbeddable.exe
   TAppEmbeddable.dll
   TAppEmbeddableOnly.dll
   TAppEmbeddableUiOrStandAlone.dll
   TAppEmbeddableUiNotStandAlone.dll
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_RApaLsSessionTestStep::DoGetAllApps()
	{
	INFO_PRINTF1(_L("DoGetAllApps"));

	TInt ret = iLs.GetAllApps();
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = iLs.GetNextApp(appInfo)) == KErrNone)
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
		};
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	TEST(bitFlags == 0x1F);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoGetEmbeddableApps
  
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
   TAppEmbeddable.dll
   TAppEmbeddableOnly.dll
   and should not return app info for:
   TAppNotEmbeddable.exe
   TAppEmbeddableUiOrStandAlone.dll
   TAppEmbeddableUiNotStandAlone.dll
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_RApaLsSessionTestStep::DoGetEmbeddableApps()
	{
	INFO_PRINTF1(_L("DoGetEmbeddableApps"));

	TInt ret = iLs.GetEmbeddableApps();
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = iLs.GetNextApp(appInfo)) == KErrNone)
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
		};
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	TEST(bitFlags == 0x06);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoEmbeddableAppCount
  
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
void CT_RApaLsSessionTestStep::DoEmbeddableAppCount()
	{
	INFO_PRINTF1(_L("DoEmbeddableAppCount"));

	TInt ret = iLs.GetEmbeddableApps();
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TInt count1 = 0;
	while ((ret = iLs.GetNextApp(appInfo)) == KErrNone)
		{
		count1 ++;
		};
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	
	TInt count2 = 0;
	ret = iLs.EmbeddableAppCount(count2);
	TEST(ret==KErrNone);
	TEST(count1 == count2);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoGetAppCapability
  
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
   TAppNotEmbeddable.exe
   TAppEmbeddable.dll
   TAppEmbeddableOnly.dll
   TAppEmbeddableUiOrStandAlone.dll
   TAppEmbeddableUiNotStandAlone.dll
  
   @SYMTestExpectedResults
   For each app, GetAppCapability should return the embeddability value
   defined in the application's AIF file.
  
 */
void CT_RApaLsSessionTestStep::DoGetAppCapability()
	{
	INFO_PRINTF1(_L("DoGetAppCapability"));

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
		INFO_PRINTF2(_L("Testing embeddability value, %d"), uidArray[ii]);
		TApaAppCapabilityBuf capabilityBuf;
		TInt ret = iLs.GetAppCapability(capabilityBuf, uidArray[ii]);
		TEST(ret==KErrNone);
		TEST(capabilityBuf().iEmbeddability == embeddabilityArray[ii]);
		}
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoGetFilteredApps1
  
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
void CT_RApaLsSessionTestStep::DoGetFilteredApps1()
	{
	INFO_PRINTF1(_L("DoGetFilteredApps1"));

	TApaEmbeddabilityFilter filter;
	TInt ret = iLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TEST(iLs.GetNextApp(appInfo) == RApaLsSession::ENoMoreAppsInList);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoGetFilteredApps2
  
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
   TAppNotEmbeddable.exe
   and should not return app info for:
   TAppEmbeddable.dll
   TAppEmbeddableOnly.dll
   TAppEmbeddableUiOrStandAlone.dll
   TAppEmbeddableUiNotStandAlone.dll
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
 
 */
void CT_RApaLsSessionTestStep::DoGetFilteredApps2()
	{
	INFO_PRINTF1(_L("DoGetFilteredApps2"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::ENotEmbeddable);
	TInt ret = iLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = iLs.GetNextApp(appInfo)) == KErrNone)
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
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoGetFilteredApps3
  
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
   TAppEmbeddableUiOrStandAlone.dll
   and should not return app info for:
   TAppEmbeddable.dll
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
   
 */
void CT_RApaLsSessionTestStep::DoGetFilteredApps3()
	{
	INFO_PRINTF1(_L("DoGetFilteredApps3"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone);
	TInt ret = iLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = iLs.GetNextApp(appInfo)) == KErrNone)
		{
		if (appInfo.iUid == KUidAppEmbeddable)
			bitFlags |= 0x02;
		else if (appInfo.iUid == KUidAppEmbeddableUiOrStandAlone)
			bitFlags |= 0x08;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);
	TEST(bitFlags == 0x08);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoGetFilteredApps4
  
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
   TAppEmbeddableUiOrStandAlone.dll
   TAppEmbeddableUiNotStandAlone.dll
   and should not return app info for:
   TAppNotEmbeddable.exe
   TAppEmbeddable.dll
   TAppEmbeddableOnly.dll
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_RApaLsSessionTestStep::DoGetFilteredApps4()
	{
	INFO_PRINTF1(_L("DoGetFilteredApps4"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone);
	TInt ret = iLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = iLs.GetNextApp(appInfo)) == KErrNone)
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
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoGetFilteredApps5
  
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
void CT_RApaLsSessionTestStep::DoGetFilteredApps5()
	{
	INFO_PRINTF1(_L("DoGetFilteredApps5"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::ENotEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableOnly);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone);
	TInt ret = iLs.GetFilteredApps(filter);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TInt filteredCount = 0;
	while ((ret = iLs.GetNextApp(appInfo)) == KErrNone)
		{
		filteredCount++;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);

	ret = iLs.GetAllApps();
	TEST(ret==KErrNone);

	TInt allAppsCount = 0;
	while ((ret = iLs.GetNextApp(appInfo)) == KErrNone)
		{
		allAppsCount++;
		}
	TEST(ret==RApaLsSession::ENoMoreAppsInList);

	TEST(filteredCount == allAppsCount);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoGetFilteredApps6
  
   @SYMPREQ PHAR-5Q4FX8
  
   @SYMTestCaseDesc
   None of the following embeddability test apps support
   screen mode 1, so none of them should be returned even
   if the filter specifies all embeddability types.
   TAppNotEmbeddable.exe
   TAppEmbeddable.dll
   TAppEmbeddableOnly.dll
   TAppEmbeddableUiOrStandAlone.dll
   TAppEmbeddableUiNotStandAlone.dll
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
  
   @SYMTestActions
   Call GetFilteredApps passing a filter containing all embeddability values
   and screen mode 1, followed by repeated calls to GetNextApp until something
   other than KErrNone is returned
  
   @SYMTestExpectedResults
   GetNextApp should not return app info for any of the following test apps:
   TAppNotEmbeddable.exe
   TAppEmbeddable.dll
   TAppEmbeddableOnly.dll
   TAppEmbeddableUiOrStandAlone.dll
   TAppEmbeddableUiNotStandAlone.dll
   The last call to GetNextApp should return RApaLsSession::ENoMoreAppsInList
  
 */
void CT_RApaLsSessionTestStep::DoGetFilteredApps6(/*RApaLsSession& iLs*/)
	{
	INFO_PRINTF1(_L("DoGetFilteredApps6"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::ENotEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableOnly);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone);
	const TInt screenMode1 = 1;
	TInt ret = iLs.GetFilteredApps(filter, screenMode1);
	TEST(ret==KErrNone);

	TApaAppInfo appInfo;
	TUint bitFlags = 0;
	while ((ret = iLs.GetNextApp(appInfo)) == KErrNone)
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
	INFO_PRINTF1(KCompleted);
	}
	
/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoTestEmbeddabilityFilter1
  
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
   All calls to MatchesEmbeddability should return fiLse.
  
 */
void CT_RApaLsSessionTestStep::DoTestEmbeddabilityFilter1()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter1"));

	TApaEmbeddabilityFilter filter;
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::ENotEmbeddable));
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddable));
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableOnly));
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableUiOrStandAlone));
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone));
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoTestEmbeddabilityFilter2
  
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
   MatchesEmbeddability should return fiLse.
  
 */
void CT_RApaLsSessionTestStep::DoTestEmbeddabilityFilter2()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter2"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone);
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableOnly));
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoTestEmbeddabilityFilter3
  
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
   MatchesEmbeddability should return fiLse.
  
 */
void CT_RApaLsSessionTestStep::DoTestEmbeddabilityFilter3()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter3"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableOnly);
	TEST(!filter.MatchesEmbeddability(TApaAppCapability::EEmbeddableUiNotStandAlone));
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoTestEmbeddabilityFilter4
  
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
void CT_RApaLsSessionTestStep::DoTestEmbeddabilityFilter4()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter4"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	TEST(filter.MatchesEmbeddability(TApaAppCapability::EEmbeddable));
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoTestEmbeddabilityFilter5
  
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
void CT_RApaLsSessionTestStep::DoTestEmbeddabilityFilter5()
	{
	INFO_PRINTF1(_L("DoTestEmbeddabilityFilter5"));

	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::ENotEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	TEST(filter.MatchesEmbeddability(TApaAppCapability::EEmbeddable));
	INFO_PRINTF1(KCompleted);
	}

 /**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestGetAppCapabilityL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test interrogates the Application Architecture Server. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
  
   RApaLsSession::GetAppCapability(TDes8& aCapabilityBuf,TUid aAppUid) const\n
   RApaLsSession::RecognizeSpecificData(const TDesC& aName, const TDesC8& aBuffer, const TDataType& aDataType, TBool& aResult) const\n
   RApaLsSession::RecognizeData(const TDesC& aName, const TDesC8& aBuffer, TDataRecognitionResult& aDataType) const\n
   RApaLsSession::GetAppIcon(TUid aAppUid, TInt aSize, CApaMaskedBitmap& aAppBitmap) const\n
   
   @SYMTestExpectedResults All tests compare the results obtained with the desired result.
    
 */
void CT_RApaLsSessionTestStep::TestGetAppCapabilityL()
	{
	TApaAppCapabilityBuf buf;
	INFO_PRINTF1(_L("Checking GetAppCapability from Test App Uid..."));
	TInt ret = iLs.GetAppCapability(buf,KUidTestApp);
	TEST(ret==KErrNone);
	TApaAppCapability cap=buf();
	TEST(cap.iEmbeddability==TApaAppCapability::EEmbeddable);
	TEST(!cap.iSupportsNewFile);
	TEST(!cap.iAppIsHidden);
	INFO_PRINTF1(KCompleted);

	INFO_PRINTF1(_L("Checking GetAppCapability Null Id returns KErrNotFound..."));
	ret = iLs.GetAppCapability(buf,KNullUid);
	TEST(ret==KErrNotFound);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T_RApaLsSessionTestStep_TestRecognizeSpecificData
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test interrogates the Application Architecture Server. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
  
   RApaLsSession::RecognizeSpecificData(const TDesC& aName, const TDesC8& aBuffer, const TDataType& aDataType, TBool& aResult) const\n
   RApaLsSession::RecognizeData(const TDesC& aName, const TDesC8& aBuffer, TDataRecognitionResult& aDataType) const\n
   RApaLsSession::GetAppIcon(TUid aAppUid, TInt aSize, CApaMaskedBitmap& aAppBitmap) const\n
   
   @SYMTestExpectedResults All tests compare the results obtained with the desired result.
    
 */
void CT_RApaLsSessionTestStep::TestRecognizeSpecificData()
{
	INFO_PRINTF1(_L("Checking data type information"));
	INFO_PRINTF1(_L("Checking RecognizeSpecificData..."));
	TBool isText=EFalse;
	TInt ret=iLs.RecognizeSpecificData(_L("a file name.txt"),_L8("Some plain text"),TDataType(_L8("text/plain")),isText);
	TEST(ret==KErrNone);
	TEST(isText);

	INFO_PRINTF1(_L("Checking RecognizeData..."));
	TDataRecognitionResult result;
	ret=iLs.RecognizeData(_L("a file name.txt"),_L8("Some plain text"),result);
	TEST(ret==KErrNone);
	TEST(result.iDataType==TDataType(_L8("text/plain")));
	TEST(result.iConfidence==CApaDataRecognizerType::EProbable);
		
	isText=EFalse;
	INFO_PRINTF1(_L("Checking RecognizeSpecificData...unknown data type..."));
	ret=iLs.RecognizeSpecificData(_L("a file name.txt"),_L8("Some plain text"),TDataType(_L8("foo/bar")),isText);
	TEST(!isText);
	INFO_PRINTF1(KCompleted);
}

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestGetAppIconL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test interrogates the Application Architecture Server. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented   
   
   @SYMTestExpectedResults All tests compare the results obtained with the desired result.
    
 */
void CT_RApaLsSessionTestStep::TestGetAppIconL()
	{
	INFO_PRINTF1(_L("Checking icon loading by index"));
	CApaMaskedBitmap* icon=CApaMaskedBitmap::NewLC();
	TSize lastSize(0,0);

	INFO_PRINTF1(_L("Checking index 0"));
	TInt ret=iLs.GetAppIcon(KUidTestApp,0,*icon);
	TEST(ret==KErrNone);
	TEST(icon->SizeInPixels().iWidth*icon->SizeInPixels().iHeight > lastSize.iWidth*lastSize.iHeight);
	lastSize=icon->SizeInPixels();

	INFO_PRINTF1(_L("Checking icon index 1"));	
	ret=iLs.GetAppIcon(KUidTestApp,1,*icon);
	TEST(ret==KErrNone);
	TEST(icon->SizeInPixels().iWidth*icon->SizeInPixels().iHeight == lastSize.iWidth*lastSize.iHeight);
	lastSize=icon->SizeInPixels();

	INFO_PRINTF1(_L("Checking icon index 2"));
	ret=iLs.GetAppIcon(KUidTestApp,2,*icon);
	TEST(ret==KErrNone);		
	TEST((icon->SizeInPixels().iWidth*icon->SizeInPixels().iHeight) > (lastSize.iWidth*lastSize.iHeight));
	lastSize=icon->SizeInPixels();

	INFO_PRINTF1(_L("Checking icon loading unknown Uid...."));
	TUid uid={9999};
	ret=iLs.GetAppIcon(uid,0,*icon);
	TEST(ret==KErrNotFound);
	CleanupStack::PopAndDestroy(icon);
	INFO_PRINTF1(KCompleted);
	}
/**
    @SYMTestCaseID APPFWK-APPARC-0085
   
    @SYMDEF PDEF113054: Apparc fails to detect the presence of localized MBM files  
	
	@SYMTestCaseDesc 
	Negative Test: Test an icon file with an extension which is not supported.
	
    
    @SYMTestPriority High 
   
    @SYMTestStatus Implemented
    
    Call GetAppViewIcon API on an App which has a localized MBM icon file associated with it with an invalid extension.
	GetAppViewIcon returns KErrNotSupported 
    
    @SYMTestExpectedResults RApaLsSession::GetAppViewIcon should return KErrNotSupported when 
    the view icon file is an invalid icon file
    */
void CT_RApaLsSessionTestStep::TestGetAppIcon2L()
	{
	CApaAppViewArray* appViews = new(ELeave) CApaAppViewArray(4);
	CleanupStack::PushL(appViews);
	TInt ret = iLs.GetAppViews(*appViews, KUidTestApp);
	TEST(ret==KErrNone);
	 	
	INFO_PRINTF1(_L("Negative test- checking for file extension which is not supported"));
	//the third view icon is not a valid mbm file
	const TApaAppViewInfo& viewInfo=(*appViews)[2];
	TEST(viewInfo.iUid != KNullUid);
	TEST(viewInfo.iViewCaption.Length() > 0);
	TEST(viewInfo.iScreenMode == 0x00);
	CApaMaskedBitmap* viewBitmap = CApaMaskedBitmap::NewLC();
	ret = iLs.GetAppViewIcon(KUidTestApp, viewInfo.iUid, TSize(50,50), *viewBitmap);
	TEST(ret==KErrNotSupported);
	INFO_PRINTF2(_L("The View icon's UID is - %d"), viewInfo.iUid);
		
	HBufC* fullIconFileName = NULL;
	ret = iLs.GetAppViewIcon(KUidTestApp, viewInfo.iUid, fullIconFileName);
	TEST(ret == KErrNone);
	TEST(fullIconFileName != NULL);
	if (fullIconFileName != NULL)
		{
		TEST(fullIconFileName->Length() > 0);
		INFO_PRINTF2(_L("Invalid View's icon file name is - %S"), fullIconFileName);
		}
	delete fullIconFileName;		
	fullIconFileName = NULL;
	 	
	CleanupStack::PopAndDestroy(viewBitmap);
	CleanupStack::PopAndDestroy(appViews);
	 	
	INFO_PRINTF1(KCompleted);
	}	

/**
   @SYMTestCaseID APPFWK-APPARC-0073
  
   @SYMDEF INC098717: support for scalable midlet icons is missing  
  
   @SYMTestCaseDesc Test interrogates the Application Architecture Server's GetAppIcon API . 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call GetAppIcon API on an app which has an icon file associated with it.
   Again Call GetAppIcon API on an app which does not have an icon file associated with it.
   Again Call GetAppIcon with an invalid app uid. Do it for SVG and MBM icon file.
   
   @SYMTestExpectedResults RApaLsSession::GetAppIcon should return KErrNotFound when 
   an invalid app uid is passed, KErrNotFound when application does not have an icon 
   file, KErrNone and an open file handle to the application's icon file if application 
   has an icon file.
 */
void CT_RApaLsSessionTestStep::TestGetAppIcon1L()
	{
	TestGetAppIconForMBMIconsL();
	TestGetAppIconForSVGIconsL();
	
	//Generic tests for both MBM and SVG icon files
	RFile iconFile;
	CleanupClosePushL(iconFile);
	//Testing GetAppIcon for native app without an icon file.
	INFO_PRINTF1(_L("Testing GetAppIcon API for an app that does not have an icon file"));
	const TUint KGroupNameApp = 0x10208185; //T_groupNametest app, does not have an icon file.
	TInt err = iLs.GetAppIcon(TUid::Uid(KGroupNameApp), iconFile);
	TEST(err == KErrNotFound);

	//Checking the iconfile name when the icon file is not a physical file on the file system.
	CApaAppViewArray* appViews = new(ELeave) CApaAppViewArray(4);
	CleanupStack::PushL(appViews);
	TInt ret = iLs.GetAppViews(*appViews, TUid::Uid(KGroupNameApp));
	TEST(ret==KErrNone);
	TEST(appViews->Count() > 0);
	 	
	const TApaAppViewInfo& viewInfo=(*appViews)[0];
	TEST(viewInfo.iUid != KNullUid);

	HBufC* fullIconFileName = NULL;
	ret = iLs.GetAppViewIcon(TUid::Uid(KGroupNameApp), viewInfo.iUid, fullIconFileName);
	TEST(ret == KErrNone);
	TEST(fullIconFileName != NULL);
	INFO_PRINTF2(_L("The View icon's UID is - %X"), viewInfo.iUid);
	TEST(!fullIconFileName->Compare(_L("file:///c/resource/apps/tcheckiconapp.xyz")));
	INFO_PRINTF2(_L("View's icon file name is - %S"), fullIconFileName);
	
	delete fullIconFileName;		
	CleanupStack::PopAndDestroy(appViews);
	
	//Testing GetAppIcon with an invalid app UID.
	INFO_PRINTF1(_L("Testing GetAppIcon API with unknown app uid"));
	TUid invalidAppUid = {9999};
	err = iLs.GetAppIcon(invalidAppUid, iconFile);
	TEST(err == KErrNotFound);
	
	//Close icon file
	CleanupStack::PopAndDestroy(&iconFile);	
	
	INFO_PRINTF1(KCompleted);
	}

void CT_RApaLsSessionTestStep::TestGetAppIconForMBMIconsL()
	{
	//Testing GetAppIcon for native app with MBM icon file.
	INFO_PRINTF1(_L("Testing GetAppIcon API for an app to retrieve an open file handle to an MBM icon file"));
	const TUint KTestApp = 10; //Tstapp app, has MBM icon file	
	
	RFile mbmIconFile;
	CleanupClosePushL(mbmIconFile);
	
	TInt err = iLs.GetAppIcon(TUid::Uid(KTestApp), mbmIconFile);
	TEST(err == KErrNone);
	
	_LIT(KMBMIconFileName, "z:\\resource\\apps\\tstapp.mbm");
	//Get the name of the icon file 
	TBuf<KMaxFileName> mbmIconFileName;	
	mbmIconFile.FullName(mbmIconFileName);
	TEST(mbmIconFileName.Length() != 0);
	if (mbmIconFileName.Length() != 0)
		{
		mbmIconFileName.LowerCase();
		TEST(mbmIconFileName.Compare(KMBMIconFileName) == 0);
		}
		
	//Try to write into the MBM icon file whose handle was returned.
	INFO_PRINTF1(_L("..writing into the MBM icon file whose handle was returned."));
	_LIT8(KSomeText, "It would not be written into the file");
	err = mbmIconFile.Write(KSomeText);
	TEST(err == KErrAccessDenied);
	
	//Try to read from the MBM icon file whose handle was returned.
	INFO_PRINTF1(_L("..reading from the MBM icon file whose handle was returned."));
	TBuf8<KBytesToRead> buffer; //buffer to read first KBytesToRead bytes of the mbm icon file.
	err = mbmIconFile.Read(buffer);
	TEST(err == KErrNone);
	
	//Close icon files.
	CleanupStack::PopAndDestroy(&mbmIconFile);
	
	//First KBytesToRead bytes of the icon file, this should be same as in mbm file.
	RFile tempFile;
	CleanupClosePushL(tempFile);
	err = tempFile.Open(iFs, KMBMIconFileName, EFileShareReadersOnly);
	TEST(err == KErrNone);
	TBuf8<KBytesToRead> buffer1;
	err = tempFile.Read(buffer1);
	TEST(err == KErrNone);	
	TEST(buffer == buffer1);
	CleanupStack::PopAndDestroy(&tempFile);
	}
	
void CT_RApaLsSessionTestStep::TestGetAppIconForSVGIconsL()
	{
	//Testing GetAppIcon for native app with SVG icon file.		
	RFile svgIconFile;
	CleanupClosePushL(svgIconFile);
	INFO_PRINTF1(_L("Testing GetAppIcon API for an app to retrieve an open file handle to an SVG icon file"));
	const TUint KApparcTestApp = 0x100048F3; //Tstapp app, has SVG icon file
	TInt err = iLs.GetAppIcon(TUid::Uid(KApparcTestApp), svgIconFile);
	TEST(err == KErrNone);
	
	_LIT(KSVGIconFileName, "z:\\resource\\apps\\svg_icon.svg");
	//Get the name of the icon file 
	TBuf<KMaxFileName> svgIconFileName;	
	svgIconFile.FullName(svgIconFileName);
	TEST(svgIconFileName.Length() != 0);
	if (svgIconFileName.Length() != 0)
		{
		svgIconFileName.LowerCase();
		TEST(svgIconFileName.Compare(KSVGIconFileName) == 0);
		}
			
	//Try to write into the icon file whose handle was returned.
	INFO_PRINTF1(_L("..writing into the SVG icon file whose handle was returned."));
	_LIT8(KSomeText, "It would not be written into the file");
	err = svgIconFile.Write(KSomeText);
	TEST(err == KErrAccessDenied);
	
	//Try to read from the icon file whose handle was returned.
	INFO_PRINTF1(_L("..reading from the SVG icon file whose handle was returned."));
	TBuf8<KBytesToRead> svgBuffer; //buffer to read first KBytesToRead bytes of the icon file. 
	err = svgIconFile.Read(svgBuffer);
	TEST(err == KErrNone);
	
	//Close icon file
	CleanupStack::PopAndDestroy(&svgIconFile);
	
	//First KBytesToRead bytes of the icon file, this should be same as in mbm file.
	RFile tempFile;	
	CleanupClosePushL(tempFile);
	err = tempFile.Open(iFs, KSVGIconFileName, EFileRead);
	TEST(err == KErrNone);
	TBuf8<KBytesToRead> buffer1;
	err = tempFile.Read(buffer1);
	TEST(err == KErrNone);
	TEST(svgBuffer == buffer1);
	CleanupStack::PopAndDestroy(&tempFile);		
	}
	
/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppListInstallationL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test updating of Application list on installation of a new application. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Test updating of application list on installation of a new application. 
   Copy the tstapp from z: import\apps folder to c: import\apps folder. 
   Call RApaLsSession::GetAppInfo() and test the application information obtained to 
   verify whether the application architecture server updates the application to reside in c: drive.
   Delete the copied application and call RApaLsSession::GetAppInfo().
   Verify that the application list updates the tstapp application to reside
   on z: drive.\n
   API Calls:\n	
   RApaLsSession::GetAppInfo(TApaAppInfo& aInfo,TUid aAppUid) const\n
   
   @SYMTestExpectedResults The test confirms that the application architecture
   server updates the application list on installation of a new application. It also confirms that 
   if UID of an application which is present on import\apps folder on any non-ROM drive, is same 
   as UID of an existing application present on z: import\apps folder then the apparc server 
   updates the applist and uses the application residing on non-ROM drive's import\apps folder.
    
 */
void CT_RApaLsSessionTestStep::TestAppListInstallationL()
 	{ 
 	_LIT(KTestAppDestDir, "C:\\private\\10003a3f\\import\\apps\\" );
 	_LIT(KTestAppSource, "Z:\\private\\10003a3f\\import\\apps\\tstapp_reg.rsc" );
 	_LIT(KTestAppDest, "C:\\private\\10003a3f\\import\\apps\\tstapp_reg.rsc" );

 	_LIT(KTestWaitingForApplistUpdate,"Waiting %d microseconds for applist to be updated");
 	const TInt KApplistUpdateTime = 10000000;

 	// Copy App files around and delete them to check whether 
	// the app list updates and stores the cache correctly.
 	RFs	theFS;
 	theFS.Connect();
 
 	// Remove Test app from the file system
 	CFileMan* fileManager = CFileMan::NewL (theFS);
 
 	INFO_PRINTF1(_L("Copying the app to C"));
 	TEST(KErrNone == fileManager->Copy (KTestAppSource, KTestAppDest, CFileMan::ERecurse));
 	
 	INFO_PRINTF2(KTestWaitingForApplistUpdate, KApplistUpdateTime);
 	User::After(KApplistUpdateTime);
 
 	TApaAppInfo aInfo;
 	TEST(KErrNone == iLs.GetAppInfo (aInfo, KUidTestApp));

	TParsePtrC parse (aInfo.iFullName);
 	_LIT (KCdrive, "C:");
	INFO_PRINTF1(_L("Comparing App drive location is C:... "));
	TEST(parse.Drive ().CompareF (KCdrive) == 0);
 
 	INFO_PRINTF1(_L("Removing the app from C"));
	TRequestStatus status;
	TTime tempTime(0); // added tempTime to avoid asynch CFileMan::Attribs request completing with KErrArgument
	TEST(fileManager->Attribs(KTestAppDest,0,KEntryAttReadOnly, tempTime, CFileMan::ERecurse, status)==KErrNone);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
 	TEST(KErrNone == fileManager->Delete (KTestAppDest, CFileMan::ERecurse));
	INFO_PRINTF1(_L("Removing the app dir from C"));
	TEST(fileManager->RmDir(KTestAppDestDir)==KErrNone);
 	
	INFO_PRINTF2(KTestWaitingForApplistUpdate, KApplistUpdateTime);
	User::After(KApplistUpdateTime);
 
 	// That should put the file in the right place
 	TEST(KErrNone == iLs.GetAppInfo( aInfo, KUidTestApp));

 	TParsePtrC parse1 (aInfo.iFullName);
 	_LIT (KZdrive, "Z:");
	INFO_PRINTF1(_L("Comparing App drive location is Z:... "));
 	TEST((parse1.Drive().CompareF(KZdrive)) == 0);

 	delete fileManager;
 	theFS.Close();

 	INFO_PRINTF1(_L("Test TestAppListInstallationL completed"));
 	}
 	
/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppListInstallation1L
  
   @SYMDEF DEF055654: Platform-security loop-hole for "import-directory" applications
  
   @SYMTestCaseDesc Test updating of Application list on installation of a new application. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Test updating of application list on installation of a new application. 
   Copy the SimpleApparcTestApp from z:\private\10003a3f\apps folder to c:\private\10003a3f\import\apps
   folder. Call RApaLsSession::GetAppInfo() and test the application information obtained to
   verify that apparc server does not update the application to reside in c: drive
   but on z drive. Delete the copied application and call RApaLsSession::GetAppInfo().
   Verify that the application list does not update app list again.\n
   API Calls:\n	
   RApaLsSession::GetAppInfo(TApaAppInfo& aInfo,TUid aAppUid) const\n
   
   @SYMTestExpectedResults The test confirms that the scanning order of apparc server 
   for applications is 'apps' folder for all ROM drives and then 'import\apps' folder 
   for all other drives. It also confirms that if UID of an application which is present in 
   import\apps folder on any non-ROM drive, is same as UID of an existing application present 
   on z: apps folder then the apparc server does not update app list and uses the application
   residing on z drive.
    
 */
void CT_RApaLsSessionTestStep::TestAppListInstallation1L()
 	{ 
 	_LIT(KTestAppDestDir, "C:\\private\\10003a3f\\import\\apps\\" );
 	_LIT(KTestAppSource, "Z:\\private\\10003a3f\\apps\\SimpleApparcTestApp_reg.rsc" );
 	_LIT(KTestAppDest, "C:\\private\\10003a3f\\import\\apps\\SimpleApparcTestApp_reg.rsc" );

 	_LIT(KTestWaitingForApplistUpdate,"Waiting %d microseconds for applist to be updated");
 	const TInt KApplistUpdateTime=10000000;

 	// Copy reg files around and delete them to check the app list does not update. 
 	RFs	theFS;
 	User::LeaveIfError(theFS.Connect());
 	CleanupClosePushL(theFS);
 	
 	// Remove simple app from the file system
 	CFileMan* fileManager=CFileMan::NewL(theFS);
 	CleanupStack::PushL(fileManager);
 
 	INFO_PRINTF1(_L("Copying the app to C \\import\\apps folder"));
 	TEST(KErrNone==fileManager->Copy(KTestAppSource,KTestAppDest,CFileMan::ERecurse));
 	
 	INFO_PRINTF2(KTestWaitingForApplistUpdate,KApplistUpdateTime);
 	User::After(KApplistUpdateTime);
 
 	TApaAppInfo appInfo;
 	const TUid KUidSimpleApparcTestApp={0x12008ACE};
 	TEST(KErrNone==iLs.GetAppInfo(appInfo,KUidSimpleApparcTestApp));

	TParsePtrC parse(appInfo.iFullName);
 	_LIT (KZdrive,"Z:");
 	INFO_PRINTF1(_L("After copying app to C:, there should not be any update in app list:... "));
 	INFO_PRINTF1(_L("Comparing App drive location is Z:... "));
	TEST(parse.Drive().CompareF(KZdrive)==0);
 
 	INFO_PRINTF1(_L("Removing the app from C"));
	TRequestStatus status;
	TTime tempTime(0); // added tempTime to avoid asynch CFileMan::Attribs request completing with KErrArgument
	TEST(fileManager->Attribs(KTestAppDest,0,KEntryAttReadOnly,tempTime,CFileMan::ERecurse,status)==KErrNone);
	User::WaitForRequest(status);
	TEST(status.Int()==KErrNone);
 	TEST(KErrNone==fileManager->Delete(KTestAppDest, CFileMan::ERecurse));

	INFO_PRINTF1(_L("Removing the app dir from C"));
	TEST(fileManager->RmDir(KTestAppDestDir)==KErrNone);

 	
	INFO_PRINTF2(KTestWaitingForApplistUpdate,KApplistUpdateTime);
	User::After(KApplistUpdateTime);
 
 	TEST(KErrNone==iLs.GetAppInfo(appInfo,KUidSimpleApparcTestApp));

 	TParsePtrC parse1(appInfo.iFullName);
 	INFO_PRINTF1(_L("After removing app from C:, there should not be any update in app list:... "));
 	INFO_PRINTF1(_L("Comparing App drive location is Z:... "));
 	TEST((parse1.Drive().CompareF(KZdrive))==0);
 	
 	CleanupStack::PopAndDestroy(2, &theFS);

 	INFO_PRINTF1(_L("Test TestAppListInstallation1L completed"));
 	} 	

/**
   @SYMTestCaseID T-RApaLsSessionTestStep-TestAppFolderNonRomDrivesL
  
   @SYMDEF DEF055654: Platform-security loop-hole for "import-directory" applications
  
   @SYMTestCaseDesc Test scanning of app folder for non-ROM drives. 
  
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Test scanning of app folder for non-ROM drives. 
   Copy a new application AAA_reg from z:\Apparctest folder to c:\private\10003a3f\apps folder. 
   Call RApaLsSession::GetAppInfo() and test application architecture server does not 
   populate the new application in app list. Delete the copied application.\n
   API Calls:\n	
   RApaLsSession::GetAppInfo(TApaAppInfo& aInfo,TUid aAppUid) const\n
   
   @SYMTestExpectedResults The test confirms that the application architecture server does not 
   scans applications residing in 'apps' folder for all non-ROM drives.    
 */
void CT_RApaLsSessionTestStep::TestAppFolderNonRomDrivesL()
 	{ 
 	_LIT(KTestAppDestDir, "C:\\private\\10003a3f\\apps\\" );
 	_LIT(KTestAppSource, "Z:\\Apparctest\\AAA_reg.rsc" );
 	_LIT(KTestAppDest, "C:\\private\\10003a3f\\apps\\AAA_reg.rsc" );

 	_LIT(KTestWaitingForApplistUpdate,"Waiting %d microseconds for applist to be updated");
 	const TInt KApplistUpdateTime=10000000;

	INFO_PRINTF1(_L("Test scanning of app folder for non-ROM drives"));
 	// Copy reg files around and delete them to check 
	// the app list does not update. 
 	RFs	theFS;
 	User::LeaveIfError(theFS.Connect());
 	CleanupClosePushL(theFS);
 
 	// Remove simple app from the file system
 	CFileMan* fileManager=CFileMan::NewL(theFS);
 	CleanupStack::PushL(fileManager);
 
 	INFO_PRINTF1(_L("Copying AAA app to C apps folder"));
 	TEST(KErrNone==fileManager->Copy(KTestAppSource,KTestAppDest,CFileMan::ERecurse));
 	
 	INFO_PRINTF2(KTestWaitingForApplistUpdate,KApplistUpdateTime);
 	User::After(KApplistUpdateTime);
 
 	TApaAppInfo appInfo;
 	// UID of AAA app is 0x10207f74
 	const TUid KUidSimpleApp={0x10207f74};
 	INFO_PRINTF1(_L("Finding AAA app in app list... "));
 	TEST(KErrNotFound==iLs.GetAppInfo(appInfo,KUidSimpleApp));

	INFO_PRINTF1(_L("Removing AAA app from C apps folder"));
	TRequestStatus status;
	TTime tempTime(0); // added tempTime to avoid asynch CFileMan::Attribs request completing with KErrArgument
	TEST(fileManager->Attribs(KTestAppDest,0,KEntryAttReadOnly,tempTime,CFileMan::ERecurse,status)==KErrNone);
	User::WaitForRequest(status);
	TEST(status.Int()==KErrNone);
 	TEST(KErrNone==fileManager->Delete(KTestAppDest,CFileMan::ERecurse));

	INFO_PRINTF1(_L("Removing the app dir from C"));
	TEST(fileManager->RmDir(KTestAppDestDir)==KErrNone);

 	
	INFO_PRINTF2(KTestWaitingForApplistUpdate,KApplistUpdateTime);
	User::After(KApplistUpdateTime);
 	
 	INFO_PRINTF1(_L("Finding AAA app in app list... "));
 	TEST(KErrNotFound==iLs.GetAppInfo(appInfo,KUidSimpleApp));
 	
 	CleanupStack::PopAndDestroy(2, &theFS);

 	INFO_PRINTF1(_L("Test scanning of app folder for non-ROM drives completed"));
 	}
 	
/**
   @SYMTestCaseID T-RApaLsSessionTestStep-DoNumDefIconsTestL
  
   @SYMDEF DEF037571 - Provide a better alternative to CApaAppInfoFileReader::NumberOfBitmaps'
  
   @SYMTestCaseDesc  
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions 
   API Calls:\n	
   RApaLsSession::GetAppIconSizes() \n
   RApaLsSession::NumberOfOwnDefinedIcons(appUid, iconCount)
   @SYMTestExpectedResults
    
 */
void CT_RApaLsSessionTestStep::DoNumDefIconsTestL()
	{
	INFO_PRINTF1(_L("Testing RApaLsSession::NumberOfOwnDefinedIcons"));

	CArrayFixFlat<TSize>* array = new(ELeave) CArrayFixFlat<TSize>(4);
	CleanupStack::PushL(array);

	// Test the GetAppIconSizes function, check 3 icons are returned (default icons)
	INFO_PRINTF1(_L("Testing GetAppIconSizes"));
	TUid appUid;
	appUid.iUid = 0x12008ACE;	// SimpleApparcTestApp UID
	TInt err = iLs.GetAppIconSizes(appUid, *array);
	TEST(err == KErrNone);
	TEST(array->Count() == 3);

	// Test the NumberOfOwnDefinedIcons function, check count is zero (no own defined icons)
	INFO_PRINTF1(_L("Testing NumberOfOwnDefinedIcons (SimpleApparcTestApp)"));
	TInt iconCount = -1;
	err = iLs.NumberOfOwnDefinedIcons(appUid, iconCount);
	TEST(err == KErrNone);
	TEST(iconCount == 0);

	// Test NumberOfOwnDefinedIcons again, this time with Agenda app UID (icons defined)
	INFO_PRINTF1(_L("Testing NumberOfOwnDefinedIcons (Paint app)"));
	iconCount = -1;
	appUid.iUid = 0x10003A5C;//agenda  app UID
	err = iLs.NumberOfOwnDefinedIcons(appUid, iconCount);
	TEST(err == KErrNone);
	TEST(iconCount == 3);

	CleanupStack::PopAndDestroy(array);

	INFO_PRINTF1(_L("NumberOfOwnDefinedIcons test complete"));
	}


void CT_RApaLsSessionTestStep::TestMatchesSecurityPolicy()
	{
	TUid appUid;
	appUid.iUid=0X10009f9a;
    TBool matches;
   
    //Testing with valid TSecurityPolicy
    INFO_PRINTF1(_L("Testing MatchesSecurityPolicy with valid TSecurityPolicy"));
    iLs.MatchesSecurityPolicy(matches, appUid, TSecurityPolicy(TVendorId(0x70000001)));
    TEST(matches);
    INFO_PRINTF1(KCompleted);
    
    //Testing with invalid TSecurityPolicy
    INFO_PRINTF1(_L("Testing MatchesSecurityPolicy with invalid TSecurityPolicy"));
    APPFWK_NEGATIVE_PLATSEC_START;
    iLs.MatchesSecurityPolicy(matches, appUid, TSecurityPolicy(TVendorId(0x70000002)));
    APPFWK_NEGATIVE_PLATSEC_FINISH;
    TEST(!matches);
    INFO_PRINTF1(KCompleted);
	}

void CT_RApaLsSessionTestStep::TestGetAppViewsL()
	{
	INFO_PRINTF1(_L("Testing RApaLsSession::GetAppViews() with a valid app uid"));

	CApaAppViewArray* appViews = new(ELeave) CApaAppViewArray(4);
	CleanupStack::PushL(appViews);

	TInt ret = iLs.GetAppViews(*appViews, KUidTestApp);
	TEST(ret==KErrNone);
	
	// check appviews contain entries
	const TInt count=appViews->Count();
	TEST(count > 0);
	TInt testNumberOfIcons = count-1;
	//the last view is invalid. Hence it checks till count-1 
	for (TInt ii=0; ii<testNumberOfIcons; ii++)
		{
		const TApaAppViewInfo& viewInfo=(*appViews)[ii];
		TEST(viewInfo.iUid != KNullUid);
		TEST(viewInfo.iViewCaption.Length() > 0);
		TEST(viewInfo.iScreenMode == 0x00);

		// check icons in the views
		CApaMaskedBitmap* viewBitmap = CApaMaskedBitmap::NewLC();
		ret = iLs.GetAppViewIcon(KUidTestApp, viewInfo.iUid, TSize(50,50), *viewBitmap);
		TEST(ret==KErrNone);
		CleanupStack::PopAndDestroy(viewBitmap);
		
		HBufC* fullFileName = NULL;
		ret = iLs.GetAppViewIcon(KUidTestApp, viewInfo.iUid, fullFileName);
		TEST(ret == KErrNone);
		TEST(fullFileName != NULL);
		if (fullFileName != NULL)
			{
			TEST(fullFileName->Length() > 0);
			}
		delete fullFileName;		
		fullFileName = NULL;		
		}

	CleanupStack::PopAndDestroy(appViews);
    INFO_PRINTF1(KCompleted);
	}

void CT_RApaLsSessionTestStep::TestGetAppViewsFailureL()
	{
	INFO_PRINTF1(_L("Testing RApaLsSession::GetAppViews() with an invalid app uid"));

	const TUid appUid={10000}; // invalid uid
	CApaAppViewArray* appViews = new(ELeave) CApaAppViewArray(2);
	CleanupStack::PushL(appViews);
	
	//Create App View array
	TInt ret = iLs.GetAppViews(*appViews, appUid);
	TEST(ret=KErrNotFound);
	
	CleanupStack::PopAndDestroy(appViews);
    INFO_PRINTF1(KCompleted);
	}

void CT_RApaLsSessionTestStep::IconLoadingTestCasesL()
	{
	CArrayFixFlat<TSize>* testIconSizeArray;
	testIconSizeArray=new(ELeave) CArrayFixFlat<TSize>(3);
	CleanupStack::PushL(testIconSizeArray);
	TestServerIconSizeCountL(*testIconSizeArray);
	TestServerIconLoadingBySize1L(*testIconSizeArray);
	TestServerIconLoadingBySize2L(*testIconSizeArray);
	TestServerIconLoadingBySize3L(*testIconSizeArray);
	CleanupStack::PopAndDestroy(testIconSizeArray);
	}

void CT_RApaLsSessionTestStep::AppInfoTestCasesL()
	{
	TestAppListInfoDataInterrogationSetupL();
					
	TestAppInfo1();
	TestAppInfo2();
	TestAppInfo3();
	TestAppInfo4();
	
	TestGetAppCapabilityL();
	TestRecognizeSpecificData();

	TestSetGetAcceptedConfidenceL();
	TestSetGetMaxDataBufSizeL();
			
	TestGetSupportedDataTypesL();
	TestGetAppIconL();
	TestGetAppIcon1L();
	TestGetAppIcon2L();

	TestGetAppViewsL();
	TestGetAppViewsFailureL();
	}

void CT_RApaLsSessionTestStep::EmbeddedAppsTestCases()
	{
	DoGetAllApps();
	DoGetEmbeddableApps();
	DoEmbeddableAppCount();
	DoGetAppCapability();
	DoGetFilteredApps1();
	DoGetFilteredApps2();
	DoGetFilteredApps3();
	DoGetFilteredApps4();
	DoGetFilteredApps5();
	DoGetFilteredApps6();
	DoTestEmbeddabilityFilter1();
	DoTestEmbeddabilityFilter2();
	DoTestEmbeddabilityFilter3();
	DoTestEmbeddabilityFilter4();
	DoTestEmbeddabilityFilter5();
	}
	
void CT_RApaLsSessionTestStep::TestNotifyOnDataMappingChangeL()
	{
	INFO_PRINTF1(_L("Test RApaLsSession::TestNotifyOnDataMappingChangeL()"));
	RApaLsSession lsMappingChange;
	lsMappingChange.Connect();
	CleanupClosePushL(lsMappingChange);
	TRequestStatus status;
	lsMappingChange.NotifyOnDataMappingChange(status);
	//Test for TestCancelNotifyOnDataMappingChange
	lsMappingChange.CancelNotifyOnDataMappingChange();
	User::WaitForRequest(status);
	TEST( status ==KErrCancel);
	//Test for TestNotifyOnDataMappingChangeL
	lsMappingChange.NotifyOnDataMappingChange(status);
	const TUid KTestUidValue1={10101010};
	const TDataType dataType(_L8("sonew/else"));
	iLs.InsertDataMapping(dataType,KDataTypePriorityHigh,KTestUidValue1);
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(KErrNone == iLs.DeleteDataMapping(dataType));
	CleanupStack::PopAndDestroy(&lsMappingChange);
	INFO_PRINTF1(KCompleted);
	}

/**
   @SYMTestCaseID T_RApaLsSessionTestStep_TestDataPriorityForUnTrustedApps
  
   @SYMDEF	DEF063982
  
   @SYMTestCaseDesc Tests the Un-Trusted Apps Datatypepriority assigned through 
	RApaLsSession::InsertDataMapping(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid)\n
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented

   @SYMTestExpectedResults All tests compare the results obtained with the desired result.
*/
void CT_RApaLsSessionTestStep::TestDataPriorityForUnTrustedApps()
	{
	INFO_PRINTF1(_L("TestDataPriorityForUnTrustedApps about to start..."));
	const TUid KUidUnTrustedApp = {0x10207f8C};
	const TUid KUidTrustedApp = {0x10207f8D};
	TInt ret;
	TBool insertVal = EFalse;
	//There is no restriction for the Trusted Apps datapriority
	TDataTypePriority KTrustedAppPriority = {KDataTypeUnTrustedPriorityThreshold+1};
	//The data priority for UnTrusted Apps is greater than the Threshold
	TDataTypePriority KUnTrustedAppPriority = {KDataTypeUnTrustedPriorityThreshold+10};
	_LIT8(KLitMimeTypeTestData,"test/data");
	ret = iLs.InsertDataMapping(TDataType(KLitMimeTypeTestData),KTrustedAppPriority,KUidTrustedApp);
	//UnTrusted Apps data priority will be reduced to Security Threshold if it is greater than it.
	ret = iLs.InsertDataMappingIfHigher(TDataType(KLitMimeTypeTestData),KUnTrustedAppPriority,KUidUnTrustedApp,insertVal);
	TUid testUid;
	ret = iLs.AppForDataType(TDataType(KLitMimeTypeTestData),testUid);
	TEST(ret == KErrNone);
	TEST(testUid == KUidTrustedApp);
	INFO_PRINTF1(_L("Completed"));
	}

/**
   @SYMTestCaseID T_RApaLsSessionTestStep_TestDataPriorityForUnTrustedAppsRegFile
  
   @SYMDEF	DEF063982
  
   @SYMTestCaseDesc Tests the UnTrusted Apps Datatypepriority assigned through the registration files
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestExpectedResults All tests compare the results obtained with the desired result.
 */
void CT_RApaLsSessionTestStep::TestDataPriorityForUnTrustedAppsRegFile()
	{
	INFO_PRINTF1(_L("TestDataPriorityForUnTrustedAppsRegFile about to start..."));
	const TUid KUidRegTrustedApp = {0x10207f8F};
	_LIT8(KLitMimeTypeTestData,"text/html");
	TUid testUid;
	TInt ret;
	//The datatype priority of UnTrusted Apps is reduced to Threshold.  
	ret = iLs.AppForDataType(TDataType(KLitMimeTypeTestData),testUid);
	TEST(ret == KErrNone);
	TEST(testUid == KUidRegTrustedApp);
	INFO_PRINTF1(_L("Completed"));
	}
	
/**
   @SYMTestCaseID T_RApaLsSessionTestStep_TestIconLoaderAndIconArrayMemoryLeaksL
  
   @SYMDEF	PDEF104113
  
   @SYMTestCaseDesc Checks there's no memory leaks when a CApaAppIconArray oject 
   is created and deleted
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions It creates an object of CApaAppIconArray class and deletes it. 
   Then test there is no memory leak from the creation to destory the object.
   
   @SYMTestExpectedResults There should be no memory leak checked by __UHEAP_MARK 
   and __UHEAP_MARKEND.
 */
void CT_RApaLsSessionTestStep::TestIconLoaderAndIconArrayMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Test TestIconLoaderAndIconArrayMemoryLeaksL"));
	__UHEAP_MARK;
	TIconLoaderAndIconArrayForLeaks::TestIconLoaderAndIconArrayL();
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Completed"));
	}

void CT_RApaLsSessionTestStep::RunTestCasesL()
	{
	//make sure the RFbsSession doesn't have to allocate buffer since it would change the heap
	CFbsBitmap* bufferAllocator = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bufferAllocator);
	bufferAllocator->Create(TSize(200,1), EColor16M);
	CleanupStack::PopAndDestroy(bufferAllocator);

	
	HEAP_TEST_LS_SESSION(iLs, 0, 0, TestAppListInvalidL(), iLs.ClearAppInfoArray() );
	//DONT_CHECK due to file system changes
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestAppListInstallationL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestAppListInstallation1L(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestAppFolderNonRomDrivesL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, 0, IconLoadingTestCasesL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, 0, AppInfoTestCasesL(), iLs.ClearAppInfoArray(); NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, 0, EmbeddedAppsTestCases(), iLs.ClearAppInfoArray() );
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, DoNumDefIconsTestL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, 0, TestMatchesSecurityPolicy(), NO_CLEANUP);
	//DONT_CHECK since there's a new typestore
	HEAP_TEST_LS_SESSION(iLs, 0, DONT_CHECK, TestNotifyOnDataMappingChangeL(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, 0, TestAppListRecognizeDataBufferOnlyL(), iLs.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(iLs, 0, 0, TestAppListRecognizeDataL(), iLs.FlushRecognitionCache() );
	HEAP_TEST_LS_SESSION(iLs, 0, 0, TestDataPriorityForUnTrustedApps(), NO_CLEANUP);
	HEAP_TEST_LS_SESSION(iLs, 0, 0, TestDataPriorityForUnTrustedAppsRegFile(), NO_CLEANUP);
	TestIconLoaderAndIconArrayMemoryLeaksL();
	}

/**
   Destructor
 */
CT_RApaLsSessionTestStep::~CT_RApaLsSessionTestStep()
	{
	RFbsSession::Disconnect();
	iFs.Close();
	iLs.Close();
	}

/**
   Constructor
 */
CT_RApaLsSessionTestStep::CT_RApaLsSessionTestStep()
	{
	FbsStartup();
	TInt ret=RFbsSession::Connect();
	TEST(ret==KErrNone);

	ret=iFs.Connect();
	TEST(ret==KErrNone);
	
	setup();

	ret = iLs.Connect();
	TEST(ret==KErrNone);
	
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_RApaLsSessionStep);
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CT_RApaLsSessionTestStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CT_RApaLsSessionTestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}


TVerdict CT_RApaLsSessionTestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparc...T_RApaLsSession Test Cases Running..."));

	TRAPD(ret,RunTestCasesL())
	TEST(ret==KErrNone);
	
	INFO_PRINTF1(_L("...T_RApaLsSession Test Cases Completed."));

	return TestStepResult();
	}
	
