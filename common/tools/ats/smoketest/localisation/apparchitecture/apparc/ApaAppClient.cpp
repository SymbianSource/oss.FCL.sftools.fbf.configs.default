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
//

#include "ApaServerAppPriv.h"

void AppServerPanic(TApaAppServerPanic aReason)
	{
	_LIT(KPanic, "ApaAppClient");
	User::Panic(KPanic, aReason);
	}
	
void CleanupServerName(TAny* aParam)
	{
	HBufC** pServerName = static_cast<HBufC**>(aParam);
	delete *pServerName;
	*pServerName = NULL;
	}

/** Protected constructor, instantiable services must derive from this class.*/
EXPORT_C RApaAppServiceBase::RApaAppServiceBase()
: iServerName(NULL)
	{
	}

/** Closes the service and the connection to the server app.*/
EXPORT_C void RApaAppServiceBase::Close()
	{
	delete iServerName;
	iServerName = NULL;
	RSessionBase::Close();
	}
	
/** Connects to an already running server application, through an already connected service. 
@param aClient A service which is already connected.*/
EXPORT_C void RApaAppServiceBase::ConnectExistingAppL(const RApaAppServiceBase& aClient)
	{
	__ASSERT_DEBUG(!iServerName, AppServerPanic(EApaAppServerPanicClientAlreadyConnected));
	iServerName = aClient.iServerName->AllocL();
	ConnectL();
	}

/** Connects to an already running server application, through an already connected service. 
@param aClient A service which is already connected.
@param aSecurityPolicy The TSecurityPolicy which should be matched with the TSecurityPolicy of the app to be connected.

@see TSecurityPolicy*/
EXPORT_C void RApaAppServiceBase::ConnectExistingAppL(const RApaAppServiceBase& aClient, const TSecurityPolicy& aSecurityPolicy)
	{
	__ASSERT_DEBUG(!iServerName, AppServerPanic(EApaAppServerPanicClientAlreadyConnected));
	iServerName = aClient.iServerName->AllocL();
	ConnectL(aSecurityPolicy);
	}

/** Connects to an already running server application, by name.
@param aName The name of the server hosted by the server app.*/
EXPORT_C void RApaAppServiceBase::ConnectExistingByNameL(const TDesC& aServerName)
	{
	__ASSERT_DEBUG(!iServerName, AppServerPanic(EApaAppServerPanicClientAlreadyConnected));
	iServerName = aServerName.AllocL();
	ConnectL();
	}

/** Connects to an already running server application, by name.
@param aName The name of the server hosted by the server app.
@param aSecurityPolicy The TSecurityPolicy which should be matched with the TSecurityPolicy of the app to be connected.

@see TSecurityPolicy*/
EXPORT_C void RApaAppServiceBase::ConnectExistingByNameL(const TDesC& aServerName, const TSecurityPolicy& aSecurityPolicy)
	{
	__ASSERT_DEBUG(!iServerName, AppServerPanic(EApaAppServerPanicClientAlreadyConnected));
	iServerName = aServerName.AllocL();
	ConnectL(aSecurityPolicy);
	}

void RApaAppServiceBase::ConnectL()
	{
	CleanupStack::PushL(TCleanupItem(CleanupServerName, &iServerName));
	TUid serviceUid = ServiceUid();
	User::LeaveIfError(CreateSession(*iServerName,*reinterpret_cast<TVersion*>(&serviceUid)));
	CleanupStack::Pop(&iServerName);
	}

void RApaAppServiceBase::ConnectL(const TSecurityPolicy& aSecurityPolicy)
	{
	CleanupStack::PushL(TCleanupItem(CleanupServerName, &iServerName));
	TUid serviceUid = ServiceUid();
	User::LeaveIfError(CreateSession(*iServerName, *reinterpret_cast<TVersion*>(&serviceUid), -1, EIpcSession_Unsharable, &aSecurityPolicy));
	CleanupStack::Pop(&iServerName);
	}

/** Gets the name of the server hosted by the server application.
@return the name of the server application, will be empty if
the service is not connected*/
EXPORT_C TPtrC RApaAppServiceBase::ServerName() const
	{
	if (iServerName)
		{
		return *iServerName;
		}
	else
		{
		return KNullDesC();
		}
	}

/** Requests notification of server application exit.
@param aStatus On completion, this contains the exit code of the server application.
@see CApaServerAppExitMonitor*/
EXPORT_C void RApaAppServiceBase::NotifyServerExit(TRequestStatus& aStatus) const
	{
	SendReceive(EApaAppServerNotifyServerExit,TIpcArgs(),aStatus);
	}
	
/** Cancels the request for notification of server application exit.*/
EXPORT_C void RApaAppServiceBase::CancelNotifyServerExit() const
	{
	SendReceive(EApaAppServerCancelNotifyServerExit);	
	}

/** Extension mechanism - for internal BC proofing. */
EXPORT_C void RApaAppServiceBase::ExtensionInterface(TUid /*aInterfaceId*/, TAny*& /*aImplementaion*/)
	{
	}
EXPORT_C void RApaAppServiceBase::RApaAppServiceBase_Reserved1()
	{
	}

EXPORT_C void RApaAppServiceBase::RApaAppServiceBase_Reserved2()
	{
	}

/** Transfers the ownership of the session passed to self.
@param aClient A service which is already connected. On return, this service will be reset.
@leave KErrArgument The session passed is not connected.
@panic ApaAppClient 3 The client getting the ownership already has a connected session. Debug builds only.
*/	
EXPORT_C void RApaAppServiceBase::TransferExistingSessionL(RApaAppServiceBase& aClient)
	{
	__ASSERT_DEBUG(!iServerName, AppServerPanic(EApaAppServerPanicClientAlreadyConnected));
	if(aClient.iServerName == NULL)
		{
		User::Leave(KErrArgument);
		}
	if(this != &aClient)
		{
		iServerName = aClient.iServerName;
		iApaReserved1 = aClient.iApaReserved1;
		iApaReserved2 = aClient.iApaReserved2;
		SetHandle(aClient.Handle());
		aClient.iServerName = NULL;
		aClient.SetHandle(0);
		}
	}
	
//
// CApaServerAppExitMonitor
//

/** Creates a new monitor object and starts monitoring server app lifetime.
@param aClient A connected server app session, which requires monitoring.
@param aObserver An implementer of the MApaServerAppExitObserver that wants to be notified of server app exits.
@param aPriority The active object priority that this monitor should run at.
@return a new instance of a monitor. */
EXPORT_C CApaServerAppExitMonitor* CApaServerAppExitMonitor::NewL(RApaAppServiceBase& aClient, MApaServerAppExitObserver& aObserver, TInt aPriority)
	{
	CApaServerAppExitMonitor* self = new(ELeave) CApaServerAppExitMonitor(aClient, aObserver, aPriority);
	return self;
	}
	
/** Creates a new monitor object, places it on the cleanup stack and starts monitoring server app lifetime.
@param aClient A connected server app session, which requires monitoring.
@param aObserver An implementer of the MApaServerAppExitObserver that wants to be notified of server app exits.
@param aPriority The active object priority that this monitor should run at.
@return a new instance of a monitor, which is placed on the cleanup stack. */
EXPORT_C CApaServerAppExitMonitor* CApaServerAppExitMonitor::NewLC(RApaAppServiceBase& aClient, MApaServerAppExitObserver& aObserver, TInt aPriority)
	{
	CApaServerAppExitMonitor* self = new(ELeave) CApaServerAppExitMonitor(aClient, aObserver, aPriority);
	CleanupStack::PushL(self);
	return self;
	}
	
/** The destructor stops monitoring of server app exit. */
EXPORT_C CApaServerAppExitMonitor::~CApaServerAppExitMonitor()
	{
	Cancel();
	}

CApaServerAppExitMonitor::CApaServerAppExitMonitor(RApaAppServiceBase& aClient, MApaServerAppExitObserver& aObserver, TInt aPriority)
: CActive(aPriority), iClient(aClient), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	iClient.NotifyServerExit(iStatus);
	SetActive();
	}

void CApaServerAppExitMonitor::RunL()
	{
	TInt reason = iStatus.Int();
	iObserver.HandleServerAppExit(reason);
	}

void CApaServerAppExitMonitor::DoCancel()
	{
	iClient.CancelNotifyServerExit();
	}

TInt CApaServerAppExitMonitor::RunError(TInt aError)
	{
	return aError;
	}
