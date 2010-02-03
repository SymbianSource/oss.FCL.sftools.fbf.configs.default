// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test for PDEF104113: This method is for memory leaks test when iconArray is created and deleted. 
// This method is put in a separate dll in order to avoid exporting methods 
// in server side classes (e.g., APPARC\apgrfx\CApaAppList) for test code. 
// It is called by void CT_RApaLsSessionTestStep::TestIconLoaderAndIconArrayMemoryLeaksL() 
// in apparctestserver
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include <APGAPLST.H>
#include <bautils.h>
#include <APGICNFL.H> 
#include <APFDEF.H>
#include "APGAIR.H"
#include <APGICNFL.H> 
#include "apprivate.h"
#include <barsc2.h>
#include <barsread2.h>
#include <e32uid.h>
#include "APGSTD.H"
#include "TIconLoaderAndIconArrayForLeaks.h"
#include "apsiconcaptionoverride.h"


EXPORT_C void TIconLoaderAndIconArrayForLeaks::TestIconLoaderAndIconArrayL()
	{
	RFs tempFs;
	User::LeaveIfError(tempFs.Connect());
	CleanupClosePushL(tempFs);

	CApaIconLoader* iconLoader = CApaIconLoader::NewLC(tempFs);
	CApaAppIconArray* icons = CApaAppIconArray::NewRealDefaultIconsLC(KNumberOfIconsInDefaultMbm,KDefaultAppIconMbm, *iconLoader);
	CleanupStack::PopAndDestroy(2,iconLoader);
	CleanupStack::PopAndDestroy(&tempFs);
	}

EXPORT_C void TIconLoaderAndIconArrayForLeaks::TestIconCaptionOverridesL()
	{
	CApaIconCaptionOverrides* iconcap = NULL;
	CApaIconCaptionOverridesForApp* iconCapOverride = CApaIconCaptionOverridesForApp::NewL();
	CleanupStack::PushL(iconCapOverride);
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CApaIconCaptionCenrepObserver* observer = CApaIconCaptionCenrepObserver::NewL(*iconcap);
	CleanupStack::PushL(observer);
	CleanupStack::PopAndDestroy(3,iconCapOverride);
	}
