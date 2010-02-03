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

//user include
#include "TestStepSmokeTest.h"

//Epoc include
#include <testconfigfileparser.h>
#include <f32file.h>
#include <e32std.h>

/*@{*/
_LIT(KExpectedCapCheckResult,	"ExpectedCapCheckResult");
_LIT(KConfigFile,				"C:\\plattest\\platsec\\plattest_capabilities.config");
_LIT(KDefault,					"default");
_LIT(KCapTest,					"CapabilityTest");
_LIT(KNo,						"No");

_LIT(KLogging,					"logging");
_LIT(KLoggingMin,				"min");
_LIT(KLoggingNormal,			"normal");
_LIT(KLoggingMax,				"max");

_LIT(KRepetition,				"repetition");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 Constructor
*/
EXPORT_C CTestStepSmokeTest::CTestStepSmokeTest()
:	CTestStep()
,	iLoggingDetail(ETestLoggingDetail_Normal)
,	iRepetition(1)
	{
	}

/**
 Destructor
*/
EXPORT_C CTestStepSmokeTest::~CTestStepSmokeTest()
	{
	}

	
/**
 The overriden implementation of CTestStep::doTestStepPreambleL()
 @return TVerdict- result of the test step
 @leave - system wide error codes
*/
EXPORT_C TVerdict CTestStepSmokeTest::doTestStepPreambleL()
	{
	TVerdict ret = CTestStep::doTestStepPreambleL();

	//	Get logging
	TPtrC	logging;
	if ( GetStringFromConfig(ConfigSection(), KLogging, logging) )
		{
		INFO_PRINTF2(_L("Logging value %S"), &logging);
		if ( logging.Compare(KLoggingMin) == 0 )
			{
			iLoggingDetail=ETestLoggingDetail_Min;
			}
		else if ( logging.Compare(KLoggingNormal) == 0 )
			{
			iLoggingDetail=ETestLoggingDetail_Normal;
			}
		else if ( logging.Compare(KLoggingMax) == 0 )
			{
			iLoggingDetail=ETestLoggingDetail_Max;
			}
		else
			{
			ERR_PRINTF1(_L("Illegal logging value, use min, normal or max."));
			ret=EFail;
			}
		}

	//	Get repetition count of test
	GetIntFromConfig(ConfigSection(), KRepetition, iRepetition);
	INFO_PRINTF2(_L("Repetition %d"), iRepetition);

	return ret;
	}

/**
 The overriden implementation of CTestStep::doTestStepPostambleL()
 It implements the functionality of capability checking if
 _PLATTEST_CAPTEST_ is ON.
 @return TVerdict- result of the test step
 @leave - system wide error codes
*/

EXPORT_C TVerdict CTestStepSmokeTest::doTestStepPostambleL()
	{

	//Read whether to do capability testing..
	TPtrC captest(KNo);
	GetStringFromConfig(KDefault, KCapTest, captest);
	if( captest.Compare(_L("YES")) == 0)
		{
		INFO_PRINTF1(_L("---Capability Testing---"));
		//Gets the capabilities config file
		TBuf<KMaxTestExecuteCommandLength> scriptFile;
		scriptFile.Copy(KConfigFile);
		INFO_PRINTF2(_L("The config file %S"), &scriptFile);
	
		TBuf8<KMaxTestExecuteCommandLength> scriptSection;
		scriptSection.Copy(_L("Capabilities"));
			
		TPtrC serverName = GetServerName();
		INFO_PRINTF2(_L("The server name is  %S"), &serverName);
		HBufC8 *scriptItem = HBufC8::NewLC(serverName.Length());
		scriptItem->Des().Copy(serverName);
		 
		//Get the file server session object
		RFs		rFs;
		TInt	err=rFs.Connect();
		if(err != KErrNone)
			{
			ERR_PRINTF1(_L("Failed to connect with File server"));
			SetTestStepResult(EFail);
			}
		else
			{
			CleanupClosePushL(rFs);
			//Get the config file
			CTestConfig* configFile = CTestConfig::NewLC(rFs, KNullDesC, scriptFile);
			if( configFile == NULL)
				{
				WARN_PRINTF1(_L("NO Config file found -- Plattest_capabilites.config"));
				}
			else
				{
				//Get the item value from config file
				TBuf8<KMaxTestExecuteCommandLength> itemValue;
				itemValue = configFile->Section(scriptSection)->Item(scriptItem->Des())->Value();
				TLex8 capabilityParser(itemValue);
				TBool capCheckResult= ETrue;
				
				while(!capabilityParser.Eos())
					{
					//Parse the capabilities
					TPtrC8 capability(capabilityParser.NextToken());
								
					HBufC* capabilityRead = HBufC::NewLC(capability.Length());
					capabilityRead->Des().Copy(capability);
					INFO_PRINTF2(_L("Capability Read is %S"), capabilityRead);
					
					//Get the capability value
					TCapability capabilityValue;
					TPtrC ptrCapabilty(capabilityRead->Des());
					TInt err = GetCapability(ptrCapabilty, capabilityValue);
					if( err != KErrNone)
						{
						WARN_PRINTF1(_L("The capability is not found"));
						}
					else
						{
						//Check if the current process posses the required capability
						if( !RProcess().HasCapability(capabilityValue) )
							{
							INFO_PRINTF2(_L("The capability %S is not possesed."), capabilityRead);
							capCheckResult=EFalse;
							}
						else
							{
							INFO_PRINTF2(_L("The capability %S is possesed."), capabilityRead);
							}
						}
					CleanupStack::PopAndDestroy(capabilityRead);
					}
					
				//read the expected capability check result
				TBool expectedCapCheckResult = ETrue;
				GetBoolFromConfig(ConfigSection(), KExpectedCapCheckResult, expectedCapCheckResult);
				INFO_PRINTF2(_L("The expected cap check result is %D"), expectedCapCheckResult);
				INFO_PRINTF2(_L("The cap check result is %D"), capCheckResult);

				if(expectedCapCheckResult == capCheckResult)
					{
					SetTestStepResult(EPass);
					}
				else
					{
					SetTestStepResult(EFail);
					}
				}
			CleanupStack::PopAndDestroy(2, &rFs);
			}
		CleanupStack::PopAndDestroy(scriptItem);
		if(TestStepError() == KErrPermissionDenied)
			{
			INFO_PRINTF1(_L(" The failure is due to Platform Security"));
			}
		}

	TVerdict ret = CTestStep::doTestStepPostambleL();
	return ret;
	}

/**
 Get the capability id (enum value) for the capability name 
 given as string.
 @param aCapability - Capability name 
 @param aCapabilityValue - value for the capability name
 @return TInt - error codes
 @leave - None
*/
EXPORT_C TInt CTestStepSmokeTest::GetCapability(TPtrC aCapability, TCapability& aCapabilityValue)
	{
	INFO_PRINTF2(_L("The capability get is %S"), &aCapability);
	TInt	ret=KErrNone;

	TBuf<KMaxTestExecuteCommandLength>	capCaseValue(aCapability);
	capCaseValue.LowerCase();

	if(!capCaseValue.Compare(_L("tcb" )))
		{
		aCapabilityValue=ECapabilityTCB;
		}
	else if(!capCaseValue.Compare(_L("commdd" )))
		{
		aCapabilityValue=ECapabilityCommDD;
		}
	else if(!capCaseValue.Compare(_L("powermgmt" )))
		{
		aCapabilityValue=ECapabilityPowerMgmt;
		}
	else if(!capCaseValue.Compare(_L("multimediadd" )))
		{
		aCapabilityValue=ECapabilityMultimediaDD;
		}
	else if(!capCaseValue.Compare(_L("readdevicedata" )))
		{
		aCapabilityValue=ECapabilityReadDeviceData;
		}
	else if(!capCaseValue.Compare(_L("writedevicedata" )))
		{
		aCapabilityValue=ECapabilityWriteDeviceData;
		}
	else if(!capCaseValue.Compare(_L("drm" )))
		{
		aCapabilityValue=ECapabilityDRM;
		}
	else if(!capCaseValue.Compare(_L("trustedui" )))
		{
		aCapabilityValue=ECapabilityTrustedUI;
		}
	else if(!capCaseValue.Compare(_L("protserv" )))
		{
		aCapabilityValue=ECapabilityProtServ;
		}
	else if(!capCaseValue.Compare(_L("diskadmin" )))
		{
		aCapabilityValue=ECapabilityDiskAdmin;
		}
	else if(!capCaseValue.Compare(_L("networkcontrol" )))
		{
		aCapabilityValue=ECapabilityNetworkControl;
		}
	else if(!capCaseValue.Compare(_L("allfiles" )))
		{
		aCapabilityValue=ECapabilityAllFiles;
		}
	else if(!capCaseValue.Compare(_L("swevent" )))
		{
		aCapabilityValue=ECapabilitySwEvent;
		}
	else if(!capCaseValue.Compare(_L("networkservices" )))
		{
		aCapabilityValue=ECapabilityNetworkServices;
		}
	else if(!capCaseValue.Compare(_L("localservices" )))
		{
		aCapabilityValue=ECapabilityLocalServices;
		}
	else if(!capCaseValue.Compare(_L("readuserdata" )))
		{
		aCapabilityValue=ECapabilityReadUserData;
		}
	else if(!capCaseValue.Compare(_L("writeuserdata")))
		{
		aCapabilityValue=ECapabilityWriteUserData;
		}
	else if(!capCaseValue.Compare(_L("location")))
		{
		aCapabilityValue=ECapabilityLocation;
		}
	else if(!capCaseValue.Compare(_L("none")))
		{
		aCapabilityValue=ECapability_None;
		}
	else
		{
		INFO_PRINTF2(_L("Unrecognised capability %S, will be rejected"), &capCaseValue);
		ret=KErrNotFound;
		}

	return ret;
	}


/**
 Verifies the given time is nearly equal to current UTC time.
 @param aTime - Time needs to be verified 
 @return TBool - Returns ETrue if aTime is nearly equal to current UTC 
 time, otherwise EFalse
 @leave - None
*/
EXPORT_C TBool CTestStepSmokeTest::VerifyUTCTime(TTime aTime)
	{
	TTime homeTime;
	TTimeIntervalSeconds utcOffset;

	homeTime.HomeTime();
	homeTime.SecondsFrom(aTime, utcOffset);
	INFO_PRINTF2(_L("Calculated utcOffset: %D"), utcOffset.Int());
	if(Abs<TInt>(utcOffset.Int()) <= (User::UTCOffset().Int()+120) 
		&& Abs<TInt>(utcOffset.Int()) >= (User::UTCOffset().Int()-120))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
