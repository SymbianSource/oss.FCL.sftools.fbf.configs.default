// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "APGPRIV.H"
#include <e32hal.h>
#include <s32strm.h>
#include "APGCLI.H"
#include "APGICNFL.H"
#include "APGSTD.H"

#define KDefaultIconSizeInPixels TSize(96,96)

CApaIconPicture* CApaIconPicture::NewL(const TSize& aIconSizeInTwips, const TUid aAppUid)
	{
	CApaIconPicture* self=new(ELeave) CApaIconPicture(aIconSizeInTwips, aAppUid);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->CreateIconL();
	CleanupStack::Pop(self);
	return self;
	}

CApaIconPicture::CApaIconPicture(const TSize& aIconSizeInTwips, const TUid aAppUid)
	: iAppUid(aAppUid),
	iOriginalSizeInTwips(aIconSizeInTwips),
	iScaleFactorWidth(1000),
	iScaleFactorHeight(1000)
	{
	__DECLARE_NAME(_S("CApaIconPicture"));
	}

void CApaIconPicture::ConstructL()
	{
	TMachineInfoV1Buf machineInfoBuf;
	User::LeaveIfError( UserHal::MachineInfo(machineInfoBuf) );
	TMachineInfoV1& machineInfo=machineInfoBuf();
	iDisplaySizeInTwips = machineInfo.iPhysicalScreenSize;
	iDisplaySizeInPixels = machineInfo.iDisplaySizeInPixels;
	//
	}

CApaIconPicture::~CApaIconPicture()
	{
	delete iIcon;
	}


void CApaIconPicture::CreateIconL()
	{
	// calculate the size of icon required in pixels
	TSize sizeInPixels;
	sizeInPixels.iWidth = (iOriginalSizeInTwips.iWidth*iDisplaySizeInPixels.iWidth*ScaleFactorWidth())/(iDisplaySizeInTwips.iWidth*1000);
	sizeInPixels.iHeight = (iOriginalSizeInTwips.iHeight*iDisplaySizeInPixels.iHeight*ScaleFactorHeight())/(iDisplaySizeInTwips.iHeight*1000);
	// if the required icon is not square get a large one and gdi will squash it during drawing
	CApaMaskedBitmap* newIcon = CApaMaskedBitmap::NewLC();
	RApaLsSession ls;
	CleanupClosePushL(ls);
	User::LeaveIfError(ls.Connect());
	User::LeaveIfError(ls.GetAppIcon(iAppUid, sizeInPixels, *newIcon));
	delete iIcon;
	iIcon = newIcon;
	CleanupStack::PopAndDestroy(&ls);
	CleanupStack::Pop(newIcon);
	//
	// set physical size
	TSize iconSizeInTwips;
	iconSizeInTwips.iWidth = (iOriginalSizeInTwips.iWidth*ScaleFactorWidth())/1000;
	iconSizeInTwips.iHeight = (iOriginalSizeInTwips.iHeight*ScaleFactorHeight())/1000;
	iIcon->SetSizeInTwips(iconSizeInTwips);
	}

void CApaIconPicture::Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
						   MGraphicsDeviceMap* aMap) const
	{
	__ASSERT_DEBUG(iIcon,Panic(EDPanicNoIconInPicture));
	//
	TSize sizeInPixels(0,0);
	sizeInPixels.iWidth = aMap->HorizontalTwipsToPixels(iOriginalSizeInTwips.iWidth);
	sizeInPixels.iHeight = aMap->VerticalTwipsToPixels(iOriginalSizeInTwips.iHeight);
	sizeInPixels.iWidth = (ScaleFactorWidth()*sizeInPixels.iWidth)/1000;
	sizeInPixels.iHeight = (ScaleFactorHeight()*sizeInPixels.iHeight)/1000;
	//
	TRect originalRectInPixels(aTopLeft,sizeInPixels);
	aGc.SetClippingRect(aClipRect);
	aGc.DrawBitmap(originalRectInPixels,iIcon);
	aGc.CancelClippingRect();
	}

	
void CApaIconPicture::InternalizeL(RReadStream& /*aStream*/)
	{} //lint !e1762 Suppress member function could be made const


	
void CApaIconPicture::ExternalizeL(RWriteStream& /*aStream*/)const
	{}


void CApaIconPicture::GetOriginalSizeInTwips(TSize& aOriginalSize) const
	{
	aOriginalSize = iOriginalSizeInTwips;
	}


void CApaIconPicture::SetScaleFactor(TInt aScaleFactorWidth,TInt aScaleFactorHeight)
	{
	iScaleFactorWidth = aScaleFactorWidth;
	iScaleFactorHeight = aScaleFactorHeight;
	TRAP_IGNORE(CreateIconL()); // ignore any error, just keep using the current icon instead
	}


TInt CApaIconPicture::ScaleFactorWidth() const
	{
	return iScaleFactorWidth;
	}


TInt CApaIconPicture::ScaleFactorHeight() const
	{
	return iScaleFactorHeight;
	}


TPictureCapability CApaIconPicture::Capability() const
	{
	return TPictureCapability(TPictureCapability::ENotScaleable,EFalse); // scalable, not cropable
	}
