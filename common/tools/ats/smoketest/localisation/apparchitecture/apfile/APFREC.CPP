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

#include <apfrec.h>
#include <e32uid.h>
#include <f32file.h>
#include <apadef.h>
#include "APFSTD.H" // Panics etc.

#ifdef USING_ECOM_RECOGS
#include <ecom/ecom.h>
#endif

//
// class TRecognizer
//

CApaScanningFileRecognizer::TRecognizer::TRecognizer(HBufC* aName)
	: iName(aName)
	{
	}

///////////////////////////////
// CApaRecognizerDll
///////////////////////////////

class CApaRecognizerDll : public CBase
	{
public:
	CApaRecognizerDll(HBufC* aName);
	~CApaRecognizerDll(); // closes lib and deletes next

	enum TPresence { ENotPresent, EIsPresent, ESuperseded };
	inline TPresence Present() const { return iPresent; }
	inline void Present(TPresence aPresence) { iPresent = aPresence; }
	
	inline CApaFileRecognizerType* Recognizer() { return iRec; }
	inline void SetRecognizer(CApaFileRecognizerType& aRec) { iRec = &aRec; }
public:
	//lint --e{1925} Suppress "public data member" - bad OO but handy
	CApaRecognizerDll* iNext;
	CApaScanningFileRecognizer::TRecognizer iId; // cached so that this data can be accessed from other threads (RLibrary cant be used in this case)
private:
	RLibrary iLibrary;
	TPresence iPresent;
	CApaFileRecognizerType* iRec; // not ownership
	};


CApaRecognizerDll::~CApaRecognizerDll()
	{
	delete iId.iName;
	iLibrary.Close();
	delete iNext;
	iRec = NULL;
	}

CApaRecognizerDll::CApaRecognizerDll(HBufC* aName)
	: iId(aName)
	{
	}

#ifdef USING_ECOM_RECOGS
// CleanupEComArray function is used for cleanup support 
// of locally declared arrays.
void CleanUpEComInfoArray(TAny* aInfoArray)
	{
	RImplInfoPtrArray* infoArray = (static_cast<RImplInfoPtrArray*>(aInfoArray));
	infoArray->ResetAndDestroy();
	infoArray->Close();
	}
#endif

// class CApaBackupOperationObserver
CApaScanningFileRecognizer::CApaBackupOperationObserver::CApaBackupOperationObserver(CApaScanningFileRecognizer& aRecognizer)
	: iRecognizer(aRecognizer)
	{
	TRAP_IGNORE(iSession = CBaBackupSessionWrapper::NewL());
	}

CApaScanningFileRecognizer::CApaBackupOperationObserver::~CApaBackupOperationObserver()
	{
	if (iSession)
		{
		iSession->DeRegisterBackupOperationObserver(*this);
		delete iSession;
		}
	}

void CApaScanningFileRecognizer::CApaBackupOperationObserver::RegisterObserverL()
	{
	if (iSession)
		{
		iSession->RegisterBackupOperationObserverL(*this);
		}
	}

void CApaScanningFileRecognizer::CApaBackupOperationObserver::HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	if ((aBackupOperationAttributes.iFileFlag == MBackupObserver::EReleaseLockReadOnly ||
		aBackupOperationAttributes.iFileFlag == MBackupObserver::EReleaseLockNoAccess)
		&& aBackupOperationAttributes.iOperation == EStart)
		{
		// release recognizers not in rom
		CApaRecognizerDll* rec=iRecognizer.iRecognizerLib;
		while (rec)
			{
			CApaRecognizerDll* next=rec->iNext;
			TDriveUnit drive(rec->iId.iDrive);
			const TChar driveName=drive.Name()[0];
			if(driveName != TChar('Z'))
				{
				iRecognizer.RemoveRecognizer(*rec);// ignore errors
				}
			rec = next;
			}
		}

	if (aBackupOperationAttributes.iFileFlag == MBackupObserver::ETakeLock &&
		(aBackupOperationAttributes.iOperation == EEnd ||
		aBackupOperationAttributes.iOperation == EAbort))
		{
		iRecognizer.ScanForRecognizersL();
		}
	}

TInt CApaScanningFileRecognizer::CApaBackupOperationObserver::UpdateCounter() const
	{
	return iUpdateCounter;
	}

void CApaScanningFileRecognizer::CApaBackupOperationObserver::SetUpdateCounter(TInt aValue)
	{
	iUpdateCounter = aValue;
	}


///////////////////////////////
// CApaScanningFileRecognizer
///////////////////////////////


EXPORT_C CApaScanningFileRecognizer* CApaScanningFileRecognizer::NewL(RFs& aFs,MApaAppStarter* aAppStarter)
	{
	CApaScanningFileRecognizer* self=new(ELeave) CApaScanningFileRecognizer(aFs,aAppStarter);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->ScanForRecognizersL();
	CleanupStack::Pop(); // self
	return self;
	}


EXPORT_C CApaScanningFileRecognizer::CApaScanningFileRecognizer(RFs& aFs,MApaAppStarter* aAppStarter)
	: CApaFileRecognizer(aFs), iAppStarter(aAppStarter)
	{}


EXPORT_C void CApaScanningFileRecognizer::ConstructL()
	{
	iObserver = new (ELeave) CApaBackupOperationObserver(*this);
	iObserver->RegisterObserverL();
	}


EXPORT_C CApaScanningFileRecognizer::~CApaScanningFileRecognizer()
	{
	DestroyRecognizerList();
	delete iRecognizerLib;
	delete iObserver;
	iAppStarter = NULL;
	}

EXPORT_C void CApaScanningFileRecognizer::ScanForRecognizersL()
	{
	// set all recognizers to not present - pending rediscoivery
	CApaRecognizerDll* rec=iRecognizerLib;
	while (rec)
		{
		rec->Present(CApaRecognizerDll::ENotPresent);
		rec = rec->iNext;
		}

	ScanForEcomRecognizersL();
	//
	// remove any recognizers that are no longer present
	rec=iRecognizerLib;
	while (rec)
		{
		CApaRecognizerDll* next=rec->iNext;
		if (rec->Present()==CApaRecognizerDll::ENotPresent)
			RemoveRecognizer(*rec);
		rec = next;
		}
	}


TInt CApaScanningFileRecognizer::RemoveRecognizer(CApaRecognizerDll& aDll)
	{
	// try to remove the recognizer from the list
	TInt ret=RemoveFileRecognizerType(aDll.Recognizer());
	if (ret!=KErrNone && ret!=KErrNotFound)
		return ret;
	//
	// get a handle to the previous entry in the list
	CApaRecognizerDll* prev=NULL;
	CApaRecognizerDll* dll=iRecognizerLib;
	while (dll && (dll != &aDll))
		{
		prev = dll;
		dll = dll->iNext;
		}
		
	if (!dll)
		{
		Panic(EPanicLibraryNotInList);
		}
	else
		{
		// remove the dll
		if (prev)
			prev->iNext = dll->iNext;
		else
			iRecognizerLib = dll->iNext;
		dll->iNext = NULL;
		delete dll;
		iObserver->SetUpdateCounter( iObserver->UpdateCounter()+ 1 );
		}
	return KErrNone;
	}

EXPORT_C TInt CApaScanningFileRecognizer::RecognizerCount()
	{
	CApaRecognizerDll* lib=iRecognizerLib;
	TInt count=0;
	while (lib)
		{
		count++;
		lib = lib->iNext;
		}
	return count;
	}


EXPORT_C CApaScanningFileRecognizer::CRecognizerArray* CApaScanningFileRecognizer::RecognizerListLC()const
	{
	CRecognizerArray* list=new(ELeave) CArrayFixFlat<TRecognizer>(1);
	CleanupStack::PushL(list);
	CApaRecognizerDll* dll=iRecognizerLib;
	while (dll)
		{
		list->AppendL(dll->iId);
		dll = dll->iNext;
		}
	return list;
	}

EXPORT_C const CApaScanningFileRecognizer::TRecognizer& CApaScanningFileRecognizer::operator[](TInt aIndex)const
	{
	__ASSERT_ALWAYS(aIndex>=0,Panic(EPanicNegativeIndex));
	// scan to correct dll
	TInt num=0;
	CApaRecognizerDll* dll=iRecognizerLib;
	while (dll && num++<aIndex)
		dll = dll->iNext;
	__ASSERT_ALWAYS(dll,Panic(EPanicIndexOutOfRange));
	//
	// get info from dll
	return dll->iId; //lint !e613 Suppress possible use of null pointer - asserted above
	}

EXPORT_C TInt CApaScanningFileRecognizer::UpdateCounter()const
	{
	return iObserver->UpdateCounter();
	}

EXPORT_C void CApaScanningFileRecognizer::SetEcomRecognizersFromListL(const CRecognizerArray& aList)
	{
	//for ecom style
	for (TInt i=0 ; i<aList.Count() ; i++)
		{
		TDriveUnit drive(aList[i].iDrive);
		TRAPD(ret, LoadEcomRecognizerL(drive,aList[i].iUid));
		if (ret==KErrNoMemory)
			{
			User::Leave(ret); 
			}
		}
	}

EXPORT_C void CApaScanningFileRecognizer::SetEcomRecognizerL(const TRecognizer& aRecognizer)
	{
	//for ecom style 
	TDriveUnit drive(aRecognizer.iDrive);
	LoadEcomRecognizerL(drive,aRecognizer.iUid);
	}

//to find the name of  ecom style file recognizer corresponding to aImplUID
#ifdef USING_ECOM_RECOGS
HBufC* RecognizerNameL(TUid aImplUID)
	{
	RImplInfoPtrArray implementationInfoArray;
	TCleanupItem cleanup(CleanUpEComInfoArray, &implementationInfoArray);
	CleanupStack::PushL(cleanup);
	REComSession::ListImplementationsL(KUidFileRecogInterfaceUid,implementationInfoArray);
	const TInt availCount = implementationInfoArray.Count();
	for (TInt ii=0;ii<availCount;++ii)
		{
		CImplementationInformation* info = implementationInfoArray[ii];
		TUid uid = info->ImplementationUid();
		if(uid.iUid == aImplUID.iUid )
			{
			HBufC* recogName = info->DisplayName().AllocL();
			CleanupStack::PopAndDestroy(&implementationInfoArray);
			return recogName;
			}
		}
	CleanupStack::PopAndDestroy(&implementationInfoArray);
	return KNullDesC().AllocL();
	}

//load the ecom file recognizer
void CApaScanningFileRecognizer::LoadEcomRecognizerL(TDriveUnit& aDrive,TUid aUid)
	{
	// check we haven't loaded this one already
	CApaRecognizerDll* lib=iRecognizerLib;
	CApaRecognizerDll* last=NULL; // the previous one in the list
	while (lib)
		{
		if (lib->iId.iUid==aUid)
			{// we may have to override one
			if (lib->Present()!=CApaRecognizerDll::ENotPresent)
				{
				return; // already found
				}
			else
				{
				lib->Present(CApaRecognizerDll::EIsPresent);
				return;
				}
			}			
		last = lib;
		lib = lib->iNext;
		}
	// load the library
	HBufC* recogName=RecognizerNameL(aUid);
	CleanupStack::PushL(recogName);
	lib = new(ELeave) CApaRecognizerDll(recogName);
	CleanupStack::Pop(recogName);
	CleanupStack::PushL(lib);
	TRAPD(error,lib->SetRecognizer(*CApaFileRecognizerType::CreateFileRecognizerL(aUid)));
	if(!error && lib->Recognizer())
		{
		lib->Present(CApaRecognizerDll::EIsPresent);
		SetAppStarter(lib->Recognizer(),iAppStarter);
		AddFileRecognizerType(lib->Recognizer());
		lib->iId.iUid = aUid;
		lib->iId.iDrive = aDrive;
		iObserver->SetUpdateCounter( iObserver->UpdateCounter() + 1 );
		//
		// add lib to the library list
		if (last)
			{
			while (last->iNext)
				{
				last=last->iNext;
				}
			__ASSERT_ALWAYS(last->iNext==NULL,Panic(EPanicIndexOutOfRange));
			last->iNext = lib;
			}
		else
			{
			iRecognizerLib = lib;
			}
		CleanupStack::Pop(lib); // lib
		}
	else
		{
		CleanupStack::PopAndDestroy(lib); // lib
		}
	}
#else
void CApaScanningFileRecognizer::LoadEcomRecognizerL(TDriveUnit&,TUid)
	{
	} //lint !e1762 Suppress member function could be made const

#endif

void CApaScanningFileRecognizer::ScanForEcomRecognizersL()
	{
#ifdef USING_ECOM_RECOGS
	//scan ecom plugin
	RImplInfoPtrArray implementationArray;
	TCleanupItem cleanup(CleanUpEComInfoArray, &implementationArray);
	CleanupStack::PushL(cleanup);
	REComSession::ListImplementationsL(KUidFileRecogInterfaceUid,implementationArray);
	const TInt availCount = implementationArray.Count();
	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		TUid uid = info->ImplementationUid();
		TDriveUnit drive=info-> Drive();
		TRAP_IGNORE(LoadEcomRecognizerL(drive,uid));
		}
	CleanupStack::PopAndDestroy(&implementationArray); 
#endif
	} //lint !e1762 Suppress member function could be made const
