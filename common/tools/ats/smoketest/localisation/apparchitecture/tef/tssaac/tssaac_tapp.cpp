// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is a Startup State Aware (SSA) test application used to
// test apparc StartApp and StartDocument functionality.
// 
//



/**
 @file
 @test
 @internalComponent  Internal Symbian test code
*/


#include <coeccntx.h>

#include <apgtask.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>

#include <eikon.hrh>

#include <tssaac_tapp.rsg>

#include <eikstart.h>
#include <s32file.h>
#include "tssaac.h"
#include <apacmdln.h>
#include "appfwk_test_AppUi.h"
#include "tstapp.h"



////////////////////////////////////////////////////////////////////////
//
// CExampleAppView
//
////////////////////////////////////////////////////////////////////////
class CExampleAppView : public CCoeControl
    {
public:
	static CExampleAppView* NewL(const TRect& aRect);
	CExampleAppView();
	~CExampleAppView();
    void ConstructL(const TRect& aRect);
private:
	void Draw(const TRect& /*aRect*/) const;
private:
	HBufC*  iExampleText;
    };

CExampleAppView::CExampleAppView()
	{
	}

CExampleAppView* CExampleAppView::NewL(const TRect& aRect)
	{
	CExampleAppView* self = new(ELeave) CExampleAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

CExampleAppView::~CExampleAppView()
	{
	delete iExampleText;
	}

void CExampleAppView::ConstructL(const TRect& aRect)
    {
    _LIT(KExampleText, "This is a Startup State Aware (SSA) test application");
	TPtrC ptr(KExampleText);
	iExampleText = ptr.AllocL();
	CreateWindowL();
	SetRect(aRect);
	ActivateL();
	}

void CExampleAppView::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();
	TRect      drawRect = Rect();
	const CFont*     fontUsed;

	gc.Clear();
  	fontUsed = iEikonEnv->TitleFont();
	gc.UseFont(fontUsed);
	TInt   baselineOffset=(drawRect.Height() - fontUsed->HeightInPixels())/2;
	gc.DrawText(*iExampleText,drawRect,baselineOffset,CGraphicsContext::ECenter, 0);
	gc.DiscardFont();
	}


////////////////////////////////////////////////////////////////////////
//
// CExampleAppUi
//
////////////////////////////////////////////////////////////////////////
class CExampleAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CExampleAppUi();
private:
	void HandleCommandL(TInt aCommand);
private:
	CCoeControl* iAppView;
	};


void CExampleAppUi::ConstructL()
    {
    BaseConstructL();
	iAppView = CExampleAppView::NewL(ClientRect());
	}

CExampleAppUi::~CExampleAppUi()
	{
	delete iAppView;
	}

void CExampleAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	case EEikCmdExit:
		Exit();
		break;
		}
	}



////////////////////////////////////////////////////////////////////////
//
// CExampleDocument
//
////////////////////////////////////////////////////////////////////////
class CExampleDocument : public CEikDocument
	{
public:
	static CExampleDocument* NewL(CEikApplication& aApp);
	CExampleDocument(CEikApplication& aApp);
	void ConstructL();
private:
	CEikAppUi* CreateAppUiL();
	};

CExampleDocument::CExampleDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}

CEikAppUi* CExampleDocument::CreateAppUiL()
	{
    return new(ELeave) CExampleAppUi;
	}


////////////////////////////////////////////////////////////////////////
//
// CSysatartApparc
//
////////////////////////////////////////////////////////////////////////

class CSysatartApparc : public CEikApplication
	{
private:
	// Inherited from class CApaApplication
	CApaDocument* CreateDocumentL();
	void WriteResultFileL();
	TUid AppDllUid() const;
	//Test functions and variables
	CApaDocument* CreateDocumentL(CApaProcess* aApaProcess);
private:	
	TInt iLocalizedCaptionLRes1;
	TInt iLocalizedCaptionLRes2;
	};

const TUid KUidSSAACTestApp = { 0x10207F7A };

TUid CSysatartApparc::AppDllUid() const
	{
	return KUidSSAACTestApp;
	}

/**
   @SYMTestCaseID          APPFWK-APPARC-0029

   @SYMPREQ                PREQ967

   @SYMREQ                 REQ3889

   @SYMTestCaseDesc        Test the functionality of EnsureCaptionIsLocalisedL()

   @SYMTestPriority        High

   @SYMTestStatus          Implemented

   @SYMTestActions         Call CEikApplication::CreateDocumentL() with a local resource file argument
   						   When Application specified exists.

                           API Calls:\n
						   CEikApplication::EnsureCaptionIsLocalisedL(TDesC&)

   @SYMTestExpectedResults (1) AppCaption() returns "tssaac_tapp"
   						   (2) AppCaption() returns "NewCaption"

*/
CApaDocument* CSysatartApparc::CreateDocumentL(CApaProcess* aApaProcess)
	{
	_LIT(KPath, "z:\\resource\\apps\\tssaac_tapp_loc.rsc");
	_LIT(KCurrentCaption, "tssaac_tapp");
	_LIT(KNewCaption, "NewCaption");

	//(1) Fetch the existing name of the exe (tssaac_tapp)
	TApaAppCaption caption = AppCaption();

	iLocalizedCaptionLRes1 = caption.Compare(KCurrentCaption);
	RDebug::Print(_L("Expected existing caption: '%S' Actual caption: '%S'"), &KCurrentCaption, &caption);

	//Assign a new name to the exe from the local resource file
	CEikApplication::EnsureCaptionIsLocalisedL(KPath);

	//(2) Fetch the new name of the exe (NewCaption)
	caption = AppCaption();

	//Assign the result to the member variable
	iLocalizedCaptionLRes2 = caption.Compare(KNewCaption);
	RDebug::Print(_L("Expected new caption: '%S' Actual caption: '%S'"), &KNewCaption, &caption);

	WriteResultFileL();

	// Completes all Rendezvous' with the current process in order to notify sysstart and continue
	//	launching the next process in the SSC list (this app is launched with EWaitForStart)
	RProcess::Rendezvous(KErrNone);

	return CEikApplication::CreateDocumentL(aApaProcess);
	}


CApaDocument* CSysatartApparc::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}


LOCAL_C CApaApplication* NewApplication()
	{
	return new CSysatartApparc;
	}


GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

/**
  Write the results to the result file, to be later extracted by
  testexecute test case.
  These contain the results of the test. The order in
  which these results are written in MUST match the order in which
  they are read and tested in CSysStartApparcTestStep::GetApparcResultsL()
  and the expected result tested in CSysStartApparcTestStep::Testxxx must
  match that in the test specification\n

  The format is:
  LocalisedCaptionRes1 result=value1 		Pass = 0 Fail = non-zero
  LocalisedCaptionRes2 result=value2 		Pass = 0 Fail = non-zero

  '=' is used as the deliminator.
*/
void CSysatartApparc::WriteResultFileL()
	{
	RFs	fs;
 	User::LeaveIfError(fs.Connect());
 	CleanupClosePushL(fs);

	RFileWriteStream writer;
	User::LeaveIfError(writer.Replace(fs, KSsaacResultFile, EFileWrite));
	CleanupClosePushL(writer);

	// write data to file
	writer << _L("LocalizedCaptionRes1 count=");
	writer.WriteInt16L(iLocalizedCaptionLRes1);
	writer << _L("\n");
	writer << _L("LocalizedCaptionRes2 count=");
	writer.WriteInt16L(iLocalizedCaptionLRes2);
	writer << _L("\n");
	writer.CommitL();
	
	CleanupStack::PopAndDestroy(2, &fs);
	}
