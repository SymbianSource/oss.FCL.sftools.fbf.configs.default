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
// This class is the base class for create CDMA or GSM SMS utility class
// 
//

#if (!defined __TEST_MESS_CREATE_SMS_UTIL_BASE_H__)
#define __TEST_MESS_CREATE_SMS_UTIL_BASE_H__

#include "TestMessBase.h"

// EPOC includes
#include <mtmuids.h>
#include <txtrich.h>

#include <smut.h>

/*@{*/
_LIT(KBearer,				"bearer");
_LIT(KEncoding,				"encoding");
_LIT(KDef,					"default");
_LIT(KDefBearer,			"smsbearer");
_LIT(KDefEncoding,			"smsencoding");
_LIT(KDefDeliveryReport,	"smsDeliveryReport");
_LIT(KDeliveryReport,		"DeliveryReport");
_LIT(KRejectDuplicate,		"SmsRejectDuplicate");
_LIT(KReplyPath,			"SmsReplyPath");
_LIT(KConCatenate,			"SmsConCatenate");

_LIT(KBearerWap,			"wap");
_LIT(KBearerWapSecure,		"wapsecure");
/*@}*/

/**
 This class is the base class for create GSM or CDMA SMS utility classes
*/
class CTestMessCreateSmsUtilBase : public CBase
	{
public:
	//Constructor
	CTestMessCreateSmsUtilBase(CTestMessBase &aTestStep);

	/**
	 Prepare sms account details
	*/
	virtual void		PrepareAccountL(CMsvSession& aSession) = 0;

	/**
	 Creates the new header object
	*/
	virtual CSmsHeader*	NewHeaderL(CRichText &aBodyText) = 0;

	/**
	 Fills up the fields of the sms header object
	*/
	virtual void 		PrepareHeaderL(CSmsHeader &aSmsHeader) =0;

	/**
	 Fills up the fields of the sms account settings 
	*/
	virtual void 		SetSmsAccountSettings(CSmsSettings &aSmsSettings) =0;

protected:
	/**
	 Reference to the test step which creates the utility class
	*/
	CTestMessBase&		iTestStep;
	};

#endif /* __TEST_MESS_CREATE_SMS_UTIL_BASE_H__ */
