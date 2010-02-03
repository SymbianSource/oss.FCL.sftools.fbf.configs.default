/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <ocrsrv.h>
#include <ptiengine.h>
#include <ptiuserdictionary.h>
#include <Ecom/Ecom.h>
#include "InputMethodSmokeTestModule.h"
#include "TestOcrObserver.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TInt KMaxSize = 32;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CInputMethodSmokeTestModule::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CInputMethodSmokeTestModule::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CInputMethodSmokeTestModule::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CInputMethodSmokeTestModule::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CreateOcrEngine", CInputMethodSmokeTestModule::CreateOcrEngineL ),
        ENTRY( "CreatePtiEngine", CInputMethodSmokeTestModule::CreatePtiEngineL ),
        ENTRY( "CreatePtiDictionary", CInputMethodSmokeTestModule::CreatePtiDictionaryL ),
        ENTRY( "MultitapWord", CInputMethodSmokeTestModule::MultitapWordL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CInputMethodSmokeTestModule::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CInputMethodSmokeTestModule::CreateOcrEngineL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KInputMethodSmokeTestModule, "InputMethodSmokeTestModule" );
    _LIT( KCreateOcrEngine, "In Create OCR Engine" );
    
    iLog->Log( KCreateOcrEngine );
    
    TOcrEngineEnv engineEnv;
    engineEnv.iPriority = EPriorityNormal;
    engineEnv.iMaxHeapSize = 1200+KMinHeapGrowBy+1;
    
    TTestOcrObserver observer;
    
    MOCREngineInterface* ocrEngine = NULL;
    ocrEngine = OCREngineFactory::CreateOCREngineL(observer, engineEnv, OCREngineFactory::EEngineLayoutRecognize);
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CInputMethodSmokeTestModule::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CInputMethodSmokeTestModule::CreatePtiEngineL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KInputMethodSmokeTestModule, "InputMethodSmokeTestModule" );
    _LIT( KCreatePtiEngine, "In Create PTI Engine" );
    
    iLog->Log( KCreatePtiEngine );
    
    CPtiEngine* ptiEngine = CPtiEngine::NewL();
    CleanupStack::PushL(ptiEngine);
    
    ptiEngine->ActivateLanguageL(ELangEnglish, EPtiEngineMultitapping);
    
    CleanupStack::PopAndDestroy(ptiEngine);
    REComSession::FinalClose();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CInputMethodSmokeTestModule::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CInputMethodSmokeTestModule::CreatePtiDictionaryL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KInputMethodSmokeTestModule, "InputMethodSmokeTestModule" );
    _LIT( KCreatePtiDictionary, "In Create PTI Dictionary" );
    
    iLog->Log( KCreatePtiDictionary );
    
    CPtiUserDictionary* ptiDictionary = CPtiUserDictionary::NewL(KMaxSize);

    delete ptiDictionary;
    ptiDictionary = NULL;
    
    REComSession::FinalClose();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CInputMethodSmokeTestModule::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CInputMethodSmokeTestModule::MultitapWordL(CStifItemParser& aItem)
	{
    // Print to UI
    _LIT( KInputMethodSmokeTestModule, "InputMethodSmokeTestModule" );
    _LIT( KMultitapWord, "In Multitap Word" );
    
    iLog->Log( KMultitapWord );
	
	TPtrC inputSequence;
	if ( aItem.GetNextString(inputSequence) == KErrNone )
		{
		CPtiEngine *ptiEngine = CPtiEngine::NewL();
		CleanupStack::PushL(ptiEngine);

		// setup multitap
		ptiEngine->ActivateLanguageL(ELangEnglish, EPtiEngineMultitapping);
		PtiEngSimulateInput(ptiEngine, inputSequence);
		
		TPtrC expectedWord;
		if ( aItem.GetNextString(expectedWord) == KErrNone )
			{
			if ( ptiEngine->CurrentWord() == expectedWord )
				{
				iLog->Log(_L("Words matched"));
				}
			else
				{
				User::Leave(KErrArgument);
				}
			}

		ptiEngine->CommitCurrentWord();

		ptiEngine->CloseCurrentLanguageL();
		CleanupStack::PopAndDestroy(ptiEngine);
		REComSession::FinalClose();
		}

	return KErrNone;
	}

void CInputMethodSmokeTestModule::PtiEngSimulateInput(CPtiEngine* aEngine, const TDesC& aKeys)
    {
    for(int i=0; i<aKeys.Length(); ++i)
        {
        if (aKeys[i] == ' ')
            {
            continue;
            }
        else if (aKeys[i] == 'p')
            {
            continue;
            }
        else if (aKeys[i] == 'd')
            {
            aEngine->DeleteKeyPress();
            }
        else
            {
            aEngine->AppendKeyPress(aEngine->CharacterToKey(aKeys[i]));
            }
        }
    }


// -----------------------------------------------------------------------------
// CInputMethodSmokeTestModule::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CInputMethodSmokeTestModule::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
