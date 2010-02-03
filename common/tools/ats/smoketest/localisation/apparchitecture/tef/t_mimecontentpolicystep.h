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
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __T_MIMECONTENTPOLICY_STEP_H__)
#define __T_MIMECONTENTPOLICY_STEP_H__

#include "testableapalssession.h"
#include "appfwk_test_AppUi.h"
#include <ApfMimeContentPolicy.h>

_LIT(KT_MimeContentPolicyStep,"t_mimecontentpolicy");

class CT_MimeContentPolicyStep : public CTestStep
	{
public:
	CT_MimeContentPolicyStep();
	~CT_MimeContentPolicyStep();
	virtual TVerdict doTestStepL();
	
private:
	void RunTestCasesL();
    void CCPTestIsClosedType();
    void CCPTestIsClosedExtension();
    void CCPTestIsDRMEnvelopeFileNameL();
    void CCPTestIsClosedFileFileNameL();
    void CCPTestIsDRMEnvelopeFileHandleL();
    void CCPTestIsClosedFileFileHandleL();
    // aIsDRMEnvelope is ETrue for DRM Envelope and EFalse for ClosedFile
    TBool DoCCPTestUsingFileHandleL(const TDesC &aName, TBool aIsDRMEnvelope);
    void CCPOOMTestL();

private:
	CApfMimeContentPolicy* iCcp;
	RTestableApaLsSession iApaLsSession;
	RFs iFs;
	};

#endif
