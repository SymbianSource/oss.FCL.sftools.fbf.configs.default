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
 @test
 @internalComponent - Internal Symbian test code  
*/

#include <E32STD.H>
#include <E32BASE.H>
#include <F32FILE.H>
#include <APGCLI.H>
#include <APMREC.H>
#include <APAID.H>
#include "testableapalssession.h"
#include <APGICNFL.H>
#include <APMSTD.H>
#include <APACMDLN.H>
#include <E32PROPERTY.H>
#include <apgnotif.h>

#include "..\tef\TNonNative\tnnapp1.h"
#include "T_NonNativeAppsStep.h"
#include <appfwk_test.h>


const TUint KApplicationType=0x10207f90;
const TUint KApplicationA=0x10207f91;
TUint KApplicationB = 0x2002237C;
const TUint KMySID=0x101F289C;				// apparctestserver UID
const TUint KPropertyCategory=KMySID;
const TUint KPropertyKey=0;
const TUint KBytesToRead=100;

_LIT8(KLit8_DataType_Gif, "x-epoc/nna-gif");
_LIT8(KLit8_DataType_Html, "x-epoc/nna-html");
_LIT8(KLit8_DataType_Vcard, "x-epoc/nna-vcf");
_LIT8(KLit8_DataType_plainText, "x-epoc/nna-txt");

_LIT(KLitLogicalExecutableA, "c:\\A.NNAPP2"); // this file never actually needs to exist
_LIT(KLitApplicationCaptionA, "Caption NNA A");
_LIT(KLitDocumentName1, "nnapp2:1");
_LIT(KLitDocumentName2, "nnapp2:2");
_LIT(KLitDocumentName3, "nnapp2:3");
_LIT(KLitNativeExecutable, "z:\\sys\\bin\\TNNAPP2.EXE");
_LIT8(KLitMimeTypeA, "x-epoc/a-nnapp2");
_LIT8(KLitMimeTypeB, "x-epoc/dummy");

enum TOption 
	{
	EAppA,
	EAppB
	};


LOCAL_C void DeregisterNonNativeL(TAny* aApparcServer)
	{
	RApaLsSession& apparcServer=*STATIC_CAST(RApaLsSession*, aApparcServer);
	apparcServer.PrepareNonNativeApplicationsUpdatesL();
	apparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	apparcServer.CommitNonNativeApplicationsUpdatesL();
	apparcServer.DeregisterNonNativeApplicationTypeL(TUid::Uid(KApplicationType));
	}


// RTstFileArray

RTstFileArray::RTstFileArray(TInt aGranularity)
	:iFileArray(aGranularity)
	{
	}

void RTstFileArray::Close()
	{
	for (TInt i=iFileArray.Count()-1; i>=0; --i)
		{
		const SFile& file=iFileArray[i];
		delete file.iFullFileName;
		delete file.iExpectedDataType;
		}
	iFileArray.Close();
	}

void RTstFileArray::AppendL(const TDesC& aFullFileName, const TDesC8& aExpectedDataType)
	{
	SFile file;
	file.iFullFileName=aFullFileName.AllocLC();
	file.iExpectedDataType=aExpectedDataType.AllocLC();
	iFileArray.AppendL(file);
	CleanupStack::Pop(2, file.iFullFileName);
	}

void RTstFileArray::Get(TPtrC& aFullFileName, TPtrC8& aExpectedDataType, TInt aIndex) const
	{
	const SFile& file=iFileArray[aIndex];
	aFullFileName.Set(*file.iFullFileName);
	aExpectedDataType.Set(*file.iExpectedDataType);
	}


// RTstAppService

TInt RTstAppService::DoTestL(TUid aServiceUid, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath)
	{ // static
	RTstAppService appService(aServiceUid);
	CleanupClosePushL(appService);
	appService.ConnectL();
	TRequestStatus requestStatus;
	appService.ReceiveTestResult(requestStatus, aPassingFileByHandle, aFileNameWithoutDriveOrPath);
	User::WaitForRequest(requestStatus);
	const TInt result=User::LeaveIfError(requestStatus.Int());
	CleanupStack::PopAndDestroy(&appService);
	User::After(5 * 1000000); //Wait 5sec for Apparc update due to paging
	return result;
	}

RTstAppService::RTstAppService(TUid aServiceUid)
	:iServiceUid(aServiceUid)
	{
	}

void RTstAppService::ConnectL()
	{
	ConnectExistingByNameL(KLitServerName);
	}

void RTstAppService::ReceiveTestResult(TRequestStatus& aRequestStatus, TBool aPassingFileByHandle, const TDesC& aFileNameWithoutDriveOrPath)
	{
	SendReceive(EOpcode_receiveTestResult, TIpcArgs(aPassingFileByHandle, &aFileNameWithoutDriveOrPath), aRequestStatus);
	}

TUid RTstAppService::ServiceUid() const
	{
	return iServiceUid;
	}


// top-level functions

HBufC8* CT_NonNativeAppsStep::OpenFileAndReadBufferLC(RApaLsSession& aApparcServer, RFs& aFileServer, const TDesC& aFullFileName)
	{
	TInt preferredBufSize=0;
	User::LeaveIfError(aApparcServer.GetPreferredBufSize(preferredBufSize));
	HBufC8* const buffer=HBufC8::NewLC(preferredBufSize);
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(aFileServer, aFullFileName, EFileShareReadersOnly|EFileStream|EFileRead));
	{TPtr8 buffer_asWritable(buffer->Des());
	User::LeaveIfError(file.Read(buffer_asWritable));}
	CleanupStack::PopAndDestroy(&file);
	return buffer;
	}

void CT_NonNativeAppsStep::OpenFileAndSetFilePositionToSomewhereUnusualLC(RFile& aFile, TInt& aFilePosition, RFs& aFileServer, const TDesC& aFullFileName)
	{
	CleanupClosePushL(aFile);
	User::LeaveIfError(aFile.Open(aFileServer, aFullFileName, EFileShareReadersOnly|EFileStream|EFileRead));
	aFilePosition=0;
	User::LeaveIfError(aFile.Seek(ESeekEnd, aFilePosition));
	aFilePosition=(aFilePosition*83)/100; // set it to somewhere "random" in the middle of the file (83% of the way through)
	User::LeaveIfError(aFile.Seek(ESeekStart, aFilePosition));
	}

void CT_NonNativeAppsStep::TestFilePosition(RFile& aFile, TInt aFilePosition)
	{
	TInt filePosition=0;
	TEST(aFile.Seek(ESeekCurrent, filePosition) == KErrNone);
	TEST(filePosition==aFilePosition);
	}

/**
   @SYMTestCaseID T_NonNativeAppsStep-TestAppForDocument
  
   @SYMDEF	DEF065742
  
   @SYMTestCaseDesc Tests AppForDocument API for non native applications.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions	Call AppForDocument, first with the file name, and then with the file data, with a series of different known files.
   		API Calls:\n	
   		RApaLsSession::AppForDocument(const TDesC &aFileName, TUid &aAppUid, TDataType &aDataType) const;\n
   		RApaLsSession::AppForDocument(RFile &aFile, TUid &aAppUid, TDataType &aDataType) const;\n
   
   @SYMTestExpectedResults Test checks MIME type and UIDs obtained match the expected ones.
 */
void CT_NonNativeAppsStep::TestAppForDocumentL(RApaLsSession& aApparcServer, RFs& aFileServer, const RTstFileArray& aFileArray)
	{
	INFO_PRINTF1(_L("Testing AppForDocumentL()..."));
	for (TInt i=aFileArray.Count()-1; i>=0; --i)
		{
		TPtrC fullFileName;
		TPtrC8 expectedDataType;
		aFileArray.Get(fullFileName, expectedDataType, i);
 
		TPtrC fileNameWithoutDriveOrPath(TParsePtrC(fullFileName).NameAndExt());
		INFO_PRINTF2(_L("..using following file: '%S'"), &fileNameWithoutDriveOrPath);

		{
		TDataType dataType;
		TUid appUid;
		//due to access of file in private folder
		APPFWK_EXPECTED_PLATSEC_START;
		User::LeaveIfError(aApparcServer.AppForDocument(fullFileName, appUid, dataType));
		RDebug::Print(_L("The Full FileName is '%S' "),&fullFileName);
		APPFWK_EXPECTED_PLATSEC_FINISH;
		TEST(dataType.Des8().CompareF(expectedDataType)==0);
		TEST(appUid.iUid==0x10207f92);
		}

		RFile file;
		TInt originalFilePosition=0;
		OpenFileAndSetFilePositionToSomewhereUnusualLC(file, originalFilePosition, aFileServer, fullFileName);

		{
		TDataType dataType;
		TUid appUid;
		User::LeaveIfError(aApparcServer.AppForDocument(file, appUid, dataType));
		TEST(dataType.Des8().CompareF(expectedDataType)==0);
		TEST(appUid.iUid==0x10207f92);

		TestFilePosition(file, originalFilePosition);
		CleanupStack::PopAndDestroy(&file);
		}
		}
	}

void CT_NonNativeAppsStep::TestResultsOfAppForDocumentAndServiceL(TUid aServiceUid, TInt aError,
						const TUid& aAppUid, // aAppUid needs to be passed by reference (rather than by value) because of the way that this function is called, namely the aAppUid parameter seems to be put into the stack before the expression passed to the aError parameter has executed, the latter having the side-effect of changing the app-UID
						const TDataType& aDataType, const TDesC8& aExpectedDataType)
	{
	if (aServiceUid.iUid==0x10207f99)
		{
		User::LeaveIfError(aError);
		TEST(aDataType.Des8().CompareF(aExpectedDataType)==0);
		(aDataType.Des8().CompareF(KLit8_DataType_Gif)==0) ? TEST(aAppUid.iUid==0x10207f94) : TEST(aAppUid.iUid==0);
		}
	else
		{
		User::LeaveIfError(aError);
		TEST(aDataType.Des8().CompareF(aExpectedDataType)==0);
		TEST(aAppUid.iUid==0x10207f92);
		}
	}

/**
   @SYMTestCaseID T_NonNativeAppsStep-TestAppForDocumentAndServiceL
  
   @SYMDEF	DEF065742
  
   @SYMTestCaseDesc Tests AppForDocumentAndService API for non native applications.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions	Call AppForDocumentAndService, first with the file name, and then with the file data, with a series of different known files and serviceUIDs.
   		Note the following:
   			- TNNAPP1 defines in the datatype_list the 4 datatypes, while in the service_list, only 1 datatype is defined.
   			- TNNAPP2 defines no datatype_list, while it just contains 1 datatype in the service_list.
   			- The datatype of the file to be opened is searched in the service_list corresponding to the serviceUID. If not found,
   				the datatype_list of the same reg file is searched. If not found there, it returns a KNullUid as the app.
   			- It means that the service_list has 'priority' over the datatype_list in the same reg file.
   		API Calls:\n	
   		RApaLsSession::AppForDocumentAndService(const TDesC &aFileName, TUid aServiceUid, TUid &aAppUid, TDataType &aDataType) const;\n
   		RApaLsSession::AppForDocumentAndService(RFile &aFile, TUid aServiceUid, TUid &aAppUid, TDataType &aDataType) const;\n
   
   @SYMTestExpectedResults Test checks MIME type and UIDs obtained match the expected ones.
 */
void CT_NonNativeAppsStep::TestAppForDocumentAndServiceL(RApaLsSession& aApparcServer, RFs& aFileServer, const RTstFileArray& aFileArray)
	{
	INFO_PRINTF1(_L("Testing AppForDocumentAndServiceL()..."));
	for (TInt i=aFileArray.Count()-1; i>=0; --i)
		{
		TPtrC fullFileName;
		TPtrC8 expectedDataType;
		aFileArray.Get(fullFileName, expectedDataType, i);

		TPtrC fileNameWithoutDriveOrPath(TParsePtrC(fullFileName).NameAndExt());
		INFO_PRINTF2(_L("..using following file: '%S'"), &fileNameWithoutDriveOrPath);

		{
		TDataType dataType;
		TUid appUid;
		TUid serviceUid(TUid::Uid(0x10207f99));
		//due to access of file in private folder
		APPFWK_EXPECTED_PLATSEC_START;
		TestResultsOfAppForDocumentAndServiceL(serviceUid, aApparcServer.AppForDocumentAndService(fullFileName, serviceUid, appUid, dataType), appUid, dataType, expectedDataType);
		APPFWK_EXPECTED_PLATSEC_FINISH;
		serviceUid.iUid=0x10207f93;
		//due to access of file in private folder
		APPFWK_EXPECTED_PLATSEC_START;
		TestResultsOfAppForDocumentAndServiceL(serviceUid, aApparcServer.AppForDocumentAndService(fullFileName, serviceUid, appUid, dataType), appUid, dataType, expectedDataType);
		APPFWK_EXPECTED_PLATSEC_FINISH;
		}

		{
		RFile file;
		TInt originalFilePosition=0;
		OpenFileAndSetFilePositionToSomewhereUnusualLC(file, originalFilePosition, aFileServer, fullFileName);

		TDataType dataType;
		TUid appUid;
		TUid serviceUid(TUid::Uid(0x10207f99));
		TestResultsOfAppForDocumentAndServiceL(serviceUid, aApparcServer.AppForDocumentAndService(file, serviceUid, appUid, dataType), appUid, dataType, expectedDataType);
		serviceUid.iUid=0x10207f93;
		TestResultsOfAppForDocumentAndServiceL(serviceUid, aApparcServer.AppForDocumentAndService(file, serviceUid, appUid, dataType), appUid, dataType, expectedDataType);

		TestFilePosition(file, originalFilePosition);
		CleanupStack::PopAndDestroy(&file);
		}
		}
	}

/**
   @SYMTestCaseID T_NonNativeAppsStep-TestRecognizingL
  
   @SYMDEF	DEF065742
  
   @SYMTestCaseDesc Tests RecognizeData API for non native applications.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions	Call RecognizeData, first with the file name, and then with the file data, with a series of different known files.
   		API Calls:\n	
   		RApaLsSession::RecognizeData(const TDesC &aName, const TDesC8 &aBuffer, TDataRecognitionResult &aDataType) const;\n
   		RApaLsSession::RecognizeData(RFile &aFile, TDataRecognitionResult &aDataType) const;\n
   
   @SYMTestExpectedResults Test checks MIME type and conficence obtained match the expected ones.
 */
void CT_NonNativeAppsStep::TestRecognizingL(RApaLsSession& aApparcServer, RFs& aFileServer, const RTstFileArray& aFileArray)
	{
	INFO_PRINTF1(_L("Testing RecognizingL()..."));
	for (TInt i=aFileArray.Count()-1; i>=0; --i)
		{
		TPtrC fullFileName;
		TPtrC8 expectedDataType;
		aFileArray.Get(fullFileName, expectedDataType, i);

		TPtrC fileNameWithoutDriveOrPath(TParsePtrC(fullFileName).NameAndExt());
		INFO_PRINTF2(_L("..using following file: '%S'"), &fileNameWithoutDriveOrPath);

		{
		RFile file;
		TInt originalFilePosition=0;
		OpenFileAndSetFilePositionToSomewhereUnusualLC(file, originalFilePosition, aFileServer, fullFileName);

		TDataRecognitionResult dataRecognitionResult;
		dataRecognitionResult.Reset();
		User::LeaveIfError(aApparcServer.RecognizeData(file, dataRecognitionResult));
		TEST(dataRecognitionResult.iConfidence>=CApaDataRecognizerType::EProbable);
		TEST(dataRecognitionResult.iDataType.Des8().CompareF(expectedDataType)==0);

		TestFilePosition(file, originalFilePosition);
		CleanupStack::PopAndDestroy(&file);
		}

		{
		RFile file;
		TInt originalFilePosition=0;
		OpenFileAndSetFilePositionToSomewhereUnusualLC(file, originalFilePosition, aFileServer, fullFileName);

		TBool result=EFalse;
		User::LeaveIfError(aApparcServer.RecognizeSpecificData(file, TDataType(expectedDataType), result));
		TEST(result);

		TestFilePosition(file, originalFilePosition);
		CleanupStack::PopAndDestroy(&file);
		}

		{
		HBufC8* const buffer=OpenFileAndReadBufferLC(aApparcServer, aFileServer, fullFileName);

		TDataRecognitionResult dataRecognitionResult;
		dataRecognitionResult.Reset();
		APPFWK_EXPECTED_PLATSEC_START;
		User::LeaveIfError(aApparcServer.RecognizeData(fullFileName, *buffer, dataRecognitionResult));
		APPFWK_EXPECTED_PLATSEC_FINISH;
		TEST(dataRecognitionResult.iConfidence>=CApaDataRecognizerType::EProbable);
		TEST(dataRecognitionResult.iDataType.Des8().CompareF(expectedDataType)==0);

		CleanupStack::PopAndDestroy(buffer);
		}

		{
		HBufC8* const buffer=OpenFileAndReadBufferLC(aApparcServer, aFileServer, fullFileName);

		TBool result=EFalse;
		User::LeaveIfError(aApparcServer.RecognizeSpecificData(fullFileName, *buffer, TDataType(expectedDataType), result));
		TEST(result);

		CleanupStack::PopAndDestroy(buffer);
		}
		}
	}

/**
   @SYMTestCaseID T_NonNativeAppsStep-TestStartDocumentL
  
   @SYMDEF	DEF065742
  
   @SYMTestCaseDesc Tests StartDocument API for non native applications.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions	Call StartDocument, either with the file name as a parameter or a file handle. The test presupposes that when
   		the file is passed by name, it can't be oppened as it's in a private folder (since the app doesn't have AllFiles capability).
   		These provokes few negative platsec errors in the epocwind.out file.
   		On the other hand, when the file is passed by handle, there are no problems opening the file. No platsec errors should be reported.
   		API Calls:\n	
   		RApaLsSession::StartDocument(const TDesC &aFileName, TThreadId &aThreadId, TLaunchType aLaunchType=ELaunchNewApp);\n
   		RApaLsSession::StartDocument(RFile &aFile, TThreadId &aThreadId, TRequestStatus *aRequestStatusForRendezvous=0);\n
   		RApaLsSession::StartDocument(const TDesC &aFileName, const TDataType &aDataType, TThreadId &aThreadId, TLaunchType aLaunchType=ELaunchNewApp);\n
   		RApaLsSession::StartDocument(RFile &aFile, const TDataType &aDataType, TThreadId &aThreadId, TRequestStatus *aRequestStatusForRendezvous=0);\n
   
   @SYMTestExpectedResults Test the requeststatus obtained match the expected one.
 */
void CT_NonNativeAppsStep::TestStartDocumentL(RApaLsSession& aApparcServer, RFs& aFileServer, const RTstFileArray& aFileArray)
	{
	INFO_PRINTF1(_L("Testing StartDocumentL()..."));
	for (TInt i=aFileArray.Count()-1; i>=0; --i)
		{
		TPtrC fullFileName;
		TPtrC8 expectedDataType;
		aFileArray.Get(fullFileName, expectedDataType, i);

		TPtrC fileNameWithoutDriveOrPath(TParsePtrC(fullFileName).NameAndExt());
		INFO_PRINTF2(_L("..using following file: '%S'"), &fileNameWithoutDriveOrPath);

		for (TInt j=0; j<2; ++j)
			{
			const TUid serviceUid=(j==0)? KTstServiceUid_nonSecure: KTstServiceUid_secure;
			{
			RFile file;
			TInt originalFilePosition=0;
			OpenFileAndSetFilePositionToSomewhereUnusualLC(file, originalFilePosition, aFileServer, fullFileName);

			TThreadId threadId;
			TRequestStatus requestStatusForRendezvous;
			User::LeaveIfError(aApparcServer.StartDocument(file, threadId, &requestStatusForRendezvous));
			User::WaitForRequest(requestStatusForRendezvous);
			TEST(RTstAppService::DoTestL(serviceUid, ETrue, fileNameWithoutDriveOrPath) == KCheckPass);

			TestFilePosition(file, originalFilePosition);
			CleanupStack::PopAndDestroy(&file);
			}

			{
			RFile file;
			TInt originalFilePosition=0;
			OpenFileAndSetFilePositionToSomewhereUnusualLC(file, originalFilePosition, aFileServer, fullFileName);

			TThreadId threadId;
			TRequestStatus requestStatusForRendezvous;
			User::LeaveIfError(aApparcServer.StartDocument(file, TDataType(expectedDataType), threadId, &requestStatusForRendezvous));
			User::WaitForRequest(requestStatusForRendezvous);
			TEST(RTstAppService::DoTestL(serviceUid, ETrue, fileNameWithoutDriveOrPath) == KCheckPass);

			TestFilePosition(file, originalFilePosition);
			CleanupStack::PopAndDestroy(&file);
			}

			{
			RFile file;
			TInt originalFilePosition=0;
			OpenFileAndSetFilePositionToSomewhereUnusualLC(file, originalFilePosition, aFileServer, fullFileName);

			TThreadId threadId;
			TRequestStatus requestStatusForRendezvous;
			User::LeaveIfError(aApparcServer.StartDocument(file, TUid::Uid(0x10207f92), threadId, &requestStatusForRendezvous));
			User::WaitForRequest(requestStatusForRendezvous);
			TEST(RTstAppService::DoTestL(serviceUid, ETrue, fileNameWithoutDriveOrPath) == KCheckPass);

			TestFilePosition(file, originalFilePosition);
			CleanupStack::PopAndDestroy(&file);
			}

			{
			TThreadId threadId;
			//due to access of file in private folder
			APPFWK_EXPECTED_PLATSEC_START;
			User::LeaveIfError(aApparcServer.StartDocument(fullFileName, threadId));
			APPFWK_EXPECTED_PLATSEC_FINISH;
			User::After(1500000);
			TEST(RTstAppService::DoTestL(serviceUid, EFalse, fileNameWithoutDriveOrPath) == KCheckPass);
			}

			{
			TThreadId threadId;
			User::LeaveIfError(aApparcServer.StartDocument(fullFileName, TDataType(expectedDataType), threadId));
			User::After(1500000);
			TEST(RTstAppService::DoTestL(serviceUid, EFalse, fileNameWithoutDriveOrPath) == KCheckPass);
			}

			{
			TThreadId threadId;
			User::LeaveIfError(aApparcServer.StartDocument(fullFileName, TUid::Uid(0x10207f92), threadId));
			User::After(1500000);
			TEST(RTstAppService::DoTestL(serviceUid, EFalse, fileNameWithoutDriveOrPath) == KCheckPass);
			}
			}
		}
	//avoid race conditions
	User::After(1500000);
	}
	
void CT_NonNativeAppsStep::CheckPropertyUpdateAndResetL(const TDesC& aExpectedNewValue)
	{
	__ASSERT_ALWAYS(aExpectedNewValue.Length()>0, User::Invariant());
	RProperty property;
	CleanupClosePushL(property);
	User::LeaveIfError(property.Attach(TUid::Uid(KPropertyCategory), KPropertyKey));
	TRequestStatus requestStatus;
	property.Subscribe(requestStatus);
	TBuf<50> propertyValue;
	User::LeaveIfError(property.Get(propertyValue));
	if (propertyValue.Length()>0)
		{
		property.Cancel();
		}
	User::WaitForRequest(requestStatus);
	if (propertyValue.Length()==0)
		{
		User::LeaveIfError(property.Get(propertyValue));
		}
	TEST(propertyValue.CompareF(aExpectedNewValue)==0);
	User::LeaveIfError(property.Set(KNullDesC)); // reset it to an empty descriptor
	CleanupStack::PopAndDestroy(&property);
	}

void CT_NonNativeAppsStep::CheckApplicationLaunchesOkayL(RApaLsSession& aApparcServer, const CApaCommandLine& aCommandLine, TAny* aOption)
	{
	TThreadId notUsed;
	const TInt option = TInt(aOption);
	switch(option)
		{
			case EAppA:
				INFO_PRINTF1(_L("..Checking ApplicationLaunchesOkayL for app 'A'.."));
				User::LeaveIfError(aApparcServer.StartDocument(KLitDocumentName1, TDataType(KLitMimeTypeA), notUsed));
				CheckPropertyUpdateAndResetL(KLitDocumentName1);
				User::LeaveIfError(aApparcServer.StartApp(aCommandLine));
				CheckPropertyUpdateAndResetL(KLitDocumentName2);
				break;
			case EAppB:
				INFO_PRINTF1(_L("..Checking ApplicationLaunchesOkayL for app 'B'.."));
				TEST( aApparcServer.StartDocument(KLitDocumentName3, TDataType(KLitMimeTypeB), notUsed) == KErrNone);
				break;
			default:
				break;
		}
	}

void CT_NonNativeAppsStep::CheckApplicationFailsToLaunchL(RApaLsSession& aApparcServer, const CApaCommandLine& aCommandLine, TAny* aOption)
	{
	TThreadId notUsed;
	const TInt option = TInt(aOption);
	switch(option)
		{
			case EAppA:
				INFO_PRINTF1(_L("..Checking ApplicationFailsToLaunchL for app 'A'.."));
				TEST(aApparcServer.StartDocument(KLitDocumentName1, TDataType(KLitMimeTypeA), notUsed) != KErrNone);
				TEST(aApparcServer.StartApp(aCommandLine)!=KErrNone);
				break;
			case EAppB:
				INFO_PRINTF1(_L("..Checking ApplicationFailsToLaunchL for app 'B'.."));
				TEST( aApparcServer.StartDocument(KLitDocumentName3, TDataType(KLitMimeTypeB), notUsed) != KErrNone);
				break;
			default:
				break;
		}
	}
	
/**
   @SYMTestCaseID T_NonNativeAppsStep-TestRegisterNonNativeApplicationL
  
   @SYMDEF	DEF065742
  
   @SYMTestCaseDesc Tests RegisterNonNativeApplicationL and DeregisterNonNativeApplicationL API for non native applications.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call RegisterNonNativeApplicationL and try to launch the app. Then call 
   DeregisterNonNativeApplicationL and try to launch the app again. This is repeated with 
   localisable resource files, etc., etc.. 
   		API Calls:\n	
   		RApaLsSession::RegisterNonNativeApplicationL(TUid aApplicationType, const TDriveUnit& aDrive, CApaRegistrationResourceFileWriter& aRegistrationResourceFile, CApaLocalisableResourceFileWriter* aLocalisableResourceFile, RFile* aIconFile); \n
   		RApaLsSession::DeregisterNonNativeApplicationL(TUid aApplication);\n
   		RApaLsSession::GetAppIcon(TUid aAppUid, RFile& aFile) const; \n
   
   @SYMTestExpectedResults Test checks if app is registered (and launches ok) and deregistered (and fails to launch).
 */
void CT_NonNativeAppsStep::TestRegisterNonNativeApplicationL(RApaLsSession& aApparcServer, RFs& aFileServer )
	{
	INFO_PRINTF1(_L("Testing RegisterNonNativeApplicationL()..."));

	RProcess().SetPriority(EPriorityLow); // so that we're pre-empted by the Apparc server when it does its scan

	CApaCommandLine* commandLine=NULL;
	User::LeaveIfError(CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine));
	CleanupStack::PushL(commandLine);
	const TPtrC executableName(commandLine->ExecutableName());

	_LIT_SECURITY_POLICY_S0(readSecurityPolicy, KMySID);
	_LIT_SECURITY_POLICY_PASS(writeSecurityPolicy);
	const TInt error=RProperty::Define(KPropertyKey, RProperty::EText, readSecurityPolicy, writeSecurityPolicy);
	if (error!=KErrAlreadyExists)
		{
		User::LeaveIfError(error);
		}
	User::LeaveIfError(RProperty::Set(TUid::Uid(KPropertyCategory), KPropertyKey, KNullDesC));

	// make sure we're starting from a clean sheet
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationTypeL(TUid::Uid(KApplicationType));
	CleanupStack::PushL(TCleanupItem(DeregisterNonNativeL, &aApparcServer));

	aApparcServer.RegisterNonNativeApplicationTypeL(TUid::Uid(KApplicationType), KLitNativeExecutable);

	CApaRegistrationResourceFileWriter* const registrationResourceFileWriter=CApaRegistrationResourceFileWriter::NewL(TUid::Uid(KApplicationA), KLitLogicalExecutableA, TApaAppCapability::ENonNative);
	CleanupStack::PushL(registrationResourceFileWriter);
	registrationResourceFileWriter->AddDataTypeL(KDataTypePriorityNormal, KLitMimeTypeA);

	CApaCommandLine* const nextCommandLine=CApaCommandLine::NewLC();
	nextCommandLine->SetDocumentNameL(KLitDocumentName2);
	nextCommandLine->SetExecutableNameL(KLitLogicalExecutableA);
	
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, *registrationResourceFileWriter, NULL, NULL);
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	CheckApplicationLaunchesOkayL(aApparcServer, *nextCommandLine, (TAny*)EAppA);
	//Check if there is no problem in registering same app again(basically simulating an upgrade)
	INFO_PRINTF1(_L("..registering same app again using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, *registrationResourceFileWriter, NULL, NULL);
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	CheckApplicationLaunchesOkayL(aApparcServer, *nextCommandLine, (TAny*)EAppA);
	
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	CheckApplicationFailsToLaunchL(aApparcServer, *nextCommandLine, (TAny*)EAppA);
	
	_LIT8(KLitOpData, "opaquedata");
	registrationResourceFileWriter->SetOpaqueDataL(KLitOpData);

	CApaLocalisableResourceFileWriter* const localisableResourceFileWriter_noIcons=CApaLocalisableResourceFileWriter::NewL(KNullDesC, KLitApplicationCaptionA, 0, KNullDesC);
	CleanupStack::PushL(localisableResourceFileWriter_noIcons);
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() with localisableResourceFileWriter_noIcons"));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, *registrationResourceFileWriter, localisableResourceFileWriter_noIcons, NULL);
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	CleanupStack::PopAndDestroy(localisableResourceFileWriter_noIcons);
	CheckApplicationLaunchesOkayL(aApparcServer, *nextCommandLine, (TAny*)EAppA);

	//Testing with OpaqueData
	TPtrC8 opData =nextCommandLine->OpaqueData();
	TInt tst=opData.CompareF(KLitOpData);
	TEST(opData.CompareF(KLitOpData)==0);
		
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() with localisableResourceFileWriter_noIcons"));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	CheckApplicationFailsToLaunchL(aApparcServer, *nextCommandLine, (TAny*)EAppA);

	//Testing with IconFile
	CApaLocalisableResourceFileWriter* const localisableResourceFileWriter_withIcons=CApaLocalisableResourceFileWriter::NewL(KNullDesC, KLitApplicationCaptionA, 2, KNullDesC);
	CleanupStack::PushL(localisableResourceFileWriter_withIcons);
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() with localisableResourceFileWriter_withIcons"));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();

	RFile iconFile;
	CleanupClosePushL(iconFile);
	_LIT(KIconFullFileName, "z:\\resource\\apps\\tstapp.mbm");
	User::LeaveIfError(iconFile.Open(aFileServer, KIconFullFileName, EFileRead));
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, *registrationResourceFileWriter, localisableResourceFileWriter_withIcons, &iconFile);
	iconFile.Close();
	CleanupStack::PopAndDestroy(&iconFile);
	
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	CheckApplicationLaunchesOkayL(aApparcServer, *nextCommandLine, (TAny*)EAppA);

	HBufC* iconFileName = NULL;
	TInt err = aApparcServer.GetAppIcon(TUid::Uid(KApplicationA),iconFileName);
	TEST(err == KErrNone);
	_LIT(KIconFileName, "tstapp.mbm");
	TEST(iconFileName != NULL);
	if(iconFileName != NULL)
		{
		TEST(iconFileName->Des().Find(KIconFileName) >= 0);
		delete iconFileName;
		}
	
	//Test other overload of GetAppIcon for MBM icon file.
	RFile mbmIconFile;
	CleanupClosePushL(mbmIconFile);
	INFO_PRINTF1(_L("..testing GetAppIcon for non-native app to retrieve an open file handle to an MBM icon file"));
	err = aApparcServer.GetAppIcon(TUid::Uid(KApplicationA), mbmIconFile);
	TEST(err == KErrNone);
	
	TBuf<KMaxFileName> mbmIconFileName;
	mbmIconFile.FullName(mbmIconFileName);
	TEST(mbmIconFileName.Length() != 0);	
	
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
	
	//First KBytesToRead bytes of the icon file, this should be same as in mbm file.
	RFile tempFile;
	CleanupClosePushL(tempFile);
	err = tempFile.Open(aFileServer, KIconFullFileName, EFileRead);
	TEST(err == KErrNone);
	TBuf8<KBytesToRead> buffer1;
	err = tempFile.Read(buffer1);
	TEST(err == KErrNone);	
	TEST(buffer == buffer1);
	
	//Close icon files.
	CleanupStack::PopAndDestroy(2, &mbmIconFile);
	
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() with localisableResourceFileWriter_withIcons"));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	CheckApplicationFailsToLaunchL(aApparcServer, *nextCommandLine, (TAny*)EAppA);
	
	//Testing GetAppIcon API for non-native application with SVG icon.
	TestGetAppIconForNonNativeL(aApparcServer,aFileServer, registrationResourceFileWriter, localisableResourceFileWriter_withIcons);
	
 	// clean-up
	CleanupStack::PopAndDestroy(5, commandLine);
	}

/**
   @SYMTestCaseID APPFWK-APPARC-0072
  
   @SYMDEF INC098717: support for scalable midlet icons is missing 
  
   @SYMTestCaseDesc Tests GetAppIcon API for non native applications with SVG icon file.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Register a non-native app with a SVG icon file, Call GetAppIcon API, 
   deregister the app. Again register the same app without the icon file, call GetAppIcon 
   API, deregister the app. Call GetAppIcon with an invalid app uid.
   
   @SYMTestExpectedResults RApaLsSession::GetAppIcon should return KErrNotFound when 
   an invalid app uid is passed, KErrNotFound when application does not have an icon 
   file, KErrNone and an open file handle to the application's icon file if app has icon file.
 */
void CT_NonNativeAppsStep::TestGetAppIconForNonNativeL(RApaLsSession& aApparcServer,
                                                       RFs& aFileServer, 
                                                       CApaRegistrationResourceFileWriter* const aRegistrationResourceFileWriter, 
                                                       CApaLocalisableResourceFileWriter* const aLocalisableResourceFileWriter)
	{
	//Testing GetAppIcon for non-native app having SVG icon file
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() with localisableResourceFileWriter_withIcons"));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();

	RFile iconFile;
	CleanupClosePushL(iconFile);
	_LIT(KSVGIconSourceFile, "z:\\resource\\apps\\svg_icon.svg");
	User::LeaveIfError(iconFile.Open(aFileServer, KSVGIconSourceFile, EFileRead));
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, *aRegistrationResourceFileWriter, aLocalisableResourceFileWriter, &iconFile);
	CleanupStack::PopAndDestroy(&iconFile);
	
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	
	RFile svgIconFile;
	CleanupClosePushL(svgIconFile);
	INFO_PRINTF1(_L("..testing GetAppIcon for non-native app to retrieve an open file handle to an SVG icon file"));
	TInt err = aApparcServer.GetAppIcon(TUid::Uid(KApplicationA),svgIconFile);
	TEST(err == KErrNone);
	
	_LIT(KSVGIconFileName, "c:\\private\\10003a3f\\import\\apps\\nonnative\\resource\\svg_icon.svg");
	TBuf<KMaxFileName> iconFileName;
	//Get the name of the icon file 
	svgIconFile.FullName(iconFileName);
	TEST(iconFileName.Length() != 0);
	if (iconFileName.Length() != 0)
		{
		INFO_PRINTF1(_L("..testing if the SVG icon file for non-native app gets installed in apparc's data cage"));
		iconFileName.LowerCase();
		TEST(iconFileName.Compare(KSVGIconFileName) == 0);
		}	
		
	//Try to write into the icon file whose handle was returned.
	INFO_PRINTF1(_L("..writing into the SVG icon file whose handle was returned."));
	_LIT8(KSomeText, "It would not be written into the file");
	err = svgIconFile.Write(KSomeText);
	TEST(err == KErrAccessDenied);
	
	//Try to read from the icon file whose handle was returned.
	INFO_PRINTF1(_L("..reading from the SVG icon file whose handle was returned."));
	TBuf8<KBytesToRead> buffer; //buffer to read first KBytesToRead bytes of the icon file. 
	err = svgIconFile.Read(buffer);
	TEST(err == KErrNone);
	
	//First KBytesToRead bytes of the icon file, this should be same as in svg file.
	RFile tempFile;
	CleanupClosePushL(tempFile);
	err = tempFile.Open(aFileServer, KSVGIconSourceFile, EFileRead);
	TEST(err == KErrNone);
	TBuf8<KBytesToRead> buffer1;
	err = tempFile.Read(buffer1);
	TEST(err == KErrNone);
	TEST(buffer == buffer1);
	
	//Close icon files
	CleanupStack::PopAndDestroy(2, &svgIconFile);
		
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() with localisableResourceFileWriter_withIcons"));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	
	//Testing GetAppIcon for non-native app without SVG Icon file.
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() with localisableResourceFileWriter_withIcons"));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, *aRegistrationResourceFileWriter, aLocalisableResourceFileWriter, NULL);		
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	
	INFO_PRINTF1(_L("..testing GetAppIcon for non-native app that does not have an icon file"));
	RFile iconFile1;
	CleanupClosePushL(iconFile1);
	err = aApparcServer.GetAppIcon(TUid::Uid(KApplicationA),iconFile1);
	TEST(err == KErrNotFound);
			
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() with localisableResourceFileWriter_withIcons"));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	
	//Testing GetAppIcon with an invalid app UID.
	INFO_PRINTF1(_L("..testing GetAppIcon, if an invalid app UID is passed"));
	err = aApparcServer.GetAppIcon(TUid::Uid(KApplicationA),iconFile1);
	TEST(err == KErrNotFound);
	
	//Close icon file
	CleanupStack::PopAndDestroy(&iconFile1);
	}


/**
   @SYMTestCaseID APPFWK-APPARC-0099
  
   @SYMDEF	PDEF129466
  
   @SYMTestCaseDesc Tests ForceCommitNonNativeApplicationsUpdatesL() API.
  
   @SYMTestPriority High
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Call RegisterNonNativeApplicationL and try to launch the app. Then call 
   DeregisterNonNativeApplicationL and try to launch the app again. ForceCommitNonNativeApplicationsUpdatesL
   API is used to commit the non-native application updates.
   
   @SYMTestExpectedResults Test checks ForceCommitNonNativeApplicationsUpdatesL behaviour and completed with success.
 */
void CT_NonNativeAppsStep::TestCommitNNAppUpdatesL(RApaLsSession& aApparcServer)
	{
	INFO_PRINTF1(_L("Testing ForceCommitNonNativeApplicationsUpdatesL API..."));

	RProcess().SetPriority(EPriorityLow); // so that we're pre-empted by the Apparc server when it does its scan

	CApaCommandLine* commandLine=NULL;
	User::LeaveIfError(CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine));
	CleanupStack::PushL(commandLine);
	const TPtrC executableName(commandLine->ExecutableName());


	_LIT_SECURITY_POLICY_S0(readSecurityPolicy, KMySID);
	_LIT_SECURITY_POLICY_PASS(writeSecurityPolicy);
	
	//TNNAPP2.exe sets the document name passed as part of the command line to this property.
	const TInt error=RProperty::Define(KPropertyKey, RProperty::EText, readSecurityPolicy, writeSecurityPolicy);
	if (error!=KErrAlreadyExists)
		{
		User::LeaveIfError(error);
		}
	User::LeaveIfError(RProperty::Set(TUid::Uid(KPropertyCategory), KPropertyKey, KNullDesC));

	// make sure we're starting from a clean sheet
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationTypeL(TUid::Uid(KApplicationType));
	CleanupStack::PushL(TCleanupItem(DeregisterNonNativeL, &aApparcServer));

	aApparcServer.RegisterNonNativeApplicationTypeL(TUid::Uid(KApplicationType), KLitNativeExecutable);

	CApaRegistrationResourceFileWriter* const registrationResourceFileWriter=CApaRegistrationResourceFileWriter::NewL(TUid::Uid(KApplicationA), KLitLogicalExecutableA, TApaAppCapability::ENonNative);
	CleanupStack::PushL(registrationResourceFileWriter);
	registrationResourceFileWriter->AddDataTypeL(KDataTypePriorityNormal, KLitMimeTypeA);

	CApaCommandLine* const nextCommandLine=CApaCommandLine::NewLC();
	nextCommandLine->SetDocumentNameL(KLitDocumentName2);
	nextCommandLine->SetExecutableNameL(KLitLogicalExecutableA);
	
	
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, *registrationResourceFileWriter, NULL, NULL);
	TTime startTime,endTime;
	TTimeIntervalMicroSeconds timeTakenToCommitRegistration;
	INFO_PRINTF1(_L("..start commiting the registration with CommitNonNativeApplicationsUpdatesL"));
	startTime.HomeTime();
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	endTime.HomeTime();
	INFO_PRINTF1(_L("..commiting the registration is done"));	
	timeTakenToCommitRegistration=endTime.MicroSecondsFrom(startTime);

	TInt64 value = timeTakenToCommitRegistration.Int64();
	INFO_PRINTF2(_L("..Time taken for registration of application by CommitNonNativeApplicationsUpdatesL is %d micro seconds"), value);	
	CheckApplicationLaunchesOkayL(aApparcServer, *nextCommandLine, (TAny*)EAppA);
	
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	TTimeIntervalMicroSeconds timeTakenToCommitDeregistration;
	INFO_PRINTF1(_L("..start commiting the deregistration with CommitNonNativeApplicationsUpdatesL"));
	startTime.HomeTime();	
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	endTime.HomeTime();	
	INFO_PRINTF1(_L("..commiting the deregistration is done"));
	timeTakenToCommitDeregistration=endTime.MicroSecondsFrom(startTime);
	value = timeTakenToCommitDeregistration.Int64();
	INFO_PRINTF2(_L("..Time taken for deregistration of application by CommitNonNativeApplicationsUpdatesL is %d micro seconds"), value);	
	CheckApplicationFailsToLaunchL(aApparcServer, *nextCommandLine, (TAny*)EAppA);

	
	INFO_PRINTF1(_L("..registering app using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, *registrationResourceFileWriter, NULL, NULL);
	TTimeIntervalMicroSeconds timeTakenToForceCommitRegistration;
	INFO_PRINTF1(_L("..start commiting the registration with ForceCommitNonNativeApplicationsUpdatesL"));
	startTime.HomeTime();
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	endTime.HomeTime();
	INFO_PRINTF1(_L("..commiting the registration is done"));
	timeTakenToForceCommitRegistration=endTime.MicroSecondsFrom(startTime);
	value = timeTakenToForceCommitRegistration.Int64();
	INFO_PRINTF2(_L("..Time taken for registration of application by ForceCommitNonNativeApplicationsUpdatesL is %d micro seconds"), value);
	TRequestStatus appScanCompleted=KRequestPending; 
	aApparcServer.SetNotify(EFalse,appScanCompleted);    
	User::WaitForRequest(appScanCompleted); 
	TEST(appScanCompleted.Int()==MApaAppListServObserver::EAppListChanged);
	CheckApplicationLaunchesOkayL(aApparcServer, *nextCommandLine, (TAny*)EAppA);
	
	INFO_PRINTF1(_L("..deregistering app using DeregisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	TTimeIntervalMicroSeconds timeTakenToForceCommitDeregistration;
	INFO_PRINTF1(_L("..start commiting the deregistration with ForceCommitNonNativeApplicationsUpdatesL"));
	startTime.HomeTime();	
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	endTime.HomeTime();	
	INFO_PRINTF1(_L("..commiting the deregistration is done"));
	timeTakenToForceCommitDeregistration=endTime.MicroSecondsFrom(startTime);
	value = timeTakenToForceCommitDeregistration.Int64();
	INFO_PRINTF2(_L("..Time taken for deregistration of application by ForceCommitNonNativeApplicationsUpdatesL is %d micro seconds"), value);
	appScanCompleted=KRequestPending; 
	aApparcServer.SetNotify(EFalse,appScanCompleted);    
	User::WaitForRequest(appScanCompleted);
	TEST(appScanCompleted.Int()==MApaAppListServObserver::EAppListChanged);
	CheckApplicationFailsToLaunchL(aApparcServer, *nextCommandLine, (TAny*)EAppA);
	TEST(timeTakenToForceCommitRegistration<timeTakenToCommitRegistration);	
	TEST(timeTakenToForceCommitDeregistration<timeTakenToCommitDeregistration);
	
	CleanupStack::PopAndDestroy(4, commandLine);
	}
	
/**
   @SYMTestCaseID 			APPFWK-APPARC-0103
  
   @SYMDEF					PDEF134174
  
   @SYMTestCaseDesc 		Tests ForceCommitNonNativeApplicationsUpdatesL() API.
  
   @SYMTestPriority 		High
  
   @SYMTestStatus 			Implemented
   
   @SYMTestActions 			Make back to back installations of apps and notice that the app in not updated to the applist
   							immediately. All apps are updated only after the last ForceCommitNonNativeApplicationsUpdatesL.
   				   			Note that previous ForceCommitNonNativeApplicationsUpdatesL actions are updates in applist 
   							when RollbackNonNativeApplicationsUpdates is called.
   
   @SYMTestExpectedResults 	Test checks ForceCommitNonNativeApplicationsUpdatesL and RollbackNonNativeApplicationsUpdates behaviour and completed with success.
 */
 void CT_NonNativeAppsStep::TestForceCommitNNAppUpdatesL(RApaLsSession& aApparcServer)
 	{
 	INFO_PRINTF1(_L("Testing ForceCommitNonNativeApplicationsUpdatesL API..."));

	RProcess().SetPriority(EPriorityLow); // so that we're pre-empted by the Apparc server when it does its scan

	CApaCommandLine* commandLine=NULL;
	User::LeaveIfError(CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine));
	CleanupStack::PushL(commandLine);
	const TPtrC executableName(commandLine->ExecutableName());


	_LIT_SECURITY_POLICY_S0(readSecurityPolicy, KMySID);
	_LIT_SECURITY_POLICY_PASS(writeSecurityPolicy);
	
	//TNNAPP2.exe sets the document name passed as part of the command line to this property.
	const TInt error=RProperty::Define(KPropertyKey, RProperty::EText, readSecurityPolicy, writeSecurityPolicy);
	if (error!=KErrAlreadyExists)
		{
		User::LeaveIfError(error);
		}
	User::LeaveIfError(RProperty::Set(TUid::Uid(KPropertyCategory), KPropertyKey, KNullDesC));

	// make sure we're starting from a clean sheet
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.CommitNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationTypeL(TUid::Uid(KApplicationType));
	CleanupStack::PushL(TCleanupItem(DeregisterNonNativeL, &aApparcServer));

	INFO_PRINTF1(_L("..Registering type KApplicationType using RegisterNonNativeApplicationTypeL() "));
	aApparcServer.RegisterNonNativeApplicationTypeL(TUid::Uid(KApplicationType), KLitNativeExecutable);

	CApaRegistrationResourceFileWriter* const registrationResourceFileWriter=CApaRegistrationResourceFileWriter::NewL(TUid::Uid(KApplicationA), KLitLogicalExecutableA, TApaAppCapability::ENonNative);
	CleanupStack::PushL(registrationResourceFileWriter);
	INFO_PRINTF1(_L("..Add datatype to app 'A'"));
	registrationResourceFileWriter->AddDataTypeL(KDataTypePriorityNormal, KLitMimeTypeA);

	CApaCommandLine* const nextCommandLine=CApaCommandLine::NewLC();
	nextCommandLine->SetDocumentNameL(KLitDocumentName2);
	nextCommandLine->SetExecutableNameL(KLitLogicalExecutableA);
	
	TApaAppInfo info;
	CApaRegistrationResourceFileWriter* const registrationResourceFileWriterB=CApaRegistrationResourceFileWriter::NewL(TUid::Uid(KApplicationB), KLitLogicalExecutableA, TApaAppCapability::ENonNative);
	CleanupStack::PushL(registrationResourceFileWriterB);
	registrationResourceFileWriterB->AddDataTypeL(KDataTypePriorityNormal, KLitMimeTypeB);
	
	TestForceCommitL(aApparcServer, *registrationResourceFileWriter, *registrationResourceFileWriterB, *nextCommandLine);
	TestRollbackRegistrationL(aApparcServer, *registrationResourceFileWriter, *registrationResourceFileWriterB, *nextCommandLine);
	TestRollbackDeregistrationL(aApparcServer, *registrationResourceFileWriter, *registrationResourceFileWriterB, *nextCommandLine);
	
	CleanupStack::PopAndDestroy(5, commandLine);
 	}
 	
 void CT_NonNativeAppsStep::WaitForApplistUpdate(RApaLsSession& aApparcServer)
 	{
 	INFO_PRINTF1(_L("..waiting for applist to be updated "));
 	TRequestStatus appScanCompleted=KRequestPending; 
	aApparcServer.SetNotify(EFalse,appScanCompleted);    
	User::WaitForRequest(appScanCompleted);
	TEST(appScanCompleted.Int()==MApaAppListServObserver::EAppListChanged);
 	}
 	
 void CT_NonNativeAppsStep::TestForceCommitL(RApaLsSession& aApparcServer, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriter, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriterB, CApaCommandLine& aNextCommandLine)
 	{
 	INFO_PRINTF1(_L("..Start Testing TestForceCommitL API..."));
 	
 	//Register app A and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..registering app 'A' using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, aRegistrationResourceFileWriter, NULL, NULL);
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppA);//Since applist update has not yet completed
	
	//Register app B and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..registering app 'B' using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, aRegistrationResourceFileWriterB, NULL, NULL);
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	TThreadId notUsed2;
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppB);
	
	//Wait for applist to be updated
	WaitForApplistUpdate(aApparcServer);
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppA);//Check if app A launches since applist update is complete now
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppB);//Similarly check if app B launches
	
	//DeRegister app B and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..deregistering app 'B' using DeregisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationB));
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	//Since applist update has not yet completed, both apps should launch
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppA); 
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppB);
	
	//Wait for applist to be updated
	WaitForApplistUpdate(aApparcServer);
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppA);
	//Check if app B fails to launches Since applist update is complete now
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppB);
	
	//Deregister app A and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..deregistering app 'A' using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	WaitForApplistUpdate(aApparcServer);
	//Both apps fail to launch since they are deregistered
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppA);
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppB);
	
	INFO_PRINTF1(_L("..End Testing TestForceCommitL API..."));
 	}
 
  void CT_NonNativeAppsStep::TestRollbackRegistrationL(RApaLsSession& aApparcServer, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriter, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriterB, CApaCommandLine& aNextCommandLine)
 	{
 	INFO_PRINTF1(_L("..Start Testing TestRollbackRegistrationL API..."));
 	
 	//Register app A and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..registering app 'A' using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, aRegistrationResourceFileWriter, NULL, NULL);
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppA);//Since applist update has not yet completed
	
	//Register app B and RollbackNNAUpdates
	INFO_PRINTF1(_L("..registering app 'B' using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, aRegistrationResourceFileWriterB, NULL, NULL);
	aApparcServer.RollbackNonNativeApplicationsUpdates();
	
	//Wait for applist to be updated
	WaitForApplistUpdate(aApparcServer);
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppA);//Check if app A launches
	//App B's registration was rolled back, so cannot launch it
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppB);
	
	//DeRegister app A and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..deregistering app 'A' using DeregisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	//Since applist update has not yet completed, app A should launch
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppA); 
	
	//Wait for applist to be updated
	WaitForApplistUpdate(aApparcServer);
	//Both apps fail to launch
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppA);
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppB);
	
	INFO_PRINTF1(_L("..End Testing TestRollbackRegistrationL API..."));
	}
 	
  void CT_NonNativeAppsStep::TestRollbackDeregistrationL(RApaLsSession& aApparcServer, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriter, CApaRegistrationResourceFileWriter& aRegistrationResourceFileWriterB, CApaCommandLine& aNextCommandLine)
 	{
 	INFO_PRINTF1(_L("..Start Testing TestRollbackDeregistrationL API..."));
 	
 	//Register app A and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..registering app 'A' using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, aRegistrationResourceFileWriter, NULL, NULL);
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppA);//Since applist update has not yet completed
	
	//Register app B and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..registering app 'B' using RegisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.RegisterNonNativeApplicationL(TUid::Uid(KApplicationType), EDriveC, aRegistrationResourceFileWriterB, NULL, NULL);
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppB);
	
	//Wait for applist to be updated
	WaitForApplistUpdate(aApparcServer);
	//Check if both apps launches since applist update is complete now
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppA);
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppB);
	
	//DeRegister app B and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..deregistering app 'B' using DeregisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationB));
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();
	
	//DeRegister app A and ForceCommitNNAUpdate
	INFO_PRINTF1(_L("..deregistering app 'A' using DeregisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.RollbackNonNativeApplicationsUpdates();
	
	//Wait for applist to be updated
	WaitForApplistUpdate(aApparcServer);
	//Since deregistration of app A was rolled back it still launches
	CheckApplicationLaunchesOkayL(aApparcServer, aNextCommandLine, (TAny*)EAppA); 
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppB);

	INFO_PRINTF1(_L("..deregistering app 'A' using DeregisterNonNativeApplicationL() "));
	aApparcServer.PrepareNonNativeApplicationsUpdatesL();
	aApparcServer.DeregisterNonNativeApplicationL(TUid::Uid(KApplicationA));
	aApparcServer.ForceCommitNonNativeApplicationsUpdatesL();

	//Wait for applist to be updated
	WaitForApplistUpdate(aApparcServer);
	//Both apps fail to launch
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppA);
	CheckApplicationFailsToLaunchL(aApparcServer, aNextCommandLine, (TAny*)EAppB);
	
	INFO_PRINTF1(_L("..End Testing TestRollbackDeregistrationL API..."));
 	}
 
// CTestStep derived functions
	
CT_NonNativeAppsStep::~CT_NonNativeAppsStep()
	{
	}

CT_NonNativeAppsStep::CT_NonNativeAppsStep()
	{
	SetTestStepName(KT_NonNativeAppsStep);
	}

TVerdict CT_NonNativeAppsStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_NonNativeAppsStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CT_NonNativeAppsStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Test NonNativeApps starting....\n"));

	RTstFileArray fileArray(2);
	CleanupClosePushL(fileArray);
	fileArray.AppendL(_L("z:\\private\\101F289C\\GIF.NNA1"), KLit8_DataType_Gif);
	fileArray.AppendL(_L("z:\\private\\101F289C\\HTML.NNA2"), KLit8_DataType_Html);
	fileArray.AppendL(_L("z:\\private\\101F289C\\CONTACT.NNA3"), KLit8_DataType_Vcard);
	fileArray.AppendL(_L("z:\\private\\101F289C\\TXT.NNA4"), KLit8_DataType_plainText);

	RTestableApaLsSession apparcServer;
	CleanupClosePushL(apparcServer);
	User::LeaveIfError(apparcServer.Connect());

	RFs fileServer;
	CleanupClosePushL(fileServer);
	User::LeaveIfError(fileServer.Connect());
	User::LeaveIfError(fileServer.ShareProtected());

	apparcServer.FlushRecognitionCache();
	HEAP_TEST_LS_SESSION(apparcServer, 0, 0, TestAppForDocumentL(apparcServer, fileServer, fileArray), apparcServer.FlushRecognitionCache());
	HEAP_TEST_LS_SESSION(apparcServer, 0, 0, TestAppForDocumentAndServiceL(apparcServer, fileServer, fileArray), apparcServer.FlushRecognitionCache());
	HEAP_TEST_LS_SESSION(apparcServer, 0, 0, TestRecognizingL(apparcServer, fileServer, fileArray), apparcServer.FlushRecognitionCache());
	
	//avoid race conditions
	User::After(1500000);
	//DONT_CHECK Skips the heap check at server side. This heap imbalance happens intermittently,with no known reason (See PDEF111975).
	HEAP_TEST_LS_SESSION(apparcServer, 0, DONT_CHECK, TestStartDocumentL(apparcServer, fileServer, fileArray), apparcServer.FlushRecognitionCache());
	//DONT_CHECK due to file system changes
	HEAP_TEST_LS_SESSION(apparcServer, 0, DONT_CHECK, TestRegisterNonNativeApplicationL(apparcServer,fileServer), NO_CLEANUP);

	//INC127734: Test case is added to test ForceCommitNonNativeApplicationsUpdatesL API
	//DONT_CHECK due to file system changes
	HEAP_TEST_LS_SESSION(apparcServer, 0, DONT_CHECK, TestCommitNNAppUpdatesL(apparcServer), NO_CLEANUP);
	//DONT_CHECK due to file system changes
	HEAP_TEST_LS_SESSION(apparcServer, 0, DONT_CHECK, TestForceCommitNNAppUpdatesL(apparcServer), NO_CLEANUP);
	
	CleanupStack::PopAndDestroy(3, &fileArray);

	INFO_PRINTF1(_L("....Test NonNativeApps completed!"));
	return TestStepResult();
	}
