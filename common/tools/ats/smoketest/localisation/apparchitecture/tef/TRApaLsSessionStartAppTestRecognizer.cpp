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
// This recognizer is part of the supporting test code
// for T_RApaLsSessionStep.CPP
// This recognizer only exists to associate the document
// name KLitTRApaLsSessionStartAppTest (defined in TRApaLsSessionStartAppTest.h) with the
// TRApaLsSessionStartAppTestApp.app application.
// This enables the start app test case T_RApaLsSessionStep.CPP to launch TRApaLsSessionStartAppTestApp.app
// by starting the document KLitTRApaLsSessionStartAppTest.
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
#include "TRApaLsSessionStartAppTest.h"

#include <ImplementationProxy.h>

//CRApaLsSessionStartAppTestRec

class CRApaLsSessionStartAppTestRec : public CApaDataRecognizerType
	{
public:
	CRApaLsSessionStartAppTestRec();
	static CApaDataRecognizerType* CreateRecognizerL();
private:
	// from CApaDataRecognizerType
	virtual TUint PreferredBufSize();
	virtual TDataType SupportedDataTypeL(TInt aIndex) const;
	virtual void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};

CRApaLsSessionStartAppTestRec::CRApaLsSessionStartAppTestRec()
	:CApaDataRecognizerType(KUidMimeTRApaLsSessionStartAppTestRecognizer, EHigh)
	{
	iCountDataTypes=KNumDataTypes;
	}

TUint CRApaLsSessionStartAppTestRec::PreferredBufSize()
	{
	return 0;
	}

TDataType CRApaLsSessionStartAppTestRec::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType(KLitMimeType_TRApaLsSessionStartAppTest);
	}

void CRApaLsSessionStartAppTestRec::DoRecognizeL(const TDesC& aName, const TDesC8&)
	{
	if (aName==KLitTRApaLsSessionStartAppTest)
		{
		iDataType=TDataType(KLitMimeType_TRApaLsSessionStartAppTest); // TRApaLsSessionStartAppTest.app supports mime type KLitMimeType_TRApaLsSessionStartAppTest
		iConfidence=ECertain;
		}
	}

// stand-alone functions


GLDEF_C TInt E32Dll(
					)
	{
	return KErrNone;
	}

CApaDataRecognizerType* CRApaLsSessionStartAppTestRec::CreateRecognizerL()
	{
	return new (ELeave) CRApaLsSessionStartAppTestRec();
	}

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(0x10004c50,CRApaLsSessionStartAppTestRec::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
