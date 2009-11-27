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
//

#include <e32def.h>

/*
 * patchable const data values
 * 
 * To patch these values, add a line to an iby or oby file that is included in the rom being built using the following format:
 *
 * "patchdata <dll> @ <symbol> <newvalue>"
 *
 * e.g. to enable on-demand recognizer loading, use the following line:
 * "patchdata apserv.dll @ KApaLoadDataRecognizersOnDemand 1"
 */

/* The default value here is zero. A more appropriate value should 
   be set in the SystemGUI via the patchable constant.
*/
EXPORT_C extern const TInt KMinApplicationStackSize = 0;
