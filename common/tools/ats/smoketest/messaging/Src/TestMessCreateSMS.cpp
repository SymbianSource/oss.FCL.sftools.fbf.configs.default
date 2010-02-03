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

#include "Configuration.cfg"

#include "TestMessCreateSMS.h"
#include "TestMessEditEntryUtil.h"

//Usage of macros of Cdma mtm
#include "TestMessCreateGsmSmsUtil.h"
#if (defined CDMA_API_ENABLED)
#include "TestMessCreateCdmaSmsUtil.h"
#endif

// EPOC includes
#include <mtmuids.h>
#include <txtrich.h>
#include <smut.h>
#include <etelmm.h>
#include <SMUTSET.h> //For CSmsMessageSettings

/*@{*/
/// Literal constants read from the ini file
_LIT(KRecipients,			"recipients");
_LIT(KTo,					"to%d");
_LIT(KPtSaveDraft,			"SaveInDraft");
_LIT(KDefTo,				"smsto");

_LIT(KPtMsgBearerGsm,		"GSM");
//Usage of macros for Cdma mtm
#if (defined CDMA_API_ENABLED)
_LIT(KPtMsgBearerType,		"MessageBearerType");
_LIT(KPtMsgBearerCdma,		"CDMA");
#endif
/*@}*/

#if (defined CDMA_API_ENABLED)
#define KPtDefaultCount		1
#endif

CTestMessCreateSMS::CTestMessCreateSMS()
:	CTestMessBase(EFalse)
,	iSimInDatabase(EFalse)
	{
	SetTestStepName(_L("CreateSms"));
	}

/**
 Creates the SMS message for GSM or CDMA.
 @return TVerdict
*/

TVerdict CTestMessCreateSMS::doTestStepPreambleL()
	{
	TVerdict	ret=CTestMessBase::doTestStepPreambleL();
	// IMK iSimInDatabase=(GetPhoneSmsNumberL(iSimPhoneNumber)==KErrNone);
	iSimInDatabase=false; //IMK (GetPhoneSmsNumberL(iSimPhoneNumber)==KErrNone);
	return ret;
	}

/**
 Creates the SMS message for GSM or CDMA.
 @return TVerdict
*/
TVerdict CTestMessCreateSMS::doTestStepL()
	{
	//	Get scheduling data
	TTimeIntervalMinutes	scheduledTime=ReadScheduleTime();

	//create the message
	TBool					saveDraft = EFalse;
	GetBoolFromConfig(ConfigSection(), KPtSaveDraft, saveDraft);

	if ( saveDraft )
		{
		//Create the message in Draft
		CreateSmsL(KMsvDraftEntryId, scheduledTime);
		}
	else
		{
		//Creates the message in Outbox
		CreateSmsL(KMsvGlobalOutBoxIndexEntryId, scheduledTime);
		}

	return TestStepResult();
	}

void CTestMessCreateSMS::CreateSmsL(const TMsvId aMsvId, TTimeIntervalMinutes aScheduledTime)
	{
	//	Get the bearertype (CDMA/GSM)
	TPtrC	messageBearerType;
	messageBearerType.Set(KPtMsgBearerGsm);
#if (defined CDMA_API_ENABLED)
	GetStringFromConfig(ConfigSection(), KPtMsgBearerType, messageBearerType);
	INFO_PRINTF2(_L("The message bearertype is : %S"), &messageBearerType);
#endif

	CTestMessCreateSmsUtilBase*	createSmsUtil = NULL;
	//	Create the repective util class
	if ( messageBearerType.Compare(KPtMsgBearerGsm)==0 )
		{
		createSmsUtil=new (ELeave) CTestMessCreateGsmSmsUtil(*this);
		}
#if (defined CDMA_API_ENABLED)
	else if ( messageBearerType.Compare(KPtMsgBearerCdma)==0 )
		{
		createSmsUtil=new (ELeave) CTestMessCreateCdmaSmsUtil(*this);
		}
#endif
	else
		{
		WARN_PRINTF1(_L("Unknown bearer type using Gsm"));
		createSmsUtil=new (ELeave) CTestMessCreateGsmSmsUtil(*this);
		}
	CleanupStack::PushL(createSmsUtil);

//	All references to PrepareAccountL can be removed when DEF054045 is fixed
//	createSmsUtil->PrepareAccountL(*iSession);

	if(!iBodyAvailable)
       {
		ERR_PRINTF1(_L("No Body"));
		SetTestStepResult(EFail);
	   }

	if ( TestStepResult()==EPass )
		{
		TInt	err=KErrNone;
		RSendAs			sendAsServer;
		User::LeaveIfError(sendAsServer.Connect());
		CleanupClosePushL(sendAsServer);
		AddMtmCapabilitiesL(sendAsServer);

		RSendAsMessage	sendAs;
		TRAP(err, sendAs.CreateL(sendAsServer, KUidMsgTypeSMS));
		PrepareSendAsL(sendAs);

		if ( err!=KErrNone )
			{
			SetTestStepError(err);
			}
		else
			{
			INFO_PRINTF1(_L("sendAs.SetBodyTextL"));
			sendAs.SetBodyTextL(*iBodyText);

#if (!defined CDMA_API_ENABLED)
			TInt	recipients=1;
			GetIntFromConfig(ConfigSection(), KRecipients, recipients);

			TPtrC	to(iSimPhoneNumber.iTelNumber);
			TBool	returnValue=ETrue;
			for ( TInt recipient=0; (recipient<recipients) && (TestStepResult()==EPass); )
				{
				++recipient;

				if ( iSimInDatabase )
					{
					returnValue=ETrue;
					}
				else
					{
					TBuf<KMaxTestExecuteCommandLength>	tempStore;
					tempStore.Format(KTo(), recipient);
					returnValue=GetStringFromConfig(ConfigSection(), tempStore, to);
					if ( !returnValue )
						{
						returnValue=GetStringFromConfig(KDef, KDefTo, to);
						}
					}
				if ( !returnValue )
					{
					ERR_PRINTF1(_L("No To"));
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF2(_L("To = %S"), &to);
					sendAs.AddRecipientL(to, RSendAsMessage::ESendAsRecipientTo);
					}
				}
#endif

			TRAP(err, sendAs.SaveMessageAndCloseL());
			if ( err!=KErrNone )
				{
				ERR_PRINTF1(_L("Save Message Failiure"));
				SetTestStepResult(EFail);
				sendAs.Close();
				}
			else
				{
				TMsvId				messageId=KMsvNullIndexEntryId;
				CMsvEntry*			parent=iSession->GetEntryL(KMsvDraftEntryIdValue);
				CleanupStack::PushL(parent);
				parent->SetSortTypeL(SelectionOrdering());
				CMsvEntrySelection*	selection=parent->ChildrenL();
				CleanupStack::PushL(selection);

				if ( selection->Count()>0 )
					{
					messageId=selection->At(0);
					}
				if ( messageId!=KMsvNullIndexEntryId )
					{
					SetEntryL(messageId);
					TMsvEntry	child=EntryL().Entry();

					//Get the message header
					CSmsHeader*	smsHeader = createSmsUtil->NewHeaderL(*iBodyText);
					CleanupStack::PushL(smsHeader);

					//If message is too big lets concatenate.....
					ConCatMessageTextL(*smsHeader);

					CMsvStore*	store =NULL;
					TRAPD(error, store = EntryL().EditStoreL());
					if(error != KErrNone)
						{
						ERR_PRINTF2(_L("Edit Store Failed. Error %d"), error);
						SetTestStepError(error);
						}
					else
						{
						CleanupStack::PushL(store);
						smsHeader->RestoreL(*store);
#if (defined CDMA_API_ENABLED)
						//Adding the recipients...
						CArrayPtrFlat<CSmsNumber>& recips = smsHeader->Recipients();

						TInt	recipients=KPtDefaultCount;
						GetIntFromConfig(ConfigSection(), KRecipients, recipients);

						TBool	returnValue;
						for ( TInt recipient=1; recipient<=recipients; recipient++)
							{
							TBuf<KMaxTestExecuteCommandLength>	tempStore;	

							tempStore.Format(KTo(), recipient);	

							TPtrC	to;
							returnValue=GetStringFromConfig(ConfigSection(), tempStore, to);
							if ( !returnValue )
								{
								returnValue=GetStringFromConfig(KDef, KDefTo, to);
								}
							if ( !returnValue )
								{
								ERR_PRINTF1(_L("No To"));
								SetTestStepResult(EFail);
								}
							else
								{
								INFO_PRINTF2(_L("To = %S"), &to);

								//Get the recipient array
								CSmsNumber* newRecip = CSmsNumber::NewL();
								CleanupStack::PushL(newRecip);

								// Check for the email address in recipient
								// This is for email over sms feature
								if(to.Locate('@') != KErrNotFound)
									{
									newRecip->SetAddressL(to, EMsvAddressTypeEMail);
									}
								else
									{
									newRecip->SetAddressL(to, EMsvAddressTypeISDN);
									}

								//Append the recipients
								recips.AppendL(newRecip);
								CleanupStack::Pop(newRecip);
								}
							}
#endif
						child.SetSendingState(KMsvSendStateSuspended);
						child.SetInPreparation(EFalse);
						child.SetVisible(ETrue);

						//Get the details
						TBuf<KSmsDetailsLength>	details;

//Usage of macros of Cdma mtm
#if (!defined CDMA_API_ENABLED)
						if ( TSmsUtilities::GetDetails(EntryL().Session().FileSession(), smsHeader->Message(), details) == KErrNone )
							{
							child.iDetails.Set(details);
							}
#else
						//if ( TSmsUtilities::GetDetails(EntryL().Session().FileSession(), smsHeader->Message(), details) == KErrNone )
						CArrayPtrFlat<CSmsNumber>& smsNumberArray = smsHeader->Recipients();
						if ( smsNumberArray.Count() )
							{
							details.Copy(smsNumberArray[0]->Address());
							}
						if ( details.Length() )
							{
							child.iDetails.Set(details);
							}
#endif

						//Get the description
						TBuf<KSmsDescriptionLength>	desc;
						if ( TSmsUtilities::GetDescription(smsHeader->Message(), desc) == KErrNone )
							{
							child.iDescription.Set(desc);
							}

						//fill up the fields of the message header
						PrepareHeaderL(*createSmsUtil, *smsHeader);

						//set the scheduled time if any
						if ( aScheduledTime!=TTimeIntervalMinutes(0) )
							{
							child.SetSendingState(KMsvSendStateScheduled);
							child.SetScheduled(ETrue);
							child.iDate.UniversalTime();
							child.iDate+=aScheduledTime;
							}

						//	Update entry with data set from .ini file
						EntryL().ChangeL(child);
						smsHeader->StoreL(*store);
						store->CommitL();
						CleanupStack::PopAndDestroy(store);

						CTestMessEditEntryUtil*	editUtil=new (ELeave) CTestMessEditEntryUtil(*this, ETrue, iSimInDatabase, iSimPhoneNumber);
						CleanupStack::PushL(editUtil);
						editUtil->ProcessEntryL(EntryL().EntryId());
						CleanupStack::PopAndDestroy(editUtil);

						if ( aMsvId==KMsvGlobalOutBoxIndexEntryId )
							{
							// Move the selected message to out box
							CTestActive*	active=CTestActive::NewLC(*this);
							CMsvOperation*	op=parent->MoveL(child.Id(), KMsvGlobalOutBoxIndexEntryIdValue, active->iStatus);
							CleanupStack::PushL(op);
							active->Activate();
							CActiveScheduler::Start();
							CleanupStack::PopAndDestroy(2, active);
							}
						}
					CleanupStack::PopAndDestroy(smsHeader);
					}
				CleanupStack::PopAndDestroy(2, parent);
				}
			}

		CleanupStack::PopAndDestroy(&sendAsServer);
		}
	CleanupStack::PopAndDestroy(createSmsUtil);
	}

void CTestMessCreateSMS::AddMtmCapabilitiesL(RSendAs& aSendAs)
	{
	aSendAs.ResetMessageFilter();
	}

void CTestMessCreateSMS::PrepareSendAsL(RSendAsMessage& /*aSendAs*/)
	{
	}

void CTestMessCreateSMS::PrepareHeaderL(CTestMessCreateSmsUtilBase& aCreateUtil, CSmsHeader& aSmsHeader)
	{
	aCreateUtil.PrepareHeaderL(aSmsHeader);
	}

//Usage of macro for CDMA mtm
#if (defined CDMA_API_ENABLED)
void CTestMessCreateSMS::ConCatMessageTextL(CSmsHeader& /*aHeader*/)
	{
	}
#else
void CTestMessCreateSMS::ConCatMessageTextL(CSmsHeader& aHeader)
	//Doxygen compatible comments
	/**
		This method Concatenates the long message. Checks for the length of 
		body, if it is greater than 160 characters sets the concatenation 
		option on for the Sms.

		@param aHeader
		Header for which the concatenation option is set on... 

	*/
	{
	CSmsSettings*	smsSettings = CSmsSettings::NewL();
	CleanupStack::PushL(smsSettings);
	//if the length of the iBodyText is greater than the sms maximum length
	//i.e 160 chars...Set the Concatenation On....
	if( aHeader.Message().MessageLengthL() > aHeader.Message().MaxMessageLength() )
		{
		INFO_PRINTF1(_L("Concatenate Message..."));
		INFO_PRINTF1(_L("Message Exceeds max length setting Concatenate On..."));
		aHeader.GetSmsSettingsL(*smsSettings);
		smsSettings->SetCanConcatenate(ETrue);
		aHeader.SetSmsSettingsL(*smsSettings);
		}
	CleanupStack::PopAndDestroy(smsSettings);
	}
#endif
