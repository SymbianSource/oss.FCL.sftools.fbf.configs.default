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
// Header File for the data recognizer
// Declares a test recognizer class which recognizes data type "test/plain".\n
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#if !defined(__TESTREC_H__)
#define __TESTREC_H__

#if !defined(__APMREC_H__)
#include <apmrec.h>
#endif

#include <apmstd.h>
#include <ImplementationProxy.h>

class CAppTestRecognizer : public CApaDataRecognizerType
	{
public: // from CApaDataRecognizerType
	CAppTestRecognizer();
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL(TInt aIndex) const;
	//for ECOM
	static CApaDataRecognizerType* CreateRecognizerL();
private: // from CApaFileRecognizerType
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};

const TInt KTestTxtRecognizerValue=0x102032A6;
const TUid KUidTestTxtRecognizer={0x102032A6};
const TInt KMinBufferLength=42;  // minimum amount of file needed to determine a text file IF it's not called .TEST
const TInt KMaxBufferLength=1024; // maximum amount of buffer space we will ever use
_LIT8(KDataTypeTestPlain,"test/plain");
_LIT(KTestFileExt,".TEST");


#endif
