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

#include "APGWGNAM.H"
#include "APGSTD.H"
#include "APGTASK.H"

const TInt KUidBufLength=8;
const TInt KMinWindowGroupNameLength=5/*delimiters+status)*/+KUidBufLength;
const TInt KMaxWindowGroupNameLength=KMinWindowGroupNameLength+2*KMaxFileName;
const TInt KMaxSearchStringLength=KMaxFileName+6/* 3*delim+3*wild */;
typedef TBuf<KMaxSearchStringLength> TApaSearchString;

_LIT(KDefaultWindowGroupName,"00\x00\x30\x30\x30\x30\x30\x30\x30\x30\x0\x0");
_LIT(KSearchAnyFile,"*");
_LIT(KFormatStatus,"%02x");

EXPORT_C CApaWindowGroupName::~CApaWindowGroupName()
/** Destructor.

Frees resources owned by the object prior to its destruction. */
	{	   
	delete iBuf;
	}

EXPORT_C CApaWindowGroupName* CApaWindowGroupName::New(const RWsSession& aWsSession, HBufC* aWgName)
	//
	// non leaving static initializer from full name - takes ownership
	// ONLY TO BE USED WITH A VALID (i.e existing) WINDOW GROUP NAME
	//
/** Creates a window group name object and takes ownership of the heap descriptor 
containing a name.

The name must have valid format. Typically, this is an existing window group 
name.

Note that the function cannot leave.

@param aWsSession A window server session.
@param aWgName A heap descriptor containing a valid window group name. This 
pointer must not be null, otherwise the function raises an APGRFX 3 panic.
@return A pointer to the new window group name object. */
	{
	CApaWindowGroupName* This=NULL;
	if (aWgName)
		{
		This=new CApaWindowGroupName(aWsSession);
		if (This)
			{
			This->iBuf=aWgName;
			This->GetStatusFromName();
			}
		}
	else
		Panic(EPanicNullWindowGroupName);
	return This;
	}

EXPORT_C CApaWindowGroupName* CApaWindowGroupName::NewL(const RWsSession& aWsSession)
	//
	// static initializer for blank window group name
	//
/** Creates a window group name object and assigns a default name to it.

@param aWsSession A window server session.
@return A pointer to the new window group name object. */
	{
	CApaWindowGroupName* This=NewLC(aWsSession);
	CleanupStack::Pop();
	return This;
	}

EXPORT_C CApaWindowGroupName* CApaWindowGroupName::NewL(const RWsSession& aWsSession, TInt aWgId)
	//
	// static initializer for existing window group name given by aWgId
	//
/** Creates a window group name object and assigns to it the name taken from the 
specified window group.

@param aWsSession A window server session.
@param aWgId The ID of a window group.
@return A pointer to the new window group name object. */
	{
	CApaWindowGroupName* This=NewLC(aWsSession, aWgId);
	CleanupStack::Pop();
	return This;
	}

EXPORT_C CApaWindowGroupName* CApaWindowGroupName::NewL(const RWsSession& aWsSession, const TDesC& aWgName)
	//
	// static initializer from full name
	// ONLY TO BE USED WITH A VALID (i.e existing) WINDOW GROUP NAME
	//
/** Creates a window group name object and assigns to it the specified name.

The name must have a valid format. Typically, this is an existing window group 
name.

@param aWsSession A window server session.
@param aWgName A valid window group name.
@return A pointer to the new window group name object. */
	{
	CApaWindowGroupName* This=NewLC(aWsSession, aWgName);
	CleanupStack::Pop();
	return This;
	}

EXPORT_C CApaWindowGroupName* CApaWindowGroupName::NewLC(const RWsSession& aWsSession)
	//
	// static initializer for blank window group name
	//
/** Creates a window group name object, assigns a default name to it, and puts 
a pointer to the new object onto the cleanup stack.

@param aWsSession A window server session. 
@return A pointer to the new window group name object. */
	{
	CApaWindowGroupName* This=new(ELeave) CApaWindowGroupName(aWsSession);
	CleanupStack::PushL(This);
	This->DefaultConstructL();
	return This;
	}

EXPORT_C CApaWindowGroupName* CApaWindowGroupName::NewLC(const RWsSession& aWsSession, TInt aWgId)
	//
	// static initializer for existing window group name given by aWgId
	//
/** Creates a window group name object, assigns to it the name taken from the specified 
window group, and puts a pointer to the new object onto the cleanup stack.

@param aWsSession A window server session.
@param aWgId The ID of a window group.
@return A pointer to the new window group name object. */
	{
	CApaWindowGroupName* This=new(ELeave) CApaWindowGroupName(aWsSession);
	CleanupStack::PushL(This);
	This->ConstructFromWgIdL(aWgId);
	return This;
	}

EXPORT_C CApaWindowGroupName* CApaWindowGroupName::NewLC(const RWsSession& aWsSession, const TDesC& aWgName)
	//
	// static initializer from full name
	// ONLY TO BE USED WITH A VALID (i.e existing) WINDOW GROUP NAME
	//
/** Creates a window group name object, assigns to it the specified name, and puts 
a pointer to the new object onto the cleanup stack.

The name must have valid format. Typically, this is an existing window group 
name.

@param aWsSession A window server session.
@param aWgName A valid window group name.
@return A pointer to the new window group name object. */
	{
	CApaWindowGroupName* This=new(ELeave) CApaWindowGroupName(aWsSession);
	CleanupStack::PushL(This);
	This->iBuf=aWgName.AllocL();
	This->GetStatusFromName();
	return This;
	}

CApaWindowGroupName::CApaWindowGroupName(const RWsSession& aWsSession) 
	: iWsSession(aWsSession)
	//
	// private c'tor 
	//
	{
	}


void CApaWindowGroupName::DefaultConstructL()
	{
	iStatus=0;
	iBuf = KDefaultWindowGroupName().AllocL();
	}	

EXPORT_C void CApaWindowGroupName::ConstructFromWgIdL(TInt aWgId)
	//
	// Allocate and format iBuf according to existing name given by aWgId
	//
/** Assigns to this object the name taken from the specified window group.

Any previously existing name contained by this window group name object is 
lost.

If the specified window group does not exist or it has no name, then a default 
name is assigned.

@param aWgId The ID of a window group. */
	{
	delete iBuf;
	iBuf=NULL;
	if (aWgId>0)
		{
		TBuf<KMaxWindowGroupNameLength> name;
		((RWsSession&)iWsSession).GetWindowGroupNameFromIdentifier(aWgId, name);
		if (name.Length()==0) // name not set
			DefaultConstructL();
		else
			{
			iBuf=name.AllocL();
			GetStatusFromName();
			}
		}
	else
		DefaultConstructL();
	}

EXPORT_C void CApaWindowGroupName::SetWindowGroupNameL(const TDesC& aWgName)
/** Sets the full window group name in this object.

@param aWgName The full window group name. */
	{
	delete iBuf;
	iBuf=NULL;
	iBuf=aWgName.AllocL();
	GetStatusFromName();
	}

EXPORT_C void CApaWindowGroupName::SetWindowGroupName(HBufC* aWgName)
/** Sets the full window group name in this object, taking ownership of the specified 
heap descriptor.

The function cannot leave.

@param aWgName A heap descriptor containing the full window group name. The 
pointer must not be null, otherwise the function raises a APGRFX 3 panic. */
	{
	if (aWgName)
		{
		if (iBuf != aWgName)
			{
			delete iBuf;
			iBuf=aWgName;
			}
		GetStatusFromName();
		}
	else
		Panic(EPanicNullWindowGroupName);
	}

EXPORT_C void CApaWindowGroupName::FindByAppUid(TUid aAppUid, RWsSession& aWsSession, TInt& aPrevWgId)
/** Searches for the next window group by application UID.

A running application, also known as a task, is associated with a window group. 
The function searches for the next task running the specified application, 
and returns its associated window group ID.

@param aAppUid The application specific UID.
@param aWsSession A window server session.
@param aPrevWgId On entry, the previous window group ID. On return, the next 
window group ID. On first call to this function pass a zero value. When there 
are no more matching window groups, contains KErrNotFound. */
	{
	TBuf<20> match(KSearchAnyFile);	// status
	match.Append(0);
	TBuf<8> uidBuf;
	//Type casting the Uid (which is TInt) to TUint to perform proper conversion
	//when aAppUid.iUid is negative.
	uidBuf.Num(TUint(aAppUid.iUid), EHex);
	match.Append(uidBuf);  // uid
	match.Append(0);
	match.Append('*');	// caption
	match.Append(0);	
	match.Append('*');	// docname
	aPrevWgId=aWsSession.FindWindowGroupIdentifier(aPrevWgId, match, 0);
	}

EXPORT_C void CApaWindowGroupName::FindByCaption(const TDesC& aCaption, RWsSession& aWsSession, TInt& aPrevWgId)
/** Searches for the next window group by caption.

A running application, also known as a task, is associated with a window group. 
The function searches for the next task having the specified caption, and 
returns its associated window group ID.

@param aCaption The caption.
@param aWsSession A window server session.
@param aPrevWgId On entry, the previous window group ID. On return, the next 
window group ID. On first call to this function pass a zero value. When there 
are no more matching window groups, contains KErrNotFound. */
	{
	TApaSearchString match(KSearchAnyFile); // status
	match.Append(0);
	match.Append('*'); // uid
	match.Append(0);
	match.Append(aCaption);	 // caption
	match.Append(0);
	match.Append('*'); //doc name
	aPrevWgId=aWsSession.FindWindowGroupIdentifier(aPrevWgId, match, 0);
	}

EXPORT_C void CApaWindowGroupName::FindByDocName(const TDesC& aDocName, RWsSession& aWsSession, TInt& aPrevWgId)
/** Searches for the next window group by document name.

A running application, also known as a task, is associated with a window group. 
The function searches for the next task handling the specified document, and 
returns its associated window group ID.

@param aDocName The name of the document.
@param aWsSession A window server session.
@param aPrevWgId On entry, the previous window group ID. On return, the next 
window group ID. On first call to this function pass a zero value. When there 
are no more matching window groups, contains KErrNotFound. */
	{
	TApaSearchString match(KSearchAnyFile); // status
	match.Append(0);
	match.Append('*'); // uid
	match.Append(0);
	match.Append('*'); // caption
	match.Append(0);
	match.Append(aDocName);
	aPrevWgId=aWsSession.FindWindowGroupIdentifier(aPrevWgId, match, 0);
	}

EXPORT_C void CApaWindowGroupName::SetBusy(TBool aBusy)
/** Sets the task's busy status in this object.

@param aBusy ETrue, marks the task as busy; EFalse, marks the task as not 
busy.
@see CEikonEnv::SetBusy() */
	{
	iStatus&=(~EBusy);
	if (aBusy)
		iStatus|=EBusy;
	WriteStatusToName();
	}

EXPORT_C TBool CApaWindowGroupName::IsBusy() const
/** Tests whether the task is marked as busy.

@return True, if the task is marked as busy; false, otherwise.
@see CEikonEnv::IsBusy() */
	{
	return iStatus&EBusy;
	}

EXPORT_C void CApaWindowGroupName::SetSystem(TBool aSystem)
/** Sets the task's system status in this object.

@param aSystem ETrue, marks the task as a system task; EFalse, marks the task 
as not a system task.
@see CEikonEnv::SetSystem() */
	{
	iStatus&=(~ESystem);
	if (aSystem)
		iStatus|=ESystem;
	WriteStatusToName();
	}

EXPORT_C TBool CApaWindowGroupName::IsSystem() const
/** Tests whether the task is marked as a system task.

@return True, if the task is marked as a system task; false, otherwise. 
@see CEikonEnv::IsSystem() */
	{
	return iStatus&ESystem;
	}

EXPORT_C void CApaWindowGroupName::SetDocNameIsAFile(TBool aDocNameIsAFile)
/** Sets the document name status in this object.

@param aDocNameIsAFile ETrue, the document name is a filename; EFalse, the 
document name is not a filename.
@see CEikonEnv::SetDocNameIsAFile() */
	{
	iStatus&=(~EDocNameNotAFile);
	if (!aDocNameIsAFile)
		iStatus|=EDocNameNotAFile;
	WriteStatusToName();
	}

EXPORT_C TBool CApaWindowGroupName::DocNameIsAFile() const
/** Tests whether the document name is a file.

@return True, if the document name is a file; false, otherwise.
@see CEikonEnv::DocNameIsAFile() */
	{
	return !(iStatus&EDocNameNotAFile);
	}

EXPORT_C void CApaWindowGroupName::SetRespondsToShutdownEvent(TBool aRespondsToShutdownEvent)
/** Sets the task's shutdown handling status in this object.

@param aRespondsToShutdownEvent ETrue, if the task can deal with a shutdown 
request; EFalse, if the task cannot deal with a shutdown request.
@see CEikonEnv::SetRespondsToShutdownEvent() */
	{
	iStatus&=(~EDoesNotRespondToShutdownEvent);
	if (!aRespondsToShutdownEvent)
		iStatus|=EDoesNotRespondToShutdownEvent;
	WriteStatusToName();
	}

EXPORT_C TBool CApaWindowGroupName::RespondsToShutdownEvent() const
/** Tests whether the task can deal with a request to shutdown.

@return True, if the task can deal with a request to shutdown; false, otherwise. 

@see CEikonEnv::RespondsToShutdownEvent() */
	{
	return !(iStatus&EDoesNotRespondToShutdownEvent);
	}

EXPORT_C void CApaWindowGroupName::SetRespondsToSwitchFilesEvent(TBool aRespondsToSwitchFilesEvent)
/** Sets the task's switch file handling status in this object.

@param aRespondsToSwitchFilesEvent ETrue, if the task can deal with a request 
to switch file; EFalse, if the task cannot deal with with a request to switch 
files.
@see CEikonEnv::SetRespondsToSwitchFilesEvent() */
	{
	iStatus&=(~EDoesNotRespondToSwitchFilesEvent);
	if (!aRespondsToSwitchFilesEvent)
		iStatus|=EDoesNotRespondToSwitchFilesEvent;
	WriteStatusToName();
	}

EXPORT_C TBool CApaWindowGroupName::RespondsToSwitchFilesEvent() const
/** Tests whether the task can deal with a request to switch files.

@return True, if the task can deal with a request to switch files; false otherwise.
@see CEikonEnv::RespondsToSwitchFilesEvent() */
	{
	return !(iStatus&EDoesNotRespondToSwitchFilesEvent);
	}

EXPORT_C void CApaWindowGroupName::SetHidden(TBool aIsHidden)
/** Marks the task as hidden.

In general, tasks marked as hidden do not appear in tasklists. 
Specifically, TApaTask::FindByPos() will ignore any tasks marked as hidden.

@param aIsHidden ETrue if the task is to be marked as hidden; EFalse if not. */
	{
	iStatus&=(~EIsHidden);
	if (aIsHidden)
		iStatus|=EIsHidden;
	WriteStatusToName();
	}

EXPORT_C TBool CApaWindowGroupName::Hidden() const
/** Tests whether the task is marked as hidden.

@return True if the task is hidden; false otherwise. */
	{
	return (iStatus&EIsHidden);
	}

EXPORT_C void CApaWindowGroupName::SetAppReady(TBool aIsReady)
/** Sets the task as ready.

@param aIsReady ETrue if the task is to be marked as ready; EFalse if not. */
	{
	iStatus&=(~EAppReady);
	if (aIsReady)
		iStatus|=EAppReady;
	WriteStatusToName();
	}

EXPORT_C TBool CApaWindowGroupName::IsAppReady() const
/** Tests whether the task is marked as ready.

@return True if the task is ready; false otherwise. */
	{
	return (iStatus&EAppReady);
	}

EXPORT_C void CApaWindowGroupName::SetAppUid(TUid aAppUid)
/** Sets the specified UID into the window group name in this object.

@param aAppUid The application specific UID. */
	{
	TInt start=FindDelimiter(EEndStatus);
	if (start>0)
		{
		start++;
		TInt end=FindDelimiter(EEndUid);
		if (end>0)
			{
			TBuf<KUidBufLength> uidBuf;
		 	//Type casting the Uid (which is TInt) to TUint to perform proper conversion
			//when aAppUid.iUid is negative.
			uidBuf.Num(TUint(aAppUid.iUid), EHex);
			TPtr des=iBuf->Des();
			des.Replace(start, end-start, uidBuf);
			}
		}
	}


EXPORT_C TUid CApaWindowGroupName::AppUid() const
/** Gets the UID of the task's application.

@return The application specific UID. If the format of the window group name 
is not recognized, then this is zero. */
	{
	TUid uid=TUid::Null();
	TInt start=FindDelimiter(EEndStatus);
	if (start>0)
		{
		start++;
		TInt end=FindDelimiter(EEndUid);
		if ((end-start) == KUidBufLength)
			{
			TBuf<KUidBufLength> uidBuf=iBuf->Mid(start, end-start);
			TLex lex(uidBuf);
			TUint32 val = 0;
			lex.Val(val, EHex); //Ignoring error code since we anyways have to return null uid if this returns an error.
			uid.iUid = val;
			}
		}
	return uid;
	}

EXPORT_C void CApaWindowGroupName::SetCaptionL(const TDesC& aCaption)
/** Sets the specified caption into the window group name in this object.

@param aCaption The caption. */
	{	 
	TInt start=FindDelimiter(EEndUid);
	if (start>0)
		{
		start++;
		TInt end=FindDelimiter(EEndCaption);
		if (end>0)
			{
			TInt length=end-start;
			ReAllocIfNecessaryL(aCaption.Length()-length);
			TPtr des=iBuf->Des();
			des.Replace(start, length, aCaption);
			}
		}
	}

EXPORT_C TPtrC CApaWindowGroupName::Caption() const
/** Gets the task's caption.

@return A non-modifiable pointer descriptor representing the caption. If the 
format of the window group name is not recognized, then this is a zero length 
descriptor. */
	{
	TInt start=FindDelimiter(EEndUid);
	if (start>0)
		{
		start++;
		TInt end=FindDelimiter(EEndCaption);
		if (end>0)
			return iBuf->Mid(start, end-start);
		}
	return TPtrC(); // error
	}

EXPORT_C void CApaWindowGroupName::SetDocNameL(const TDesC& aDocName)
/** Sets the specified document name into the window group name in this object.

@param aDocName The document name. */
	{
	TInt start=FindDelimiter(EEndCaption);
	if (start>0)
		{
		start++;
		TInt end=iBuf->Length();
		if (end>0)
			{
			TInt length=end-start;
			ReAllocIfNecessaryL(aDocName.Length()-length);
			TPtr des=iBuf->Des();
			des.Replace(start, length, aDocName);
			}
		}
	}

EXPORT_C TPtrC CApaWindowGroupName::DocName() const
/** Gets the name of the document that the task is handling.

@return A non-modifiable pointer descriptor representing the document name. 
If the format of the window group name is not recognized, then this is a zero 
length descriptor. */
	{
	TInt start=FindDelimiter(EEndCaption);
	if (start>0)
		{
		start++;
		TInt end=iBuf->Length();
		if (end>0)
			return iBuf->Mid(start, end-start);
		}
	return TPtrC(); // error
	}

EXPORT_C TInt CApaWindowGroupName::SetWindowGroupName(RWindowGroup& aGroupWin) const
	//
	// Set aGroupWin's name to the current name in iBuf (returns WServ error code)
	//
/** Sets the window group name contained by this object into the specified window 
group.

@param aGroupWin The window group.
@return KErrNone, if successful; otherwise, one of the other system-wide error 
codes. */
	{
	return aGroupWin.SetName(*iBuf);
	}

EXPORT_C TPtrC CApaWindowGroupName::WindowGroupName() const
/** Gets the full window group name.

@return A non-modifiable pointer descriptor representing the full window group 
name. */
	{
	return *iBuf;
	}

void CApaWindowGroupName::WriteStatusToName()
	//
	// replaces two digit hex number at front of iBuf by iStatus 
	//
	{
	TBuf<2> status;
	status.Format(KFormatStatus, iStatus);
	TPtr des=iBuf->Des();
	des.Replace(0,2,status);
	}

void CApaWindowGroupName::GetStatusFromName()
	//
	// Extracts the two digit hex number at front of iBuf into iStatus 
	//
	{
	TBuf<2> status=iBuf->Left(2);
	TLex lex(status);
	lex.Val(iStatus, EHex);
	}

TInt CApaWindowGroupName::FindDelimiter(TApaDelimiter aDelim) const
	//
	// returns the pos of aDelim or KErrNotFound
	//
	{
	TInt pos=-1;
	TInt length=iBuf->Length();
	for (TInt i=0; i<aDelim; i++)
		{
		TInt nextPos=(iBuf->Right(length-pos-1)).Locate(0);
		if (nextPos<0)
			return KErrNotFound;
		pos=pos+nextPos+1;
		}
	return pos;
	}

void CApaWindowGroupName::ReAllocIfNecessaryL(TInt aExtraLengthReqd)
	//
	// Reallocates iBuf if currentLength+extraLength<totalLength
	//
	{
	TInt existing=iBuf->Length();
	TInt total=existing+aExtraLengthReqd;
	if (total>iBuf->Des().MaxLength())
		iBuf=iBuf->ReAllocL(total);
	}
