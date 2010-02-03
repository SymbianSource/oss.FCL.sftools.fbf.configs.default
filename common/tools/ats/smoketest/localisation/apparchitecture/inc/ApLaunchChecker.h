// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#ifndef __APLAUNCHCHECKER_H__
#define __APLAUNCHCHECKER_H__

#ifndef __E32BASE_H__
#include <e32base.h>
#endif

#ifndef __E32CMN_H__
#include <e32cmn.h>
#endif

#ifndef __APGTASK_H__
#include <apgtask.h>
#endif

#ifndef __ECOMRESOLVERPARAMS_H__
#include <ecom/ecomresolverparams.h>
#endif

#ifndef __ECOM_H__
#include <ecom/ecom.h>
#endif

class CAppLaunchChecker : public CBase
/** Launch checker.

This is an interface for rule-based launching of an application.

An instance of the CAppLaunchChecker class is used to notify an application server whether or not
an application should be launched. Inside the plug-in could be implemented some addition actions, 
for instance closing some low priority applications.

@publishedPartner
@prototype
@see TApaTaskList
@see TUid */
	{
public:
	/** Defines codes for launching an application */
	enum TAppLaunchCode
		{
		/** Plug-in doesn't care whether an application should be launched or not */
		EAppLaunchIndifferent,
		/** Rule-based framework shouldn't launch an application */
		EAppLaunchDecline,
		/** Rule-based framework can launch an application */
		EAppLaunchAccept,
		};
// Wraps ECom object instantiation
	static CAppLaunchChecker* NewL(const TUid aUid);
	IMPORT_C ~CAppLaunchChecker();
	/** The function to be implemented in inherited classes. */	
	TAppLaunchCode virtual OkayToLaunchL(const TUid aAppToLaunch, TApaTaskList& aTaskList) = 0;
private:	
	IMPORT_C virtual void Reserved_1();
	IMPORT_C virtual void Reserved_2();
	IMPORT_C virtual void Reserved_3();
private:
	/** ECOM identifier */
    TUid iDtor_ID_Key;

	TInt iReserved1;    
	TInt iReserved2;    
	TInt iReserved3;
	};
	
#endif
