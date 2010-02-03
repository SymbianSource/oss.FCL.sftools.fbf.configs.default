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
// @file CT_LbsClientStep_SrvVersion.h
// This contains the header file for Server Version Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_SRV_VERSION_H__
#define __CT_LBS_CLIENT_STEP_SRV_VERSION_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"

// Literals used
_LIT(KLbsClientStep_SrvVersion, "LbsClientStep_SrvVersion");

class CT_LbsClientStep_SrvVersion : public CT_LbsClientStep
	{
public:
	~CT_LbsClientStep_SrvVersion();

	static CT_LbsClientStep_SrvVersion* NewL(CT_LbsClientServer& aParent);
	static CT_LbsClientStep_SrvVersion* New(CT_LbsClientServer& aParent);

	virtual TVerdict doTestStepL();

private:
	CT_LbsClientStep_SrvVersion(CT_LbsClientServer& aParent);

	};

#endif //__CT_LBS_CLIENT_STEP_SRV_VERSION_H__
