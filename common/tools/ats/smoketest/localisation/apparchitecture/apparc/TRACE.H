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
// peppers apparc code with RDebug::Print()'s under certain circumstances...
//

// #define __SHOW_TRACE(aString) RDebug::Print(aString)
//



#define __SHOW_TRACE(aString)

/*
#define __APA_PROFILE_START(aNum) RDebug::ProfileStart(aNum)
#define __APA_PROFILE_END(aNum) RDebug::ProfileEnd(aNum)
*/
#define __APA_PROFILE_START(aNum)
#define __APA_PROFILE_END(aNum)
