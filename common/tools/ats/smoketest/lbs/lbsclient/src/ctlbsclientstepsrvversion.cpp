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
// @file ctlbsclientstepsrvversion.cpp
// This is the class implementation for the Server Version Tests
// 
//
 
#include "ctlbsclientstepsrvversion.h"

#include <lbs.h>

	
_LIT(Ksrvvermajor, "srv_ver_major");
_LIT(Ksrvverminor, "srv_ver_minor");
_LIT(Ksrvverbuild, "srv_ver_build");


/**
 * Destructor
 */
CT_LbsClientStep_SrvVersion::~CT_LbsClientStep_SrvVersion()
	{
	}


/**
 * Constructor
 */
CT_LbsClientStep_SrvVersion::CT_LbsClientStep_SrvVersion(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent)
	{
	SetTestStepName(KLbsClientStep_SrvVersion);
	}


/**
Static Constructor
*/
CT_LbsClientStep_SrvVersion* CT_LbsClientStep_SrvVersion::New(CT_LbsClientServer& aParent)
	{
	return new CT_LbsClientStep_SrvVersion(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}



/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_SrvVersion::doTestStepL()
	{
	// Generic test step used to test the LBS Client server open and close APIs.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_SrvVersion::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		RPositionServer server;
		TInt err = KErrNone;
		
		// Carryout common test action.
		err = server.Connect();
		if (KErrNone == err)
			{
			INFO_PRINTF1(_L("server opened"));
			
			// Carryout unique test actions.
			// Test case LBS-APIVersion-0001
			
			TVersion serverversion;
			serverversion = server.Version();
			TInt MajorVersionNumber = 0;
			TInt MinorVersionNumber = 0;
			TInt BuildVersionNumber = 0;
			if(!(GetIntFromConfig(ConfigSection(), Ksrvvermajor, MajorVersionNumber) &&
			     GetIntFromConfig(ConfigSection(), Ksrvverminor, MinorVersionNumber) &&
				 GetIntFromConfig(ConfigSection(), Ksrvverbuild, BuildVersionNumber)))
				{
				INFO_PRINTF1(_L("Bad ini file Build info."));
				User::Leave(KErrArgument);
				}
			
			//Check expected Version matches tested version
			if (MajorVersionNumber == static_cast<TInt>(serverversion.iMajor) &&
				MinorVersionNumber == static_cast<TInt>(serverversion.iMinor) &&
				BuildVersionNumber == static_cast<TInt>(serverversion.iBuild))
				{
				INFO_PRINTF1(_L("Expected Version Matches"));
				}
			else
				{
				INFO_PRINTF1(_L("Expected Version Differs"));
				SetTestStepResult(EFail);
				}
				
			}
		else
			{
			INFO_PRINTF1(_L("server failed to open"));
			SetTestStepResult(EFail);
			}
			
	
		// Carryout final common test action.			
		if (KErrNone == err)
			{
			server.Close();
			}
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_SrvVersion::doTestStepL()"));

	return TestStepResult();
	}
