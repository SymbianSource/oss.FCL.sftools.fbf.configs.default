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
// Security issues related methods used across apparc
// 
//

#include <e32std.h>
#include <e32cmn.h> //for TSecureId
#include "apsecutils.h"


/**
 * Check if application has a WriteDeviceData capability
 * and if it's SID is in the protected range
 * 
 * @param aAppFilename path to application exe file
 * @param aHasWriteDeviceDataCap returns ETrue if app has WriteDeviceData cap
 * @param aIsSidProtected returns ETrue if application SID is in the protected range
 * @return KErrNone if succesful, error code otherwise
 */
EXPORT_C TInt CApaSecurityUtils::CheckAppSecurity( const TPtrC& aAppFilename, 
                                              TBool& aHasWriteDeviceDataCap, 
                                              TBool& aIsSidProtected )
    { 
    aHasWriteDeviceDataCap = EFalse;
    aIsSidProtected = EFalse;
    
    RLibrary::TInfoBuf infoBuf;
    TInt ret = RLibrary::GetInfo( aAppFilename, infoBuf );
    if ( KErrNone == ret )
        {
        if ( infoBuf().iSecurityInfo.iCaps.HasCapability(
                                                ECapabilityWriteDeviceData ) )
            {
            aHasWriteDeviceDataCap = ETrue;
            }

        const TSecureId appSid = infoBuf().iSecurityInfo.iSecureId;

        if( IsSidProtected( TUint32( appSid ) ) )
            {
            aIsSidProtected = ETrue;
            }

        }
    return ret;
    }

//End of file
