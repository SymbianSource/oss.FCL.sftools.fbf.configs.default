// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#if (!defined __APPFWK_TEST_H__)
#define __APPFWK_TEST_H__

#define APPFWK_NEGATIVE_PLATSEC_START RDebug::Print(_L("*PlatSec* ERROR - NEGATIVE TEST - START"))
#define APPFWK_NEGATIVE_PLATSEC_FINISH RDebug::Print(_L("*PlatSec* ERROR - NEGATIVE TEST - FINISH"))

#define APPFWK_EXPECTED_PLATSEC_START RDebug::Print(_L("*PlatSec* ERROR - EXPECTED ERROR - START"))
#define APPFWK_EXPECTED_PLATSEC_FINISH RDebug::Print(_L("*PlatSec* ERROR - EXPECTED ERROR - FINISH"))

#endif
