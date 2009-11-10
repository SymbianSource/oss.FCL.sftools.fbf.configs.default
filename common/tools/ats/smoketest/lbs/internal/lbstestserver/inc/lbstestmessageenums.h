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
// The message enums/opcodes shared between the client and the server sides
// 
//

#ifndef LBSTESTMESSAGEENUMS_H__
#define LBSTESTMESSAGEENUMS_H__

#include "lbsmessageenums.h"

enum ELbsTestClientServerMessageEnums
	{	
	//
	// If you need to add any more message Ids, add them between
	// EFirstLbsTestServerMessage and ELastLbsTestServerMessage.
	// (Otherwise the security policy in lbstestserver.cpp may not
	// allow the new message.)
	//

	// Start of lbstest specific messages after the last of the
	// basic server framework messages, ELastSessionMessageId,
	// which is defined as KMaxInt32/2 aka 16383 (0x3FFFFFFF)
	// in lbsmessageenums.h
	EFirstLbsTestServerMessage						= 0x40000000,
	
	ELbsTestStopLbsSystem							= EFirstLbsTestServerMessage,
	ELbsTestStartLbsSystem							= 0x40000001,
	ELbsTestResetLbsSystem							= 0x40000002,
	
	ELastLbsTestServerMessage,
	
	};

#endif //LBSTESTMESSAGEENUMS_H__
