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

#ifndef __APADLL_H__
#define __APADLL_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__APADEF_H__)
#include <apadef.h>
#endif
#if !defined(__APPARC_H__)
#include <apparc.h>
#endif


// classes referenced
class CApaApplication;
class RFs;

NONSHARABLE_CLASS(CApaAppHolder) : public CBase
/** Provides an interface for obtaining data gathered differently for DLLs and EXEs. Derived
classes create and own the application object. The equivalent of the old CApaDll derives from this.*/
{
public:
	CApaAppHolder();
	~CApaAppHolder();

	virtual TFileName FileName() const = 0;
	virtual TUid Uid() const = 0;
	virtual CApaApplication* Application() const = 0;

protected:
	void UpdateAppsRefToThis();
};

NONSHARABLE_CLASS(CApaExe) : public CApaAppHolder
	{
public:
	CApaExe();
	~CApaExe();

	TFileName FileName() const;
	TUid Uid() const;
	CApaApplication* Application() const;

	void CreateApplicationL(TApaApplicationFactory aApplicationFactory); // Uses the factory to create the application
private:
	CApaApplication* iApplication;
	HBufC* iAppName;
	TUid iFileUid;
	};

#endif
