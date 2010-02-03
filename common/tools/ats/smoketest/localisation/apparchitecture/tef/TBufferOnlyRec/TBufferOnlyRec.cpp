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
//

#include "TBufferOnlyRec.h"
#include "TBufferOnlyRecUid.hrh"
#include <ImplementationProxy.h>

enum TMimeTypes
    {
    EMimeType = 0,
    ELastType
    };


_LIT8(KMimeType,"text/plain");

CBufferOnlyRec::CBufferOnlyRec() 
: CApaDataRecognizerType(TUid::Uid(KBufferOnlyRecDllUid), CApaDataRecognizerType::ENormal)
    {
    iCountDataTypes = ELastType;
    iRecogs = 0;
    }

CApaDataRecognizerType* CBufferOnlyRec::CreateRecognizerL()
    {
    return new(ELeave) CBufferOnlyRec;
    }

TUint CBufferOnlyRec::PreferredBufSize()
    {
    return 250;
    }

TDataType CBufferOnlyRec::SupportedDataTypeL(TInt aIndex) const
    {
    if (aIndex != EMimeType)
        {
        User::Leave(KErrNotSupported);
        }
    return TDataType(KMimeType);
    }

void CBufferOnlyRec::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& aBuffer)
    {
    if (aBuffer.CompareC(_L8("Text_TestRecognizer")) == 0)
        {
        iDataType = TDataType(KMimeType);
        iConfidence = ECertain;
        }
    else if (aBuffer.CompareC(_L8("AnotherText")) == 0)
        {
        iDataType = TDataType(KNullDesC8);
        iConfidence = EProbable;
        }
    else
    	{
    	iDataType = TDataType(KNullDesC8); 	
    	iConfidence = ENotRecognized;
    	}
    iRecogs++;  
    }

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KBufferOnlyRecImplUid,
        CBufferOnlyRec::CreateRecognizerL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
