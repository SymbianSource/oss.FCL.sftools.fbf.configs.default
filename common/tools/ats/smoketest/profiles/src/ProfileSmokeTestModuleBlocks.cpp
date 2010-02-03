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
#include <MProEngEngine.h>
#include <MProEngProfile.h>
#include <MProEngProfileName.h>
#include <MProEngToneSettings.h>
#include <ProEngFactory.h>
#include <Profile.hrh>
#include "ProfileSmokeTestModule.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
_LIT( KProfileSmokeTestModule, "ProfileSmokeTestModule" );
_LIT( KProfile, "Profile: %S" );
_LIT( KRingingVolume, "Ringing Volume: %d");

_LIT(KGeneral, 	"General");
_LIT(KSilent,  	"Silent");
_LIT(KMeeting, 	"Meeting");
_LIT(KOutdoor,	"Outdoor");

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
// CProfileSmokeTestModule::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CProfileSmokeTestModule::Delete() 
    {
    }

// -----------------------------------------------------------------------------
// CProfileSmokeTestModule::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CProfileSmokeTestModule::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CheckActiveProfile", CProfileSmokeTestModule::CheckActiveProfileL),
        ENTRY( "SetActiveProfile", CProfileSmokeTestModule::SetActiveProfileL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CProfileSmokeTestModule::CheckActiveProfileL
// Changes the current active profile according to the parameter
// -----------------------------------------------------------------------------
//
TInt CProfileSmokeTestModule::CheckActiveProfileL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KSetProfile, "In CheckActiveProfile" );
    TestModuleIf().Printf( 0, KProfileSmokeTestModule, KSetProfile );
    // Print to log file
    iLog->Log( KSetProfile );

	MProEngEngine* engine = ProEngFactory::NewEngineLC( iFs );
	MProEngProfile* active = engine->ActiveProfileLC();
    
    TPtrC expectedName;
    if ( aItem.GetNextString( expectedName ) == KErrNone )
    	{
    	TL( active->ProfileName().Name() == expectedName)
    	}
    
    TInt expectedRingVol;
    if ( aItem.GetNextInt( expectedRingVol ) == KErrNone )
    	{
    	TL(active->ToneSettings().RingingVolume() == expectedRingVol);
    	}
    
    TInt expectedRingType;
    if ( aItem.GetNextInt( expectedRingType ) == KErrNone )
    	{
    	TL(active->ToneSettings().RingingType() == expectedRingType);
    	}
    
    TPtrC vibrate;
    if ( aItem.GetNextString(vibrate) == KErrNone )
    	{
    	TBool expectedVibrate;
    	if ( vibrate == _L("true"))
    		{
    		expectedVibrate = ETrue;
    		}
    	else if ( vibrate == _L("false"))
    		{
    		expectedVibrate = EFalse;
    		}
    	else
    		{
    		User::Leave(KErrArgument);
    		}
    	
    	TL(expectedVibrate == active->ToneSettings().VibratingAlert());
    	}
    
    CleanupStack::PopAndDestroy(2);
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CProfileSmokeTestModule::SetProfileL
// Changes the current active profile according to the parameter
// -----------------------------------------------------------------------------
//
TInt CProfileSmokeTestModule::SetActiveProfileL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KSetProfile, "In SetActiveProfile" );
    TestModuleIf().Printf( 0, KProfileSmokeTestModule, KSetProfile );
    // Print to log file
    iLog->Log( KSetProfile );

    TPtrC profile;
    if ( aItem.GetNextString( profile ) == KErrNone )
    	{
    	MProEngEngine* engine = ProEngFactory::NewEngineLC( iFs );

    	// create the profile engine and set to the correct profile
    	if ( profile == KSilent)
    		{
    		//set to silent
    		engine->SetActiveProfileL( EProfileSilentId );
    		}
    	else if ( profile == KMeeting )
    		{
    		// set to meeting
    		engine->SetActiveProfileL( EProfileMeetingId );
    		}
    	else if ( profile == KGeneral )
    		{
    		// set to general
    		engine->SetActiveProfileL( EProfileGeneralId );
    		}
    	else if (profile == KOutdoor)
    		{
    		// set to outdoors
    		engine->SetActiveProfileL( EProfileOutdoorId );
    		}

    	CleanupStack::PopAndDestroy();
    	}
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CProfileSmokeTestModule::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CProfileSmokeTestModule::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
