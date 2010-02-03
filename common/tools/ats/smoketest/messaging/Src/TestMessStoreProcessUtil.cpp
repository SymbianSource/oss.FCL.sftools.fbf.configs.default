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
// This contains CTestMessStoreProcessUtil. Base class process what is in the message store
// 
//



/**
 @file TestMessStoreProcessUtil.cpp
*/

#include "TestMessStoreProcessUtil.h"

//	EPOC include
#include <txtrich.h>

/**
 * @name Constant Literals used.
 */
/*@{*/
/// Literal constants used for the character set
_LIT(KChineseGB2312,		"ChineseGB2312");
_LIT(KChineseHz,			"ChineseHz");
_LIT(KChineseBig5,			"ChineseBig5");
/*@}*/

CTestMessStoreProcessUtil::CTestMessStoreProcessUtil(CTestMessBase& aTestStep, TBool aReadOnly)
:	CBase()
,	iTestStep(aTestStep)
,	iReadOnly(aReadOnly)
	{
	}

/**
 * Process the message entry
 * @param  TMsvId aMsvId - Id of message to edit
 * @return TInt - return TErrNone if all worked
 *  
*/
TInt CTestMessStoreProcessUtil::ProcessEntryL(TMsvId aMsvId)
	{
	TInt		ret=KErrNone;
	CMsvEntry*	msvEntry=iTestStep.Session().GetEntryL(aMsvId);
	CleanupStack::PushL(msvEntry);

	if ( msvEntry->HasStoreL() )
		{
		CMsvStore*	store=NULL;
		if ( iReadOnly )
			{
			store=msvEntry->ReadStoreL();
			}
		else
			{
			store=msvEntry->EditStoreL();
			}
		CleanupStack::PushL(store);

		RArray<TUid>	ids;
		ids.Append(KUidMsgFileIMailHeader);
		ids.Append(KUidMsgFileMimeHeader);
		ids.Append(KUidMsgFileInternetAccessPreferences);
		ids.Append(KUidMsvSMSHeaderStream);
		ids.Append(KUidMsvDefaultServices);
		ids.Append(KMsvEntryRichTextBody);
		for ( TUint uidIndex=ids.Count(); uidIndex>0; )
			{
			TUid	uid=ids[--uidIndex];

			if ( store->IsPresentL(uid) )
				{
				if ( uid==KUidMsgFileIMailHeader )
					{
					// Process the CImHeader info
					ProcessImHeaderL(*store);
					}
				else if ( uid==KUidMsgFileMimeHeader )
					{
					// Process the CImMimeHeader info
					ProcessImMimeHeaderL(*store);
					}
				else if ( uid==KUidMsgFileInternetAccessPreferences )
					{
					}
				else if ( uid==KUidMsvSMSHeaderStream )
					{
					// Process the CSmsHeader info
					ProcessSmsHeaderStreamL(*store);
					}
				else if ( uid==KUidMsvDefaultServices )
					{
					}
				else if ( uid==KMsvEntryRichTextBody )
					{
					// Process the CRichText info
					ProcessRichTextBodyL(*store);
					}
				else
					{
					iTestStep.INFO_PRINTF2(_L("Cannot process %x"), uid.iUid);
					}
				}
			}

		// Commit to the store
		if ( !iReadOnly )
			{
			store->CommitL();
			}
		CleanupStack::PopAndDestroy(store);
		}

	CleanupStack::PopAndDestroy(msvEntry);

	return ret;
	}

// Process the CImHeader information
void CTestMessStoreProcessUtil::ProcessImHeaderL(CMsvStore& aStore)
	{
	CImHeader*	header=CImHeader::NewLC();
	TRAPD(err, header->RestoreL(aStore));

	if ( err==KErrNone )
		{
		ProcessImHeaderL(*header);
		if ( !iReadOnly )
			{
			header->StoreL(aStore);
			}
		}

	CleanupStack::PopAndDestroy(header);
	}

// Process the CImMimeHeader information
void CTestMessStoreProcessUtil::ProcessImMimeHeaderL(CMsvStore& aStore)
	{
	CImMimeHeader*	header=CImMimeHeader::NewLC();
	TRAPD(err, header->RestoreL(aStore));

	if ( err==KErrNone )
		{
		ProcessImMimeHeaderL(*header);
		if ( !iReadOnly )
			{
			header->StoreL(aStore);
			}
		}

	CleanupStack::PopAndDestroy(header);
	}


// Process the CSmsHeader information
void CTestMessStoreProcessUtil::ProcessSmsHeaderStreamL(CMsvStore& aStore)
	{
	CParaFormatLayer*	paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer*	charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText*			bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer);
	CleanupStack::PushL(bodyText);

	CSmsHeader*			header = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *bodyText);
	CleanupStack::PushL(header);

	TRAPD(err, header->RestoreL(aStore));
	if ( err == KErrNone )
		{
		ProcessSmsHeaderStreamL(*header);
		if ( !iReadOnly )
			{
			header->StoreL(aStore);
			}
		}

	CleanupStack::PopAndDestroy(4, paraFormatLayer);
	}


// Process the CRichText information
void CTestMessStoreProcessUtil::ProcessRichTextBodyL(CMsvStore& aStore)
	{
	CParaFormatLayer*	paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer*	charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText*			bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer);
	CleanupStack::PushL(bodyText);

	TRAPD(err, aStore.RestoreBodyTextL(*bodyText));
	if ( err == KErrNone )
		{
		ProcessRichTextBodyL(*bodyText);
		if ( !iReadOnly )
			{
			aStore.StoreBodyTextL(*bodyText);
			}
		}

	CleanupStack::PopAndDestroy(3, paraFormatLayer);
	}

/**
 * Saves the new character set to the message 
 * @param  TPtrC aCharacterSetType - New Character set type to be stored for the received message
 * @param  TUint &aCharSetVal - character set value
 * @return TBool 
 * @panic None
 * @leave None
 *
*/
TBool CTestMessStoreProcessUtil::ValidCharacterSetInfo(const TPtrC& aCharacterSetType, TUint &aCharSetVal)
	{
	TBool	hasSet = ETrue;

	if ( aCharacterSetType.Compare(KChineseGB2312) == 0 )
		{
		iTestStep.INFO_PRINTF1(_L("Character set set to Chinese GB2312"));
		aCharSetVal =  KCharacterSetIdentifierGb2312;
		}
	else if (aCharacterSetType.Compare(KChineseHz) == 0)
		{
		iTestStep.INFO_PRINTF1(_L("Character set set to Chinese Hz"));
		aCharSetVal = KCharacterSetIdentifierHz;
		}
	else if ( aCharacterSetType.Compare(KChineseBig5) == 0 )
		{
		iTestStep.INFO_PRINTF1(_L("Character set set to Chinese Big5"));
		aCharSetVal = KCharacterSetIdentifierBig5;
		}
	else
		{
		// To be updated later
		hasSet= EFalse;
		iTestStep.ERR_PRINTF2(_L("Character not supported in test code: %S"), &aCharacterSetType);
		iTestStep.SetTestStepResult(EFail);
		}

	return hasSet;
	}
