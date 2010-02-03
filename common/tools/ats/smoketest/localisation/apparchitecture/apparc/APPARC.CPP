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

#include <apparc.h> // stuff everyone will want ie most things
#include <apacln.h> // CleanupStack protection for CApaDocument
#include "APADLL.H" // CApaDll CApaExe CApaAppHolder
#include "APASTD.H" // Panics etc.
#include <e32uid.h> // KExecutableImageUid

#include <s32stor.h>
#include <s32file.h>
#include <s32std.h>

#ifdef USING_ECOM_RECOGS
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#endif

#include "../apparc/TRACE.H"

const TInt KAppProcessArrayGranularity(1);

_LIT(KApplicationLocation,"\\sys\\bin\\");
  
/////////////////////////////
// Doc cleanup method
/////////////////////////////

EXPORT_C void TApaDocCleanupItem::DoCleanup(TAny* aPtr)
	{
	__ASSERT_ALWAYS(aPtr,Panic(EPanicNoCleanupItem));
	TApaDocCleanupItem* cleanup = reinterpret_cast<TApaDocCleanupItem*>(aPtr);
	__ASSERT_ALWAYS(cleanup->iApaProcess,Panic(EPanicNoCleanupItem));//lint !e613 Possible use of null pointer - Asserted above
	cleanup->iApaProcess->DestroyDocument(cleanup->iApaDoc); //lint !e613 Possible use of null pointer - Asserted above
	}


/////////////////////////////
// CApaAppHolder
/////////////////////////////

CApaAppHolder::CApaAppHolder()
	{}


CApaAppHolder::~CApaAppHolder()
	{
	}

void CApaAppHolder::UpdateAppsRefToThis()
	{
	CApaApplication* app = Application();
	__ASSERT_ALWAYS(app,Panic(EPanicNoApplication));
	app->iAppHolder = this;	//lint !e613 Possible use of null pointer - Asserted above
	}

#ifdef USING_ECOM_RECOGS
/////////////////////////////
// CApaExe
/////////////////////////////

CApaExe::CApaExe()
	{}

CApaExe::~CApaExe()
	{
	delete iAppName;
	delete iApplication;
	}

TFileName CApaExe::FileName()const
	{
	if (iAppName)
		{
		return *iAppName;
		}
	else
		{
		return KNullDesC();
		}
	}

TUid CApaExe::Uid()const
	{
	return iFileUid;
	}

CApaApplication* CApaExe::Application() const
	{
	return iApplication;
	}

void CApaExe::CreateApplicationL(TApaApplicationFactory aApplicationFactory)
	{
	__ASSERT_ALWAYS(!iApplication,Panic(EPanicApplicationAlreadyExists));
	iApplication = aApplicationFactory.CreateApplicationL();
	iFileUid = aApplicationFactory.AppFileUid();
	User::LeaveIfNull(iApplication);
	iAppName = aApplicationFactory.AppFileNameL();
	UpdateAppsRefToThis();
	}

#endif // USING_ECOM_RECOGS

EXPORT_C void CApaDocument::OpenFileL(CFileStore*&, RFile&)
	{
	}

EXPORT_C void CApaDocument::Reserved_2()
	{}


/////////////////////
// CApaApplication
/////////////////////

/** Constructor for CApaApplication */
EXPORT_C CApaApplication::CApaApplication()
	{
	}

EXPORT_C TFileName CApaApplication::AppFullName()const
/** Returns the full name and path of the application.

The default implementation returns the full path name of the application DLL.

An application can provide its own implementation. 

@return Full path name of the application.
@see CApaApplication::DllName() */
	{
	return DllName();
	}


EXPORT_C TFileName CApaApplication::DllName()const
/** Returns the full name and path of the loaded application DLL.

@return Full path name of the application DLL. */
	{
	__ASSERT_DEBUG(iAppHolder, Panic(EDPanicNoAppHolder));
	return iAppHolder->FileName();
	}


EXPORT_C TInt CApaApplication::GenerateFileName(RFs& aFs,TFileName& aRootName)
/** Generates a unique filename based on the file name contained within the specified 
full path name.

If necessary, the function creates the directory structure that is defined 
in the specified full path name.

If the file name does not yet exist, then this is the file name chosen. If 
this file name already exists, then a file name of the form: name(nn) is generated, 
where nn are decimal digits. The value of nn is incremented until a name is 
generated that is unique within the directory structure. A minimum of two 
decimal digits is generated.

The function is used by the UI framework.

@param aFs Handle to a file server session. 
@param aRootName The full path name.
@return KErrNone if successful, otherwise one of the other system-wide error 
codes. Specifically: KErrBadName if the file name portion of the specified 
full path name has invalid format; KErrArgument if the drive, path or file 
name parts are missing from the specified full path name; KErrOverflow if 
the generated filename becomes too long; KErrNoMemory if there is insufficient 
memory to perform the operation.
@see CEikAppUi */
	{
	// check that filename is valid
	if (!aFs.IsValidName(aRootName))
		return KErrBadName;
	//
	// check that a drive, path and root filename have been specified
	TParsePtr parsePtr(aRootName);
	if (!parsePtr.DrivePresent() || !parsePtr.PathPresent() || !parsePtr.NamePresent())
		return KErrArgument;
	//
	// create the path if necessary
	TInt ret=aFs.MkDirAll(parsePtr.DriveAndPath());
	if (ret!=KErrNone && ret!=KErrAlreadyExists)
		return ret;
	//
	// Create the Rbuf object to hold a filename (return if no mem available)
	RBuf newName;
	ret = newName.Create(aRootName, KMaxFileName+8);
	if (ret!=KErrNone)
		return KErrNoMemory;
	//	
	// generate a valid filename that doesn't already exist...
	TEntry entry;
	TInt i=1;
	_LIT(KFormatStringOne,"%S%S(%02d)%S");
	TBuf<16> format;
	format=KFormatStringOne;
	while (aFs.Entry(newName,entry)==KErrNone)		// Continue until DoesNotExist or PathDoesNotExist, etc
		{
		if (i>=100)
			{
			_LIT(KFormatStringTwo,"%S%S(%d)%S");
			format=KFormatStringTwo;
			}
		TPtrC driveAndPath=parsePtr.DriveAndPath();
		TPtrC name=parsePtr.Name();
		TPtrC ext=parsePtr.Ext();
		newName.Format(format,&driveAndPath,&name,i++,&ext);
		if (newName.Length()>KMaxFileName)
			{
			newName.Close();
			return KErrOverflow;
			}
		}
	//
	// set the new filename and return
	aRootName = newName;
	newName.Close();
	return KErrNone;
	}


EXPORT_C CDictionaryStore* CApaApplication::OpenIniFileL(RFs& aFs)const
/** Opens the .ini file associated with the application, constructs the dictionary 
store object and returns a pointer to it.

The implementation of this function is provided by the OpenIniFileLC() function. 
The function pops the pointer returned by OpenIniFileLC() from the cleanup 
stack.

@param aFs Handle to a file server session. 
@return A pointer to the dictionary store object representing the application's 
.ini file. 
@see CApaApplication::OpenIniFileLC() */
	{
	CDictionaryStore* store=OpenIniFileLC(aFs);
	CleanupStack::Pop(); // store
	return store;
	}

EXPORT_C CApaApplication::~CApaApplication()
	{
#ifdef USING_ECOM_RECOGS
	if (iDtorKey!=TUid::Null()) // only some CApaApplication objects are ECom objects (i.e. only those corresponding to embedded applications, not top-level applications)
		{
		REComSession::DestroyedImplementation(iDtorKey);
		}
#endif // USING_ECOM_RECOGS
	iAppHolder = NULL;
	}

EXPORT_C void CApaApplication::NewAppServerL(CApaAppServer*& /*aAppServer*/)
/** Virtual function called by the framework when the application
has been launched as a server application.
Applications that wish to be used as server applications must
override this function to return their implemetation of the server.
@param aAppServer The server pointer to be set. */
	{
	User::Leave(KErrNotSupported);
	}

/** Reserved for future use */
EXPORT_C void CApaApplication::CApaApplication_Reserved1()
	{
	}

/** Reserved for future use */
EXPORT_C void CApaApplication::CApaApplication_Reserved2()
	{
	}

/////////////////////////////
// CApaDocument
/////////////////////////////

/** Constructor for CApaDocument */
EXPORT_C CApaDocument::CApaDocument()
	{
	}

EXPORT_C CApaDocument::CApaDocument(CApaApplication& aApp,CApaProcess& aProcess)
	: iApplication(&aApp),
	iApaProcess(&aProcess)
/** Constructs the document object with the specified application and process.

Derived classes must define and implement a constructor through which both 
the associated application and process can be specified. A typical implementation 
calls this constructor through a constructor initialization list.

@param aApp The application.
@param aProcess The process.
@see CEikDocument */
	{}


EXPORT_C CApaDocument::~CApaDocument()
/** Destructor.

The implementation is empty. */
	{
	iContainer = NULL;
	iApplication = NULL;
	iApaProcess = NULL;
	}


EXPORT_C CApaDocument::TCapability CApaDocument::Capability() const
/** Gets the document's capabilities.

Capabilities are encapsulated by an instance of a TCapability class, a public 
class defined inside this class.

The default implementation returns a default TCapability object, indicating 
that the document does not support any of the defined capabilities.

If a document does support one or more of the capabilities, it should override 
this function to return a suitably initialised object.

@return The document's capabilities */
	{
	return TCapability();
	}


EXPORT_C void CApaDocument::ValidatePasswordL() const
/** Checks the document password.

The default implementation is empty.

If a document is intended to be password protected, the UI application should 
provide an implementation that forces the user to enter the password and validate 
the input.

If the document is protected by a password and the password entered by the 
user is incorrect, the function should leave with KErrLocked, otherwise it 
should just return. */
	{}


EXPORT_C CPicture* CApaDocument::GlassPictureL()
// Return handle to glass picture, creating one if not already created.
// returns NULL as glass pictures are not supported by default
/** Gets an object that can draw a representation of the document's content.

If the document supports being embedded as a glass door, then the UI application 
must provide an implementation for this function.

The default implementation raises an APPARC 8 panic.

@return A pointer to a glass door. */
	{
	Panic(EPanicNoGlassDoorMethodSupplied);
	//
	return NULL;
	}


EXPORT_C void CApaDocument::ExternalizeL(RWriteStream& /*aStream*/)const
	{}

EXPORT_C CApaDocument::TCapability::TCapability()
	:iCapability(0),TCapability_Reserved1(0)
/** Constructs a default capability object.

All capabilities are marked as "not supported". */
	{}

/////////////////////////////
// TApaAppHolderInfo
/////////////////////////////

class TApaAppHolderInfo
	{
public:
	TApaAppHolderInfo(CApaAppHolder* aAppHolder);
public:
	CApaAppHolder* iAppHolder;
	TBool iToBeRemoved;
	};

TApaAppHolderInfo::TApaAppHolderInfo(CApaAppHolder* aAppHolder)
	:iAppHolder(aAppHolder), iToBeRemoved(EFalse)
	{
	}

/////////////////////////////
// CApaParentProcessMonitor
/////////////////////////////

class CApaParentProcessMonitor : public CActive
	{
public: // Construction / destruction
	static CApaParentProcessMonitor* NewL(TProcessId aProcessId);
	~CApaParentProcessMonitor();
	void ConstructL();
private:
	CApaParentProcessMonitor(TProcessId aProcessId);
public: // From CActive
	void RunL();
	void DoCancel();
private:
	TProcessId iProcessId;
	RProcess iProcess;
	};

CApaParentProcessMonitor* CApaParentProcessMonitor::NewL(TProcessId aProcessId)
	{
	CApaParentProcessMonitor* self=new (ELeave) CApaParentProcessMonitor(aProcessId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CApaParentProcessMonitor::CApaParentProcessMonitor(TProcessId aProcessId)
	: CActive(EPriorityLow)
	{
	iProcessId=aProcessId;
	}

CApaParentProcessMonitor::~CApaParentProcessMonitor()
	{
	Cancel();
	}

void CApaParentProcessMonitor::ConstructL()
	{
	User::LeaveIfError(iProcess.Open(iProcessId));
	iProcess.Logon(iStatus);
	if(iStatus==KErrNoMemory)
		{
		User::WaitForRequest(iStatus);
		User::Leave(KErrNoMemory);
		}
	CActiveScheduler::Add(this);
	SetActive();
	}

void CApaParentProcessMonitor::RunL()
	{
	// Do something that will kill the child when the parent process terminates
	if(iStatus==KErrNone)
		{
		RProcess proc;
		proc.Terminate(KErrNone);
		}
	}

void CApaParentProcessMonitor::DoCancel()
	{
	iProcess.LogonCancel(iStatus);
	}

/////////////////////////////
// CApaProcess
/////////////////////////////

/** Constructor for CApaProcess */
EXPORT_C CApaProcess::CApaProcess()
	{
	}

EXPORT_C CApaProcess* CApaProcess::NewL(const RFs& aFs)
/** Creates and returns a pointer to a new application process.

This function is not used by UI applications.

@param aFs Handle to a file server session.
@return Pointer to the new application process. */
	{
	CApaProcess* self=new(ELeave) CApaProcess(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CApaProcess::CApaProcess(const RFs& aFs)
	:iFsSession(aFs)
/** Constructs the application process object with the specified file session handle.

Derived classes must define and implement a constructor through which 
the file session handle can be specified. A 
typical implementation calls this constructor through a constructor initialization 
list.

This constructor is used by the UI framework.

@deprecated
@param aFs Handle to a file server session */
	{}

const TInt KPriorityGreaterThanShutter=102;

EXPORT_C void CApaProcess::ConstructL()
/** Completes construction of the application process object.

Implementers of derived classes must call this function as part of the second 
stage construction of an object. Typically, derived classes implement their 
own NewL() function and call ConstructL() as part of that implementation. */
	{
	//
	iAppList = new(ELeave) CArrayFixFlat<TApaAppHolderInfo>(KAppProcessArrayGranularity);
	iDocList = new(ELeave) CArrayFixFlat<CApaDocument*>(KAppProcessArrayGranularity);
	iMainDocFileName = HBufC::NewL(KMaxFileName);
	iApplicationRemover=CIdle::NewL(KPriorityGreaterThanShutter);	// Use an idle object so that app has chance to clear its call stack
	}

EXPORT_C void CApaProcess::ConstructL(TProcessId aParentProcessId)
/** Completes construction of the application process object, passing in a Parent Process Identifier.

Implementers of derived classes must call this function as part of the second 
stage construction of an object. Typically, derived classes implement their 
own NewL() function and call ConstructL() as part of that implementation.

@param aParentProcessId Id of the parent process. This process will terminate when the parent does. */
	{
	ConstructL();
	if(KNullProcessId!=aParentProcessId)
		{
		iMonitor=CApaParentProcessMonitor::NewL(aParentProcessId);
		}
	}

EXPORT_C CApaProcess::~CApaProcess()
// If this is called without calling ResetL() or CApaDocument::SaveL() first, data may be lost
//
/** Frees resources prior to destruction.

Documents must be saved before the application process is deleted, otherwise 
data may be lost.

In debug mode, the destructor raises an APPARC 6 panic if documents still 
exist, and an APPARC 5 panic if applications still exist. */
	{
	if (iMainDoc)
		{
		DestroyDocument(iMainDoc);
		iMainDoc = NULL;
		}
	if (iDocList)
		{
		__ASSERT_DEBUG(iDocList->Count()==0,Panic(EPanicDocListNotEmpty));
		for (TInt i=iDocList->Count()-1 ; i>=0 ; i--)
			{
			delete (*iDocList)[i]; // delete stray doc's in release mode, just to be tidy
			}
		}
	if (iAppList)
		{
		for (TInt i=iAppList->Count()-1 ; i>=0 ; i--)
			{
			delete ((*iAppList)[i]).iAppHolder;
			}
		}
	delete iAppList;
	delete iDocList;
	delete iMainDocFileName;
	delete iApplicationRemover;
	delete iMonitor;
	}


EXPORT_C void CApaProcess::ResetL()
/** Resets the the application process to its initial state.

Specifically, it saves the main document, deletes the main and all embedded 
documents from memory, resets the main document filename and deletes all applications 
except the main application.

The function can leave if saving the main document fails. */
	{
	if (iMainDoc)
		{
		iMainDoc->SaveL();
		DeleteAllDocs(); // sets iMainDoc to NULL, deletes all apps except main
		}
	__ASSERT_DEBUG(iMainDocFileName, Panic(EPanicNoDocument));
	*iMainDocFileName=KNullDesC;
	}


void CApaProcess::DeleteAllDocs()
// deletes all docs
// deletes all apps except main app
// sets iMainDoc* to NULL
//
	{
	CApaAppHolder* mainAppHolder=NULL;
	if (iMainDoc)
		{
		__ASSERT_DEBUG(iMainDoc->Application(), Panic(EDPanicNoApp));
		mainAppHolder = iMainDoc->Application()->iAppHolder;
		for (TInt i=iDocList->Count()-1 ; i>=0 ; i--)
			if ((*iDocList)[i]==iMainDoc)
				{
				iDocList->Delete(i); // removes from array, but doesnt destroy
				delete iMainDoc;
				iMainDoc = NULL;
				}
		}
	__ASSERT_ALWAYS(iDocList->Count()==0,Panic(EPanicDocListNotEmpty));
	iDocList->Reset();
	if (iAppList)
		{
		for (TInt ii=iAppList->Count()-1 ; ii>=0 ; ii--) // need to iterate backwards as the array changes size during the loop
			{
			if ((*iAppList)[ii].iAppHolder!=mainAppHolder)
				{
				delete (*iAppList)[ii].iAppHolder;
				iAppList->Delete(ii);
				}
			}
		iAppList->Compress();
		}
	}


EXPORT_C void CApaProcess::SetMainDocFileName(const TDesC& aMainDocFileName)
/** Sets the filename of the main document.

@param aMainDocFileName The filename to be set.
@panic APPARC 7 If the length of aMainDocFileName is greater than KMaxFileName or the
length of the last filename set by SetMainDocFileNameL if greater
@see KMaxFileName */
	{
	__ASSERT_DEBUG( iMainDocFileName, Panic(EPanicNullPointer));
	__ASSERT_ALWAYS( aMainDocFileName.Length()<=iMainDocFileName->Des().MaxLength() ,Panic(EPanicFileNameTooLong));
	*iMainDocFileName = aMainDocFileName;
	}

EXPORT_C void CApaProcess::SetMainDocFileNameL(const TDesC& aMainDocFileName)
/** Sets the filename of the main document.

@param aMainDocFileName The filename to be set. There is no restriction on the
length of this descriptor. */
	{
	__ASSERT_ALWAYS( iMainDocFileName, Panic(EPanicNullPointer));
	const TInt newLength = aMainDocFileName.Length() < KMaxFileName ? KMaxFileName : aMainDocFileName.Length();
	if (newLength != iMainDocFileName->Des().MaxLength())
		{
		HBufC* const newMainDocFileName = HBufC::NewL(newLength);
		delete iMainDocFileName;
		iMainDocFileName = newMainDocFileName;
		}
	SetMainDocFileName(aMainDocFileName);
	}

EXPORT_C void CApaProcess::SetMainDocument(CApaDocument* aDocument)
/** Sets the main document.

@param aDocument A pointer to the document to be set as the main document 
of the application process. This must be a an object created by the AddNewDocumentL() 
or OpenNewDocumentL() functions 
@see CApaProcess::AddNewDocumentL()
@see CApaProcess::OpenNewDocumentL() */
	{
	__ASSERT_ALWAYS( iDocList, Panic(EPanicNullPointer));
	// check that the prospective main doc has actually been added to the array
	for (TInt i=iDocList->Count()-1 ; i>=0 ; i--)
		{
		if ((*iDocList)[i]==aDocument)
			break;
		if (i==0)
			Panic(EPanicNoDocument);
		}
	// assign it once it has checked out
	iMainDoc = aDocument;
	}

EXPORT_C CApaDocument* CApaProcess::AddNewDocumentL(TApaApplicationFactory aApplicationFactory)
/** Creates and adds a new document using the specified application factory.

The document may be a main document or an embedded document.

Any document created with this function must be destroyed using DestroyDocument().

@param aApplicationFactory Should be created implicitly by passing a pointer to
a factory function, an ECOM plugin UID, or a CImplementationInformation reference.
@return A pointer to the new document.
@see CApaProcess::DestroyDocument()
@see CApaApplication */
	{
#ifdef USING_ECOM_RECOGS
	__SHOW_TRACE(_L("Starting CApaProcess::AddNewDocumentL"));
	__APA_PROFILE_START(0);

	CApaAppHolder* appHolder = AddAppExeL(aApplicationFactory);	

	// use the app to create a doc
	CApaDocument* doc=NULL;
	TRAPD(ret,doc=CreateDocL(appHolder->Application()));
	if (ret!=KErrNone)
		// remove app as it has been orphaned
		RemoveApp(appHolder);
	User::LeaveIfError(ret);
	__PROFILE_END(0);
	return doc;
#else // USING_ECOM_RECOGS
	(void)aApplicationFactory;
	return NULL;
#endif // USING_ECOM_RECOGS
	} //lint !e1762 Member function could be made const - Not true



void CApaProcess::RemoveApp(CApaAppHolder* aAppHolder)
// removes app holder from the list if it exists, panics otherwise
	{
	__ASSERT_ALWAYS(iAppList, Panic(EPanicNullPointer));
	TInt i = 0;
	for (i=iAppList->Count()-1 ; i>=0 ; i--)
		{
		if ((*iAppList)[i].iAppHolder==aAppHolder) // the main app may be alive on its own if Reset() has just been called
			{
			delete aAppHolder; // the main app may be alive on its own if Reset() has just been called
			iAppList->Delete(i);
			break;
			}
		}
	if (i<0)
		Panic(EPanicAppNotInList);
	}


EXPORT_C CApaDocument* CApaProcess::OpenNewDocumentL(CFileStore*& aStore,CStreamDictionary*& aStreamDic,const TDesC& aDocFullFileName,TUint aFileMode)
/** Opens the specified file and restores the content as a document.

The created document can be merged into or embedded in another document.

Any document created with this function must be destroyed using DestroyDocument().

@param aStore On return, this contains a pointer to the store object created 
during the restore.
@param aStreamDic On return, this contains a pointer to the stream dictionary 
object created during the restore. 
@param aDocFullFileName The name of the file containing the document. 
@param aFileMode The mode in which to open the file. 
@return A pointer to the restored document.
@see TFileMode
@see CApaProcess::DestroyDocument() */
	{
	__SHOW_TRACE(_L("Starting CApaProcess::OpenNewDocumentL"));
	__APA_PROFILE_START(1);
	TParse parser;
	User::LeaveIfError(iFsSession.Parse(aDocFullFileName,parser)); 
	// open doc as a file store & read in the header
	CFileStore* docStore;
	CStreamDictionary* streamDic = ReadRootStreamLC(FsSession(),docStore,parser.FullName(),aFileMode);
	CleanupStack::PushL(docStore);
	// read in the app id info
	TApaAppIdentifier appId=ReadAppIdentifierL(*docStore,*streamDic);
	// create the doc
	CApaDocument* importedDoc =	AddNewDocumentL(appId.iAppUid);
	// restore the document
	TApaDocCleanupItem cleanup(this,importedDoc);
	CleanupStack::PushL(cleanup);
	importedDoc->RestoreL(*docStore,*streamDic);
	CleanupStack::Pop(3); //docStore,importedDoc,streamDic
	aStore = docStore;
	aStreamDic = streamDic;
	__PROFILE_END(1);
	return importedDoc;
	}

EXPORT_C TApaAppIdentifier CApaProcess::ReadAppIdentifierL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic)
// this is a static method
/** Reads the application identifier from its stream in the specified store and 
returns it.

The location of the stream is found in the specified stream dictionary.

@param aStore The store from which the application identifier should be read. 
@param aStreamDic The stream dictionary containing the stream ID of the application 
identifier stream. The stream dictionary can be found in the root stream of 
the store.
@return The application identifier. */
	{
	__SHOW_TRACE(_L("Starting CApaProcess::ReadAppIdentifierL"));
	TStreamId infoStreamId=aStreamDic.At(KUidAppIdentifierStream);
	TApaAppIdentifier appId;
	// create a stream and read in the data
	RStoreReadStream stream;
	stream.OpenLC(aStore,infoStreamId);
	stream>> appId;
	stream.Close();
	CleanupStack::PopAndDestroy(); // stream
	return appId;	
	}


EXPORT_C void CApaProcess::WriteAppIdentifierL(CStreamStore& aStore,CStreamDictionary& aStreamDic,const TApaAppIdentifier& aAppId)
// this is a static method
/** Writes the application identifier to a new stream in the specified store and 
records the location of this stream in the specified stream dictionary.

@param aStore The store to which the application identifier should be written. 

@param aStreamDic The stream dictionary. 
@param aAppId The application identifier to be externalised to a stream. */
	{
	__SHOW_TRACE(_L("Starting CApaProcess::WriteAppIdentifierL"));
	// create a stream
	RStoreWriteStream stream;
	TStreamId streamId=stream.CreateLC(aStore);
	// stream the header
	stream<< aAppId;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // id stream
	// enter the stream in the dictionary
	aStreamDic.AssignL(KUidAppIdentifierStream,streamId);
	}


EXPORT_C CStreamDictionary* CApaProcess::ReadRootStreamLC(RFs& aFs,CFileStore*& aStore,const TDesC& aDocFullFileName,TUint aFileMode)
/** Reads the stream dictionary contained as the root stream in the specified document 
file.

The function constructs, and returns a pointer to the stream dictionary object 
and puts the pointer to the stream dictionary object onto the cleanup stack. 
It also returns a pointer to the created file store object through an argument 
reference. 

The file must be a valid document file; otherwise the function leaves with one of 
the system-wide error codes.

@param aFs Handle to a file server session.
@param aStore On return, a pointer to the newly created file store object. 
@param aDocFullFileName The full path name of the document file. 
@param aFileMode The mode in which to open the file.
@return A pointer to the stream dictionary object read from the root stream 
of the store. 
@see TFileMode */
	{ // static
	__SHOW_TRACE(_L("Starting CApaProcess::ReadRootStreamLC (file-name overload)"));
	CStreamDictionary* const streamDictionary=CStreamDictionary::NewLC();
	CFileStore* const store=CFileStore::OpenLC(aFs,aDocFullFileName,aFileMode);
	DoReadRootStreamL(*streamDictionary, *store);
	aStore=store; // delay assignment until nothing can go wrong to avoid destroying the store twice if a leave occurs
	CleanupStack::Pop(store);
	return streamDictionary;
	}


EXPORT_C CStreamDictionary* CApaProcess::ReadRootStreamLC(CFileStore*& aStore, const RFile& aFile)
/**
@internalTechnology
*/
	{ // static
	__SHOW_TRACE(_L("Starting CApaProcess::ReadRootStreamLC (file-handle overload)"));
	CStreamDictionary* const streamDictionary=CStreamDictionary::NewLC();
	RFile duplicateFile;
	CleanupClosePushL(duplicateFile);
	User::LeaveIfError(duplicateFile.Duplicate(aFile)); // this is because CFileStore::FromLC closes the file its passed (and stores its own duplicate)
	CFileStore* const store=CFileStore::FromL(duplicateFile);
	CleanupStack::PopAndDestroy(&duplicateFile);
	CleanupStack::PushL(store);
	DoReadRootStreamL(*streamDictionary, *store);
	aStore=store; // delay assignment until nothing can go wrong to avoid destroying the store twice if a leave occurs
	CleanupStack::Pop(store);
	return streamDictionary;
	}


void CApaProcess::DoReadRootStreamL(CStreamDictionary& aStreamDictionary, const CFileStore& aStore)
	{ // static
	const TStreamId rootStreamId=aStore.Root();
	if ((aStore.Type()[1]!=KUidAppDllDoc) || (rootStreamId==KNullStreamId))
		{
		User::Leave(KErrCorrupt);
		}
	RStoreReadStream rootStream;
	rootStream.OpenLC(aStore, rootStreamId);
	rootStream>>aStreamDictionary;
	CleanupStack::PopAndDestroy(&rootStream);
	}


EXPORT_C void CApaProcess::WriteRootStreamL(CPersistentStore& aStore,CStreamDictionary& aStreamDic,const CApaApplication& aApp)
// this is a static method
/** Writes the application identifier (derived from the application object CApaApplication) 
followed by the stream dictionary to the store and makes the stream dictionary the root stream of the
store.

Typically, the function is called by the application when it 
implements a file create or file save type operation. It is called after all 
model and UI data has been persisted. The IDs of the streams containing the 
model and UI data should have been lodged in the stream dictionary.

In effect, the function finishes off the file save or file
create type operation, leaving the file containing the store in a valid state
with the standard interface.

@param aStore  The store to which the root stream is to be written. Before
calling this function, a reference to the store must be saved by putting a
pointer onto the cleanup stack or by making it member data of a class. This
ensures that it is not orphaned in the event of this function leaving.
@param aStreamDic The stream dictionary containing the locations and associated 
UIDs of other streams in the store.
@param aApp  The application used to create the main document in the file
being written. The application identifier to be written is constructed from
this application object. */
	{
	__SHOW_TRACE(_L("Starting CApaProcess::WriteRootStreamL(app)"));
	// get the app dll name
	TParse dllPath;
	dllPath.SetNoWild(aApp.DllName(),NULL,NULL);
	// set up an app identifier
	TApaAppIdentifier appId(aApp.AppDllUid(),dllPath.NameAndExt());
	// Write the root stream
	WriteRootStreamL(aStore,aStreamDic,appId);
	}


EXPORT_C void CApaProcess::WriteRootStreamL(CPersistentStore& aStore,CStreamDictionary& aStreamDic,const TApaAppIdentifier& aAppId)
// this is a static method
/** Writes the application identifier followed by the stream dictionary 
to the store and makes the stream dictionary the root stream of the store.

Typically, the function is called by the application when it 
implements a file create or file save type operation. It is called after all 
model and UI data has been persisted. The IDs of the streams containing the 
model and UI data should have been lodged in the stream dictionary.

In effect, the function finishes off the file save or file
create type operation, leaving the file containing the store in a valid state
with the standard interface.

@param aStore  The store to which the root stream is to be written. Before
calling this function, a reference to the store must be saved by putting a
pointer onto the cleanup stack or by making it member data of a class. This
ensures that it is not orphaned in the event of this function leaving.
@param aStreamDic The stream dictionary containing the locations and associated 
UIDs of other streams in the store.
@param aAppId  The application identifier to be written into the application
identifier stream. */
	{
	__SHOW_TRACE(_L("Starting CApaProcess::WriteRootStreamL(id)"));
	// create a stream
	WriteAppIdentifierL(aStore,aStreamDic,aAppId);
	// externalize the dictionary
	RStoreWriteStream stream;
	TStreamId streamId=stream.CreateLC(aStore);
	stream<< aStreamDic;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // dictionary stream
	// set the dictionary stream as the root stream
	aStore.SetRootL(streamId);
	}


EXPORT_C void CApaProcess::DestroyDocument(CApaDocument* aDoc)
/** Destroys the specified document.

All references to the document are removed, and associated resources are freed. 
Specifically, the function deletes any associated application and unloads 
the application DLL, provided that no other documents of that application 
type are still open.

All document objects created through CApaProcess must be deleted using this 
function.

@param aDoc A pointer to the document to be destroyed. 
@see CApaApplication
@see CApaProcess */
	{
	__SHOW_TRACE(_L("Starting CApaProcess::DestroyDocument(app)"));
	//
	if (aDoc)
		{
		// delete the doc, keeping a handle to its app
		CApaApplication* app=aDoc->Application();
		__ASSERT_DEBUG(app!=NULL,Panic(EDPanicDocWithNoApp));
		// remove the doc from the list, keeping a handle to the doc
		TBool appStillRequired=EFalse;
		__ASSERT_ALWAYS(iDocList, Panic(EPanicNullPointer));
		for (TInt i=iDocList->Count()-1 ; i>=0 ; i--)
			{//check through the list, remove the right doc, and see if the app is used by any other docs
			if ((*iDocList)[i]==aDoc)
				{
				iDocList->Delete(i); // removes from array, but doesnt destroy
				iDocList->Compress();
				}
			else if ((*iDocList)[i]->Application()==app)
				appStillRequired = ETrue;
			}
		// null the main doc handle if we delete the main doc
		if (aDoc==iMainDoc)
			iMainDoc = NULL;
		
		delete aDoc; // del
		
		// remove app if no other doc's use it and it's not the main app
		if ((!appStillRequired)&&(iMainDoc)&&(app!=iMainDoc->Application()))
			{
			MarkApplicationForRemoval(app);
			__ASSERT_DEBUG(iApplicationRemover, Panic(EDPanicNoAppRemover));
			if (!iApplicationRemover->IsActive())
				{
				iApplicationRemover->Start(TCallBack(CApaProcess::IdleRemoveApplications,this));
				}
			}
		}
	}


TInt CApaProcess::IdleRemoveApplications(TAny* aThis)
// Remove applications on callback of idle object. Using an idle object gives an embedded application a chance to clear
// its call stack before its dll is closed
//
	{
	CApaProcess* process=reinterpret_cast<CApaProcess*>(aThis);
	__ASSERT_DEBUG(process, Panic(EDPanicNoProcess));
	process->RemoveMarkedApplications();
	return 0;
	}


void CApaProcess::RemoveMarkedApplications()
// Remove any applications that have been marked for removal, closing their dlls also
//
	{
	__ASSERT_DEBUG(iAppList, Panic(EPanicNullPointer));
	for (TInt i=iAppList->Count()-1 ; i>=0 ; i--)
		if ((*iAppList)[i].iToBeRemoved)
			{
			delete (*iAppList)[i].iAppHolder;
			iAppList->Delete(i); // remove from array
			iAppList->Compress();
			}
	}


void CApaProcess::MarkApplicationForRemoval(const CApaApplication* aApp)
// Mark the application in the app list for removal by idle object
//
	{
	__ASSERT_DEBUG(aApp!=NULL,Panic(EDPanicRemovingNullApp));
	__ASSERT_DEBUG(iAppList, Panic(EPanicNullPointer));
	// remove the app from the list, keeping a handle to it
	for (TInt i=iAppList->Count()-1 ; i>=0 ; i--)
		{
		__ASSERT_DEBUG((*iAppList)[i].iAppHolder, Panic(EDPanicNoAppHolder));
		if ((*iAppList)[i].iAppHolder->Application()==aApp)
			{
			(*iAppList)[i].iToBeRemoved=ETrue;
			}
	}

	}


CApaDocument* CApaProcess::CreateDocL(CApaApplication* aApp)
// creates a new doc with aApp and adds it to the list before returning a handle to it
//
	{
	__SHOW_TRACE(_L("Starting CApaProcess::CreateDocL"));
	__ASSERT_DEBUG(aApp,Panic(EDPanicNoApp));
	//
	// create a new doc with the app
	CApaDocument* doc=aApp->CreateDocumentL(this); //lint !e613 Possible use of null pointer - Asserted above
	__ASSERT_ALWAYS(doc!=NULL,Panic(EPanicDocumentNotCreated));
	// add the doc to the list
	CleanupStack::PushL(doc);
	iDocList->AppendL(doc);
	CleanupStack::Pop(); // doc
	// return a	handle to the doc
	return doc;
	}


CApaAppHolder* CApaProcess::FindAppInListL(const TDesC& aAppFileName,TUid aUid)const
// returns pointer to a matching app, or NULL if not in list
//
	{
	__ASSERT_DEBUG(iAppList, Panic(EPanicNullPointer));
	TInt index=iAppList->Count();
	if (aUid!=KNullUid)
		{// search by UID
		while (--index>=0)
			{
			__ASSERT_DEBUG((*iAppList)[index].iAppHolder, Panic(EDPanicNoAppHolder));
			if ((*iAppList)[index].iAppHolder->Uid()==aUid)
				{
				(*iAppList)[index].iToBeRemoved = FALSE;
				return (*iAppList)[index].iAppHolder; // match found
				}
			}
		}
	else
		{// search by name as no UID has been supplied
		TParse app; TParse suspect;
		User::LeaveIfError(app.Set(aAppFileName,NULL,NULL));
		while (--index>=0)
			{
			__ASSERT_DEBUG((*iAppList)[index].iAppHolder, Panic(EDPanicNoAppHolder));
			suspect.SetNoWild((*iAppList)[index].iAppHolder->FileName(),NULL,NULL);
			if (!app.Name().CompareF(suspect.Name()))
				{
				(*iAppList)[index].iToBeRemoved = FALSE;
				return (*iAppList)[index].iAppHolder; // match found
				}
			}
		}
	return NULL; // no match found
	}

#ifdef USING_ECOM_RECOGS
CApaExe* CApaProcess::AddAppExeL(TApaApplicationFactory aApplicationFactory)
	{
	CApaExe* exe = new(ELeave) CApaExe();
	CleanupStack::PushL(exe);
	
	// create the app
	exe->CreateApplicationL(aApplicationFactory);
	__ASSERT_DEBUG(exe->Application(), Panic(EPanicNullPointer));
	exe->Application()->PreDocConstructL();
	
	// add the application to the list and return a pointer to it
	TApaAppHolderInfo info(exe);
	__ASSERT_DEBUG(iAppList, Panic(EPanicNullPointer));
	iAppList->AppendL(info);
	CleanupStack::Pop(exe);

	return exe;
	}
#endif // USING_ECOM_RECOGS


EXPORT_C TPtrC CApaProcess::MainDocFileName()const
/** Returns the filename of the main document.

@return A non-modifiable pointer descriptor to the main document filename. 
For non file-based applications, the length of this descriptor is zero. */
	{
	if (iMainDocFileName!=NULL)
		{
		return *iMainDocFileName;
		}
	return KNullDesC();
	}

/** Reserved for future use */
EXPORT_C void CApaProcess::CApaProcess_Reserved1()
	{
	}

/** Reserved for future use */
EXPORT_C void CApaProcess::CApaProcess_Reserved2()
	{
	}

/////////////////////////////
// TApaApplicationFactory
/////////////////////////////

/**
Default constructor
*/

/** Constructor for TApaApplicationFactory */
EXPORT_C TApaApplicationFactory::TApaApplicationFactory()
	:iType(ETypeFunction),
	 iData(0),
	 iApplication(NULL)
	{
	}

/** 
Constructor.
@publishedAll
@released
@param aFunction The function from which the application is to be created.
*/
EXPORT_C TApaApplicationFactory::TApaApplicationFactory(TFunction aFunction)
	:iType(ETypeFunction),
	 iData(reinterpret_cast<TUint>(aFunction)),
	 iApplication(NULL)
	{
	}

/** 
Constructor. Use this constructor in preference to the constructor taking a "TUid" parameter 
if at all possible as it is much more efficient.
@publishedAll
@released
@param aEmbeddedApplicationInformation The ECOM implementation-information of the embedded application to be created.
*/
EXPORT_C TApaApplicationFactory::TApaApplicationFactory(const CImplementationInformation& aEmbeddedApplicationInformation)
	:iType(ETypeEmbeddedApplicationInformation),
	 iData(reinterpret_cast<TUint>(&aEmbeddedApplicationInformation)),
	 iApplication(NULL)
	{
	}

/** 
Constructor. Use the constructor taking a "const CImplementationInformation&" parameter in preference 
to this constructor if at all possible as it is much more efficient.
@publishedAll
@released
@param aEmbeddedApplicationUid The ECOM implementation-UID of the embedded application to be created.
*/
EXPORT_C TApaApplicationFactory::TApaApplicationFactory(TUid aEmbeddedApplicationUid)
	:iType(ETypeEmbeddedApplicationUid),
	 iData(aEmbeddedApplicationUid.iUid),
	 iApplication(NULL)
	{
	}

#ifdef USING_ECOM_RECOGS
CApaApplication* TApaApplicationFactory::CreateApplicationL() const
	{
	CApaApplication* application = NULL;

	switch (iType)
		{
		case ETypeFunction:
			{
			__ASSERT_DEBUG(iData, Panic(EPanicNullPointer));
			application=(*reinterpret_cast<TFunction>(iData))();
			break;
			}
		case ETypeEmbeddedApplicationInformation:
			{
			__ASSERT_DEBUG(iData, Panic(EPanicNullPointer));
			const CImplementationInformation& embeddedApplicationInformation=*reinterpret_cast<const CImplementationInformation*>(iData);
			TUid uid = embeddedApplicationInformation.ImplementationUid();
			application=CreateEmbeddedApplicationL(uid);
			break;
			}
		case ETypeEmbeddedApplicationUid:
			{
			TUid uid = TUid::Uid(iData);
			application=CreateEmbeddedApplicationL(uid);
			break;
			}
		default:
			Panic(EPanicBadApplicationFactoryType);
		}

	return application;
	}

HBufC* TApaApplicationFactory::AppFileNameL() const
	{
	HBufC* appFileName = NULL;
	switch (iType)
		{
		case ETypeFunction:
			{
			// Assume that if the type is a function pointer then the app is not embedded (so
			// the filename is the filename of this process).
			appFileName = RProcess().FileName().AllocL();
			break;
			}
		case ETypeEmbeddedApplicationInformation:
			{
			const CImplementationInformation& embeddedApplicationInformation=*REINTERPRET_CAST(const CImplementationInformation*,iData);
			appFileName = FullAppFileNameL(embeddedApplicationInformation.DisplayName());
			break;
			}
		case ETypeEmbeddedApplicationUid:
			{
			TUid uid = TUid::Uid(iData);
			HBufC* displayName = EmbeddedApplicationDisplayNameLC(uid);
			appFileName = FullAppFileNameL(*displayName);
			CleanupStack::PopAndDestroy(displayName);
			break;
			}
		default:
			Panic(EPanicBadApplicationFactoryType);
		}

	return appFileName;
	}

TUid TApaApplicationFactory::AppFileUid() const
	{
	TUid uid=KNullUid;
	switch (iType)
		{
		case ETypeFunction:
			{
			uid = RProcess().Type()[2];
			break;
			}
		case ETypeEmbeddedApplicationInformation:
			{
			const CImplementationInformation& embeddedApplicationInformation=*REINTERPRET_CAST(const CImplementationInformation*,iData);
			uid = embeddedApplicationInformation.ImplementationUid();
			break;
			}
		case ETypeEmbeddedApplicationUid:
			{
			uid = TUid::Uid(iData);
			break;
			}
		default:
			Panic(EPanicBadApplicationFactoryType);
		}
	return uid;
	}

HBufC* TApaApplicationFactory::FullAppFileNameL(const TDesC& aAppName)
	{
	// This was appropriately changed for data caging (binaries placed in \sys\bin\)
	TFileName fileName;
	Dll::FileName(fileName);

	TParse parse;
	parse.SetNoWild(aAppName, &KApplicationLocation, &fileName);
	return parse.FullName().AllocL();
	}

CApaApplication* TApaApplicationFactory::CreateEmbeddedApplicationL(TUid aUid)
	{ // static
	CApaApplication* const application=static_cast<CApaApplication*>(REComSession::CreateImplementationL(aUid,_FOFF(CApaApplication,iDtorKey)));
	const TUid appUid = application->AppDllUid();
	__ASSERT_ALWAYS(appUid==aUid, Panic(EPanicUidsDoNotMatch));
	return application;
	}


HBufC* TApaApplicationFactory::EmbeddedApplicationDisplayNameLC(TUid aUid)
	{ // static
	HBufC* displayName=NULL;

	RImplInfoPtrArray implementationArray;
	CleanupStack::PushL(TCleanupItem(CleanupImplementationArray,&implementationArray));
	REComSession::ListImplementationsL(KUidFileEmbeddedApplicationInterfaceUid,implementationArray);
	for (TInt i=implementationArray.Count()-1; i>=0; --i)
		{
		const CImplementationInformation& implementationInformation=*implementationArray[i];
		if (implementationInformation.ImplementationUid().iUid==aUid.iUid)
			{
			displayName=implementationInformation.DisplayName().AllocL();
			break;
			}
		}
	CleanupStack::PopAndDestroy(&implementationArray);
	if (displayName==NULL)
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PushL(displayName);

	return displayName;
	}

void TApaApplicationFactory::CleanupImplementationArray(TAny* aImplementationArray)
	{ // static
	__ASSERT_DEBUG(aImplementationArray, Panic(EPanicNullPointer));
	RImplInfoPtrArray& implementationArray=*static_cast<RImplInfoPtrArray*>(aImplementationArray);
	implementationArray.ResetAndDestroy();
	implementationArray.Close();
	}
	
//
// MApaEmbeddedDocObserver
//

/** Constructor for MApaEmbeddedDocObserver */
EXPORT_C MApaEmbeddedDocObserver::MApaEmbeddedDocObserver()
	{
	}

/** Reserved for future use */
EXPORT_C void MApaEmbeddedDocObserver::MApaEmbeddedDocObserver_Reserved1()
	{
	}

/** Reserved for future use */
EXPORT_C void MApaEmbeddedDocObserver::MApaEmbeddedDocObserver_Reserved2()
	{
	}
#endif // USING_ECOM_RECOGS
