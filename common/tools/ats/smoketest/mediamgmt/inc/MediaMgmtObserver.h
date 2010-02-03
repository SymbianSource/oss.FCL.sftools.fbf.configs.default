/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:        ?Description
*
*/

#ifndef MEDIAMGMTOBSERVER_H
#define MEDIAMGMTOBSERVER_H

#include <MMGFetchVerifier.h>
#include <MMGFetchCanceler.h>

class MediaMgmtObserver : public MMGFetchCanceler, public MMGFetchVerifier
{
public:
	TBool VerifySelectionL( const MDesCArray* /*aSelectedFiles*/ );
	void CancelFetcherL();
};

#endif      // MEDIAMGMTOBSERVER_H