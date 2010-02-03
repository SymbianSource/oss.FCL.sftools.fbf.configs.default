// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __APSECUTILS_INL__
#define __APSECUTILS_INL__

/** 
 * Determines whether an EXE SID is within the protected range\d
 * 
 * @param aSid The SID to check
 * @return ETrue if it is protected
 */
inline TBool CApaSecurityUtils::IsSidProtected( const TUint32 aSid )
     {
     // If bit 31 is set then Sid is unprotected
     return (aSid & 0x80000000) ? EFalse : ETrue;
     }

#endif // __APSECUTILS_INL__
