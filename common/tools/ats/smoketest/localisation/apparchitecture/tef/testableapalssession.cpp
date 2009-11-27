// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RTestableApaLsSession is used to make it possible to heap test a RApaLsSession
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/



#include "testableapalssession.h"
#include "../apserv/APSCLSV.H"


/**
Connects to the server and makes sure that recognizers are loaded on demand,
for heap check reasons.
Use this only when you don't want to wait for the app list to be populated.
*/
TInt RTestableApaLsSession::ConnectWithoutWaitingForListPopulation()
	{
	TInt err = RApaLsSession::Connect();
	if(err == KErrNone)
		err = SetLoadRecognizersOnDemand(ETrue);
	return err;
	}

/**
Connects to the server and makes sure that recognizers are loaded on demand,
for heap check reasons.
It also makes sure that the app list is populated before continuing.
*/
TInt RTestableApaLsSession::Connect()
	{
	const TInt err = ConnectWithoutWaitingForListPopulation();
	if(err != KErrNone)
		return err;
		
	TRequestStatus requeststatus;
	RegisterListPopulationCompleteObserver(requeststatus);
	User::WaitForRequest(requeststatus);
	
	return requeststatus.Int();
	}

/**
Makes a __UHEAP_MARK on the server.
*/
TInt RTestableApaLsSession::HeapMark() const 
	{
	return SendReceive(EDebugHeapMark, TIpcArgs(TIpcArgs::ENothing));
	}

/**
Makes a __UHEAP_MARKEND on the server.
*/
TInt RTestableApaLsSession::HeapMarkEnd() const
	{
	return HeapMarkEnd(0);
	}

/**
Makes a __UHEAP_MARKENDC(aCount) on the server.
*/
TInt RTestableApaLsSession::HeapMarkEnd(TInt aCount) const
	{
	return SendReceive(EDebugHeapMarkEnd, TIpcArgs(aCount));
	}

/**
Makes a __UHEAP_FAILNEXT on the server.
*/
TInt RTestableApaLsSession::HeapFailNext(TInt aCount) const
	{
	return SendReceive(EDebugHeapFailNext, TIpcArgs(aCount));
	}

/**
Clears the app info array which is created on the server when calling 
GetAllApps() and similar methods.
*/
TInt RTestableApaLsSession::ClearAppInfoArray() const
	{
	return SendReceive(EDebugClearAppInfoArray, TIpcArgs(TIpcArgs::ENothing));
	}

/**
Flushes the recognition cache.
*/
TInt RTestableApaLsSession::FlushRecognitionCache() const
	{
	return SendReceive(EDebugFlushRecognitionCache, TIpcArgs(TIpcArgs::ENothing));
	}

/**
Sets whether or not the recognizers should be loaded on demand.
*/
TInt RTestableApaLsSession::SetLoadRecognizersOnDemand(TBool aLoadRecognizersOnDemand) const
	{
	return SendReceive(EDebugSetLoadRecognizersOnDemand, TIpcArgs(aLoadRecognizersOnDemand));
	}

/**
When recognizers are loaded on demand a timer is used to do the actual unloading.
Use this method to perform this unloading synchronously instead of waiting for 
the timer to go off.
*/
TInt RTestableApaLsSession::PerformOutstandingRecognizerUnloading() const
	{
	return SendReceive(EDebugPerformOutstandingRecognizerUnloading, TIpcArgs(TIpcArgs::ENothing));
	}

/**
Deletes RApaLsSession's iExtension for heap checking purposes.

A result is that outstanding asynchronous file recognitions are cancelled.
*/
void RTestableApaLsSession::DeleteExtension()
 	{
 	//this allows us to delete RApaLsSession's iExtension for heap check reasons
 	//RApaLsSession is able to cope with a missing iExtension and must remain able to do so
 	//since it's a R-class (i.e. no ConstructL() or equal where it can create it)
 	//RApaLsSession recreates iExtension when needed
 	//this assumes that iExtension pointer is at the end of the RApaLsSession object
 	CBase** RApaLsSession_iExtension = (CBase**)((TInt)this + sizeof(RApaLsSession) - sizeof(CBase*));
 	delete *RApaLsSession_iExtension;
 	*(RApaLsSession_iExtension) = NULL;
 	}
 	
 /**
 Waits for the type store to be updated by calling @c NotifyOnDataMappingChange()
 */
 TInt RTestableApaLsSession::WaitForTypeStoreUpdate()
 	{
 	TRequestStatus status;
 	NotifyOnDataMappingChange(status);
 	User::WaitForRequest(status);
 	return status.Int();
 	}

 /**
 Inserts an update object into the list of updates to perform that will fail during the commit stage
 */
TInt RTestableApaLsSession::AddFailingNonNativeApplicationsUpdate()
	{
	return SendReceive(EDebugAddFailingNonNativeApplicationsUpdate, TIpcArgs());
	}

 /**
 Inserts an update object into the list of updates to perform that will Panic during the commit stage
 */
TInt RTestableApaLsSession::AddPanicingNonNativeApplicationsUpdate()
	{
	return SendReceive(EDebugAddPanicingNonNativeApplicationsUpdate, TIpcArgs());
	}

 /**
 Inserts an update object into the list of updates to perform that will Panic if the system tries to
 roll it back, e.g. because an update further down the list failed.
 */
TInt RTestableApaLsSession::AddRollbackPanicingNonNativeApplicationsUpdate()
	{
	return SendReceive(EDebugAddRollbackPanicingNonNativeApplicationsUpdate, TIpcArgs());
	}
