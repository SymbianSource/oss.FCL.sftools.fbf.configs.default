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
#include <MGFetch.h>
#include <mediafiletypes.hrh>
#include "MediaMgmtSmokeTestModule.h"
#include "MediaMgmtObserver.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
_LIT(KTypeImage, "Images");
_LIT(KTypeAudio, "Audio");
_LIT(KTypeVideo, "Video");
_LIT(KTypeAny,	 "Any");

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
// CMediaMgmtSmokeTestModule::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMediaMgmtSmokeTestModule::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMediaMgmtSmokeTestModule::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMediaMgmtSmokeTestModule::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "ListMediaItems", CMediaMgmtSmokeTestModule::ListMediaItemsL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMediaMgmtSmokeTestModule::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediaMgmtSmokeTestModule::ListMediaItemsL( CStifItemParser& aItem )
    {
    CPtrCArray* expectedItems = new (ELeave) CPtrCArray(5);
    CleanupArrayDeletePushL(expectedItems);
    
    // Print to UI
    _LIT( KMediaMgmtSmokeTestModule, "MediaMgmtSmokeTestModule" );
    _LIT( KListImages, "In ListMediaItemsL" );
    TestModuleIf().Printf( 0, KMediaMgmtSmokeTestModule, KListImages );
    // Print to log file
    iLog->Log( KListImages );

    TInt i = 0;
    TPtrC type;
    _LIT( KParam, "Param[%i]: %S" );
    if ( aItem.GetNextString( type ) == KErrNone )
    	{
    	TPtrC itemPath;
    	while ( aItem.GetNextString ( itemPath ) == KErrNone )
    		{
    		TestModuleIf().Printf( i, KMediaMgmtSmokeTestModule, 
									KParam, i, &itemPath );
    		expectedItems->AppendL( itemPath );
    		i++;
    		}
    
    	CDesCArrayFlat* media = new (ELeave) CDesCArrayFlat(5);
    	CleanupArrayDeletePushL(media);
    	
    	// work out the file type we are looking for
    	TMediaFileType mediaType;
    	if (type == KTypeImage)
    		{
    		mediaType = EImageFile;
    		}
    	else if (type == KTypeAudio)
    		{
    		mediaType = EAudioFile;
    		}
    	else if (type == KTypeVideo)
    		{
    		mediaType = EVideoFile;
    		}
    	else
    		{
    		User::Leave(KErrArgument);
    		}
    	
    	MediaMgmtObserver* observer = new (ELeave) MediaMgmtObserver;
    
    	if (MGFetch::RunL(*media, mediaType, EFalse, observer))
    		{
    		//compare the two
    		TInt total = media->Count();
    		}
    
    	CleanupStack::PopAndDestroy(media);
    	}
    
    CleanupStack::PopAndDestroy(expectedItems);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediaMgmtSmokeTestModule::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CMediaMgmtSmokeTestModule::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
