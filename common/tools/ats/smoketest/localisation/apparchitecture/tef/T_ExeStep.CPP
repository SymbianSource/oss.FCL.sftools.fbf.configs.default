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
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#include <e32std.h>
#include <e32uid.h>
#include <e32base.h>
#include <e32test.h>
#include <apadef.h>
#include "T_ExeStep.h"

/**
  Auxiliary Fn for Test Case ID T-ExeStep-doTestStepL
 
  This function accepts the data passed to the thread function and prints it.
  
*/
void CT_ExeStep::DoThings(const TApaCommandLine& aCommandLine)
	{
	INFO_PRINTF1(_L("The test exe has been loaded."));
	INFO_PRINTF1(_L("\n"));
	INFO_PRINTF1(aCommandLine);
	INFO_PRINTF1(_L("\n"));
	}

CT_ExeStep::~CT_ExeStep()
/**
   Destructor
 */
	{
	}

CT_ExeStep::CT_ExeStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_ExeStep);
	}

TVerdict CT_ExeStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_ExeStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}
	
	
/**
   @SYMTestCaseID T-ExeStep-doTestStepL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test retrieving data from main thread of a process. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The test creates a new process by calling the constructor of
   RProcess. Call RProcess::CommandLine() to get a copy of data which is
   passed to the thread function of the newly created process's main thread.
   Pass the data thus copied to method DoThings() to print the information.\n
   API Calls:\n	
   RProcess::CommandLine(TDes& aCommand) const
   
   @SYMTestExpectedResults Test should create a new process and copy data from
   main thread of newly created process.
    
 */
TVerdict CT_ExeStep::doTestStepL()
{
	__UHEAP_MARK;
    TApaCommandLine commandline;
	User::CommandLine(commandline);
    DoThings(commandline);
	__UHEAP_MARKEND;

	return TestStepResult();
}
