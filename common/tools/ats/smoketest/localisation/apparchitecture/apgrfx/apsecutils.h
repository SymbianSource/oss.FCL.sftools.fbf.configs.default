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

#ifndef __APSECUTILS_H__
#define __APSECUTILS_H__

/** AppArc security utils.

This is a set of security related methods for apparc internal usage.

@internalComponent 
*/
class CApaSecurityUtils
    {
public:
    IMPORT_C static TInt CheckAppSecurity( const TPtrC& aAppFilename, 
                                           TBool& aHasWriteDeviceDataCap, 
                                           TBool& aIsSidProtected );
private:
    inline static TBool IsSidProtected( const TUint32 aSid );
    };
    
#include "apsecutils.inl"
    
#endif // __APSECUTILS_H__
