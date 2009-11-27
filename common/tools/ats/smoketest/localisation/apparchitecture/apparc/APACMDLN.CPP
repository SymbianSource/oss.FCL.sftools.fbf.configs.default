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

#include <apacmdln.h>
#include "APASTD.H" // Panics etc.
#include <s32mem.h>

#include <e32svr.h>

_LIT(KLitTokenServerDifferentiator, "-srvDfr=");
_LIT(KLitTokenDefaultScreenNumber, "-dsc=");
_LIT(KLitTokenParentWindowGroupID, "-prntwgid=");
_LIT(KLitTokenDebugMemFail, "-debugMemFail:");
_LIT(KLitTokenAppStartupInstrumentationEventIdBase, "-appStartupInstrEvIdBase=");
_LIT(KLitTokenOpaqueData, "-opaque=");

// CApaCommandLine

CApaCommandLine::CApaCommandLine() : 
	iDocumentName( NULL), iExecutableName(NULL), iOpaqueData(NULL),
	iTailEnd(NULL), iCommand(EApaCommandRun),
	iServerDifferentiator(0), iDefaultScreenNumber(-1),
	iParentWindowGroupID(0), iDebugMemFail(0), iAppStartupInstrumentationEventIdBase(0),
	iFile(), iParentProcessId(KNullProcessId)
	//
	// private c'tor - initialize using NewLC()
	//
	{
	}

EXPORT_C CApaCommandLine::~CApaCommandLine()
/** Destructor.

Frees resources owned by the object prior to deletion. */
	{
	delete iDocumentName;
	delete iExecutableName;
	delete iOpaqueData;
	delete iTailEnd;
	iFile.Close();
	}

EXPORT_C CApaCommandLine* CApaCommandLine::NewL()
/** Creates an empty command line object.

@return A pointer to the new command line object. */
	{
	CApaCommandLine* This=CApaCommandLine::NewLC();
	CleanupStack::Pop();
	return This;
	}

EXPORT_C void CApaCommandLine::ConstructCmdLineFromMessageL(const RMessage2& aMessage)
/** Acts as a second constructor and completes a commandline object from 
	the aMessage object.

@internalTechnology */
	{
	const TInt bufLen = aMessage.GetDesLengthL(EIpcSlotMain);
	HBufC8* const buffer=HBufC8::NewLC(bufLen);
	{TPtr8 buffer_asWritable(buffer->Des());
	aMessage.ReadL(EIpcSlotMain, buffer_asWritable);}
	RDesReadStream stream;
	stream.Open(*buffer);
	InternalizeL(stream);
	CleanupStack::PopAndDestroy(buffer);

	iFile.AdoptFromClient(aMessage, EIpcSlotFsSession, EIpcSlotFile); // ignore the returned error - assume it means that no file has been passed across
	}

void CApaCommandLine::GetCommandLineFromProcessEnvironmentL()
	{
	const TInt bufLen = User::ParameterLength(EEnvironmentSlotMain);
	if (bufLen==KErrNotFound)
		{
		HBufC* const commandLineString=HBufC::NewLC(User::CommandLineLength());
		{TPtr commandLineString_asWritable(commandLineString->Des());
		User::CommandLine(commandLineString_asWritable);}
		User::LeaveIfError(Parse(*commandLineString));
		CleanupStack::PopAndDestroy(commandLineString);
		}
	else
		{
		User::LeaveIfError(bufLen); // in case bufLen is some error other than KErrNotFound
		HBufC8* const buffer=HBufC8::NewLC(bufLen);
		{TPtr8 buffer_asWritable(buffer->Des());
		User::LeaveIfError(User::GetDesParameter(EEnvironmentSlotMain, buffer_asWritable));}
		RDesReadStream stream;
		stream.Open(*buffer);
		InternalizeL(stream);
		CleanupStack::PopAndDestroy(buffer);
		}

	iFile.AdoptFromCreator(EEnvironmentSlotFsSession, EEnvironmentSlotFile); // ignore the returned error - assume it means that no file has been passed across
	}

EXPORT_C CApaCommandLine* CApaCommandLine::NewLC()
/** Creates an empty command line object, and puts a pointer to it onto the cleanup 
stack.

@return A pointer to the new command line object. */
	{
	CApaCommandLine* This=new(ELeave) CApaCommandLine;
	CleanupStack::PushL(This);
	return This;
	}

EXPORT_C void CApaCommandLine::SetDocumentNameL(const TDesC& aDocName)
/** Sets the document name from the specified descriptor.

If the document name has embedded spaces, then it must be enclosed within 
quotation marks.

@param aDocName A descriptor containing the document name. */
	{
	HBufC* const documentName=aDocName.AllocL();
	delete iDocumentName; // only done after the AllocL succeeds
	iDocumentName=documentName;
	}

EXPORT_C TPtrC CApaCommandLine::DocumentName() const
/** Gets the document name from the launch information.

@return A pointer descriptor representing the document name. The document 
name is returned without any enclosing quotation marks. If the launch information 
contains no document name, then the pointer descriptor has zero length. */
	{
	if (iDocumentName!=NULL)
		{
		return *iDocumentName;
		}
	return KNullDesC();
	}

EXPORT_C void CApaCommandLine::SetExecutableNameL(const TDesC& aExecutableName)
/** Sets the executable name from the specified descriptor.

@param aExecutableName A descriptor containing the executable name. */
	{
	HBufC* const executableName=aExecutableName.AllocL();
	delete iExecutableName; // only done after the AllocL succeeds
	iExecutableName=executableName;
	}

EXPORT_C TPtrC CApaCommandLine::ExecutableName() const
/** Gets the executable name from the launch information.

@return A pointer descriptor representing the executable name. */
	{
	if (iExecutableName!=NULL)
		{
		return *iExecutableName;
		}
	return KNullDesC();
	}

EXPORT_C void CApaCommandLine::SetOpaqueDataL(const TDesC8& aOpaqueData)
/** Sets the opaque-data from the specified 8-bit descriptor.

This is called internally and populated from the data in the application's registration resource file, 
i.e. from the resource indicated by the opaque_data field of the APP_REGISTRATION_INFO resource (if the 
opaque_data field was non-zero).

@param aOpaqueData An 8-bit descriptor containing the opaque-data. */
	{
	HBufC8* const opaqueData = aOpaqueData.AllocL();
	delete iOpaqueData;
	iOpaqueData = opaqueData;
	}

EXPORT_C TPtrC8 CApaCommandLine::OpaqueData() const
/** Gets the opaque-data from the launch information.

See the description of SetOpaqueDataL. By default, this attribute is an empty descriptor.

@see SetOpaqueDataL
@return An 8-bit pointer descriptor representing the opaque-data. */
	{
	if (iOpaqueData != NULL)
		{
		return *iOpaqueData;
		}
	return KNullDesC8();
	}

EXPORT_C void CApaCommandLine::SetCommandL(TApaCommand aCommand)
/** Sets the command code.

The command code is used to indicate how an application is to be launched.

@see TApaCommand
@param aCommand The command code. */
	{
	iCommand=aCommand;
	}

EXPORT_C TApaCommand CApaCommandLine::Command() const
/** Gets the command code from the launch information.

See the description of SetCommandL.

@see SetCommandL
@see TApaCommand
@return The command code. */
	{
	return iCommand;
	}

EXPORT_C void CApaCommandLine::SetTailEndL(const TDesC8& aTailEnd)
/** Sets the trailing data.

The UI Framework provides a specific set of pre-defined command line 
options. Additional user defined or pre-defined command line options, 
may be passed to an application by setting the TailEnd.

@param aTailEnd An 8 bit descriptor containing the trailing data. */
	{
	HBufC8* const newTailEnd=aTailEnd.AllocL();
	delete iTailEnd; // only done after the AllocL succeeds
	iTailEnd=newTailEnd;
	}

EXPORT_C TPtrC8 CApaCommandLine::TailEnd() const
/** Gets the trailing data from the launch information.

See the description of SetTailEndL.

@see SetTailEndL
@return A pointer descriptor representing the trailing data. If the launch 
information contains no trailing data, then the pointer descriptor has zero 
length. */
	{
	if (iTailEnd!=NULL)
		{
		return *iTailEnd;
		}
	return KNullDesC8();
	}

EXPORT_C void CApaCommandLine::SetFileByHandleL(const RFile& aFile)
/** Sets the file to be passed into the application (by handle). 
This will then be retrieved by that application-process calling GetFileByHandleL().

@publishedPartner
@released
@param aFile The file object to be passed into the application. No transfer of this 
object's ownership takes place in this function. */
	{
	__ASSERT_ALWAYS(aFile.SubSessionHandle()!=KNullHandle, Panic(EPanicInvalidHandle));
	__ASSERT_ALWAYS(iFile.SubSessionHandle()==KNullHandle, Panic(EPanicHandleAlreadySet));
	User::LeaveIfError(iFile.Duplicate(aFile));
	}

EXPORT_C void CApaCommandLine::GetFileByHandleL(RFile& aFile) const
/** Opens (by handle) the file that was passed into SetFileByHandleL by the process launching the local application.

On entering this function, aFile must be non-open. It is recommended that aFile is 
pushed onto the cleanup-stack (via CleanupClosePushL()) before this function is called.

@publishedPartner
@released
@param aFile The file object to be set up from the handle passed into the application. The 
caller has the responsibility to Close() this object, even if this function leaves. */
	{
	__ASSERT_ALWAYS(aFile.SubSessionHandle()==KNullHandle, Panic(EPanicHandleAlreadySet));
	if (iFile.SubSessionHandle()!=KNullHandle)
		{
		User::LeaveIfError(aFile.Duplicate(iFile));
		}
	}

/** Assigns a command line to a process (EKA2 only). 

This replaces the EKA1 method which involved retrieving the full command line (using 
CApaCommandLine::FullCommandLine()) and passing it to the process (or thread on the 
emulator).

This function is used as follows (the order of the first 2 steps is irrelevant):-
- create the process and load the executable (RProcess::Create()),
- create the command line object (CApaCommandLine::NewLC()), and set it up using 
the various setter functions, for instance SetDocumentNameL(),
- call SetProcessEnvironmentL() to assign the command line to the process,
- call Resume() on the process.

Note that this sequence of steps bypasses the application architecture, and is 
not recommended. RApaLsSession::StartApp() is the recommended way to 
launch an application with a command line.

@param aProcess The process to which the command line is assigned.
@leave KErrNotSupported This indicates that the function was called on EKA1.
@see RApaLsSession::StartApp()
*/
EXPORT_C void CApaCommandLine::SetProcessEnvironmentL(RProcess& aProcess) const
	{
	HBufC8* const streamableAttributes=StreamableAttributesLC();
	User::LeaveIfError(aProcess.SetParameter(EEnvironmentSlotMain, *streamableAttributes));
	CleanupStack::PopAndDestroy(streamableAttributes);

	if (iFile.SubSessionHandle()!=KNullHandle)
		{
		User::LeaveIfError(iFile.TransferToProcess(aProcess, EEnvironmentSlotFsSession, EEnvironmentSlotFile));
		}
	}

EXPORT_C void CApaCommandLine::GetIpcArgsLC(TIpcArgs& aIpcArgs) const
/**
@internalTechnology
*/
	{
	aIpcArgs.Set(EIpcSlotMain, StreamableAttributesLC());
	if (iFile.SubSessionHandle()!=KNullHandle)
		{
		User::LeaveIfError(iFile.TransferToServer(aIpcArgs, EIpcSlotFsSession, EIpcSlotFile));
		}
	}

EXPORT_C TInt CApaCommandLine::GetCommandLineFromProcessEnvironment(CApaCommandLine*& aCommandLine)
/** Constructs a command line object.

If command line information is provided in the environment-slots it creates command line object from
process environment-slots, else creates it from the information returned by User::CommandLine().

It can be called from a context where there is no CTrapCleanup.

Calling this function more than once in a process is not supported and will result in an empty command
line being returned. If an application wants to inspect any part of its command line, it 
should override CEikAppUi::ProcessCommandParametersL(CApaCommandLine& aCommandLine) and call the base
class implementation if required.

@see CEikAppUi::ProcessCommandParametersL(CApaCommandLine& aCommandLine).
@param aCommandLine On return, a pointer to a newly constructed command line object.
@return KErrNone, if successful; otherwise one of the other system-wide error codes.
@internalTechnology */
	{ // static
	aCommandLine = NULL;
	CApaCommandLine* const commandLine = new CApaCommandLine;
	if(!commandLine)
		return KErrNoMemory;
	
	CTrapCleanup* trapCleanup = NULL;
	if (!User::TrapHandler())
		{
		trapCleanup = CTrapCleanup::New(); // we're being called from an environment without a cleanup-stack, so temporarily create one here
		if(!trapCleanup)
			{
			delete commandLine;
			return KErrNoMemory;
			}
		}
	
	TRAPD(error, commandLine->GetCommandLineFromProcessEnvironmentL());
	aCommandLine = commandLine;
	delete trapCleanup;
	return error;
	}

EXPORT_C void CApaCommandLine::SetParentProcessId(TProcessId aProcessId)
/** Sets the Parent Process ID for the Child Process launched with this command line.

This establishes a Parent-Child relationship which ensures that the child process is
terminated when the parent terminates.

@param aProcessId The Process ID. */
	{
	iParentProcessId=aProcessId;
	}

EXPORT_C TProcessId CApaCommandLine::ParentProcessId() const
/** Gets the Parent Process ID of the Child Process launched with this command line.

See the description of SetParentProcessId.

@see SetParentProcessId
@return The Parent Process ID. */
	{
	return iParentProcessId;
	}


void CApaCommandLine::ExternalizeL(RWriteStream& aStream) const
	{
	// iFile is not supported via RReadStream/RWriteStream
	aStream << DocumentName();
	aStream << ExecutableName();
	aStream << OpaqueData();
	aStream << TailEnd();
	aStream.WriteInt32L(iCommand);
	aStream.WriteInt32L(iServerDifferentiator);
	aStream.WriteInt32L(iDefaultScreenNumber);
	aStream.WriteInt32L(iParentWindowGroupID);
	aStream.WriteInt32L(iDebugMemFail);
	aStream.WriteInt32L(iAppStartupInstrumentationEventIdBase);
	aStream.WriteInt32L(iParentProcessId);
	}


void CApaCommandLine::InternalizeL(RReadStream& aStream)
	{
	// iFile is not supported via RReadStream/RWriteStream
	const TInt KMaxBufLength = 4000;
	iDocumentName = HBufC::NewL(aStream,KMaxBufLength);
	iExecutableName = HBufC::NewL(aStream,KMaxBufLength);
	iOpaqueData = HBufC8::NewL(aStream, KMaxBufLength);
	iTailEnd = HBufC8::NewL(aStream,KMaxBufLength);
	iCommand = static_cast<TApaCommand>(aStream.ReadInt32L());
	iServerDifferentiator = aStream.ReadInt32L();
	iDefaultScreenNumber = aStream.ReadInt32L();
	iParentWindowGroupID = aStream.ReadInt32L();
	iDebugMemFail = aStream.ReadInt32L();
	iAppStartupInstrumentationEventIdBase = aStream.ReadInt32L();
	iParentProcessId=aStream.ReadInt32L();
	}

HBufC8* CApaCommandLine::StreamableAttributesLC() const
	{
	CBufFlat* const buffer = CBufFlat::NewL(128);
	CleanupStack::PushL(buffer);
	RBufWriteStream writeStream;
	writeStream.Truncate(*buffer);
	ExternalizeL(writeStream);
	writeStream.CommitL();
	HBufC8* const bufferAsDescriptor = buffer->Ptr(0).AllocL();
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PushL(bufferAsDescriptor);
	return bufferAsDescriptor;
	}

EXPORT_C void CApaCommandLine::SetServerNotRequiredL()
/** Sets that no server is required.

The value of server differentiator is set to zero, to indicate that no server
is required.

See the description of SetServerRequiredL.
@see SetServerRequiredL
*/
	{
	SetServerDifferentiatorL(0);
	}

EXPORT_C void CApaCommandLine::SetServerRequiredL(TUint aServerDifferentiator)
/** Sets the required server.

The server differentiator is a number generated by the client that helps to uniquely 
identify the server. It is used by an application to indicate whether a server should
be created and how it should be named.

@param aServerDifferentiator A differentiator for the required server.*/

	{
	SetServerDifferentiatorL(aServerDifferentiator);
	}

void CApaCommandLine::SetServerDifferentiatorL(TUint aServerDifferentiator)
	{
	iServerDifferentiator=aServerDifferentiator;
	}
	
EXPORT_C TUint CApaCommandLine::ServerRequired() const
/** Gets the server differentiator.

See the description of SetServerRequiredL.

@see SetServerRequiredL
@return The non-zero differentiator for the server, else zero indicating a server 
is not required.*/
	{
	return iServerDifferentiator;
	}

EXPORT_C void CApaCommandLine::SetDefaultScreenL(TInt aDefaultScreenNumber)
/** Provides support for devices with more than one screen.  A number representing the default
or startup screen may be passed to an application.
Screen numbers and characteristics are defined in the window server initialisation 
file (wsini.ini).

@param aDefaultScreenNumber The number of the default (startup) screen. */
	{
	__ASSERT_ALWAYS(aDefaultScreenNumber>=0, Panic(EPanicInvalidScreenNumber));
	iDefaultScreenNumber=aDefaultScreenNumber;
	}

EXPORT_C TInt CApaCommandLine::DefaultScreen() const
/** Extracts and returns the default (startup) screen that was specified in the command line.

@return	A number representing the default (startup) screen.  0 (Zero) if nothing present. */
	{
	return Max(0, iDefaultScreenNumber);
	}
	
EXPORT_C TBool CApaCommandLine::IsDefaultScreenSet() const
/**
@internalTechnology
*/
	{
	return (iDefaultScreenNumber>=0);
 	}

EXPORT_C void CApaCommandLine::SetParentWindowGroupID(TInt aParentWindowGroupID)
/** Sets the ID of the parent window-group - the application should create its own 
window-group as a child off this parent.

@param aParentWindowGroupID The ID of the parent window-group - the application 
should create its window-group as a child off this parent. */
	{
	iParentWindowGroupID=aParentWindowGroupID;
	}

EXPORT_C TInt CApaCommandLine::ParentWindowGroupID() const
/** Returns the ID of the parent window-group - the application should create its own 
window-group as a child of this parent.

@return The ID of the parent window-group - the application should create its 
window-group as a child off this . */
	{
	return iParentWindowGroupID;
	}

EXPORT_C void CApaCommandLine::SetDebugMemFailL(TInt aDebugMemFail)
/** @internalAll */
	{
	iDebugMemFail=aDebugMemFail;
	}

EXPORT_C TInt CApaCommandLine::DebugMemFail() const
/** @internalAll */
	{
	return iDebugMemFail;
	}

EXPORT_C void CApaCommandLine::SetAppStartupInstrumentationEventIdBaseL(TInt aAppStartupInstrumentationEventIdBase)
/** @internalAll */
	{
	iAppStartupInstrumentationEventIdBase=aAppStartupInstrumentationEventIdBase;
	}

EXPORT_C TInt CApaCommandLine::AppStartupInstrumentationEventIdBase() const
/** @internalAll */
	{
	return iAppStartupInstrumentationEventIdBase;
	}

EXPORT_C TInt CApaCommandLine::EnvironmentSlotForPublicUse(TInt aIndex)
/** Returns the index of a process environment-slot for public use (in other words, 
one that is not used internally by CApaCommandLine). The number of slots available 
for public use can be found in the (private) enum value CApaCommandLine::ENumberOfEnvironmentSlotsForPublicUse, 
(this value may be increased over time). The returned value can then be passed into 
any of the Open(TInt,...) functions on RSessionBase, RMutex, RChunk, RCondVar, etc, 
or into User::GetTIntParameter(), User::GetDesParameter(), etc, depending on the type 
of the object in that environment slot.

@param aIndex The logical index of the public environment-slot. This must be greater 
than or equal to zero, and less than CApaCommandLine::ENumberOfEnvironmentSlotsForPublicUse.
@return The physical index of an environment-slot in the local process. */
	{ // static
	__ASSERT_ALWAYS((aIndex>=0) && (aIndex<ENumberOfEnvironmentSlotsForPublicUse), Panic(EPanicEnvironmentSlotNotForPublicUse));
	return EFirstEnvironmentSlotForPublicUse+aIndex;
	}

TInt CApaCommandLine::Parse(const TDesC& aCmdLine)
// does the opposite of SetCmdLineL, i.e. sets iDocumentName, iExecutableName, iTailEnd, iCommand, iServerDifferentiator, iDefaultScreenNumber, iParentWindowGroupID , iDebugMemFail & iAppStartupInstrumentationEventIdBase from aCmdLine
// also sets iOpaqueData
	{
	const TInt cmdLength=aCmdLine.Length();

	// these variables are all "shadows" of member variables - we'll set the member variables corresponding to these at the end of this function, once all memory-allocation has succeeded, to make this function atomic
	TInt endLibNameOffset=cmdLength-1;
	TInt endDocNameOffset=cmdLength-1;
	HBufC* documentName=NULL;
	HBufC* executableName=NULL;
	HBufC8* tailEnd=NULL;
	HBufC8* opaqueData=NULL;
	TApaCommand command=EApaCommandRun;
	TInt serverDifferentiator=0;
	TInt defaultScreenNumber=-1;
	TInt parentWindowGroupID=0;
	TInt debugMemFail=0;
	TInt appStartupInstrumentationEventIdBase=0;
	TInt notUsed;

	TBool openQuote=EFalse;
	TBool foundEndLibName=EFalse;
	for (TInt i=0; i<cmdLength; ++i)
		{
		TChar current=aCmdLine[i];
		if (current=='"')
			{
			openQuote=!openQuote;
			continue;
			};
		if ((current==' ') && !openQuote)
			{
			// space found outside of quotes
			if (foundEndLibName)
				{
				endDocNameOffset=i-1;
				break; // parse no further
				}
			endLibNameOffset=i-1;
			foundEndLibName=ETrue;
			}
		}
	if (endLibNameOffset>-1)
		{
		executableName=StripQuotes(aCmdLine.Left(endLibNameOffset+1)).Alloc();
		if (executableName==NULL)
			{
			delete documentName;
			delete executableName;
			delete tailEnd;
			delete opaqueData;
			return KErrNoMemory;
			}
		}
	TInt offset=endDocNameOffset-endLibNameOffset;
	if (offset>1)
		{
		TChar commandLetter=aCmdLine[endLibNameOffset+2];
		switch (commandLetter)
			{
		case KApaCommandLetterOpen:
			command=EApaCommandOpen;
			break;
		case KApaCommandLetterCreate:
			command=EApaCommandCreate;
			break;
		case KApaCommandLetterViewActivate:
			command=EApaCommandViewActivate;
			break;
		case KApaCommandLetterRunWithoutViews:
			command=EApaCommandRunWithoutViews;
			break;
		case KApaCommandLetterBackgroundAndWithoutViews:
			command=EApaCommandBackgroundAndWithoutViews;
			break;
		case KApaCommandLetterRun:
		default:
			break;
		case KApaCommandLetterBackground:
			command=EApaCommandBackground;
			break;
			}

		if (offset>2)
			{
			const TInt documentNameStartPosition=endLibNameOffset+3;
			documentName=StripQuotes(aCmdLine.Mid(documentNameStartPosition, (endDocNameOffset+1)-documentNameStartPosition)).Alloc();
			if (documentName==NULL)
				{
				delete documentName;
				delete executableName;
				delete tailEnd;
				delete opaqueData;
				return KErrNoMemory;
				}
			}
		}
	const TInt KNumberOfSupportedOptions=6;
	TFixedArray<SOption, KNumberOfSupportedOptions> optionArray;
	optionArray[0].iToken=&KLitTokenServerDifferentiator;
	optionArray[0].iResult=&serverDifferentiator;
	optionArray[0].iRadix=EDecimal;
	optionArray[1].iToken=&KLitTokenDefaultScreenNumber;
	optionArray[1].iResult=&defaultScreenNumber;
	optionArray[1].iRadix=EDecimal;
	optionArray[2].iToken=&KLitTokenParentWindowGroupID;
	optionArray[2].iResult=&parentWindowGroupID;
	optionArray[2].iRadix=EDecimal;
	optionArray[3].iToken=&KLitTokenDebugMemFail;
	optionArray[3].iResult=&debugMemFail;
	optionArray[3].iRadix=EHex;
	optionArray[4].iToken=&KLitTokenAppStartupInstrumentationEventIdBase;
	optionArray[4].iResult=&appStartupInstrumentationEventIdBase;
	optionArray[4].iRadix=EDecimal;
	optionArray[5].iToken=&KLitTokenOpaqueData;
	optionArray[5].iResult=&notUsed;
	optionArray[5].iRadix=EDecimal; // should not used if the command-line is well-formed
	TLex lex(aCmdLine.Mid(endDocNameOffset+1));
	lex.Mark();
	for (TInt optionIndex=0; optionIndex<KNumberOfSupportedOptions; ++optionIndex)
		{
		lex.SkipSpace();
		SOption& option=optionArray[optionIndex];
		const TPtrC remainder(lex.Remainder());
		__ASSERT_DEBUG(option.iToken, Panic(EDPanicInvalidToken));
		const TInt tokenLength=option.iToken->Length();
		if ((remainder.Length()>=tokenLength) && (remainder.Left(tokenLength).CompareF(*option.iToken)==0))
			{
			if (option.iToken==&KLitTokenOpaqueData)
				{
				TInt endOfOpaqueDataIndex = 0;			
				for (TInt i=tokenLength; i<remainder.Length(); ++i)
					{
					TChar current=remainder[i];
					if (current==' ')
						{
						endOfOpaqueDataIndex = i;
						break; // parse no further
						}
					}
				if(endOfOpaqueDataIndex > tokenLength)
					{
					const TInt opaqueDataLength = endOfOpaqueDataIndex - tokenLength;
					delete opaqueData;
					opaqueData=TPtrC8(reinterpret_cast<const TUint8*>(remainder.Mid(tokenLength, opaqueDataLength).Ptr()),opaqueDataLength*sizeof(TText)).Alloc();
					if (opaqueData==NULL)
						{
						delete documentName;
						delete executableName;
						delete tailEnd;
						return KErrNoMemory;
						}
					lex.Inc(tokenLength + opaqueDataLength);
					lex.Mark();
					}
				else
					{
					delete opaqueData;
					delete documentName;
					delete executableName;
					delete tailEnd;
					// invalid command line. copy TLex.Val behavior
					return KErrGeneral;
					}
				}
			else
				{
				ASSERT(option.iResult);
				const TInt originalValue=*option.iResult;
				lex.Inc(tokenLength);
				TUint16 val = static_cast<TUint16> (*option.iResult);
				if (lex.Val(val, option.iRadix)==KErrNone)
					{
					lex.Mark();
					}
				else
					{
					*option.iResult=originalValue;
					}
				}
			}
		}
	lex.UnGetToMark();
	lex.SkipSpace();
	const TPtrC remainder(lex.Remainder());
	const TInt lengthOfRemainder=remainder.Length();
	if (lengthOfRemainder>0)
		{
		tailEnd=TPtrC8(reinterpret_cast<const TUint8*>(remainder.Ptr()),lengthOfRemainder*sizeof(TText)).Alloc();
		if (tailEnd==NULL)
			{
			delete documentName;
			delete executableName;
			delete tailEnd;
			delete opaqueData;
			return KErrNoMemory;
			}
		}

	// we can now set the member variables as all memory-allocation has succeeded
	delete iDocumentName;
	iDocumentName=documentName;
	delete iExecutableName;
	iExecutableName=executableName;
	delete iTailEnd;
	iTailEnd=tailEnd;
	iCommand=command;
	iServerDifferentiator=serverDifferentiator;
	iDefaultScreenNumber=defaultScreenNumber;
	iParentWindowGroupID=parentWindowGroupID;
	iDebugMemFail=debugMemFail;
	iAppStartupInstrumentationEventIdBase=appStartupInstrumentationEventIdBase;
	delete iOpaqueData;
	iOpaqueData = opaqueData;
	return KErrNone;
	}

TPtrC CApaCommandLine::StripQuotes(const TDesC& aDes) const
	//
	// return aDes stripped of any enclosing quotes 
	//
	{
	TInt start=0;
	TInt end=aDes.Length()-1;
	TPtrC ret;
	if (end>=0)
		{
		if (aDes[0]=='"')
			{
			start++;
			}
		if (aDes[end]=='"')
			{
			end--;
			}
		TInt length=end-start+1;
		if (length>0)
			{
			ret.Set(aDes.Mid(start, length));
			}
		}
	return ret;
	}
