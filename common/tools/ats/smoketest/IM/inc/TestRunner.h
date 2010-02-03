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
* Description: The class handles the test case running.
*
*/


#ifndef TESTRUNNER_H
#define TESTRUNNER_H

// INCLUDES
#include <f32file.h>
#include <e32test.h>
#include "IMApiMTHandlers.h"
#include "StifTestModule.h"
#include <StifLogger.h>
#include <badesca.h>

#include <cntdb.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntitem.h>
#include <commdb.h>



// FORWARD DECLARATIONS
class CTestRunner;

// FORWARD DECLARATIONS
class CErrorHandler;
class CAccessHandler;
class CTestIdleTimer;
class CTestTimer;
class CPureHandler;

// ENUMS

enum TWhichCase
    {
    InvalidLogin = 1,
    Login,
    SendMessageValidUserId,
    SendMessageValidTwoUserId,
    SendMessageValidContact,
    SendMessageValidTwoContact,
    SendPToPContactContentType,
    SendPToPUserIDContentType,
    Logout,
    CancelLogin,
    Finish,
	CreateImClient
    };


// CONSTANTS
const TInt KImpsMaxLen       = 100;
const TInt KImpsMaxSapLen       = 100;
const TInt KImpsMaxUserIdLen    =  50;
const TInt KImpsMaxClientIdLen 	= 64;  // maximum Client ID, this is not sure!


// CLASS DECLARATIONS


class CTestRunner : public CActive
    {
    public:
        /**
        * Static factory method,
        *
        * Entry point into Symbian's 2 phase construction pattern.
        */
        static CTestRunner* NewL();

        /**
        * Destructor.
        */
        virtual ~CTestRunner();

        /**
        * Start the execution.
        */
        void StartRunL( TWhichCase aCase );

        CContactIdArray* GetWVContactIdsLC();

        /**
        * Symbian's 2 phase construction pattern.
        */
        void ConstructL( );

        TInt CreateInstanceL();
        TInt RegisterConnObserverL();
        TInt LoadIMInterfaceL();
        TInt IMRegisterL();
        TInt LoginL( const TDesC& aUserID );
        TInt SendPToPValidUserIDL();
        TInt SendPToPValidTwoUserIDL();
        TInt SendPToPValidContactL();
        TInt SendPToPValidTwoContactL();
        TInt SendPToPContactContentTypeL();
        TInt SendPToPUserIDContentTypeL();
        TInt QueryIMConnectionStatusL();
        TInt CancelLoginL();

        void GetDefaultAccessPoint();

        void ReadIniFileL();
        TBuf<KImpsMaxSapLen> iSapValue;
        TBuf<KImpsMaxUserIdLen> iUserIdValue;
        TBuf<KImpsMaxLen> iPassword;
        TBuf<KImpsMaxClientIdLen>  iClientId;  // 1.3 Client ID Implementation
        TBuf<KImpsMaxLen> iUserId1;
        TBuf<KImpsMaxLen> iUserId2;
        TInt iIAP;

    public: // Functions from base classes

        /**
        * Active object completion
        */
        void RunL();
        /**
        * Active object cancellation
        */
        void DoCancel();

    private:

        // By default, prohibit copy constructor
        CTestRunner( const CTestRunner& );

        // Prohibit assigment operator
        CTestRunner& operator= ( const CTestRunner& );

        /**
        * C-style constructor.
        */
        CTestRunner();

        // actual runner, that is leave trapped
        void DoRunL();

    private:    // Data

        TWhichCase				iCase;
        CIMApiClientHandler*     iImObserver; //earlier code
        //MImObserver*     iImObserver;
        CConnectionObserver*    iConnObserver;
        CImConnection*          iOpenApi;
        MImClient*              iImClient;
        CStifLogger*			iLog;
        CContactDatabase*       iDatabase;
    };



class CTestScheduler : public CActiveScheduler
    {
    public:
        void Error ( TInt aError ) const;
    };

class CTestLogger1 : public CBase
    {
    public:
        //IMPORT_C static void CTestLogger::Log(TRefByValue<const TDesC> aFmt,...);
        IMPORT_C static void Log( TRefByValue<const TDesC> aFmt, ... );
    };

#endif
