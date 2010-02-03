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
// This contains CTestCase which is the base class for all the TestCase DLLs
// 
//

#include "Configuration.cfg"

#include "TestMessBase.h"
#include "TestMessProcessor.h"
#include "TestMessTimer.h"
#include "TestMessPrintEntryUtil.h"

// EPOC includes
#include <SmtpSet.h>
#include <Pop3Set.h>
#include <ImapSet.h>
#include <txtrich.h>
#include <f32file.h>
#include <msvids.h>
#include <fbs.h>
#include <EMSUserPromptIE.h>
#ifdef __MESSAGING_API_V2__
#include <smut.h>
#include <miutset.h>
#include <cemailaccounts.h>
#include <csmsaccount.h>
#endif
/**
 * @name Constant Literals used.
 */
/*@{*/
_LIT(KDepth,					"depth");
_LIT(KFolder,					"folder%d");

/// Default value for depth of the count
#define KDefaultDepthCount		1

/// Default value for depth of the count
#define KDefaultDepthCount		1

_LIT(KDefault,					"default");

_LIT(KScheduled,				"scheduled");
_LIT(KProgress,					"progress");

//_LIT(KIap,						"iap");
//_LIT(KIapName,					"iapname");

_LIT(KElements,					"elements");
_LIT(KStartPosition,			"StartPosition%d");
_LIT(KFile,						"File%d");
_LIT(KType,						"Type%d");
_LIT(KTypeFormat,				"Format");
_LIT(KTypeSound,				"Sound");
_LIT(KTypePreDefSound,			"PreDefSound");
_LIT(KTypeAnimation,			"Animation");
_LIT(KTypePreDefAnimation,		"PreDefAnimation");
_LIT(KTypePicture,				"Picture");
_LIT(KTypeUserPrompt,			"UserPrompt");
_LIT(KFormatLength,				"FormatLength%d");
_LIT(KBold,						"Bold%d");
_LIT(KItalic,					"Italic%d");
_LIT(KUnderline,				"Underline%d");
_LIT(KStrikethrough,			"Strikethrough%d");
_LIT(KAlignment,				"Alignment%d");
_LIT(KFontSize,					"FontSize%d");

_LIT(KLeftAlign,				"LeftAlign");
_LIT(KRightAlign,				"RightAlign");
_LIT(KCenterAlign,				"CenterAlign");
_LIT(KLangDepend,				"LangDepend");

_LIT(KNormal,					"Normal");
_LIT(KLarge,					"Large");
_LIT(KSmall,					"Small");
_LIT(KVariable,					"Variable");
_LIT(KReserved,					"Reserved");

_LIT(KEmsPreDef,				"emspredef%d");

_LIT(KEmsAnimTypeFlirty,		"a0");
_LIT(KEmsAnimTypeGlad,			"a1");
_LIT(KEmsAnimTypeSceptic,		"a2");
_LIT(KEmsAnimTypeSad,			"a3");
_LIT(KEmsAnimTypeWow,			"a4");
_LIT(KEmsAnimTypeCrying,		"a5");
_LIT(KEmsAnimTypeWinking,		"a6");
_LIT(KEmsAnimTypeLaughing,		"a7");
_LIT(KEmsAnimTypeIndifferent,	"a8");
_LIT(KEmsAnimTypeKissing,		"a9");
_LIT(KEmsAnimTypeConfused,		"a10");
_LIT(KEmsAnimTypeTongueOut,		"a11");
_LIT(KEmsAnimTypeAngry,			"a12");
_LIT(KEmsAnimTypeGlasses,		"a13");
_LIT(KEmsAnimTypeDevil,			"a14");

_LIT(KEmsSoundTypeChimesHigh,	"s0");
_LIT(KEmsSoundTypeChimesLow,	"s1");
_LIT(KEmsSoundTypeDing,			"s2");
_LIT(KEmsSoundTypeTaDa,			"s3");
_LIT(KEmsSoundTypeNotify,		"s4");
_LIT(KEmsSoundTypeDrum,			"s5");
_LIT(KEmsSoundTypeClaps,		"s6");
_LIT(KEmsSoundTypeFanFar,		"s7");
_LIT(KEmsSoundTypeChordHigh,	"s8");
_LIT(KEmsSoundTypeChordLow,		"s9");

/*
_LIT(KBioIAP,					"IAP");
_LIT(KBioEmailNotification,		"EmailNotification");
_LIT(KBioBusinessCard,			"BusinessCard");
_LIT(KBioWAPAccessPoint,		"WAPAccessPoint");
_LIT(KBioVCalendar,				"VCalendar");
_LIT(KBioVCard,					"VCard");
_LIT(KBioRingingTones,			"RingingTones");
_LIT(KBioOperatorLogo,			"OperatorLogo");
_LIT(KBioWPRV,					"WPRV");
_LIT(KBioCLILogo,				"CLILogo");
*/
_LIT(KObjectSize,				"ObjectSize%d");
_LIT(KBody,						"body");
/*@}*/

const TInt KSmallObject		= 0;
const TInt KLargeObject		= 1;
const TInt KVariableObject	= 2;

const TInt KDefaultStartPos	= 1;

/**
 * Constructor.
 * @return - None
 * 
*/	
CTestMessBase::CTestMessBase(TBool aSetDirectory)
:	CTestStepSmokeTest()
,	iSession(NULL)
,	iMessageProcessor(NULL)
,   iParaFormatLayer(NULL)
,   iCharFormatLayer(NULL)
,   iBodyText(NULL)
,	iBodyAvailable(EFalse)
,	iSetDirectory(aSetDirectory)
,	iMessageTimer(NULL)
,	iSchedular(NULL)
,	iEntry(NULL)
,	iOrder(KMsvGroupByType|KMsvGroupByStandardFolders, EMsvSortById, ETrue)
	{
	}

CTestMessBase::~CTestMessBase()
	{
	delete iEntry;
	iEntry=NULL;
	delete iMessageTimer;
	iMessageTimer=NULL;
	delete iMessageProcessor;
	iMessageProcessor=NULL;
	delete iSession;
	iSession=NULL;
	delete iSchedular;
	iSchedular=NULL;
	delete iBodyText;
	iBodyText=NULL;
	delete iCharFormatLayer;
	iCharFormatLayer=NULL;
	delete iParaFormatLayer;
	iParaFormatLayer=NULL;
	}

enum TVerdict CTestMessBase::doTestStepPreambleL()
	{
	TVerdict	ret=CTestStepSmokeTest::doTestStepPreambleL();

	// Printing to the console and log file
	const TDesC&	stepName=TestStepName();
	INFO_PRINTF2(_L("%S"), &stepName);

	iSchedular=new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iSchedular);
	iSession=CMsvSession::OpenSyncL(*this);

	//	Get progress
	TInt	progress=0;
	TBool	hasProgress=GetIntFromConfig(ConfigSection(), KProgress, progress);
	if ( !hasProgress )
		{
		hasProgress=GetIntFromConfig(KDefault, KProgress, progress);
		}

	iMessageProcessor=CTestMessProcessor::NewL(*this, *iSession);
	if ( hasProgress )
		{
		iMessageTimer=CTestMessTimer::NewL(progress*KOneSecond, *iMessageProcessor);
		}

	if ( iSetDirectory )
		{
		TInt	depth=KDefaultDepthCount;
		TBool	returnValue=GetIntFromConfig(ConfigSection(), KDepth, depth);
		if ( !returnValue )
			{
			depth=KDefaultDepthCount;
			}
		INFO_PRINTF2(_L("Depth = %d"), depth);

		//	Searce for the entry
		TRAPD(error,SetEntryL(KMsvRootIndexEntryId));
		if( error != KErrNone)
			{
			INFO_PRINTF2(_L("Set Entry failed, Error %D"),error);
			SetTestStepError(error);
			ret=EFail;
			}
		else
			{

			TBuf<KMaxTestExecuteCommandLength>	tempStore;
			TBool								found=ETrue;
			for (TInt entry=0; entry<depth && found;)
				{
				TPtrC	subject;
				tempStore.Format(KFolder(), ++entry);
				returnValue =GetStringFromConfig(ConfigSection(), tempStore, subject);
				if ( !returnValue )
					{
					INFO_PRINTF1(_L("Null Folder"));
					}
				INFO_PRINTF2(_L("Folder = %S"), &subject);

				CMsvEntrySelection*	selection=EntryL().ChildrenL();
				CleanupStack::PushL(selection);
				TInt				count=selection->Count();
		
				found=EFalse;
				for (TInt i=count; i>0 && !found; )
					{
					TMsvId	childId=selection->At(--i);

					SetEntryL(childId);
					INFO_PRINTF2(_L("Entry Detail : %S"), &(EntryL().Entry().iDetails));
					INFO_PRINTF2(_L("Entry Description: %S"), &(EntryL().Entry().iDescription));
					if ( subject.Compare(EntryL().Entry().iDetails) == 0 ||
						 subject.Compare(EntryL().Entry().iDescription) == 0 )
						{
						found=ETrue;
						}
					}
				CleanupStack::PopAndDestroy(selection);
				}

			if ( !found )
				{
				ERR_PRINTF1(_L("Folder not found"));
				ret=EFail;
				}
			}
		}
	if (TestStepResult() == EPass)
		{
		//Get body(file path) from ini file and read respective file. Store it into CRichText format.
		iParaFormatLayer=CParaFormatLayer::NewL();
		iCharFormatLayer=CCharFormatLayer::NewL(); 
		iBodyText=CRichText::NewL(iParaFormatLayer, iCharFormatLayer);
		iBodyAvailable=EFalse;
	
		TPtrC	body;
		if(GetStringFromConfig(ConfigSection(), KBody, body))
			{
			INFO_PRINTF2(_L("Body = %S"), &body);
			iBodyAvailable=ETrue;
			}
		else
			{
			INFO_PRINTF1(_L("No Body"));
			}

		if ( iBodyAvailable )
			{
			// open the file
			RFile	file;
	
			TInt anError = file.Open(iSession->FileSession(), body, EFileShareAny);
			if(anError == KErrNone)
				{
				TBool		reading=ETrue;
				TInt		pos=0;
				TBuf8<1>	charIn;
				while (reading)
					{
					file.Read(charIn, 1);
					if(charIn.Length())
						{
						switch ( charIn[0] )
							{
						case '\r':
							iBodyText->InsertL(pos++, CEditableText::ELineBreak);
						case '\n':
							break;
						default:
							iBodyText->InsertL(pos++, TChar(charIn[0]));
							}
						}
					else
						{
						reading = EFalse;	// stop at the end of the file
						}
					}
				iBodyAvailable=ETrue;
				file.Close();
				}
			else
			{
			INFO_PRINTF2(_L("Cannot open file = %S"), &body);
			iBodyAvailable=EFalse;
			}
		}//End of read file.
	}

	SetTestStepResult(ret);
	return ret;
	}

enum TVerdict CTestMessBase::doTestStepPostambleL()
	{
	delete iEntry;
	iEntry=NULL;
	delete iMessageTimer;
	iMessageTimer=NULL;
	delete iMessageProcessor;
	iMessageProcessor=NULL;
	delete iSession;
	iSession=NULL;
	delete iSchedular;
	iSchedular=NULL;
	CActiveScheduler::Install(NULL);
	return CTestStepSmokeTest::doTestStepPostambleL();
	}

void CTestMessBase::SetEntryL(TMsvId aId)
	{
	delete iEntry;
	iEntry=NULL;
	iEntry=iSession->GetEntryL(aId);
	}

CMsvEntry& CTestMessBase::EntryL()
	{
	if ( iEntry == NULL )
		{
		User::Leave(KErrGeneral);
		}

	return *iEntry;
	}


HBufC8* CTestMessBase::CopyToBuf8LC(const TDesC& aDesC)
//
// @return - HBufC8*
// Converts TDesC to HBufC8*.
//
	{
	HBufC8*	itemBuff8 = HBufC8::NewLC(aDesC.Length());
	itemBuff8->Des().Copy(aDesC);

	return itemBuff8;
	}

HBufC* CTestMessBase::CopyToBufLC(const TDesC8& aDesC)
//
// @return - HBufC8*
// Converts TDesC to HBufC8*.
//
	{
	HBufC*	itemBuff = HBufC::NewLC(aDesC.Length());
	itemBuff->Des().Copy(aDesC);

	return itemBuff;
	}


#ifndef __MESSAGING_API_V2__
void CTestMessBase::SetDefaultServiceL(TUid aMtm, TMsvId aService) const
	{
	// --- Now restore the default services from the root store ---
	CMsvEntry*				rootEntry=iSession->GetEntryL(KMsvRootIndexEntryIdValue);
	CleanupStack::PushL(rootEntry);
	CMsvStore*				store = rootEntry->EditStoreL();
	CleanupStack::PushL(store);
	CMsvDefaultServices*	services = new(ELeave)CMsvDefaultServices;
	CleanupStack::PushL(services);
	services->RestoreL(*store);

	// --- And now re-set the default service for the MTM to the one given ---
	TMsvDefaultService	defaultService;
	defaultService.iMtm=aMtm;
	defaultService.iService=aService;
	services->ChangeDefaultServiceL(defaultService);
	services->StoreL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy(3, rootEntry);
	}

TInt CTestMessBase::DefaultServiceL(TUid aMtm, TMsvId& aService) const
	{
	// --- Now restore the default services from the root store ---
	CMsvEntry*				rootEntry=iSession->GetEntryL(KMsvRootIndexEntryIdValue);
	CleanupStack::PushL(rootEntry);
	CMsvStore*				store=rootEntry->ReadStoreL();
	CleanupStack::PushL(store);
	CMsvDefaultServices*	services=new(ELeave)CMsvDefaultServices;
	CleanupStack::PushL(services);
	services->RestoreL(*store);

	TInt					ret=services->DefaultService(aMtm, aService);

	CleanupStack::PopAndDestroy(3, rootEntry);

	return ret;
	}
#endif

void CTestMessBase::PrintTreeL(TMsvId aRoot)
	{
	INFO_PRINTF1(_L("PrintTreeL In"));
	CMsvEntry*			entry=iSession->GetEntryL(aRoot);
	CleanupStack::PushL(entry);
	entry->SetSortTypeL(SelectionOrdering());
	CMsvEntrySelection*	selection=entry->ChildrenL();
	CleanupStack::PushL(selection);

	PrintEntryL(*entry);
	for (TInt i=0; i<selection->Count(); )
		{
		PrintTreeL(selection->At(i++));
		}

	CleanupStack::PopAndDestroy(2, entry);
	INFO_PRINTF1(_L("PrintTreeL Out"));
	}

void CTestMessBase::PrintEntryL(CMsvEntry& aEntry)
	{
	TMsvEntry	entry=aEntry.Entry();

	INFO_PRINTF2(_L("Entry Detail : %S"), &(entry.iDetails));
	INFO_PRINTF2(_L("  Description: %S"), &(entry.iDescription));
	INFO_PRINTF2(_L("  Id         : %x"), entry.Id());
	INFO_PRINTF2(_L("  Parent Id  : %x"), entry.Parent());
	INFO_PRINTF2(_L("  Service Id : %x"), entry.iServiceId);
	if ( entry.iRelatedId != 0 )
		{
		INFO_PRINTF2(_L("  Related Id : %x"), entry.iRelatedId);
		}
	INFO_PRINTF2(_L("  Type       : %x"), entry.iType);
	INFO_PRINTF2(_L("  Mtm        : %x"), entry.iMtm);


	if ( entry.iSize != 0 )
		{
		INFO_PRINTF2(_L("  Size       : %x"), entry.iSize);
		}
	if ( entry.iError != 0 )
		{
		INFO_PRINTF2(_L("  Error      : %d"), entry.iError);
		}
	if ( entry.iBioType != 0 )
		{
		INFO_PRINTF2(_L("  BioType    : %x"), entry.iBioType);
		}
	if ( entry.MtmData1() != 0 )
		{
		INFO_PRINTF2(_L("  MtmData1   : %x"), entry.MtmData1());
		}
	if ( entry.MtmData2() != 0 )
		{
		INFO_PRINTF2(_L("  MtmData2   : %x"), entry.MtmData2());
		}
	if ( entry.MtmData3() != 0 )
		{
		INFO_PRINTF2(_L("  MtmData3   : %x"), entry.MtmData3());
		}
	if ( entry.Attachment() != 0 )
		{
		INFO_PRINTF2(_L("  Attachment : %x"), entry.Attachment());
		}
	if ( entry.Complete() != 0 )
		{
		INFO_PRINTF2(_L("  Complete   : %x"), entry.Complete());
		}
	if ( entry.Connected() != 0 )
		{
		INFO_PRINTF2(_L("  Connected  : %x"), entry.Connected());
		}
	if ( entry.Deleted() != 0 )
		{
		INFO_PRINTF2(_L("  Deleted    : %x"), entry.Deleted());
		}
	if ( entry.Failed() != 0 )
		{
		INFO_PRINTF2(_L("  Failed     : %x"), entry.Failed());
		}
	if ( entry.InPreparation() != 0 )
		{
		INFO_PRINTF2(_L("  InPrep     : %x"), entry.InPreparation());
		}
	if ( entry.MultipleRecipients() != 0 )
		{
		INFO_PRINTF2(_L("  MultiRecip : %x"), entry.MultipleRecipients());
		}
	if ( entry.OffPeak() != 0 )
		{
		INFO_PRINTF2(_L("  OffPeak    : %x"), entry.OffPeak());
		}
	if ( entry.Operation() != 0 )
		{
		INFO_PRINTF2(_L("  Operation  : %x"), entry.Operation());
		}
	if ( entry.Owner() != 0 )
		{
		INFO_PRINTF2(_L("  Owner      : %x"), entry.Owner());
		}
	if ( entry.PcSyncCount() != 0 )
		{
		INFO_PRINTF2(_L("  PcSyncCount: %x"), entry.PcSyncCount());
		}
	if ( entry.Priority() != 0 )
		{
		INFO_PRINTF2(_L("  Priority   : %x"), entry.Priority());
		}
	if ( entry.ReadOnly() != 0 )
		{
		INFO_PRINTF2(_L("  ReadOnly   : %x"), entry.ReadOnly());
		}
	if ( entry.Scheduled() != 0 )
		{
		INFO_PRINTF2(_L("  Scheduled  : %x"), entry.Scheduled());
		}
	if ( entry.SendingState() != 0 )
		{
		INFO_PRINTF2(_L("  Send State : %x"), entry.SendingState());
		}
	if ( entry.StandardFolder() != 0 )
		{
		INFO_PRINTF2(_L("  StdFolder  : %x"), entry.StandardFolder());
		}
	if ( entry.Unread() != 0 )
		{
		INFO_PRINTF2(_L("  Unread     : %x"), entry.Unread());
		}
	if ( entry.Visible() != 0 )
		{
		INFO_PRINTF2(_L("  Visible    : %x"), entry.Visible());
		}

#if (!defined CDMA_API_ENABLED)
	if ( aEntry.HasStoreL() )
		{
		PrintStoreL(aEntry);
		}
#endif
	}

void CTestMessBase::PrintStoreL(CMsvEntry& aEntry)
	{
	TMsvEntry					entry=aEntry.Entry();
	CTestMessPrintEntryUtil*	printUtil=new (ELeave) CTestMessPrintEntryUtil(*this);
	CleanupStack::PushL(printUtil);

	printUtil->ProcessEntryL(entry.Id());
	CleanupStack::PopAndDestroy(printUtil);

	if ( entry.iMtm==KUidMsgTypeSMTP )
		{
		PrintStoreSmtpL(aEntry);
		}
	else if ( entry.iMtm == KUidMsgTypePOP3 )
		{
		PrintStorePop3L(aEntry);
		}
	else if ( entry.iMtm == KUidMsgTypeIMAP4 )
		{
		PrintStoreImap4L(aEntry);
		}
	else if ( entry.iMtm == KUidMsgTypeSMS )
		{
		PrintStoreSmsL(aEntry);
		}
	}

void CTestMessBase::PrintStoreSmtpL(CMsvEntry& aEntry)
	{
	TMsvEntry	entry=aEntry.Entry();
	CMsvStore*	store=aEntry.ReadStoreL();
	CleanupStack::PushL(store);

	TPtrC	ptrTemp;
	if ( entry.iType == KUidMsvServiceEntry )
		{
		CImSmtpSettings*	settings = new(ELeave) CImSmtpSettings();
		CleanupStack::PushL(settings);
#ifdef __MESSAGING_API_V2__
		CEmailAccounts*		accounts = CEmailAccounts::NewLC();
		TSmtpAccount smtpAccountId;
		accounts->GetSmtpAccountL(aEntry.EntryId(), smtpAccountId);
		TRAPD(err, accounts->LoadSmtpSettingsL(smtpAccountId, *settings));
		CleanupStack::PopAndDestroy(accounts);
#else
		TRAPD(err, settings->RestoreL(*store));
#endif
		if ( err == KErrNone )
			{
			ptrTemp.Set(settings->ServerAddress());
			INFO_PRINTF2(_L("  ServerAddress                : %S"), &ptrTemp);
#ifndef __MESSAGING_API_V2__
			ptrTemp.Set(settings->UserAddress());
			INFO_PRINTF2(_L("  UserAddress                  : %S"), &ptrTemp);
			INFO_PRINTF2(_L("  Version                      : %x"), (TUint)settings->Version());
#endif
			INFO_PRINTF2(_L("  Port                         : %x"), settings->Port());
			INFO_PRINTF2(_L("  SecureSockets                : %x"), (TUint)settings->SecureSockets());
			ptrTemp.Set(settings->EmailAlias());
			INFO_PRINTF2(_L("  EmailAlias                   : %S"), &ptrTemp);
			ptrTemp.Set(settings->EmailAddress());
			INFO_PRINTF2(_L("  EmailAddress                 : %S"), &ptrTemp);
			ptrTemp.Set(settings->ReplyToAddress());
			INFO_PRINTF2(_L("  ReplyToAddress               : %S"), &ptrTemp);
			ptrTemp.Set(settings->ReceiptAddress());
			INFO_PRINTF2(_L("  ReceiptAddress               : %S"), &ptrTemp);
			if ( settings->RequestReceipts() != 0 )
				{
				INFO_PRINTF2(_L("  RequestReceipts              : %x"), (TUint)settings->RequestReceipts());
				}
			if ( settings->SendCopyToSelf() != 0 )
				{
				INFO_PRINTF2(_L("  SendCopyToSelf               : %x"), (TUint)settings->SendCopyToSelf());
				}
			INFO_PRINTF2(_L("  SendMessageOption            : %x"), (TUint)settings->SendMessageOption());
			}
		CleanupStack::PopAndDestroy(settings);
		}

	CleanupStack::PopAndDestroy(store);
	}

void CTestMessBase::PrintStorePop3L(CMsvEntry& aEntry)
	{
	TMsvEntry	entry=aEntry.Entry();
	CMsvStore*	store=aEntry.ReadStoreL();
	CleanupStack::PushL(store);

	TPtrC	ptrTemp;
	if ( entry.iType == KUidMsvServiceEntry )
		{
		CImPop3Settings*	settings = new(ELeave) CImPop3Settings();
		CleanupStack::PushL(settings);

#ifdef __MESSAGING_API_V2__
		CEmailAccounts*		accounts = CEmailAccounts::NewLC();
		TPopAccount popAccountId;
		accounts->GetPopAccountL(aEntry.EntryId(), popAccountId);
		TRAPD(err, accounts->LoadPopSettingsL(popAccountId, *settings));
		CleanupStack::PopAndDestroy(accounts);
#else
		TRAPD(err, settings->RestoreL(*store));
#endif
		if ( err == KErrNone )
			{
			ptrTemp.Set(settings->ServerAddress());
			INFO_PRINTF2(_L("  ServerAddress                : %S"), &ptrTemp);
#ifndef __MESSAGING_API_V2__
			ptrTemp.Set(settings->UserAddress());
			INFO_PRINTF2(_L("  UserAddress                  : %S"), &ptrTemp);
			INFO_PRINTF2(_L("  Version                      : %x"), (TUint)settings->Version());
#endif
			INFO_PRINTF2(_L("  Port                         : %x"), settings->Port());
			INFO_PRINTF2(_L("  SecureSockets                : %x"), (TUint)settings->SecureSockets());
			HBufC*	loginName=CopyToBufLC(settings->LoginName());
			INFO_PRINTF2(_L("  LoginName                    : %S"), loginName);
			CleanupStack::PopAndDestroy(loginName);
			if ( settings->AutoSendOnConnect() != 0 )
				{
				INFO_PRINTF2(_L("  AutoSendOnConnect            : %x"), (TUint)settings->AutoSendOnConnect());
				}
			if ( settings->Apop() != 0 )
				{
				INFO_PRINTF2(_L("  Apop                         : %x"), (TUint)settings->Apop());
				}
			if ( settings->DisconnectedUserMode() != 0 )
				{
				INFO_PRINTF2(_L("  DisconnectedUserMode         : %x"), (TUint)settings->DisconnectedUserMode());
				}
			if ( settings->DeleteEmailsWhenDisconnecting() != 0 )
				{
				INFO_PRINTF2(_L("  DeleteEmailsWhenDisconnecting: %x"), (TUint)settings->DeleteEmailsWhenDisconnecting());
				}
			if ( settings->AcknowledgeReceipts() != 0 )
				{
				INFO_PRINTF2(_L("  AcknowledgeReceipts          : %x"), (TUint)settings->AcknowledgeReceipts());
				}
			if ( settings->MaxEmailSize() != 0 )
				{
				INFO_PRINTF2(_L("  MaxEmailSize                 : %x"), (TUint)settings->MaxEmailSize());
				}
			if ( settings->GetMailOptions() != 0 )
				{
				INFO_PRINTF2(_L("  GetMailOptions               : %x"), (TUint)settings->GetMailOptions());
				}
			if ( settings->InboxSynchronisationLimit() != 0 )
				{
				INFO_PRINTF2(_L("  InboxSynchronisationLimit    : %x"), (TUint)settings->InboxSynchronisationLimit());
				}
			}
		CleanupStack::PopAndDestroy(settings);
		}

	CleanupStack::PopAndDestroy(store);
	}

void CTestMessBase::PrintStoreImap4L(CMsvEntry& aEntry)
	{
	TMsvEntry	entry=aEntry.Entry();
	CMsvStore*	store=aEntry.ReadStoreL();
	CleanupStack::PushL(store);

	TPtrC	ptrTemp;
	if ( entry.iType == KUidMsvServiceEntry )
		{
		CImImap4Settings*	settings = new(ELeave) CImImap4Settings();
		CleanupStack::PushL(settings);

#ifdef __MESSAGING_API_V2__
		CEmailAccounts*		accounts = CEmailAccounts::NewLC();
		TImapAccount imapAccountId;
		accounts->GetImapAccountL(aEntry.EntryId(), imapAccountId);
		TRAPD(err, accounts->LoadImapSettingsL(imapAccountId, *settings));
		CleanupStack::PopAndDestroy(accounts);
#else
		TRAPD(err, settings->RestoreL(*store));
#endif
		if ( err == KErrNone )
			{
			ptrTemp.Set(settings->ServerAddress());
			INFO_PRINTF2(_L("  ServerAddress                : %S"), &ptrTemp);
#ifndef __MESSAGING_API_V2__
			ptrTemp.Set(settings->UserAddress());
			INFO_PRINTF2(_L("  UserAddress                  : %S"), &ptrTemp);
			INFO_PRINTF2(_L("  Version                      : %x"), (TUint)settings->Version());
#endif
			INFO_PRINTF2(_L("  Port                         : %x"), settings->Port());
			INFO_PRINTF2(_L("  SecureSockets                : %x"), (TUint)settings->SecureSockets());

			HBufC*	loginName=CopyToBufLC(settings->LoginName());
			INFO_PRINTF2(_L("  LoginName                    : %S"), loginName);
			HBufC*	folderPath=CopyToBufLC(settings->FolderPath());
			INFO_PRINTF2(_L("  FolderPath                   : %S"), folderPath);
			CleanupStack::PopAndDestroy(2, loginName);
			if ( settings->PathSeparator() != 0 )
				{
				INFO_PRINTF2(_L("  PathSeparator                : %c"), (TText)settings->PathSeparator());
				}
			if ( settings->DisconnectedUserMode() != 0 )
				{
				INFO_PRINTF2(_L("  DisconnectedUserMode         : %x"), (TUint)settings->DisconnectedUserMode());
				}
			if ( settings->Synchronise() != 0 )
				{
				INFO_PRINTF2(_L("  Synchronise                  : %x"), (TUint)settings->Synchronise());
				}
			if ( settings->Subscribe() != 0 )
				{
				INFO_PRINTF2(_L("  Subscribe                    : %x"), (TUint)settings->Subscribe());
				}
			if ( settings->AutoSendOnConnect() != 0 )
				{
				INFO_PRINTF2(_L("  AutoSendOnConnect            : %x"), (TUint)settings->AutoSendOnConnect());
				}
			if ( settings->MaxEmailSize() != 0 )
				{
				INFO_PRINTF2(_L("  MaxEmailSize                 : %x"), (TUint)settings->MaxEmailSize());
				}
			if ( settings->DeleteEmailsWhenDisconnecting() != 0 )
				{
				INFO_PRINTF2(_L("  DeleteEmailsWhenDisconnecting: %x"), (TUint)settings->DeleteEmailsWhenDisconnecting());
				}
			if ( settings->AcknowledgeReceipts() != 0 )
				{
				INFO_PRINTF2(_L("  AcknowledgeReceipts          : %x"), (TUint)settings->AcknowledgeReceipts());
				}
			if ( settings->GetMailOptions() != 0 )
				{
				INFO_PRINTF2(_L("  GetMailOptions               : %x"), (TUint)settings->GetMailOptions());
				}
			if ( settings->InboxSynchronisationLimit() != 0 )
				{
				INFO_PRINTF2(_L("  InboxSynchronisationLimit    : %x"), (TUint)settings->InboxSynchronisationLimit());
				}
			if ( settings->MailboxSynchronisationLimit() != 0 )
				{
				INFO_PRINTF2(_L("  MailboxSynchronisationLimit  : %x"), (TUint)settings->MailboxSynchronisationLimit());
				}
			}
		CleanupStack::PopAndDestroy(settings);
		}

	CleanupStack::PopAndDestroy(store);
	}

void CTestMessBase::PrintStoreSmsL(CMsvEntry& aEntry)
	{
	TMsvEntry	entry=aEntry.Entry();
	CMsvStore*	store=aEntry.ReadStoreL();
	CleanupStack::PushL(store);

	TPtrC	ptrTemp;
	if ( entry.iType == KUidMsvServiceEntry )
		{
		CSmsSettings*	settings = CSmsSettings::NewL();
		CleanupStack::PushL(settings);

#ifdef __MESSAGING_API_V2__
		CSmsAccount*		smsAccount = CSmsAccount::NewLC();
		TRAPD(err, smsAccount->LoadSettingsL(*settings));
		CleanupStack::PopAndDestroy(smsAccount);
#else
		TRAPD(err, settings->RestoreL(*store));
#endif
		if ( err == KErrNone )
			{
			TSmsDelivery	delivery(settings->Delivery());
			INFO_PRINTF2(_L("  Delivery                     : %d"), delivery);
			CSmsSettings::TSmsReportHandling	reportHandling(settings->StatusReportHandling());
			INFO_PRINTF2(_L("  StatusReportHandling         : %d"), reportHandling);
			CSmsSettings::TSmsReportHandling	specialMsgHandling(settings->SpecialMessageHandling());
			INFO_PRINTF2(_L("  SpecialMessageHandling       : %d"), specialMsgHandling);
			INFO_PRINTF2(_L("  ReplyQuoted                  : %d"), settings->ReplyQuoted());
#ifdef __MESSAGING_API_V2__
			const TInt	numSCAddr = settings->ServiceCenterCount();
#else
			const TInt	numSCAddr = settings->NumSCAddresses();
#endif
			for( TInt index=0;index<numSCAddr;index++ )
				{
#ifdef __MESSAGING_API_V2__
				CSmsServiceCenter&	scAddr = settings->GetServiceCenter(index);
#else
				CSmsNumber&			scAddr = settings->SCAddress(index);
#endif
				ptrTemp.Set(scAddr.Name());
				INFO_PRINTF2(_L("  SC Name                      : %S"), &ptrTemp);
				ptrTemp.Set(scAddr.Address());
				INFO_PRINTF2(_L("  SC Address                   : %S"), &ptrTemp);
				}
			CSmsSettings::TSmsSettingsCommDbAction	commDbAction(settings->SmsBearerAction());
			INFO_PRINTF2(_L("  SmsBearerAction              : %d"), commDbAction);

			CSmsSettings::TMobileSmsBearer	smsBearer(settings->SmsBearer());
			INFO_PRINTF2(_L("  SmsBearer                    : %d"), smsBearer);
			}
		CleanupStack::PopAndDestroy(settings);
		}

	CleanupStack::PopAndDestroy(store);
	}

void CTestMessBase::IssueTimerRequest()
	{
	if (iMessageTimer!= NULL)
		{
		iMessageTimer->IssueRequest();
		}
	}

void CTestMessBase::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny*, TAny*)
/**
 Handler for the Session Event
 @param aEvent - TMsvSessionEvent object
 @param aArg1 - TAny object pointer, used to store the argument for the event
 @leave - System wide error codes
*/
	{
	TBool	printSelection=EFalse;

	switch (aEvent)
		{
	case EMsvEntriesCreated:
		INFO_PRINTF1(_L("EMsvEntriesCreated"));
		printSelection=ETrue;
		break;
	case EMsvEntriesChanged:
		INFO_PRINTF1(_L("EMsvEntriesChanged"));
		printSelection=ETrue;
		break;
	case EMsvEntriesDeleted:
		INFO_PRINTF1(_L("EMsvEntriesDeleted"));
		break;
	case EMsvEntriesMoved:
		INFO_PRINTF1(_L("EMsvEntriesMoved"));
		printSelection=ETrue;
		break;
	case EMsvMtmGroupInstalled:
		INFO_PRINTF1(_L("EMsvMtmGroupInstalled"));
		break;
	case EMsvMtmGroupDeInstalled:
		INFO_PRINTF1(_L("EMsvMtmGroupDeInstalled"));
		break;
	case EMsvGeneralError:
		INFO_PRINTF1(_L("EMsvGeneralError"));
		break;
	case EMsvCloseSession:
		INFO_PRINTF1(_L("EMsvCloseSession"));
		break;
	case EMsvServerReady:
		INFO_PRINTF1(_L("EMsvServerReady"));
		break;
	case EMsvServerFailedToStart:
		INFO_PRINTF1(_L("EMsvServerFailedToStart"));
		break;
	case EMsvCorruptedIndexRebuilt:
		INFO_PRINTF1(_L("EMsvCorruptedIndexRebuilt"));
		break;
	case EMsvServerTerminated:
		INFO_PRINTF1(_L("EMsvServerTerminated"));
		break;
	case EMsvMediaChanged:
		INFO_PRINTF1(_L("EMsvMediaChanged"));
		break;
	case EMsvMediaUnavailable:
		INFO_PRINTF1(_L("EMsvMediaUnavailable"));
		break;
	case EMsvMediaAvailable:
		INFO_PRINTF1(_L("EMsvMediaAvailable"));
		break;
	case EMsvMediaIncorrect:
		INFO_PRINTF1(_L("EMsvMediaIncorrect"));
		break;
	case EMsvCorruptedIndexRebuilding:
		INFO_PRINTF1(_L("EMsvCorruptedIndexRebuilding"));
		break;
	default:
		INFO_PRINTF2(_L("EMsv Unknown event:%d"), aEvent);
		break;
	}

	if ( printSelection )
		{
		CMsvEntrySelection*	selection=STATIC_CAST(CMsvEntrySelection*, aArg1);
		CMsvEntry*			entry=NULL;
		for ( int i=0; i<selection->Count(); )
			{
			entry=iSession->GetEntryL(selection->At(i++));
			CleanupStack::PushL(entry);
			PrintEntryL(*entry);
			CleanupStack::PopAndDestroy(entry);
			}
		}
	}


void CTestMessBase::PrepareSmsHeaderFromIniFileL(CSmsHeader& aSmsHeader)
	{
	// Starts the Bitmap server
	FbsStartup();
	// start the server
	RFbsSession::Connect();

	// Add the Fromating required for EMS if required
	TInt	emsElements=0;
	if ( !GetIntFromConfig(ConfigSection(), KElements, emsElements) )
		{
		ERR_PRINTF1(_L("Elements not set"));
		SetTestStepResult(EFail);
		}

// Macro usage for CDMA mtm
#if (defined CDMA_API_ENABLED)
	TMsvMessageSms&	message=aSmsHeader.SmsMessage();
#else
	CSmsMessage&	message=aSmsHeader.Message(); 
#endif

	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	for ( TInt element=0; element<emsElements && TestStepResult()==EPass;)
		{
		++element;
		INFO_PRINTF2(_L("==== PROCESSING ELEMENT %d ===="), element);
		//	Start position
		tempStore.Format(KStartPosition, element);
		TInt	startPosition=KDefaultStartPos;
		if ( GetIntFromConfig(ConfigSection(), tempStore, startPosition) )
			{
			INFO_PRINTF2(_L("Start position %d"), startPosition);
			}

		//	File
		tempStore.Format(KFile, element);
		TPtrC	file;
		TParse	fileName;
		TBool	hasFile=GetStringFromConfig(ConfigSection(), tempStore, file);
		if ( hasFile )
			{
			INFO_PRINTF2(_L("File %S"), &file);
			fileName.Set(file,NULL,NULL);
			}

		tempStore.Format(KType, element);
		TPtrC	type;
		if ( GetStringFromConfig(ConfigSection(), tempStore, type) )
			{
			if ( type.Compare(KTypeFormat) == 0 )
				{
				//	=== Process text formating Information Element ===
				CEmsFormatIE*	newElement=CEmsFormatIE::NewL();
				CleanupStack::PushL(newElement);
				newElement->SetStartPosition(startPosition);

				TInt			intTemp;
				TBool			boolTemp;
				TPtrC			alignment;
				TPtrC			fontSize;

				// Set the Format length and other information
				tempStore.Format(KFormatLength, element);
				if ( GetIntFromConfig(ConfigSection(), tempStore, intTemp) )
					{
					INFO_PRINTF2(_L("Format Length %d"), intTemp);
					newElement->SetFormatLength(intTemp);
					}
				tempStore.Format(KBold, element);
				if ( GetBoolFromConfig(ConfigSection(), tempStore, boolTemp) )
					{
					INFO_PRINTF2(_L("Bold %d"), boolTemp);
					newElement->SetBold(boolTemp);
					}
				tempStore.Format(KItalic, element);
				if ( GetBoolFromConfig(ConfigSection(), tempStore, boolTemp) )
					{
					INFO_PRINTF2(_L("Italic %d"), boolTemp);
				    newElement->SetItalic(boolTemp);
					}
				tempStore.Format(KUnderline, element);
				if ( GetBoolFromConfig(ConfigSection(), tempStore, boolTemp) )
					{
					INFO_PRINTF2(_L("Underline %d"), boolTemp);
					newElement->SetUnderline(boolTemp);
					}
				tempStore.Format(KStrikethrough, element);
				if ( GetBoolFromConfig(ConfigSection(), tempStore, boolTemp) )
					{
					INFO_PRINTF2(_L("Strikethrough %d"), boolTemp);
					newElement->SetStrikethrough(boolTemp);
					}
				tempStore.Format(KAlignment,element);
				if(GetStringFromConfig(ConfigSection(),tempStore,alignment))
					{
					SetAlignmentFormat(*newElement, alignment);
					}
				
				tempStore.Format(KFontSize,element);
				if(GetStringFromConfig(ConfigSection(),tempStore,fontSize))
					{
					//
					SetFontSizeFormat(*newElement, fontSize);
					}
			    message.AddEMSInformationElementL(*newElement); 
				CleanupStack::PopAndDestroy(newElement);
				}
			else if ( type.Compare(KTypeSound) == 0 )
				{
				//	=== Process sound Information Element ===
				if ( hasFile )
					{
					HBufC8*			buffFile=CopyToBuf8LC(fileName.FullName());
					CEmsSoundIE*	newElement=CEmsSoundIE::NewL(buffFile->Des());
					CleanupStack::PushL(newElement);
					newElement->SetStartPosition(startPosition); 
				    message.AddEMSInformationElementL(*newElement); 
					CleanupStack::PopAndDestroy(2, buffFile);
					}
				else
					{
					ERR_PRINTF1(_L("No Sound file name"));
					SetTestStepResult(EFail);
					}
				}
			else if ( type.Compare(KTypePreDefSound) == 0 )
				{
				//	=== Process predefined sound Information Element ===
				TPtrC	preDefSound;
				tempStore.Format(KEmsPreDef, element);
				if ( !GetStringFromConfig(ConfigSection(),tempStore, preDefSound))
					{
					ERR_PRINTF1(_L("Predefined sound not present"));
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF2(_L("Predefined sound %S"), &preDefSound);
					CEmsPreDefSoundIE*	newElement=NewPreDefSoundL(preDefSound);

					if ( newElement != NULL )
						{
						//Assign the predef animation element to newElement
						CleanupStack::PushL(newElement);
						newElement->SetStartPosition(startPosition);
					    message.AddEMSInformationElementL(*newElement); 
						CleanupStack::PopAndDestroy(newElement);
						}
					else
						{
						ERR_PRINTF1(_L("Unable to create predefined sound"));
						SetTestStepResult(EFail);
						}
					}
				}
			else if ( type.Compare(KTypeAnimation) == 0 )
				{
				//	=== Process animation Information Element ===
				if ( hasFile )
					{
					TPtrC	objectSize;

					tempStore.Format(KObjectSize, element);
					if ( !GetStringFromConfig(ConfigSection(),tempStore,objectSize) )
						{
						ERR_PRINTF1(_L("No size provided"));
						SetTestStepResult(EFail);
						}
					else
						{
						CFbsBitmap*	bmp=NewBitmapL(fileName, objectSize);
						if ( bmp == NULL )
							{
							ERR_PRINTF1(_L("Failed to Load Animation File. Error %d"));
							SetTestStepResult(EFail);
							}
						else
							{
							CleanupStack::PushL(bmp);
							CEmsAnimationIE*	newElement=CEmsAnimationIE::NewL(*bmp);
							CleanupStack::PushL(newElement);
							newElement->SetStartPosition(startPosition);
						    message.AddEMSInformationElementL(*newElement); 
							CleanupStack::PopAndDestroy(2, bmp);
							}
						}
					}
				else
					{
					ERR_PRINTF1(_L("No Animation file name"));
					SetTestStepResult(EFail);
					}
				}
			else if ( type.Compare(KTypePreDefAnimation) == 0 )
				{
				//	=== Process predefined animation Information Element ===
				TPtrC	preDefAnim;
				tempStore.Format(KEmsPreDef, element);
				if ( !GetStringFromConfig(ConfigSection(),tempStore, preDefAnim) )
					{
					ERR_PRINTF1(_L("Predefined animations not present"));
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF2(_L("Predefined animation %S"), &preDefAnim);
					CEmsPreDefAnimationIE*	newElement=NewPreDefAnimationL(preDefAnim);

					if ( newElement != NULL )
						{
						//Assign the predef animation element to newElement
						CleanupStack::PushL(newElement);
						newElement->SetStartPosition(startPosition);
					    message.AddEMSInformationElementL(*newElement); 
						CleanupStack::PopAndDestroy(newElement);
						}
					else
						{
						ERR_PRINTF1(_L("Unable to create predefined animation"));
						SetTestStepResult(EFail);
						}
					}
				}
			else if ( type.Compare(KTypePicture) == 0 )
				{
				//	=== Process picture Information Element ===
				if ( hasFile )
					{
					TPtrC	objectSize;

					tempStore.Format(KObjectSize, element);
					if ( !GetStringFromConfig(ConfigSection(),tempStore,objectSize) )
						{
						ERR_PRINTF1(_L("No size provided"));
						SetTestStepResult(EFail);
						}
					else
						{
						CFbsBitmap*	bmp=NewBitmapL(fileName, objectSize);
						if ( bmp == NULL )
							{
							ERR_PRINTF1(_L("Failed to Load Animation File. Error %d"));
							SetTestStepResult(EFail);
							}
						else
							{
							CleanupStack::PushL(bmp);
							CEmsPictureIE*	newElement=CEmsPictureIE::NewL(*bmp);
							CleanupStack::PushL(newElement);
							newElement->SetStartPosition(startPosition);
						    message.AddEMSInformationElementL(*newElement); 
							CleanupStack::PopAndDestroy(2, bmp);
							}
						}
					}
				else
					{
					ERR_PRINTF1(_L("No Picture file name"));
					SetTestStepResult(EFail);
					}
				}
			else if ( type.Compare(KTypeUserPrompt) == 0 )
				{
				//	=== Process user prompt Information Element ===
				CEmsUserPrompt*	newElement=CEmsUserPrompt::NewL(2);
				CleanupStack::PushL(newElement);
				newElement->SetStartPosition(startPosition);
			    message.AddEMSInformationElementL(*newElement); 
				CleanupStack::PopAndDestroy(newElement);
				}
			else
				{
				ERR_PRINTF2(_L("Unknown element type %S"), &type);
				SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("No type given"));
			SetTestStepResult(EFail);
			}
		}

	RFbsSession::Disconnect(); 
	}

/**
 This function reads the schedule time from ini file
 @return - returns the schedule time read from the ini file
*/
TTimeIntervalMinutes CTestMessBase::ReadScheduleTime()
	{
	// Read  the schedule time from ini file. Add this schedule time to current time.
	// This time will be set as the scheduled time for sending the SMS message from task scheduler
	// By default schedule time will be 1 min
	TInt	scheduled=0;
	GetIntFromConfig(ConfigSection(), KScheduled, scheduled);
	INFO_PRINTF2(_L("Scheduled Time in minutes is %d"), scheduled);
	return TTimeIntervalMinutes(scheduled);
	}

void CTestMessBase::SetAlignmentFormat(CEmsFormatIE& aEmsFormatIE, const TDesC& aAlignment)
	{
	//Sets the alignment to the text
	INFO_PRINTF2(_L("Alignment %S"), &aAlignment);
	if( aAlignment.Compare(KLeftAlign)== 0 )
		{
		aEmsFormatIE.SetAlignment(CEmsFormatIE::ELeft);
		}
	else if( aAlignment.Compare(KRightAlign)== 0 )
		{
		aEmsFormatIE.SetAlignment(CEmsFormatIE::ERight);
		}
	else if( aAlignment.Compare(KCenterAlign)== 0 )
		{
		aEmsFormatIE.SetAlignment(CEmsFormatIE::ECenter);
		}
	else if( aAlignment.Compare(KLangDepend)== 0 )
		{
		aEmsFormatIE.SetAlignment(CEmsFormatIE::ELangDepend);
		}
	else
		{
		ERR_PRINTF1(_L("Illegal alignment value"));
		SetTestStepResult(EFail);
		}
	}

void CTestMessBase::SetFontSizeFormat(CEmsFormatIE& aEmsFormatIE, const TDesC& aFontSize)
	{
	//Sets the specified font to the text 
	INFO_PRINTF2(_L("FontSize %S"), &aFontSize);
	if( aFontSize.Compare(KNormal)== 0 )
		{
		aEmsFormatIE.SetFontSize(CEmsFormatIE::ENormal);
		}
	else if( aFontSize.Compare(KLarge)== 0 )
		{
		aEmsFormatIE.SetFontSize(CEmsFormatIE::ELarge);
		}
	else if( aFontSize.Compare(KSmall)== 0 )
		{
		aEmsFormatIE.SetFontSize(CEmsFormatIE::ESmall);
		}
	else if( aFontSize.Compare(KReserved)== 0 )
		{
		aEmsFormatIE.SetFontSize(CEmsFormatIE::EReserved);
		}
	else
		{
		ERR_PRINTF1(_L("Illegal fonst size value"));
		SetTestStepResult(EFail);
		}
	}

CEmsPreDefAnimationIE* CTestMessBase::NewPreDefAnimationL(const TDesC& aPreDefType) 
//
// Attaches the required predef animation. 
// Arguments - aString - holds the TAnim type to be attached 
// Return Value - Returns a Pointer of CEmsPreDefAnimationIE 
//
	{
	//Initialize emsPreDef to NULL
	CEmsPreDefAnimationIE::TAnimType	animType=CEmsPreDefAnimationIE::EFlirty;
	TBool								validType=ETrue;

	//Set a value for emsPreDef 
	if ( aPreDefType.Compare(KEmsAnimTypeFlirty) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EFlirty;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeGlad) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EGlad;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeSceptic) == 0 )
		{
		animType = CEmsPreDefAnimationIE::ESceptic;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeSad) == 0 )
		{
		animType = CEmsPreDefAnimationIE::ESad;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeWow) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EWow;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeCrying) == 0 )
		{
		animType = CEmsPreDefAnimationIE::ECrying;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeWinking) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EWinking;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeLaughing) == 0 )
		{
		animType = CEmsPreDefAnimationIE::ELaughing;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeIndifferent) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EIndifferent;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeKissing) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EKissing;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeConfused) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EConfused;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeTongueOut) == 0 )
		{
		animType = CEmsPreDefAnimationIE::ETongueOut;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeAngry) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EAngry;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeGlasses) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EGlasses;
		}
	else if ( aPreDefType.Compare(KEmsAnimTypeDevil) == 0 )
		{
		animType = CEmsPreDefAnimationIE::EDevil;
		}
	else
		{
		validType=EFalse;
		}

	CEmsPreDefAnimationIE*	emsPreDef=NULL;
	if ( validType )
		{
		emsPreDef=CEmsPreDefAnimationIE::NewL(animType);
		}

	return emsPreDef;
	}

CEmsPreDefSoundIE* CTestMessBase::NewPreDefSoundL(const TDesC& aPreDefType) 
//
// Attaches the required predef animation. 
// Arguments - aString - holds the TAnim type to be attached 
// Return Value - Returns a Pointer of CEmsPreDefAnimationIE 
//
	{
	//Initialize emsPreDef to NULL
	CEmsPreDefSoundIE::TPredefinedSound	soundType=CEmsPreDefSoundIE::EChimesHigh;
	TBool								validType=ETrue;

	//Set a value for emsPreDef 
	if ( aPreDefType.Compare(KEmsSoundTypeChimesHigh) == 0 )
		{
		soundType = CEmsPreDefSoundIE::EChimesHigh;
		}
	else if ( aPreDefType.Compare(KEmsSoundTypeChimesLow) == 0 )
		{
		soundType = CEmsPreDefSoundIE::EChimesLow;
		}
	else if ( aPreDefType.Compare(KEmsSoundTypeDing) == 0 )
		{
		soundType = CEmsPreDefSoundIE::EDing;
		}
	else if ( aPreDefType.Compare(KEmsSoundTypeTaDa) == 0 )
		{
		soundType = CEmsPreDefSoundIE::ETaDa;
		}
	else if ( aPreDefType.Compare(KEmsSoundTypeNotify) == 0 )
		{
		soundType = CEmsPreDefSoundIE::ENotify;
		}
	else if ( aPreDefType.Compare(KEmsSoundTypeDrum) == 0 )
		{
		soundType = CEmsPreDefSoundIE::EDrum;
		}
	else if ( aPreDefType.Compare(KEmsSoundTypeClaps) == 0 )
		{
		soundType = CEmsPreDefSoundIE::EClaps;
		}
	else if ( aPreDefType.Compare(KEmsSoundTypeFanFar) == 0 )
		{
		soundType = CEmsPreDefSoundIE::EFanFar;
		}
	else if ( aPreDefType.Compare(KEmsSoundTypeChordHigh) == 0 )
		{
		soundType = CEmsPreDefSoundIE::EChordHigh;
		}
	else if ( aPreDefType.Compare(KEmsSoundTypeChordLow) == 0 )
		{
		soundType = CEmsPreDefSoundIE::EChordLow;
		}
	else
		{
		validType=EFalse;
		}

	CEmsPreDefSoundIE*	emsPreDef=NULL;
	if ( validType )
		{
		emsPreDef=CEmsPreDefSoundIE::NewL(soundType);
		}

	return emsPreDef;
	}

CFbsBitmap* CTestMessBase::NewBitmapL(const TParse& aFileName, const TDesC& aObjectSize)
	{
	TInt	bitmapType=KSmallObject;
	TBool	validType=ETrue;
	if( aObjectSize.Compare(KSmall) == 0 )
		{
		bitmapType=KSmallObject;
		}
	else if ( aObjectSize.Compare(KLarge) == 0 )
		{
		bitmapType=KLargeObject;
		}
	else if ( aObjectSize.Compare(KVariable) == 0 )
		{
		bitmapType=KVariableObject;
		}
	else
		{
		ERR_PRINTF2(_L("Invalid bitmap type %S"), &aObjectSize);
		validType=EFalse;
		}

	CFbsBitmap*	bmp = NULL;
	if ( validType )
		{
		bmp = new (ELeave) CFbsBitmap();

		TInt	error = bmp->Load(aFileName.FullName(), bitmapType, EFalse);
		INFO_PRINTF2(_L("Bimap load %d"), error);
		if ( error != KErrNone)
			{
			delete bmp;
			bmp=NULL;
			}
		}

	return bmp;
	}
