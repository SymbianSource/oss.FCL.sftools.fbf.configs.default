// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TESTABLEAPALSSESSION_H__
#define __TESTABLEAPALSSESSION_H__

#include <apgcli.h>
	
class RTestableApaLsSession : public RApaLsSession
	{
public: //from RApaLsSession
	TInt Connect();	
public:
	TInt ConnectWithoutWaitingForListPopulation();
	TInt HeapMark() const;
	TInt HeapMarkEnd() const;
	TInt HeapMarkEnd(TInt aCount) const;
	TInt HeapFailNext(TInt aCount) const;
	TInt ClearAppInfoArray() const;
	TInt FlushRecognitionCache() const;
	TInt SetLoadRecognizersOnDemand(TBool aLoadRecognizersOnDemand) const;
	TInt PerformOutstandingRecognizerUnloading() const;
	void DeleteExtension();
	TInt WaitForTypeStoreUpdate();
	TInt AddFailingNonNativeApplicationsUpdate();
	TInt AddPanicingNonNativeApplicationsUpdate();
	TInt AddRollbackPanicingNonNativeApplicationsUpdate();
	};
	
/**
Use as @c aCleanup argument to @c HEAP_TEST_LS_SESSION if the test doesn't need
any cleanup in order to be heap checkable.
*/
#define NO_CLEANUP
/**
Use as @c aClientCount or @c aServerCount arguments to @c HEAP_TEST_LS_SESSION
if you don't want the heap to be checked
*/
#define DONT_CHECK -1

/**
Heap tests a RTestableApaLsSession by calling __UHEAP_MARK before some test code
and __UHEAP_MARKEND after.

@param aApaLsSession A reference to a RTestableApaLsSession
@param aClientCount The value sent to __UHEAP_MARKENDC() on the client side. 
       Usually 0. DONT_CHECK if no check shall be made.
@param aServerCount The value sent to __UHEAP_MARKENDC() on the server side. 
       Usually 0. DONT_CHECK if no check shall be made.
@param aTest The test code
@param aCleanup Any cleanup that is needed before the heap check is made. 
       NO_CLEANUP if no cleanup is needed.
*/
#define HEAP_TEST_LS_SESSION(aApaLsSession, aClientCount, aServerCount, aTest, aCleanup) \
	if(aClientCount != DONT_CHECK)														\
		{																				\
		__UHEAP_MARK;																	\
		}																				\
	if(aServerCount != DONT_CHECK) 														\
		{																				\
		const TInt _error = aApaLsSession.HeapMark();									\
		if(_error != KErrNone)															\
			{																			\
			INFO_PRINTF2(_L("__UHEAP_MARK failed on the server (error=%d)"), _error);	\
			}																			\
		TESTL(KErrNone == _error);														\
		}																				\
	{																					\
	TRAPD(_error, aTest); /* execute the test code */									\
	if(_error != KErrNone)																\
		{																				\
		INFO_PRINTF2(_L("Test code issued a leave (%d)"), _error);						\
		}																				\
	TESTL(KErrNone == _error);															\
	}																					\
	aCleanup; /* execute the cleanup */													\
	aApaLsSession.DeleteExtension();													\
	aApaLsSession.PerformOutstandingRecognizerUnloading();										\
	if(aServerCount != DONT_CHECK)														\
		{																				\
		const TInt _error = aApaLsSession.HeapMarkEnd(aServerCount);					\
		if(_error != KErrNone)															\
			{																			\
			INFO_PRINTF2(_L("__UHEAP_MARKEND failed on the server (error=%d)"), _error);\
			}																			\
		TESTL(KErrNone == _error);														\
		}																				\
	if(aClientCount != DONT_CHECK)														\
		{																				\
		__UHEAP_MARKENDC(aClientCount);													\
		}																				\
	
		

#endif //__TESTABLEAPALSSESSION_H__
