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
// This recognizer is part of the supporting test code
// for T_StartDocStep.CPP
// This recognizer only exists to associate the document
// name KLitLongURL (defined in TStartDoc.h) with the
// TStartDocApp.app application.
// This enables T_StartDocStep.CPP to launch TStartDocApp.app
// by starting the document KLitLongURL.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include <E32STD.H>
#include <E32BASE.H>
#include <APMSTD.H>
#include <APMREC.H>
#include "TStartDoc.h"

#include <apfrec.h>
#include <ImplementationProxy.h> 


const TUid KUidMimeLongURLRecognizer={0x10004c4e};
const TInt KNumDataTypes = 1;
_LIT8(KLitMimeType_LongURL, "x-epoc/long-url");

// CLongUrlRec

class CLongUrlRec : public CApaDataRecognizerType
	{
public:
	CLongUrlRec();
	static CApaDataRecognizerType* CreateRecognizerL();
private:
	// from CApaDataRecognizerType
	virtual TUint PreferredBufSize();
	virtual TDataType SupportedDataTypeL(TInt aIndex) const;
	virtual void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};

CLongUrlRec::CLongUrlRec()
	:CApaDataRecognizerType(KUidMimeLongURLRecognizer, EHigh)
	{
	iCountDataTypes=KNumDataTypes;
	}

TUint CLongUrlRec::PreferredBufSize()
	{
	return 0;
	}

TDataType CLongUrlRec::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType(KLitMimeType_LongURL);
	}

void CLongUrlRec::DoRecognizeL(const TDesC& aName, const TDesC8&)
	{
	if (aName==KLitLongURL)
		{
		iDataType=TDataType(KLitMimeType_LongURL); // TStartDocApp.app supports mime type KLitMimeType_LongURL
		iConfidence=ECertain;
		}
	}

// stand-alone functions


GLDEF_C TInt E32Dll(
					)
	{
	return KErrNone;
	}
	
CApaDataRecognizerType* CLongUrlRec::CreateRecognizerL()
	{
	return new (ELeave) CLongUrlRec();
	}

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(0x101F7D8D,CLongUrlRec::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
