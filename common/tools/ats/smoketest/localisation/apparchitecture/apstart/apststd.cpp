// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "apststd.h"

void Panic(TApaPanic aPanic)
//
// Panic the process with APSTART as the category.
//
	{
	_LIT(KApStartPanic,"APSTART");
	User::Panic(KApStartPanic, aPanic);
	}
