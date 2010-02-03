// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestMessSession.h"

CTestMessSession::CTestMessSession(CTestMessSharedData* aSharedData)
:	CTestSession()
,	iSharedData(aSharedData)
	{
	}


void CTestMessSession::ServiceL(const RMessage2& aMessage)
	{
	CTestSession::ServiceL(aMessage);
	switch(aMessage.Function())
		{
		case EOpenTestStep :
			{
			}
			break;
		case ERunTestStep :
			{
			iSharedData->DoProcessL();
			}
			break;
		case EAbortTestStep :
			{
			}
			break;
		case ECloseTestStep :
			{
			}
			break;
		default:
			break;
		}
	}
