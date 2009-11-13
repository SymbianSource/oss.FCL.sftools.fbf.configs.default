/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  test ocrsrv.h
 *
*/

#ifndef TESTOCROBSERVER_H
#define TESTOCROBSERVER_H

#include <ocrsrv.h>

/**
 *  CTestNotesControl is used to add setting page control in its window
 */
class TTestOcrObserver : public MOCREngineObserver
    {
public:
    // from MOCREngineObserver
    void RecognizeBlockComplete( TInt /*aError*/, const TOCRTextRgnInfo* /*aBlocks*/, const TInt /*aBlockCount*/ ) {}
    void RecognizeSpecialRegionComplete( TInt /*aError*/, const TOCRTextRgnInfo* /*aBlocks*/, const TInt /*aBlockCount*/ ) {}
    void RecognizeComplete( TInt /*aError*/, const TOCRTextRgnInfo* /*aBlocks*/, const TInt /*aBlockCount*/ ) {}
    void LayoutComplete( TInt /*aError*/, const TOCRBlockInfo* /*aBlocks*/, const TInt /*aBlockCount*/ ) {}
    void RecognizeProcess( const TUint /*aPercent*/ ) {}
    };

#endif /*TESTOCROBSERVER_H*/
