/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  The class handles IMPS Setting file reading.
*
*/


#include <f32file.h>
#include <s32file.h>

// FORWARD DECLARATION

//**********************************
// CImpsSettingFile
//**********************************
// Read key-value pairs from ASCII file
class CImpsSettingFile: public CBase
    {
    public:
        /**
        * Constructor.
        */
        static CImpsSettingFile* NewL(
            RFs& aFs );

        ~CImpsSettingFile();

        /**
        * Opens a file. It is closed in destructor!
        */
        void OpenL( TDesC& aResFile );

        /**
        * Get the key value. Leave with KErrNotFound if not found.
        */
        TPtrC8 KeyValueL( const TDesC8& aKey );

    private:
        CImpsSettingFile( RFs& aFs );

        void ReadRowL( TPtrC8& aKey, TPtrC8& aValue );
    private:
        // Data
        RFs& iFs;
        RFileReadStream iReader;
        TBuf8<128> iRowBuffer;
        TFileName iFileName;
        TBool     iOpen;

    };

// End of File
