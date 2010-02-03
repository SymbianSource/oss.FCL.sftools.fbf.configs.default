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
#include <ImplementationProxy.h>

#include "appfwk_test_utils.h"
#include "recmime_1.h"

/////////////////////////////////////////////////

const TInt KMaxBufferLength=1024; // maximum amount of buffer space we will ever use

/////////////////////////////////////////////////

/*
DLL entry point
*/
GLDEF_C TInt E32Dll()
	{
	return KErrNone;
	}

/*
Private constructor
*/
CTestMimeRecognizer::CTestMimeRecognizer()
	:CApaDataRecognizerType(KUidMmrRecognizer_1, CApaDataRecognizerType::EHigh)
	{
	iCountDataTypes = KNumMimeTypes_1;
	}
	
/*
Ecom factory function
*/
CApaDataRecognizerType* CTestMimeRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CTestMimeRecognizer();
	}
	
TUint CTestMimeRecognizer::PreferredBufSize()
	{
	return KMaxBufferLength;
	}

TDataType CTestMimeRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType(KMimeMmrType);
	}

void CTestMimeRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& /*aBuffer*/)
	{
    // If an error stops CTestMimeRecognizer being removed as part of test cleanup, 
    // returning ENotRecognized will avoid the possibility of impacting other test code.
	if (aName == KLitDocPath)
		{
		iDataType = TDataType(KMimeMmrType);
		iConfidence = EPossible;
		}
	else
		{
		iDataType=TDataType();
		iConfidence=ENotRecognized;
		}	
	}

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(KUidMmrRecognizerValue_1, CTestMimeRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
