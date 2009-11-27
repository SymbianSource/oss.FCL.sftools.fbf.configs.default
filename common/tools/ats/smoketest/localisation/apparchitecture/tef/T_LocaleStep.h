// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/
 
#if (!defined __T_LOCALE_STEP_H__)
#define __T_LOCALE_STEP_H__

#include "ApparcTestServer.h"
#include <apgnotif.h>
#include "testableapalssession.h"

//!  A CT_LocaleStep test class. 

/** Class tests for UIKON GT0143 Typhoon Work Series 60 Changes. 
It also tests CR0902 work : Enable Dynamic Language Switching in APPARC.
*/

class CT_LocaleStep : public CTestStep
	{
public:
	CT_LocaleStep();
	~CT_LocaleStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	void ChangeLocaleL(TLanguage aLanguage);
	void CheckIcons(CArrayFixFlat<TSize>* aNewIconSizes,TSize aSmall,TSize aMedium,TSize aLarge, TSize& afstIcon, TSize& asecIcon, TSize& atrdIcon);
	void TestLocaleDefaultIconL();
	
	//Functions to test application information on language change event.
	void TestLocaleApplicationInfoOnDemandL();
	void TestLocaleApplicationInfoL();
	void TestLocaleApplicationInfo1L();
	void TestAppInfoL(TBool aOnDemand);
	void TestAppIconL(TSize aSize);
	
	//Functions to test view's caption and icons on language change event.	
	void TestLocaleViewCaptionAndIconOnDemandL();
	void TestLocaleViewCaptionAndIconL();
	void TestViewCaptionAndIconL(TBool aOnDemand);
	void CheckViewCaptionAndIconL(TLanguage aLanguage, CApaAppViewArray* aAppViews);

private:
	RFs iFs;
	RTestableApaLsSession iLs;
	};

_LIT(KT_LocaleStep,"T_Locale");
#endif
