// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestMessCreateSMS. This class creates the sms message 
// for GSM or CDMA. For CDMA mtm, it uses utility classes to create 
// the respective GSM or CDMA sms message
// 
//

#if (!defined __TEST_MESS_CREATE_SMS_H__)
#define __TEST_MESS_CREATE_SMS_H__

//Epoc include
#include <SmokeTestActive.h>
#include <sendas2.h>
#include <csendasaccounts.h>
#include <csendasmessageTypes.h>

#include "TestMessBase.h"
#include "TestMessCreateSmsUtilBase.h"

/**
 This class creates the sms message for GSM or CDMA. For CDMA mtm, it 
 uses utility classes to create the respective GSM or CDMA sms message
*/
class CTestMessCreateSMS : public CTestMessBase
,	private MTestActiveCallback
	{
public:
	CTestMessCreateSMS();

	virtual	TVerdict	doTestStepPreambleL();
	virtual	TVerdict	doTestStepL();

protected:
	virtual void		AddMtmCapabilitiesL(RSendAs& aSendAs);
	virtual void		PrepareSendAsL(RSendAsMessage& aSendAs);
	virtual void		PrepareHeaderL(CTestMessCreateSmsUtilBase& aCreateUtil, CSmsHeader& aSmsHeader);
	void				ConCatMessageTextL(CSmsHeader& aHeader);
	void				CreateSmsL(const TMsvId aMsvId, TTimeIntervalMinutes aScheduledTime);

private:
	virtual void	RunL() { CActiveScheduler::Stop(); }
	/**
	* Method from which CTestActive informs the user DoCancel call
	*/
	virtual void	DoCancel() { }

private:
	TBool							iSimInDatabase;
	RMobilePhone::TMobileAddress	iSimPhoneNumber;
	};

#endif /* __TEST_MESS_CREATE_SMS_H__ */
