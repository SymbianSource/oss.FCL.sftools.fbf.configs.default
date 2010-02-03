// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "APASTD.H"
#include <f32file.h>

GLDEF_C void Panic(TApaPanic aPanic)
//
// Panic the process with APPARC as the category.
//
	{
	_LIT(KAppArcPanic,"APPARC");
	User::Panic(KAppArcPanic,aPanic);
	}
