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
 
#include "Configuration.cfg"

#include "TestMessCreateGsmSmsUtil.h"

/** 
 Constructor
*/
CTestMessCreateGsmSmsUtil::CTestMessCreateGsmSmsUtil(CTestMessBase &aTestStep)
: 	CTestMessCreateSmsUtilBase(aTestStep)
	{
	aTestStep.INFO_PRINTF1(_L("The GSM SMS util is created"));
	}

/**
 Prepares the SMS account details for CSendAs/RSendAs
 @leave - KErrNoMemory
 @leave - System wide error codes
*/
void CTestMessCreateGsmSmsUtil::PrepareAccountL(CMsvSession& /*aSession*/)
	{
	}

/**
 Creates the header object for the GSM sms message
 @param aBodyText - body text of the message
 @return CSmsHeader* - pointer to CSmsHeader
 @leave - KErrNoMemory
 @leave - system wide error codes
*/
CSmsHeader* CTestMessCreateGsmSmsUtil::NewHeaderL(CRichText &aBodyText)
	{
	return CSmsHeader::NewL(CSmsPDU::ESmsSubmit, aBodyText);
	}

/**
 Fills up the header object with the data read from the ini file.
 @param aSmsHeader - sms header object ofthe sms message
 @leave - system wide error codes
*/
void CTestMessCreateGsmSmsUtil::PrepareHeaderL(CSmsHeader &aSmsHeader)
	{
	//	Get bearer data
	TPtrC	ptrBearer;
	TBool	returnValue=iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), KBearer, ptrBearer);
	if ( !returnValue )
		{
		//If bearer is not provided, read from the default section of the ini file
		returnValue=iTestStep.GetStringFromConfig(KDef, KDefBearer, ptrBearer);
		}

	if ( returnValue )
		{
		//default value for Bio message ID type
		TBioMsgIdType	bearer = EBioMsgIdNbs;

		iTestStep.INFO_PRINTF2(_L("Bearer = %S"), &ptrBearer);
		if (ptrBearer.CompareF(KBearerWap) == 0)
			{
			//Set Wap message ID
			bearer = EBioMsgIdWap;
			}
		else if (ptrBearer.CompareF(KBearerWapSecure) == 0)
			{
			//Set WapSecure message ID
			bearer = EBioMsgIdWapSecure;
			}
		else
			{
			iTestStep.INFO_PRINTF1(_L("No supported bearer is provided. Default is used"));
			}

		//Set the Bio message ID type.
#if (defined CDMA_API_ENABLED)
		aSmsHeader.BioMessage().SetBioMsgIdType(bearer);
#else
		aSmsHeader.SetBioMsgIdType(bearer);
#endif
		}

	//	Get encoding data
	TInt	encoding;
	returnValue=iTestStep.GetIntFromConfig(iTestStep.ConfigSection(), KEncoding, encoding);
	if ( !returnValue )
		{
		//If encoding is not provied, read it from the default section of the ini file
		returnValue=iTestStep.GetIntFromConfig(KDef, KDefEncoding, encoding);
		}
	if ( returnValue )
		{
		iTestStep.INFO_PRINTF2(_L("Encoding = %d"), encoding);
#if (defined CDMA_API_ENABLED)
		TSmsCharacterEncoding	charEncoding;

		switch (encoding)
			{
		case 7:
			//7 bit encoding format
			charEncoding = KSmsEncoding7BitGsm;
			break;
		case 8:
			//8 bit encoding format
			charEncoding = KSmsEncodingBinary;
			break;
		case 16:
			//Unicode encoding format
			charEncoding = KSmsEncodingUnicode;
			break;
		default:
			//default encoding format
			charEncoding = KSmsEncoding7BitGsm;
			break;
			}

		//Set the character encoding
		aSmsHeader.BioMessage().SetEncoding(charEncoding);
#else
		TSmsDataCodingScheme::TSmsAlphabet	alpha = TSmsDataCodingScheme::ESmsAlphabet7Bit;

		switch (encoding)
			{
		case 7:
			alpha = TSmsDataCodingScheme::ESmsAlphabet7Bit;
			break;
		case 8:
			alpha = TSmsDataCodingScheme::ESmsAlphabet8Bit;
			break;
		case 16:
			alpha = TSmsDataCodingScheme::ESmsAlphabetUCS2;
			break;
		default:
			alpha = TSmsDataCodingScheme::ESmsAlphabet7Bit;
			break;
			}

		CSmsPDU&	pdu=aSmsHeader.Message().SmsPDU();
		if ( pdu.DataCodingSchemePresent() )
			{
			pdu.SetAlphabet(alpha);
			}
#endif
		}

	//	Get delivery report data
	TPtrC	ptrDeliveryReport;
	returnValue=iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), KDeliveryReport, ptrDeliveryReport);
	if ( !returnValue )
		{
		returnValue=iTestStep.GetStringFromConfig(KDef, KDefDeliveryReport, ptrDeliveryReport);
		}
	if ( returnValue )
		{
		TBool	changeDR = EFalse;
		TBool	deliveryReport = EFalse;

		iTestStep.INFO_PRINTF2(_L("Delivery Report = %S"), &ptrDeliveryReport);
		if (ptrDeliveryReport.CompareF(_L("NO")) == 0)
			{
			changeDR = ETrue;
			deliveryReport = EFalse;
			}
		else if (ptrDeliveryReport.CompareF(_L("YES")) == 0)
			{
			changeDR = ETrue;
			deliveryReport = ETrue;
			}

		if ( changeDR )
			{
#if (defined CDMA_API_ENABLED)
			aSmsHeader.SetAcknowledgementRequest(ESmsAckTypeDelivery, deliveryReport);
#else
			switch ( aSmsHeader.Type() )
				{
			case CSmsPDU::ESmsSubmit:
				aSmsHeader.Submit().SetStatusReportRequest(deliveryReport);
				break;
			case CSmsPDU::ESmsCommand:
				aSmsHeader.Command().SetStatusReportRequest(deliveryReport);
				break;
			default:
				User::Leave(KErrNotSupported);
				}
#endif
			}
		}
	}
/**
 Sets the fields for the sms account settings using data read from the ini file
 If no data is given in ini file, the default settings in the account is used.
 @param aSmsSettings - CSmsSettings reference
*/
void CTestMessCreateGsmSmsUtil::SetSmsAccountSettings(CSmsSettings &aSmsSettings)
	{
	iTestStep.INFO_PRINTF1(_L("Set SMS account Settings...."));
	TBool	boolTemp;

	//	Set the Reply quoted setting.
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KReplyQuoted, boolTemp) )
		{
		aSmsSettings.SetReplyQuoted(boolTemp);
		}

	//	Set the Reject duplicate settings.
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KRejectDuplicate, boolTemp) )
		{
#if (defined CDMA_API_ENABLED)
		aSmsSettings.MessageSettings().Gsm().SetRejectDuplicate(boolTemp);
#else
		aSmsSettings.SetRejectDuplicate(boolTemp);
#endif
		}

	//	Set the option for delivery report.
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KDeliveryReport, boolTemp) )
		{
		aSmsSettings.SetDeliveryReport(boolTemp);
		}

	//	Set the concatenate feature
	if ( iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KConCatenate, boolTemp) )
		{
#if (defined CDMA_API_ENABLED)
		aSmsSettings.MessageSettings().Gsm().SetCanConcatenate(boolTemp);
#else
		aSmsSettings.SetCanConcatenate(boolTemp);
#endif
		}

	//	Set whether to use the same path while replying.
	if(iTestStep.GetBoolFromConfig(iTestStep.ConfigSection(), KReplyPath, boolTemp))
		{
#if (defined CDMA_API_ENABLED)
		aSmsSettings.MessageSettings().Gsm().SetReplyPath(boolTemp);
#else
		aSmsSettings.SetReplyPath(boolTemp);
#endif
		}

	//can implement these features later...
	aSmsSettings.SetValidityPeriod(ESmsVPWeek);	// week 
	aSmsSettings.SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFInteger); //relative
	aSmsSettings.SetDelivery(ESmsDeliveryImmediately);
	aSmsSettings.SetMessageConversion(ESmsConvPIDNone);
	}
