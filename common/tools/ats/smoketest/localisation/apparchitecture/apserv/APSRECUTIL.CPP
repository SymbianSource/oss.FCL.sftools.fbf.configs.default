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
// Classes for synchronous and asynchronous file recognitions (of a directory)
// 
//


#include "APSRECUTIL.H"
#include "APSRECCACHE.h"

//////////////////////////////
// CRecognitionResult
//////////////////////////////

CRecognitionResult* CRecognitionResult::NewL(const TDesC& aFileName, const TDataRecognitionResult& aRecognitionResult)
	{
	ASSERT(aFileName.Find(_L("\\")) == KErrNotFound); // the filename should not contain any path

	HBufC* fileName = aFileName.AllocLC();
	HBufC8* dataType = aRecognitionResult.iDataType.Des8().AllocLC();	
	CRecognitionResult* self = new(ELeave) CRecognitionResult(fileName, dataType, aRecognitionResult.iDataType.Uid(), aRecognitionResult.iConfidence);
	CleanupStack::Pop(2, fileName);
	return self;
	}

CRecognitionResult::CRecognitionResult(HBufC* aFileName, HBufC8* aDataType, TUid aUid, TInt aConfidence)
	: iFileName(aFileName),
	  iDataType(aDataType),
 	  iUid(aUid), 
	  iConfidence(aConfidence)
	{
	}

CRecognitionResult::~CRecognitionResult()
	{
	delete iFileName;
	delete iDataType;
	}

void CRecognitionResult::Get(TDataRecognitionResult& aRecognitionResult)
	{
	if(iUid == KNullUid)

		{
		aRecognitionResult.iDataType = TDataType(*iDataType);
		}
	else
		{
		aRecognitionResult.iDataType = TDataType(iUid);
		}
	aRecognitionResult.iConfidence = iConfidence;
	}
	


TUint CRecognitionResult::GetRequiredBufferSize() const
	{
	return iFileName->Size() + iDataType->Size() + sizeof(TUid) + sizeof(TInt) + 2;
	}

void CRecognitionResult::WriteToStreamL(RWriteStream& aWriteStream)
	{
	aWriteStream.WriteUint8L(iFileName->Length());
	aWriteStream.WriteL(*iFileName);
	aWriteStream.WriteUint8L(iDataType->Length());
	aWriteStream.WriteL(*iDataType);
	aWriteStream.WriteInt32L(iUid.iUid);
	aWriteStream.WriteInt32L(iConfidence);
	}
	
///////////////////////////////////
// CDirectoryRecognitionResult
///////////////////////////////////

CDirectoryRecognitionResult::CDirectoryRecognitionResult(HBufC* aPath, HBufC8* aDataTypeFilter)
	: iPath(aPath), iDataTypeFilter(aDataTypeFilter)
	{
	iRequiredBufferSize = sizeof(TUint);
	}

CDirectoryRecognitionResult::~CDirectoryRecognitionResult()
	{
	delete iPath;
	delete iDataTypeFilter;
	
	const TInt count = iEntries.Count();
	for(TInt i = 0; i < count; i++)
		{
		iEntries[i]->Close();
		}
	iEntries.Reset();
	}

void CDirectoryRecognitionResult::AppendL(CRecognitionResult* aEntry)
	{
	if(iDataTypeFilter)
		{
		if(aEntry->DataType().Match(*iDataTypeFilter) == KErrNotFound)
			{
			aEntry->Close(); //if we don't take ownership we must decrease reference count
			return;
			}
		}

	iEntries.AppendL(aEntry);
	iRequiredBufferSize += aEntry->GetRequiredBufferSize();
	} //lint !e818: Pointer parameter could be declared as pointing to const

void CDirectoryRecognitionResult::WriteToStreamL(RWriteStream& aWriteStream)
	{
	aWriteStream.WriteUint32L(iEntries.Count());
	for (TInt i=0; i < iEntries.Count(); i++)
		{
		iEntries[i]->WriteToStreamL(aWriteStream);
		}
	aWriteStream.CommitL();
	}

//////////////////////////////////////////////////////////
// CFileRecognitionUtility
//////////////////////////////////////////////////////////

CFileRecognitionUtility::CFileRecognitionUtility(CApaAppListServer& aServer, TInt aMaxBufSize, RFs& aFs)
	: CActive(0), 
	  iServer(aServer), 
	  iMaxBufSize(aMaxBufSize), 
	  iFs(aFs),
	  iIndex(0),
	  iStep(0)
	{
	CActiveScheduler::Add(this);
	}

CFileRecognitionUtility::~CFileRecognitionUtility()
	{
	Cancel();
	delete iEntryList;
	
	iPath = NULL;
	iResult = NULL;
	}

void CFileRecognitionUtility::RecognizeSynchronouslyL(CDirectoryRecognitionResult& aResult)
	{
	iPath = &(aResult.Path());
	iResult = &aResult;

	User::LeaveIfError(iFs.IsValidName(iPath->Des()));

	ReadDirectoryL();

	// recognize files and calculate size of buffer
	for(iIndex=0;iIndex<iEntryList->Count();iIndex++)
		{
		RecognizeFileL();
		}
		
	delete iEntryList;
	iEntryList = NULL;
	}

void CFileRecognitionUtility::RecognizeAsynchronously(CDirectoryRecognitionResult& aResult, const RMessage2& aMessage)
	{
	iIndex = 0;
	iStep = 0;
	iPath = &(aResult.Path());
	iResult = &aResult;
	iMessage = aMessage;

	NextStep();
	}

void CFileRecognitionUtility::ReadDirectoryL()
	{
	delete iEntryList;
	iEntryList = NULL;

	// read directory content
	User::LeaveIfError(iFs.GetDir(iPath->Des(),KEntryAttNormal,EDirsAnyOrder,iEntryList));
	}
	
TInt CFileRecognitionUtility::BufferSizeL() const
	{
	const TInt preferredBufferSize = iServer.DataRecognizerPreferredBufSizeL();
	TInt bufSize=Min(preferredBufferSize,iMaxBufSize);
	if (bufSize<1)
		{
		bufSize=8; // sensible minimum.
		}
	return bufSize;
	}

void CFileRecognitionUtility::RecognizeFileL()
	{
	TFileName fullFileName(*iPath);
	fullFileName.Append((*iEntryList)[iIndex].iName);
	RFile file;
	User::LeaveIfError(file.Open(iFs, fullFileName, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);
	
	CRecognitionResult* result = NULL;
	TRAP_IGNORE(result = iServer.RecognizeDataAsCRecognitionResultL(file, BufferSizeL()));
	if(result)
		{
		CleanupClosePushL(*result);
		iResult->AppendL(result);
		CleanupStack::Pop(result);
		}
	
	CleanupStack::PopAndDestroy(&file);
	}

void CFileRecognitionUtility::CancelRecognitionRequest()
	{
	if(iStep <= 1)
		{
		Cancel();
		iMessage.Complete(KErrCancel);	
		}
	}

void CFileRecognitionUtility::NextStep()
	{
	TRequestStatus* pStatus = &iStatus;
	User::RequestComplete(pStatus, KErrNone);
	SetActive();
	}

void CFileRecognitionUtility::RunL()
	{
	switch(iStep)
		{
		case 0:		// first step: read the directory content
			ReadDirectoryL();
			iIndex=0;
			iStep++;
			NextStep();
			break;
		case 1:		// read current file and recognize it
			if(iIndex < iEntryList->Count())
				{
				RecognizeFileL();
				iIndex++;
				NextStep();
				}
			else	// finished
				{
				iMessage.WriteL(1,TPckgBuf<TUint>(iResult->RequiredBufferSize()));
				iMessage.Complete(KErrNone);
				iStep++;	// to show that we have completed the request
				delete iEntryList;
				iEntryList = NULL;
				}
			break;
		default:
			break;
		}
	}

void CFileRecognitionUtility::DoCancel()
	{
	iIndex = 0;
	iStep = 0;
	}
