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
//

#if !defined(__APSRECCACHE_H__)
#define __APSRECCACHE_H__

#include <e32base.h>
#include <apmrec.h>
#include "APSRECUTIL.H"

const TUint KFileHashMapEntries = 40;

/**
An entry for a recognized file in the recognition cache.
The entries is organized as single-linked list.
The recognition result itself is reference-counted.
@internalComponent
*/
class CRecognitionResultHashMapEntry : public CBase
	{
public:
	static CRecognitionResultHashMapEntry* NewL(const TDesC& aFileName, TTime aLastModified, const TDataRecognitionResult& aResult, CRecognitionResultHashMapEntry* aNext);
	~CRecognitionResultHashMapEntry();
	
	inline const CRecognitionResultHashMapEntry* Next() const { return iNext; }
	inline CRecognitionResultHashMapEntry* Next() { return iNext; }
	inline TTime LastModified() const { return iLastModified; }
	inline const TDesC& FileName() const { return iResult->FileName(); }
	inline CRecognitionResult* Result() const { iResult->Open(); return iResult; }
		
	void UpdateL(TTime aLastModified, const TDataRecognitionResult& aResult);
private:
	CRecognitionResultHashMapEntry(TTime aLastModified, CRecognitionResult* aResult, CRecognitionResultHashMapEntry* aNext);
private:
	TTime iLastModified;
	CRecognitionResult* iResult;
	CRecognitionResultHashMapEntry* iNext;
	};

/**
A hash map for finding and adding recognition results.

@internalComponent
*/
class CRecognitionResultHashMap : public CBase
	{
public:
	CRecognitionResultHashMap();
	~CRecognitionResultHashMap();
	CRecognitionResult* Get(const TDesC& aKey, TTime& aLastModified) const;
	TBool AddL(const TDesC& aKey, TTime aLastModified, const TDataRecognitionResult& aResult);
	inline TUint NumberOfEntries() const { return iNumberOfEntries; }
protected:
	TUint GetIndex(const TDesC& aKey) const;
private:
	inline CRecognitionResultHashMapEntry* Entry(TUint aIndex);
	inline const CRecognitionResultHashMapEntry* Entry(TUint aIndex) const;
	inline void SetEntry(TUint aIndex, CRecognitionResultHashMapEntry* aEntry);
private:
	TUint iNumberOfEntries;
	CRecognitionResultHashMapEntry* iEntries[KFileHashMapEntries];
	};

/**
A directory entry in the cache.
Such an entry doesn't necessarily hold all files of a directory, but
only the ones that have been recognized so far.
@internalComponent
*/
class CCacheDirectoryEntry : public CBase
	{
public:
	static CCacheDirectoryEntry* NewL(const TDesC& aDirectory);
	~CCacheDirectoryEntry();
	inline const TDesC& Directory() const { return *iDirectory; }
	inline CRecognitionResultHashMap& Files() { return iFiles; } //lint !e1536 Suppress exposing low access member
	inline TUint NumberOfEntries() const { return iFiles.NumberOfEntries(); }
private:
	CCacheDirectoryEntry();
public:
	static const TInt iOffset;
private:
	HBufC* iDirectory; // own copy!
	CRecognitionResultHashMap iFiles;
	TDblQueLink iDlink;
	friend class CApsRecognitionCache;
	};

/**
A recognition result cache.
The cache holds a list of directories (stored as an RPointerArray). Recently
used directories are stored on top of the list, rarely used directories are
stored at the bottom. 

This strategy improves performance: 
search for files is started at the top (principle of locality) 
and cleanup is done from the bottom.

The files themselves are stored in a hash map (within directories).
@internalComponent
*/
class CApsRecognitionCache : public CBase
	{
public:
	CApsRecognitionCache(RFs& aFs);
	~CApsRecognitionCache();
	void AddL(const TDesC& aDirectory, const TDesC& aFileName, const TDataRecognitionResult& aRecognitionResult);
	void AddL(const RFile& aFile, const TDesC& aDirectory, const TDesC& aFileName, const TDataRecognitionResult& aRecognitionResult);
	TBool Get(const TDesC& aDirectory, const TDesC& aFileName, TDataRecognitionResult& aRecognitionResult);
	CRecognitionResult* Get(const RFile& aFile, const TDesC& aDirectory, const TDesC& aFileName);
	void Flush();
private:
	void DoAddL(const TDesC& aDirectory, const TDesC& aFileName, const TTime& aLastModified, const TDataRecognitionResult& aRecognitionResult);
	CRecognitionResult* DoGet(const TDesC& aDirectory, const TDesC& aFileName, const TTime& aLastModified);
	TBool CompareDirectories(const TDesC& aDir1, const TDesC& aDir2) const;
	void Cleanup();
private: // data
	RFs& iFs;
	TDblQue<CCacheDirectoryEntry> iDirectoryHeader;
	TDblQueIter<CCacheDirectoryEntry> iIter;
	TUint iNumberOfEntries;
	};


//inlines

inline CRecognitionResultHashMapEntry* CRecognitionResultHashMap::Entry(TUint aIndex)
	{
	ASSERT(aIndex < KFileHashMapEntries);
	return iEntries[aIndex];
	}  //lint !e1762 Suppress member function could be made const

inline const CRecognitionResultHashMapEntry* CRecognitionResultHashMap::Entry(TUint aIndex) const
	{
	ASSERT(aIndex < KFileHashMapEntries);
	return iEntries[aIndex];
	}

inline void CRecognitionResultHashMap::SetEntry(TUint aIndex, CRecognitionResultHashMapEntry* aEntry)
	{
	ASSERT(Entry(aIndex) == aEntry->Next());
	iEntries[aIndex] = aEntry;
	}

#endif // __APSRECCACHE_H__
