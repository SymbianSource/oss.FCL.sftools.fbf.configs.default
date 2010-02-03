// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestMessEditEntryUtil. A utility to edit an entry
// 
//

#include "Configuration.cfg"

#include "TestMessPrintEntryUtil.h"

CTestMessPrintEntryUtil::CTestMessPrintEntryUtil(CTestMessBase& aTestStep)
:	CTestMessStoreProcessUtil(aTestStep, ETrue)
	{
	}

/**
 * Prints the ImHeaders of the Entry
 * @param  CImHeader& aHeader - Im Header to edit
 * @return None
 * @panic None
 * @leave KErrNoMemory
 *  
*/
void CTestMessPrintEntryUtil::ProcessImHeaderL(CImHeader& aHeader)
	{
	TPtrC	ptrTemp;

	ptrTemp.Set(aHeader.From());
	iTestStep.INFO_PRINTF2(_L("  From                         : %S"), &ptrTemp);
	ptrTemp.Set(aHeader.ReceiptAddress());
	iTestStep.INFO_PRINTF2(_L("  ReceiptAddress               : %S"), &ptrTemp);
	ptrTemp.Set(aHeader.ReplyTo());
	iTestStep.INFO_PRINTF2(_L("  ReplyTo                      : %S"), &ptrTemp);
	ptrTemp.Set(aHeader.Subject());
	iTestStep.INFO_PRINTF2(_L("  Subject                      : %S"), &ptrTemp);
	}

/**
 * Prints the ImMimeHeaders of the message entry
 * @param  CImMimeHeader& aHeader - Mime Header to edit
 * @return None
 * @panic None
 * @leave KErrNoMemory
 *  
*/
void CTestMessPrintEntryUtil::ProcessImMimeHeaderL(CImMimeHeader& /*aHeader*/)
	{
	}

/**
 * Prints the ImIAPPreferences of the entry
 * @param  CImIAPPreferences& aPreferences - IAP Preferences to edit
 * @return None
 * @panic None
 * @leave KErrNoMemory
 *  
*/
void CTestMessPrintEntryUtil::ProcessInternetAccessPreferencesL(CImIAPPreferences& aPreferences)
	{
	TInt	prefs=aPreferences.NumberOfIAPs();
	for ( TInt pref=0; pref<prefs; )
		{
		TImIAPChoice	choice=aPreferences.IAPPreference(pref++);
		iTestStep.INFO_PRINTF2(_L("  IAP Preference               : %d"), pref);
		iTestStep.INFO_PRINTF2(_L("  iIAP                         : %d"), choice.iIAP);
		iTestStep.INFO_PRINTF2(_L("  iDialogPref                  : %d"), choice.iDialogPref);
		}
	}

/**
 * Prints the CSmsHeader of the entry
 * @param  CSmsHeader& aHeader - Sms Header to edit
 * @return None
 * @panic None
 * @leave KErrNoMemory
 *  
*/
void CTestMessPrintEntryUtil::ProcessSmsHeaderStreamL(CSmsHeader& aHeader)
	{
#if (defined CDMA_API_ENABLED)
	CSmsSettings*	smsSettings = CSmsSettings::NewL();
	CleanupStack::PushL(smsSettings);
	aHeader.GetDefaultMessageSettingsL(*smsSettings);
	TSmsMessageType	pduType = aHeader.MessageType();
	TBool			replyPath = smsSettings->ReplyPath();
	CleanupStack::PopAndDestroy(smsSettings);
#else
	CSmsPDU::TSmsPDUType	pduType=aHeader.Type();
	TBool					replyPath = aHeader.ReplyPathProvided();
#endif
	iTestStep.INFO_PRINTF2(_L("  pduType                      : %d"), pduType);
	iTestStep.INFO_PRINTF2(_L("  replyPathProvided            : %d"), replyPath);
	}


/**
 * Prints the CRichText of the entry
 * @param  CRichText& aBodyText - Text to edit
 * @return None
 * @panic None
 * @leave KErrNoMemory
 *  
*/
void CTestMessPrintEntryUtil::ProcessRichTextBodyL(CRichText& /*aBodyText*/)
	{
	}
