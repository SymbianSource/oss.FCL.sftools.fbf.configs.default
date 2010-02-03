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

#ifndef APASERVERAPPPRIV_H
#define APASERVERAPPPRIV_H

#include "ApaServerApp.h"


// client/server command ids for RApaAppServiceBase and CApaAppServiceBase
enum TApaAppServerCommandId
	{
	EApaAppServerNotifyServerExit = 1,
	EApaAppServerCancelNotifyServerExit
	};


void PanicAppServerClient(const RMessagePtr2& aMessage,TInt aCode);


#endif
