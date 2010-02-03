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
* Description: This module contains the implementation of CIMApiTest class
    member functions that does the actual tests.
*
*
*/



// INCLUDE FILES
#include <e32math.h>
#include "IMApiTest.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CIMApiTest class
    member functions that does the actual tests.

-------------------------------------------------------------------------------
*/

// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: CIMApiTest

    Method: Case

    Description: Returns a test case by number.

    This function contains an array of all available test cases
    i.e pair of case name and test function. If case specified by parameter
    aCaseNumber is found from array, then that item is returned.

    The reason for this rather complicated function is to specify all the
    test cases only in one place. It is not necessary to understand how
    function pointers to class member functions works when adding new test
    cases. See function body for instructions how to add new test case.

    Parameters:    const TInt aCaseNumber :in:      Test case number

    Return Values: const TCaseInfo Struct containing case name & function

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
const TCaseInfo CIMApiTest::Case (
    const TInt aCaseNumber ) const
    {

    /*
    * To add new test cases, implement new test case function and add new
    * line to KCases array specify the name of the case and the function
    * doing the test case
    * In practice, do following
    *
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to
    *    OOMHard.cpp file and to OOMHard.h
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and
    * TInt value for last heap memory allocation failure.
    *
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation
    * failure and TInt value for last heap memory allocation failure.
    */

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array

        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g.
        // CIMApiTest::PrintTest. Otherwise the compiler
        // gives errors.

		ENTRY( "Create IM Client", CIMApiTest::CreateImClientL ),
//        ENTRY( "Invalid Login", CIMApiTest::InvalidLoginL ),
//        ENTRY( "Login", CIMApiTest::LoginL ),
//        ENTRY( "Send P To P ValidUserID", CIMApiTest::SendPToPValidUserIDL ),
//        ENTRY( "Send P To P ValidTvoUserID", CIMApiTest::SendPToPValidTwoUserIDL ),
//        ENTRY( "Send P To P ValidContact", CIMApiTest::SendPToPValidContactL ),
//        ENTRY( "Send P To P ValidTwoContact", CIMApiTest::SendPToPValidTwoContactL ),
//        ENTRY( "Send P To P ContactContentType", CIMApiTest::SendPToPContactContentTypeL ),
//        ENTRY( "Send P To P UserIDContentType", CIMApiTest::SendPToPUserIDContentTypeL ),
//        ENTRY( "Logout", CIMApiTest::LogoutL ),
//        ENTRY( "Cancel Login", CIMApiTest::CancelLoginL ),
        };

    // Verify that case number is valid
    if ( ( TUint ) aCaseNumber >= sizeof( KCases ) /
         sizeof( TCaseInfoInternal ) )
        {

        // Invalid case, construct empty object
        TCaseInfo null( ( const TText* ) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;

        }

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

TInt CIMApiTest::CreateImClientL( TTestResult& aResult )
    {
    iRunner->StartRunL( CreateImClient );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "CreateImClientL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;

    }
	
TInt CIMApiTest::InvalidLoginL( TTestResult& aResult )
    {
    iRunner->StartRunL( InvalidLogin );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "LoginL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;

    }

TInt CIMApiTest::LoginL( TTestResult& aResult )
    {
    iRunner->StartRunL( Login );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "LoginL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;
    }


TInt CIMApiTest::SendPToPValidUserIDL( TTestResult& aResult )
    {
    iRunner->StartRunL( SendMessageValidUserId );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "SendPToPValidUserIDL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;
    }


TInt CIMApiTest::SendPToPValidTwoUserIDL( TTestResult& aResult )
    {
    iRunner->StartRunL( SendMessageValidTwoUserId );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "SendPToPValidTwoUserIDL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;
    }


TInt CIMApiTest::SendPToPValidContactL( TTestResult& aResult )
    {
    iRunner->StartRunL( SendMessageValidContact );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "SendPToPValidContactL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;
    }


TInt CIMApiTest::SendPToPValidTwoContactL( TTestResult& aResult )
    {
    iRunner->StartRunL( SendMessageValidTwoContact );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "SendPToPValidTwoContactL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;
    }

TInt CIMApiTest::SendPToPContactContentTypeL( TTestResult& aResult )
    {
    iRunner->StartRunL( SendPToPContactContentType );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "SendPToPContactContentTypeL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;
    }

TInt CIMApiTest::SendPToPUserIDContentTypeL( TTestResult& aResult )
    {
    iRunner->StartRunL( SendPToPUserIDContentType );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "SendPToPUserIDContentTypeL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;
    }

TInt CIMApiTest::LogoutL( TTestResult& aResult )
    {
    iRunner->StartRunL( Logout );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "LogoutL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;
    }


TInt CIMApiTest::CancelLoginL( TTestResult& aResult )
    {
    iRunner->StartRunL( CancelLogin );
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "CancelLoginL Passed" );
    aResult.SetResult( KErrNone, KDescription );
    // Case was executed
    return KErrNone;
    }

// End of File
