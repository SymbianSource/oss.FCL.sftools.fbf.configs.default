// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CONSOLEALARMALERTSERVERMAIN_H__
#define __CONSOLEALARMALERTSERVERMAIN_H__

// System includes
#include <e32base.h>

// User includes

// Type definitions

// Constants

// Enumerations

// Classes referenced


///////////////////////////////////////////////////////////////////////////////////////
// ----> ConsoleAlarmAlertServer (header)
///////////////////////////////////////////////////////////////////////////////////////
class ConsoleAlarmAlertServer
/**
@internalAll
@released
*/
	{
///////////////////////////////////////////////////////////////////////////////////////
public:										// ACCESS
///////////////////////////////////////////////////////////////////////////////////////
	IMPORT_C static void					StartConsoleAlarmAlertServerL(TAny* instructionSet = NULL);

///////////////////////////////////////////////////////////////////////////////////////
private:									// INTERNAL
///////////////////////////////////////////////////////////////////////////////////////
	static TInt								StartServerThreadFunction(TAny* instructionSet = NULL);
	static void								StartServerL(TAny* instructionSet = NULL);
	};

#endif
