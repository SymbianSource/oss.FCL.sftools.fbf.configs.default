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

#ifndef TBUFFERONLYREC_H
#define TBUFFERONLYREC_H

#include <apmrec.h> 
#include <f32file.h>

class CBufferOnlyRec : public CApaDataRecognizerType
    {
public: // Constructors and destructor
    CBufferOnlyRec();
    static CApaDataRecognizerType* CreateRecognizerL();

public: // Functions from base classes
    TUint PreferredBufSize();
    TDataType SupportedDataTypeL(TInt aIndex) const;
    void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);

private:
    TInt iRecogs;
    };

#endif
