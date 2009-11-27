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

#if !defined(__TRAPALSSESSIONSTARTAPPTEST_H__)
#define __TRAPALSSESSIONSTARTAPPTEST_H__

#if !defined(__E32STD_H__)
#include <E32STD.H>
#endif

/** KLitLongURL must be > 256 characters long, in this case it's 259 characters long
    The name is a little mis-leading, as it's not a real URL. URL is used in the name
    because one of the use cases for needing document names longer than 256 characters
    is when starting a very long URL to be handled by a web browser application
*/


_LIT(KLitTRApaLsSessionStartAppTest, "Z:\\system\\data\\TRApaLsSessionStartAppTest.tst");



const TUid KUidMimeTRApaLsSessionStartAppTestRecognizer={0x10004c50};
const TInt KNumDataTypes = 1;
_LIT8(KLitMimeType_TRApaLsSessionStartAppTest, "x-epoc/start-app-test");
#endif
