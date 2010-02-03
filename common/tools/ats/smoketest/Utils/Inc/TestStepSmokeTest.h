// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestStepSmokeTest
// 
//

#if (!defined __TEST_STEP_SMOKETEST_H__)
#define __TEST_STEP_SMOKETEST_H__

//TEF include
#include <test/testexecutestepbase.h>
#include "Configuration.cfg"

enum TTestLoggingDetail
	{
	ETestLoggingDetail_Min,
	ETestLoggingDetail_Normal,
	ETestLoggingDetail_Max,
	};

#if (defined __VERSION_70S__)
enum TCapability
	{
	ECapabilityTCB				= 0,
	};
#endif

/**
 This class is the base class for all the test steps in smoketest.
*/
class CTestStepSmokeTest : public CTestStep
	{
public:
	//Destructor
	IMPORT_C virtual ~CTestStepSmokeTest();

	/**
	 Pure virtual, to be implemented by the derived test steps.
	 It implements the required functionality of the test step.
	 @return TVerdict - result of the test step
	 @leave - System wide error codes
	*/
	IMPORT_C virtual enum TVerdict	doTestStepL() = 0;

	//Implements the preoperational functionalities for a test step
	IMPORT_C virtual enum TVerdict	doTestStepPreambleL();

	//Implements the post operational fucntionalities for a test step
	IMPORT_C virtual enum TVerdict	doTestStepPostambleL();

	TTestLoggingDetail		LoggingDetail() const { return iLoggingDetail; }
	TInt					Repetition() const { return iRepetition; }

protected:
	//Constructor
	IMPORT_C CTestStepSmokeTest();

	//Gets the TCapability value for the capabilty string literal
	IMPORT_C TInt			GetCapability(TPtrC aCapability, TCapability& aCapabilityValue);

	//Verifies the given time is nearly eaual to current UTC time
	IMPORT_C TBool			VerifyUTCTime(TTime aTime);

private:
	TTestLoggingDetail			iLoggingDetail;
	TInt						iRepetition;
	};

#endif /* __TEST_STEP_SMOKETEST_H__ */
