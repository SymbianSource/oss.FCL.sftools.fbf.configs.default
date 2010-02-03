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
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#if !defined(__TWINDOWCHAINING_H__)
#define __TWINDOWCHAINING_H__

#if !defined(__E32STD_H__)
#include <E32STD.H>
#endif

/** Ordinal priority chosen at random */
const TInt KExoticOrdinalPriority=748;

/** UIDs of the child and server (launch) application */
const TUid KUidTChainChildApp = { 0x10009e9f };
const TUid KUidTChainServerApp = { 0x10009f9a };

/** KTWindowChainingTestPassed is a number chosen at random to be used as an exit reason
    for TWindowChainingApp.app's application thread to signify that the test passed */
const TInt KTWindowChainingTestPassed = 2387;

const TInt KChainPass = 42;
const TInt KChainFail = 59;

/** Message ID (chosen at random) to query child to check parent id matches */
const TInt KQueryChainChild1 = 428;
const TInt KQueryChainChild2 = 429;

_LIT(KWinChainChildAppFileName, "z:\\sys\\bin\\t_winchainLaunch.exe");
#endif // __TWINDOWCHAINING_H__
