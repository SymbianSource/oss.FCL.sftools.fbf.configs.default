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

#include "../apserv/APSCLSV.H"
#include "APGCLI.H"
#include "APACMDLN.H"
#include "APGSTD.H"
#include "APGICNFL.H"
#include <apsserv.h>
#include <apmrec.h>
#include <s32mem.h>
#include "APGPRIV.H"
#include <apadef.h>
#include "apprivate.h"

#if defined(USE_IH_RAISE_EVENT)
#include <SystemMonitor/RaiseEvent.h>
#include <test/testinstrumentation.h>
#endif

class CApaLsSessionExtension;

const TUint KInitialIconBufSize=sizeof(TSize)*6+sizeof(TInt);
const TUint KInitialViewDataBufSize=sizeof(TApaAppViewInfo)*6+sizeof(TInt);
const TUint KInitialOwnedFilesBufSize=sizeof(TFileName)*6+sizeof(TInt);
const TUint KDefaultBufSize = 0x400;
const TUint KAsyncMessageSlots=4; //one slot each for, RApaLsSession::SetNotify, RApaLsSession::
                                  //RegisterListPopulationCompleteObserver and RApaLsSession::NotifyOnDataMappingChange
                                  //CAsyncFileRecognition::Start makes an asynch request of apparc server.

_LIT8(KAllDataTypes,"*");

extern void CleanupServiceArray(TAny* aServiceArray);

IMPORT_C extern const TInt KMinApplicationStackSize;

class MArrayFiller
	{
public:
	virtual void AppendItemL(RReadStream& aReadStream)=0;
	};

class TSizeArrayFiller : public MArrayFiller
	{
public:
	inline TSizeArrayFiller(CArrayFixFlat<TSize>& aArray) :iArray(aArray) {}
	virtual void AppendItemL(RReadStream& aReadStream);
private:
	CArrayFixFlat<TSize>& iArray;
	};

class TViewDataArrayFiller : public MArrayFiller
	{
public:
	inline TViewDataArrayFiller(CApaAppViewArray& aArray) :iArray(aArray) {}
	virtual void AppendItemL(RReadStream& aReadStream);
private:
	CApaAppViewArray& iArray;
	};

class TDesCArrayFiller : public MArrayFiller
	{
public:
	inline TDesCArrayFiller(CDesCArray& aArray) :iArray(aArray) {}
	virtual void AppendItemL(RReadStream& aReadStream);
private:
	CDesCArray& iArray;
	};

/**
This class is used to simplify the usage of the asynchronous RApaLsSession::RecognizeFilesL() function.
@internalComponent
*/
class CAsyncFileRecognition : public CActive
	{
public:
	CAsyncFileRecognition(CDataRecognitionResultArray& aResult, TRequestStatus& aUsersRequestStatus, CApaLsSessionExtension& aSession);
	~CAsyncFileRecognition();
	void Start(const TDesC& aPath, const TDesC8& aDataType);
private:
	void RunL();
	void DoCancel();
private: // data
	TPckgBuf<TUint> iRequiredBufferSizePckg;
	CDataRecognitionResultArray& iResult;
	TRequestStatus& iUsersRequestStatus;
	CApaLsSessionExtension& iSession;
	const TDesC* iPath;
	const TDesC8* iDataType;
	};

/**
This class is an extension class for RApaLsSession. It was added to reduce the need
of BC breaks in later releases.
@internalComponent
*/
class CApaLsSessionExtension : public CBase
	{
public:
	CApaLsSessionExtension(RApaLsSession& aSession);
	~CApaLsSessionExtension();

	void SendReceive(TInt aFunction,const TIpcArgs& aArgs,TRequestStatus& aStatus) const;
	TInt SendReceive(TInt aFunction,const TIpcArgs& aArgs) const;

	// file recognition functions
	void RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus);
	void CancelRecognizeFiles();
	void FilesRecognized(const TDesC& aPath, TInt aRequiredBufferSize, CDataRecognitionResultArray& aResult, TRequestStatus& aUsersRequestStatus, const TRequestStatus& aErrorCode);
	TBool RecognitionActive() const;
private:
	RApaLsSession& iSession;	// we are a friend of this class!
	CAsyncFileRecognition* iAsyncFileRecognition;
	};

/**
An entry of a CDataRecognitionResultArray object. Stores TDataRecognitionResult and the file name
in a compact format.
@internalComponent
*/
class CDataRecognitionResultArrayEntry : public CBase
	{
public:
	CDataRecognitionResultArrayEntry(HBufC* aFileName, HBufC8* aDataType, TUid aUid, TInt aConfidence);
	~CDataRecognitionResultArrayEntry();
	void GetDataRecognitionResult(TDataRecognitionResult& aResult) const;
	const TDesC& FileName() const;
private:
	HBufC* iFileName;
	HBufC8* iDataType;
	TUid iUid;
	TInt iConfidence;
	};

//////////////////////////////
// CAsyncFileRecognition
//////////////////////////////

CAsyncFileRecognition::CAsyncFileRecognition(CDataRecognitionResultArray& aResult, TRequestStatus& aUsersRequestStatus, CApaLsSessionExtension& aSession)
	: CActive(EPriorityStandard),
	  iResult(aResult),
	  iUsersRequestStatus(aUsersRequestStatus),
	  iSession(aSession)
	{
	CActiveScheduler::Add(this);
	}

void CAsyncFileRecognition::Start(const TDesC& aPath, const TDesC8& aDataType)
	{
	iPath = &aPath;
	iDataType = &aDataType;
	iStatus = KRequestPending;
	iSession.SendReceive(EAppListServRecognizeFilesAsync,TIpcArgs(&aPath,&iRequiredBufferSizePckg,&aDataType),iStatus);
	SetActive();
	}

CAsyncFileRecognition::~CAsyncFileRecognition()
	{
	Cancel();
	iPath = NULL;
	iDataType = NULL;
	}

void CAsyncFileRecognition::RunL()
	{
	iSession.FilesRecognized(*iPath, iRequiredBufferSizePckg(), iResult, iUsersRequestStatus, iStatus);
	}

void CAsyncFileRecognition::DoCancel()
	{
	iSession.SendReceive(ECancelRecognizeFiles,TIpcArgs(TIpcArgs::ENothing));
	TRequestStatus* status = &iUsersRequestStatus;
	User::RequestComplete( status, KErrCancel );
	}

//////////////////////////////
// CApaLsSessionExtension
//////////////////////////////

CApaLsSessionExtension::CApaLsSessionExtension(RApaLsSession& aSession)
	: iSession(aSession),
	  iAsyncFileRecognition(NULL)
	{
	}

CApaLsSessionExtension::~CApaLsSessionExtension()
	{
	delete iAsyncFileRecognition;
	}

void CApaLsSessionExtension::SendReceive(TInt aFunction,const TIpcArgs& aArgs,TRequestStatus& aStatus) const
	{
	iSession.SendReceive(aFunction,aArgs,aStatus); //lint !e1060 Suppress protected member is not accessible to non-member non-friend
	}

TInt CApaLsSessionExtension::SendReceive(TInt aFunction,const TIpcArgs& aArgs) const
	{
	return iSession.SendReceive(aFunction,aArgs); //lint !e1060 Suppress protected member is not accessible to non-member non-friend
	}

/**
Asynchronous recognition of a directory.
Function uses an internal CAsyncFileRecognition object to hide the 
second server message (transferring the data to the client) from the user.
When the path is not correct or the server is already busy with another 
recognition request, the function completes the request with an error.
@internalComponent
*/
void CApaLsSessionExtension::RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus)
	{
	_LIT(KBackslash,"\\");

	if(aPath.Right(1) != KBackslash)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrPathNotFound);
		}
	
	aResult.SetPath(aPath);
	aStatus = KRequestPending;

	if(!iAsyncFileRecognition)
		{
		iAsyncFileRecognition = new (ELeave) CAsyncFileRecognition(aResult, aStatus, *this);
		iAsyncFileRecognition->Start(aPath, aDataType);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrInUse);
		}
	}

TBool CApaLsSessionExtension::RecognitionActive() const
	{
	return iAsyncFileRecognition ? ETrue : EFalse;
	}

void CApaLsSessionExtension::CancelRecognizeFiles()
	{
	if(iAsyncFileRecognition)
		{
		iAsyncFileRecognition->Cancel();
		delete iAsyncFileRecognition;
		iAsyncFileRecognition = NULL;
		}
	}

void CApaLsSessionExtension::FilesRecognized(const TDesC& aPath, TInt aRequiredBufferSize, CDataRecognitionResultArray& aResult, TRequestStatus& aUsersRequestStatus, const TRequestStatus& aErrorCode)
/**
Callback function called by an CAsyncFileRecognition object, when an asynchronous
recognition has finished. If the recognition was successful, the data is transferred
to the client-side.
@internalComponent
*/
	{
	TRequestStatus* status = &aUsersRequestStatus;
	if(aErrorCode == KErrNone)
		{
		// transfer the result
		TRAPD(error,iSession.TransferAndInternalizeDataL(aPath, aRequiredBufferSize, aResult));
		User::RequestComplete(status, error);
		}
	else
		{
		User::RequestComplete(status, aErrorCode.Int());
		}
	delete iAsyncFileRecognition;
	iAsyncFileRecognition = 0;
	}


///////////////////////////////////////
// CDataRecognitionResultArrayEntry
///////////////////////////////////////

CDataRecognitionResultArrayEntry::CDataRecognitionResultArrayEntry(HBufC* aFileName, HBufC8* aDataType, TUid aUid, TInt aConfidence)
	: iFileName(aFileName), 
	  iDataType(aDataType), 
	  iUid(aUid), 
	  iConfidence(aConfidence)
	{
	}

CDataRecognitionResultArrayEntry::~CDataRecognitionResultArrayEntry()
	{
	delete iFileName;
	delete iDataType;
	}

void CDataRecognitionResultArrayEntry::GetDataRecognitionResult(TDataRecognitionResult& aResult) const
	{
	TDataRecognitionResult result;
	if(iDataType->Length() != 0)
		{
		TDataType dataType(*iDataType);
		result.iDataType = dataType;
		}
	else
		{
		TDataType dataType(iUid);
		result.iDataType = dataType;
		}
	result.iConfidence = iConfidence;
	aResult = result;
	}

const TDesC& CDataRecognitionResultArrayEntry::FileName() const
	{
	if (iFileName == NULL)
		return KNullDesC;
	else
		return *iFileName;
	}


///////////////////////////////////
// CDataRecognitionResultArray
///////////////////////////////////

/**
Constructor
@publishedAll
@released
*/
EXPORT_C CDataRecognitionResultArray::CDataRecognitionResultArray()
	{
	}

/**
Destructor
@publishedAll
@released
*/
EXPORT_C CDataRecognitionResultArray::~CDataRecognitionResultArray()
	{
	iEntries.ResetAndDestroy();
	}

/**
Returns the path of the recognition results.
@publishedAll
@released
@return The path of the recognition results
*/
EXPORT_C const TFileName& CDataRecognitionResultArray::Path() const
	{
	return iPath;
	}

/**
Returns the number of entries in the CDataRecognitionResultArray.
@publishedAll
@released
@return The number of entries in the CDataRecognitionResultArray.
*/
EXPORT_C TUint CDataRecognitionResultArray::Count() const
	{
	return iEntries.Count();
	}

/**
Returns the recognition result of the given index.
@publishedAll
@released
@param aResult On successful completion, this parameter contains the recognition result at the given index.
@param aIndex The index of the recognition result to be returned. An invalid index causes
the function to leave with KErrNotFound.
@return The requested TDataRecognitionResult object
@leave KErrNotFound An invalid index is passed
*/
EXPORT_C void CDataRecognitionResultArray::GetDataRecognitionResultL(TDataRecognitionResult& aResult, const TUint aIndex) const
	{
	if( aIndex >= iEntries.Count() )
		{
		User::Leave(KErrNotFound);
		}
	iEntries[aIndex]->GetDataRecognitionResult(aResult);
	}

/**
Returns the file name of the object at the given index.
@publishedAll
@released
@param aFileName On successful completion, this parameter contains the file name of the object at the given index. 
@param aIndex Specifies which file name to be returned. An invalid index causes
the function to leave with KErrNotFound.
@return The requested file name
@leave KErrNotFound An invalid index is passed
*/
EXPORT_C void CDataRecognitionResultArray::GetFileNameL(TFileName& aFileName, const TUint aIndex) const
	{
	if( aIndex >= iEntries.Count() )
		{
		User::Leave(KErrNotFound);
		}
	aFileName.Zero();
	aFileName.Copy( (iEntries[aIndex]->FileName()) );
	}

TUint CDataRecognitionResultArray::InternalizeL(const CBufFlat& aBuffer)
	{
	RBufReadStream readStream;
	
	readStream.Open(aBuffer);
	const TInt count=readStream.ReadUint32L();
	for (TInt i=0; i<count; ++i)
		{
		const TInt fileNameLength = readStream.ReadUint8L();
		HBufC* const fileName = HBufC::NewLC(fileNameLength);
		TPtr fileNamePtr = fileName->Des();
		readStream.ReadL(fileNamePtr,fileNameLength);

		const TInt dataTypeLength = readStream.ReadUint8L();
		HBufC8* const dataType = HBufC8::NewLC(dataTypeLength);
		TPtr8 dataTypePtr = dataType->Des();
		readStream.ReadL(dataTypePtr,dataTypeLength);

		TUid uid;
		uid.iUid = readStream.ReadInt32L();
		
		TInt const confidence = readStream.ReadInt32L();
		
		CDataRecognitionResultArrayEntry* entry = new (ELeave) CDataRecognitionResultArrayEntry(fileName,dataType,uid,confidence);
		iEntries.Append(entry);
		
		CleanupStack::Pop(dataType);	// ownership was transferred to "entry"
		CleanupStack::Pop(fileName);	// ownership was transferred to "entry"
		}

	readStream.Close();
		
	return KErrNone;
	}

void CDataRecognitionResultArray::SetPath(const TFileName& aPath)
	{
	iPath.Copy(aPath);
	}

////////////////////////////////////
//RApaLsSession
////////////////////////////////////

EXPORT_C RApaLsSession::RApaLsSession()
	: iExtension(0)
	{}



/** Connects a client to the application architecture server, creating a session 
with it.

@return KErrNone if successful; otherwise, one of the system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::Connect()
	{
	TInt r=CreateSession(KAppListServerName,Version(),KAsyncMessageSlots);
	return(r); 
	}



/** Gets the version of the application architecture server.

@return The version number. 
*/
EXPORT_C TVersion RApaLsSession::Version(void) const
	{
	return(TVersion(KAppListServMajorVersionNumber,KAppListServMinorVersionNumber,KAppListServBuildVersionNumber));
	}



/** Gets the total number of applications.

Control panel applications are excluded.

@param aCount On return, the total number of applications.
@return KErrNone if successful, otherwise one of the system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::AppCount(TInt& aCount) const
	{
	return DoAppCount(aCount,EAppListServAppCount);
	}



/** Gets the total number of embeddable applications.

Control panel applications are excluded.

@param aCount On return, the total number of embeddable applications.
@return KErrNone, if successful, otherwise one of the system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::EmbeddableAppCount(TInt& aCount) const
	{
	return DoAppCount(aCount,EAppListServEmbedCount);
	}

TInt RApaLsSession::DoAppCount(TInt& aCount,TInt aCommand) const
// returns the number of embeddable apps in the server side list
	{
	__ASSERT_DEBUG(aCommand==EAppListServEmbedCount || aCommand==EAppListServAppCount,Panic(EDPanicWrongCommand));
	const TInt returnValue=SendReceiveWithReconnect(aCommand,TIpcArgs());
	if (returnValue<0)
		{
		return returnValue;
		}
	aCount=returnValue;
	return KErrNone;
	}



/** Initialises the process of getting all applications in the cached list.

Control panel applications are excluded.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve all applications in the cached list.

@return KErrNone, if successful, otherwise one of the system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAllApps() const
	{
	return GetAllApps(0);
	}



/** Initialises the process of getting all applications in the cached list.

Control panel applications are excluded.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve all applications supporting aScreenMode in the cached list.

@param aScreenMode Only applications which define a view supporting aScreenMode
will be returned by subsequent calls to GetNextApp(). If an application does not
define views in it's application information file, only screen mode 0 is supported.
@return KErrNone, if successful, otherwise one of the system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAllApps(TInt aScreenMode) const
	{
	return GetFilteredApps(TApaAppCapability::EControlPanelItem, 0, aScreenMode); // exclude control panel apps
	}



/** Initialises the process of getting all embeddable applications from the cached list.

Control panel applications are excluded.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve embeddable applications in the cached list.

Only applications which specify KAppEmbeddable or KAppEmbeddableOnly in their
application information file will be returned by subsequent calls to GetNextApp().

@return KErrNone, if successful, otherwise one of the system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetEmbeddableApps() const
	{
	return GetEmbeddableApps(0);
	}



/** Initialises the process of getting embeddable applications from the cached list
that support the specified screen mode.

Control panel applications are excluded.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve embeddable applications in the cached list.

Only applications which specify KAppEmbeddable or KAppEmbeddableOnly in their
application information file will be returned by subsequent calls to GetNextApp().

@param aScreenMode Only embeddable applications which define a view supporting
aScreenMode will be returned by subsequent calls to GetNextApp(). If an application
does not define views in it's application information file, only screen mode 0 is supported.
@return KErrNone, if successful, otherwise one of the system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetEmbeddableApps(TInt aScreenMode) const
	{
	TApaEmbeddabilityFilter filter;
	filter.AddEmbeddability(TApaAppCapability::EEmbeddable);
	filter.AddEmbeddability(TApaAppCapability::EEmbeddableOnly);
	return GetFilteredApps(filter, aScreenMode);
	}



/** Initialises the process of getting all applications matching aFilter in the
cached list.

Control panel applications are excluded.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve all applications matching aFilter in the cached list.

@param aFilter Defines the filter to be applied to the cached list.
@return KErrNone, if successful, otherwise one of the system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetFilteredApps(const TApaEmbeddabilityFilter& aFilter) const
	{
	return GetFilteredApps(aFilter, 0);
	}



/** Initialises the process of getting all applications matching aFilter in the
cached list.

Control panel applications are excluded.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve all applications matching aFilter in the cached list.

@param aFilter Defines the filter to be applied to the cached list.
@param aScreenMode Only applications which define a view supporting aScreenMode
will be returned by subsequent calls to GetNextApp(). If an application does not
define views in it's application information file, only screen mode 0 is supported.
@return KErrNone, if successful, otherwise one of the system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetFilteredApps(const TApaEmbeddabilityFilter& aFilter, TInt aScreenMode) const
	{
	const TPckgC<TApaEmbeddabilityFilter> filter(aFilter);
	return SendReceiveWithReconnect(EAppListServInitFilteredEmbedList,TIpcArgs(aScreenMode,&filter));
	}



/** Initialises the process of getting all applications matching the specified
application attributes.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve all applications matching the filter in the cached list.

Attributes are defined by TApaAppCapability::TCapabilityAttribute

@param aCapabilityAttributeMask Specifies the attributes whose values will be
used to filter the cached list. If the mask specifies more than one attribute,
all associated attribute values must match.
@param aCapabilityAttributeValue Specifies the attribute values for each attribute
identified by the mask.
@return KErrNone, if successful, otherwise one of the system-wide error codes.
@see TCapabilityAttribute 
*/
EXPORT_C TInt RApaLsSession::GetFilteredApps(TUint aCapabilityAttributeMask, TUint aCapabilityAttributeValue) const
	{
	return GetFilteredApps(aCapabilityAttributeMask, aCapabilityAttributeValue, 0);
	}



/** Initialises the process of getting all applications matching the specified
application attributes.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve all applications matching the filter in the cached list.

Attributes are defined by TApaAppCapability::TCapabilityAttribute

@param aCapabilityAttributeMask Specifies the attributes whose values will be
used to filter the cached list. If the mask specifies more than one attribute,
all associated attribute values must match.
@param aCapabilityAttributeValue Specifies the attribute values for each attribute
identified by the mask.
@param aScreenMode Only applications which define a view supporting aScreenMode
will be returned by subsequent calls to GetNextApp(). If an application does not
define views in it's application information file, only screen mode 0 is supported.
@return KErrNone, if successful, otherwise one of the system-wide error codes.
@see TCapabilityAttribute 
*/
EXPORT_C TInt RApaLsSession::GetFilteredApps(TUint aCapabilityAttributeMask, TUint aCapabilityAttributeValue, TInt aScreenMode) const
	{
	return SendReceiveWithReconnect(EAppListServInitAttrFilteredList,TIpcArgs(aScreenMode, aCapabilityAttributeMask, aCapabilityAttributeValue));
	}



/** Initialises the process of getting server applications in the cached list.

Control panel applications are excluded.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve server applications in the cached list.

Only applications which specify one or more services in their application
information file will be returned by subsequent calls to GetNextApp().

@return KErrNone, if successful, otherwise one of the system-wide error codes.
@publishedPartner
@released
*/
EXPORT_C TInt RApaLsSession::GetServerApps(TUid aServiceUid) const
	{
	return GetServerApps(aServiceUid, 0);
	}



/** Initialises the process of getting server applications in the cached list.

Control panel applications are excluded.

A call to this function is followed by subsequent and repeated calls to GetNextApp() 
to retrieve server applications in the cached list.

Only applications which specify one or more services in their application
information file will be returned by subsequent calls to GetNextApp().

@param aScreenMode Only server applications which define a view supporting
aScreenMode will be returned by subsequent calls to GetNextApp(). If an application
does not define views in its application information file, only screen mode 0 is supported.
@return KErrNone, if successful, otherwise one of the system-wide error codes.
@publishedPartner
@released
*/
EXPORT_C TInt RApaLsSession::GetServerApps(TUid aServiceUid, TInt aScreenMode) const
	{
	return SendReceiveWithReconnect(EAppListServInitServerAppList,TIpcArgs(aScreenMode, aServiceUid.iUid));
	}



/** Gets information about the next application or embeddable application from 
the cached list.

A sequence of calls to this function must always be preceded by a call to 
one of GetAllApps(), GetEmbeddableApps() or GetFilteredApps().

@param aInfo On return, contains application information.
@return KErrNone if successful; ENoMoreAppsInList, if there are no more applications 
in the list; EAppListInvalid if the server's initial population of the list has
not completed; otherwise one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetNextApp(TApaAppInfo& aInfo) const
	{
	return GetNextApp(aInfo,0);
	}



/** Gets information about the next application or embeddable application from 
the cached list.

A sequence of calls to this function must always be preceded by a call to 
one of GetAllApps(), GetEmbeddableApps() or GetFilteredApps().

@param aInfo On return, contains application information.
@param aScreenMode This parameter is ignored.
@return KErrNone if successful; ENoMoreAppsInList, if there are no more applications 
in the list; EAppListInvalid if the server's initial population of the list has
not completed; otherwise one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetNextApp(TApaAppInfo& aInfo,TInt aScreenMode) const
	{
	TPckg<TApaAppInfo> info(aInfo);
	TInt ret=SendReceiveWithReconnect(EAppListServGetNextApp,TIpcArgs(aScreenMode,&info));
	if (ret==KErrNone)
		aInfo = info();
	else if (ret==KErrNotFound)
		ret = ENoMoreAppsInList;
	else if (ret==KErrCorrupt)
		ret = EAppListInvalid;
	return ret;
	}



/** Gets information about the application with the specified UID.

@param aInfo On return, contains the application information, if an application 
with the specified UID is found. If no matching application is found, then 
this object is not changed.
@param aAppUid The application specific UID.
@return KErrNone if successful; KErrNotFound if a matching entry could not be found; 
otherwise one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAppInfo(TApaAppInfo& aInfo,TUid aAppUid) const
	{
	TPckg<TApaAppInfo> info(aInfo);
	return SendReceiveWithReconnect(EAppListServGetAppInfo,TIpcArgs(aAppUid.iUid,&info));
	} //lint !e1764 Suppress reference parameter 'aInfo' could be declared const ref
	
/** Sets the short caption of the application.

Overrides the short caption specified in the localizable resource file for this application.
Short captions set using this API will only take effect until the next device reset.

@param aShortCaption The short caption of the application. The maximum length allowed is KApaMaxAppCaption.
@param aLanguage The language corresponding to the caption. If this is ELangNone the caption is used
for all languages for which a language specific short caption has not been set.
@param aAppUid The uid of the application.
@return KErrNone if successful, otherwise one of the system wide error codes.
*/ 
EXPORT_C TInt RApaLsSession::SetAppShortCaption(const TDesC& aShortCaption, TLanguage aLanguage, TUid aAppUid)
	{
	if (aShortCaption.Length() > KApaMaxAppCaption || aShortCaption.Length() == 0)
		{
		return KErrArgument;
		}
	return SendReceiveWithReconnect(EAppListServSetAppShortCaption, TIpcArgs(aAppUid.iUid, &aShortCaption, aLanguage));
	} //lint !e1762 Suppress member function could be made const

/** Gets the default screen-number of the application with the specified UID.

@param aDefaultScreenNumber On return, contains the default screen-number, if an application 
with the specified UID is found. If no matching application is found, then 
this object is not changed.
@param aAppUid The application specific UID.
@return KErrNone if successful; KErrNotFound if a matching entry could not be found; 
otherwise one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetDefaultScreenNumber(TInt& aDefaultScreenNumber, TUid aAppUid) const
	{
	const TInt result=SendReceiveWithReconnect(EAppListServGetDefaultScreenNumber, TIpcArgs(aAppUid.iUid));
	if (result<0)
		{
		return result;
		}
	aDefaultScreenNumber=result;
	return KErrNone;
	}


/** Gets the capabilities of the application with the specified UID.

@param aCapabilityBuf A modifiable descriptor that, on return, contains the 
application's capability information. The data returned in the descriptor 
is mapped by the TApaAppCapability class. If no matching application is found, 
then the content of this descriptor is not changed.
@param aAppUid The application specific UID.
@return KErrNone, if successful; KErrNotFound, if no matching entry can be found; otherwise 
one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAppCapability(TDes8& aCapabilityBuf,TUid aAppUid) const
	{
	return SendReceiveWithReconnect(EAppListServGetAppCapability,TIpcArgs(&aCapabilityBuf,aAppUid.iUid));
	}



/** Starts an application defined by the specified command line information.
    This is an asynchronous method which doesn't wait for the process creation to complete.
    To be informed of the process creation success, 
    then appropriate overloaded method taking a TRequestStatus parameter should be used.
    
This is only recommended for non document based applications.

View based applications are usually started by activating a specific view 
using CCoeAppUi::ActivateViewL. Alternatively, using StartApp() to start a 
view based application will activate the application's default view.

@param aCommandLine The command line.
@return KErrNone, if successful; KErrNotFound, if the application cannot be 
found; otherwise one of the other system-wide error codes. 
@see CCoeAppUi::ActivateViewL() 
*/
EXPORT_C TInt RApaLsSession::StartApp(const CApaCommandLine& aCommandLine)
	{
	return DoStartApp(aCommandLine, NULL,NULL);
	}
	
/** Starts an application defined by the specified command line information.
    This is an asynchronous method which doesn't wait for the process creation to complete.
    To be informed of the process creation success, 
    then appropriate overloaded method taking a TRequestStatus parameter should be used.

This is only recommended for non document based applications.

View based applications are usually started by activating a specific view 
using CCoeAppUi::ActivateViewL. Alternatively, using StartApp() to start a 
view based application will activate the application's default view.

@param aCommandLine The command line.
@param aThreadId On return, the id of the main thread started.
@return KErrNone, if successful; KErrNotFound, if the application cannot be 
found; otherwise one of the other system-wide error codes. 
@see CCoeAppUi::ActivateViewL() 
*/	
EXPORT_C TInt RApaLsSession::StartApp(const CApaCommandLine& aCommandLine,TThreadId& aThreadId)
	{
	return DoStartApp(aCommandLine, &aThreadId,NULL);
	}

	
/** Starts an application defined by the specified command line information.

This is only recommended for non document based applications.

View based applications are usually started by activating a specific view 
using CCoeAppUi::ActivateViewL. Alternatively, using StartApp() to start a 
view based application will activate the application's default view.

@param aCommandLine The command line.
@param aThreadId On return, the id of the main thread started.
@param aRequestStatusForRendezvous If not NULL, the asynchronous RProcess::Rendezvous() 
function is called (being passed this TRequestStatus object) before RProcess::Resume() is called on 
the new application process. If this function does not return KErrNone, RProcess::Rendezvous() will
not be called passing aRequestStatusForRendezvous, so in this case the caller must not wait
on aRequestStatusForRendezvous.
@return KErrNone, if successful; KErrNotFound, if the application cannot be 
found; otherwise one of the other system-wide error codes. 
@see CCoeAppUi::ActivateViewL() 
*/
EXPORT_C TInt RApaLsSession::StartApp(const CApaCommandLine& aCommandLine,TThreadId& aThreadId,TRequestStatus* aRequestStatusForRendezvous )
	{
	return DoStartApp(aCommandLine, &aThreadId,aRequestStatusForRendezvous);
	}


TInt RApaLsSession::DoStartApp(const CApaCommandLine& aCommandLine, TThreadId* aThreadId,TRequestStatus* aRequestStatusForRendezvous)
	{
	TRAPD(error, DoStartAppL(aCommandLine, aThreadId, aRequestStatusForRendezvous));
	return error;
	}

void RApaLsSession::DoStartAppL(const CApaCommandLine& aCommandLine, TThreadId* aThreadId,TRequestStatus* aRequestStatusForRendezvous)
	{
	// This function does not require "this" object to be connected to the Apparc server,
	// but if not, it works with some limitations (see the following document for a list
	// of these limitations: 
	// generic/app-framework/Documentation/PREQ967_solution_constraints.doc).

#if defined(USE_IH_RAISE_EVENT)
	const TInt appStartupInstrumentationEventIdBase=aCommandLine.AppStartupInstrumentationEventIdBase();
	if (appStartupInstrumentationEventIdBase!=0)
		{
		IH_DECLARE( lInstrumentationHandler );
		IH_CREATE( lInstrumentationHandler );
		IH_RAISE_EVENT( lInstrumentationHandler, appStartupInstrumentationEventIdBase+MTestInstrumentation::TIDOffsetBeginApplicationFirstRedraw );
		IH_RAISE_EVENT( lInstrumentationHandler, appStartupInstrumentationEventIdBase+MTestInstrumentation::TIDOffsetBeginApplicationReadyForInput );
		IH_DELETE( lInstrumentationHandler );
		}
#endif

 	// Retrieve the executable name from the CApaCommandLine object passed in.
	const TPtrC logicalExecutableName(aCommandLine.ExecutableName());
	// Rule-based app launching is not allowed unless there is a connected RApaLsSession object.
	if(Handle() != KNullHandle)
		{
		// requesting from rule-based plug-ins if we can run an application 
		// if server fails while requested rule-based plug-ins it returns a negative value - proceed with launching the application in this case
		TBool okayToRun = SendReceiveWithReconnect(EAppListServRuleBasedLaunching, TIpcArgs(&logicalExecutableName));
		if(!okayToRun)
			{
			User::Leave(KErrCancel);
			}
		}

	TFileName nativeExecutableNameOfNonNativeApplication;
	RProcess process;
	HBufC8* opaqueData=NULL;
	CleanupStack::PushL(TCleanupItem(DeletePointerToPointerToTAny, &opaqueData));

	// if we're connected to the Apparc server, try to get the opaque-data and native-executable name
	// (the latter is only if it's a non-native application that we're launching)
	if (Handle()!=KNullHandle)
		{
		const TInt lengthOfOpaqueData=User::LeaveIfError(SendReceiveWithReconnect(EAppListServGetNativeExecutableNameIfNonNative, TIpcArgs(&nativeExecutableNameOfNonNativeApplication, &logicalExecutableName)));
		if (lengthOfOpaqueData>0)
			{
			User::LeaveIfError(GetOpaqueData(opaqueData, lengthOfOpaqueData));
			}
		}

	// try first to create the application process without interacting with the Apparc server at all - 
	// assumes "logicalExecutableName" is itself a native executable
	TUidType uidType(KNullUid, KNullUid, KNullUid);
	TInt err = process.CreateWithStackOverride(logicalExecutableName, KNullDesC, uidType, MinApplicationStackSize(), EOwnerProcess);
	 
	// If we haven't been able to create the process using the native executable name from the command line
	// object, instead try to create it using the native executable name of the non-native application.
	// Can only do this if apparc is connected and thus this name has been retrieved above and 
	// nativeExecutableNameOfNonNativeApplication populated.
	if ((err != KErrNone) && nativeExecutableNameOfNonNativeApplication.Length()>0)
			{
			err = process.CreateWithStackOverride(nativeExecutableNameOfNonNativeApplication, KNullDesC, uidType, MinApplicationStackSize(), EOwnerProcess);
			}

	// if we managed to create the process via either of the two methods attempted above (with the native 
	// name or the native name of the non-native app), finish setting it up and "resume" it
	if (err == KErrNone)
		{
		CleanupStack::PushL(TCleanupItem(CleanupOperation, &process));
		if (opaqueData!=NULL)
			{
			const_cast<CApaCommandLine&>(aCommandLine).SetOpaqueDataL(*opaqueData);
			}
		aCommandLine.SetProcessEnvironmentL(process);
		if (aThreadId!=NULL)
			{
			GetThreadIdL(*aThreadId, process);
			}
		if (aRequestStatusForRendezvous!=NULL)
			{
			process.Rendezvous(*aRequestStatusForRendezvous);
			}
		// Note - must not leave between here and the end of this method because we only expect
		// the caller to wait on aRequestStatusForRendezvous if this method does not leave.
		if(aRequestStatusForRendezvous != NULL && *aRequestStatusForRendezvous != KRequestPending)
			{
			User::WaitForRequest(*aRequestStatusForRendezvous);
			User::Leave(aRequestStatusForRendezvous->Int()); // item on cleanupstack terminates and closes the process
			}
		else
			{
			process.Resume();	
			}
		CleanupStack::Pop(&process);	
		process.Close();
		}
	else // Couldn't create a process
		{
		if (Handle()==KNullHandle) // not connected to apparc
			{
			const TPtrC executableName(aCommandLine.ExecutableName());
			
			// Java applications have an empty ExecutableName, as they use the CApaCommandLine's
			// opaque-data attribute. Not connected to apparc so can't start a non-native app.
			// Therefore we should handle this case with KErrNotSupported rather than whatever
			// RProcess::Create returns when passed an empty descriptor.  
			if (executableName.Length() == 0)
				{
				User::Leave(KErrNotSupported);
				}
				
			// Otherwise return the error code from RProcess::Create.
			User::Leave(err);
			
			}
		else // Handle()!=KNullHandle
			{
			// we can only proceed with the remaining option if we're connected to the Apparc server	
			// as a last resort, try use the legacy file-recognizer framework to launch the application
			TThreadId dummyThreadId;
			TPckg<TThreadId> threadId((aThreadId!=NULL)? *aThreadId: dummyThreadId);
			const TInt opcode=(aThreadId==NULL)?
									EAppListServStartAppWithoutReturningThreadId:
									EAppListServStartAppReturningThreadId;
			TIpcArgs ipcArgs;
			aCommandLine.GetIpcArgsLC(ipcArgs);
			if (aThreadId!=NULL)
				{
				ipcArgs.Set(CApaCommandLine::EIpcFirstFreeSlot, &threadId);
				}
			User::LeaveIfError(SendReceiveWithReconnect(opcode, ipcArgs));
			if (aRequestStatusForRendezvous!=NULL)
				{
				User::RequestComplete(aRequestStatusForRendezvous,KErrNone);
				}
			CleanupStack::PopAndDestroy(); // the TIpcArgs
			}
		}
	CleanupStack::PopAndDestroy(&opaqueData);
	} //lint !e1762 Suppress member function could be made const
	
void RApaLsSession::CleanupOperation(TAny* aAny)
	{
	RProcess* activeProcess = reinterpret_cast<RProcess*>(aAny);	
	activeProcess->Terminate(KErrGeneral);
	activeProcess->Close();
	}
	
void RApaLsSession::GetThreadIdL(TThreadId& aThreadId, const RProcess& aProcess)
	{ // static
	TFullName fullName(aProcess.Name());
	_LIT(KCCMain,"::Main");
	fullName.Append(KCCMain);
	RThread thread;
	User::LeaveIfError(thread.Open(fullName,EOwnerThread));
	aThreadId=thread.Id();
	thread.Close();
	}

void RApaLsSession::DeletePointerToPointerToTAny(TAny* aPointerToPointerToTAny)
	{ // static
	__ASSERT_ALWAYS(aPointerToPointerToTAny, Panic(EPanicNullPointer));
	delete *static_cast<TAny**>(aPointerToPointerToTAny); //lint !e613 Suppress possible use of null pointer
	} //lint !e818 Suppress pointer parameter could be declared as pointing to const

/**
Sets up slots 0 and 1 in aIpcArgs - assumes that slots 2 and 3 have already been set up; 
it then invokes aOpcode
*/
TInt RApaLsSession::GetExecutableName(TDes& aNativeExecutableName, TDes& aLogicalExecutableName, HBufC8*& aOpaqueData, TIpcArgs& aIpcArgs, TInt aOpcode) const
	{
	aNativeExecutableName.SetLength(0); // if this comes out zero-length from EAppListServExecutableNameForXxx below then use the logicalExecutableName in RProcess::Create (in this case it's a native C++ application)
	aIpcArgs.Set(0, &aNativeExecutableName);
	aIpcArgs.Set(1, &aLogicalExecutableName);
	const TInt lengthOfOpaqueData=SendReceiveWithReconnect(aOpcode, aIpcArgs);
	if (lengthOfOpaqueData<0)
		{
		return lengthOfOpaqueData; // it's an error code
		}
	if (lengthOfOpaqueData>0)
		{
		return GetOpaqueData(aOpaqueData, lengthOfOpaqueData);
		}
	aOpaqueData=NULL;
	return KErrNone;
	}

TInt RApaLsSession::GetOpaqueData(HBufC8*& aOpaqueData, TInt aLengthOfOpaqueData) const
	{
	HBufC8* const opaqueData=HBufC8::New(aLengthOfOpaqueData);
	if (opaqueData==NULL)
		{
		return KErrNoMemory;
		}
	TPtr8 opaqueData_asWritable(opaqueData->Des());
	const TInt error=SendReceiveWithReconnect(EAppListServGetOpaqueData, TIpcArgs(&opaqueData_asWritable));
	if (error!=KErrNone)
		{
		delete opaqueData;
		return error;
		}
	aOpaqueData=opaqueData;
	return error;
	}

TInt RApaLsSession::StartApplicationPassingFileHandle(const TDesC& aNativeExecutableName, const TDesC& aLogicalExecutableName, const TDesC8* aOpaqueData, const RFile& aFile, TThreadId& aThreadId, TRequestStatus* aRequestStatusForRendezvous)
	{
	TFileName documentName;
	aFile.FullName(documentName);
	CApaCommandLine* commandLine=NULL;	
	TRAPD(error, commandLine=CApaCommandLine::NewLC();
			commandLine->SetCommandL(EApaCommandOpen);
			commandLine->SetExecutableNameL(aLogicalExecutableName);
			commandLine->SetFileByHandleL(aFile);
			commandLine->SetDocumentNameL(documentName);
			if (aOpaqueData!=NULL)
				{
				commandLine->SetOpaqueDataL(*aOpaqueData);
				}
			StartApplicationL(aNativeExecutableName, *commandLine, aThreadId, aRequestStatusForRendezvous);
			CleanupStack::PopAndDestroy(commandLine));
	return error;
	}
	
TInt RApaLsSession::StartApplicationPassingDocumentName(const TDesC& aNativeExecutableName, const TDesC& aLogicalExecutableName, const TDesC8* aOpaqueData, const TDesC& aDocumentName, TThreadId& aThreadId,TApaCommand aCommand, TRequestStatus* aRequestStatusForRendezvous)
	{
	CApaCommandLine* commandLine=NULL;
	TRAPD(error, commandLine=CApaCommandLine::NewLC();
			commandLine->SetCommandL(aCommand);
			commandLine->SetExecutableNameL(aLogicalExecutableName);
			commandLine->SetDocumentNameL(aDocumentName);
			if (aOpaqueData!=NULL)
				{
				commandLine->SetOpaqueDataL(*aOpaqueData);
				}
			StartApplicationL(aNativeExecutableName, *commandLine, aThreadId, aRequestStatusForRendezvous);
			CleanupStack::PopAndDestroy(commandLine));
	return error;
	}

void RApaLsSession::StartApplicationL(const TDesC& aNativeExecutableName, const CApaCommandLine& aCommandLine, TThreadId& aThreadId, TRequestStatus* aRequestStatusForRendezvous)
	{ 
	RProcess process;
	TPtrC actualNativeExecutableName(aNativeExecutableName);
	if (actualNativeExecutableName.Length()==0)
		{
		actualNativeExecutableName.Set(aCommandLine.ExecutableName()); // it's a native C++ application, rather than a MIDlet, Python script, etc
		}

	if(Handle() != KNullHandle)
		{
		//requesting from rule-based plug-ins if we can run an application 
		TBool okayToRun = SendReceiveWithReconnect(EAppListServRuleBasedLaunching, TIpcArgs(&actualNativeExecutableName));
		//If server fails while requested rule-based plug-ins it returns a negative value. 
		//We shall proceed with launching an application in this case.
		if(!okayToRun)
			{
			User::Leave(KErrCancel); 
			}
		}

	TUidType uidType(KNullUid, KNullUid, KNullUid);
	User::LeaveIfError(process.CreateWithStackOverride(actualNativeExecutableName, KNullDesC, uidType, MinApplicationStackSize(), EOwnerProcess)); // RProcess::CreateWithStackOveride - TESTED
		
	CleanupClosePushL(process);
	aCommandLine.SetProcessEnvironmentL(process);
	GetThreadIdL(aThreadId, process);
	if (aRequestStatusForRendezvous!=NULL)
		{
		process.Rendezvous(*aRequestStatusForRendezvous);
		}
	process.Resume();
	CleanupStack::PopAndDestroy(&process);
	} //lint !e1762 Suppress member function could be made const



/** Gets the data (MIME) type for data taken from a file with a specified name.

@param aName The full filename, including drive and path, of the file containing the data.
@param aBuffer A buffer containing data taken from the specified file; typically 
the data is read from the beginning of the file.
@param aDataType On return, contains the result of the attempt to recognize 
data. 
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
*/
EXPORT_C TInt RApaLsSession::RecognizeData(const TDesC& aName, const TDesC8& aBuffer, TDataRecognitionResult& aDataType) const
	{
	TPckg<TDataRecognitionResult> result(aDataType);
	return SendReceiveWithReconnect(EAppListServRecognizeData,TIpcArgs(&result, &aName, &aBuffer));
	} //lint !e1764: Suppress reference parameter could be declared const ref

/** Gets the data (MIME) type for data in a file passed by handle.

@param aFile The file containing the data. Before this function can be called,
the file server session which owns this file handle must first be marked as shareable by 
calling RFs::ShareProtected().
@param aDataType On return, contains the result of the attempt to recognize 
data. 
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. */
EXPORT_C TInt RApaLsSession::RecognizeData(const RFile& aFile, TDataRecognitionResult& aDataType) const
	{
	TPckg<TDataRecognitionResult> result(aDataType);
	TIpcArgs ipcArgs(&result);
	const TInt error = aFile.TransferToServer(ipcArgs, 1, 2);
	if (error != KErrNone)
		{
		return error;
		}
	return SendReceiveWithReconnect(EAppListServRecognizeDataPassedByFileHandle,ipcArgs);
	} //lint !e1764 Suppress reference parameter 'aDataType' could be declared const ref



/** Tests whether data taken from a named file has the specified 
data (MIME) type.

@param aName The name of the file containing the data.
@param aBuffer A buffer containing data taken from the specified file; typically 
the data is read from the beginning of the file.
@param aDataType The data (MIME) type.
@param aResult On return, contains the result of the test.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
*/
EXPORT_C TInt RApaLsSession::RecognizeSpecificData(const TDesC& aName, const TDesC8& aBuffer, const TDataType& aDataType, TBool& aResult) const
	{
	const TPckgC<TDataType> dataType(aDataType);
	const TInt returnValue=SendReceiveWithReconnect(EAppListServRecognizeSpecificData,TIpcArgs(&dataType, &aName, &aBuffer));
	if (returnValue<0)
		{
		return returnValue;
		}
	aResult=returnValue;
	return KErrNone;
	}
	
EXPORT_C TInt RApaLsSession::RecognizeFilesL(const TDesC& aPath, CDataRecognitionResultArray& aResult) const
/** 
Gets the data (MIME) type for files in a specified directory. Because this call may take a long
time to complete, the asynchronous version is strongly recommended. Furthermore, it is not possible
to use this synchronous function while an asynchronous request is still active.
@publishedAll
@released
@param aPath A valid path. Note that the path must end with a backslash.
@param aResult If the call was successful, this parameter contains the recognition result.
@return KErrNone, if successful; otherwise one of the other system-wide error codes.
*/
	{
	return RecognizeFilesL(aPath, KAllDataTypes, aResult);
	}

EXPORT_C TInt RApaLsSession::RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult) const
/** 
Gets the data (MIME) type for files in a specified directory. Because this call may take a long
time to complete, the asynchronous version is strongly recommended. Furthermore, it is not possible
to use this synchronous function while an asynchronous request is still active.
@publishedAll
@released
@param aPath A valid path. Note that the path must end with a backslash.
@param aDataType A data type filter. Wildcards are allowed. For example, "text*" would also
add "text/plain" data types to the result.
@param aResult If the call was successful, this parameter contains the recognition result.
@return KErrNone, if successful; otherwise one of the other system-wide error codes.
*/
	{
	if(iExtension && iExtension->RecognitionActive())
		{
		return KErrInUse;
		}
	_LIT(KBackslash,"\\");
	if(aPath.Right(1) != KBackslash)
		{
		return KErrPathNotFound;
		}

	TPckgBuf<TUint> requiredBufferSizePckg;
	aResult.SetPath(aPath);
	TInt error = SendReceiveWithReconnect(EAppListServRecognizeFiles,TIpcArgs(&aPath,&requiredBufferSizePckg,&aDataType));
	if(KErrNone == error)
		{
		error=TransferAndInternalizeDataL(aPath, requiredBufferSizePckg(), aResult);
		}
	return error;

	}

EXPORT_C void RApaLsSession::RecognizeFilesL(const TDesC& aPath, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus)
/** 
Gets the data (MIME) type for files in a specified directory.
@publishedAll
@released
@param aPath A valid path. Note that the path must end with a backslash.
@param aResult If the call was successful, this parameter contains the recognition result.
@param aStatus A request status object.
*/
	{
	RecognizeFilesL(aPath,KAllDataTypes,aResult,aStatus);
	}

EXPORT_C void RApaLsSession::RecognizeFilesL(const TDesC& aPath, const TDesC8& aDataType, CDataRecognitionResultArray& aResult, TRequestStatus& aStatus)
/** 
Gets the data (MIME) type for files in a specified directory.
@publishedAll
@released
@param aPath A valid path. Note that the path must end with a backslash.
@param aDataType A data type filter. Wildcards are allowed. For example, "text*" would also
add "text/plain" data types to the result.
@param aResult If the call was successful, this parameter contains the recognition result.
@param aStatus A request status object
*/
	{
	if(iExtension == NULL)
		{
		iExtension = new (ELeave) CApaLsSessionExtension(*this);
		}
	iExtension->RecognizeFilesL(aPath, aDataType, aResult, aStatus);
	}

TInt RApaLsSession::TransferAndInternalizeDataL(const TDesC& aPath, const TInt aRequiredBufferSize, CDataRecognitionResultArray& aResult) const
/**
This function transfers the data to the client-side and "fills" the CDataRecognitionResultArray object. 
@internalComponent
*/
{
	CBufFlat* const buffer=CBufFlat::NewL(aRequiredBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0,aRequiredBufferSize);
	TPtr8 bufPtr=buffer->Ptr(0);

	// transfer recognition buffer
	TInt error = SendReceiveWithReconnect(EAppListServTransferRecognitionResult,TIpcArgs(&aPath,&bufPtr,aRequiredBufferSize));
	if(KErrNone == error)
		{
		error = aResult.InternalizeL(*buffer);
		}
	CleanupStack::PopAndDestroy(buffer);
	return error;
	}

EXPORT_C void RApaLsSession::CancelRecognizeFiles()
/** 
Cancels any outstanding asynchronous recognition requests.
@publishedAll
@released
*/
	{
	if(iExtension)
		{
		iExtension->CancelRecognizeFiles();
		}
	}

EXPORT_C void RApaLsSession::Close()
/** 
Closes the session. Needs to be called to avoid memory leaks.
@publishedAll
@released
*/
	{
	CancelRecognizeFiles();
	RHandleBase::Close();
	delete iExtension;
	iExtension = NULL;
	}

EXPORT_C TInt RApaLsSession::RecognizeSpecificData(const RFile& aFile, const TDataType& aDataType, TBool& aResult) const
/** Tests whether data taken from a file passed by handle has the specified 
data (MIME) type.

@param aFile The file containing the data. Before this function can be called,
the file server session which owns this file handle must first be marked as shareable by 
calling RFs::ShareProtected().
@param aDataType The data (MIME) type.
@param aResult On return, contains the result of the test.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. */
	{
	const TPckgC<TDataType> dataType(aDataType);
	TIpcArgs ipcArgs(&dataType);
	TInt returnValue = aFile.TransferToServer(ipcArgs, 1, 2);
	if (returnValue == KErrNone)
		{
		returnValue=SendReceiveWithReconnect(EAppListServRecognizeSpecificDataPassedByFileHandle,ipcArgs);
		}
	if (returnValue < KErrNone)
		{
		return returnValue;
		}
	aResult=returnValue;
	return KErrNone;
	}


/** Gets the UID of an application that can handle the specified data (MIME) type.

If no application can be found, the function returns the UID of the preferred 
default handler. If none of the default handlers can handle the data type, 
then a NULL UID is returned.

@param aDataType The data (MIME) type.
@param aAppUid On return, the UID of the application that can handle the data 
(MIME) type; this may be NULL.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
*/
EXPORT_C TInt RApaLsSession::AppForDataType(const TDataType& aDataType, TUid& aAppUid) const
	{
	const TPckgC<TDataType> dataType(aDataType);
	TPckg<TUid> uid(aAppUid);
	return SendReceiveWithReconnect(EAppListServAppForDataType,TIpcArgs(&dataType,&uid));
	} //lint !e1764 Suppress reference parameter 'aAppUid' could be declared const ref


/** Finds and launches an application to handle the document contained in the specified 
file.

@param aFileName The document name.
@param aThreadId On return, the id of the main thread started.
@param aLaunchType Not used. Reserved for future use.
@return KErrNone, if successful; EAppListInvalid, if the server's initial population of 
the list has not completed; KErrNotFound, if a matching entry could not be found; otherwise 
one of the other system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::StartDocument(const TDesC& aFileName, TThreadId& aThreadId, TLaunchType /*aLaunchType*/)
	{
	HBufC8* buffer=NULL;
	TInt error=GetBufferFromFile(buffer, aFileName);
	if (error!=KErrNone)
		{
		return error;
		}
	TFileName nativeExecutableName; // the name of the EXE that we pass to RProcess::Create
	TFileName logicalExecutableName; // the name of the MIDlet, Python script, etc
	HBufC8* opaqueData=NULL;
	TIpcArgs ipcArgs;
	ipcArgs.Set(2, &aFileName);
	ipcArgs.Set(3, buffer);
	error=GetExecutableName(nativeExecutableName, logicalExecutableName, opaqueData, ipcArgs, EAppListServGetExecutableNameGivenDocument);
	if (error!=KErrNone)
		{
		delete buffer;
		return error;
		}
	error = StartApplicationPassingDocumentName(nativeExecutableName, logicalExecutableName, opaqueData, aFileName, aThreadId, EApaCommandOpen, NULL);
	delete opaqueData;
	if(error!=KErrNone && error!=KErrCancel)
		{
		TPckg<TThreadId> threadId(aThreadId);
		error = SendReceiveWithReconnect(EAppListServStartDocument,TIpcArgs(&threadId, &aFileName, buffer));
		}
	delete buffer;
	return error;
	}

EXPORT_C TInt RApaLsSession::StartDocument(RFile& aFile, TThreadId& aThreadId, TRequestStatus* aRequestStatusForRendezvous/*=NULL*/)
/** Finds and launches an application to handle the document contained in the specified file

@param aFile  The file handle. Before this function can be called,
the file server session which owns this file handle must first be marked as shareable by 
calling RFs::ShareProtected().
@param aThreadId On return, the id of the main thread started.
@param aRequestStatusForRendezvous If not NULL, the asynchronous RProcess::Rendezvous() 
function is called (being passed this TRequestStatus object) before RProcess::Resume() is called on 
the new application process.
@return KErrNone, if successful; KErrNotFound, if no suitable application can 
be found; otherwise one of the other system-wide error codes. */
	{
	TFileName nativeExecutableName; // the name of the EXE that we pass to RProcess::Create
	TFileName logicalExecutableName; // the name of the MIDlet, Python script, etc
	HBufC8* opaqueData=NULL;
	TIpcArgs ipcArgs;
	TInt error=aFile.TransferToServer(ipcArgs, 2, 3);
	if (error == KErrNone)
		{
		error=GetExecutableName(nativeExecutableName, logicalExecutableName, opaqueData, ipcArgs, EAppListServGetExecutableNameGivenDocumentPassedByFileHandle);
		}
	if (error!=KErrNone)
		{
		return error;
		}
	error=StartApplicationPassingFileHandle(nativeExecutableName, logicalExecutableName, opaqueData, aFile, aThreadId, aRequestStatusForRendezvous);
	delete opaqueData;
	return error;
	} //lint !e1764 Suppress reference parameter could be declared const ref


/** Launches an application that can handle the specified data (MIME) type.

The application handles the document contained in the specified file.

@param aFileName The document name.
@param aDataType The data (MIME) type.
@param aThreadId On return, the id of the main thread started.
@param aLaunchType Not used. Reserved for future use.
@return KErrNone, if successful; EAppListInvalid if the server's initial population of 
the list has not completed; KErrNotFound, if no suitable application can 
be found; otherwise one of the other system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::StartDocument(const TDesC& aFileName, const TDataType& aDataType, TThreadId& aThreadId, TLaunchType /*aLaunchType*/)
	{
	TFileName nativeExecutableName; // the name of the EXE that we pass to RProcess::Create
	TFileName logicalExecutableName; // the name of the MIDlet, Python script, etc
	HBufC8* opaqueData=NULL;
	const TPckgC<TDataType> dataType(aDataType);
	TIpcArgs ipcArgs;
	ipcArgs.Set(2, &dataType);
	TInt error=GetExecutableName(nativeExecutableName, logicalExecutableName, opaqueData, ipcArgs, EAppListServGetExecutableNameGivenDataType);
	if (error!=KErrNone)
		{
		return error;
		}
	error = StartApplicationPassingDocumentName(nativeExecutableName, logicalExecutableName, opaqueData, aFileName, aThreadId,EApaCommandOpen,NULL);
	delete opaqueData;
	if(error!=KErrNone && error!=KErrCancel)
		{
		TPckg<TThreadId> threadId(aThreadId);
		error = SendReceiveWithReconnect(EAppListServStartDocumentByDataType,TIpcArgs(&threadId, &aFileName, &dataType));
		}
	return error;
	}

EXPORT_C TInt RApaLsSession::StartDocument(RFile& aFile, const TDataType& aDataType, TThreadId& aThreadId, TRequestStatus* aRequestStatusForRendezvous/*=NULL*/)
/** Finds and launches an application to handle the document contained in the specified file

@param aFile The file handle.
@param aDataType The data (MIME) type.
@param aThreadId On return, the id of the main thread started.
@param aRequestStatusForRendezvous If not NULL, the asynchronous RProcess::Rendezvous() 
function is called (being passed this TRequestStatus object) before RProcess::Resume() is called on 
the new application process.
@return KErrNone, if successful; KErrNotFound, if no suitable application can 
be found; otherwise one of the other system-wide error codes. */
	{
	TFileName nativeExecutableName; // the name of the EXE that we pass to RProcess::Create
	TFileName logicalExecutableName; // the name of the MIDlet, Python script, etc
	HBufC8* opaqueData=NULL;
	const TPckgC<TDataType> dataType(aDataType);
	TIpcArgs ipcArgs;
	ipcArgs.Set(2, &dataType);
	TInt error=GetExecutableName(nativeExecutableName, logicalExecutableName, opaqueData, ipcArgs, EAppListServGetExecutableNameGivenDataType);
	if (error!=KErrNone)
		{
		return error;
		}
	error=StartApplicationPassingFileHandle(nativeExecutableName, logicalExecutableName, opaqueData, aFile, aThreadId, aRequestStatusForRendezvous);
	delete opaqueData;
	return error;
	} //lint !e1764 Suppress reference parameter could be declared const ref

EXPORT_C TInt RApaLsSession::StartDocument(const TDesC& aFileName, TUid aAppUid, TThreadId& aThreadId, TLaunchType aLaunchType)
/** Launches the application identified by the specified UID.

The application handles the document contained in the specified file.

@param aFileName The document name.
@param aAppUid The application specific UID.
@param aThreadId On return, the id of the main thread started.
@param aLaunchType Not used. Reserved for future use.
@return KErrNone, if successful; EAppListInvalid if the server's initial population of 
the list has not completed; KErrNotFound, if no suitable application can 
be found; otherwise one of the other system-wide error codes. */
	{
	TFileName nativeExecutableName; // the name of the EXE that we pass to RProcess::Create
	TFileName logicalExecutableName; // the name of the MIDlet, Python script, etc
	HBufC8* opaqueData=NULL;
	TIpcArgs ipcArgs;
	ipcArgs.Set(2, aAppUid.iUid);
	TInt error=GetExecutableName(nativeExecutableName, logicalExecutableName, opaqueData, ipcArgs, EAppListServGetExecutableNameGivenAppUid);
	if (error!=KErrNone)
		{
		return error;
		}
	error = StartApplicationPassingDocumentName(nativeExecutableName, logicalExecutableName, opaqueData, aFileName, aThreadId,EApaCommandOpen,NULL);
	delete opaqueData;
	if(error!=KErrNone && error!=KErrCancel)
		{
		error = StartAndCreate(EAppListServStartDocumentByUid,aFileName,aAppUid,aThreadId,aLaunchType);
		}
	return error;
	}

EXPORT_C TInt RApaLsSession::StartDocument(RFile& aFile, TUid aAppUid, TThreadId& aThreadId, TRequestStatus* aRequestStatusForRendezvous/*=NULL*/)
/** Finds and launches an application to handle the document contained in the specified file

@param aFile The file handle.
@param aAppUid The application specific UID.
@param aThreadId On return, the id of the main thread started.
@param aRequestStatusForRendezvous If not NULL, the asynchronous RProcess::Rendezvous() 
function is called (being passed this TRequestStatus object) before RProcess::Resume() is called on 
the new application process.
@return KErrNone, if successful; KErrNotFound, if no suitable application can 
be found; otherwise one of the other system-wide error codes. */
	{
	TFileName nativeExecutableName; // the name of the EXE that we pass to RProcess::Create
	TFileName logicalExecutableName; // the name of the MIDlet, Python script, etc
	HBufC8* opaqueData=NULL;
	TIpcArgs ipcArgs;
	ipcArgs.Set(2, aAppUid.iUid);
	TInt error=GetExecutableName(nativeExecutableName, logicalExecutableName, opaqueData, ipcArgs, EAppListServGetExecutableNameGivenAppUid);
	if (error!=KErrNone)
		{
		return error;
		}
	error=StartApplicationPassingFileHandle(nativeExecutableName, logicalExecutableName, opaqueData, aFile, aThreadId, aRequestStatusForRendezvous);
	delete opaqueData;
	return error;
	} //lint !e1764 Suppress reference parameter could be declared const ref


/** Launches the application identified by the specified UID and creates a new document.

To create a document file with the passed document name, the application needs to override the 3-parameter
overload of ProcessCommandParametersL() to call the 2-parameter overload.

Otherwise, a document will be created with the default document name present in the application resource file.
If default document name is empty or not provided, no document is created.

If the application resource file is not present, a document with application caption name is created.

@param aFileName The document name.
@param aAppUid The application specific UID. 
@param aThreadId On return, the id of the main thread started.
@param aLaunchType Not used. Reserved for future use.
@return KErrNone, if successful; EAppListInvalid if the server's initial population of 
the list has not completed; KErrNotFound, if no suitable application can 
be found; otherwise one of the other system-wide error codes. 
@see CEikAppUi::ProcessCommandParametersL().
*/
EXPORT_C TInt RApaLsSession::CreateDocument(const TDesC& aFileName, TUid aAppUid, TThreadId& aThreadId, TLaunchType aLaunchType)
	{
	TFileName nativeExecutableName; // the name of the EXE that we pass to RProcess::Create
	TFileName logicalExecutableName; // the name of the MIDlet, Python script, etc
	HBufC8* opaqueData=NULL;
	TIpcArgs ipcArgs;
	ipcArgs.Set(2, aAppUid.iUid);
	TInt error=GetExecutableName(nativeExecutableName, logicalExecutableName, opaqueData, ipcArgs, EAppListServGetExecutableNameGivenAppUid);
	if (error!=KErrNone)
		{
		return error;
		}
	error = StartApplicationPassingDocumentName(nativeExecutableName, logicalExecutableName, opaqueData, aFileName, aThreadId,EApaCommandCreate,NULL);
	delete opaqueData;
	if(error!=KErrNone && error!=KErrCancel)
		{
		error = StartAndCreate(EAppListServCreateDocumentByUid,aFileName,aAppUid,aThreadId,aLaunchType);
		}
	return error;
	}
	
TInt RApaLsSession::StartAndCreate(TInt aRqst,const TDesC& aFileName, TUid aAppUid, TThreadId& aThreadId, TLaunchType /*aLaunchType*/)
	{
	TPckg<TThreadId> threadId(aThreadId);
	return SendReceiveWithReconnect(aRqst,TIpcArgs(&threadId, &aFileName, aAppUid.iUid));
	} 	//lint !e1762 !e1764
		//Suppress reference parameter 'aThreadId' could be declared const ref
		//Suppress member function could be made const




/** Gets the available icon sizes for the application with the specified UID.

@param aAppUid The application specific UID.
@param aArrayToFill On return, the array contains all of the available icon 
sizes.
@return KErrNone, if successful; KErrNotFound, if no matching application can 
be found; KErrNotSupported, if the application provides icons in non-MBM format;
otherwise one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAppIconSizes(TUid aAppUid, CArrayFixFlat<TSize>& aArrayToFill) const
	{
	TRAPD(error,DoGetAppIconSizesL(aAppUid,aArrayToFill));
	return error;
	}

void RApaLsSession::FetchArrayL(MArrayFiller& aArrayFiller,TUid aAppUid, TInt aOpcode, TInt aInitialBufSize) const
	{
	CBufFlat* buffer=CBufFlat::NewL(aInitialBufSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0,aInitialBufSize);
	TPtr8 bufPtr=buffer->Ptr(0);
	const TInt sizeRequired=User::LeaveIfError(SendReceiveWithReconnect(aOpcode,TIpcArgs(aAppUid.iUid,buffer->Size(),&bufPtr)));
	if (sizeRequired>0)
		{
		__ASSERT_DEBUG(sizeRequired>buffer->Size(),User::Invariant());
		CleanupStack::PopAndDestroy(buffer);
		buffer=CBufFlat::NewL(sizeRequired);
		CleanupStack::PushL(buffer);
		buffer->ExpandL(0,sizeRequired);
		bufPtr.Set(buffer->Ptr(0));
#if defined(_DEBUG)
		const TInt check=
#endif
		User::LeaveIfError(SendReceiveWithReconnect(aOpcode,TIpcArgs(aAppUid.iUid,buffer->Size(),&bufPtr)));
		__ASSERT_DEBUG(check==0,User::Invariant());
		}
	RBufReadStream readStream;
	readStream.Open(*buffer);
	const TInt count=readStream.ReadUint32L();
	for (TInt i=0; i<count; ++i)
		{
		aArrayFiller.AppendItemL(readStream);
		}
	CleanupStack::PopAndDestroy(buffer);
	}

void RApaLsSession::DoGetAppIconSizesL(TUid aAppUid, CArrayFixFlat<TSize>& aArrayToFill) const
	{
	TSizeArrayFiller arrayFiller(aArrayToFill);
	FetchArrayL(arrayFiller, aAppUid, EAppListServGetAppIconSizes, KInitialIconBufSize);
	}



/** Gets the nearest matching application icon for the application with the specified 
UID.

The function gets the icon whose size matches the specified size. If there 
is no exact match, then the function gets the closest smaller icon.

This function should be used in preference to the TInt GetAppIcon(TUid,TInt,CApaMaskedBitmap&); 
overload.

@param aAppUid The application specific UID.
@param aSize The required size of the icon.
@param aAppBitmap On return, the application icon.
@return KErrNone, if successful; KErrNotFound, if a matching entry could not be found, or no 
icon equal to or smaller than the specified size can be found; KErrNotSupported, if the
application provides icons in non-MBM format; otherwise one of the other system wide
error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAppIcon(TUid aAppUid, TSize aSize, CApaMaskedBitmap& aAppBitmap) const
	{
	SReturnData_AppIconByUidAndSize returnData = {0,0};
	TPckg<SReturnData_AppIconByUidAndSize> returnData_asDescriptor(returnData);
	TInt error=SendReceiveWithReconnect(EAppListServAppIconByUidAndSize,TIpcArgs(aAppUid.iUid,aSize.iWidth,aSize.iHeight,&returnData_asDescriptor));
	if (error==KErrNone)
		{
		error=aAppBitmap.Duplicate(returnData.iIcon);
		if (error==KErrNone)
			{
			error=aAppBitmap.Mask()->Duplicate(returnData.iIconMask);
			}
		}
	return error;
	}



/** 
 gets the bitmap handles from the Server, forms these up into a CApaMaskedBitmap

Sets aAppBitmap to be the small, medium or large app icon of the app with uid 
aAppUid, when aSize=0, 1 or 2 respectively.
Panics the caller if a different index is specified.
The overload which takes a TSize should be used instead.

@deprecated 
@param aAppUid The application specific UID.
@param aSize The required size of the icon.
@param aAppBitmap On return, the application icon.
@return KErrNone, if successful; KErrNotFound, if a matching entry could not be found, or no 
icon equal to or smaller than the specified size can be found; KErrNotSupported, if the 
application provides icons in non-MBM format, otherwise one of the other system wide 
error codes.
*/
EXPORT_C TInt RApaLsSession::GetAppIcon(TUid aAppUid, TInt aSize, CApaMaskedBitmap& aAppBitmap) const
	{
	__ASSERT_ALWAYS((aSize>=0) && (aSize<3), Panic(EDPanicBadIconSize));
	SReturnData_AppIconByUid returnData = {0,0};
	TPckg<SReturnData_AppIconByUid> returnData_asDescriptor(returnData);
	TInt error=SendReceiveWithReconnect(EAppListServAppIconByUid,TIpcArgs(aAppUid.iUid,aSize,&returnData_asDescriptor));
	if (error==KErrNone)
		{
		error=aAppBitmap.Duplicate(returnData.iIcon);
		if (error==KErrNone)
			{
			error=aAppBitmap.Mask()->Duplicate(returnData.iIconMask);
			}
		}
	return error;
	}


	
EXPORT_C TInt RApaLsSession::GetAppIcon(TUid aAppUid, RFile& aFile) const 
/** Gets an open shareable read only file handle to the application icon file for the 
application with the specified UID. 

An icon file can only be defined by applications providing an application registration file.

An icon file may be in any graphics format and contain one or more icons.

On entering this function, aFile must be non-open. It is recommended that aFile is 
pushed onto the cleanup-stack (via CleanupClosePushL()) before this function is called.

@param aAppUid The application specific UID.
@param aFile On return, a read only open file handle to the icon file.
@return KErrNone, if successful; KErrNotFound, if a matching application could not be found, 
or an icon filename was not defined; otherwise one of the other system wide error codes. 
@see GetAppIcon 
*/ 
	{
	__ASSERT_ALWAYS(aFile.SubSessionHandle() == KNullHandle, Panic(EDPanicHandleAlreadySet));
	TPckgBuf<TInt> fileHandle;
	TInt sessionHandleOrErrorCode = SendReceiveWithReconnect(EAppListServAppIconFileHandle, TIpcArgs(aAppUid.iUid, &fileHandle));
	if (sessionHandleOrErrorCode >= KErrNone)
	    {
	    sessionHandleOrErrorCode = aFile.AdoptFromServer(sessionHandleOrErrorCode, fileHandle());
	    }
	return sessionHandleOrErrorCode;
	}   



EXPORT_C TInt RApaLsSession::AppForDocument(const TDesC& aFileName, TUid& aAppUid, TDataType& aDataType) const
/** Gets the data (MIME) type of the data in the specified file and gets the UID 
of an application that can handle this type.

@param aFileName The name of the file containing the data.
@param aAppUid On return, the UID of the application that can handle the data 
(MIME) type; this may be NULL.
@param aDataType On return, the data (MIME) type.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. */
	{
	return DoAppForDocumentOptionallySpecifyingService(aFileName, TUid::Null(), aAppUid, aDataType, EAppListServAppForDocument);
	}


/** Gets the data (MIME) type of the data in the specified file and gets the UID 
of an application that can handle this type.

@param aFile The file containing the data. Before this function can be called,
the file server session which owns this file handle must first be marked as shareable by 
calling RFs::ShareProtected().
@param aAppUid On return, the UID of the application that can handle the data 
(MIME) type; this may be NULL.
@param aDataType On return, the data (MIME) type.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
*/
EXPORT_C TInt RApaLsSession::AppForDocument(const RFile& aFile, TUid& aAppUid, TDataType& aDataType) const
	{
	return DoAppForDocumentOptionallySpecifyingService(aFile, TUid::Null(), aAppUid, aDataType, EAppListServAppForDocumentPassedByFileHandle);
	}

/** Tests whether the file is a native executable (DLL or EXE).

@param aFileName The name of the file containing the data.
@param aProgram On return, true, if the file contains application code; false, 
otherwise.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
*/
EXPORT_C TInt RApaLsSession::IsProgram(const TDesC& aFileName, TBool& aProgram) const
	{
	_LIT(KLitSysBin, "\\sys\\bin\\");
	aProgram=(TParsePtrC(aFileName).Path().CompareF(KLitSysBin)==0);
	return KErrNone;
	}

/** Gets the confidence threshold for successful data recognition.

This is the minimum acceptable confidence level that must be reported by a 
data recognizer for data to be accepted as of a given type.

@param aConfidence On return, the confidence threshold. 
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
*/
EXPORT_C TInt RApaLsSession::GetAcceptedConfidence(TInt& aConfidence) const
	{
	TPckg<TInt> confidence(aConfidence);
	return SendReceiveWithReconnect(EAppListServGetConfidence,TIpcArgs(&confidence));
	} //lint !e1764 Suppress reference parameter could be declared const ref



/** Sets the confidence threshold for successful data recognition.

This is the minimum acceptable confidence level that must be reported by a 
data recognizer for data to be accepted as of a given type.

@param aConfidence The confidence threshold. Although this is an integer value, 
data recognizers use the discrete values defined by the CApaDataRecognizerType::TRecognitionConfidence 
enumeration.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
@capability WriteDeviceData 
*/
EXPORT_C TInt RApaLsSession::SetAcceptedConfidence(TInt aConfidence)
	{
	return SendReceiveWithReconnect(EAppListServSetConfidence,TIpcArgs(aConfidence));
	} //lint !e1762 Suppress member function could be made const


EXPORT_C TInt RApaLsSession::GetPreferredBufSize(TInt& aPreferredBufSize) const
/** Gets the preferred number of bytes of data to read from a file for the purpose 
of recognizing the data type.

This should be used to determine the size of buffer to pass to the 3-parameter 
overload of RecognizeData() or to the 4-parameter overload of RecognizeSpecificData().

@param aPreferredBufSize On return, contains either the largest buffer size required 
by any of the currently installed data-recognizers, or the value that would be 
returned by GetMaxDataBufSize(), whichever is less.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes.
@see GetMaxDataBufSize() */
	{
	const TInt preferredBufSize=SendReceiveWithReconnect(EAppListServPreferredBufSize, TIpcArgs());
	if (preferredBufSize<KErrNone)
		{
		return preferredBufSize; // it's an error
		}
	aPreferredBufSize=preferredBufSize;
	return KErrNone;
	}

EXPORT_C TInt RApaLsSession::GetMaxDataBufSize(TInt& aBufSize) const
/** Gets the maximum size of the data that can be read from a file for the purpose 
of recognizing the data type.

To determine the size of buffer to pass to the 3-parameter overload of RecognizeData() 
or to the 4-parameter overload of RecognizeSpecificData(), use GetPreferredBufSize() 
rather than this function.

@param aBufSize On return, contains the maximum size.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes.
@see SetMaxDataBufSize()
@see GetPreferredBufSize() */
	{
	const TInt returnValue=SendReceiveWithReconnect(EAppListServGetBufSize,TIpcArgs());
	if (returnValue<0)
		{
		return returnValue;
		}
	aBufSize=returnValue;
	return KErrNone;
	}



/** Sets the maximum size of the data that can be read from a file for the purpose 
of recognizing the data type.

The value is not used when the client explicitly supplies a buffer, for example 
in calls to RecognizeData() and RecognizeSpecificData(), but is used in the 
implementation of functions such as StartDocument() and CreateDocument().

Unless explicitly set, a default value of KApaAppListServMaxBuffer is used.

@param aBufSize The maximum size of data to be read.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
@see CreateDocument()
@see StartDocument()
@see RecognizeData()
@see RecognizeSpecificData() 
*/
EXPORT_C TInt RApaLsSession::SetMaxDataBufSize(TInt aBufSize)
	{
	return SendReceiveWithReconnect(EAppListServSetBufSize,TIpcArgs(aBufSize));
	} //lint !e1762 Suppress member function could be made const



/** Gets a list of recognized data(MIME) types by all recognizers.

@param aDataTypes The array of data (MIME) types.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
*/
EXPORT_C TInt RApaLsSession::GetSupportedDataTypesL(CDataTypeArray& aDataTypes) const
	{
	// gets the datatype count in terms of buffer length; negative value means one of the system-wide error
	TInt ret=SendReceiveWithReconnect(EAppListServGetDataTypesPhase1,TIpcArgs());
	if (ret>0)
		{
		CBufFlat* const buf=CBufFlat::NewL(ret);
		CleanupStack::PushL(buf);
		buf->ExpandL(0,ret);
		TPtr8 ptr=buf->Ptr(0);
		ret=SendReceiveWithReconnect(EAppListServGetDataTypesPhase2,TIpcArgs(&ptr));
		if (ret==KErrNone)
			{
			RBufReadStream readStream(*buf);
			readStream >> aDataTypes;
			}
		CleanupStack::PopAndDestroy(buf);
		}
	return ret;
	}



/** Gets the icon for the specified view published by the application that has 
the specified UID.

The icon returned is that which is closest in size to the specified size.

@param aAppUid The application specific UID.
@param aViewUid The UID identifying the view.
@param aSize The requested size of the icon.
@param aViewBitmap On return, the icon that is closest in size to the requested 
size.
@return KErrNone, if successful; KErrNotFound, if no matching application nor matching view 
can be found; KErrNotSupported, if the application provides icons in non-MBM format;
otherwise one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAppViewIcon(TUid aAppUid, TUid aViewUid, const TSize& aSize, CApaMaskedBitmap& aViewBitmap) const
	{
	const TApaAppViewIconSizeData appViewIconSizeData(aAppUid, aViewUid, aSize);
	const TPckgC<TApaAppViewIconSizeData> inputData(appViewIconSizeData);
	SReturnData_ViewIconByUidAndSize returnData = {0,0};
	TPckg<SReturnData_ViewIconByUidAndSize> returnData_asDescriptor(returnData);
	TInt error=SendReceiveWithReconnect(EAppListServViewIconByUidAndSize,TIpcArgs(&inputData,&returnData_asDescriptor));
	if (error==KErrNone)
		{
		error=aViewBitmap.Duplicate(returnData.iIcon);
		if (error==KErrNone)
			{
			error=aViewBitmap.Mask()->Duplicate(returnData.iIconMask);
			}
		}
	return error;
	}



/** Gets the views published by the application that has the specified UID.

Information on each view is contained in a TApaAppViewInfo object, and this 
set of objects is put into the array supplied by the caller.

@param aAppViews On return, the array contains information on all of the views 
published by the specified application.
@param aAppUid The application specific UID.
@return KErrNone, if successful; KErrNotFound, if no matching application can 
be found; otherwise one of the other system wide error codes.
@see TApaAppViewInfo 
*/
EXPORT_C TInt RApaLsSession::GetAppViews(CApaAppViewArray& aAppViews, TUid aAppUid) const
	{
	TRAPD(error,DoGetAppViewsL(aAppViews,aAppUid));
	return error;
	}

void RApaLsSession::DoGetAppViewsL(CApaAppViewArray& aArrayToFill, TUid aAppUid) const
	{
	TViewDataArrayFiller arrayFiller(aArrayToFill);
	FetchArrayL(arrayFiller, aAppUid, EAppListServGetAppViews, KInitialViewDataBufSize);
	}



/** Gets the list of file names for which the application with the specified 
UID claims ownership.

The list is written to a descriptor array supplied by the caller.

Note that if the function fails due to lack of memory, the array is left in 
an undefined state.

@param aAppOwnedFiles On return, the descriptor array contains the file names.
@param aAppUid The application specific UID.
@return KErrNone, if successful; KErrNotFound, if no matching application can 
be found; otherwise one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAppOwnedFiles(CDesCArray& aAppOwnedFiles, TUid aAppUid) const
	{
	TRAPD(error,DoGetAppOwnedFilesL(aAppOwnedFiles,aAppUid));
	return error;
	}



/** Gets the number of icons defined by the app that has the specified UID

Applications that don't define icons in their application information file will
return an aCount value of zero when this function is called.

@param aAppUid The application specific UID
@param aCount On return, contains the number of icons defined by the application
@return KErrNone, if successful; KErrNotFound, if a matching application could not be found;
KErrNotSupported, if the application provides icons in non-MBM format; otherwise one of 
the other system-wide error codes. 
*/
EXPORT_C TInt RApaLsSession::NumberOfOwnDefinedIcons(TUid aAppUid, TInt& aCount) const
	{
	TPckgBuf<TInt> pckg;
	TInt ret=SendReceiveWithReconnect(EAppListServNumberOfOwnDefinedIcons,TIpcArgs(aAppUid.iUid,&pckg));
	if (ret==KErrNone)
		{
		aCount=pckg();
		}
	return ret;
	}



/** Gets the full filename of a file containing application icons for the
application with the specified UID.

An icon file can only be defined by applications providing an application registration file.

An icon file may be in any graphics format and contain one or more icons.

@param aAppUid The application specific UID.
@param aFullFileName On return, the full filename of a file containing one or more
application icons. Returns a pointer to the filename and transfers ownership to the caller.
@return KErrNone, if successful; KErrNotFound, if a matching application could not be found,
or an icon filename was not defined; KErrNotSupported, if the application does not
provide an application registration file; otherwise one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAppIcon(TUid aAppUid, HBufC*& aFullFileName) const
	{
	TFileName fileName;
	TPckg<TFileName> filenamePckg(fileName);
	TInt ret=SendReceiveWithReconnect(EAppListServAppIconFileName, TIpcArgs(aAppUid.iUid, &filenamePckg));
	if (ret==KErrNone)
		{
		HBufC* fullFileName = HBufC::New(fileName.Length());
		if (fullFileName == NULL)
			{
			return KErrNoMemory;
			}
		else
			{
			*fullFileName = fileName;
			aFullFileName = fullFileName; // ownership transferred to caller
			}
		}
	return ret;
	}



/** Gets the full filename of a file containing view-specific icons for the application
with the specified UID and view.

A file containing view-specific icons can only be defined by applications providing
an application registration file.

A view icon file may be in any graphics format and contain one or more view icons.

@param aAppUid The application specific UID.
@param aViewUid The UID identifying the view.
@param aFullFileName On return, the full filename of a file containing one or more
view icons. Returns a pointer to the filename and transfers ownership to the caller.
@return KErrNone, if successful; KErrNotFound, if no matching application nor matching view
could be found, or a view icon filename was not defined; KErrNotSupported, if the application
does not provide an application registration file; otherwise one of the other system wide error codes. 
*/
EXPORT_C TInt RApaLsSession::GetAppViewIcon(TUid aAppUid, TUid aViewUid, HBufC*& aFullFileName) const
	{
	TFileName fileName;
	TPckg<TFileName> filenamePckg(fileName);
	TInt ret=SendReceiveWithReconnect(EAppListServAppViewIconFileName, TIpcArgs(aAppUid.iUid, aViewUid.iUid, &filenamePckg));
	if (ret==KErrNone)
		{
		HBufC* fullFileName = HBufC::New(fileName.Length());
		if (fullFileName == NULL)
			{
			return KErrNoMemory;
			}
		else
			{
			*fullFileName = fileName;
			aFullFileName = fullFileName; // ownership transferred to caller
			}
		}
	return ret;
	}



/** Changes an existing data type mapping, or adds a new one.

If the data type is not currently mapped, a new mapping is added. 
If the data type is mapped, its mapping is replaced.

@capability WriteDeviceData Prevent addition of data type mappings by malicious programs.
@param aDataType A new or existing data type.
@param aPriority The priority with which the application handles the data type.
@param aUid The UID of the application to associate with the data type.
@return KErrNone on success, or a system-wide error code. 
*/
EXPORT_C TInt RApaLsSession::InsertDataMapping(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid)
	{
	return InsertDataMapping(aDataType, aPriority, aUid, KOpenServiceUid);
	}

/** Changes an existing data type mapping, or adds a new one.

If the data type is not currently mapped, a new mapping is added. 
If the data type is mapped, its mapping is replaced.

@capability WriteDeviceData Prevent addition of data type mappings by malicious programs.
@param aDataType A new or existing data type.
@param aPriority The priority with which the application handles the data type.
@param aUid The UID of the application to associate with the data type.
@param aServiceUid The UID of the service.
@return KErrNone on success, or a system-wide error code. 
*/
EXPORT_C TInt RApaLsSession::InsertDataMapping(const TDataType& aDataType, TDataTypePriority aPriority, 
	TUid aUid, TUid aServiceUid)
	{
	const TPckgC<TDataType> dataType(aDataType);
	return SendReceiveWithReconnect(EAppListInsertDataMapping, 
		TIpcArgs(&dataType, TInt(aPriority), aUid.iUid, aServiceUid.iUid));
	} //lint !e1762 Suppress member function could be made const

/** Changes an existing data type mapping, or adds a new one.
If the data type is not currently mapped, it is added.
If the data type is mapped with a priority lower than aPriority, the new mapping replaces the existing one. 
Otherwise, no change is made.

@capability WriteDeviceData Prevent addition of data type mappings by malicious programs.
@param aDataType A new or existing data type.
@param aPriority The priority with which the application handles the data type.
@param aUid The UID of the application to associate with the data type.
@param aInserted Non-zero if the new mapping was added or an existing mapping replaced, zero otherwise.
@return KErrNone on success, or a system-wide error code. 
*/
EXPORT_C TInt RApaLsSession::InsertDataMappingIfHigher(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid, TBool& aInserted)
	{
	TPckgBuf<TBool> inserted(EFalse);
	const TPckgC<TDataType> dataType(aDataType);
	const TInt ret = SendReceiveWithReconnect(EAppListInsertDataMappingIfHigher, TIpcArgs(&dataType, TInt(aPriority), aUid.iUid, &inserted));
	if(ret == KErrNone)
		{
		aInserted = inserted();
		}
	return ret;
	} //lint !e1762 Suppress member function could be made const



/** Removes an existing user mapping between an application and data-type made through InsertDataMapping() or InsertDataMappingIfHigher().

@capability WriteDeviceData Prevent removal of data type mappings by malicious programs.
@param aDataType Data type whose mapping should be removed.
@panic USER 0 The specified data type cannot be found. Debug builds only.
@return KErrNone on success, or a system-wide error code. 
@see InsertDataMapping()
@see InsertDataMappingIfHigher()
*/
EXPORT_C TInt RApaLsSession::DeleteDataMapping(const TDataType& aDataType)
	{
	return DeleteDataMapping(aDataType, KOpenServiceUid);
	}

/** Removes an existing data type mapping.

@capability WriteDeviceData Prevent removal of data type mappings by malicious programs.
@param aDataType Data type whose mapping should be removed.
@param aServiceUid The UID of the service.
@panic USER 0 The specified data type cannot be found. Debug builds only.
@return KErrNone on success, or a system-wide error code. 
*/
EXPORT_C TInt RApaLsSession::DeleteDataMapping(const TDataType& aDataType, TUid aServiceUid)
	{
	const TPckgC<TDataType> dataType(aDataType);
	return SendReceiveWithReconnect(EAppListDeleteDataMapping, TIpcArgs(&dataType, aServiceUid.iUid));
	} //lint !e1762 Suppress member function could be made const
	
/** Gets the application associated with the data type and the service uid from
the datatype store. 

The function will only look into the datatype store and will not use the 
default type associations. This is different from the AppForDataTypeAndService() function.

@param aDataType The data (MIME) type.
@param aAppUid On return, the UID of the application that can handle the data 
(MIME) type; this may be NULL.
@param aServiceUid The UID of the service.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes. 
*/
EXPORT_C TInt RApaLsSession::GetAppByDataType(const TDataType& aDataType, TUid aServiceUid, TUid& aAppUid) const
	{
	const TPckgC<TDataType> dataType(aDataType);
	TPckg<TUid> uid(aAppUid);
	const TInt returnValue=SendReceiveWithReconnect(EAppListServGetAppByDataType,TIpcArgs(&dataType, aServiceUid.iUid, &uid));
	if (returnValue<0)
		{
		return returnValue;
		}
	if (aAppUid == KNullUid)
		{
		return KErrNotFound;
		}
	else
		{
		return KErrNone;
		}
	} //lint !e1764 Suppress reference parameter 'aAppUid' could be declared const ref

/** Determines the current language an application is using to display its
user interface.
@param aAppUid The application specific UID.
@param aLanguage On return, the application language.  
@return KErrNone, if successful; KErrNotFound, if a matching application could not be found; 
otherwise one of the other system wide error codes.
*/
EXPORT_C TInt RApaLsSession::ApplicationLanguage(TUid aAppUid, TLanguage& aLanguage) const
	{
	TPckgBuf<TLanguage> pckg;
	TInt errVal = SendReceiveWithReconnect(EAppListServApplicationLanguage, TIpcArgs(aAppUid.iUid, &pckg));
	if (errVal == KErrNone)
		{
		aLanguage = pckg();
		}
	return errVal;
	}

/** Gets the services implemented by the application that has the specified
application UID.

The returned CApaAppServiceInfoArray object contains an array of TApaAppServiceInfo objects.

Information on each service implementation is contained in a TApaAppServiceInfo object.

TApaAppServiceInfo::Uid() returns the service UID of the service implemented by the
specified application UID.

@param aAppUid The application specific UID.
@return A pointer to a CApaAppServiceInfoArray object left on the cleanup stack.
@leave KErrNotFound No matching application can be found, or a matching application
does not implement any services.
@leave KErrNotSupported The specified application does not provide an application
registration file.
@leave KErrNoMemory There is insufficient memory to perform the operation.
@see CApaAppServiceInfoArray::Array()
@see TApaAppServiceInfo
@publishedPartner
@released
*/
EXPORT_C CApaAppServiceInfoArray* RApaLsSession::GetAppServicesLC(TUid aAppUid) const
	{
	CArrayFixFlat<TApaAppServiceInfo>* serviceArray = new(ELeave) CArrayFixFlat<TApaAppServiceInfo>(4);
	CleanupStack::PushL(TCleanupItem(CleanupServiceArray, serviceArray));
	CBufBase* buffer = GetServiceBufferLC(EAppListServGetAppServices, aAppUid);
	RBufReadStream readStream(*buffer);
	readStream >> *serviceArray;
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::Pop(serviceArray);
	CApaAppServiceInfoArrayWrapper* wrapper = CApaAppServiceInfoArrayWrapper::NewL(serviceArray); // takes ownership of serviceArray
	CleanupStack::PushL(wrapper);
	return wrapper;
	}

CBufBase* RApaLsSession::GetServiceBufferLC(TInt aOpcode, TUid aUid1, TUid aUid2/*=KNullUid*/) const
	{
	CBufFlat* buffer=CBufFlat::NewL(KDefaultBufSize); // allocate buffer with a default size that should be large enough in most cases
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0,KDefaultBufSize);
	TPtr8 bufPtr=buffer->Ptr(0);
	const TInt sizeRequired=User::LeaveIfError(SendReceiveWithReconnect(aOpcode,TIpcArgs(aUid1.iUid,aUid2.iUid,buffer->Size(),&bufPtr)));
	if (sizeRequired>0)
		{
		CleanupStack::PopAndDestroy(buffer);
		buffer=CBufFlat::NewL(sizeRequired);
		CleanupStack::PushL(buffer);
		buffer->ExpandL(0,sizeRequired);
		bufPtr.Set(buffer->Ptr(0));
#if defined(_DEBUG)
		const TInt check=
#endif
		User::LeaveIfError(SendReceiveWithReconnect(aOpcode,TIpcArgs(aUid1.iUid,aUid2.iUid,0,&bufPtr)));
		__ASSERT_DEBUG(check==0,User::Invariant());
		}
	return buffer;
	}
	
CBufBase* RApaLsSession::GetServiceBufferLC(TInt aOpcode, TUid aUid1, const TDataType& aDataType) const
	{
	const TPckgC<TDataType> dataType(aDataType);
	CBufFlat* buffer=CBufFlat::NewL(KDefaultBufSize); // allocate buffer with a default size that should be large enough in most cases
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0,KDefaultBufSize);
	TPtr8 bufPtr=buffer->Ptr(0);
	const TInt sizeRequired=User::LeaveIfError(SendReceiveWithReconnect(aOpcode,TIpcArgs(aUid1.iUid,&dataType,buffer->Size(),&bufPtr)));
	if (sizeRequired>0)
		{
		CleanupStack::PopAndDestroy(buffer);
		buffer=CBufFlat::NewL(sizeRequired);
		CleanupStack::PushL(buffer);
		buffer->ExpandL(0,sizeRequired);
		bufPtr.Set(buffer->Ptr(0));
#if defined(_DEBUG)
		const TInt check=
#endif
		User::LeaveIfError(SendReceiveWithReconnect(aOpcode,TIpcArgs(aUid1.iUid,&dataType,0,&bufPtr)));
		__ASSERT_DEBUG(check==0,User::Invariant());
		}
	return buffer;	
	}



/** Gets the service implementations for the specified service UID.

The returned CApaAppServiceInfoArray object contains an array of TApaAppServiceInfo objects.

Information on each implementation is contained in a TApaAppServiceInfo object.

TApaAppServiceInfo::Uid() returns the UID of the application that implements the
specified service UID.

@param aServiceUid The service UID.
@return A pointer to a CApaAppServiceInfoArray object left on the cleanup stack.
@leave KErrNotFound No service implementations for the specified service UID can be found.
@leave EAppListInvalid The server's initial population of the list has not completed.
@leave KErrNoMemory There is insufficient memory to perform the operation.
@see CApaAppServiceInfoArray::Array()
@see TApaAppServiceInfo
@publishedPartner
@released
*/
EXPORT_C CApaAppServiceInfoArray* RApaLsSession::GetServiceImplementationsLC(TUid aServiceUid) const
	{
	CArrayFixFlat<TApaAppServiceInfo>* serviceArray = new(ELeave) CArrayFixFlat<TApaAppServiceInfo>(4);
	CleanupStack::PushL(TCleanupItem(CleanupServiceArray, serviceArray));
	CBufBase* buffer = GetServiceBufferLC(EAppListServGetServiceImplementations, aServiceUid);
	RBufReadStream readStream(*buffer);
	readStream >> *serviceArray;
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::Pop(serviceArray);
	CApaAppServiceInfoArrayWrapper* wrapper = CApaAppServiceInfoArrayWrapper::NewL(serviceArray); // takes ownership of serviceArray
	CleanupStack::PushL(wrapper);
	return wrapper;
	}
	
EXPORT_C CApaAppServiceInfoArray* RApaLsSession::GetServiceImplementationsLC(TUid aServiceUid, const TDataType& aDataType) const
/** Gets the service implementations for the specified service UID. The implementation must also be
able to handle the data type given as argument.

The returned CApaAppServiceInfoArray object contains an array of TApaAppServiceInfo objects.

Information on each implementation is contained in a TApaAppServiceInfo object.

TApaAppServiceInfo::Uid() returns the UID of the application that implements the
specified service UID.

@param aServiceUid The service UID.
@param aDataType The data type that must be supported by the implementation.
@return A pointer to a CApaAppServiceInfoArray object left on the cleanup stack.
@leave KErrNotFound No service implementations for the specified service UID can be found.
@leave EAppListInvalid The server's initial population of the list has not completed.
@leave KErrNoMemory There is insufficient memory to perform the operation.
@see CApaAppServiceInfoArray::Array()
@see TApaAppServiceInfo
@publishedPartner
@released
*/
	{
	CArrayFixFlat<TApaAppServiceInfo>* serviceArray = new(ELeave) CArrayFixFlat<TApaAppServiceInfo>(4);
	CleanupStack::PushL(TCleanupItem(CleanupServiceArray, serviceArray));
	CBufBase* buffer = GetServiceBufferLC(EAppListServGetServiceImplementationsDataType, aServiceUid, aDataType);
	RBufReadStream readStream(*buffer);
	readStream >> *serviceArray;
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::Pop(serviceArray);
	CApaAppServiceInfoArrayWrapper* wrapper = CApaAppServiceInfoArrayWrapper::NewL(serviceArray); // takes ownership of serviceArray
	CleanupStack::PushL(wrapper);
	return wrapper;
	}

/** Gets the service UIDs implemented by the application with the specified UID.

@param aAppUid The application specific UID.
@param aServiceUids On return, contains the service UIDs implemented by the specified
application UID.
@leave KErrNotFound No matching application can be found, or a matching application
does not implement any services.
@leave KErrNotSupported The specified application does not provide an application
registration file.
@leave KErrNoMemory There is insufficient memory to perform the operation.
@publishedPartner
@released
*/
EXPORT_C void RApaLsSession::GetAppServicesL(TUid aAppUid, CArrayFixFlat<TUid>& aServiceUids) const
	{
	CBufBase* buffer = GetServiceBufferLC(EAppListServGetAppServiceUids, aAppUid);
	RBufReadStream readStream(*buffer);
	readStream >> aServiceUids;
	CleanupStack::PopAndDestroy(buffer);
	}



/** Gets the service implementation's opaque data for the specified application and service.

The specified application may provide more than one implementation of the specified service.

The returned CApaAppServiceInfoArray object contains an array of TApaAppServiceInfo objects,
each of which provides information on an implementation.

For each TApaAppServiceInfo object, TApaAppServiceInfo::Uid() returns the specified service UID.

@param aAppUid The application specific UID.
@param aServiceUid The service UID.
@return A pointer to a CApaAppServiceInfoArray object left on the cleanup stack.
@leave KErrNotFound No matching application can be found, or a matching application
does not implement the specified service.
@leave KErrNotSupported The specified application does not provide an application
registration file.
@leave KErrNoMemory There is insufficient memory to perform the operation.
@see CApaAppServiceInfoArray::Array()
@see TApaAppServiceInfo
@publishedPartner
@released
*/
EXPORT_C CApaAppServiceInfoArray* RApaLsSession::GetAppServiceOpaqueDataLC(TUid aAppUid, TUid aServiceUid) const
	{
	CArrayFixFlat<TApaAppServiceInfo>* serviceArray = new(ELeave) CArrayFixFlat<TApaAppServiceInfo>(4);
	CleanupStack::PushL(TCleanupItem(CleanupServiceArray, serviceArray));
	CBufBase* buffer = GetServiceBufferLC(EAppListServGetAppServiceOpaqueData, aAppUid, aServiceUid);
	RBufReadStream readStream(*buffer);
	readStream >> *serviceArray;
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::Pop(serviceArray);
	CApaAppServiceInfoArrayWrapper* wrapper = CApaAppServiceInfoArrayWrapper::NewL(serviceArray); // takes ownership of serviceArray
	CleanupStack::PushL(wrapper);
	return wrapper;
	}



/** Gets the UID of an application that can handle the specified data (MIME) type and service.

If no application can be found, the function returns the UID of the preferred 
default handler. If none of the default handlers can handle the combination
of data type and service, then a NULL UID is returned in aAppUid.

@param aDataType The data (MIME) type.
@param aServiceUid The service UID.
@param aAppUid On return, the UID of the application that can handle the data 
(MIME) type and service; this may be NULL.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes.
@publishedPartner
@released
*/
EXPORT_C TInt RApaLsSession::AppForDataTypeAndService(const TDataType& aDataType, TUid aServiceUid, TUid& aAppUid) const
	{
	const TPckgC<TDataType> dataType(aDataType);
	TPckg<TUid> uid(aAppUid);
	return SendReceiveWithReconnect(EAppListServAppForDataTypeAndService,TIpcArgs(&dataType, aServiceUid.iUid,&uid));
	} //lint !e1764 Suppress reference parameter 'aAppUid' could be declared const ref

EXPORT_C TInt RApaLsSession::AppForDocumentAndService(const TDesC& aFileName, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType) const
/** Gets the data (MIME) type of the data in the specified file and gets the UID 
of an application that can handle this type and service.

@param aFileName The name of the file containing the data.
@param aServiceUid The service UID
@param aUid On return, the UID of the application that can handle the data 
(MIME) type and service; this may be NULL.
@param aDataType On return, the data (MIME) type.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes.
@publishedPartner
@released
*/
	{
	return DoAppForDocumentOptionallySpecifyingService(aFileName, aServiceUid, aAppUid, aDataType, EAppListServAppForDocumentAndService);
	}

EXPORT_C TInt RApaLsSession::AppForDocumentAndService(const RFile& aFile, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType) const
/** Gets the data (MIME) type of the data in the specified file and gets the UID 
of an application that can handle this type and service.

@param aFile The file handle.  Before this function can be called,
the file server session which owns this file handle must first be marked as shareable by 
calling RFs::ShareProtected().
@param aServiceUid The service UID.
@param aUid On return, the UID of the application that can handle the data 
(MIME) type and service; this may be NULL.
@param aDataType On return, the data (MIME) type.
@return KErrNone, if successful; otherwise one of the other system-wide error 
codes.
@publishedPartner
@released
*/
	{
	return DoAppForDocumentOptionallySpecifyingService(aFile, aServiceUid, aAppUid, aDataType, EAppListServAppForDocumentAndServicePassedByFileHandle);
	}

TInt RApaLsSession::DoAppForDocumentOptionallySpecifyingService(const TDesC& aFileName, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType, TInt aOpcode) const
	{
	if (aFileName.Length()==0)
		{
		aAppUid = KNullUid;
		return KErrNone;
		}
	HBufC8* buffer=NULL;
	TInt error=GetBufferFromFile(buffer, aFileName);
	if (error!=KErrNone)
		{
		return error;
		}
	SReturnData_AppForDocument returnData;
	TPckg<SReturnData_AppForDocument> returnData_asDescriptor(returnData);
	error=SendReceiveWithReconnect(aOpcode,TIpcArgs(&returnData_asDescriptor, aServiceUid.iUid, &aFileName, buffer));
	delete buffer;
	buffer=NULL;
	if (error==KErrNone)
		{
		aAppUid=returnData.iUid;
		aDataType=returnData.iDataType;
		}
	return error;
	}

TInt RApaLsSession::DoAppForDocumentOptionallySpecifyingService(const RFile& aFile, TUid aServiceUid, TUid& aAppUid, TDataType& aDataType, TInt aOpcode) const
	{
	SReturnData_AppForDocument returnData;
	TPckg<SReturnData_AppForDocument> returnData_asDescriptor(returnData);
	TIpcArgs ipcArgs(&returnData_asDescriptor, aServiceUid.iUid);
	TInt error=aFile.TransferToServer(ipcArgs, 2, 3);
	if (error == KErrNone)
		{
		error=SendReceiveWithReconnect(aOpcode, ipcArgs);
		}
	if (error==KErrNone)
		{
		aAppUid=returnData.iUid;
		aDataType=returnData.iDataType;
		}
	return error;
	}

TInt RApaLsSession::GetBufferFromFile(HBufC8*& aBuffer, const TDesC& aFileName) const
	{
	TInt preferredBufSize=0;
	const TInt error=GetPreferredBufSize(preferredBufSize);
	if (error<KErrNone)
		{
		return error;
		}
	HBufC8* const buffer=HBufC8::New(Max(8, preferredBufSize)); // 8 is a sensible minimum
	if (buffer==NULL)
		{
		return KErrNoMemory;
		}
	const RFs* fsSession=FsSession();
	RFs tempFsSession;
	if (fsSession==NULL)
		{
		const TInt error=tempFsSession.Connect();
		if (error!=KErrNone)
			{
			delete buffer;
			return error;
			}
		fsSession=&tempFsSession;
		}
	if (fsSession->IsValidName(aFileName))
		{
		TPtr8 buffer_asWritable(buffer->Des());
		const TInt error=fsSession->ReadFileSection(aFileName, 0, buffer_asWritable, preferredBufSize);
		if (error!=KErrNone)
			{
			delete buffer;
			tempFsSession.Close();
			return error;
			}
		}
	if (fsSession==&tempFsSession)
		{
		tempFsSession.Close();
		}
	aBuffer=buffer;
	return KErrNone;
	}

EXPORT_C void RApaLsSession::SetFsSessionL(RFs& aFsSession)
/**
@internalTechnology
*/
	{ // static
	User::LeaveIfError(Dll::SetTls(&aFsSession));
	}

EXPORT_C void RApaLsSession::ClearFsSession()
/**
@internalTechnology
*/
	{ // static
	Dll::FreeTls();
	}

EXPORT_C RFs* RApaLsSession::FsSession()
/**
@internalComponent
*/
	{ // static
	return static_cast<RFs*>(Dll::Tls());
	}

/** @publishedPartner */
EXPORT_C void RApaLsSession::RegisterNonNativeApplicationTypeL(TUid aApplicationType, const TDesC& aNativeExecutable)
	{
	User::LeaveIfError(SendReceiveWithReconnect(EAppListServRegisterNonNativeApplicationType, TIpcArgs(aApplicationType.iUid, &aNativeExecutable)));
	} //lint !e1762 Suppress member function could be made const

/** @publishedPartner */
EXPORT_C void RApaLsSession::DeregisterNonNativeApplicationTypeL(TUid aApplicationType)
	{
	User::LeaveIfError(SendReceiveWithReconnect(EAppListServDeregisterNonNativeApplicationType, TIpcArgs(aApplicationType.iUid)));
	} //lint !e1762 Suppress member function could be made const
	
/** @publishedPartner */
EXPORT_C void RApaLsSession::PrepareNonNativeApplicationsUpdatesL()
	{
	TIpcArgs ipcArgs(0, 0, 0, 0);
	User::LeaveIfError(SendReceiveWithReconnect(EAppListServPrepareNonNativeApplicationsUpdates, ipcArgs));
	} //lint !e1762 Suppress member function could be made const

/** @publishedPartner */
EXPORT_C void RApaLsSession::RegisterNonNativeApplicationL(TUid aApplicationType, const TDriveUnit& aDrive, CApaRegistrationResourceFileWriter& aRegistrationResourceFile, CApaLocalisableResourceFileWriter* aLocalisableResourceFile, const RFile* aIconFile)
	{
	TIpcArgs ipcArgs(0, 0, 0, 0);
	RBuf8 ipcParameter0;
	CleanupClosePushL(ipcParameter0);
	RBuf8 ipcParameter1;
	CleanupClosePushL(ipcParameter1);
	if (aLocalisableResourceFile==NULL)
		{
		__ASSERT_ALWAYS(aIconFile==NULL, Panic(EPanicIconFileWithoutLocalisableResourceFile));
		ipcArgs.Set(1, NULL);
		}
	else
		{
		TParse* const parser=new(ELeave) TParse;
		CleanupStack::PushL(parser);
		const TDriveName driveName(aDrive.Name()); // TDriveName is a TBuf<2>

		if (aIconFile!=NULL)
			{
			User::LeaveIfError(aIconFile->TransferToServer(ipcArgs, 2, 3));

			TFileName* const fileName=new(ELeave) TFileName;
			CleanupStack::PushL(fileName);
			User::LeaveIfError(aIconFile->Name(*fileName));
			parser->SetNoWild(*fileName, &KLitPathForNonNativeResourceAndIconFiles, &driveName);
			aLocalisableResourceFile->SetIconFileL(parser->FullName());
			CleanupStack::PopAndDestroy(fileName);
			}
		aLocalisableResourceFile->GenerateFileContentsL(ipcParameter1); // must be done after the aLocalisableResourceFile->SetIconFileL call (if there is one)
		const TDesC8& ipcParameter1_asConst=ipcParameter1;
		ipcArgs.Set(1, &ipcParameter1_asConst);

		TBuf<30> fileName;
		fileName.Format(KLitFormatForLocalisableResourceFile, aRegistrationResourceFile.AppUid().iUid);
		parser->SetNoWild(fileName, &KLitPathForNonNativeResourceAndIconFiles, &driveName);
		aRegistrationResourceFile.SetLocalisableResourceFileL(parser->FullName());

		CleanupStack::PopAndDestroy(parser);
		}
	aRegistrationResourceFile.GenerateFileContentsL(ipcParameter0); // must be done after the aRegistrationResourceFile.SetLocalisableResourceFileL call (if there is one)
	SNonNativeApplicationInfo nonNativeApplicationInfo;
	nonNativeApplicationInfo.iApplicationType=aApplicationType;
	nonNativeApplicationInfo.iDrive=aDrive;
	ipcParameter0.ReAllocL(sizeof(SNonNativeApplicationInfo)+ipcParameter0.Length());
	ipcParameter0.Insert(0, TPckgC<SNonNativeApplicationInfo>(nonNativeApplicationInfo));
	const TDesC8& ipcParameter0_asConst=ipcParameter0;
	ipcArgs.Set(0, &ipcParameter0_asConst);

	User::LeaveIfError(SendReceiveWithReconnect(EAppListServRegisterNonNativeApplication, ipcArgs));
	CleanupStack::PopAndDestroy(2, &ipcParameter0);
	} //lint !e1762 Suppress member function could be made const

/** @publishedPartner */
EXPORT_C void RApaLsSession::DeregisterNonNativeApplicationL(TUid aApplication)
	{
	User::LeaveIfError(SendReceiveWithReconnect(EAppListServDeregisterNonNativeApplication, TIpcArgs(aApplication.iUid)));
	} //lint !e1762 Suppress member function could be made const
	
/**
Commits the non-native application updates. This is a synchronous method which waits 
until the application list is updated.
	 
@see ForceCommitNonNativeApplicationsUpdatesL
@publishedPartner
@released
*/

EXPORT_C void RApaLsSession::CommitNonNativeApplicationsUpdatesL()
	{
	TIpcArgs ipcArgs(EFalse, 0, 0, 0);
	User::LeaveIfError(SendReceiveWithReconnect(EAppListServCommitNonNativeApplications, ipcArgs));
	} //lint !e1762 Suppress member function could be made const


/**
Commits the non-native application updates. This is an asynchronous method which will not wait until 
the application list is updated. CApaAppListNotifier class should be used to synchronize the completion 
of updating the application list. 
 
@see CommitNonNativeApplicationsUpdatesL
@see CApaAppListNotifier
@publishedPartner
@released
*/

EXPORT_C void RApaLsSession::ForceCommitNonNativeApplicationsUpdatesL()
	{
	TIpcArgs ipcArgs(ETrue, 0, 0, 0);
	User::LeaveIfError(SendReceiveWithReconnect(EAppListServCommitNonNativeApplications, ipcArgs));
	}

/** 
Rolls back all changes made to the list of installed non-native applications since the last call to
PrepareNonNativeApplicationsUpdatesL().

This function can be called even if PrepareNonNativeApplicationsUpdatesL() hasn't been called before (in which
case it does nothing).

@publishedPartner
*/
EXPORT_C TInt RApaLsSession::RollbackNonNativeApplicationsUpdates()
	{
	TIpcArgs ipcArgs(0, 0, 0, 0);
	return SendReceiveWithReconnect(EAppListServRollbackNonNativeApplications, ipcArgs);
	} //lint !e1762 Suppress member function could be made const

EXPORT_C void RApaLsSession::SetNotify(TBool aCompleteImmediatelyIfNoScanImpendingOrInProgress, TRequestStatus& aStatus)
/**
@internalTechnology */
	{
	SendReceive(ESetNotify,TIpcArgs(aCompleteImmediatelyIfNoScanImpendingOrInProgress),aStatus);
	} //lint !e1762 Suppress member function could be made const

EXPORT_C void RApaLsSession::CancelNotify()
/**
@internalTechnology */
	{
	SendReceive(ECancelNotify,TIpcArgs());
	} //lint !e1762 Suppress member function could be made const
	
/**
Gets the application type of the application. For a native application the type is KNullUid.
@return A standard error code.
@publishedPartner
@released
@param aTypeUid On return contains the application's type
@param aAppUid The application's UID passed into TIpcArgs	
*/
EXPORT_C TInt RApaLsSession::GetAppType(TUid& aTypeUid, TUid aAppUid) const
	{
	TPckg<TUid> uid(aTypeUid);
	return SendReceiveWithReconnect(EAppListServGetAppType,TIpcArgs(aAppUid.iUid,&uid));
	} //lint !e1764 Suppress reference parameter 'aTypeUid' could be declared const ref
	
/**
This function is only for use by Software Install.

As part of the fix for defect INC069526, we added a check in apparc. We check if the application has 
been installed before adding it to the apparc db. A side-effect of this fix is that it is not possible 
to launch applications that are being installed from the installation process itself. This is a regresssion.

To fix this regression we added this function. It allows Software Install to specify a list of registration
files that need to be included in apparc's list even if they have not been marked as installed in the
SISRegistry or JavaRegistry. The list of registration files is cleared once Software Install notifies
(via P&S) the end of the installation (whether successful or not).
The function also forces a rescan and only returns when this rescan is complete. This is because 
Software Install needs to be sure the registration files have been added to apparc's list before 
trying to launch the recently installed applications.

@param aRegFiles The list of registration files for which the SISRegistry check must be ignored.
@return A standard error code.
@internalAll
@released
*/
EXPORT_C TInt RApaLsSession::ForceRegistration(const RPointerArray<TDesC>& aRegFiles)
	{
	CBufFlat* buffer = 0;
	TRAPD(err, buffer = CreateRegFilesBufferL(aRegFiles));
	if (err != KErrNone)
		{
		return err;
		}
	TPtr8 ptr = buffer->Ptr(0);
	const TInt returnValue=SendReceiveWithReconnect(EAppListServForceRegistration,TIpcArgs(&ptr));
	delete buffer;
	return returnValue;
	} //lint !e1762 Suppress member function could be made const
	
TInt RApaLsSession::SendReceiveWithReconnect(TInt aFunction,const TIpcArgs& aIpcArgs) const
	{
	TInt ret = SendReceive(aFunction, aIpcArgs);
	if(ret != KErrServerTerminated)
		{
		return ret;
		}

	RApaLsSession ls;
	TInt err=ls.Connect();
	if (err==KErrNone)
		{
		RApaLsSession* const localThis = const_cast<RApaLsSession*>(this);
		localThis->Close();
		localThis->iHandle=ls.iHandle;
		ret = SendReceive(aFunction, aIpcArgs);
		}
	return ret;
	}

void RApaLsSession::DoGetAppOwnedFilesL(CDesCArray& aArrayToFill, TUid aAppUid) const
	{
	TDesCArrayFiller arrayFiller(aArrayToFill);
	FetchArrayL(arrayFiller, aAppUid, EAppListServGetFileOwnershipInfo, KInitialOwnedFilesBufSize);
	}

EXPORT_C void RApaLsSession::RegisterListPopulationCompleteObserver(TRequestStatus& aStatus) const
/** Registers an observer with apparc server to notify when the initial population of applist is completed

@param aStatus Request status object. On successful completion contains KErrNone, otherwise one of the 
system-wide error codes. 
@see CancelListPopulationCompleteObserver()
*/
	{
	SendReceive(ERegisterListPopulationCompleteObserver,TIpcArgs(TIpcArgs::ENothing),aStatus);
	}

EXPORT_C TInt RApaLsSession::CancelListPopulationCompleteObserver() const
/** Cancels the observer registered with apparc server to notify when the initial population of applist is completed
 
@return KErrNone, if successful; otherwise one of the system-wide error codes. 
*/
	{
	return SendReceiveWithReconnect(ECancelListPopulationCompleteObserver,TIpcArgs(TIpcArgs::ENothing));
	}

EXPORT_C TInt RApaLsSession::MatchesSecurityPolicy(TBool& aMatches, TUid aAppUid, const TSecurityPolicy& aSecurityPolicy) const
/** Tests whether the given TSecurityPolicy matches with the application TSecurityPolicy.

@param aMatches On return, contains the result. ETrue if the application TSecurityPolicy matches the given TSecurityPolicy or else EFalse
@param aAppUid Uid of the application for which the security policy has to be matched
@param aSecurityPolicy TSecurityPolicy to test whether the application with given uid matches with its TSecurityPolicy or not.
@return KErrNone, if successful; otherwise one of the other system-wide error codes.
@see TSecurityPolicy
*/
	{
	const TPckgC<TSecurityPolicy> securityPolicy(aSecurityPolicy);
	const TInt result = SendReceiveWithReconnect(EMatchesSecurityPolicy,TIpcArgs(aAppUid.iUid, &securityPolicy));

	if (result>=0)
		{
		aMatches = result;
		}
	return result;
	}

EXPORT_C void RApaLsSession::RApaLsSession_Reserved1()
	{
	}
	
EXPORT_C void RApaLsSession::RApaLsSession_Reserved2()
	{
	}
	
EXPORT_C void RApaLsSession::NotifyOnDataMappingChange(TRequestStatus& aRequestStatus)
/** Notification of changes in data-type mapping

This asynchronous function (whose corresponding "cancel" operation is CancelNotifyOnDataTypeMappingChange) completes when any data-type / application-UID association changes, i.e. when the default application handling a particular MIME-type changes.

@param aRequestStatus As is normal for an asynchronous operation, this object is set to something other than KRequestPending when the asynchronous operation that has been triggered by this function completes.
@see CancelNotifyOnDataTypeMappingChange
*/
	{
	SendReceive(ENotifyOnDataMappingChange,TIpcArgs(), aRequestStatus);
 	} //lint !e1762 Suppress member function could be made const
 	
EXPORT_C void RApaLsSession::CancelNotifyOnDataMappingChange()
/** Cancellation of notification of changes in data-type mapping

This cancels the outstanding the NotifyOnDataTypeMappingChange issued by this client, if there is one outstanding. Otherwise it does nothing.
@see NotifyOnDataTypeMappingChange
*/
	{
	SendReceive(ECancelNotifyOnDataMappingChange,TIpcArgs());
	} //lint !e1762 Suppress member function could be made const

CBufFlat* RApaLsSession::CreateRegFilesBufferL(const RPointerArray<TDesC>& aRegFiles)
	{
	// Serialize the array
	// Format of the buffer is :
	// 4 bytes for array item count
	// for each item : 4 bytes for length and then the string contents
	const TInt count = aRegFiles.Count();
	TInt requiredBufferSize = 4;	// For the array item count
	for (TInt index = 0; index < count; ++index)
		{
		requiredBufferSize += 4;	// For the string length
		requiredBufferSize += aRegFiles[index]->Size();
		}
	
	CBufFlat* const buffer = CBufFlat::NewL(requiredBufferSize);
	CleanupStack::PushL(buffer);
	buffer->ExpandL(0,requiredBufferSize);
	RBufWriteStream writeStream;
	writeStream.Open(*buffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteUint32L(count);
	for (TInt index = 0; index < count; ++index)
		{
		writeStream.WriteUint32L(aRegFiles[index]->Length());
		writeStream.WriteL(*aRegFiles[index]);
		}
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::Pop(buffer);
	return buffer;
	}

// TSizeArrayFiller

void TSizeArrayFiller::AppendItemL(RReadStream& aReadStream)
	{
	TSize size;
	size.iWidth =aReadStream.ReadUint32L();
	size.iHeight=aReadStream.ReadUint32L();
	iArray.AppendL(size);
	}



// TViewDataArrayFiller

void TViewDataArrayFiller::AppendItemL(RReadStream& aReadStream)
	{
	TApaAppViewInfo info;
	aReadStream >> info;
	iArray.AppendL(info);
	}



// TDesCArrayFiller

void TDesCArrayFiller::AppendItemL(RReadStream& aReadStream)
	{
	TFileName info;
	aReadStream >> info;
	iArray.AppendL(info);
	}

/* @internalComponent
*/
EXPORT_C TUint MinApplicationStackSize()
	{
	return KMinApplicationStackSize;
	}
