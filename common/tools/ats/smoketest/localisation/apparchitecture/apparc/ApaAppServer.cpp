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

#include "ApaServerAppPriv.h"

//
// CApaAppServiceBase
//

/** Constructor. */
EXPORT_C CApaAppServiceBase::CApaAppServiceBase()
	{
	}
	
/** An implementation of the CreateL() method from CSession2.
This can be further overridden by derived classes, but such
implementations should also base-call. */
EXPORT_C void CApaAppServiceBase::CreateL()
	{
	}

/** Destructor. */
EXPORT_C CApaAppServiceBase::~CApaAppServiceBase()
	{
	if (!iNotifyExitMsg.IsNull() && (iExitReason != KRequestPending))
		{
		iNotifyExitMsg.Complete(iExitReason);
		}
	}

/** Virtual framework function that is called on receipt
of a message from the client. This allows the service implementation
to define a security policy for messages from the client.
The default behaviour of this function is to pass security checks.
@param	aMsg The message to check.
@param	aAction A reference to the action to take if the security check
		fails. This is either a value from TFailureAction or a negative
		value which has meaning to the CustomFailureActionL() method of
		a derived class.
		The policy server framework gives this value a default of
		EFailClient.  If a derived implementation wishes a
		different value, then it should change this.
@param 	aMissing A reference to the list of security attributes missing
		from the checked process.  The policy server initialises this
		object to zero (that is a sid of 0, a vid of 0, and no capabilities).
		If derived implementations wish to take advantage of a list of
		missing attributes in their implementation of CustomFailureActionL(),
		then they should set those missing attributes here in
		CustomSecurityCheckL().
@return A value from CPolicyServer::TCustomResult.
@see CPolicyServer */
EXPORT_C CPolicyServer::TCustomResult CApaAppServiceBase::SecurityCheckL(const RMessage2& /*aMsg*/, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	return CPolicyServer::EPass;	// default is to pass security checks
	}

/** Framework function to handle client messages.
Derived classes should override this method to receive messages from the client.
These messages should have function ids starting from
REikAppServiceBase::KServiceCmdBase.
The behaviour for any unrecognised messages must be to
base-call.
@param aMessage The clients request message.
@panic ApaAppServer 0 If this function does not recognise the function ID.*/
EXPORT_C void CApaAppServiceBase::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case EApaAppServerNotifyServerExit:
			NotifyServerExit(aMessage);
			break;
			
		case EApaAppServerCancelNotifyServerExit:
			CancelNotifyServerExit(aMessage);
			break;
			
		default:
			PanicAppServerClient(aMessage, EApaAppServerPanicIllegalFunction);
			break;
		}
	}

/** Handle leave errors from ServiceL().
@param aMessage The client message that was being processed when the leave occured.
@param aError The leave code. */
EXPORT_C void CApaAppServiceBase::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	CSession2::ServiceError(aMessage,aError);
	}
EXPORT_C TInt CApaAppServiceBase::CountResources()
	{
	return CSession2::CountResources();
	}
	
EXPORT_C void CApaAppServiceBase::Disconnect(const RMessage2& aMessage)
	{
	CSession2::Disconnect(aMessage);
	}
void CApaAppServiceBase::NotifyServerExit(const RMessage2& aMessage)
	{
	if (!iNotifyExitMsg.IsNull())
		{
		PanicAppServerClient(aMessage, EApaAppServerPanicNotifyExitActive);
		}
	else
		{
		iNotifyExitMsg = aMessage;
		iExitReason = KRequestPending;
		}
	}

void CApaAppServiceBase::CancelNotifyServerExit(const RMessage2& aMessage) const
	{
	if (!iNotifyExitMsg.IsNull())
		{
		iNotifyExitMsg.Complete(KErrCancel);
		}
	aMessage.Complete(KErrNone);
	}

void CApaAppServiceBase::SendAppServerExitNotification(TInt aExitReason)
	{
	if (iExitReason == KRequestPending)
		{
		iExitReason = aExitReason;
		}
	}

/** Extension mechanism - for internal BC proofing. */
EXPORT_C void CApaAppServiceBase::ExtensionInterface(TUid /*aInterfaceId*/, TAny*& /*aImplementaion*/)
	{
	}
EXPORT_C void CApaAppServiceBase::CApaAppServiceBase_Reserved1()
	{
	}
	
EXPORT_C void CApaAppServiceBase::CApaAppServiceBase_Reserved2()
	{
	}
//
// CApaAppServer
//

const TUint KApaAppServerRangeCount = 1;
const TInt KApaAppServerRanges[KApaAppServerRangeCount] = 
	{
	0, //range is 0-KMaxTInt inclusive
	};
const TUint8 KApaAppServerElementsIndex[KApaAppServerRangeCount] = 
	{
	CPolicyServer::ECustomCheck, 	//applies to 0th range (req num: 0-KMaxTInt)
	};
//const CPolicyServer::TPolicyElement KApaAppServerElements[0] = 
//	{
//	}
const CPolicyServer::TPolicy KApaAppServerPolicy =
	{
	CPolicyServer::ECustomCheck,
	KApaAppServerRangeCount,					
	KApaAppServerRanges,
	KApaAppServerElementsIndex,
	0,
	};
/** Constructor. */
EXPORT_C CApaAppServer::CApaAppServer()
:CPolicyServer(0,
			   KApaAppServerPolicy,
			   EUnsharableSessions)
	{
	}

/** Destructor */
EXPORT_C CApaAppServer::~CApaAppServer()
	{
	}

/** Virtual ConstructL() function to allow second stage construction.
This can be overridden for derived class construction, but must be
base-called.
@param aFixedServerName - the name that this server will use. */
EXPORT_C void CApaAppServer::ConstructL(const TDesC& aFixedServerName)
	{
	StartL(aFixedServerName);
	}

EXPORT_C CSession2* CApaAppServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& aMessage) const
	{
	TUid serviceUid = ConnectMessageServiceUid(aMessage);
	return CreateServiceL(serviceUid);
	}

/** The server's main security checking function.
This delegates connect checks to CreateServiceSecurityCheckL
and checks on other messages to the target service implementation's
SecurityCheckL function.
For framework use only.
@internalComponent */
EXPORT_C CPolicyServer::TCustomResult CApaAppServer::CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing)
	{
	if (aMsg.Function() == RMessage2::EConnect)
		{
		TUid serviceUid = ConnectMessageServiceUid(aMsg);
		return CreateServiceSecurityCheckL(serviceUid, aMsg, aAction, aMissing);
		}
	else
		{
		CApaAppServiceBase* service = static_cast<CApaAppServiceBase*>(aMsg.Session());
		if (service)
			{
			return service->SecurityCheckL(aMsg, aAction, aMissing);
			}
		else
			{
			return CPolicyServer::EFail;
			}
		}
	}

/** Service creation function.
When a client wants to connect to a server app, this function will be
called to create the service requested by the client.
This function must be overridden by server apps to create their
implementations of the requested service.
If the server app's implementation of this function does not recognise
the service UID, it must base-call.
@param aServiceType The UID of the service that the client wants to connect to.
@return an object derived from CApaAppServiceBase that can handle the
service type requested by the client. */
EXPORT_C CApaAppServiceBase* CApaAppServer::CreateServiceL(TUid /*aServiceType*/) const
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/** Notifies clients that the server is going to exit.
If a server application exits for any non-standard reason (ie anything
except EEikCmdExit) this function should be called to forward the reason
to the client. The UI framework will call this with EEikCmdExit if it has
not already been done.
@param aReason The exit reason from the server app. This will either be a
               command ID, eg EEikCmdExit, or an error code, eg KErrServerTerminated.
@see CEikAppUi::Exit */
EXPORT_C void CApaAppServer::NotifyServerExit(TInt aReason)
	{
	iSessionIter.SetToFirst();
	CSession2* s;
	while ((s=iSessionIter++)!=NULL)
		{
		static_cast<CApaAppServiceBase*>(s)->SendAppServerExitNotification(aReason);
		}
	}

/** Function to allow security checks on the client before they
connect to a service.
Server apps should override this method if they want to restict
connections to some services to clients with particular capabilities.
@param aServiceType The UID of the service that the client wants to connect to.
@param	aMsg The message to check.
@param	aAction A reference to the action to take if the security check
		fails. This is either a value from TFailureAction or a negative
		value which has meaning to the CustomFailureActionL() method of
		a derived class.
		The policy server framework gives this value a default of
		EFailClient.  If a derived implementation wishes a
		different value, then it should change this.
@param 	aMissing A reference to the list of security attributes missing
		from the checked process.  The policy server initialises this
		object to zero (that is a sid of 0, a vid of 0, and no capabilities).
		If derived implementations wish to take advantage of a list of
		missing attributes in their implementation of CustomFailureActionL(),
		then they should set those missing attributes here in
		CustomSecurityCheckL().
@return A value from CPolicyServer::TCustomResult.
@see CPolicyServer */
EXPORT_C CPolicyServer::TCustomResult CApaAppServer::CreateServiceSecurityCheckL(TUid /*aServiceType*/, const RMessage2& /*aMsg*/, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	return CPolicyServer::EPass;
	}

TUid CApaAppServer::ConnectMessageServiceUid(const RMessage2& aMessage) const
	{
	// note: this function uses knowledge of the internal structure
	// of connect messages
	return TUid::Uid(aMessage.Int0());
	}
// From CServer2
EXPORT_C void CApaAppServer::DoConnect(const RMessage2& aMessage)
	{
	CServer2::DoConnect(aMessage);
	}
	
// Reserved
EXPORT_C void CApaAppServer::CApaAppServer_Reserved1()
	{
	}
	
EXPORT_C void CApaAppServer::CApaAppServer_Reserved2()
	{
	}
/** Extension mechanism - for internal BC proofing. */
EXPORT_C void CApaAppServer::ExtensionInterface(TUid /*aInterfaceId*/, TAny*& /*aImplementaion*/)
	{
	}
EXPORT_C MApaServerAppExitObserver::MApaServerAppExitObserver()
	{
	}
	
EXPORT_C void MApaServerAppExitObserver::MApaServerAppExitObserver_Reserved1()
	{
	}
	
EXPORT_C void MApaServerAppExitObserver::MApaServerAppExitObserver_Reserved2()
	{
	}
//
// static functions
//

void PanicAppServerClient(const RMessagePtr2& aMessage,TInt aPanic)
	{
	_LIT(KPanic,"ApaAppServer");
	aMessage.Panic(KPanic,aPanic);
	}
