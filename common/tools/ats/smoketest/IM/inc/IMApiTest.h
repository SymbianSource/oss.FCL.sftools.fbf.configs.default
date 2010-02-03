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
* Description: This class contains all test framework related parts of
    this test module.
*
*
*/



/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This file contains the header file of IMApiTest of
    STIF TestFramework.

-------------------------------------------------------------------------------
*/
#ifndef IMApiTest_H
#define IMApiTest_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include "IMApiMTHandlers.h"
#include "imerrors.h"
#include "imconnection.h"
#include "imclient.h"
#include "TestRunner.h"

// CONSTANTS
// None

// MACROS
// None

// Logging path
_LIT( KIMApiTestLogPath, "\\logs\\testframework\\IMApiTest\\" );
// Log file
_LIT( KIMApiTestLogFile, "IMApiTest.txt" );

// Function pointer related internal definitions

#define GETPTR &
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CIMApiTest;

// DATA TYPES
// None

// A typedef for function that does the actual testing,
// function is a type
// TInt CIMApiTest::<NameOfFunction> ( TTestResult& aResult )
typedef TInt ( CIMApiTest::* TestFunction )( TTestResult& );

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfoInternal
    {
    public:
        const TText* iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

// CLASS DECLARATION

/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfo
    {
    public:
        TPtrC iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

        TCaseInfo( const TText* a ) : iCaseName( ( TText* ) a )
            {
            };

    };


// CLASS DECLARATION

/**
*  This a IMApiTest class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CIMApiTest ) : public CTestModuleBase
    {
public:  // Constructors and destructor


    /**
    * Two-phased constructor.
    */
    static CIMApiTest* NewL();

    /**
    * Destructor.
    */
    virtual ~CIMApiTest();

public: // New functions
    // None

public: // Functions from base classes

    /**
    * From CTestModuleBase InitL is used to initialize the
    *       IMApiTest. It is called once for every instance of
    *       TestModuleIMApiTest after its creation.
    * @since ?Series60_version
    * @param aIniFile Initialization file for the test module (optional)
    * @param aFirstTime Flag is true when InitL is executed for first
    *               created instance of IMApiTest.
    * @return Symbian OS error code
    */
    TInt InitL( TFileName& aIniFile, TBool aFirstTime );

    /**
    * From CTestModuleBase GetTestCasesL is used to inquiry test cases
    *   from IMApiTest.
    * @since ?Series60_version
    * @param aTestCaseFile Test case file (optional)
    * @param aTestCases  Array of TestCases returned to test framework
    * @return Symbian OS error code
    */
    TInt GetTestCasesL( const TFileName& aTestCaseFile,
                        RPointerArray<TTestCaseInfo>& aTestCases );

    /**
    * From CTestModuleBase RunTestCaseL is used to run an individual
    *   test case.
    * @since ?Series60_version
    * @param aCaseNumber Test case number
    * @param aTestCaseFile Test case file (optional)
    * @param aResult Test case result returned to test framework (PASS/FAIL)
    * @return Symbian OS error code (test case execution error, which is
    *           not reported in aResult parameter as test case failure).
    */
    TInt RunTestCaseL( const TInt aCaseNumber,
                       const TFileName& aTestCaseFile,
                       TTestResult& aResult );

    /**
    * From CTestModuleBase; OOMTestQueryL is used to specify is particular
    * test case going to be executed using OOM conditions
    * @param aTestCaseFile Test case file (optional)
    * @param aCaseNumber Test case number (optional)
    * @param aFailureType OOM failure type (optional)
    * @param aFirstMemFailure The first heap memory allocation failure value (optional)
    * @param aLastMemFailure The last heap memory allocation failure value (optional)
    * @return TBool
    */
    virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */,
                                 const TInt /* aCaseNumber */,
                                 TOOMFailureType& aFailureType,
                                 TInt& /* aFirstMemFailure */,
                                 TInt& /* aLastMemFailure */ );

    /**
    * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
    * test environment
    * @param aTestCaseFile Test case file (optional)
    * @param aCaseNumber Test case number (optional)
    * @return None
    */
    virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */,
                                     const TInt /* aCaseNumber */ );
    /**
    * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
    * test environment
    * @param aTestCaseFile Test case file (optional)
    * @param aCaseNumber Test case number (optional)
    * @return None
    */
    virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */,
                                   const TInt /* aCaseNumber */ );

    /**
    * From CTestModuleBase; OOMHandleWarningL
    * @param aTestCaseFile Test case file (optional)
    * @param aCaseNumber Test case number (optional)
    * @param aFailNextValue FailNextValue for OOM test execution (optional)
    * @return None
    */
    virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                    const TInt /* aCaseNumber */,
                                    TInt& /* aFailNextValue */ );
protected:  // New functions
    // None

protected:  // Functions from base classes
    // None

private:

    /**
    * C++ default constructor.
    */
    CIMApiTest();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    // Prohibit copy constructor if not deriving from CBase.
    // ?classname( const ?classname& );
    // Prohibit assigment operator if not deriving from CBase.
    // ?classname& operator=( const ?classname& );

    /**
    * Function returning test case name and pointer to test case function.
    * @since ?Series60_version
    * @param aCaseNumber test case number
    * @return TCaseInfo
    */
    const TCaseInfo Case ( const TInt aCaseNumber ) const;

    /**
    * Printing loop test case.
    * @since ?Series60_version
    * @param aResult Test case result (PASS/FAIL)
    * @return Symbian OS error code (test case execution error
    *   that is not returned as test case result in aResult)
    */
    TInt CreateImClientL( TTestResult& aResult );
	TInt InvalidLoginL( TTestResult& aResult );
    TInt LoginL( TTestResult& aResult );
    TInt SendPToPValidUserIDL( TTestResult& aResult );
    TInt SendPToPValidTwoUserIDL( TTestResult& aResult );
    TInt SendPToPValidContactL( TTestResult& aResult );
    TInt SendPToPValidTwoContactL( TTestResult& aResult );
    TInt SendPToPContactContentTypeL( TTestResult& aResult );
    TInt SendPToPUserIDContentTypeL( TTestResult& aResult );
    TInt LogoutL( TTestResult& aResult );
    TInt CancelLoginL( TTestResult& aResult );

public:     // Data
    // None

protected:  // Data
    // None

private:    // Data
    // Pointer to test (function) to be executed
    TestFunction iMethod;

    // Pointer to logger
    CStifLogger * iLog;
    CTestRunner*   iRunner;

    CIMApiClientHandler*     iImObserver;
    CConnectionObserver*    iConnObserver;
    CImConnection*          iOpenApi;
    MImClient*              iImClient;
    CConsoleBase*           iConsole;
    TBool                   iRepeat;

    /*TInt      iAP;
    TBuf<50>  iUser;
    TBuf<50>  iPwd;
    TBuf<50>  iRecipient;
    TBuf<50>  iRecipient1;
    TBuf<50>  iMySapAp;
    TBuf<50>  iClientId;
    */
    // ?one_line_short_description_of_data
    //?data_declaration;

    // Reserved pointer for future extension
    //TAny* iReserved;

public:     // Friend classes
    // None
    friend class CTestRunner;
protected:  // Friend classes
    // None

private:    // Friend classes
    // None

    };

#endif      // IMApiTest_H

// End of File