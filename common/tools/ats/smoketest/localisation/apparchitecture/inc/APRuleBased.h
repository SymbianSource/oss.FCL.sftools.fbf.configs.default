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
// @file
// 
//

#if !defined(__APRULEBASED_H__)
#define __APRULEBASED_H__

#if !defined(__APADEF_H__)
#include <apadef.h>
#endif

#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif

/**
@publishedPartner
@released
Uid for access to all implementations of rule-based interface
*/
const TUid KUidRuleBasedInterfaceUid = { 0x1020d466 };

class CApaRuleBasedDll;
class CAppLaunchChecker;

class CApaRuleBasedDll;
/**
@publishedPartner
@released
Rule-based framework.
Loads all rule-based plug-ins available in the system and interrogates them 
whether or not an activated application should be launched
*/
class CApaScanningRuleBasedPlugIns : public CBase
	{
public:	
	IMPORT_C static CApaScanningRuleBasedPlugIns* NewL();
	IMPORT_C ~CApaScanningRuleBasedPlugIns();
	IMPORT_C CAppLaunchChecker* operator[](TInt aIndex) const;
	IMPORT_C void ScanForRuleBasedPlugInsL();
	IMPORT_C TInt ImplementationCount() const; 
private:
	CApaScanningRuleBasedPlugIns()
		{
		}
	void LoadEcomRuleBasedPlugInL(TUid aUid);
	HBufC* ImplementationDisplayNameL(TUid aImplUID) const;
	TInt RemovePlugIn(CApaRuleBasedDll* aDll);
private:
	RPointerArray<CApaRuleBasedDll> iRuleBasedLib; //all rule-based plug-ins available in the system
	};

#endif
