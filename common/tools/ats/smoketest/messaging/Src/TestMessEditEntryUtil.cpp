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


#include "TestMessEditEntryUtil.h"

// EPOC includes

#include <smut.h>
#include <miutmsg.h>
#include <mtclreg.h>

/**
 * @name Constant Literals used.
 */
/*@{*/
_LIT(KToRecipients,			"toRecipients");
_LIT(KTo,					"to%d");
_LIT(KDef,					"default");
_LIT(KDefTo,				"emailto");

/// Literal constants used
_LIT(KCharacterSetType,		"characterSetType");
_LIT(KPtEditBody,			"EditBody");
_LIT(KPtEditRecipients,		"EditRecipients");
_LIT(KPtRecipient,			"recipient%d");
/*@}*/

CTestMessEditEntryUtil::CTestMessEditEntryUtil(CTestMessBase& aTestStep, TBool aSimChecked, TBool aSimInDatabase, RMobilePhone::TMobileAddress aSimPhoneNumber)
:	CTestMessStoreProcessUtil(aTestStep, EFalse)
,	iSimChecked(aSimChecked)
,	iSimInDatabase(aSimInDatabase)
,	iSimPhoneNumber(aSimPhoneNumber)
	{
	}

/**
 * Edits the ImHeaders of the Entry
 * @param  CImHeader& aHeader - Im Header to edit
 * @return None
 * @panic None
 * @leave KErrNoMemory
 *  
*/
void CTestMessEditEntryUtil::ProcessImHeaderL(CImHeader& aHeader)
	{
	// Edit the CImHeader information
	TInt	recipients;
	if ( !iTestStep.GetIntFromConfig(iTestStep.ConfigSection(), KToRecipients, recipients) )
		{
		recipients=0;
		}

	for ( TInt recipient=1; recipient<=recipients; recipient++)
		{
		TBuf<KMaxTestExecuteCommandLength>	tempStore;
		tempStore.Format(KTo(), recipient);

		TPtrC	to;
		TInt	returnValue=iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), tempStore, to);
		if ( !returnValue )
			{
			returnValue=iTestStep.GetStringFromConfig(KDef, KDefTo, to);
			}
		if ( !returnValue )
			{
			iTestStep.ERR_PRINTF1(_L("No To"));
			iTestStep.SetTestStepResult(EFail);
			}
		else
			{
			iTestStep.INFO_PRINTF2(_L("To = %S"), &to);
			aHeader.ToRecipients().AppendL(to);
			}
		}
	}

/**
 * Edits the ImMimeHeaders of the message entry
 * @param  CImMimeHeader& aHeader - Mime Header to edit
 * @return None
 * @panic None
 * @leave KErrNoMemory
 *  
*/
void CTestMessEditEntryUtil::ProcessImMimeHeaderL(CImMimeHeader& aHeader)
	{
	// Update the character set info
	TPtrC	charSetType;
	if ( iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), KCharacterSetType, charSetType) )
		{
		TUint	charSetVal;
		if ( ValidCharacterSetInfo(charSetType, charSetVal) )
			{
			aHeader.SetMimeCharset(charSetVal);
			}
		}
	}

/**
 * Edits the ImIAPPreferences of the entry
 * @param  CImIAPPreferences& aPreferences - IAP Preferences to edit
 * @return None
 * @panic None
 * @leave KErrNoMemory
 *  
*/
void CTestMessEditEntryUtil::ProcessInternetAccessPreferencesL(CImIAPPreferences& /*aPreferences*/)
	{
	}

/**
 Edits the CSmsHeader of the entry
 @param  CSmsHeader& aHeader - Sms Header to edit
 @return None
 @panic None
 @leave KErrNoMemory
*/
void CTestMessEditEntryUtil::ProcessSmsHeaderStreamL(CSmsHeader& aSmsHeader)
	{
	// Get the number of recipients 
	TInt	recipients = 0;
	if ( !iTestStep.GetIntFromConfig(iTestStep.ConfigSection(), KPtEditRecipients, recipients) )
		{
		recipients=0;
		iTestStep.INFO_PRINTF1(_L("No Recipients edited/added"));
		}
	iTestStep.INFO_PRINTF2(_L("The number of recipients : %d"), recipients);

	if ( !iSimChecked )
		{
		//IMK iSimInDatabase=(iTestStep.GetPhoneSmsNumberL(iSimPhoneNumber)==KErrNone);
		iSimInDatabase=0;
		}
	TPtrC	to(iSimPhoneNumber.iTelNumber);
	TInt	returnValue=ETrue;
	for ( TInt recipient=0; (recipient<recipients) && (iTestStep.TestStepResult()==EPass); )
		{
		++recipient;
		if ( iSimInDatabase )
			{
			returnValue=ETrue;
			}
		else
			{
			TBuf<KMaxTestExecuteCommandLength>	tempStore;
			tempStore.Format(KPtRecipient(), recipient);

			//Get the recipient address
			returnValue=iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), tempStore, to);
			if ( !returnValue )
				{
				returnValue=iTestStep.GetStringFromConfig(KDef, KDefTo, to);
				}
			}
		if ( !returnValue )
			{
			iTestStep.ERR_PRINTF1(_L("Recipient number is not Provided"));
			iTestStep.SetTestStepResult(EFail);
			}
		else
			{
			//Append the recipient address in recipient list
			CSmsNumber*	smsNumber = CSmsNumber::NewL();
			CleanupStack::PushL(smsNumber);
			iTestStep.INFO_PRINTF2(_L("To = %S"), &to);
			smsNumber->SetAddressL(to);
			aSmsHeader.Recipients().AppendL(smsNumber);
			//No need to destory CSmsNumber. CSmsHeader destroys it
			CleanupStack::Pop(smsNumber);
			}
		}
	}


/**
 Edits the CRichText of the entry
 @param  CRichText& aBodyText - Text to edit
 @return None
 @panic None
 @leave KErrNoMemory
*/
void CTestMessEditEntryUtil::ProcessRichTextBodyL(CRichText& aBodyText)
	{
	TPtrC	ptrBody;
	//Get the body text file
	TBool	returnValue=iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), KPtEditBody, ptrBody);
	if ( !returnValue )
		{
		iTestStep.INFO_PRINTF1(_L("Body is not edited"));
		}
	else
		{
		iTestStep.INFO_PRINTF2(_L("Body = %S"), &ptrBody);

		// open the file
		RFs rfs;
		rfs.Connect();
		RFile	file;

		//Append the body text of the entry
		TInt anError = file.Open(rfs, ptrBody, EFileShareAny);
		if(anError == KErrNone)
			{
			TBool		reading=ETrue;
			TInt		pos = aBodyText.LdDocumentLength();
			TBuf8<1>	charIn;
			while (reading)
				{
				file.Read(charIn, 1);
				if(charIn.Length())
					switch ( charIn[0] )
						{
					case '\r':
						aBodyText.InsertL(pos++, CEditableText::ELineBreak);
					case '\n':
						break;
					default:
						aBodyText.InsertL(pos++, TChar(charIn[0]));
						}
				else
					{
					reading = FALSE;	// stop at the end of the file
					}
				}
			file.Close();
			rfs.Close();
			}
		else
			{
			iTestStep.ERR_PRINTF2(_L("Cannot open file = %S"), &ptrBody);
			iTestStep.SetTestStepResult(EFail);
			}
		}	
	}
