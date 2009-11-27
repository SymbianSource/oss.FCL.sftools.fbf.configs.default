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


#include <E32STD.H>
#include <E32BASE.H>
#include <E32KEYS.H>
#include <GDI.H>
#include <W32STD.H>
#include <COEDEF.H>
#include <COEMAIN.H>
#include <COEAUI.H>
#include <COECNTRL.H>
#include <APPARC.H>
#include <APGWGNAM.H>
#include <APASERVERAPP.H>
#include <EIKENV.H>
#include <EIKAPP.H>
#include <EIKDOC.H>
#include <EIKAPPUI.H>
#include <EIKSTART.H>
#include <EIKSERVERAPP.H>
#include <UIKON.HRH>
#include "TNNAPP1.H"
#include <appfwk_test.h>

TTstTransactionType gTransactionType=ETransactionType_null;
TBool gCallMadeToNewOverload_ProcessCommandParametersL=EFalse;
TInt gErrorWhenOpeningFile=KMinTInt;
TBuf<50> gFileNameWithoutDriveOrPath;
TBuf8<40> gBufferFromFile(KNullDesC8);

// CTstAppService

class CTstAppService : public CApaAppServiceBase
	{
public:
	static CTstAppService* NewL();
private:
	inline CTstAppService() {}
	// from CApaAppServiceBase
	virtual void ServiceL(const RMessage2& aMessage);
	};

CTstAppService* CTstAppService::NewL()
	{
	return new(ELeave) CTstAppService;
	}

void CTstAppService::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	case EOpcode_receiveTestResult:
		{
		CActiveScheduler::Stop();
		const TBool passingFileByHandle=aMessage.Int0();
		TBuf<50> fileNameWithoutDriveOrPath;
		aMessage.ReadL(1, fileNameWithoutDriveOrPath);
		TBool result=(gFileNameWithoutDriveOrPath.CompareF(fileNameWithoutDriveOrPath)==0) ? KCheckPass : KCheckFail;
		switch (gTransactionType)
			{
			case ETransactionType_filePassedByName:
				if (!gCallMadeToNewOverload_ProcessCommandParametersL)
					{
					result=KCheckFail;
					}
				if (passingFileByHandle)
					{
					result=KCheckFail;
					}
				if (gErrorWhenOpeningFile!=KErrPermissionDenied)
					{
					result=KCheckFail;
					}
				if (User::CreatorSecureId().iId!=0x101F289C) // uid of the creator app => apparctestserver!
					{
					result=KCheckFail;
					}
				break;

			case ETransactionType_filePassedByHandle:
				if (!gCallMadeToNewOverload_ProcessCommandParametersL)
					{
					result=KCheckFail;
					}
				if (!passingFileByHandle)
					{
					result=KCheckFail;
					}
				if (gErrorWhenOpeningFile!=KMinTInt)
					{
					result=KCheckFail;
					}
				if (User::CreatorSecureId().iId!=0x101F289C) // uid of the creator app => apparctestserver!
					{
					result=KCheckFail;
					}
				break;

			case ETransactionType_null:
			default:
				User::Invariant();
				break;
			}
		aMessage.Complete(result);
		}
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}
	}

// CTstAppServer

class CTstAppServer : public CEikAppServer
	{
public:
	static CTstAppServer* NewL();
private:
	inline CTstAppServer() {}
	// from CApaAppServer
	virtual CApaAppServiceBase* CreateServiceL(TUid aServiceUid) const;
	virtual TCustomResult CreateServiceSecurityCheckL(TUid aServiceUid, const RMessage2& aMessage, TInt& aAction, TSecurityInfo& aMissingSecurityInfo);
	};

CTstAppServer* CTstAppServer::NewL()
	{ // static
	CTstAppServer* const appServer=new(ELeave) CTstAppServer;
	CleanupStack::PushL(appServer);
	appServer->ConstructL(KLitServerName);
	CleanupStack::Pop(appServer);
	return appServer;
	}

CApaAppServiceBase* CTstAppServer::CreateServiceL(TUid aServiceUid) const
	{
	if ((aServiceUid.iUid==KTstServiceUid_nonSecure.iUid) ||
		(aServiceUid.iUid==KTstServiceUid_secure.iUid))
		{
		return CTstAppService::NewL();
		}
	User::Leave(KErrNotSupported);
	return NULL;
	}

CPolicyServer::TCustomResult CTstAppServer::CreateServiceSecurityCheckL(TUid aServiceUid, const RMessage2& aMessage, TInt& aAction, TSecurityInfo& aMissingSecurityInfo)
	{
	if ((aServiceUid.iUid==KTstServiceUid_secure.iUid) && (aMessage.SecureId().iId!=0x101F289C))  // uid of the creator app => apparctestserver!
		{
		return EFail;
		}
	return CEikAppServer::CreateServiceSecurityCheckL(aServiceUid, aMessage, aAction, aMissingSecurityInfo);
	}

// CTstControl

class CTstControl : public CCoeControl
	{
public:
	static CTstControl* NewL();
	virtual ~CTstControl();
private:
	CTstControl();
	void ConstructL();
	// from CCoeControl
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aEventCode);
	virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	virtual void Draw(const TRect& aRectangle) const;
private:
	CTstAppServer* iAppServer;
	};

CTstControl* CTstControl::NewL()
	{ // static
	CTstControl* const control=new(ELeave) CTstControl;
	CleanupStack::PushL(control);
	control->ConstructL();
	CleanupStack::Pop(control);
	return control;
	}

CTstControl::~CTstControl()
	{
	delete iAppServer;
	iEikonEnv->RemoveFromStack(this);
	}

CTstControl::CTstControl()
	{
	}

void CTstControl::ConstructL()
	{
	RWsSession& windowServerSession=iCoeEnv->WsSession();
	windowServerSession.SetAutoFlush(ETrue);
	CreateWindowL();
	EnableDragEvents();
	ClaimPointerGrab();
	RDrawableWindow& window=*DrawableWindow();
	window.SetOrdinalPosition(0);
	window.SetShadowHeight(3);
	CWsScreenDevice& screenDevice=*iCoeEnv->ScreenDevice();
	const TSize screenSize(screenDevice.SizeInPixels());
	SetExtent(TPoint(20, 20), TSize(screenSize.iWidth-40, screenSize.iHeight-40));
	iEikonEnv->AddDialogLikeControlToStackL(this);
	iAppServer=CTstAppServer::NewL();
	ActivateL();
	}

TKeyResponse CTstControl::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aEventCode)
	{
	if (aEventCode==EEventKey)
		{
		switch (aKeyEvent.iCode)
			{
		case EKeyEscape:
			iEikonEnv->EikAppUi()->HandleCommandL(EEikCmdExit);
			break;
			}
		}
	return EKeyWasConsumed;
	}

void CTstControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

void CTstControl::Draw(const TRect&) const
	{
	CWindowGc& graphicsContext=SystemGc();
	graphicsContext.SetPenStyle(CGraphicsContext::ESolidPen);
	graphicsContext.SetPenColor(KRgbRed);
	graphicsContext.SetBrushStyle(CGraphicsContext::ESolidBrush);
	graphicsContext.SetBrushColor(KRgbGray);
	TRect rectangle(TPoint(0, 0), Rect().Size());
	graphicsContext.DrawRect(rectangle);
	rectangle.Shrink(1, 1);
	graphicsContext.SetPenColor(KRgbBlue);
	graphicsContext.SetBrushStyle(CGraphicsContext::ENullBrush);

	const TInt KGapAboveTop=40;
	const TInt KGapBetweenLines=5;
	const TInt KMaxLengthOfText=100;
	TBuf<KMaxLengthOfText> text;

	TInt gapLeft=5;
	const CFont* font=iCoeEnv->NormalFont();
	graphicsContext.UseFont(font);
	TInt heightInPixelsPlusGapBetweenLines=font->HeightInPixels()+KGapBetweenLines;
	TPtrC transactionType;
	switch (gTransactionType)
		{
		case ETransactionType_null:
			transactionType.Set(_L("null"));
			break;
		case ETransactionType_filePassedByName:
			transactionType.Set(_L("file passed by name"));
			break;
		case ETransactionType_filePassedByHandle:
			transactionType.Set(_L("file passed by handle"));
			break;
		default:
			User::Invariant();
			break;
		}
	text.Format(_L("Transaction type: %S"), &transactionType);
	TInt topOfText=KGapAboveTop+font->AscentInPixels();
	graphicsContext.DrawText(text, rectangle, topOfText, CGraphicsContext::ELeft, gapLeft);

	topOfText+=heightInPixelsPlusGapBetweenLines;
	_LIT(KLit_yes, "yes");
	_LIT(KLit_no, "no");
	text.Format(_L("ProcessCommandParametersL(CApaCommandLine&) called: %S"), gCallMadeToNewOverload_ProcessCommandParametersL? &KLit_yes: &KLit_no);
	graphicsContext.DrawText(text, rectangle, topOfText, CGraphicsContext::ELeft, gapLeft);

	topOfText+=heightInPixelsPlusGapBetweenLines;
	text.Format(_L("Error when opening file: %d"), gErrorWhenOpeningFile);
	graphicsContext.DrawText(text, rectangle, topOfText, CGraphicsContext::ELeft, gapLeft);

	topOfText+=heightInPixelsPlusGapBetweenLines;
	text=_L("File contents: {");
	const TInt bufferLength=gBufferFromFile.Length();
	for (TInt i=0; i<bufferLength; ++i)
		{
		if (KMaxLengthOfText-text.Length()<5)
			{
			break;
			}
		text.AppendFormat(_L("\\x%02x"), gBufferFromFile[i]);
		}
	text.Append('}');
	graphicsContext.DrawText(text, rectangle, topOfText, CGraphicsContext::ELeft, gapLeft);

	graphicsContext.DiscardFont();
	}

// CTstAppUi

class CTstAppUi : public CEikAppUi
	{
public:
	static CTstAppUi* NewL();
	inline CTstControl* Control() {return iControl;}
private:
	inline CTstAppUi() {}
	virtual ~CTstAppUi();
	// from MEikCommandObserver
	virtual void ProcessCommandL(TInt);
	// from MEikMenuObserver
	virtual void RestoreMenuL(CCoeControl*, TInt, TMenuType);
	virtual void DynInitMenuPaneL(TInt, CEikMenuPane*);
	virtual void DynInitMenuBarL(TInt, CEikMenuBar*);
	// from CEikAppUi
	virtual void ConstructL();
	virtual TBool ProcessCommandParametersL(TApaCommand, TFileName& aDocumentName, const TDesC8&);
	virtual TBool ProcessCommandParametersL(CApaCommandLine& aCommandLine);
	virtual void HandleCommandL(TInt aCommand);
	virtual void OpenFileL(const TDesC&);
	virtual void CreateFileL(const TDesC&);
private:
	CTstControl* iControl;
	};

CTstAppUi* CTstAppUi::NewL()
	{ // static
	return new(ELeave) CTstAppUi;
	}

CTstAppUi::~CTstAppUi()
	{
	delete iControl;
	}

void CTstAppUi::ProcessCommandL(TInt)
	{
	User::Invariant();
	}

void CTstAppUi::RestoreMenuL(CCoeControl*, TInt, TMenuType)
	{
	User::Invariant();
	}

void CTstAppUi::DynInitMenuPaneL(TInt, CEikMenuPane*)
	{
	User::Invariant();
	}

void CTstAppUi::DynInitMenuBarL(TInt, CEikMenuBar*)
	{
	User::Invariant();
	}

void CTstAppUi::ConstructL()
	{
	BaseConstructL(ENoAppResourceFile|ENoScreenFurniture);
	iControl=CTstControl::NewL();
	}

TBool CTstAppUi::ProcessCommandParametersL(TApaCommand, TFileName& aDocumentName, const TDesC8&)
	{
	aDocumentName.SetLength(0);
	return EFalse;
	}

TBool CTstAppUi::ProcessCommandParametersL(CApaCommandLine&)
	{
	gCallMadeToNewOverload_ProcessCommandParametersL=ETrue;
	iControl->DrawNow();
	return ETrue;
	}

void CTstAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	case EEikCmdExit:
		Exit();
		break;
	default:
		User::Invariant();
		}
	}

void CTstAppUi::OpenFileL(const TDesC&)
	{
	User::Invariant();
	}

void CTstAppUi::CreateFileL(const TDesC&)
	{
	User::Invariant();
	}

// CTstDocument

class CTstDocument : public CEikDocument
	{
public:
	static CTstDocument* NewL(CEikApplication& aApplication);
	inline CTstAppUi* TstAppUi() {return STATIC_CAST(CTstAppUi*, iAppUi);}
private:
	inline CTstDocument(CEikApplication& aApplication) :CEikDocument(aApplication) {}
	// from CApaDocument
	virtual CFileStore* CreateFileStoreLC(RFs&, const TDesC&);
	virtual void EditL(MApaEmbeddedDocObserver*, TBool);
	virtual void PrintL(const CStreamStore&);
	virtual void SaveL();
	virtual void StoreL(CStreamStore&, CStreamDictionary&) const;
	// from CEikDocument
	virtual CEikAppUi* CreateAppUiL();
	virtual CFileStore* OpenFileL(TBool aDoOpen, const TDesC& aFileName, RFs& aFsSession);
	virtual void OpenFileL(CFileStore*& aFileStore, RFile& aFile);
	virtual void UpdateTaskNameL(CApaWindowGroupName* aWindowGroupName);
	};

CTstDocument* CTstDocument::NewL(CEikApplication& aApplication)
	{ // static
	return new(ELeave) CTstDocument(aApplication);
	}

CFileStore* CTstDocument::CreateFileStoreLC(RFs&, const TDesC&)
	{
	User::Invariant();
	return NULL;
	}

void CTstDocument::EditL(MApaEmbeddedDocObserver*, TBool)
	{
	User::Invariant();
	}

void CTstDocument::PrintL(const CStreamStore&)
	{
	User::Invariant();
	}

void CTstDocument::SaveL()
	{
	SetChanged(EFalse);
	}

void CTstDocument::StoreL(CStreamStore&, CStreamDictionary&) const
	{
	User::Invariant();
	}

CEikAppUi* CTstDocument::CreateAppUiL()
	{
	return CTstAppUi::NewL();
	}

CFileStore* CTstDocument::OpenFileL(TBool aDoOpen, const TDesC& aFileName, RFs& aFsSession)
	{
	if (!aDoOpen)
		{
		User::Leave(KErrNotSupported);
		}
	gFileNameWithoutDriveOrPath=TParsePtrC(aFileName).NameAndExt();
	RFile file;
	CleanupClosePushL(file);
	APPFWK_NEGATIVE_PLATSEC_START;
	gErrorWhenOpeningFile=file.Open(aFsSession, aFileName, EFileShareReadersOnly|EFileStream|EFileRead);
	APPFWK_NEGATIVE_PLATSEC_FINISH;
	if (gErrorWhenOpeningFile==KErrNone)
		{
		User::LeaveIfError(file.Read(gBufferFromFile));
		}
	CleanupStack::PopAndDestroy(&file);
	gTransactionType=ETransactionType_filePassedByName;
	TstAppUi()->Control()->DrawNow();
	return NULL;
	}

void CTstDocument::OpenFileL(CFileStore*& aFileStore, RFile& aFile)
	{
	User::LeaveIfError(aFile.Name(gFileNameWithoutDriveOrPath));
	User::LeaveIfError(aFile.Read(gBufferFromFile));
	gTransactionType=ETransactionType_filePassedByHandle;
	TstAppUi()->Control()->DrawNow();
	aFileStore=NULL;
	}

void CTstDocument::UpdateTaskNameL(CApaWindowGroupName* aWindowGroupName)
	{
	CEikDocument::UpdateTaskNameL(aWindowGroupName);
	}

// CTstApplication

class CTstApplication : public CEikApplication
	{
public:
	static CTstApplication* New();
private:
	inline CTstApplication() {}
	// from CApaApplication
	virtual TUid AppDllUid() const;
	virtual CDictionaryStore* OpenIniFileLC(RFs&) const;
	// from CEikApplication
	virtual void GetDefaultDocumentFileName(TFileName& aDocumentName) const;
	virtual TFileName BitmapStoreName() const;
	virtual TFileName ResourceFileName() const;
	virtual CApaDocument* CreateDocumentL();
	virtual void NewAppServerL(CApaAppServer*& aAppServer);
	};

CTstApplication* CTstApplication::New()
	{ // static
	return new CTstApplication;
	}

TUid CTstApplication::AppDllUid() const
	{
	return TUid::Uid(0x10207f92);
	}

CDictionaryStore* CTstApplication::OpenIniFileLC(RFs&) const
	{
	return NULL; // must not leave anything on the clean-up stack if it returns NULL
	}

void CTstApplication::GetDefaultDocumentFileName(TFileName& aDocumentName) const
	{
	aDocumentName.SetLength(0);
	}

TFileName CTstApplication::BitmapStoreName() const
	{
	return KNullDesC();
	}

TFileName CTstApplication::ResourceFileName() const
	{
	return KNullDesC();
	}

CApaDocument* CTstApplication::CreateDocumentL() // must return non-NULL
	{
	return CTstDocument::NewL(*this);
	}

void CTstApplication::NewAppServerL(CApaAppServer*& aAppServer)
	{
	// this doesn't get called by the framework as we're not launching the server via the server-differentiator in CApaCommandLine, we launching the server as a fixed-name server so we have to call this function ourselves instead
	CEikApplication::NewAppServerL(aAppServer);
	}

// stand-alone functions

LOCAL_C CApaApplication* NewApplication()
	{
	return CTstApplication::New();
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
