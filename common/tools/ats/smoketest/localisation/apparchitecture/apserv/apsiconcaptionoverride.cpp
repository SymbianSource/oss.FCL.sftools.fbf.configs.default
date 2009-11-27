// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @internalComponent
*/

#include "apsiconcaptionoverride.h"
#include <centralrepository.h>
#include <apadef.h>

//constants defined
const TUid KUidIconCaptionRepository = { 0x1028583d }; // Central Repository UID
const TUint32 KAppBits = 0xFF000000; // App mask bits
const TUint32 KFieldBits = 0x00FF0000; // Field mask bits
const TUint32 KUidKey = 0x00FFFFFF; // UID mask bits

// Field id values for short caption, caption, icon count, icon file name
// of an application.
enum TFieldId
	{
	EOverrideFieldShortCaption,
	EOverrideFieldCaption,
	EOverrideFieldNumIcons,
	EOverrideFieldIconFileName
	};

/** 
Stores the Central Repository configuration details into a Hash table.
Two Hash tables defined to store integer and string type data separately.
*/
NONSHARABLE_CLASS(CApaIconCaptionOverrideStore) : public CBase
	{
public:
	CApaIconCaptionOverrideStore();
	~CApaIconCaptionOverrideStore();
	
	const TDesC* String(TUint32 aKey) const;
	void SetStringL(TUint32 aKey, const TDesC& aString);
	const TInt* Int(TUint32 aKey) const;
	void SetIntL(TUint32 aKey, TInt aInt);
	
private:
	typedef RHashMap<TUint32, TInt> RIntMap;
	RIntMap iIntMap;
	typedef RHashMap<TUint32, HBufC*> RStringMap;
	RStringMap iStringMap;
	};



// Constructor
CApaIconCaptionOverrideStore::CApaIconCaptionOverrideStore()
	{
	}

// Destructor
CApaIconCaptionOverrideStore::~CApaIconCaptionOverrideStore()
	{
	RStringMap::TIter pString(iStringMap);
	for (HBufC* const* str = pString.NextValue();
		 str;
		 str = pString.NextValue())
		 delete *str;
	iStringMap.Close();
	iIntMap.Close();
	}

/** 
Looks up a specified key in the associative array and returns a pointer to the corresponding value.

@param aKey The key object of type integer to look up.
@return A pointer to the corresponding string value.  
*/
const TDesC* CApaIconCaptionOverrideStore::String(TUint32 aKey) const
	{
	HBufC* const * pStr = iStringMap.Find(aKey);
	if (pStr)
		return *pStr;
	else
		return NULL;
	}

/**
Inserts the key-value pair into the array (string Hash table).

@param aKey The key object of type integer to add to the array.
@param aString The value object of type string to associate with aKey.
*/
void CApaIconCaptionOverrideStore::SetStringL(TUint32 aKey, const TDesC& aString)
	{
	HBufC* newStr = aString.AllocL();
	HBufC* const * pStr = iStringMap.Find(aKey);
	CleanupStack::PushL(newStr);
	if (pStr)
		{
		delete *pStr;
		}
	iStringMap.InsertL(aKey, newStr);
	CleanupStack::Pop(newStr);
	}

/**
Looks up a specified key in the associative array and returns a pointer to the corresponding value.

@param aKey The key object of type integer to look up
@return A pointer to the corresponding integer value.
*/
const TInt* CApaIconCaptionOverrideStore::Int(TUint32 aKey) const
	{
	const TInt * pInt = iIntMap.Find(aKey);
	return pInt;
	}

/**
Inserts the key-value pair into the array (integer Hash table)

@param aKey The key object of type integer to add to the array.
@param aInt The value object of type integer to associate with aKey.
*/
void CApaIconCaptionOverrideStore::SetIntL(TUint32 aKey, TInt aInt)
	{
	iIntMap.InsertL(aKey, aInt);
	}

/**
A utility class used to write information into a store (CApaIconCaptionOverrideStore).
*/
NONSHARABLE_CLASS(TApaIconCaptionOverrideWriter)
	{
public:
	TApaIconCaptionOverrideWriter(CApaIconCaptionOverrideStore& aStore);

	void LoadFieldFromCenRepL(CRepository* aRepository, TUint32 aFullKey);

private:
	CApaIconCaptionOverrideStore& iStore;
	};


TApaIconCaptionOverrideWriter::TApaIconCaptionOverrideWriter(CApaIconCaptionOverrideStore& aStore)
: iStore(aStore)
	{
	}

/**
Reads the Central Repository integer, string settings and inserts the key-value pair into Store.

Each setting is enumerated by a 32-bit key. The top 8 bits of the key is for app identification,
the next 8 bits is for field (short caption (00), caption (01),total number of icons (02), 
icon filename (03)) identification and next 16 bits for language identification.

It is computed in the following way

	00XXXXXX - App UID field
	XX00XXXX - Short Caption field
	XX01XXXX - Caption field
	XX02XXXX - Icon count field
	XX03XXXX - Icon Filename field
	XXXX0000 - Language field

First it performs a bitwise NOT operation on application mask key (KAppBits) and its value is bitwise
AND with the 32 bit key of the field.

It performs a bitwise AND with the field mask key (KFieldBits) and does a bitwise right shift to 16 bits
to extract the field bit value from the 32 bit key.

It reads each field's (short caption, caption, total number of icons, icon file name) value from the
Central Repository and inserts into a Store (Hash table).

@param aRepository The object provides access to Central Repository
@param aFullKey The 32-bit field key
*/
void TApaIconCaptionOverrideWriter::LoadFieldFromCenRepL(CRepository* aRepository, TUint32 aFullKey)
	{
	TUint32 key = aFullKey & ~KAppBits;
	TUint32 fieldId = (key & KFieldBits) >> 16;
	switch (fieldId)
		{
		case EOverrideFieldShortCaption:
			{
			TApaAppCaption shortCaption;
			if (aRepository->Get(aFullKey, shortCaption) == KErrNone)
				iStore.SetStringL(key, shortCaption);
			break;
			}
		case EOverrideFieldCaption:
			{
			TApaAppCaption caption;
			if (aRepository->Get(aFullKey, caption) == KErrNone)
				iStore.SetStringL(key, caption);
			break;
			}
		case EOverrideFieldNumIcons:
			int numIcons;
			if (aRepository->Get(aFullKey, numIcons) == KErrNone)
				iStore.SetIntL(key, numIcons);
			break;
		case EOverrideFieldIconFileName:
			{
			TFileName fileName;
			if (aRepository->Get(aFullKey, fileName) == KErrNone)
				iStore.SetStringL(key, fileName);
			break;
			}
		default:
			break;
		}
	}

// Constructor
TApaIconCaptionOverrideReader::TApaIconCaptionOverrideReader(const CApaIconCaptionOverrideStore& aStore, const RArray<TLanguage>& aLanguageDowngradePath)
: iStore(aStore), iLanguageDowngradePath(aLanguageDowngradePath)
	{
	}

/**
Gets the short caption value of the corresponding setting from store.
*/
const TDesC* TApaIconCaptionOverrideReader::ShortCaption() const
	{
	return GetString(EOverrideFieldShortCaption);
	}

/**
Gets the caption value of the corresponding setting from store.
*/
const TDesC* TApaIconCaptionOverrideReader::Caption() const
	{
	return GetString(EOverrideFieldCaption);
	}

/**
Checks whether the number of icons are set.
@return ETrue when the number of icons are set else returns EFalse.  
*/
TBool TApaIconCaptionOverrideReader::NumIconsSet() const
	{
	return GetInt(EOverrideFieldNumIcons) != NULL;
	}

/**
Gets the total number of icons of the corresponding setting from store.
*/
TInt TApaIconCaptionOverrideReader::NumIcons() const
	{
	const TInt* pInt = GetInt(EOverrideFieldNumIcons);
	if (pInt)
		return *pInt;
	else
		return 0;
	}

/**
Gets the icon file name of the corresponding setting from store.
*/
const TDesC* TApaIconCaptionOverrideReader::IconFileName() const
	{
	return GetString(EOverrideFieldIconFileName);
	}

/**
Reads store and retrieves the corresponding string value of the passed field Id.

Gets the configuration information with reference to the language downgrade path, with
ELangNone as default language, in case there is no match is found.

@param aFieldId Field Id values for short caption, caption, icon count, icon file name.
@return A pointer to the corresponding string value.
*/
const TDesC* TApaIconCaptionOverrideReader::GetString(TUint32 aFieldId) const
	{
	TInt count = iLanguageDowngradePath.Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TLanguage language = iLanguageDowngradePath[ii];
		TUint32 key = (aFieldId << 16) | language;
		const TDesC* pString = iStore.String(key);
		if (pString)
			return pString;
		}
	// default fallback language is ELangNone
	TUint32 key = (aFieldId << 16) | ELangNone;
	const TDesC* pString = iStore.String(key);
	if (pString)
		return pString;
	return NULL;
	}

/**
Reads store and retrieves the corresponding integer value of the passed field Id.

Gets the configuration information with reference to the language downgrade path, with
ELangNone as default language, in case there is no match is found.

@param aFieldId Field Id values for short caption, caption, icon count, icon file name.
@return A pointer to the corresponding integer value.
*/
const TInt* TApaIconCaptionOverrideReader::GetInt(TUint32 aFieldId) const
	{
	TInt count = iLanguageDowngradePath.Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TLanguage language = iLanguageDowngradePath[ii];
		TUint32 key = (aFieldId << 16) | language;
		const TInt* pInt = iStore.Int(key);
		if (pInt)
			return pInt;
		}
	// default fallback language is ELangNone
	TUint32 key = (aFieldId << 16) | ELangNone;
	const TInt* pInt = iStore.Int(key);
	if (pInt)
		return pInt;
	return NULL;
	}

// Constructor
CApaIconCaptionOverridesForApp::CApaIconCaptionOverridesForApp()
	{
	}

// Destructor
CApaIconCaptionOverridesForApp::~CApaIconCaptionOverridesForApp()
	{
	delete iStore;
	}

CApaIconCaptionOverridesForApp* CApaIconCaptionOverridesForApp::NewL()
	{
	CApaIconCaptionOverridesForApp* self = new (ELeave) CApaIconCaptionOverridesForApp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

/** 2nd phase constructor that creates an object to store the Central Repository configuration
details (integer & string values) into separate Hash tables.
*/
void CApaIconCaptionOverridesForApp::ConstructL()
	{
	iStore = new(ELeave) CApaIconCaptionOverrideStore;
	}

/**
Finds all the settings that exist and match the specification given by partial key (aAppKey) and
application mask key (KAppBits) from the Central Repository.

All the 32-bit field keys have been retrieved from the Central Repository and stored into an array.
It performs a bitwise NOT operation on application mask key (KAppBits) and its value is bitwise AND
with each of the value retrieved from the array. If the new value does not match with the UID mask key,
it tries to read each field setting value and stores the information into a store (Hash tables).

@param aRepository The object provides access to Central Repository.
@param aAppKey partial key.
*/
void CApaIconCaptionOverridesForApp::LoadFromCenRepL(CRepository* aRepository, TUint32 aAppKey)
	{
	TApaIconCaptionOverrideWriter writer(*iStore);
	RArray<TUint32> appKeys;
	CleanupClosePushL(appKeys);
	User::LeaveIfError(aRepository->FindL(aAppKey, KAppBits, appKeys));
	TInt count = appKeys.Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TUint32 key = appKeys[ii];
		if ((key & ~KAppBits) == KUidKey)
			continue;
		writer.LoadFieldFromCenRepL(aRepository, key);
		}
	CleanupStack::PopAndDestroy(&appKeys);	
	}

/**
Creates an object to read the configuration information with reference to the language downgrade path,
with ELangNone used as the default language, if no better match is found.
*/
TApaIconCaptionOverrideReader CApaIconCaptionOverridesForApp::Reader(const RArray<TLanguage>& aLanguageDowngradePath) const
	{
	TApaIconCaptionOverrideReader reader(*iStore, aLanguageDowngradePath);
	return reader;
	}

// Constructor
CApaIconCaptionOverrides::CApaIconCaptionOverrides()
	{
	}

// Destructor
CApaIconCaptionOverrides::~CApaIconCaptionOverrides()
	{
	DeleteOverrides();
	}

void CApaIconCaptionOverrides::LoadFromCenRepL()
	{
	CRepository* cenrep = CRepository::NewLC(KUidIconCaptionRepository);
	LoadFromCenRepL(cenrep);
	CleanupStack::PopAndDestroy(cenrep);
	}

/**
Finds all the settings that exist and match the specification given by partial and mask keys, from the
Central Repository. If the settings are found, it reads the values and stores icon, caption details
into the store.
*/
void CApaIconCaptionOverrides::LoadFromCenRepL(CRepository* aRepository)
	{
	DeleteOverrides();

	RArray<TUint32> appKeys;
	CleanupClosePushL(appKeys);
	aRepository->FindL(KUidKey, ~KAppBits, appKeys);	// ignore return. if no overrides found, array will be empty
	TInt count = appKeys.Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TUint32 key = appKeys[ii];
		TInt appUidVal;
		User::LeaveIfError(aRepository->Get(key, appUidVal));
		TUid appUid = { appUidVal };
		CApaIconCaptionOverridesForApp* app = CApaIconCaptionOverridesForApp::NewL();
		CleanupStack::PushL(app);
		app->LoadFromCenRepL(aRepository, key);
		SetOverrideForAppL(appUid, app);
		CleanupStack::Pop(app);
		}
	CleanupStack::PopAndDestroy(&appKeys);
	}

/**
Searches Hash table that keeps a map of app UIDs to configuration information for supplied application
and returns a pointer to an object that keeps information of icon caption overrides for an application, if
any suitable entry is found; otherwise NULL.
*/
CApaIconCaptionOverridesForApp* CApaIconCaptionOverrides::OverrideForApp(TUid aAppUid) const
	{
	CApaIconCaptionOverridesForApp* const * override = iMap.Find(aAppUid.iUid);
	if (override)
		return *override;
	else
		return NULL;
	}

/**
Inserts key-value pair into an array (Hash table), if found, updates it.
*/
void CApaIconCaptionOverrides::SetOverrideForAppL(TUid aAppUid, CApaIconCaptionOverridesForApp* aOverrides)
	{
	//Searches the Hash table with the app UID, if it's found, it returns the corresponding entry;
	CApaIconCaptionOverridesForApp* prev = OverrideForApp(aAppUid);
	if (aOverrides)
		{
		iMap.InsertL(aAppUid.iUid, aOverrides);
		}
	else
		{
		iMap.Remove(aAppUid.iUid);
		}
	delete prev; // delete the found entry as InsertL() replaces it.
	}

/**
Removes all the mapping done with app UIDs in Hash table for the icon caption overrides.
*/
void CApaIconCaptionOverrides::DeleteOverrides()
	{
	RAppOverrideMap::TIter pOverride(iMap);
	for (CApaIconCaptionOverridesForApp* const* override = pOverride.NextValue();
		 override;
		 override = pOverride.NextValue())
		 delete *override;
	iMap.Close();
	}


CApaIconCaptionCenrepObserver::CApaIconCaptionCenrepObserver(CApaIconCaptionOverrides& aOverrides)
: CActive(EPriorityStandard), iOverrides(aOverrides)
	{
	CActiveScheduler::Add(this);
	}
	
// Creates a Central Repository observer
CApaIconCaptionCenrepObserver* CApaIconCaptionCenrepObserver::NewL(CApaIconCaptionOverrides& aOverrides)
	{
	CApaIconCaptionCenrepObserver* self = new (ELeave) CApaIconCaptionCenrepObserver(aOverrides);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// Destructor
CApaIconCaptionCenrepObserver::~CApaIconCaptionCenrepObserver()
	{
	Cancel();
	delete iRepository;
	}

/** 2nd phase constructor that creates a Central Repository object to access the repository with the
specified UID and starts notification.
*/
void CApaIconCaptionCenrepObserver::ConstructL()
	{
	TRAP_IGNORE(iRepository = CRepository::NewL(KUidIconCaptionRepository));
	Start();
	}

/**
Finds all the settings that exist and match the specification given by partial and mask keys, from the
Central Repository, if it exists.
*/
void CApaIconCaptionCenrepObserver::LoadOverridesL()
	{
	if (iRepository)
		iOverrides.LoadFromCenRepL(iRepository);
	}

/**
Requests a notification on any change in the Central Repository. Only one notification can be
received per call to NotifyRequest().
*/
void CApaIconCaptionCenrepObserver::Start()
	{
	if (iRepository)
		{
		iRepository->NotifyRequest(0, 0, iStatus);	// notify on all key changes
		SetActive();
		}
	}

/**
Handles an active object's request completion event.
*/
void CApaIconCaptionCenrepObserver::RunL()
	{
	TInt err = iStatus.Int();
	Start();
	User::LeaveIfError(err);
	LoadOverridesL();
	}
	
/**
Cancels a notification previously requested from NotifyRequest
*/
void CApaIconCaptionCenrepObserver::DoCancel()
	{
	iRepository->NotifyCancel(0, 0);
	}
	

TInt CApaIconCaptionCenrepObserver::RunError(TInt /*aError*/)
	{
	//Return KErrNone if RunL leaves to avoid a E32User-CBase 47 panic.
	return KErrNone;
	}
