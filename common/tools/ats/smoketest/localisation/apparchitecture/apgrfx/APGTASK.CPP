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

#include "APGTASK.H"
#include "APGWGNAM.H"
#include <w32std.h>

//
// class TApatask
//


	
EXPORT_C TApaTask::TApaTask(RWsSession& aWsSession)
	: iWsSession(aWsSession), iWgId(0)
/** Constructs an empty task object, taking a reference to a window server session.

An object of this type is constructed by an instance of the TApaTaskList class.

The object represents a task when it is assigned a task's window group ID.

@param aWsSession The window server session.
@see TApaTaskList */
	{
	}

EXPORT_C void TApaTask::SetWgId(TInt aWgId)
/** Sets this task's window group ID.

@param aWgId The ID to be assigned. */
	{
	iWgId=aWgId;
	}

EXPORT_C TInt TApaTask::WgId() const
/** Gets the ID of this task's window group.

@return The window group ID. For an empty task object, this is zero. */
	{
	return iWgId;
	}

EXPORT_C TBool TApaTask::Exists() const
/** Tests whether this TApaTask object is empty. This object represents the
state of the task at the time at which it was constructed and is not subsequently
updated. Therefore, this does not indicate that the task itself exists and should not 
be used to test whether or not a particular task is running or not.

@return True, if the task is not empty; false, otherwise.
@see TApaTaskList::FindDoc()
@see TApaTaskList::FindApp()
@see TApaTaskList::FindByPos() */
	{
	return(iWgId>0);
	}

EXPORT_C TThreadId TApaTask::ThreadId() const
/** Gets the ID of this task's thread

@return The thread ID. */
	{
	TThreadId threadId;
	if (iWsSession.GetWindowGroupClientThreadId(iWgId,threadId)==KErrNotFound)
		{
		threadId=TThreadId(KNullThreadId);
		}
	return(threadId);
	}

EXPORT_C void TApaTask::BringToForeground()
/** Brings this task to the foreground.

If the task uses the View architecture, then the task's top view is activated. */
	{
	iWsSession.SetWindowGroupOrdinalPosition(iWgId,0);
	SendSystemEvent(EApaSystemEventBroughtToForeground);
	}

EXPORT_C void TApaTask::SendToBackground()
/** Sends this task to the background.

The task whose window group is at the next ordinal position is brought up 
to the foreground. In addition, the new foreground task's top view is activated, 
if it uses the View architecture. */
	{
	iWsSession.SetWindowGroupOrdinalPosition(iWgId,-1);
	}

EXPORT_C void TApaTask::EndTask()
/** Requests normal closing of this task. 

@capability PowerMgmt is required to close system tasks. */
	{
	RProcess client;
	if (client.HasCapability(ECapabilityPowerMgmt))
		{
		SendSystemEvent(EApaSystemEventSecureShutdown, EEventPowerMgmt);
		}
	
	// Always send the old shutdown message for backward compatibility. Will not shut down system tasks.
	SendSystemEvent(EApaSystemEventShutdown); 
	}
	
EXPORT_C void TApaTask::KillTask()
/** Kills this task. 
@capability PowerMgmt
*/
	{
	RThread thread;
	TInt err=thread.Open(ThreadId());
	if (!err)
		{
		RProcess process;
		thread.Process(process);
		process.Terminate(0);
		process.Close();
		thread.Close();
		}
	} //lint !e1762 Suppress Member function 'TApaTask::KillTask(void)' could be made const


EXPORT_C TInt TApaTask::SwitchOpenFile(const TDesC& aFilename)
/** Requests the task to close its existing document, and to open an existing document.

An application (task) may handle the request by overriding CEikAppUi::OpenFileL() if required.

@param aFilename The name of the document to be opened.
@return KErrNone, if the request was successfully sent to the task; otherwise one of the other
system-wide error codes. */
	{
	TInt err=CheckSwitchFile();
	if (!err)
#if defined(_UNICODE)
		{
		TPtrC8 messageBuffer((TUint8*) aFilename.Ptr(),aFilename.Length()<<1);
		err=SendMessage(KUidApaMessageSwitchOpenFile,messageBuffer);
		}
#else
		err=SendMessage(KUidApaMessageSwitchOpenFile,aFilename);
#endif
	return err;
	}

EXPORT_C TInt TApaTask::SwitchCreateFile(const TDesC& aFilename)
/** Requests the task to close its existing document, and to create and open a new 
document.

An application (task) may handle the request by overriding CEikAppUi::CreateFileL() if required.

@param aFilename The name of the new document.
@return KErrNone, if the request was successfully sent to the task; otherwise one of the other
system-wide error codes. */
	{
	TInt err=CheckSwitchFile();
	if (!err)
#if defined(_UNICODE)
		{
		TPtrC8 messageBuffer((TUint8*) aFilename.Ptr(),aFilename.Length()<<1);
		err=SendMessage(KUidApaMessageSwitchCreateFile,messageBuffer);
		}
#else
		err=SendMessage(KUidApaMessageSwitchCreateFile,aFilename);
#endif
	return err;
	}

TInt TApaTask::CheckSwitchFile() const
	//
	// private - checks whether the task will respond to a switch files event
	//
	{
	HBufC* buf=HBufC::NewMax(CApaWindowGroupName::EMaxLength);
	if (!buf)
		return KErrNoMemory;
	TInt err=KErrNone;
	TPtr des=buf->Des();
	err=iWsSession.GetWindowGroupNameFromIdentifier(iWgId, des);
	if (!err)
		{
		CApaWindowGroupName* wgName=CApaWindowGroupName::New(iWsSession, buf); // takes ownership (if it succeeds)
		if (!wgName)
			{
			delete buf;
			return KErrNoMemory;
			}
		if (wgName->IsBusy())
			err=KErrNotReady;
		if (!(wgName->RespondsToSwitchFilesEvent()))
			err=KErrNotSupported;
		delete wgName;
		}
	else
		delete buf;
	return err;   
	}

EXPORT_C TInt TApaTask::SendMessage(TUid aUid,const TDesC8& aParams)
/** Sends a message to this task's window group.

The message is handled by the UI framework, specifically by CEikAppUI::ProcessMessageL(). 


@param aUid The UID identifying the message. By default, the UI framework 
recognizes only two messages, KUidApaMessageSwitchOpenFileValue and KUidApaMessageSwitchCreateFileValue.
@param aParams The message. The format and meaning of the message depends on 
the specific type as identified by the UID.
@return KErrNone, if successful; otherwise, one of the other system-wide error 
codes.
@see CEikAppUi::ProcessMessageL()
@see TEventCode
@see TWsEvent
@see RWindowGroup::FetchMessage() */
	{
	return iWsSession.SendMessageToWindowGroup(iWgId,aUid,aParams);
	}

EXPORT_C void TApaTask::SendKey(TInt aKeyCode,TInt aModifiers)
/** Sends a key event encapsulating the specified character code and specified modifier 
keys state to the task's window group.

Key events are handled by the UI framework, specifically by CCoeAppui::HandleWsEventL().

@capability SwEvent
@param aKeyCode The character code.
@param aModifiers State of the modifier keys. 
@see CCoeAppUi::HandleWsEventL() */
	{
	TKeyEvent key;
	key.iCode=aKeyCode;
	key.iModifiers=aModifiers;
	key.iRepeats=0;
	SendKey(key);
	}

EXPORT_C void TApaTask::SendKey(const TKeyEvent& aKey)
/** Sends the specified key event to the task's window group.

Key events are handled by the UI framework, specifically by CCoeAppui::HandleWsEventL().

@capability SwEvent
@param aKey The key event.
@see CCoeAppUi::HandleWsEventL()
@see TKeyEvent */
	{
	TWsEvent event;
	event.SetType(EEventKey);
	*event.Key()=aKey;
	event.SetTimeNow();
	iWsSession.SendEventToWindowGroup(iWgId,event);
	}

EXPORT_C void TApaTask::SendSystemEvent(TApaSystemEvent aEvent)
/** Sends a system event to this task's window group.

Events are handled by the UI framework, specifically by CEikAppUi::HandleSystemEventL().

@capability SwEvent
@param aEvent The event type.
@see CEikAppUi
@see CCoeAppUi::HandleSystemEventL()
@see TApaSystemEvent */
	{
	SendSystemEvent(aEvent, EEventUser); 
	}

void TApaTask::SendSystemEvent(TApaSystemEvent aEvent, TEventCode aType)
/** 
@internalTechnology
*/
	{
	TWsEvent event;
	event.SetType(aType);
	*(TApaSystemEvent*)(event.EventData())=aEvent;
	event.SetTimeNow();
	iWsSession.SendEventToWindowGroup(iWgId,event);
	}

//
// class TApaTaskList
//


	
EXPORT_C TApaTaskList::TApaTaskList(RWsSession& aWsSession)
	: iWsSession(aWsSession)
/** Constructs the task list object, taking a reference to a window server session.

@param aWsSession The window server session. */
	{
	}

EXPORT_C TApaTask TApaTaskList::FindApp(const TDesC& aAppName)
/** Searches for a task that has the specified caption.

The result of the search depends on the number of tasks that have the specified 
caption.

If there is only one task, then that task is returned.

If there is more than one task, then the task returned depends on whether 
the first one found is in the foreground:

if the first task found is in the foreground, then the task returned by this 
function is the one with the highest window group ordinal value, i.e. the 
task which is furthest from the foreground.

if the first task found is not in the foreground, then that is the task that 
is returned.

If no matching task is found, then the object returned is an empty task, and 
calling TApaTask::Exists() on it returns false.

@param aAppName The caption.
@return A task having the specified caption, or an empty task. */
	{
	TApaTask task(iWsSession);
	TInt wgId=0;
	TInt matchId=0;
	TInt fgWgId=FindByPos(0).WgId();
	CApaWindowGroupName::FindByCaption(aAppName, iWsSession, wgId);
	if (wgId==fgWgId)
		{
		while (wgId>=0)
			{
			matchId=wgId;
			CApaWindowGroupName::FindByCaption(aAppName, iWsSession, wgId);
			}
		}
	else 
		matchId=wgId;
	task.SetWgId(matchId);
	return(task);
	}

EXPORT_C TApaTask TApaTaskList::FindDoc(const TDesC& aDocName)
/** Searches for the task that is handling the specified document.

@param aDocName The name of the document.
@return The task that is handling the specified document. If no such task exists, 
then this is an empty task, i.e. a subsequent call to TApaTask::Exists() returns 
false. */
	{
	TInt wgId=0;
	CApaWindowGroupName::FindByDocName(aDocName, iWsSession, wgId);
	TApaTask task(iWsSession);
	task.SetWgId(wgId);
	return(task);
	}

EXPORT_C TApaTask TApaTaskList::FindByPos(TInt aPos)
/** Searches for a task by the ordinal position of its window group.

@param aPos The ordinal position of a task's window group. A zero value refers 
to the foreground task.
@return The task at the specified position. If there is no task at the specified 
position, or the specified position is invalid, then the object returned is 
an empty task, and calling TApaTask::Exists() on it returns false. */
	{
	TApaTask task(iWsSession);
	TRAP_IGNORE(FindByPosL(task,aPos));
	return(task);
	}

void TApaTaskList::FindByPosL(TApaTask& aTask,TInt aPos)
	{
	TInt wgId=0;
	const TInt count=iWsSession.NumWindowGroups(0);
	if (count)
		{
		CArrayFixFlat<TInt>* wgIdArray=new(ELeave) CArrayFixFlat<TInt>(count);
		CleanupStack::PushL(wgIdArray);
		CApaWindowGroupName* wgName=CApaWindowGroupName::NewL(iWsSession);
		CleanupStack::PushL(wgName);
		User::LeaveIfError(iWsSession.WindowGroupList(0,wgIdArray)); // priority 0 == mostly normal apps but some may be hidden window groups

		for (TInt ii=0; ii<wgIdArray->Count(); ii++) // must ask for count each time as this may change
			{ 
			wgId=(*wgIdArray)[ii];
			wgName->ConstructFromWgIdL(wgId);
			if(wgName->Hidden())
				{
				wgIdArray->Delete(ii--); // array element removed so now need to do this index again
				}
			}
		
		if (aPos>=count || aPos<0)
			aPos=count-1;

		if(aPos<wgIdArray->Count())
			wgId=(*wgIdArray)[aPos];

		CleanupStack::PopAndDestroy(2);	// wgIdArray, wgName
		}
	aTask.SetWgId(wgId);
	}


EXPORT_C TApaTask TApaTaskList::FindApp(TUid aAppUid)
/** Searches for a task running the specified application.

The result of the search depends on the number of tasks that are running the 
specified application.

If there is only one task, then that task is returned.

If there is more than one task, then the task returned depends on whether 
the first one found is in the foreground:

if the first task found is in the foreground, then the task returned by this 
function is the one with the highest window group ordinal value, i.e. the 
task which is furthest from the foreground.

if the first task found is not in the foreground, then that is the task that 
is returned.

If no matching task is found, then the object returned is an empty task, and 
calling TApaTask::Exists() on it returns false.

@param aAppUid The application specific UID.
@return A task having the specified caption, or an empty task. */
	{
	TApaTask task(iWsSession);
	TInt wgId=0;
	TInt matchId=0;
	TInt fgWgId=FindByPos(0).WgId();
	CApaWindowGroupName::FindByAppUid(aAppUid,iWsSession,wgId);
	if (wgId==fgWgId)
		{
		while (wgId>=0)
			{
			matchId=wgId;
			CApaWindowGroupName::FindByAppUid(aAppUid,iWsSession,wgId);
			}
		}
	else 
		matchId=wgId;
	task.SetWgId(matchId);
	return task;
	}

EXPORT_C TInt TApaTaskList::CycleTasks(TUid aAppUid,TCycleDirection aDirection)
/** Brings the next task in the set of tasks running the specified application to 
the foreground.

If there is only one task, then no change occurs.

If the foremost task in the set is not the foreground task, then this is made 
the foreground task.

Thereafter, successive calls to this function bring the next task in the set 
to the foreground. The direction of the cycling can be specified and has the 
following effect:

for the forwards direction, the foreground task is sent to the background; 
the next foremost task is made the foreground task.

for the backwards direction, the task with the highest window group ordinal 
value, i.e. the task in the set which is furthest from the foreground, is 
brought to the foreground. The task that was the foremost task in the set 
is moved back by one position.

If the task brought to the foreground uses the View architecture, then the 
its top view is activated.

@param aAppUid The application specific UID. 
@param aDirection The direction of cycling.
@return KErrNone, if successful; KErrNotFound, if there are no tasks running 
the specified application. */
	{
	TInt sendToBgWgId=KErrNotFound;
	TInt wgId=0;
	TInt fgWgId=FindByPos(0).WgId();
	CApaWindowGroupName::FindByAppUid(aAppUid,iWsSession,wgId);
	TInt matchId=wgId;
	if (wgId==fgWgId)											// If first match is at foreground
		{
		if (aDirection==EBackwards)
			{
			while (wgId>=0)										// Go for last match
				{
				matchId=wgId;
				CApaWindowGroupName::FindByAppUid(aAppUid,iWsSession,wgId);
				}
			}
		else
			{
			CApaWindowGroupName::FindByAppUid(aAppUid,iWsSession,wgId);
			if (wgId<0)											// If no other match
				return KErrNone;
			sendToBgWgId=matchId;
			matchId=wgId;										// Go for second match
			}
		}
	if (matchId>=0)
		{
		iWsSession.SetWindowGroupOrdinalPosition(matchId,0);
		if (sendToBgWgId>=0)
			iWsSession.SetWindowGroupOrdinalPosition(sendToBgWgId,-1);	// Send it to background
		return KErrNone;
		}
	return KErrNotFound;
	}
