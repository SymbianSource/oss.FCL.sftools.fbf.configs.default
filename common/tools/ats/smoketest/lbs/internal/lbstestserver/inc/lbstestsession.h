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
// serverside.h
// server side exmaple of how to use the abstract server framework
// 
//

#ifndef LBSTESTSESSION_H__
#define LBSTESTSESSION_H__

#include "csecuresessionbase.h"
#include "csecuresessionsubsessionbase.h"
#include "csubsessionbase.h"
#include "messageutils.h"

	
/* Session class that implements the actual functions.
*/	
class CLbsTestSession : public CSecureSessionBase
	{
public:
	void CreateL();

protected:
	void DispatchL(const RMessage2& aMessage);
	void DispatchError(const RMessage2& aMessage, const TInt aError);

private:
	void StopLbsSystem(const RMessage2& aMessage);
	void StartLbsSystem(const RMessage2& aMessage);
	void ResetLbsSystem(const RMessage2& aMessage);
	
	TBool FindRootProcess(RProcess& aProcess);
	};
	
#endif //LBSTESTSESSION_H__
