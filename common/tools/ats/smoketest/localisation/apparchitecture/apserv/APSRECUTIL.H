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


#if !defined(__APSRECUTIL_H__)
#define __APSRECUTIL_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include <apmrec.h>
#include <apsserv.h>

/**
Reference-counted object which stores a recognition result in a more compact format
than TDataRecognitionResult does. Also includes the file name.

The reason why instances of this class are reference-counted is that they might 
be "owned" by the cache and CDirectoryRecognitionResult at the same time. If both
of them had their own copies unnecessarily much memory would be wasted.
@internalComponent
*/
class CRecognitionResult : public CObject
	{
public:
	static CRecognitionResult* NewL(const TDesC& aFileName, const TDataRecognitionResult& aRecognitionResult);
	TUint GetRequiredBufferSize() const;
	void WriteToStreamL(RWriteStream& aWriteStream);
	
	void Get(TDataRecognitionResult& aRecognitionResult);
	inline const TDesC8& DataType() const { return *iDataType; }
	inline const TDesC& FileName() const { return *iFileName; }
	
private:
	CRecognitionResult(HBufC* aFileName, HBufC8* aDataType, TUid aUid, TInt aConfidence);
	~CRecognitionResult();
private:
	HBufC* iFileName;
	HBufC8* iDataType;
	TUid iUid;
	TInt iConfidence;
	};

/**
Holds the recognition results of all files of a directory.
@internalComponent
*/
class CDirectoryRecognitionResult : public CBase
	{
public:
	CDirectoryRecognitionResult(HBufC* aPath, HBufC8* aDataTypeFilter);
	virtual ~CDirectoryRecognitionResult();
	void AppendL(CRecognitionResult* aEntry);
	void WriteToStreamL(RWriteStream& aWriteStream);
	inline TInt RequiredBufferSize() const {return iRequiredBufferSize;}
	inline HBufC& Path() { return *iPath; }
private:
	TInt iRequiredBufferSize;
	HBufC* iPath;
	HBufC8* iDataTypeFilter;
	RPointerArray<CRecognitionResult> iEntries;
	};

/**
Utility class which does synchronous and asynchronous file recognitions of a directory
@internalComponent
*/
class CFileRecognitionUtility : public CActive
	{
public:
	CFileRecognitionUtility(CApaAppListServer& aServer, TInt aMaxBufSize, RFs& aFs);
	~CFileRecognitionUtility();
	void RecognizeSynchronouslyL(CDirectoryRecognitionResult& aResult);
	void RecognizeAsynchronously(CDirectoryRecognitionResult& aResult, const RMessage2& aMessage);
	void CancelRecognitionRequest();
protected:
	void ReadDirectoryL();
	void RecognizeFileL();
private:
	void NextStep();
	void RunL();
	void DoCancel();
	TInt BufferSizeL() const;
	
private:
	HBufC* iPath;
	CDirectoryRecognitionResult* iResult;
	CApaAppListServer& iServer;
	TInt iMaxBufSize;
	RFs& iFs;
	CDir* iEntryList;
	TInt iIndex;
	TInt iStep;
	RMessage2 iMessage;
	};

#endif	// __APSRECUTIL_H__
