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
// This contains CTestMessBase which is the base class for all 
// messaging test steps
// 
//


#if (!defined __TEST_MESS_BASE_H__)
#define __TEST_MESS_BASE_H__

#include "Configuration.cfg"

#include <msvapi.h>
#include <SmutHdr.h>
#include <TestExecuteStepBase.h>
#include <EMSFormatIE.h>
#include <EMSSoundIE.h>
#include <EMSAnimationIE.h>
#include <IAPPrefs.h>

#include "TestMessProcessor.h"
#include "TestMessTimer.h"

#include <smut.h>
#include <Smutset.h>
#include <etelmm.h>
#if (defined CDMA_API_ENABLED)
#include <SmsMessageWrappers.h>
#include <SmsMessageSettings.h>
#endif
#include <SmutHdr.h>

#include <TestStepSmokeTest.h>
//To check whether the reply quoted is set
_LIT(KReplyQuoted,		"SmsReplyQuoted");

const TUid KUidMsvSMSHeaderStream				= {0x10001834};
const TUid KUidMsgFileInternetAccessPreferences	= {0x1000924B};
const TUid KUidMsvDefaultServices				= {0x100017FD};

const TInt	KOneSecond = 1000000;

class	CParaFormatLayer;
class 	CCharFormatLayer;
class 	CRichText;

class CTestMessBase : public CTestStepSmokeTest, private MMsvSessionObserver
	{
public:
	CTestMessBase(TBool aSetDirectory);
	virtual ~CTestMessBase();

	virtual TVerdict	doTestStepPreambleL();
	virtual TVerdict	doTestStepPostambleL();

	void							SetEntryL(TMsvId	aId);
	CMsvEntry&						EntryL();
	TBool							HasEntry() const { return iEntry!=NULL; }
	const TMsvSelectionOrdering&	SelectionOrdering() const { return iOrder; }
	CMsvSession&					Session() const { return *iSession; }
	void							PrintEntryL(CMsvEntry& aEntry);
	CEmsPreDefSoundIE*				NewPreDefSoundL(const TDesC& aPreDefType);


protected:
	//	MMsvSessionObserver
	void			HandleSessionEventL(TMsvSessionEvent, TAny*, TAny*, TAny*);

	//	Utilities
	static HBufC8*	CopyToBuf8LC(const TDesC& aPtrC);
	static HBufC*	CopyToBufLC(const TDesC8& aPtr8);
	void			PrintTreeL(TMsvId aRoot);
	void			IssueTimerRequest();


	void			PrepareSmsHeaderFromIniFileL(CSmsHeader& aSmsHeader);

	//reads the schedule time from ini file
	TTimeIntervalMinutes	ReadScheduleTime();

private:
	//	PrintEntry helper functions
	void			PrintStoreL(CMsvEntry& aEntry);
	void			PrintStoreSmtpL(CMsvEntry& aEntry);
	void			PrintStorePop3L(CMsvEntry& aEntry);
	void			PrintStoreImap4L(CMsvEntry& aEntry);
	void			PrintStoreSmsL(CMsvEntry& aEntry);

	//	PrepareSmsHeaderFromIniFileL helper functions
	void			SetAlignmentFormat(CEmsFormatIE& aEmsFormatIE, const TDesC& aAlignment);
	void			SetFontSizeFormat(CEmsFormatIE& aEmsFormatIE, const TDesC& aFontSize);

	CEmsPreDefAnimationIE*	NewPreDefAnimationL(const TDesC& aPreDefType);
	CFbsBitmap*				NewBitmapL(const TParse& aFileName, const TDesC& aObjectSize);

protected:
	CMsvSession*				iSession;
	CTestMessProcessor*			iMessageProcessor;
	CParaFormatLayer*			iParaFormatLayer;
 	CCharFormatLayer*			iCharFormatLayer;
 	CRichText*					iBodyText;
    TBool						iBodyAvailable;


private:
    TBool						iCheckForSim;
	TBool						iSetDirectory;
	CTestMessTimer*				iMessageTimer;
	CActiveScheduler*			iSchedular;
	CMsvEntry*					iEntry;
	TMsvSelectionOrdering		iOrder;
	};

#endif /* __TEST_MESS_BASE_H__ */
