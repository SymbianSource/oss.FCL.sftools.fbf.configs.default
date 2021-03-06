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
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <e32std.h>
#include <e32math.h>

#include "appfwk_test_utils.h"



const TUid KServerUid3 = {0x10009FBD};


/**
  Start the server process. Simultaneous launching of two such processes should be detected when the second one attempts to
  create the server object, failing with KErrAlreadyExists.
*/
static TInt StartServer()
	{
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);
	RProcess server;
	TInt r = server.Create(KTestServerImg,KNullDesC,serverUid);
	if (r!=KErrNone)
		{
		return r;
		}
	
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		{
		server.Kill(0);
		}
	else
		{
		server.Resume();
		}

	// wait for start or death
	User::WaitForRequest(stat);	
	server.Close();
	
	return stat.Int();
	}


/**
  Connect to the server, attempting to start it if necessary
*/
EXPORT_C TInt RSmlTestUtils::Connect()
	{
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KTestServerName,TVersion(0,0,0),1);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			{
			return r;
			}
		if (--retry==0)
			{
			return r;
			}
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			{
			return r;
			}
		}
	}

	
EXPORT_C void RSmlTestUtils::SetDevIdL(const TDesC8& aDevId)
	{
	TIpcArgs args(&aDevId);
	User::LeaveIfError(SendReceive(EChangeDevId,args));
	}


EXPORT_C void RSmlTestUtils::GetDevIdL(TDes8& aDevId)
	{
	TIpcArgs args(&aDevId);
	User::LeaveIfError(SendReceive(EGetDevId, args));
	}


EXPORT_C TInt RSmlTestUtils::WipeDataStoreL(TWipeItems aItemsToClean)
	{
	TPckgBuf<TWipeItems> temp(aItemsToClean);
	TIpcArgs args(&temp);
	User::LeaveIfError(SendReceive(EWipeDataStore, args));
	return KErrNone;
	}
	
	
EXPORT_C TInt RSmlTestUtils::RenameDirectoryL(const TDesC16& aSrc,const TDesC16& aDest)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TIpcArgs args(&aSrc,&aDest);
	TInt ret = SendReceive(ERenameDir, args);
	return ret;
	}


EXPORT_C TInt RSmlTestUtils::CreateDirectoryL(const TDesC& aPath)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TIpcArgs args(&aPath);
	TInt ret = SendReceive(ECreateDir, args);
	return ret;
	}
	

EXPORT_C TInt RSmlTestUtils::DeleteDirectoryL(const TDesC& aPath)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TIpcArgs args(&aPath);
	TInt ret = SendReceive(EDeleteDir, args);
	return ret;
	}
	
	
EXPORT_C TInt RSmlTestUtils::CreateFileL(const TDesC& aPath)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TIpcArgs args(&aPath);
	TInt ret = SendReceive(ECreateFile, args);
	return ret;
	}


EXPORT_C TInt RSmlTestUtils::DeleteFileL(const TDesC& aPath)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TIpcArgs args(&aPath);
	TInt ret = SendReceive(EDeleteFile, args);
	return ret;
	}
	
	
EXPORT_C TInt RSmlTestUtils::DeleteFileUsingWildcardL(const TDesC& aPath)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TIpcArgs args(&aPath);
	TInt ret = SendReceive(EDeleteFileUsingWildcard, args);
	return ret;
	}
	
	
EXPORT_C TInt RSmlTestUtils::CopyFileL(const TDesC& aSrc,const TDesC& aDest)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TIpcArgs args(&aSrc,&aDest);
	TInt ret = SendReceive(ECopyFile, args);
	return ret;
	}
	
// lint warning stating that aTime could be a const reference,
// but since this is derived from product code, it cannot be
// changed here, and hence the warning is suppressed
//lint -efunc(1746,RSmlTestUtils::SetHomeTimeL)	
EXPORT_C void RSmlTestUtils::SetHomeTimeL(const TTime aTime)
	{
	TPckgBuf<TTime> temp(aTime);
	TIpcArgs args(&temp);
	User::LeaveIfError(SendReceive(ESetHomeTime, args));
	}


EXPORT_C TInt RSmlTestUtils::ReplaceFileL(const TDesC& aName,TUint aFileMode)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TPckgBuf<TUint> temp(aFileMode);
	TIpcArgs args(&aName,&temp);
	TInt ret = SendReceive(EReplaceFile, args);
	return ret;
	}
	

EXPORT_C TInt RSmlTestUtils::IsFilePresent(const TDesC& aName, TBool &aPresent)
	{
	TPckgBuf<TBool> present(aPresent);
	TIpcArgs args(&aName,&present);
	TInt ret = SendReceive(EIsFilePresent, args);
	aPresent = present();
	return ret;
	}


EXPORT_C TInt RSmlTestUtils::SetReadOnly(const TDesC& aName,TUint aSetAttMask)
	{
	TPckgBuf<TUint> temp(aSetAttMask);
	TIpcArgs args(&aName,&temp);
	TInt ret = SendReceive(ESetReadOnly,args);
	return ret;
	}

	
EXPORT_C TInt RSmlTestUtils::GetAtt( const TDesC& aName, TUint& aAttValue )
	{
	TPckgBuf<TUint> attPckg( aAttValue ); 
	TIpcArgs args( &aName, &attPckg );	
	TInt ret = SendReceive( EGetAttributes, args );
	aAttValue = attPckg();
	return ret;	
	}
		
		
EXPORT_C TInt RSmlTestUtils::SetAtt( const TDesC &aName, TUint aSetAttMask, TUint aClearAttMask )
	{
	TIpcArgs args( &aName, &aSetAttMask, &aClearAttMask );
	TInt ret = SendReceive( ESetAttributes, args );
	return ret;		
	}


EXPORT_C TInt RSmlTestUtils::CopyDirectoryL(const TDesC& aSource,const TDesC& aTarget)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TIpcArgs args(&aSource,&aTarget);
	TInt ret = SendReceive(ECopyDirectory, args);
	return ret;
	}


EXPORT_C TInt RSmlTestUtils::ChangeFilePermissionL(const TDesC& aPath)
	{ // this implementation of this method does not leave but further derived implementations may if they choose
	TIpcArgs args(&aPath);
	TInt ret = SendReceive(EChangeFilePermission, args);
	return ret;	
	}
