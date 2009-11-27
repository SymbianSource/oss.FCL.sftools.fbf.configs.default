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


_LIT(KLitServerName, "AppService_10207f92");

const TUid KTstServiceUid_secure={0x10207f97};
const TUid KTstServiceUid_nonSecure={0x10207f98};

/** These numbers have been chosen at random to be used as an exit reason */
const TInt KCheckPass = 13;
const TInt KCheckFail = 77;

enum TTstTransactionType
	{
	ETransactionType_null,
	ETransactionType_filePassedByName,
	ETransactionType_filePassedByHandle
	};

enum TTstOpcode
	{
	EOpcode_receiveTestResult
	};
