// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ImplementationProxy.h>

#include "appfwk_test_utils.h"
#include "recmime.h"
#include "constants.hrh"
#include "constants.h"

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
	:CApaDataRecognizerType(KUidMmrRecognizer, CApaDataRecognizerType::EHigh)
	{
	iCountDataTypes=KNumMimeTypes;
	}
	
/*
Ecom factory function

Due to platsec we can't build plugin.dll to an alternative location and we can't
copy files from epoc32\release\winscw\udeb. Both plugin.rsc and plugin.dll must 
be on the same drive to be installed. 

The workaround to implement this tef-test is works like this:
1. This test plugin are copied into z: by the build tools
2. This plugin will not be installed during startup because CreateRecognizerL will leave
3. During the test a dummy plugin.rsc will be copied into c:\resource\plugins to trig Ecom
4. Apparc will re-scan and make sure all recognizers listed by Ecom are properly installed
5. Ecom will not install the copied dummy file as it has no matching dll on the same drive, but
6. This recognizer will detect the dummy file and chose not to leave from CreateRecognizerL
*/
CApaDataRecognizerType* CTestMimeRecognizer::CreateRecognizerL()
	{
	RSmlTestUtils fileServ;
	CleanupClosePushL(fileServ);
	User::LeaveIfError(fileServ.Connect());
	TBool fileExists = EFalse;
	User::LeaveIfError(fileServ.IsFilePresent(KRscTargetPath, fileExists));
	CleanupStack::PopAndDestroy(&fileServ);	
	
	if(!fileExists)
		User::Leave(KErrPathNotFound);			
	
	return new (ELeave) CTestMimeRecognizer();
	}
	
TUint CTestMimeRecognizer::PreferredBufSize()
	{
	return KMaxBufferLength;
	}

TDataType CTestMimeRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType(KDataTypeNew);
	}

void CTestMimeRecognizer::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& /*aBuffer*/)
	{
    // If an error stops CTestMimeRecognizer being removed as part of test cleanup, 
    // returning ENotRecognized will avoid the possibility of impacting other test code.
	iDataType=TDataType();
	iConfidence=ENotRecognized;
	}

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(KUidMmrRecognizerValue, CTestMimeRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
