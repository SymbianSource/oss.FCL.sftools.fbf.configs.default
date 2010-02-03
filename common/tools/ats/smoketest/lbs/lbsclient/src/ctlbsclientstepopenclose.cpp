// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsclientstep_openclose.cpp
// This is the class implementation for the Server Open Close Tests
// 
//
 
#include "ctlbsclientstepopenclose.h"

#include <lbs.h>


/**
 * Destructor
 */
CT_LbsClientStep_OpenClose::~CT_LbsClientStep_OpenClose()
	{
	}


/**
 * Constructor
 */
CT_LbsClientStep_OpenClose::CT_LbsClientStep_OpenClose(CT_LbsClientServer& aParent) :  CT_LbsClientStep(aParent)
	{
	SetTestStepName(KLbsClientStep_OpenClose);
	}


/**
Static Constructor
*/
CT_LbsClientStep_OpenClose* CT_LbsClientStep_OpenClose::New(CT_LbsClientServer& aParent)
	{
	return new CT_LbsClientStep_OpenClose(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_OpenClose::doTestStepL()
	{
	// Generic test step used to test the LBS Client server open and close APIs.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_OpenClose::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		
		TUint howManyModules = 0;
		TPositionModuleInfo myModuleInfo;
		TInt err = KErrNone;
		TPositionCriteria criteria;
		
		
		// Carryout common test action.
		err = iServer.Connect();	// Function under test.
		if (KErrNone == err)
			{
			INFO_PRINTF1(_L("server opened"));
			
						
			// Carryout unquie test actions.
			TInt testCaseId;
			if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
				{
					switch (testCaseId)
					{
						// Test case LBS-OpenClose-0001
						case 1:
							{	
							TPositionModuleId serverDefaultModuleId;
							User::LeaveIfError(iServer.GetDefaultModuleId(serverDefaultModuleId));
							err = iPositioner.Open(iServer, serverDefaultModuleId);
							if(err != KErrNone)
								{
								INFO_PRINTF1(_L("** SubSession Failed to open **"));
								SetTestStepResult(EFail);
								}
							}
							break;
					
						// Test case LBS-OpenClose-0002
						case 2:
							{
							User::LeaveIfError(iServer.GetNumModules(howManyModules));  
							User::LeaveIfError(iServer.GetModuleInfoByIndex(howManyModules-1, myModuleInfo));
							err = iPositioner.Open(iServer, myModuleInfo.ModuleId());
							if(err != KErrNone)
								{
								INFO_PRINTF1(_L("** SubSession Failed to open **"));
								SetTestStepResult(EFail);	
								}
							}
							break;
					
						// Test case LBS-OpenClose-0003
						case 3:
							{
							// PassInNullUID
							err = iPositioner.Open(iServer, KPositionNullModuleId );
							if (KErrNotFound == err)
								{
								INFO_PRINTF1(_L("** SubSession failed to open: Negative Test Successful **"));
								}
								else
								{
								INFO_PRINTF1(_L("** SubSession Opened, test failed **"));
								SetTestStepResult(EFail);
								}
							}
							break;
							
						// Test case LBS-OpenCloseExtend-0001
						case 11:
							{
							criteria.AddRequiredCapabilities(TPositionModuleInfo::ECapabilitySatellite);
							err = iPositioner.Open(iServer, criteria);
							if(err != KErrNone)
								{
								INFO_PRINTF1(_L("** SubSession Failed to open **"));
								SetTestStepResult(EFail);	
								}
							}
							break;
							
						// Test case LBS-OpenCloseExtend-0002
						case 12:
							{
						    // Set capabilities not supported by the module:
							criteria.AddRequiredCapabilities(TPositionModuleInfo::ECapabilityAddress);
							err = iPositioner.Open(iServer, criteria);
							if (KErrNotFound == err)
								{
								INFO_PRINTF1(_L("** SubSession failed to open: Negative Test Successful **"));
								}
							else
								{
								INFO_PRINTF1(_L("** SubSession Opened, test failed **"));
								SetTestStepResult(EFail);
								}
							}
							break;
							
						// Test case LBS-OpenCloseMultiple-0001
						case 20:
							{
							User::LeaveIfError(iPositioner.Open(iServer));
							RPositioner positioner2;
							err = positioner2.Open(iServer);
							if(err != KErrNone)
								{
								INFO_PRINTF1(_L("** SubSession Failed to open **"));
								SetTestStepResult(EFail);	
								}
							positioner2.Close();
							}
							break;

						// Test case LBS-ReOpen-0001
						case 30:
							{
							User::LeaveIfError(iPositioner.Open(iServer));
							User::LeaveIfError(iPositioner.Open(iServer));
							}
							break;
						// Test case LBS-OpenClose-0041
						case 41:
							{	
							err = iServer.CancelRequest(EPositionerNotifyPositionUpdate);
							if(err != KErrNotFound)
								{
								INFO_PRINTF2(_L("** Failed to return KErrnotFound, instead returned %d**"),err);
								SetTestStepResult(EFail);
								}
							}
							break;
							
						// Test case LBS-OpenClose-0042
						case 42:
							{
							err = iPositioner.Open(iServer);
							if(err != KErrNone)
								{
								INFO_PRINTF1(_L("** SubSession Failed to open **"));
								SetTestStepResult(EFail);
								}
							
							err = 0; // reset Err
								
							err = iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
							if(err != KErrNotFound)
								{
								INFO_PRINTF2(_L("** Failed to return KErrnotFound, instead returned %d**"),err);
								SetTestStepResult(EFail);
								}
							}
							break;

						// Test case LBS-OpenClose-0043
						case 43:
							{
							err = iPositioner.Open(iServer);
							if(err != KErrNone)
								{
								INFO_PRINTF1(_L("** SubSession Failed to open **"));
								SetTestStepResult(EFail);
								}
							
							err = 0; // reset Err
								
							err = iPositioner.CancelRequest(EPositionerGetLastKnownPosition);
							if(err != KErrNotFound)
								{
								INFO_PRINTF2(_L("** Failed to return KErrnotFound, instead returned %d**"),err);
								SetTestStepResult(EFail);
								}
							}
							break;
							
						// Test case LBS-OpenClose-0044
						case 44:
							{	
							err = iServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
							if(err != KErrNotFound)
								{
								INFO_PRINTF2(_L("** Failed to return KErrnotFound, instead returned %d**"),err);
								SetTestStepResult(EFail);
								}
							}
							break;
					
					default:
						User::Leave(KErrArgument);
						break;
					}
				
				iPositioner.Close();
				iServer.Close();	
				}
			else
				{ 
				INFO_PRINTF1(_L("Bad Ini file"));
				SetTestStepResult(EFail);
				}
			} 
		else
			{
			INFO_PRINTF1(_L("SubSession failed to open"));
			SetTestStepResult(EFail);
			}

		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_OpenClose::doTestStepL()"));

	return TestStepResult();
	}
