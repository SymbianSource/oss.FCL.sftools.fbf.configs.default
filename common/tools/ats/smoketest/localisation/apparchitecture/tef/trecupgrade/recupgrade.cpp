// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <implementationproxy.h>

#include "recupgrade.h"
#include "upgconstants.h"

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
	iCountDataTypes=0;
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
	return TDataType();
	}

void CTestUpgradeRecognizer::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& /*aBuffer*/)
	{
    	// returning ENotRecognized will avoid the possibility of impacting other test code.
	iDataType=TDataType();
	iConfidence=ENotRecognized;
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
