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
// TestMessCreateCdmsSmsUtil.cpp
// This class creates the Cdma sms header and the service settings
// 
//

#include "TestMessCreateCdmaSmsUtil.h"
#include "TestMessCreateSmsAccount.h"

// EPOC includes
#include <csmsaccount.h>

/*@{*/
_LIT(KPtDeliveryAck,			"DeliveryAck");
_LIT(KPtUserAck,				"UserAck");
_LIT(KPtReadAck,				"ReadAck");
_LIT(KPtMessageConversion,		"MessageConversion");
_LIT(KPtPriorityIndicator,		"PriorityIndicator");
_LIT(KPtPrivacyIndicator,		"PrivacyIndicator");
_LIT(KPtReplyOption,			"ReplyOption");
_LIT(KPtAlertonDelivery,		"AlertonDelivery");
_LIT(KPtLanguageIndicator,		"LanguageIndicator");

//Constants used
_LIT(KPtTeleService,			"TeleService");
_LIT(KPtSubmit,					"Submit");
_LIT(KPtCancel,					"Cancel");

// Supported Teleservices
_LIT(KPtTeleWmt,				"WMT");
_LIT(KPtTeleWemt,				"WEMT");
_LIT(KPtTeleWpt,				"WPT");
_LIT(KPtTeleVmn,				"VMN");
_LIT(KPtTeleScpt,				"SCPT");
_LIT(KPtTeleCatpt,				"CATPT");
_LIT(KPtTeleCmt91,				"CMT91");
_LIT(KPtTeleWap,				"WAP");

// Supported message conversion formats
_LIT(KPtPidFax,					"Fax");
_LIT(KPtPidX400,				"X400");
_LIT(KPtPidPaging,				"Paging");
_LIT(KPtPidMail,				"Mail");
_LIT(KPtPidErmes,				"Ermes");
_LIT(KPtPidSpeech,				"Speech");
/*@}*/

/**
 Constructor
*/
CTestMessCreateCdmaSmsUtil ::CTestMessCreateCdmaSmsUtil(CTestMessBase& aTestStep)
: 	CTestMessCreateSmsUtilBase(aTestStep)
	{
	aTestStep.INFO_PRINTF1(_L("The CDMA SMS util is created"));
	}

/**
 Prepares the SMS account details for CSendAs/RSendAs
 @leave - KErrNoMemory
 @leave - System wide error codes
*/
void CTestMessCreateCdmaSmsUtil::PrepareAccountL(CMsvSession& /*aSession*/)
	{
	// Reads the teleservice
	tia637::TTeleserviceId	teleServiceId=tia637::KTeleserviceWMT;
	TPtrC 					ptrTeleService;
	if( !(iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), KPtTeleService, ptrTeleService)))
		{
		iTestStep.INFO_PRINTF1(_L("Teleservice is not provided. Default teleservice (WMT) is used"));
		}
	else
		{
		iTestStep.INFO_PRINTF2(_L("Teleservice to be set is : %S"), &ptrTeleService);

		//Get the teleservice Id
		teleServiceId = GetTeleServiceId(ptrTeleService);
		}

	if ( teleServiceId!=tia637::KTeleserviceWEMT )
		{
		teleServiceId=tia637::KTeleserviceWMT;
		}

	CSmsSettings*	smsSettings = CSmsSettings::NewL();
	CleanupStack::PushL(smsSettings);

	CSmsAccount*	smsAccount=CSmsAccount::NewLC();
	smsAccount->LoadSettingsL(*smsSettings);
	smsSettings->SetDefaultTeleservice(teleServiceId);
	smsAccount->SaveSettingsL(*smsSettings);
	CleanupStack::PopAndDestroy(smsAccount);
	CleanupStack::PopAndDestroy(smsSettings);
	}

/**
 Creates a new CDMA sms header object
 @param aBodyText - body text of the message
 @return CSmsHeader - pointer of the CSmsHeader object
 @leave - KErrNoMemory
 @leave - System wide error codes
*/
CSmsHeader* CTestMessCreateCdmaSmsUtil::NewHeaderL(CRichText& aBodyText)
	{
	return CSmsHeader::NewL(KSmsTypeSubmit, aBodyText);
	}

/**
 Fill up the header object with data read from the ini file
 @param aSmsHeader - reference for the sms header object
 @leave - System wide error codes
*/
void CTestMessCreateCdmaSmsUtil::PrepareHeaderL(CSmsHeader& aSmsHeader)
	{
	//	Get bearer data
	TPtrC	ptrBearer;
	TBool	returnValue=iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), KBearer, ptrBearer);
	if ( !returnValue )
		{
		//If no bearer provided, the bearer is read from the default section ofthe
		// ini file
		returnValue=iTestStep.GetStringFromConfig(KDef, KDefBearer, ptrBearer);
		}

	if ( returnValue )
		{
		//default value for the bearer
		TBioMsgIdType	bearer = EBioMsgIdNbs;

		iTestStep.INFO_PRINTF2(_L("Bearer = %S"), &ptrBearer);
		if (ptrBearer.CompareF(KBearerWap) == 0)
			{
			//Wap message ID
			bearer = EBioMsgIdWap;
			}
		else if (ptrBearer.CompareF(KBearerWapSecure) == 0)
			{
			//WapSecure message ID
			bearer = EBioMsgIdWapSecure;
			}
		else
			{
			iTestStep.ERR_PRINTF1(_L("No bearer provided. Default is used"));
			}

		//Set the BIO Message ID type
		aSmsHeader.BioMessage().SetBioMsgIdType(bearer);
		}

	//	Get encoding data
	TInt	encoding;
	returnValue=iTestStep.GetIntFromConfig(iTestStep.ConfigSection(), KEncoding, encoding);
	if ( !returnValue )
		{
		//If no encoding is provided, the encoding is read from the default section 
		//of the ini file
		returnValue=iTestStep.GetIntFromConfig(KDef, KDefEncoding, encoding);
		}
	if ( returnValue )
		{
		//Default encoding value.
		TSmsCharacterEncoding charEncoding = KSmsEncodingUnicode;

		iTestStep.INFO_PRINTF2(_L("Encoding = %d"), encoding);
		switch (encoding)
			{
		case 7:
			//7 bit encoding format
			charEncoding = KSmsEncoding7BitASCII;
			break;
		case 8:
			//8 bit encoding format
			charEncoding = KSmsEncodingBinary;
			break;
		case 16:
			//unicode encoding format
			charEncoding = KSmsEncodingUnicode;
			break;
		default:
			//default encoding format
			charEncoding = KSmsEncodingUnicode;
			break;
			}
		//Set the character encoding format
		aSmsHeader.BioMessage().SetEncoding(charEncoding);
		}

	
	TBool	deliveryAck = EFalse;
	TBool	userAck = EFalse;
	TBool	readAck = EFalse;

	//	Get acknowledgements report request
	TPtrC	ptrDeliveryReport;
	returnValue=iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), KDeliveryReport, ptrDeliveryReport);
	if ( !returnValue )
		{
		returnValue=iTestStep.GetStringFromConfig(KDef, KDefDeliveryReport, ptrDeliveryReport);
		}
	if ( returnValue )
		{
		iTestStep.INFO_PRINTF2(_L("The acknowledgement request flag : %S"), &ptrDeliveryReport);
		if(ptrDeliveryReport.Compare(_L("NO")) == 0)
			{
			deliveryAck = EFalse;
			userAck = EFalse;
			readAck = EFalse;
			}
		if(ptrDeliveryReport.Compare(_L("YES")) == 0)
			{
			// Read different type of acknowledgements
			iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KPtDeliveryAck, deliveryAck);
			iTestStep.INFO_PRINTF2(_L("The Delivery acknowledgement request flag : %d"), deliveryAck);
			iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KPtUserAck, userAck);
			iTestStep.INFO_PRINTF2(_L("The User acknowledgement request flag : %d"), userAck);
			iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KPtReadAck, readAck);
			iTestStep.INFO_PRINTF2(_L("The Read acknowledgement request flag : %d"), readAck);
			}
		}
	
	aSmsHeader.SetAcknowledgementRequest(ESmsAckTypeDelivery, deliveryAck);
	aSmsHeader.SetAcknowledgementRequest(ESmsAckTypeUser, userAck);
	aSmsHeader.SetAcknowledgementRequest(ESmsAckTypeRead, readAck);
	
	//Get the message conversion type
	TPtrC	ptrMessageConversion;
	TSmsPIDConversion smsPidConversion = ESmsConvPIDNone;
	returnValue=iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), KPtMessageConversion, ptrMessageConversion);
	if ( !returnValue )
		{
		iTestStep.INFO_PRINTF1(_L("The message conversion is not provided. Default (PIDNone) is taken"));
		}
	if ( returnValue )
		{
		iTestStep.INFO_PRINTF2(_L("The message conversion to be set is  : %S"), &ptrMessageConversion);
		//Get the Message conversion id
		smsPidConversion = GetMessageConversionId(ptrMessageConversion);
		}
	//Set the message conversion.
	CSmsSettings *smsSettings = CSmsSettings::NewL();
	CleanupStack::PushL(smsSettings);
	aSmsHeader.GetDefaultMessageSettingsL(*smsSettings);
	smsSettings->SetMessageConversion(smsPidConversion);
	CleanupStack::Pop(smsSettings);

	// Set the other message settings
	aSmsHeader.CdmaMessage().SetPrivacyIndicatorL(tia637::KBdRestricted);
	aSmsHeader.CdmaMessage().SetPriorityIndicatorL(tia637::KBdUrgent);
	aSmsHeader.CdmaMessage().SetAlertOnDeliveryL(tia637::KBdUseMediumPriorityAlert);
	aSmsHeader.CdmaMessage().SetLanguageIndicatorL(tia637::KLanguageEnglish);
	}

/**
 Sets the cdma sms service settings fields using data read from the
 ini file. If no data is provided/read from ini file, the default 
 account settings is used.
 @param aSmsSettings - reference of the CSmsSettings
*/
void CTestMessCreateCdmaSmsUtil::SetSmsAccountSettings(CSmsSettings& aSmsSettings)
	{
	iTestStep.INFO_PRINTF1(_L("Set SMS account Settings...."));
	TBool	boolTemp;
	
	//	Set the Reply quoted setting.
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KReplyQuoted, boolTemp) )
		{
		aSmsSettings.SetReplyQuoted(boolTemp);
		}

	// Set the priority indicator
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KPtPriorityIndicator, boolTemp) )
		{
		aSmsSettings.MessageSettings().Cdma().ActivateMessageSetting(tia637::KTPriorityIndicator, boolTemp);
		}

	//Set the Privacy Indicator	
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KPtPrivacyIndicator, boolTemp) )
		{
		aSmsSettings.MessageSettings().Cdma().ActivateMessageSetting(tia637::KTPrivacyIndicator, boolTemp);
		}

	//Set the Reply option
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KPtReplyOption, boolTemp) )
		{
		aSmsSettings.MessageSettings().Cdma().ActivateMessageSetting(tia637::KTReplyOption, boolTemp);
		}

	//Set the alert on Message deliver
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KPtAlertonDelivery, boolTemp) )
		{
		aSmsSettings.MessageSettings().Cdma().ActivateMessageSetting(tia637::KTAlertOnMessageDelivery, boolTemp);
		}

	//Set the Language Indicator
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KPtLanguageIndicator, boolTemp) )
		{
		aSmsSettings.MessageSettings().Cdma().ActivateMessageSetting(tia637::KTLanguageIndicator, boolTemp);
		}

	aSmsSettings.SetValidityPeriod(ESmsVPWeek);	// week 
	aSmsSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFInteger); //relative
	aSmsSettings.SetDelivery(ESmsDeliveryImmediately);
	}

/**
 Get the message type id
 @param aMessageType - TPtrC object holding the message type
 @return TSmsMessageType - message type id
*/
TSmsMessageType CTestMessCreateCdmaSmsUtil::GetMessageTypeId(const TDesC& aMessageType)
	{
	//Default message type is assigned
	TSmsMessageType smsMessageType = KSmsTypeSubmit;

	if( aMessageType.Compare(KPtSubmit) ==0)
		{
		smsMessageType = KSmsTypeSubmit;
		}
	else if( aMessageType.Compare(KPtCancel) ==0)
		{
		smsMessageType = KSmsTypeCdmaCancellation;
		}
	else
		{
		iTestStep.WARN_PRINTF1(_L("Invalid message type provided. Default is used"));
		}
	return smsMessageType;
	}
	
/**
 Gets the Teleservice Id
 @param aTeleService - TPtrC holding the tele service name
 @return tia637::TTeleserviceId - teleservice id
*/
tia637::TTeleserviceId CTestMessCreateCdmaSmsUtil::GetTeleServiceId(const TDesC& aTeleService)
	{
	//default teleservice ID assigned
	tia637::TTeleserviceId	teleServiceId = tia637::KTeleserviceWMT;

	iTestStep.INFO_PRINTF2(_L("Teleservice set is : %S"), &aTeleService);
	if( aTeleService.Compare(KPtTeleScpt) ==0)
		{
		teleServiceId = tia637::KTeleserviceSCPT;
		}
	else if( aTeleService.Compare(KPtTeleCatpt) ==0)
		{
		teleServiceId = tia637::KTeleserviceCATPT;
		}
	else if( aTeleService.Compare(KPtTeleCmt91) ==0)
		{
		teleServiceId = tia637::KTeleserviceCMT91;
		}
	else if( aTeleService.Compare(KPtTeleVmn) ==0)
		{
		teleServiceId = tia637::KTeleserviceVMN;
		}
	else if( aTeleService.Compare(KPtTeleWap) ==0)
		{
		teleServiceId = tia637::KTeleserviceWAP;
		}
	else if( aTeleService.Compare(KPtTeleWemt) ==0)
		{
		teleServiceId = tia637::KTeleserviceWEMT;
		}
	else if( aTeleService.Compare(KPtTeleWmt) ==0)
		{
		teleServiceId = tia637::KTeleserviceWMT;
		}
	else if( aTeleService.Compare(KPtTeleWpt) ==0)
		{
		teleServiceId = tia637::KTeleserviceWPT;
		}
	else
		{
		iTestStep.WARN_PRINTF1(_L("Invalid Teleservice provided. Default WMT used"));
		}

	return teleServiceId;
	}

/**
 Gets the Message Conversion Id
 @param aTeleService - TPtrC holding the tele service name
 @return tia637::TTeleserviceId - teleservice id
*/
TSmsPIDConversion CTestMessCreateCdmaSmsUtil::GetMessageConversionId(const TDesC& aMessageConversion)
	{
	TSmsPIDConversion	smsPidConv = ESmsConvPIDNone;

	iTestStep.INFO_PRINTF2(_L("The message conversion set is  : %S"), &aMessageConversion);
	if( aMessageConversion.Compare(KPtPidFax) ==0)
		{
		smsPidConv = ESmsConvFax;
		}
	else if( aMessageConversion.Compare(KPtPidX400) ==0)
		{
		smsPidConv = ESmsConvX400;
		}
	else if( aMessageConversion.Compare(KPtPidPaging) ==0)
		{
		smsPidConv = ESmsConvPaging;
		}
	else if( aMessageConversion.Compare(KPtPidMail) ==0)
		{
		smsPidConv = ESmsConvMail;
		}
	else if( aMessageConversion.Compare(KPtPidErmes) ==0)
		{
		smsPidConv = ESmsConvErmes;
		}
	else if( aMessageConversion.Compare(KPtPidSpeech) ==0)
		{
		smsPidConv = ESmsConvSpeech;
		}
	else
		{
		iTestStep.INFO_PRINTF1(_L("The message conversion provided is invalid"));
		iTestStep.INFO_PRINTF1(_L("The default NONE message conversion is set"));
		}
	
	return smsPidConv;
	}
