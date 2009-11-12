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
* Description: Handler class
*
*/


#ifndef TESTHANDLERS_H
#define TESTHANDLERS_H


#include <e32base.h>
#include "imconnection.h"
#include "imclient.h"
#include "imerrors.h"
#include    <flogger.h>
// FORWARD Declarations

// CLASS DECLARATIONS

//*************************************************************************************
// CIMApiClientHandler
//*************************************************************************************
class CIMApiClientHandler : public MImObserver, public CBase
    {
    public:
        static CIMApiClientHandler* NewL( TRequestStatus& aStatus );
        virtual ~CIMApiClientHandler();

        void HandleRegisterL(
            const TInt aErrorCode );

        void HandleMessageSentL(
            const TInt aOpCode,
            const TInt aErrorCode );

        void HandleNewPToPMessageL(
            const TInt aErrorCode,
            const TContactItemId  aContactId,
            const TDesC& aUserId,
            const TDesC& aMessageType,
            const TDesC8& aContent );

        void HandleNewPToPMessageL(
            const TInt aErrorCode,
            const TContactItemId  aContactId,
            const TDesC& aUserId,
            const TDesC& aMessageType,
            const TDesC16& aContent );

        void HandleSendErrorL(
            const TInt aOpCode,
            const TInt aErrorCode,
            MImClientDetailedError* aDetailedError );

//    protected:
        void TimedOut();

    private:
        CIMApiClientHandler( TRequestStatus& aStatus );
        void ConstructL();

    private:
        TRequestStatus&      iStatus;

    };


class CConnectionObserver : public CBase,
            public MImConnectionObserver
    {
    public:
        static CConnectionObserver* NewL( TRequestStatus& aStatus );
        virtual ~CConnectionObserver();

        void HandleConnectL( const TInt aErrorCode );
        void HandleDisconnectL();

        void HandleLoginL( const TInt aErrorCode );
        void HandleCancelLoginL( const TInt aErrorCode );
        void HandleLogoutL( const TInt aErrorCode );

    private:
        CConnectionObserver( TRequestStatus& aStatus );
        void ConstructL();

    private:
        TRequestStatus&     iStatus;
    };

class CTestLogger : public CBase
    {
    public:
        //IMPORT_C static void CTestLogger::Log(TRefByValue<const TDesC> aFmt,...);
        IMPORT_C static void Log( TRefByValue<const TDesC> aFmt, ... );
    };




#endif
