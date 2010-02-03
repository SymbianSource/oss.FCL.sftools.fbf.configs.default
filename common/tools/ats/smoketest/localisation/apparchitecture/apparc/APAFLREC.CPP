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

#include <f32file.h>
#include <apaflrec.h>
#include <apaid.h>
#include <apacmdln.h>
#include "APASTD.H"
#include "APGCLI.H"

#ifdef USING_ECOM_RECOGS
#include <ecom/ecom.h>
#endif

//
//CFileRecognizerExtension Class
//
//stores the destructor key to track the instance of ecom implementation class
class CFileRecognizerExtension : public CBase
 	{
public:
	static CFileRecognizerExtension* NewL(TUid aDtorKey);
 	~CFileRecognizerExtension();
	//returns the destructor key
 	TUid DestructorUid()const;
private:
	CFileRecognizerExtension(TUid aDtorKey);
private:
 	//destructor key to track the instance of ecom implementation class
	TUid iDtorKey;
	};

//
// Class CApaFileRecognizer
//

EXPORT_C CApaFileRecognizer::CApaFileRecognizer(RFs& aFs)
	:iFs(aFs)
	{}


EXPORT_C CApaFileRecognizer::~CApaFileRecognizer()
	{
	DestroyRecognizerList();
	delete iAppLocator;
	//lint -esym(1740,CApaFileRecognizer::iFileRecognizerList) not directly freed - see DestroyRecognizerList()
	}

	
EXPORT_C void CApaFileRecognizer::DestroyRecognizerList()
// this method exists to allow subclassers to destroy the list earlier if they wish
	{
	CApaFileRecognizerType* rec=iFileRecognizerList;
	//delete one element after the other in the list
	while (rec!=NULL )
		{
		CApaFileRecognizerType* prev=NULL;
		prev = rec;
		rec=rec->iNext;
		delete prev;
		}
	iFileRecognizerList=NULL;
	}


EXPORT_C void CApaFileRecognizer::SetAppLocator(CApaAppLocator* aAppLocator)
// takes ownership of the locator
	{
	iAppLocator = aAppLocator;
	}

static TUidType UidTypeL(const TDesC& aFullFileName, const RFs& aFs)
	{	
	_LIT(KSysBin, "\\sys\\bin\\");
	if (TParsePtrC(aFullFileName).Path().CompareF(KSysBin)==0)
		{
		RLoader loader;
		User::LeaveIfError(loader.Connect());
		CleanupClosePushL(loader);
		TPckgBuf<RLibrary::TInfo> dllInfo;
		User::LeaveIfError(loader.GetInfo(aFullFileName, dllInfo));
		CleanupStack::PopAndDestroy(&loader);
		return dllInfo().iUids;
		}
	TEntry entry;
	User::LeaveIfError(aFs.Entry(aFullFileName,entry));
	return entry.iType;
	}

EXPORT_C CApaFileRecognizerType* CApaFileRecognizer::RecognizeFileL(const TDesC& aFullFileName,const TUidType* aUidType)
// Returns the specific recognizer if the file is recogized as a runnable file. or NULL if not.
// TUidType is optional, if not supplied it will be read from the file.
// Leaves, if any files required cannot be found or if OOM.
// Leaves with KErrNotSupported, if file cannot be recognized.
	{
	const TUidType uidType((aUidType!=NULL)? *aUidType: UidTypeL(aFullFileName, iFs));
	CApaFileRecognizerType* rec=iFileRecognizerList;
	CApaFileRecognizerType::TRecognizedType type;
	while (rec!=NULL)
		{
		type=rec->RecognizeFileL(iFs,aFullFileName,uidType);
		if (type==CApaFileRecognizerType::EOtherFile)	
			{
			rec=NULL;						// Recognised but not runnable so stop search now
			break;
			}
		if (type!=CApaFileRecognizerType::ENotRecognized)
			{
 			break;
			}
		rec=rec->iNext;
		}
	if (rec==NULL)
		{
		User::Leave(KErrNotSupported);
		}
	return rec;
	}

EXPORT_C void CApaFileRecognizer::AddFileRecognizerType(CApaFileRecognizerType* aFileRecognizerType)
// Add given RecognizerType to the end of the recognizer list.
// Set's it's iFileRecognizer pointer to "this" - recognizers may call AppDataByUid
	{
	aFileRecognizerType->iNext=NULL;
	aFileRecognizerType->iFileRecognizer=this;
	CApaFileRecognizerType* rec=iFileRecognizerList;
	if (rec==NULL)
		iFileRecognizerList=aFileRecognizerType;
	else
		{
		while (rec->iNext!=NULL)
			rec=rec->iNext;
		rec->iNext=aFileRecognizerType;
		}
	}


EXPORT_C TInt CApaFileRecognizer::RemoveFileRecognizerType(const CApaFileRecognizerType* aFileRecognizerType)
// remove the given recognizer from the list if it is not locked
// return an error code if removal failed
	{
	CApaFileRecognizerType* rec=iFileRecognizerList;
	CApaFileRecognizerType* prev=NULL;
	// find the recognizer in the list
	while (rec!=NULL && rec!=aFileRecognizerType)
		{
		prev = rec;
		rec=rec->iNext;
		}
	// did we find a match
	if (!rec)
		return KErrNotFound;
	// is the matching recognizer locked
	if (rec->Locked())
		return KErrLocked;
	// remove the recognizer from the list, then delete it
	if (prev)
		prev->iNext = rec->iNext;
	else
		iFileRecognizerList = rec->iNext;
	rec->iNext = NULL;
	delete rec;
	return KErrNone;
	}


EXPORT_C CApaAppLocator* CApaFileRecognizer::AppLocator() const
	{
	__ASSERT_ALWAYS(iAppLocator,Panic(EPanicNoAppLocator));
	//
	return iAppLocator;
	}


//
// Class CApaFileRecognizerType
//

EXPORT_C CApaFileRecognizerType::CApaFileRecognizerType():iFileRecognizerExtn(NULL)
	{
	}


EXPORT_C CApaFileRecognizerType::~CApaFileRecognizerType()
	{
#ifdef USING_ECOM_RECOGS
	//if ecom plugin is used destroy its implementation
	if(iFileRecognizerExtn!=NULL)
		{
		REComSession::DestroyedImplementation(iFileRecognizerExtn->DestructorUid());
		delete iFileRecognizerExtn;
		}
#else
	iFileRecognizerExtn = NULL;
#endif
	delete iCapabilityBuf;
	delete iFullFileName;
	iFileRecognizer = NULL;
	iAppStarter = NULL;
	iNext = NULL;
	}

EXPORT_C TThreadId CApaFileRecognizerType::AppRunL(const CApaCommandLine& aCommandLine) const
	{
	__ASSERT_ALWAYS(iAppStarter,Panic(EPanicNoAppStarter));
	//
	return iAppStarter->StartAppL(aCommandLine);
	}

CApaFileRecognizerType::TRecognizedType CApaFileRecognizerType::RecognizeFileL(RFs& aFs,const TDesC& aFullFileName,TUidType aUidType)
	{
	// set the UID's and name
	iFileType = aUidType[1];
	iAppUid = aUidType[2];
	delete iFullFileName;
	iFullFileName = NULL;
	iFullFileName = aFullFileName.AllocL();
	//
	// see if we recognize it
	iRecognizedType = ENotRecognized;
	TRecognizedType type=DoRecognizeFileL(aFs,aUidType);
	if (type==ENotRecognized)
		{
		delete iFullFileName;
		iFullFileName=NULL;
		}
	else
		{
		if(!iCapabilityBuf)
			{
			// Actually, iCapabilityBuf is not needed anymore, but in order not to break BC,
			// we must still support it (in case someone calls CApaFileRecognizerType::Capability).
			iCapabilityBuf = new(ELeave) TApaAppCapabilityBuf;
			iCapabilityBuf->FillZ(iCapabilityBuf->MaxLength());
			}
		}
	return type;
	}


EXPORT_C void CApaFileRecognizerType::Capability(TDes8& aCapabilityBuf)const
	{
	__ASSERT_ALWAYS(iCapabilityBuf,Panic(EPanicCapabilityNotSet)); // capability has been called when no file has been recognized
	//
	TApaAppCapability::CopyCapability(aCapabilityBuf,*iCapabilityBuf);
	}


EXPORT_C void CApaFileRecognizerType::Lock()
	{
	iLock++;
	}


EXPORT_C void CApaFileRecognizerType::Unlock()
	{
	if (iLock>0)
		iLock--;
	}


TBool CApaFileRecognizerType::Locked()const
	{
	return (iLock>0);
	}

EXPORT_C void CApaFileRecognizerType::Reserved_1()
	{}

#ifdef USING_ECOM_RECOGS
// instantiate the ecom implementation class 
EXPORT_C CApaFileRecognizerType* CApaFileRecognizerType::CreateFileRecognizerL(TUid aImplUid)
	{
	CApaFileRecognizerType* fileRecType = NULL;
	TUid tempDtorKey = KNullUid;
	fileRecType = static_cast<CApaFileRecognizerType*>(REComSession::CreateImplementationL(aImplUid, tempDtorKey));
	CleanupStack::PushL(fileRecType);
	fileRecType->iFileRecognizerExtn=CFileRecognizerExtension::NewL(tempDtorKey);
	CleanupStack::Pop(fileRecType);
	return fileRecType;
	}
#else
EXPORT_C CApaFileRecognizerType* CApaFileRecognizerType::CreateFileRecognizerL(TUid)
	{
	return NULL;
	}
#endif

CFileRecognizerExtension::CFileRecognizerExtension(TUid aDtorKey)
 	:iDtorKey(aDtorKey)
 	{
	}

CFileRecognizerExtension* CFileRecognizerExtension::NewL(TUid aDtorKey)
	{
	//instantiate CFileRecognizerExtension with the destructor key of the ecom implentation instance
	CFileRecognizerExtension* self=new(ELeave) CFileRecognizerExtension(aDtorKey);
	return self;
	}

CFileRecognizerExtension::~CFileRecognizerExtension()
 	{
 	}

//returns the destructor key of the ecom implentation instance
TUid CFileRecognizerExtension::DestructorUid()const
 	{
 	return iDtorKey;
 	}

//
// MApaAppStarter
//

/** Constructor for MApaAppStarter. */
EXPORT_C MApaAppStarter::MApaAppStarter()
	{
	}

/** Reserved for future use */
EXPORT_C void MApaAppStarter::MApaAppStarter_Reserved1()
	{
	}
	
/** Reserved for future use */
EXPORT_C void MApaAppStarter::MApaAppStarter_Reserved2()
	{
	}
