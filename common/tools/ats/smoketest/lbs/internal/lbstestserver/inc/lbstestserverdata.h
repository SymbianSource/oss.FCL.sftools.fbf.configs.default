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
// data which identifies the server used to communicate with the LBS
// Network Request Handler
// 
//

#ifndef LBSTESTSERVERDATA_H__
#define LBSTESTSERVERDATA_H__

// Define the file and process of the Lbs test server
_LIT(KTestServerFileName, "\\sys\\bin\\lbstestserver.exe");
_LIT(KTestServerName, "lbstestserver.exe");

// Current version number of the Lbs test server.
const TInt8 KTestServerMajorVersionNumber	= 1;
const TInt8 KTestServerMinorVersionNumber	= 0;
const TInt16 KTestServerBuildVersionNumber	= 0;

// Uid3 of the Lbs test server
const TUint KLbsTestServerUidValue			= 0x10282281;
const TUid  KLbsTestServerUid				= {KLbsTestServerUidValue};


#endif // LBSTESTSERVERDATA_H__
