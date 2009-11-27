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

#include <aplaunchchecker.h>

CAppLaunchChecker*	CAppLaunchChecker::NewL(const TUid aUid)
/** Create an interface implementation to satisfy the specified interface.
@param aUid Com interface implementation UID.
@return The new application launch checker object. */
	{
	TAny* ptr = REComSession::CreateImplementationL(aUid, 
		_FOFF(CAppLaunchChecker, iDtor_ID_Key));
	return reinterpret_cast<CAppLaunchChecker*> (ptr);
	}

/** The destruction of interface implementation. */
EXPORT_C CAppLaunchChecker::~CAppLaunchChecker()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C void CAppLaunchChecker::Reserved_1()
	{
	
	}

EXPORT_C void CAppLaunchChecker::Reserved_2()
	{

	}

EXPORT_C void CAppLaunchChecker::Reserved_3()
	{

	}
