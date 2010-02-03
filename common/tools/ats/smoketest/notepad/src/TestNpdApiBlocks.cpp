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
* Description:  Test function for notepad api
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <e32cmn.h>
#include <f32file.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "TestNpdApi.h"
#include <CommonContentPolicy.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

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
// CTestNpdApi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTestNpdApi::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CTestNpdApi::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestNpdApi::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TestFetchTemplate", CTestNpdApi::FetchTemplate ),
        ENTRY( "TestFetchMemo", CTestNpdApi::FetchMemo ),
        ENTRY( "TestNoOfTemplates", CTestNpdApi::NoOfTemplates ),
        ENTRY( "SaveFileAsMemoUsingFileName", CTestNpdApi::SaveFileAsMemoUsingFileName ),
        ENTRY( "SaveFileAsMemoUsingHandle", CTestNpdApi::SaveFileAsMemoUsingHandle ),
        ENTRY( "TestAddContent", CTestNpdApi::AddContent ),
        ENTRY( "Model", CTestNpdApi::Model ),                
        ENTRY( "TestDialog", CTestNpdApi::Dialog ), 
        ENTRY( "TestExistsMemoL", CTestNpdApi::ExistsMemoL ), 
        ENTRY( "TestNumberOfTemplates", CTestNpdApi::NumberOfTemplates ), 
        ENTRY( "TestCreateModelL", CTestNpdApi::CreateModelL ), 
        ENTRY( "TestExecTemplatesL", CTestNpdApi::ExecTemplatesL ), 
        ENTRY( "TestNewL", CTestNpdApi::NewL ), 
        ENTRY( "TestProbeMemoL", CTestNpdApi::ProbeMemoL ), 
        ENTRY( "TestNewLC", CTestNpdApi::NewLC ),
        ENTRY( "TestCNotepadApiDestruction", CTestNpdApi::CNotepadApiDestruction ),
        ENTRY( "TestCreateListDialogL", CTestNpdApi::CreateListDialogL ),
        ENTRY( "TestCreateViewerDialogL", CTestNpdApi::CreateViewerDialogL ), 
        ENTRY( "TestExecTemplatesL", CTestNpdApi::ExecTemplatesL ), 
        ENTRY( "TestExecFileViewerUsingFilename", CTestNpdApi::ExecFileViewerUsingFilename ),
        ENTRY( "TestExecFileViewerUsingHandle", CTestNpdApi::ExecFileViewerUsingHandle ),
        ENTRY( "TestCreateViewer", CTestNpdApi::CreateViewer ),
        ENTRY( "TestExecTextViewerL", CTestNpdApi::ExecTextViewerL ),
        ENTRY( "TestExecTextEditorL", CTestNpdApi::ExecTextEditorL ),
        ENTRY( "TestExecReadOnlyTextViewerL", CTestNpdApi::ExecReadOnlyTextViewerL )
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

//Funcs Defns
// -----------------------------------------------------------------------------
// CTestNpdApi::FetchTemplate
// -----------------------------------------------------------------------------
TInt CTestNpdApi::FetchTemplate( CStifItemParser& aItem )
    {
			HBufC* retData = CNotepadApi::FetchTemplateL();
			if( retData )
				{
					delete retData;
					return KErrNone;
				}
				return KErrGeneral;
		}

// -----------------------------------------------------------------------------
// CTestNpdApi::FetchMemo
// -----------------------------------------------------------------------------
TInt CTestNpdApi::FetchMemo( CStifItemParser& aItem )
    {
			HBufC* retData = CNotepadApi::FetchMemoL();
			if( retData )
				{
					delete retData;
					return KErrNone;
				}
				return KErrGeneral;
		}

// -----------------------------------------------------------------------------
// CTestNpdApi::NoOfTemplates
// -----------------------------------------------------------------------------
TInt CTestNpdApi::NoOfTemplates( CStifItemParser& aItem )
    {
			TInt noOfTmplts = CNotepadApi::NumberOfTemplates();
			//By default there will be a 10 templates, so verifying for more than 0
			if ( noOfTmplts >= 0 )
				{
					return KErrNone;
				}
			return KErrGeneral;
		}

// -----------------------------------------------------------------------------
// CTestNpdApi::SaveFileAsMemoUsingFileName
// -----------------------------------------------------------------------------
TInt CTestNpdApi::SaveFileAsMemoUsingFileName( CStifItemParser& aItem )
    {
    	TInt err = 0;
    	TInt encoding;
    	aItem.GetNextInt(encoding);
			
			//Verifying parameter for correct value
			if ( (encoding != 0) && (encoding != 1) )
				{
				iLog->Log( _L("Wrong parameter, please give either 0 or 1(encoding)") );	
				return KErrGeneral;
				}
			
    	TFileName fileName;
			fileName.Append(KExampleFilePath);
			//If 0, no encoding
			if(encoding == 0)
				{
					TRAP(err, CNotepadApi::SaveFileAsMemoL(fileName));
				}
			else
				{
					TRAP(err, CNotepadApi::SaveFileAsMemoL(fileName, KCharacterSetIdentifierIso88591));			
				}

			if(err == KErrNone)
				{ 
					return KErrNone;
				}
				return err;
		}

// -----------------------------------------------------------------------------
// CTestNpdApi::SaveFileAsMemoUsingHandle
// -----------------------------------------------------------------------------
TInt CTestNpdApi::SaveFileAsMemoUsingHandle( CStifItemParser& aItem )
    {
    	TInt err = 0;
    	TInt encoding;
    	aItem.GetNextInt(encoding);
    	
			//Verifying parameter for correct value
			if ( (encoding != 0) && (encoding != 1) )
				{
				iLog->Log( _L("Wrong parameter, please give either 0 or 1(encoding)") );	
				return KErrGeneral;
				}
    	
    	TFileName fileName;
			fileName.Append( KExampleFilePath );
			
			RFile data;
	    RFs session;
	    User::LeaveIfError( session.Connect() );
	    CleanupClosePushL( session );
	    User::LeaveIfError( data.Open( session, fileName, EFileRead ) );
	    CleanupClosePushL( data );
			
			//If 0, no encoding
			if(encoding == 0)
				{
					TRAP(err, CNotepadApi::SaveFileAsMemoL(data));			
				}
			else
				{
					TRAP(err, CNotepadApi::SaveFileAsMemoL(data, KCharacterSetIdentifierIso88591));			
				}

			CleanupStack::PopAndDestroy( &data );  //data, 
    	CleanupStack::PopAndDestroy( &session );  //session
    	
			if(err == KErrNone)
				{ 
					return KErrNone;
				}
				return err;
		}

// -----------------------------------------------------------------------------
// CTestNpdApi::AddContent
// -----------------------------------------------------------------------------
TInt CTestNpdApi::AddContent( CStifItemParser& aItem )
    {
    	_LIT( KNote, "Saving this text as Notes/Memo");
			TRAPD(err, CNotepadApi::AddContentL( KNote ));		
			//Just verifying the error code
			if(err == KErrNone)
				{ 
					return KErrNone;
				}
				return err;
		}

// -----------------------------------------------------------------------------
// CTestNpdApi::Model
// -----------------------------------------------------------------------------
TInt CTestNpdApi::Model( CStifItemParser& aItem )
    {
    	
    	CNotepadApi* ptrToNpd = CNotepadApi::NewL();	
    	//Not testing the ProbeMemoL() function, but just using to test Model() function.
    	TInt key = 327;
    	ptrToNpd->ProbeMemoL(key);
		  CNotepadModel* retData = ptrToNpd->Model();
			//deleting the pointer as it is not used anymore.
			delete ptrToNpd;
			//Verifying the pointer, is exist means model() is created properly
			if( retData )
				{					
					return KErrNone;
				}
				return KErrGeneral;
		}

// -----------------------------------------------------------------------------
// CTestNpdApi::Dialog
// -----------------------------------------------------------------------------
TInt CTestNpdApi::Dialog( CStifItemParser& aItem )
    {
    CNotepadApi* ptrToNpd = CNotepadApi::NewL();	
    CEikDialog* Dialog = ptrToNpd->Dialog();

    delete ptrToNpd;
    if ( !Dialog )
        {					
        return KErrNone;
        }
    return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// CTestNpdApi::ExistsMemoL
// -----------------------------------------------------------------------------
TInt CTestNpdApi::ExistsMemoL( CStifItemParser& aItem )
    {
    CNotepadApi* ptrToNpd = CNotepadApi::NewL();	
    
    TInt key = 327;
    TBool IsFound = ptrToNpd->ExistsMemoL( key );
    
    //deleting the pointer as it is not used anymore.
    delete ptrToNpd;
    
    if ( IsFound >= 0 )
        {					
        return KErrNone;
        }    
    return KErrGeneral;
    }
    
// -----------------------------------------------------------------------------
// CTestNpdApi::NumberOfTemplates
// -----------------------------------------------------------------------------
TInt CTestNpdApi::NumberOfTemplates(CStifItemParser& aItem)
	{
	CNotepadApi* ptrToNpd = CNotepadApi::NewL();
	TInt Num = ptrToNpd->NumberOfTemplates();
	delete ptrToNpd; 
	if( Num > 0 )
		{
		return KErrNone;
		}
	return KErrGeneral;
	}
	
// -----------------------------------------------------------------------------
// CTestNpdApi::CreateModelL
// -----------------------------------------------------------------------------
TInt CTestNpdApi::CreateModelL( CStifItemParser& aItem )
	{
	CNotepadApi* ptrToNpd = CNotepadApi::NewL();
	TInt modelResId = R_NOTEPAD_FETCH_MEMO_MODEL;
	TRAPD( err, ptrToNpd->CreateModelL(modelResId));
	return err ;
	}
	
// -----------------------------------------------------------------------------
// CTestNpdApi::NewL
// -----------------------------------------------------------------------------
TInt CTestNpdApi::NewL( CStifItemParser& aItem )
	{
	CNotepadApi* ptrToNpd = CNotepadApi::NewL();
	if ( ptrToNpd )
		{
		return KErrNone;
		}
	return KErrGeneral;
	}
	
// -----------------------------------------------------------------------------
// CTestNpdApi::ProbeMemoL
// -----------------------------------------------------------------------------
TInt CTestNpdApi::ProbeMemoL( CStifItemParser& aItem )
	{
    CNotepadApi* ptrToNpd = CNotepadApi::NewL();
    TInt key = 327;
    TInt err = 0;
    TRAP( err, ptrToNpd->ProbeMemoL( key ) );
    delete ptrToNpd;
    
	if(err == KErrNone)
		{ 
		return KErrNone;
		}
	return err;
	}
TInt CTestNpdApi::NewLC( CStifItemParser& aItem )
	{
	CNotepadApi* ptrToNpd = CNotepadApi::NewLC();
	CleanupStack::Pop();
	if( ptrToNpd )
		{
		delete ptrToNpd ;
		return KErrNone ;
		}
	delete ptrToNpd ;
	return KErrGeneral;		
	}

TInt CTestNpdApi::CNotepadApiDestruction(CStifItemParser& aItem)
	{
	CNotepadApi* ptrToNpd = CNotepadApi::NewL();
	TRAPD( err, delete ptrToNpd );
	return err;	
	}

TInt CTestNpdApi::CreateListDialogL(CStifItemParser& aItem)
	{
	CNotepadApi* ptrToNpd = CNotepadApi::NewL();
	TRAPD( err, ptrToNpd->CreateListDialogL(R_NOTEPAD_TEMPLATE_LIST_DIALOG););
	delete ptrToNpd;
	return err;	
	}
TInt CTestNpdApi::CreateViewerDialogL(CStifItemParser& aItem)
	{
	CNotepadApi* ptrToNpd = CNotepadApi::NewL();
	TBool aDoWait = ETrue;
	TRAPD( err, ptrToNpd->CreateViewerDialogL( aDoWait ));
	delete ptrToNpd;
	return err ;
	}
TInt CTestNpdApi::ExecTemplatesL( CStifItemParser& aItem )
	{
	CNotepadApi* ptrToNpd = CNotepadApi::NewL();
	TInt returnValueOfDialogExecuteLD = ptrToNpd->ExecTemplatesL();
	delete ptrToNpd;
	if(returnValueOfDialogExecuteLD >= 0 )
		{
		return KErrNone;
		}
	return KErrGeneral;
	}
TInt CTestNpdApi::CreateViewer(CStifItemParser& aItem)
	{
	TFileName fileName;
	fileName.Append( KCcpFilePath );
	RFile data;
    RFs session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    User::LeaveIfError(session.ShareProtected());
    User::LeaveIfError( data.Open( session, fileName, EFileRead ) );
    CleanupClosePushL( data );
    TDesC* aTitle = NULL;
    TBool aDoWait = ETrue;
    TBool aGuessEncoding = EFalse;
    TUint aEncoding = 0;
    CNotepadApi* ptrToNpd = CNotepadApi::NewL();
    TInt err = 0;
    TRAP( err,  ptrToNpd->CreateViewerDialogL(data, aTitle, aDoWait, aGuessEncoding, aEncoding ));
	CleanupStack::PopAndDestroy( &data );  
    CleanupStack::PopAndDestroy( &session );  
    return err;
	}


TInt CTestNpdApi::ExecFileViewerUsingFilename( CStifItemParser& aItem )
	{
	TFileName fileName( KCcpFilePath ); 
	CNotepadApi* ptrToNpd = CNotepadApi::NewL();
	TDesC *title = NULL;
      TBool doWait(ETrue);
      TBool guessEncoding(EFalse);
	TUint aEncoding = KCharacterSetIdentifierIso88591;
	TInt returnValueOfDialogExecuteLD = 0;
	TInt err = 0;
	TRAP( err,  ptrToNpd->ExecFileViewerL( fileName, title, doWait, guessEncoding, aEncoding) );
	delete ptrToNpd;

	return returnValueOfDialogExecuteLD;
	}


TInt CTestNpdApi::ExecFileViewerUsingHandle( CStifItemParser& aItem )
	{
	TFileName fileName( KCcpFilePath ); 
      CNotepadApi* ptrToNpd = CNotepadApi::NewL();
      RFs fileSession;
      User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	User::LeaveIfError(fileSession.ShareProtected());
	RFile file;
	User::LeaveIfError(file.Open(fileSession, fileName, EFileRead));
	CleanupClosePushL(file);
	TDesC *title = NULL;
	TBool doWait(ETrue);
	TBool guessEncoding(EFalse);
      TUint aEncoding = KCharacterSetIdentifierIso88591;
      TInt returnValueOfDialogExecuteLD = 0;
	returnValueOfDialogExecuteLD = ptrToNpd->ExecFileViewerL( file, title, doWait, guessEncoding, aEncoding);
	delete ptrToNpd;
	CleanupStack::PopAndDestroy(2); 
	if( returnValueOfDialogExecuteLD >= 0)
	    {
	        return KErrNone;
	    }
      return KErrGeneral;
	}


TInt CTestNpdApi::ExecTextViewerL (CStifItemParser& aItem )
    {
    TDesC text( KExampleFilePath );
    CNotepadApi* ptrToNpd = CNotepadApi::NewL();
    TInt returnStatus = 0;
    TRAPD( err,ptrToNpd->ExecTextViewerL( returnStatus,text ); );
    delete ptrToNpd;
    return err;
   
    }
    
 
TInt CTestNpdApi::ExecTextEditorL(CStifItemParser& aItem )
    {
    CNotepadApi* ptrToNpd = CNotepadApi::NewL();
    TInt returnStatus = 0;
    TRAPD( err,ptrToNpd->ExecTextEditorL( returnStatus ); );
    delete ptrToNpd;
    return err;
    }
    
    


TInt CTestNpdApi::ExecReadOnlyTextViewerL(CStifItemParser& aItem )
	{
    TDesC text( KExampleFilePath );
    CNotepadApi* ptrToNpd = CNotepadApi::NewL();
    TInt returnStatus = 0;
    TRAPD( err,ptrToNpd->ExecReadOnlyTextViewerL(text ); );
    delete ptrToNpd;
    return err;	
	}
// -----------------------------------------------------------------------------
// CTestNpdApi::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CTestNpdApi::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
