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


#if !defined (__SERVICE_REGISTRY_H__)
#define __SERVICE_REGISTRY_H__

#include <e32base.h>
#include <apgcli.h>

/**
The Service Registry stores associations between (service,datatype) pairs and
applications. The (service,datatype) pair is the primary key for the registry.

@publishedPartner
@released
*/
class CServiceRegistry : public CBase
	{
public:
	IMPORT_C static CServiceRegistry* NewL();
	IMPORT_C ~CServiceRegistry();
	
private:
	CServiceRegistry();
	void ConstructL();
	
public:
	IMPORT_C TInt SetDefault(TUid aServiceUid, const TDataType& aDataType, 
		TUid aDefaultAppUid);
	IMPORT_C TInt RemoveEntry(TUid aServiceUid, const TDataType& aDataType);
	IMPORT_C TInt GetDefault(TUid aServiceUid, const TDataType& aDataType,
		TUid& aDefaultAppUid);
	
private:
	/** The registry is currently only a thin wrapper around the 
	RApaLsSession API */
	RApaLsSession iApaLsSession;
	};

#endif
