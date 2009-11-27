// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <ImplementationProxy.h>
#include "..\TRecUpgrade\upgconstants.h"
#include "..\TRecUpgrade\recupgrade.h"
#include <f32file.h>  // TParse

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
CTestUpgradeRecognizer::CTestUpgradeRecognizer()
	:CApaDataRecognizerType(KUidUpgradeRecognizer, CApaDataRecognizerType::ENormal)
	{
	iCountDataTypes = KNumUpgradeTypes;
	}
	
/*
Ecom factory function
*/
CApaDataRecognizerType* CTestUpgradeRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CTestUpgradeRecognizer();
	}
	
TUint CTestUpgradeRecognizer::PreferredBufSize()
	{
	return KMaxBufferLength;
	}

TDataType CTestUpgradeRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType(KMimeUpgType);
	}

void CTestUpgradeRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& /*aBuffer*/)
	{
    // If an error stops CTestUpgradeRecognizer being removed as part of test cleanup, 
	// returning ENotRecognized will avoid the possibility of impacting other test code.

	_LIT(KExtension , ".upg");
	TParse parse;
    parse.Set(aName,NULL,NULL);
	TPtrC ext=parse.Ext(); // extract the extension from the filename

	if (ext.CompareF(KExtension)==0)
		{
		iDataType = TDataType(KMimeUpgType);
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
	IMPLEMENTATION_PROXY_ENTRY(KUidUpgradeRecognizerValue, CTestUpgradeRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
