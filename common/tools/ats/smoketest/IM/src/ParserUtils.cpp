/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: The class handles IMPS Setting file reading.
*
*
*/


// INCLUDE FILES
#include    <e32std.h>
#ifdef _DEBUG
#include    <flogger.h>
#endif
#include "ParserUtils.h"

// ================= MEMBER FUNCTIONS =======================



//**********************************
// CImpsSettingFile
//**********************************

CImpsSettingFile* CImpsSettingFile::NewL(
    RFs& aFs )
    {
    CImpsSettingFile* self = new ( ELeave ) CImpsSettingFile( aFs );
    return self;
    }

CImpsSettingFile::~CImpsSettingFile()
    {
    if ( iOpen )
        {
        iReader.Close();
        }
    iOpen = EFalse;
    }

CImpsSettingFile::CImpsSettingFile( RFs& aFs )
        : iFs( aFs ),
        iOpen( EFalse )
    {

    }

void CImpsSettingFile::OpenL( TDesC& aResFile )
    {
    // open a file
    iFileName = aResFile;

    TInt myError = iReader.Open( iFs,
                                 iFileName,
                                 EFileShareReadersOnly );

    User::LeaveIfError( myError );
    iOpen = ETrue;

    }

TPtrC8 CImpsSettingFile::KeyValueL( const TDesC8& aKey )
    {

    TPtrC8 myKey;
    TPtrC8 myValue;
    TBool getIt( EFalse );
    TInt err = 0;

    // Reset the reader
    OpenL( iFileName );

    // Start to search
    while ( !getIt )
        {
        TRAP ( err, ReadRowL( myKey, myValue ) );
        if ( err != KErrNone )
            {
            User::Leave( KErrNotFound );
            }
        if ( !myKey.CompareF( aKey ) )
            {
            return myValue;
            }
        }
    return TPtrC8();
    }

void CImpsSettingFile::ReadRowL( TPtrC8& aKey, TPtrC8& aValue )
    {
    // READ ONE ROW
    TChar delim( 10 );
    iReader.ReadL( iRowBuffer, delim );
    TInt length = iRowBuffer.Length();
    if ( length > 2 )
        {
        // DROP CR+LF FROM THE END OF LINE
        iRowBuffer.Delete( length - 2, 2 );

        TInt pos = 0;
        pos = iRowBuffer.Find( _L8( "=" ) );
        if ( pos > 0 )
            {
            aKey.Set( iRowBuffer.Left( pos ) );
            // Cut off separator
            aValue.Set( iRowBuffer.Mid( pos + 1 ) );
            }
        }
    }

//  End of File

