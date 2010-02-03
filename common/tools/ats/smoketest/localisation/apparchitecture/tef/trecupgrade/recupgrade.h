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

#if !defined(__RECUPGRADE_H__)
#define __RECUPGRADE_H__

#include <apmrec.h>

/**
@class
@internalComponent - Internal Symbian test code


This recognizer is part of the CT_ApsUpgradeStep tef test.
*/
class CTestUpgradeRecognizer : public CApaDataRecognizerType
	{
public: 
	// for ECOM
	static CApaDataRecognizerType* CreateRecognizerL();

	// from CApaDataRecognizerType
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL(TInt aIndex) const;
	
private: 
	CTestUpgradeRecognizer();
	
	// from CApaDataRecognizerType
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};

#endif
