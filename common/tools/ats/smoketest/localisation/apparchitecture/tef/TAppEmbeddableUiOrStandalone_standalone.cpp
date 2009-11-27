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
// "stub" to start the application architecture with the ECom UID
// The actual standalone file.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#include <e32std.h>
#include <eikstart.h>

const TUid KTAppEmbeddableUid = {0x10004c4A}; //use original UID of app (before it was converted from app to exe)
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(KTAppEmbeddableUid);
	}
