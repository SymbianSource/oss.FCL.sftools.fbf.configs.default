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
 @internalComponent - Internal Symbian test code 
*/

#include <apfrec.h>
#include <ImplementationProxy.h> 

class CTestEcomRecognizer : public CApaFileRecognizerType
	{
public: // from CApaFileRecognizerType
	TThreadId RunL(TApaCommand aCommand,const TDesC* aDocFileName=NULL,const TDesC8* aTailEnd=NULL) const;
	static CApaFileRecognizerType* CreateRecognizerL();
private: // from CApaFileRecognizerType
	TRecognizedType DoRecognizeFileL(RFs& aFs,TUidType aType);
	};

TThreadId CTestEcomRecognizer::RunL(TApaCommand /*aCommand*/,const TDesC* /*aDocFileName*/,const TDesC8* /*aTailEnd*/) const
	{
	TThreadId id(NULL);
	return id;
	}

CApaFileRecognizerType::TRecognizedType CTestEcomRecognizer::DoRecognizeFileL(RFs& /*aFs*/,TUidType /*aType*/)
	{
	return iRecognizedType;
	}


GLDEF_C TInt E32Dll(
					)

// DLL entry point

	{
	return KErrNone;
	}

CApaFileRecognizerType* CTestEcomRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CTestEcomRecognizer();
	}

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY(0x101F7D8C,CTestEcomRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
