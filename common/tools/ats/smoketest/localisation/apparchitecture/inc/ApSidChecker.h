// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __APSIDCHECKER_H__
#define __APSIDCHECKER_H__

#include <e32base.h>
#include <e32hashtab.h>
class TDriveUnit;

class CAppSidChecker : public CBase
/** Sid checker.

This is a plugin interface for checking whether a Sid should be
included in apparc server's list of registered applications.


An instance of the CAppSidChecker is used to verify that an application
may be included in application list.
There should be one implementation of this
interface for each application type (native, java midlet, ...)


@internalAll */
	{
public:
	// Wraps ECom object instantiation
	static CAppSidChecker* CheckerForAppType(const TUid &aAppTypeUid);
	IMPORT_C virtual ~CAppSidChecker();

public:
	IMPORT_C virtual TBool AppRegisteredAt(const TUid& aSid, TDriveUnit aDrive) = 0;
	IMPORT_C virtual void SetRescanCallBackL(const TCallBack &aCallback);

private:
	IMPORT_C virtual void reserved1();
	IMPORT_C virtual void reserved2();
	IMPORT_C virtual void reserved3();

private:
	/** ECOM identifier */
	TUid iDtor_ID_Key;

	TAny* iReserved1;
	TAny* iReserved2;
	TAny* iReserved3;
	};

NONSHARABLE_CLASS(RSidCheckerMap) : public RHashMap<TUint,CAppSidChecker*>
/** Sid checker map

This Maps application type uids to Instances of the CAppSidChecker interface,
loading them if neccessary.

@internalComponent */
	{
public:	
	RSidCheckerMap( TCallBack aRescanCallBack );
	
	CAppSidChecker& FindSidCheckerL(TUid aAppType);
	void Close(); //lint !e1511 Member hides non-virtual member

private:
	TCallBack iRescanCallBack;
	};


#endif
