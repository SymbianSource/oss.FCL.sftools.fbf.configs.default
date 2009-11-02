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
* Description:  Testing functions present in "notepad_library_api"
*
*/



#ifndef TESTNPDAPI_H
#define TESTNPDAPI_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <NpdApi.h>
#include <NpdLib.rsg>
#include <flogger.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KTestNpdApiLogPath, "\\logs\\testframework\\TestNpdApi\\" ); 
// Log file
_LIT( KTestNpdApiLogFile, "TestNpdApi.txt" ); 
_LIT( KTestNpdApiLogFileWithTitle, "TestNpdApi_[%S].txt" );

_LIT( KExampleFilePath, "c:\\testing\\data\\TestNpdApi.txt" );
_LIT( KCcpFilePath, "c:\\data\\TestNpdApi.txt");

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CTestNpdApi;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CTestNpdApi test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CTestNpdApi) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestNpdApi* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTestNpdApi();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CTestNpdApi( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below, explanation is given in cpp file. 
        */
        virtual TInt FetchTemplate( CStifItemParser& aItem );
        virtual TInt FetchMemo( CStifItemParser& aItem );
        virtual TInt NoOfTemplates( CStifItemParser& aItem );
        virtual TInt SaveFileAsMemoUsingFileName( CStifItemParser& aItem );
        virtual TInt SaveFileAsMemoUsingHandle( CStifItemParser& aItem );
        virtual TInt AddContent( CStifItemParser& aItem );
        virtual TInt Model( CStifItemParser& aItem );
        virtual TInt Dialog( CStifItemParser& aItem );
        virtual TInt ExistsMemoL( CStifItemParser& aItem );
        virtual TInt NumberOfTemplates( CStifItemParser& aItem );
        virtual TInt CreateModelL( CStifItemParser& aItem );
        virtual TInt NewL( CStifItemParser& aItem );
        virtual TInt ProbeMemoL( CStifItemParser& aItem );
        virtual TInt CTestNpdApi::NewLC( CStifItemParser& aItem );
        virtual TInt CTestNpdApi::CNotepadApiDestruction(CStifItemParser& aItem);
        virtual TInt CTestNpdApi::CreateListDialogL(CStifItemParser& aItem);
        virtual TInt CTestNpdApi::CreateViewerDialogL(CStifItemParser& aItem);
        virtual TInt CTestNpdApi::ExecTemplatesL( CStifItemParser& aItem );        

        virtual TInt CTestNpdApi::CreateViewer(CStifItemParser& aItem);
        virtual TInt CTestNpdApi::ExecFileViewerUsingFilename( CStifItemParser& aItem );
        virtual TInt CTestNpdApi::ExecFileViewerUsingHandle( CStifItemParser& aItem );
        virtual TInt CTestNpdApi::ExecTextViewerL(CStifItemParser& aItem );       
        virtual TInt CTestNpdApi::ExecTextEditorL (CStifItemParser& aItem );
        virtual TInt CTestNpdApi::ExecReadOnlyTextViewerL(CStifItemParser& aItem );
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;
        CNotepadApi *iptrToNpd;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // TESTNPDAPI_H

// End of File
