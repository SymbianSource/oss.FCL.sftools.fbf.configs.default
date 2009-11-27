// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#if !defined(__RECMIME_1_H__)
#define __RECMIME_1_H__

#include <apmrec.h>

// Literals & Constants
_LIT(KLitDocPath, "Z:\\system\\data\\TApsRecogAppTest.mmr");

_LIT8(KMimeMmrType,"mime/mmr1");

#define KUidMmrRecognizerValue_1 0xA00025DE
const TUid KUidMmrRecognizer_1={KUidMmrRecognizerValue_1};
const TInt KNumMimeTypes_1 = 1;	

/**
@class
@internalComponent - Internal Symbian test code
*/
class CTestMimeRecognizer : public CApaDataRecognizerType
	{
public: 
	// for ECOM
	static CApaDataRecognizerType* CreateRecognizerL();

	// from CApaDataRecognizerType
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL(TInt aIndex) const;
	
private: 
	CTestMimeRecognizer();
	
	// from CApaDataRecognizerType
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};

#endif
