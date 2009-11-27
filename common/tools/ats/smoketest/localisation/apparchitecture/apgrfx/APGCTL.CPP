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

#include <e32uid.h>
#include "APGCTL.H"
#include "APGICNFL.H" 
#include "APGSTD.H" 
#include "APFDEF.H"
#include "../apparc/TRACE.H"
#include "APGCLI.H"
#include "APACMDLN.H"

///////////////////////////////////////////
// CApaSystemControl
///////////////////////////////////////////

CApaSystemControl* CApaSystemControl::NewL(RFs& aFs,const TDesC& aFullPath,const TUidType aUidType)
	{
	CApaSystemControl* self=new(ELeave) CApaSystemControl(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aFullPath,aUidType);
	CleanupStack::Pop();
	return self;
	}

CApaSystemControl::CApaSystemControl(RFs& aFs)
	:iExists(ETrue),
	 iFs(aFs)
	{}


CApaSystemControl::~CApaSystemControl()
	{
	delete iCaption;
	delete iShortCaption;
	delete iIcon;
	iNext = NULL;
	}

void CApaSystemControl::ConstructL(const TDesC& aFullPath,const TUidType aUidType)
	{
	//
	// check the file type
	if (aUidType[1]!=KUidApp) 
		User::Leave(KErrNotSupported);

	iFullPath=aFullPath;
	iUidType=aUidType;

	//Construct the new style control panel application. 
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	TApaAppInfo info;
	if(ls.GetAppInfo(info, aUidType[2])==KErrNone && info.iCaption.Length()>0)
		{
		iCaption=info.iCaption.AllocL();
		iShortCaption=iCaption->AllocL();
		}
	// get the icon for the application
	CApaMaskedBitmap *icon=CApaMaskedBitmap::NewLC();
	User::LeaveIfError(ls.GetAppIcon(aUidType[2], TSize(48,48), *icon));
	CleanupStack::Pop(icon);
	iIcon=icon;
	CleanupStack::PopAndDestroy(&ls);

	if (!iCaption)
		{
		TParsePtrC ptr(aFullPath);
		iCaption = ptr.Name().AllocL();
		iShortCaption = iCaption->AllocL();
		}
	}

EXPORT_C void CApaSystemControl::CreateL()
/*
Connects to apparc server and ask the server to launch an application.
*/
	{
	RApaLsSession apparcsession;
	User::LeaveIfError(apparcsession.Connect());
	CleanupClosePushL(apparcsession);
	TThreadId threadId;
	CApaCommandLine *commandLine=CApaCommandLine::NewLC();
	commandLine->SetExecutableNameL(iFullPath);
	commandLine->SetCommandL(EApaCommandRunWithoutViews);
	User::LeaveIfError(apparcsession.StartApp(*commandLine,threadId));
	CleanupStack::PopAndDestroy(2,&apparcsession);
		
	//To preserve the existing synchronous behaviour of control panel items
	//we logon to the newly started control panel thread and wait
	//till it exits.
	RThread thread;
	User::LeaveIfError(thread.Open(threadId,EOwnerThread));
	TRequestStatus status;
	thread.Logon(status);
	User::WaitForRequest(status);
	thread.Close();
	} //lint !e1762 Member function could be made const - Lint is wrong, it should not be const

EXPORT_C TUid CApaSystemControl::Type()const
/** Gets the UID that uniquely identifies the control.

@return The UID. */
	{
	return iUidType[2];
	}

EXPORT_C TFileName CApaSystemControl::FileName()const
/** Gets the full path name of the control. 

@return The full path name. */
	{
	return iFullPath;
	}

EXPORT_C CApaMaskedBitmap* CApaSystemControl::Icon()const
/** Gets the control's icon.

@return The icon bitmap. */
	{
	return iIcon;
	}

EXPORT_C TPtrC CApaSystemControl::Caption()const
/** Gets the control's caption.

@return A non-modifiable pointer descriptor representing the control's caption. */
	{
	__ASSERT_DEBUG(iCaption,Panic(EDPanicNoCaptionInControl));
	return *iCaption;
	}


EXPORT_C TPtrC CApaSystemControl::ShortCaption()const
/** Gets the control's short caption.

@return A non-modifiable pointer descriptor representing the control's short 
caption. */
	{
	__ASSERT_DEBUG(iCaption,Panic(EDPanicNoCaptionInControl));
	if (iShortCaption)
		return *iShortCaption;
	else
		return *iCaption;
	}

///////////////////////////////////////////
// CApaSystemControlList
///////////////////////////////////////////

EXPORT_C CApaSystemControlList* CApaSystemControlList::NewL(RFs& aFs)
/** Allocates and constructs a control panel application list. After construction, 
it calls UpdateL(), to initialise the list.

@param aFs Handle to a file server session.
@return	The newly created control panel application list. */
	{
	CApaSystemControlList* self=new(ELeave) CApaSystemControlList(aFs);
	CleanupStack::PushL(self);
	self->UpdateL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CApaSystemControlList::~CApaSystemControlList()
/** Destructor. */
	{
	CApaSystemControl* control=iControl;
	CApaSystemControl* next;
	while (control)
		{
		next = control->iNext;
		delete control;
		control = next;
		}
	iControl = NULL;
	}

EXPORT_C TInt CApaSystemControlList::Count()const
/** Gets the number of control panel applications in the list.

@return The number of control panel applications in the list. */
	{
	TInt count=0;
	CApaSystemControl* control=iControl;
	while (control)
		{
		count++;
		control = control->iNext;
		}
	return count;
	}


EXPORT_C TInt CApaSystemControlList::Index(TUid aType)const
/** Gets the index into the list of the control panel application 
whose third UID matches the specified UID.

@param aType The control panel application specific UID.
@return The index of the control panel application if there is a 
match, or KErrNotFound if not. */
	{
	TInt count=0;
	CApaSystemControl* control=iControl;
	while (control && control->Type()!=aType)
		{
		count++;
		control = control->iNext;
		}
	if (!control)
		count = KErrNotFound;
	return count;
	}


EXPORT_C CApaSystemControl* CApaSystemControlList::Control(TInt aIndex)const
/** Gets the control panel application at the specified index in the list.

@param aIndex Index of a control panel application in the list. 
@return The control panel application at the specified index in the list.
@panic APGRFX 10. The index is out of range. */

	{
	TInt count=0;
	CApaSystemControl* control=iControl;
	while (control && count!=aIndex)
		{
		count++;
		control = control->iNext;
		}
	//
	__ASSERT_ALWAYS(control,Panic(EPanicIndexOutOfRange));
	return control;
	}


EXPORT_C CApaSystemControl* CApaSystemControlList::Control(TUid aType)const
/** Gets the control panel application in the list with the specified UID. 

@param aType The control panel application's third UID. 
@return The control panel application whose type matches aType, or null 
if none match. */

	{
	CApaSystemControl* control=iControl;
	while (control && control->Type()!=aType)
		control = control->iNext;
	return control;
	}


CApaSystemControl* CApaSystemControlList::PreviousControl(const CApaSystemControl* aControl) const
	{
	CApaSystemControl* control=iControl;
	CApaSystemControl* previous=NULL;
	while (control && control!=aControl)
		{
		previous = control;
		control = control->iNext;
		}
	if (!control)
		previous = NULL;
	return previous;
	}


EXPORT_C void CApaSystemControlList::UpdateL()
/** Updates the list of control panel applications. For each new one found, a CApaSystemControl 
object is created and added to the list. Control panel applications that no longer 
exist are removed, and applications already in the list can be replaced 
by ones found on earlier drives in the search order(y:->a: then z:). */

// increments iUpdateCount if list has changed
// if an error occurs, the list will not be complete but will be functional
 
	{
	__SHOW_TRACE(_L("Starting CApaSystemControlList::UpdateL()"));
	// set all the current controls to "dont exist", so we can find them again
	CApaSystemControl* control=iControl;
	while (control)
		{
		control->iExists = EFalse;
		control = control->iNext;
		}

	//Connect to the apparc server and get the control panel application. 
	TBool listChanged=EFalse;
	RApaLsSession apparcsession;
	User::LeaveIfError(apparcsession.Connect());
	CleanupClosePushL(apparcsession);
	//Set the filter
	User::LeaveIfError(apparcsession.GetFilteredApps(TApaAppCapability::EControlPanelItem,TApaAppCapability::EControlPanelItem));
	
	TApaAppInfo aInfo;
	//Fetch the control panel information one by one and add a corresponding control
	//to the control's list.
	while(apparcsession.GetNextApp(aInfo) == KErrNone)
		{
		control=Control(aInfo.iUid);
		if (control == NULL)
			{// not in list, so add it at the start
			__SHOW_TRACE(_L("...New control located"));
			listChanged=ETrue;
			TUidType uidType(KExecutableImageUid,KUidApp,aInfo.iUid);
			TRAPD(err,control = CApaSystemControl::NewL(iFs,aInfo.iFullName,uidType));
			if (err==KErrNone)
				{
				__SHOW_TRACE(_L("...control added"));
				control->iNext = iControl;
				iControl = control;
				}
			}
		else if (!control->iExists)
			{// not already found - we made need to override this one
			if (aInfo.iFullName.CompareF(control->FileName()) != 0)
				{
				__SHOW_TRACE(_L("...new instance of control - delete old one"));
				// delete the old one before creating the new one so that the correct library is loaded
				CApaSystemControl* prev=PreviousControl(control);
				if (prev)
					{
					prev->iNext=control->iNext;
					}
				else
					{
					iControl=control->iNext;
					}
				delete control;
				control=NULL;
				listChanged=ETrue;
				// create the new one. Add it to the list if this is successful
				__SHOW_TRACE(_L("...create new one"));
				TUidType uidType(KExecutableImageUid,KUidApp,aInfo.iUid);
				TRAPD(err,control = CApaSystemControl::NewL(iFs,aInfo.iFullName,uidType));
				if (err==KErrNone)
					{
					__SHOW_TRACE(_L("...new one created"));
					control->iNext=iControl;
					iControl=control;
					}
				}
			else
				{
				control->iExists=ETrue;
				}
			}
		}
		
	CleanupStack::PopAndDestroy(&apparcsession); //apparcsession destroy

	CApaSystemControl* previousControl=NULL;
	control = iControl;
	while (control)
		{
		if (!control->iExists)
			{
			listChanged=ETrue;
			if (!previousControl)
				{// this must be the first control in the list, ie iControl
				iControl = control->iNext;
				delete control;
				control = iControl;
				}
			else
				{
				previousControl->iNext = control->iNext;
				delete control;
				previousControl = previousControl->iNext;
				control = previousControl->iNext;
				}
			}
		else
			{
			control = control->iNext;
			}
		}
	//
	// increment the counter if the list has changed
	if (listChanged)
		{
		iUpdateCount++;
		}
	}

CApaSystemControlList::CApaSystemControlList(RFs& aFs):iFs(aFs)
	{
	}
