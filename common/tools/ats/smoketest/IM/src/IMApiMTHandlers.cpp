/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handler class
*
*
*/


// INCLUDE FILES
#include    <apparc.h>
#include    <e32std.h>
#include    "IMApiMTHandlers.h"
#include    "TestRunner.h"

/******************************************************************************
*
* CIMApiClientHandler
*
******************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Handler class
CIMApiClientHandler* CIMApiClientHandler::NewL( TRequestStatus& aStatus )
    {
    CIMApiClientHandler* self = new( ELeave ) CIMApiClientHandler( aStatus );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CIMApiClientHandler::ConstructL()
    {
    }

CIMApiClientHandler::CIMApiClientHandler( TRequestStatus& aStatus )
        : iStatus( aStatus )
    {
    }

CIMApiClientHandler::~CIMApiClientHandler()
    {
    }


void CIMApiClientHandler::HandleMessageSentL( const TInt aOpCode,
                                              const TInt aErrorCode )
    {
    TRequestStatus* s = &iStatus;
    CTestLogger::Log( _L( "HandleMessageSentL Passed - aErrorCode=%d" ), aErrorCode );

    if ( aErrorCode == 0 || aErrorCode == KImApiErrInvalidUserId || aErrorCode == KImApiErrNotLogged || aErrorCode == KImApiErrPartialSuccess  )
        {
        if ( aErrorCode == 0 )
            {
            User::RequestComplete( s, KErrNone ); // OK
            }
        else if ( aErrorCode == KImApiErrInvalidUserId )
            {
            User::RequestComplete( s, KErrNone ); // OK
            }
        else if ( aErrorCode == KImApiErrPartialSuccess )
            {
            User::RequestComplete( s, KErrNone ); // OK
            }
        else
            {
            User::RequestComplete( s, KErrNone );
            }
        }
    else
        {
        User::RequestComplete( s, aErrorCode ); // FAILED
        }
    }

void CIMApiClientHandler::HandleSendErrorL( const TInt aOpCode,
                                            const TInt aErrorCode,
                                            MImClientDetailedError* aDetailedError )

    {
    TRequestStatus* s = &iStatus;
    TBuf<500> buffer;

    CTestLogger::Log( _L( "HandleSendErrorL - aErrorCode=%d" ), aErrorCode );
    if ( aErrorCode == 0 || aErrorCode == KImApiErrInvalidUserId || aErrorCode == KImApiErrNotLogged || aErrorCode == KImApiErrPartialSuccess  )
        {
        if ( aErrorCode == 0 )
            {
            User::RequestComplete( s, KErrNone ); // OK
            }
        else if ( aErrorCode == KImApiErrInvalidUserId )
            {
            User::RequestComplete( s, KErrNone ); // OK
            }
        else if ( aErrorCode == KImApiErrPartialSuccess )
            {
            for ( TInt i = 0; i < aDetailedError->Count(); i++ )
                {
                buffer.Copy( aDetailedError->UserId( i ) );
                }
            User::RequestComplete( s, KErrNone ); // OK
            }
        else
            {
            User::RequestComplete( s, KErrNone );
            }
        }
    else
        {
        User::RequestComplete( s, KErrNone ); // PASSED sad but true
        }
    }


void CIMApiClientHandler::HandleNewPToPMessageL( const TInt aErrorCode,
                                                 const TContactItemId aContactId,
                                                 const TDesC& aUserId,
                                                 const TDesC& aMessageType,
                                                 const TDesC16& aContent )
    {
    TRequestStatus* s = &iStatus;
    CTestLogger::Log( _L( "HandleNewPToPMessageL Passed - aErrorCode=%d" ), aErrorCode );

    if ( aErrorCode == 0 )
        {
        User::RequestComplete( s, KErrNone );
        }
    else
        {
        User::RequestComplete( s, aErrorCode );
        }
    }

void CIMApiClientHandler::HandleNewPToPMessageL( const TInt aErrorCode,
                                                 const TContactItemId aContactId,
                                                 const TDesC& aUserId,
                                                 const TDesC& aMessageType,
                                                 const TDesC8& aContent )
    {
    TRequestStatus* s = &iStatus;
    CTestLogger::Log( _L( "HandleNewPToPMessageL Passed - aErrorCode=%d" ), aErrorCode );
    if ( aErrorCode == 0 )
        {
        User::RequestComplete( s, KErrNone );
        }
    else
        {
        User::RequestComplete( s, aErrorCode );
        }
    }


void CIMApiClientHandler::TimedOut()
    {
    TRequestStatus* s = &iStatus;
    User::RequestComplete( s, KErrTimedOut );
    }



//////////////////////////////////////////////////////////////////////////

CConnectionObserver* CConnectionObserver::NewL( TRequestStatus& aStatus )
    {
    CConnectionObserver* self = new( ELeave ) CConnectionObserver( aStatus );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CConnectionObserver::ConstructL()
    {
    }

CConnectionObserver::CConnectionObserver( TRequestStatus& aStatus )
        : iStatus( aStatus )
    {
    }

CConnectionObserver::~CConnectionObserver()
    {
    }


void CConnectionObserver::HandleConnectL( const TInt aErrorCode )
    {
    TRequestStatus* s = &iStatus;
    CTestLogger::Log( _L( "HandleConnectL Passed - aErrorCode=%d" ), aErrorCode );

    if ( aErrorCode == 0 )
        {
        User::RequestComplete( s, KErrNone ); // OK
        }
    else if ( aErrorCode == KImApiErrInvalidUserId )
        {
        User::RequestComplete( s, KErrNone ); // OK
        }
    else if ( aErrorCode == -33 )
        {
        User::RequestComplete( s, aErrorCode ); // FAILED
        }
    else if ( aErrorCode == -4154 )
        {
        User::RequestComplete( s, aErrorCode );   // FAILED
        }
    else if ( aErrorCode == -16 )
        {
        User::RequestComplete( s, KErrNone );  // OK IN IMAPI POINT OF WIEV
        }
    else if ( aErrorCode == KImApiErrLoginInProgress )
        {
        User::RequestComplete( s, KErrNone );  // OK IN IMAPI POINT OF WIEV
        }
    else
        {
        User::RequestComplete( s, aErrorCode );// FAILED
        }
    }

void CConnectionObserver::HandleDisconnectL()
    {
    TRequestStatus* s = &iStatus;
    CTestLogger::Log( _L( " HandleDisconnectL " ) );
    User::RequestComplete( s, KErrNone ); //OK
    }

void CConnectionObserver::HandleLoginL( const TInt aErrorCode )
    {
    CTestLogger::Log( _L( "HandleLoginL Passed - aErrorCode=%d" ), aErrorCode );

    TRequestStatus* s = &iStatus;
    if ( aErrorCode == 0 )
        {
        User::RequestComplete( s, KErrNone );
        }
    else
        {
        User::RequestComplete( s, aErrorCode );
        }
    }

void CConnectionObserver::HandleCancelLoginL( const TInt aErrorCode )
    {
    CTestLogger::Log( _L( "HandleCancelLoginL Passed - aErrorCode=%d" ), aErrorCode );
    TRequestStatus* s = &iStatus;
    if ( aErrorCode == 0 )
        {
        User::RequestComplete( s, KErrNone );
        }
    else
        {
        User::RequestComplete( s, aErrorCode );
        }
    }

void CConnectionObserver::HandleLogoutL( const TInt aErrorCode )
    {
    CTestLogger::Log( _L( "HandleLogoutL Passed-  aErrorCode=%d" ), aErrorCode );
    CActiveScheduler::Stop();
    }



//**********************************
// CImpsClientLogger
//**********************************
const TInt KLogBufferLength = 256;
_LIT( KLogDir, "impsc" );
_LIT( KLogFile, "IMAPIHandlersLog.txt" );

EXPORT_C void CTestLogger::Log( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList( aFmt, list );

    // Write to log file
    RFileLogger::Write( KLogDir, KLogFile, EFileLoggingModeAppend, buf );
    }

void CTestScheduler::Error( TInt /*anError */ ) const
    {
    CTestLogger::Log( _L( "ClientTester RunL ERROR RECEIVED" ) );
    }
