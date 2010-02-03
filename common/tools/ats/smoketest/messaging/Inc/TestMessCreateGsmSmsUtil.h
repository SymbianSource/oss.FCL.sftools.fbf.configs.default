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
// This class creates the Gsm sms header and service settings. It fills up the
// fields using data read from the ini file
// 
//


#if (!defined __TEST_MESS_CREATE_GSM_SMS_UTIL_H__)
#define __TEST_MESS_CREATE_GSM_SMS_UTIL_H__

#include "TestMessCreateSmsUtilBase.h"

/**
 This class creates the Gsm sms header and the GSM SMS service settings. 
 The data is read from the ini file.
*/
class CTestMessCreateGsmSmsUtil : public CTestMessCreateSmsUtilBase
	{
public:
	//Constructor
	CTestMessCreateGsmSmsUtil(CTestMessBase &aTestStep);

	/**
	 Prepare sms account details
	*/
	virtual void		PrepareAccountL(CMsvSession& aSession);

	/**
	 Creates the new header object
	*/
	virtual CSmsHeader*	NewHeaderL(CRichText &aBodyText);

	/**
	 Fills up the fields of the sms header object
	*/
	virtual void 		PrepareHeaderL(CSmsHeader &aSmsHeader);

	/**
	 Fills up the fields of the sms account settings 
	*/
	virtual void 		SetSmsAccountSettings(CSmsSettings &aSmsSettings);
	};

#endif /* __TEST_MESS_CREATE_GSM_SMS_UTIL_H__ */
