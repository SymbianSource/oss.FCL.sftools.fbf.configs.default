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
* Description: The class handles the test case running.
*
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    <flogger.h>
#include 	"imerrors.h"
#include    <CNTDEF.H>
#include    "IMApiMTHandlers.h"
#include    "TestRunner.h"
#include 	"IMApiTest.h"
#include 	"ParserUtils.h"

_LIT( KImpsIMApiMTIniFile, "c:\\smoketest\\imapitest.ini" );
_LIT8( KImpsSAP, "SAP" );
_LIT8( KImpsUserID, "Userid" );
_LIT8( KImpsPassword, "password" );
_LIT8( KImpsClientid, "clientid" );
_LIT8( KImpsIAP, "IAP" );
_LIT8( KImpsUserid1, "userid1" );
_LIT8( KImpsUserid2, "userid2" );

void SetupSchedulerL()
    {
    // Create a scheduler
    CTestScheduler* scheduler = new( ELeave )CTestScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    CleanupStack::Pop(); // scheduler
    }

void CTestRunner::ReadIniFileL()
    {
    RFs myFs;
    TInt err = 0;
    TUint32 myTime = 0;
    TLex lex;
    err = myFs.Connect();
    //CleanupClosePushL( myFs );
    CImpsSettingFile* configFile = CImpsSettingFile::NewL( myFs );
    CleanupStack::PushL( configFile );
    TFileName fileN( KImpsIMApiMTIniFile );
    // Open config file
    TRAP( err, configFile->OpenL( fileN ) );
    User::LeaveIfError( err );

    TBuf<KImpsMaxSapLen> myPtr;
    TRAP( err, myPtr.Copy( configFile->KeyValueL( KImpsSAP ) ) );
    if ( !err )
        {
        iSapValue.Copy( myPtr );
        }
    TRAP( err, myPtr.Copy( configFile->KeyValueL( KImpsUserID ) ) );
    if ( !err )
        {
        iUserIdValue.Copy( myPtr );
        }
    TRAP( err, myPtr.Copy( configFile->KeyValueL( KImpsPassword ) ) );
    if ( !err )
        {
        iPassword.Copy( myPtr );
        }
    TRAP( err, myPtr.Copy( configFile->KeyValueL( KImpsClientid ) ) );
    if ( !err )
        {
        iClientId.Copy( myPtr );
        }
    TRAP( err, myPtr.Copy( configFile->KeyValueL( KImpsIAP ) ) );
    if ( !err )
        {
        lex.Assign( myPtr );
        lex.Val( myTime, EDecimal );
        iIAP = ( TInt ) myTime;
        }
    TRAP( err, myPtr.Copy( configFile->KeyValueL( KImpsUserid1 ) ) );
    if ( !err )
        {
        iUserId1.Copy( myPtr );
        }
    TRAP( err, myPtr.Copy( configFile->KeyValueL( KImpsUserid2 ) ) );
    if ( !err )
        {
        iUserId2.Copy( myPtr );
        }
    CleanupStack::PopAndDestroy( 1 );  // configFile
    }



CTestRunner* CTestRunner::NewL()
    {
    CTestRunner* self = new ( ELeave ) CTestRunner();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

void CTestRunner::ConstructL()
    {
    SetupSchedulerL();
    iCase = Login;
    iDatabase = CContactDatabase::OpenL();
    iImObserver = CIMApiClientHandler::NewL( iStatus );
    iConnObserver = CConnectionObserver::NewL( iStatus );
    ReadIniFileL();

    CActiveScheduler::Add( this );
    }

CTestRunner::~CTestRunner()
    {

    if ( iOpenApi != NULL )
        {
        iOpenApi->UnregisterObserver();
        }
    CTestLogger1::Log( _L( "Test Case Number #16" ) );
    CTestLogger1::Log( _L( "CImConnection::UnregisterObserver" ) );
    if ( iOpenApi != NULL )
        {
        delete iOpenApi;
        iOpenApi = NULL;
        }
    if ( iImClient != NULL )
        {
        delete iImClient;
        iImClient = NULL;
        }
    if ( iConnObserver != NULL )
        {
        delete iConnObserver;
        iConnObserver = NULL;
        }
    if ( iImObserver != NULL )
        {
        delete iImObserver;
        iImObserver = NULL;
        }
    if ( iDatabase != NULL )
        {
        delete iDatabase;
        iDatabase = NULL;
        }

    }



void CTestRunner::DoCancel()
    {
    // Complete the request with error
    TRequestStatus* s = &iStatus;
    User::RequestComplete( s, KErrCancel );
    }


CTestRunner::CTestRunner()
        : CActive( 0 )
    {
    }


void CTestRunner::StartRunL( TWhichCase aCase )
    {
    iCase = aCase;
    SetActive();
    TRequestStatus *s = &iStatus;
    User::RequestComplete( s, KErrNone );
    CActiveScheduler::Start();
    }

void CTestRunner::RunL()
    {
    TInt errxx = 0;
    TRAP( errxx, DoRunL() );
    if ( errxx < 0 )
        {
        CActiveScheduler::Stop();
        }
    }


void CTestRunner::DoRunL()
    {
    if ( iCase >= Finish )
        {
        Cancel();
        CActiveScheduler::Stop();
        return;
        }

    if ( iStatus != KErrNone )
        {
        Cancel();
        CActiveScheduler::Stop();
        return;
        }
    switch ( iCase )
        {
        case CreateImClient:
            {
            CreateInstanceL();			//NewL - instantiate the object
            RegisterConnObserverL();	//RegisterObserverL - register the observer methods
            LoadIMInterfaceL();			//CreateImClientL - get the IM interface
            }
        break;
        case InvalidLogin:
            {
            CreateInstanceL();
            RegisterConnObserverL();
            LoadIMInterfaceL();
            IMRegisterL();
            LoginL( _L( "InvalidUser" ) );
            }
        break;
        case Login:
            {
            CreateInstanceL();
            RegisterConnObserverL();
            LoadIMInterfaceL();
            IMRegisterL();
            LoginL( iUserIdValue );
            }
        break;
        case SendMessageValidUserId:
            {
            SendPToPValidUserIDL();
            }
        break;
        case SendMessageValidTwoUserId:
            {
            SendPToPValidTwoUserIDL();
            }
        break;
        case SendMessageValidContact:
            {
            SendPToPValidContactL();
            }
        break;
        case SendMessageValidTwoContact:
            {
            SendPToPValidTwoContactL();
            }
        break;
        case SendPToPContactContentType:
            {
            SendPToPContactContentTypeL();
            }
        break;
        case SendPToPUserIDContentType:
            {
            SendPToPUserIDContentTypeL();
            }
        break;

        case Logout:
            {
            QueryIMConnectionStatusL();
            }
        break;
        case CancelLogin:
            {
            CancelLoginL();
            }
        break;
        }

    iCase = Finish;

    return;
    }


CContactIdArray* CTestRunner::GetWVContactIdsLC( )
    {
    _LIT( KWVStart, "" );
    // Specify in which fields search should be performed
    CContactItemFieldDef* fieldToSearchIn = new ( ELeave ) CContactItemFieldDef();
    CleanupStack::PushL( fieldToSearchIn );
    fieldToSearchIn->AppendL( KUidContactFieldVCardMapWV );

    // search in contact database
    CContactIdArray *array = iDatabase->FindLC( KWVStart, fieldToSearchIn );
    CleanupStack::Pop(); //>> array
    CleanupStack::PopAndDestroy(); //>> fieldToSearchIn
    CleanupStack::PushL( array );

    return array;
    }



TInt CTestRunner::RegisterConnObserverL()
    {
    CTestLogger1::Log( _L( "Test Case Number #2" ) );
    TRAPD( err, iOpenApi->RegisterObserverL( iConnObserver ) );
    // Catch errors
    if ( err == KImApiGeneralError ) // -30133
        {
        iStatus = err; // Failed
        CTestLogger1::Log( _L( "CImConnection::RegisterObserverL - err=%d" ), err );
        }
    else
        {
        iStatus = KErrNone; //OK
        CTestLogger1::Log( _L( "CImConnection::RegisterObserverL - err=%d" ), err );
        }
    }


TInt CTestRunner::CreateInstanceL()
    {
    CTestLogger1::Log( _L( "Test Case Number #1" ) );
    iOpenApi = CImConnection::NewL( iClientId );
    CTestLogger1::Log( _L( "CImConnection::NewL" ) );
    return KErrNone; //OK
    }


TInt CTestRunner::LoadIMInterfaceL()
    {
    CTestLogger1::Log( _L( "Test Case Number #3" ) );
    TRAPD( err, iImClient = iOpenApi->CreateImClientL() );
    // Catch errrors
    if ( err == 0 )
        {
        iStatus = KErrNone; // OK
        CTestLogger1::Log( _L( "CreateImClientL - err=%d" ), err );
        }
    else
        {
        iStatus = err; // Failed
        CTestLogger1::Log( _L( "CreateImClientL - err=%d" ), err );
        }
    }


TInt CTestRunner::IMRegisterL()
    {
    CTestLogger1::Log( _L( "Test Case Number #4" ) );
    TRAPD( err, iImClient -> RegisterObserverL( iImObserver ) );
    // Catch errors
    if ( err == KImApiGeneralError ) //- 30133
        {
        iStatus = err; // Failes
        CTestLogger1::Log( _L( "MImClient::RegisterObserverL - err=%d" ), err );
        }
    else
        {
        iStatus = KErrNone; //OK
        CTestLogger1::Log( _L( "MImClient::RegisterObserverL - err=%d" ), err );
        }
    }

TInt CTestRunner::LoginL( const TDesC& aUserID )
    {
    if ( aUserID.Compare( _L( "InvalidUser" ) ) )
        CTestLogger1::Log( _L( "Test Case Number #6" ) );
    else
        {
        CTestLogger1::Log( _L( "Invalid Login" ) );
        CTestLogger1::Log( _L( "Test Case Number #5" ) );
        }

    if ( iSapValue.Size() == 0 || iPassword.Size() == 0 || aUserID.Size() == 0 )
        {
        iStatus = KErrNone;
        CTestLogger1::Log( _L( "Invalid login parametres" ) );
        Cancel();
        CActiveScheduler::Stop();
        return KErrNone;
        }

    TRAPD( err, iOpenApi->LoginL( iSapValue, aUserID, iPassword, iIAP ) );
    // Catch errors
    if ( err )
        {
        iStatus = err; // Failes
        CTestLogger1::Log( _L( "LoginL - err=%d" ), err );
        return err;
        }
    else
        {
        iStatus = KRequestPending;
        CTestLogger1::Log( _L( "LoginL - err=%d" ), err );
        SetActive();
        return KErrNone;
        }
    }

TInt CTestRunner::SendPToPValidUserIDL()
    {
    CTestLogger1::Log( _L( "Test Case Number #7" ) );
    if ( iUserId1.Length() == 0 )
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidUserId) UserID1 is NULL " ) );
        iStatus = KErrNone;
        Cancel();
        CActiveScheduler::Stop();
        return KErrNone;
        }
    CDesCArray* userIds = new( ELeave ) CDesCArrayFlat( 1 ); // Create user list
    CleanupStack::PushL( userIds );
    userIds->AppendL( iUserId1 ); // Add one user

    _LIT( KContentType, "text/plain" ); // Message type
    _LIT( KMessage, "5. Hello In One Time Valid UserId" ); // Message
    TInt OpId = 0;
    // Send message
    TRAPD ( err, OpId = iImClient->SendPToPMessageL( iUserId1, KMessage ) );
    // Catch errors
    if ( err == KImApiErrInvalidUserId && OpId == 0 ) // KImApiErrInvalidUserId
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidUserId) - err=%d" ), err );
        CleanupStack::PopAndDestroy(); // userIds
        iStatus = err;
        return err;
        }
    else if ( err == KImApiGeneralError && OpId == 0 ) // General ImApi errror
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidUserId) - err=%d" ), err );
        CleanupStack::PopAndDestroy(); // userIds
        iStatus = err;
        return err;
        }
    else
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidUserId) - err=%d" ), err );
        CleanupStack::PopAndDestroy(); // userIds
        iStatus = KRequestPending;
        SetActive();
        return KErrNone;
        }
    }


TInt CTestRunner::SendPToPValidTwoUserIDL()
    {
    CTestLogger1::Log( _L( "Test Case Number #8" ) );
    CDesCArray* userIds = new( ELeave ) CDesCArrayFlat( 2 ); // Create new user list
    CleanupStack::PushL( userIds );
    userIds->AppendL( iUserId1 ); // Add user 0 nok3
    userIds->AppendL( iUserId2 ); // Add user 1 nok5

    _LIT( KContentType, "text/plain" );
    _LIT( KMessage, "6. Hello In Two Times Valid UserId" );
    TInt OpId = 0;
    // Send Message
    TRAPD ( err, OpId = iImClient->SendPToPMessageL( *userIds, KMessage ) );
    // Catch errors
    if ( err == KImApiErrInvalidUserId && OpId == 0 ) // KImApiErrInvalidUserId
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidTwoUserIds) - err=%d" ), err );
        CleanupStack::PopAndDestroy(); // userIds
        iStatus = err;
        return err;
        }
    else if ( err == KImApiGeneralError && OpId == 0 ) // General ImApi errror
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidTwoUserIds) - err=%d" ), err );
        CleanupStack::PopAndDestroy(); // userIds
        iStatus = err;
        return err;
        }
    else
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidTwoUserIds) - err=%d" ), err );
        CleanupStack::PopAndDestroy(); // userIds
        iStatus = KRequestPending;
        SetActive();
        return KErrNone;
        }
    }


TInt CTestRunner::SendPToPValidContactL()
    {
    CTestLogger1::Log( _L( "Test Case Number #9" ) );
    CContactIdArray* contactIDArray = GetWVContactIdsLC();
    if ( contactIDArray->Count() == 0 )
        {
        CTestLogger1::Log( _L( "Can not find WvContact ID" ) );
        CleanupStack::PopAndDestroy();
        iStatus = KErrNone;
        Cancel();
        CActiveScheduler::Stop();
        return KErrNone;
        }
    TInt OpId = 0;
    // Be sure that two contact is real WV contact
    // Select first contact
    TContactItemId& OmaContact = ( *contactIDArray )[0];

    //Add first contact
    CContactIdArray* myContactIds = CContactIdArray::NewL();
    myContactIds->AddL( OmaContact );

    // Check that we have only one contact
    TInt nuberofcontacts = myContactIds->Count();
    //CTestLogger1::Log( _L("nuberofcontacts =%d"), nuberofcontacts );
    //Send message to myContacts
    TRAPD( err, OpId = iImClient->SendPToPMessageL( OmaContact/*myContactIds*/, _L( "12. Hello One Valid Contact" ) ) );
    // Just checking return value
    if ( err == KImApiErrInvalidContactId && OpId == 0 ) // KImApiErrInvalidContactId and OperationId is 0
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidContact) - err=%d" ), err );
        CleanupStack::PopAndDestroy();
        iStatus = KErrNone;
        Cancel();
        CActiveScheduler::Stop();
        return err;
        }
    else if ( err == KImApiGeneralError && OpId == 0 ) // General ImApi error
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidContact) - err=%d" ), err );
        CleanupStack::PopAndDestroy(); // userIds
        iStatus = err;
        return err;
        }
    else
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidContact) - err=%d" ), err );
        CleanupStack::PopAndDestroy();
        iStatus = KRequestPending;
        SetActive();
        return KErrNone; //OpId;
        }
    }


TInt CTestRunner::SendPToPValidTwoContactL()
    {
    CTestLogger1::Log( _L( "Test Case Number #10" ) );
    CContactIdArray* contactIDArray = GetWVContactIdsLC();
    TInt OpId = 0;
    // Be sure that two contact is real WV contact
    //Is there three contacts
    TInt nuberofcontacts = contactIDArray->Count();
    if ( contactIDArray->Count() == 0 )
        {
        CTestLogger1::Log( _L( "Can not find WvContact ID" ) );
        CleanupStack::PopAndDestroy();
        iStatus = KErrNone;
        Cancel();
        CActiveScheduler::Stop();
        return KErrNone;
        }
    // Remove third contact
    // Never commit the reset
    if ( contactIDArray->Count() >= 3 )
        {
        contactIDArray->Remove( 2 );
        }


    // Check if Remove is OK
    TRAPD( err1, OpId = iImClient->SendPToPMessageL( *contactIDArray, _L( "text/plain" ), _L8( "Hello All" ) ) );

    //Send message
    TRAPD( err, OpId = iImClient->SendPToPMessageL( *contactIDArray, _L( "13. Hello In Two Times Valid Contact" ) ) );

    // Just checking return value
    if ( err == KImApiErrInvalidContactId && OpId == 0 ) // KImApiErrInvalidContactId and OperationId is 0
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidTwoContact) - err=%d" ), err );
        CleanupStack::PopAndDestroy();
        iStatus = KErrNone;
        Cancel();
        CActiveScheduler::Stop();
        return err;
        }
    else if ( err == KImApiGeneralError && OpId == 0 ) // General ImApi error
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidTwoContact) - err=%d" ), err );
        CleanupStack::PopAndDestroy(); // userIds
        iStatus = err;
        return err;
        }
    else
        {
        CTestLogger1::Log( _L( "SendPToPMessageL(ValidTwoContact) - err=%d" ), err );
        CleanupStack::PopAndDestroy();
        iStatus = KRequestPending;
        SetActive();
        return KErrNone; // OpId;
        }
    }

TInt CTestRunner::SendPToPContactContentTypeL()
    {
    //This SDK API is not supported. Test case is added to increase the code coverage
    CTestLogger1::Log( _L( "Test Case Number #11" ) );
    CTestLogger1::Log( _L( "SendPToPContactContentTypeL" ) );
    TInt OpId = 0;
    CContactIdArray* contactIDArray = GetWVContactIdsLC();
    if ( contactIDArray->Count() == 0 )
        {
        CTestLogger1::Log( _L( "Can not find WvContact ID" ) );
        CleanupStack::PopAndDestroy();
        iStatus = KErrNone;
        Cancel();
        CActiveScheduler::Stop();
        return KErrNone;
        }
    TRAPD( err, OpId = iImClient->SendPToPMessageL( *contactIDArray, _L( "text/plain" ), _L8( "Hello All" ) ) );
    CleanupStack::PopAndDestroy(); // userIds
    iStatus = err;
    Cancel();
    CActiveScheduler::Stop();
    return err;
    }

TInt CTestRunner::SendPToPUserIDContentTypeL()
    {
    //This SDK API is not supported. Test case is added to increase the code coverage
    CTestLogger1::Log( _L( "Test Case Number #12" ) );
    CTestLogger1::Log( _L( "SendPToPUserIDContentTypeL" ) );
    TInt OpId = 0;
    CDesCArray* userIds = new( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( userIds );

    TRAPD( err, OpId = iImClient->SendPToPMessageL( *userIds, _L( "text/plain" ), _L8( "Hello All" ) ) );
    CleanupStack::PopAndDestroy(); // userIds
    iStatus = err;
    Cancel();
    CActiveScheduler::Stop();
    return err;
    }


TInt CTestRunner::QueryIMConnectionStatusL()
    {
    CTestLogger1::Log( _L( "Test Case Number #13" ) );
    TInt ret = iOpenApi->ImConnectionStatus();
    // Check returnvalue
    if ( ret == 0 ) // iLoggedIn
        {
        CTestLogger1::Log( _L( "LogoutL" ) );
        iOpenApi->LogoutL();
        iStatus = KErrNone;
        return 0;
        }
    else // 1
        {
        iStatus = KErrNone;
        Cancel();
        CActiveScheduler::Stop();
        return ret;
        }
    }


TInt CTestRunner::CancelLoginL()
    {
    CTestLogger1::Log( _L( "Test Case Number #14" ) );
    LoginL( iUserIdValue );
    TRAPD ( err, iOpenApi->CancelLoginL() );
    CTestLogger1::Log( _L( "CancelLoginL - err=%d" ), err );
    iImClient->UnregisterObserver();
    CTestLogger1::Log( _L( "Test Case Number #15" ) );
    CTestLogger1::Log( _L( "CImClient::UnregisterObserver" ) );
    }

//**********************************
// Logger
//**********************************
const TInt KLogBufferLength = 256;
_LIT( KLogDir, "impsc" );
_LIT( KLogFile, "IMAPITestCaseLog.txt" );

EXPORT_C void CTestLogger1::Log( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList( aFmt, list );

    // Write to log file
    RFileLogger::Write( KLogDir, KLogFile, EFileLoggingModeAppend, buf );
    }

