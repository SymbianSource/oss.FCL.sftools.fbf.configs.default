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
// Defines a data recognizer
// Defines a test recognizer class which recognizes data type "test/plain".\n
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#include "testrec.h"

/**
  
   Recognizer used as part of CommandLine tests
   
   @SYMPREQ 280 File Handle Support
 
   FunctionDesc : Constructor for the recognizer of data type "test\plain" . 
   
 */

CAppTestRecognizer::CAppTestRecognizer()
	:CApaDataRecognizerType(KUidTestTxtRecognizer,CApaDataRecognizerType::ELow)
	// Text files are low recognition - they don't have a clear signature
	{
	iCountDataTypes=1;
	}

/**
  
   Recognizer used as part of CommandLine tests
  
   @SYMPREQ 280 File Handle Support
  
   FunctionDesc : Specifies the buffer size required by the data type
  
 */
TUint CAppTestRecognizer::PreferredBufSize()
	{
	return KMaxBufferLength;
	}

/**
  
   Recognizer used as part of CommandLine tests
  
   @SYMPREQ 280 File Handle Support
   
   FunctionDesc : Returns the data type supported : test\plain
  
 */
#if defined(_DEBUG)
TDataType CAppTestRecognizer::SupportedDataTypeL(TInt aIndex) const
#else
TDataType CAppTestRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
#endif
	{
	__ASSERT_DEBUG(aIndex==0,User::Invariant());
	return TDataType(KDataTypeTestPlain);
	}

 /**
  
   Recognizer used as part of CommandLine tests
  
   @SYMPREQ 280 File Handle Support
  
   FunctionDesc :  Should recognize the data type of the file. 
   Called by the RecognizeL function.
   Checks if the first three bytes are valid uids.If yes the file is not a plain/test
   Verifies if the file extension is .test
   Checks for characters that wont be in a text file.
  
 */
void CAppTestRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
	{
	TBool nameRecognized=EFalse;
    
	// check if the file has valid UIDs 
	if (aBuffer.Length() >= 16)
		{
		// if the first 3 bytes are valid UIDs,then this file is not a plain/test. 
		// Set iConfidence appropriately and exit.
		const TCheckedUid checkUid(aBuffer.Left(16));    
		if (checkUid.UidType().IsValid())
			{
			iConfidence=ENotRecognized;
			return;
			}
		}

	// Compare if the file extension is .TEST
	if (aName.Length()>4)
		{
		nameRecognized=(aName.Right(5).CompareF(KTestFileExt)==0);
		}
		
	const TInt length=Min(aBuffer.Length(), KMaxBufferLength);
	
	if (length<KMinBufferLength && !nameRecognized)
		{
		return;
		}
		
	
	// Check for characters that wont be in a text file
	TInt index;
	for (index=0; index<length; index++)
		{
		const TUint chr=aBuffer[index];
		// these are guesses of what WON'T be in a text file
		if (chr<9 || chr==11 || chr==12 || (chr>13 && chr<32))
			{
			break;
			}
		if (chr>148)
			{
			break;
			}
		}
	const TBool validChars=(index==length);
	
	if (nameRecognized)
		{
		iConfidence=validChars? EProbable : EUnlikely;
		}
	else
		{
		if (!validChars)
			{
			return;
			}
		iConfidence=EPossible;
		}
	iDataType=TDataType(KDataTypeTestPlain);
	}

/**
  
   Recognizer used as part of CommandLine tests
  
  
   @SYMPREQ 280 File Handle Support
  
   FunctionDesc : Creates an instance of the recognizer . 
   
 */

CApaDataRecognizerType* CAppTestRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CAppTestRecognizer();
	}
	
/**
  
   Recognizer used as part of CommandLine tests
  
   @SYMPREQ 280 File Handle Support
  
   Desc : Implementation table specifying the implementation uid . 
   
 */
const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(KTestTxtRecognizerValue,CAppTestRecognizer::CreateRecognizerL)
	};

/**
  
   Recognizer used as part of CommandLine tests
  
   @SYMPREQ 280 File Handle Support
  
   FunctionDesc : Returns the implementation table. 
   
  
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
