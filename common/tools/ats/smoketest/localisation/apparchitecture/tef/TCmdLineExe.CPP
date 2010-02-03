// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// TCmdLineExe.CPP - to test commandline APIS
// This exe is called in testCmdLinesAPIsL in T_CmdlnStep.CPP
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#include <e32std.h>
#include <e32uid.h>
#include <e32base.h>
#include <e32test.h>
#include  <apacmdln.h>
#include "T_CmdlnStep.h"


// Global functions

/**  This function gets the file handle from the commandline object. It returns KErrNone when successful in reading
     a file else it returns a system-wide error
*/

TInt DoReadFileL()
	{
	RFile file;
	TBuf8<8> fileRead;
    CApaCommandLine* cmdLine;
    TInt ret(0);
	ret = CApaCommandLine::GetCommandLineFromProcessEnvironment(cmdLine);
	User::LeaveIfError(ret);
	CleanupStack::PushL(cmdLine);

	cmdLine->GetFileByHandleL(file);
    ret = file.Read(fileRead);
	file.Close();
    CleanupStack::PopAndDestroy(cmdLine); 
	return ret;
	}

GLDEF_C TInt E32Main()
	{


	__UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
   
   	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
    TInt val(0);
    
    TRAPD(err,val= DoReadFileL());
    delete(cleanup);
	__UHEAP_MARKEND;
    
    if((err==KErrNone) && (val ==KErrNone ))
    	{
    		return KFileHandleTestPassed;
    	}
    else
    	{
    		return KErrGeneral;
    	}

	}
	
