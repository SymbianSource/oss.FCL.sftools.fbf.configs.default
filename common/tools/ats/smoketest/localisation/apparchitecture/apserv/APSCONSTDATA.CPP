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

#include <e32def.h>

/*
 * patchable const data values
 * 
 * these values control whether apparc tries to save memory by unloading data
 * recognizers when they are not being used
 *
 * To patch these values, add a line to an iby or oby file that is included in the rom being built using the following format:
 *
 * "patchdata <dll> @ <symbol> <newvalue>"
 *
 * e.g. to enable on-demand recognizer loading, use the following line:
 * "patchdata apserv.dll @ KApaLoadDataRecognizersOnDemand 1"
 */


/**
 * Patchable constant.  Determines whether, in the interest of saving memory, AppArc unloads data 
 * recognizers when they are not being used.
 *
 * @SYMPatchable
 * @see KApaUnloadRecognizersTimeout 
 *
 */
EXPORT_C extern const TInt KApaLoadDataRecognizersOnDemand = 0;

/**
 * Patchable constant.  Determines how long (in microseconds) a recognizer may remain unused before it gets deleted.
 * 
 * @SYMPatchable
 * @see KApaLoadDataRecognizersOnDemand 
 *
 */ 
EXPORT_C extern const TInt KApaUnloadRecognizersTimeout = 10000000; // 10 seconds in microseconds


/* 
 * emulator support
 * to enable on-demand recognizer loading on the emulator, add the following line to your epoc.ini
 * patchdata_apserv_dll_KApaLoadDataRecognizersOnDemand 1
 */

/*
 * Patchable constant.  
 *
 * @SYMPatchable
 * @see KApaDrivesToMonitor 
 *
 */

EXPORT_C extern const TInt KApaDrivesToMonitor = 0x3FFFFFF; 
/*
The constant value 0x3FFFFFF represents that all drives (z->a(Least Significant Bit)) are monitored by default.
It is computed in the following way.
	        ZY XWVU TSRQ PONM LKJI HGFE DCBA
	 		11 1111 1111 1111 1111 1111 1111. 
If the bit representing a drive is set (to 1) it is monitored for file change notifications. 
Else it is not. 
*/

/** 
 * Patchable constant.  Determines whether apparc tries to save memory in the font bitmap
 * server, by only loading MBM icons on demand.
 * The default value is 0.
 * When set to zero, all MBM icons are loaded at device boot and never unloaded.
 * When set to non-zero, no MBM icons are loaded at device boot. MBM icons are
 * loaded on demand (and never unloaded afterwards). 
 * 
 * @SYMPatchable
 */
EXPORT_C extern const TInt KApaLoadMbmIconsOnDemand = 0;
