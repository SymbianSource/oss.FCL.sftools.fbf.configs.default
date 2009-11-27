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

/**   Recognizer doing nothing for checking the defect DEF039651.
      Change Apparc's TRecognizerName in a similar way to EMime's was in DEF037809" */

class CDoNothingFileRec : public CApaFileRecognizerType
	{
public: // from CApaFileRecognizerType
	static CApaFileRecognizerType* CreateRecognizerL();
	TThreadId RunL(TApaCommand aCommand,const TDesC* aDocFileName=NULL,const TDesC8* aTailEnd=NULL) const;
private: // from CApaFileRecognizerType
	TRecognizedType DoRecognizeFileL(RFs& aFs,TUidType aType);
	};

TThreadId CDoNothingFileRec::RunL(TApaCommand /*aCommand*/,const TDesC* /*aDocFileName*/,const TDesC8* /*aTailEnd*/) const
	{
	TThreadId id(NULL);
	return id;
	}

CApaFileRecognizerType::TRecognizedType CDoNothingFileRec::DoRecognizeFileL(RFs& /*aFs*/,TUidType /*aType*/)
	{
	return iRecognizedType;
	}



CApaFileRecognizerType* CDoNothingFileRec::CreateRecognizerL()
	{
	CDoNothingFileRec* doNothingFileRec=new (ELeave) CDoNothingFileRec();
	return doNothingFileRec;
	//return new (ELeave) CExampleFileRecognizer ();
	}

const TImplementationProxy ImplementationTable[] =
{
IMPLEMENTATION_PROXY_ENTRY(0x101DC568, CDoNothingFileRec::CreateRecognizerL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
	
GLDEF_C TInt E32Dll(
					)

// DLL entry point

	{
	return KErrNone;
	}
