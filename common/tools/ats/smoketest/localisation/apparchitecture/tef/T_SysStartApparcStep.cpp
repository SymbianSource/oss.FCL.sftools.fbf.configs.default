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

#include <s32file.h>

#include "T_SysStartApparcStep.h"
#include "tssaac\tssaac.h"

_LIT(KApparcPanicFile,"c:\\T_SSAAC_PanicResultFile.txt");


const TChar KDelimChar = '=';

/**
  The expected results are hard coded from the test specification.
*/
//Test case 1
const TInt KExpectedStartApp1LRes1 = KErrNone;
const TInt KExpectedStartApp1LRes2 = KErrNotFound;

//Test case 99
const TInt KExpectedLocalisedCaptionLRes1 = 0;
const TInt KExpectedLocalisedCaptionLRes2 = 0;

/**
  Read the results of the individual tests. The test number is determined by
  reading the configuration file z:\\apparctest\\TestCaseConfig.txt and then one
  of two functions is called, one which handles tests which panic and one which
  handles tests which don't.

  Non-paniccing test results are read from file c:\\SysStartApparc_TestStartApp1L.txt
  The order in which these results are written in CSysStartApparcTestCase::WriteResultFileL()
  MUST match the order in which they are read and tested in CheckApparcResultsL()
  The expected results are hard coded from the test specification.(see above)

  Paniccing test results are read from file c:\\T_SSAAC_PanicResultFile.txt
  The result is a '1 or 0' to represent a PASS or a FAIL The actual pass critria
  are found in file sysstart_apparc_checkEpocWind.bat where the test is performed and
  the T_SSAAC_PanicResultFile file created with the appropriate result

*/

/**
  Read the results of the individual tests that do not panic. Currently, there is
  only one test that does not panic. Results are read in from file SysStartApparc_TestStartApp1L.txt
  and compared with the expected results

*/
void CT_SysStartApparcStep::CheckApparcResultsL()
	{
	RFs	fs;
 	User::LeaveIfError(fs.Connect());
 	CleanupClosePushL(fs);

	RFileReadStream reader;
	User::LeaveIfError(reader.Open(fs, KSsaacResultFile, EFileRead));
	CleanupClosePushL(reader);

	TBuf8<64> myDes;
	TInt ret = 0;

	// Read 16 bits - StartApp1LRes1
	reader.ReadL(myDes, KDelimChar);
	ret = reader.ReadInt16L();

	if(iTestcase == 1)
		{
		INFO_PRINTF3(_L("Test StartApp1LRes1 Count - Expected: %d, Actual: %d"), KExpectedStartApp1LRes1, ret);
		TEST(ret==KExpectedStartApp1LRes1);
		}
	else
		{ // test case is 99
		INFO_PRINTF3(_L("Test LocalisedCaptionLRes1 Count - Expected: %d, Actual: %d"), KExpectedLocalisedCaptionLRes1, ret);
		TEST(ret==KExpectedLocalisedCaptionLRes1);
		}

	// Read 16 bits - StartApp1LRes2
	reader.ReadL(myDes, KDelimChar);
	ret = reader.ReadInt16L();

	if(iTestcase == 1)
		{
		INFO_PRINTF3(_L("Test StartApp1LRes2 Count - Expected: %d, Actual: %d"), KExpectedStartApp1LRes2, ret);
		TEST(ret==KExpectedStartApp1LRes2);
		}
	else // test cass is 99
		{
		INFO_PRINTF3(_L("Test LocalisedCaptionLRes2 Count - Expected: %d, Actual: %d"), KExpectedLocalisedCaptionLRes2, ret);
		TEST(ret==KExpectedLocalisedCaptionLRes2);
		}

	// clean-up
	CleanupStack::PopAndDestroy(2, &fs);
	}

/**
  Read the results of the individual tests that panic. After a panic, the
  batch file sysstartapparc_checkEpocWind.bat searched the EpocWind.out file
  for lines of text that verify that a panic has occurred with the correct
  KERN-EXEC number and then writes the result to a text file (pass=1, fail=0)
  for processing here.

*/
void CT_SysStartApparcStep::CheckApparcPanicResultsL()
	{
	RFs	fs;
 	User::LeaveIfError(fs.Connect());
	INFO_PRINTF1(_L("File server connected"));

 	CleanupClosePushL(fs);

	RFileReadStream reader;
	User::LeaveIfError(reader.Open(fs, KApparcPanicFile, EFileRead));

	CleanupClosePushL(reader);

	TBuf8<64> myDes;

	TInt expected = 1; //a PASS

	//File containd a test name and a test result, Move past the test name
	reader.ReadL(myDes, KDelimChar);

	//Read the test result
	reader.ReadL(myDes, KDelimChar);

	// Read 16 bits - StartApp1LRes1
	TChar result(reader.ReadUint8L());
	TInt ret = result.GetNumericValue();
	INFO_PRINTF3(_L("Test StartApp1LRes1 Count - Expected: %d, Actual: %d"), KTestResultPass, ret);
	TEST(ret==expected);

	// clean-up
	CleanupStack::PopAndDestroy(2, &fs);
	}


/**
  Destructor
*/
CT_SysStartApparcStep::~CT_SysStartApparcStep()
	{
	}

/**
  Constructor
*/
CT_SysStartApparcStep::CT_SysStartApparcStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_SysStartApparcStep);
	}

/**
  Override of base class virtual.
  @return - TVerdict code
*/

TVerdict CT_SysStartApparcStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Test T_SysStartApparcStep: Started"));

 	__UHEAP_MARK;
 	TInt ret=KErrGeneral;
 	iTestcase=ReadConfigFileL();

 	//Test case 1 and 99 are the only tests that do not panic
 	if( (iTestcase==1) || (iTestcase==29) )
 		{
		TRAP(ret, CheckApparcResultsL());
		}
	else
		{
		TRAP(ret, CheckApparcPanicResultsL());
		}
	INFO_PRINTF2(_L("CheckApparcResultsL ends with code %d"), ret);
	TEST(ret==KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("Test T_SysStartApparcStep: Finished"));
	return TestStepResult();
	}

/**
Reads the input config file and returns the test case number
Defined in sysstart_apparc_run.bat
@return TInt - the test case number
*/
TInt CT_SysStartApparcStep::ReadConfigFileL()
	{
	RFs	fs;
 	User::LeaveIfError(fs.Connect());
 	CleanupClosePushL(fs);

 	RFileReadStream reader;
	User::LeaveIfError(reader.Open(fs, KApparcConfigFile, EFileRead));
	CleanupClosePushL(reader);

	TBuf8<255> aDes;
	reader.ReadL(aDes, KDelimChar);

	//Read in a two character representation of a number and convert to an integer
	TChar result1(reader.ReadInt8L());
	TChar result0(reader.ReadInt8L());
	TInt aTestCase = result1.GetNumericValue()*10 + result0.GetNumericValue();

	// clean-up
	CleanupStack::PopAndDestroy(2, &fs);

	return aTestCase;

	}
