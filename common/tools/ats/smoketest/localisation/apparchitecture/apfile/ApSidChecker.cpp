// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <apsidchecker.h>
#include <ecom/ecom.h>

//const TUid KAppSidCheckerInterfaceUid = {0x10281FBB}; //old interface used when checking based only on SID
const TUid KAppSidCheckerInterfaceUid = {0x20007D8C}; // current interface used when checking based on SID and drive number


CAppSidChecker* CAppSidChecker::CheckerForAppType(const TUid &aAppTypeUid)
	{
	TEComResolverParams resolverParams;
	TBuf8<KMaxUidName> sidName;
	sidName.Copy(aAppTypeUid.Name());
	resolverParams.SetDataType(sidName);
	TAny* ptr = NULL;
	TRAP_IGNORE( ptr = REComSession::CreateImplementationL(
			KAppSidCheckerInterfaceUid,
			_FOFF(CAppSidChecker, iDtor_ID_Key),
			NULL, resolverParams) );
	return reinterpret_cast<CAppSidChecker*>(ptr);
	}

/** The destruction of the interface implementation */
EXPORT_C CAppSidChecker::~CAppSidChecker()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	iReserved1 = NULL;
	iReserved2 = NULL;
	iReserved3 = NULL;
	}

EXPORT_C void CAppSidChecker::SetRescanCallBackL(const TCallBack&)
	{
	}

EXPORT_C void CAppSidChecker::reserved1()
	{
	}

EXPORT_C void CAppSidChecker::reserved2()
	{
	}

EXPORT_C void CAppSidChecker::reserved3()
	{
	}

RSidCheckerMap::RSidCheckerMap( TCallBack aRescanCallBack ) :
		iRescanCallBack( aRescanCallBack )
	{
	}

CAppSidChecker& RSidCheckerMap::FindSidCheckerL(TUid aAppType)
	{
	CAppSidChecker* checker = NULL;
	CAppSidChecker** checkerPtr = Find(aAppType.iUid);
	if(checkerPtr)
		{
		checker = *checkerPtr;
		}
	else
		{
		// No CAppSidChecker instance loaded, try to find one
		checker = CAppSidChecker::CheckerForAppType(aAppType);
		if(checker)
			{
			CleanupStack::PushL(checker);
			InsertL(aAppType.iUid, checker);
			CleanupStack::Pop(checker);
			checker->SetRescanCallBackL(iRescanCallBack);
			}
		else
			{
			User::Leave(KErrNotFound);
			}
		}
	return *checker;
	}

void RSidCheckerMap::Close()
	{
	TIter iter(*this);
	while(iter.NextValue())
		{
		delete *(iter.CurrentValue());
		iter.RemoveCurrent();
		}
	RHashMap<TUint,CAppSidChecker*>::Close();
	}
