// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __APAPRIV_H__
#define __APAPRIV_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__S32MEM_H__)
#include <s32mem.h>
#endif
#if !defined(__GDI_H__)
#include <gdi.h>
#endif
#include <apaid.h>

// classes referenced:
class CApaMaskedBitmap;
//

class HBufBuf : public TBufBuf
	{
public:
	static HBufBuf* NewL(CBufBase& aBuf,TInt aPos,TInt aMode=ERead|EWrite);
private:
	void DoRelease();
	};


class CApaIconPicture : public CPicture
	{
public:
	static CApaIconPicture* NewL(const TSize& aIconSizeInTwips, const TUid aAppUid);
	//
	void Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,MGraphicsDeviceMap* aMap)const;
	TPictureCapability Capability() const;
	TInt ScaleFactorWidth() const;
	TInt ScaleFactorHeight() const;
	void SetScaleFactor(TInt aScaleFactorWidth,TInt aScaleFactorHeight);
	void GetOriginalSizeInTwips(TSize& aOriginalSize) const;
	//
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream)const;
	~CApaIconPicture();
private: 
	CApaIconPicture(const TSize& aIconSizeInTwips, const TUid aAppUid);
	void ConstructL();
	void CreateIconL();
private:
	CApaMaskedBitmap* iIcon; // null unless the current format is EIcon
	TUid iAppUid;
	TSize iOriginalSizeInTwips;
	TInt iScaleFactorWidth;		   // Stored as percent * 10
	TInt iScaleFactorHeight;	   // Stored as percent * 10
	TSize iDisplaySizeInPixels;
	TSize iDisplaySizeInTwips;
	};

class CApaAppServiceInfoArrayWrapper : public CApaAppServiceInfoArray
	{
public:
	static CApaAppServiceInfoArrayWrapper* NewL(CArrayFix<TApaAppServiceInfo>* aServiceInfoArray);
	~CApaAppServiceInfoArrayWrapper();
	TArray<TApaAppServiceInfo> Array();
private:
	CApaAppServiceInfoArrayWrapper(CArrayFix<TApaAppServiceInfo>* aServiceInfoArray);
private:
	CArrayFix<TApaAppServiceInfo>* iServiceInfoArray;
	};




#endif
