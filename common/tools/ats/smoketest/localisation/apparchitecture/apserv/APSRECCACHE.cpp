// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "APSRECCACHE.h"
#include "APSSES.H"

//////////////////////////////////////////////////////////////////////////////
// Constants

const TUint KMaxNumberOfEntries = 200;
//////////////////////////////////////////////////////////////////////////////

CRecognitionResultHashMapEntry* CRecognitionResultHashMapEntry::NewL(const TDesC& aFileName, TTime aLastModified, const TDataRecognitionResult& aResult, CRecognitionResultHashMapEntry* aNext)
	{
	CRecognitionResult* result = CRecognitionResult::NewL(aFileName, aResult);
	CleanupClosePushL(*result);
	CRecognitionResultHashMapEntry* self = new(ELeave) CRecognitionResultHashMapEntry(aLastModified, result, aNext);
	CleanupStack::Pop(result);	
	return self;
	}

CRecognitionResultHashMapEntry::CRecognitionResultHashMapEntry(TTime aLastModified, CRecognitionResult* aResult, CRecognitionResultHashMapEntry* aNext)
	: iLastModified(aLastModified), iResult(aResult), iNext(aNext)
	{
	}

CRecognitionResultHashMapEntry::~CRecognitionResultHashMapEntry()
	{
	iResult->Close();
	iResult = NULL;
	delete iNext;
	}

void CRecognitionResultHashMapEntry::UpdateL(TTime aLastModified, const TDataRecognitionResult& aResult)
	{
	// since other objects  might have a pointer to the CRecognitionResult object
	// and rely on the value not changing we need to create a new object to take 
	// its place.
	
	CRecognitionResult* result = CRecognitionResult::NewL(FileName(), aResult);
	iResult->Close();
	iResult = result;
	
	iLastModified = aLastModified;
	}

///////////////////////////////////////////////////////////////////////
// CRecognitionResultHashMap
///////////////////////////////////////////////////////////////////////

CRecognitionResultHashMap::CRecognitionResultHashMap()
	{
	}

CRecognitionResultHashMap::~CRecognitionResultHashMap()
	{
	for(TUint i = 0; i < KFileHashMapEntries; i++)
		{
		delete Entry(i);
		}
	}

CRecognitionResult* CRecognitionResultHashMap::Get(const TDesC& aKey, TTime& aLastModified) const
	{
	const TUint index = GetIndex(aKey);

	for(const CRecognitionResultHashMapEntry* entry = Entry(index); entry; entry = entry->Next())
		{
		if(entry->FileName().Compare(aKey) == 0)
			{
			aLastModified = entry->LastModified();
			return entry->Result();
			}
		}
	return NULL;
	}

/**
Adds a recognition result to the hash map of the particular directory.
This is only done if it isn't already existing.
The return value specifies if the number of entries was increased by one (ETrue) or not.
@internalComponent
*/
TBool CRecognitionResultHashMap::AddL(const TDesC& aKey, TTime aLastModified, const TDataRecognitionResult& aResult)
	{
	const TUint index = GetIndex(aKey);

	//check if there already is an entry for the file
	for(CRecognitionResultHashMapEntry* entry = Entry(index); entry; entry = entry->Next())
		{
		if(entry->FileName().Compare(aKey) == 0)
			{
			// already there -> check modification date
			if( entry->LastModified() != aLastModified )
				{
				entry->UpdateL(aLastModified, aResult);
				}

			return EFalse;
			}
		}

	//create new entry
	SetEntry(index, CRecognitionResultHashMapEntry::NewL(aKey, aLastModified, aResult, Entry(index)));
	iNumberOfEntries++;
	return ETrue;
	}
	
TUint CRecognitionResultHashMap::GetIndex(const TDesC& aKey) const
	{
	// That's the hash algorithm
	TUint hash = aKey.Length();
	for(TUint i=0;i< aKey.Length();i++)
		{
		hash += aKey[i];
		}
	return hash % KFileHashMapEntries;
	}
	

///////////////////////////////////////////////////////////////////////
// CCacheDirectoryEntry
///////////////////////////////////////////////////////////////////////

CCacheDirectoryEntry* CCacheDirectoryEntry::NewL(const TDesC& aDirectory)
	{
	CCacheDirectoryEntry* self = new(ELeave) CCacheDirectoryEntry;
	CleanupStack::PushL(self);
	self->iDirectory = aDirectory.AllocL();
	CleanupStack::Pop(self);
	return self;
	}

CCacheDirectoryEntry::CCacheDirectoryEntry()
	{
	}

CCacheDirectoryEntry::~CCacheDirectoryEntry()
	{
	delete iDirectory;
	}

const TInt CCacheDirectoryEntry::iOffset = _FOFF(CCacheDirectoryEntry,iDlink);

///////////////////////////////////////////////////////////////////////
// CApsRecognitionCache
///////////////////////////////////////////////////////////////////////

CApsRecognitionCache::CApsRecognitionCache(RFs& aFs)
	: iFs(aFs),
	  iDirectoryHeader(CCacheDirectoryEntry::iOffset),
	  iIter(iDirectoryHeader)
	{
	}

CApsRecognitionCache::~CApsRecognitionCache()
	{
    CCacheDirectoryEntry* anyitem;
    
    iIter.SetToFirst();
    while ((anyitem = iIter++) != NULL)
        {
        anyitem->iDlink.Deque();
        delete anyitem;
        };
	}

/**
Adds a recognition result to the cache. An object is not added, when an up-to-date
version is already in the cache or if it's not possible to check the file's LastModified().
@internalComponent
*/
void CApsRecognitionCache::AddL(const TDesC& aDirectory, const TDesC& aFileName, const TDataRecognitionResult& aRecognitionResult)
	{
	TTime lastModified;
	TFileName fullFileName(aDirectory);
	fullFileName.Append(aFileName);
	if(iFs.Modified(fullFileName, lastModified) != KErrNone)
		{
	#if defined(_DEBUG)
		RDebug::Print(_L("CApsRecognitionCache::AddL(): File '%S' was not added to the cache, cannot get the modified attribute (full filename is required for the cache to work)."), &fullFileName);
	#endif
		return;
		}
	DoAddL(aDirectory, aFileName, lastModified, aRecognitionResult);
	}
 
/**
Adds a recognition result to the cache. An object is not added, when an up-to-date
version is already in the cache or if it's not possible to check the file's LastModified().
@internalComponent
*/
void CApsRecognitionCache::AddL(const RFile& aFile, const TDesC& aDirectory, const TDesC& aFileName, const TDataRecognitionResult& aRecognitionResult)
	{
	TTime lastModified;
	if(aFile.Modified(lastModified) != KErrNone)
		{
	#if defined(_DEBUG)
		TFileName fullFileName(aDirectory);
		fullFileName.Append(aFileName);
		RDebug::Print(_L("CApsRecognitionCache::AddL(): File '%S' was not added to the cache, cannot get the modified attribute."), &fullFileName);
	#endif
		return;
		}
	DoAddL(aDirectory, aFileName, lastModified, aRecognitionResult);
	}

/**
Adds a recognition result to the cache. An object is not added, when an up-to-date
version is already in the cache or if it's not possible to check the file's LastModified().
@internalComponent
*/
void CApsRecognitionCache::DoAddL(const TDesC& aDirectory, const TDesC& aFileName, const TTime& aLastModified, const TDataRecognitionResult& aRecognitionResult)
	{
	if(iNumberOfEntries > KMaxNumberOfEntries)
		{
		Cleanup();
		}
		
	CCacheDirectoryEntry* entry = NULL;
    iIter.SetToFirst();
    while ((entry = iIter++) != NULL)
    	{
		if(CompareDirectories(entry->Directory(),aDirectory))
			{
			// move directory to top (it is likely to be used again soon)
			entry->iDlink.Deque();
			iDirectoryHeader.AddFirst(*entry);
			break;
			}
    	}

	if(!entry)
		{
		// create directory and append it
		entry = CCacheDirectoryEntry::NewL(aDirectory);
		iDirectoryHeader.AddFirst(*entry);
		}

	// insert to correct directory
	if(entry->Files().AddL(aFileName, aLastModified, aRecognitionResult))
		{
		iNumberOfEntries++;
		}
	}

/**
Searches the cache for the particular recognition result. If the file was not modified since the
file was recognized, the stored recognition result is returned.
@internalComponent
*/
TBool CApsRecognitionCache::Get(const TDesC& aDirectory, const TDesC& aFileName, TDataRecognitionResult& aRecognitionResult)
	{
	TTime lastModified;
	TFileName fileName(aDirectory);
	fileName.Append(aFileName);
	const TInt error = iFs.Modified(fileName, lastModified);
	if(error != KErrNone) 
		{
		return EFalse;
		}

	CRecognitionResult* result = DoGet(aDirectory, aFileName, lastModified);
	if(result)
		{
		result->Get(aRecognitionResult);
		result->Close(); // decrease reference count since we're not exposing the object
		return ETrue;
		}
	return EFalse;
	}

/**
Searches the cache for the particular recognition result. If the file was not modified since the
file was recognized, the stored recognition result is returned.

N.B. The returned value is reference counted!
@internalComponent
*/
CRecognitionResult* CApsRecognitionCache::Get(const RFile& aFile, const TDesC& aDirectory, const TDesC& aFileName)
	{	
	TTime lastModified;
	const TInt error = aFile.Modified(lastModified);
	if(error != KErrNone)
		{
		return NULL;
		}

	return DoGet(aDirectory, aFileName, lastModified);
	}

/**
Searches the cache for the particular recognition result. If the file was not modified since the
file was recognized, the stored recognition result is returned.

N.B. The returned value is reference counted!
@internalComponent
*/
CRecognitionResult* CApsRecognitionCache::DoGet(const TDesC& aDirectory, const TDesC& aFileName, const TTime& aLastModified)
	{	
	CRecognitionResult* result = NULL;
	CCacheDirectoryEntry* entry;

    iIter.SetToFirst();
    while ((entry = iIter++) != NULL)
    	{
    	if(CompareDirectories(entry->Directory(),aDirectory))
    		{
    		TTime cachedLastModified;
			result = entry->Files().Get(aFileName, cachedLastModified);
			if(result)
				{
				if(cachedLastModified != aLastModified)
					{
					result->Close();
					result = NULL;
					}
				}

			// move directory to top (it is likely to be used again soon)
			entry->iDlink.Deque();
			iDirectoryHeader.AddFirst(*entry);
			break;
    		}
    	}
	return result;
	}

/**
A fast comparison of two directory names.
Should be faster than the "normal" TDesC16::Compare function, because
there is no need to check which string is "greater".
@internalComponent
*/
TBool CApsRecognitionCache::CompareDirectories(const TDesC& aDir1, const TDesC& aDir2) const
	{
	TBool equal = ETrue;
	if(aDir1.Length() == aDir2.Length())
		{
		// start with the last character, as directories tend to be the same at the beginning
		for(TInt i=aDir1.Length()-1; i>=0; i--)
			{
			if(aDir1[i] != aDir2[i])
				{
				// return as soon one character is "wrong"
				equal = EFalse;
				break;
				}
			}
		}
	else
		{
		equal = EFalse;
		}
	return equal;
	}
/**
Removes the least-often used directory in the cache.
@internalComponent
*/
void CApsRecognitionCache::Cleanup()
	{
    CCacheDirectoryEntry* item = iDirectoryHeader.Last();
    iNumberOfEntries -= item->NumberOfEntries();
    item->iDlink.Deque();
    delete item;
	}

/**
Empties the cache. Useful mainly for testing.
@internalComponent
*/
void CApsRecognitionCache::Flush()
	{
	while(!iDirectoryHeader.IsEmpty())
		{
		Cleanup();
		}
	}
