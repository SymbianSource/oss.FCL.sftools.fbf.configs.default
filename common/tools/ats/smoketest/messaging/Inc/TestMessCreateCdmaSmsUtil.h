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
// This class creates a Cdma sms header and service settings. It sets the fields by reading
// from the ini file
// 
//
 

#if (!defined __TEST_MESS_CREATE_CDMA_SMS_UTIL_H__)
#define __TEST_MESS_CREATE_CDMA_SMS_UTIL_H__

#include "TestMessCreateSmsUtilBase.h"

/**
 This class creates a Cdma sms header and service settings.It sets the fields by reading
 from the ini file
*/
class CTestMessCreateCdmaSmsUtil : public CTestMessCreateSmsUtilBase
{
public:
	//Constructor
	CTestMessCreateCdmaSmsUtil(CTestMessBase &aTestStep);

	//Creates a new header
	CSmsHeader* NewHeaderL(CRichText &aBodyText);

	//Sets the fields of the header
	void PrepareHeaderL(CSmsHeader &aSmsHeader);

	//Sets the account settings.
	void SetSmsAccountSettings(CSmsSettings &aSmsSettings);

private:
	//Gets the message type of the SMS
	TSmsMessageType GetMessageTypeId(TPtrC aMessageType);
	
	//Gets the tele service ID
	tia637::TTeleserviceId GetTeleServiceId(TPtrC aTeleService);

	//Gets the message conversion ID
	TSmsPIDConversion GetMessageConversionId(TPtrC aMessageConversion);
	
};

#endif /* __TEST_MESS_CREATE_CDMA_SMS_UTIL_H__ */
