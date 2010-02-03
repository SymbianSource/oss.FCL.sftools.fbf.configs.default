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
// This recognizer is part of the supporting test code for T_NonNativeAppsStep.CPP
// This recognizer only exists to associate the document names defined in T_NonNativeAppsStep.h 
// with the TstNonNativeApp.exe application.
// This enables T_NonNativeAppsSt ep.CPP to launch TstNonNativeApp.app by starting the document names.
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
#include <F32FILE.H>

#include <apfrec.h>
#include <ImplementationProxy.h> 

_LIT8(KLit8_DataType_Gif, "x-epoc/nna-gif");
_LIT8(KLit8_DataType_Html, "x-epoc/nna-html");
_LIT8(KLit8_DataType_Vcard, "x-epoc/nna-vcf");
_LIT8(KLit8_DataType_plainText, "x-epoc/nna-txt");

const TUid KUidNonNativeRecognizer={0x10207f95};
const TUint KImplNonNativeRecognizer=0x10207f96;

enum TMimeTypes
    {
    EMimeTypeNnt1 = 0,
    EMimeTypeNnt2,
    EMimeTypeNnt3,
    EMimeTypeNnt4,
    EMimeLast
    };

_LIT(KLitMimeExtensionNnt1, ".NNA1");
_LIT(KLitMimeExtensionNnt2, ".NNA2");
_LIT(KLitMimeExtensionNnt3, ".NNA3");
_LIT(KLitMimeExtensionNnt4, ".NNA4");


// CTNonNativeRec definition

class CTNonNativeRec : public CApaDataRecognizerType
	{
public:
	CTNonNativeRec();
	static CApaDataRecognizerType* CreateRecognizerL();
private:
	// from CApaDataRecognizerType
	virtual TUint PreferredBufSize();
	virtual TDataType SupportedDataTypeL(TInt aIndex) const;
	virtual void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};


// CTNonNativeRec implementation

CTNonNativeRec::CTNonNativeRec() 
	:CApaDataRecognizerType(KUidNonNativeRecognizer, EHigh)
	{
	iCountDataTypes = EMimeLast;
	} 

TUint CTNonNativeRec::PreferredBufSize()
	{
	return 0;
	}

TDataType CTNonNativeRec::SupportedDataTypeL(TInt aIndex) const
	{
	if (aIndex == EMimeTypeNnt1)
		return TDataType(KLit8_DataType_Gif);
    
    else if (aIndex == EMimeTypeNnt2)
		return TDataType(KLit8_DataType_Html);
    
    else if (aIndex == EMimeTypeNnt3)
		return TDataType(KLit8_DataType_Vcard);
    
    else if (aIndex == EMimeTypeNnt4)
		return TDataType(KLit8_DataType_plainText);
    
    else
        return TDataType(KNullDesC8);
    }
 

void CTNonNativeRec::DoRecognizeL(const TDesC& aName, const TDesC8&)
	{

	// Compare if the file extension is known
	if (aName.Length() < 5)
		{
    	iDataType = TDataType(KNullDesC8); 	
    	iConfidence = ENotRecognized;
		return;
		}

	if (aName.Right(5).CompareF(KLitMimeExtensionNnt1) == 0)
		{
		iDataType = TDataType(KLit8_DataType_Gif);
		iConfidence = ECertain;
		}
	else if (aName.Right(5).CompareF(KLitMimeExtensionNnt2) == 0)
		{
		iDataType = TDataType(KLit8_DataType_Html);
		iConfidence = ECertain;
		}
	else if (aName.Right(5).CompareF(KLitMimeExtensionNnt3) == 0)
		{
		iDataType = TDataType(KLit8_DataType_Vcard);
		iConfidence = ECertain;
		}
	else if (aName.Right(5).CompareF(KLitMimeExtensionNnt4) == 0)
		{
		iDataType = TDataType(KLit8_DataType_plainText);
		iConfidence = ECertain;
		}
    else
    	{
    	iDataType = TDataType(KNullDesC8); 	
    	iConfidence = ENotRecognized;
    	}
	}

// stand-alone functions

CApaDataRecognizerType* CTNonNativeRec::CreateRecognizerL()
	{
	return new (ELeave) CTNonNativeRec();
	}

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(KImplNonNativeRecognizer, CTNonNativeRec::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
