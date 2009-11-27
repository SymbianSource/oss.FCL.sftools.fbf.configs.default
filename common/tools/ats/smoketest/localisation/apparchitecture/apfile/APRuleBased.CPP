// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#include <aprulebased.h>
#include <aplaunchchecker.h>
#include "APFSTD.H" // Panics etc.

void CleanUpEComInfoArray(TAny* aInfoArray);

///////////////////////////////
// CApaRuleBasedDll
///////////////////////////////
/** The rule-based framework. Mantain all rule-based plug-ins available in the system */
class CApaRuleBasedDll : public CBase
	{
public:
	CApaRuleBasedDll();
	~CApaRuleBasedDll(); // closes lib and deletes next
public:
	enum TPresence { ENotPresent, EIsPresent };
	inline TPresence Present() const { return iPresent; }
	inline void Present(TPresence aPresence) { iPresent = aPresence; }
	inline TUid Uid() const { return iUid; }
	inline void Uid(TUid aUid) { iUid = aUid; }
	inline CAppLaunchChecker* LaunchChecker() { return iLaunchChecker; }
	inline void LaunchChecker(CAppLaunchChecker* aLaunchChecker) { iLaunchChecker = aLaunchChecker; }
private:
	TPresence iPresent;
	TUid iUid;
	CAppLaunchChecker* iLaunchChecker; // pointer to plug-in implementation
	};

CApaRuleBasedDll::CApaRuleBasedDll()
	{
	}

CApaRuleBasedDll::~CApaRuleBasedDll()
	{
	delete iLaunchChecker;
	}

/** The function provides access to launch checker interface by index */
EXPORT_C CAppLaunchChecker* CApaScanningRuleBasedPlugIns::operator[](TInt aIndex) const
	{
	return iRuleBasedLib[aIndex]->LaunchChecker();
	} 

EXPORT_C CApaScanningRuleBasedPlugIns* CApaScanningRuleBasedPlugIns::NewL()
	{
	CApaScanningRuleBasedPlugIns* self = new(ELeave) CApaScanningRuleBasedPlugIns();
	CleanupStack::PushL(self);
	self -> ScanForRuleBasedPlugInsL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CApaScanningRuleBasedPlugIns::~CApaScanningRuleBasedPlugIns()
	{
	iRuleBasedLib.ResetAndDestroy();
	}

/** The function loads the ecom rule-based plug-ins */
void CApaScanningRuleBasedPlugIns::LoadEcomRuleBasedPlugInL(TUid aUid)
	{
	// check we haven't loaded this one already
	for(TInt index = 0; index < iRuleBasedLib.Count(); index++)
		{
		CApaRuleBasedDll* lib = iRuleBasedLib[index];
		if (lib->Uid() == aUid)
			{// already found
			lib -> Present(CApaRuleBasedDll::EIsPresent);
			return;
			}			
		}

	// load the library
	CApaRuleBasedDll* lib = new(ELeave) CApaRuleBasedDll();
	CleanupStack::PushL(lib);
	TRAPD(error, lib -> LaunchChecker(CAppLaunchChecker::NewL(aUid)));
	if((error == KErrNone) && lib -> LaunchChecker())
		{
		lib->Present(CApaRuleBasedDll::EIsPresent);
		lib->Uid(aUid);

		// add lib to the library list
		iRuleBasedLib.Append(lib);
		CleanupStack::Pop(lib);
		}
	else 
		{
		CleanupStack::PopAndDestroy(lib);
		}
	}

/** The function scans and initializes all rule-based plug-ins available in the system */
EXPORT_C void CApaScanningRuleBasedPlugIns::ScanForRuleBasedPlugInsL()
	{
	// set all rule based plug-ins to not present - pending rediscoivery
	TInt index;
	for (index = 0; index < iRuleBasedLib.Count(); index++)
		{
		CApaRuleBasedDll* rule = iRuleBasedLib[index];
		rule -> Present(CApaRuleBasedDll::ENotPresent);
		}
	
	//scan ecom plugin
	RImplInfoPtrArray implementationArray;
	TCleanupItem cleanup(CleanUpEComInfoArray, &implementationArray);
	CleanupStack::PushL(cleanup);
	REComSession::ListImplementationsL(KUidRuleBasedInterfaceUid, implementationArray);
	const TInt availCount = implementationArray.Count();
	for (TInt count = 0; count < availCount; ++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		TUid uid = info -> ImplementationUid();
		LoadEcomRuleBasedPlugInL(uid);
		}
	CleanupStack::PopAndDestroy(&implementationArray);
	
	// remove any rule based plug-ins that are no longer present
	index = 0;
	while (index < iRuleBasedLib.Count())
		{
		CApaRuleBasedDll* rule = iRuleBasedLib[index];
		if (rule -> Present() == CApaRuleBasedDll::ENotPresent)
			{
			iRuleBasedLib.Remove(index);
			delete rule;
			}
		else
			{
			index++;
			}
		}
	}

/** The function returns number of plug-ins available in the system*/
EXPORT_C  TInt CApaScanningRuleBasedPlugIns::ImplementationCount() const
	{
	return iRuleBasedLib.Count();
	}

	
