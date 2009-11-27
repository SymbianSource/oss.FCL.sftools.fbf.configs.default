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
// An application that uses more than the normally available amount of stack space.
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/



#include "tlargestackapp.h"

#include <eikstart.h>

	
GLDEF_C TInt E32Main()
	{
	TInt array[100000000]; 		// Create an array large enough to use up more than our stack allocation
	array[0]=1; 				// just a line to get rid of the warning in the build
	array[1]=array[0]+1;		// just a line to get rid of the warning in the build
	return KErrNone;
	}
