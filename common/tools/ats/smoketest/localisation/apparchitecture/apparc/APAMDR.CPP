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

#include "APASTD.H" // Panics etc.
#include <apadbase.h>
#include <apamdr.h>
#include <apparc.h>
#include <apgdoor.h>

#include <s32stor.h>
#include <s32file.h>
#include <s32std.h>
#include <s32mem.h>

#ifdef _UNICODE
#define KUidApaDoorBaseStream KUidApaDoorBaseStream16
#else
#define KUidApaDoorBaseStream KUidApaDoorBaseStream8
#endif

#ifdef _UNICODE
const TUid KUidApaDoorBaseStream16={0x10003A36};
#else
const TUid KUidApaDoorBaseStream8={0x10000146};
#endif
const TInt KHugeGranularity=4096; // 4k granularity for the door's host buffer

////////////////////////////////////
// HBufBuf
////////////////////////////////////

class HBufBuf : public TBufBuf
	{
public:
	static HBufBuf* NewL(CBufBase& aBuf,TInt aPos,TInt aMode=ERead|EWrite);
private:
	void DoRelease();
	};


HBufBuf* HBufBuf::NewL(CBufBase& aBuf,TInt aPos,TInt aMode)
//
// Create a pre-set buffer stream buffer.
//
	{
	HBufBuf* buf=new(ELeave) HBufBuf;
	buf->Set(aBuf,aPos,aMode);
	return buf;
	}


void HBufBuf::DoRelease()
//
// Finished with this stream buffer.
//
	{
	delete this;
	}


////////////////////////////////////
// CApaDoorBase
////////////////////////////////////

EXPORT_C CApaDoorBase::CApaDoorBase()
	{
	}

EXPORT_C void CApaDoorBase::ExternalizeBaseStreamL(CStreamStore& aStore,CStreamDictionary& aStreamDict)const
/** Externalises the information contained in this base class to a stream in the 
specified store.

The resulting stream ID is associated with an internal UID, and placed in 
the specified stream dictionary.

@publishedAll
@released
@param aStore The store to contain the stream.
@param aStreamDict The stream dictionary to contain the stream ID (and an associated 
internal UID). */
	{
	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(aStore);
	//
	// write the format
	if (iFormat==ETemporarilyIconic)
		stream.WriteInt8L(EGlassDoor);
	else
		stream.WriteInt8L(iFormat);
	//
	// write the size
	TSize size;
	if (iFormat==ETemporarilyIconic)
		size = GlassDoorSize();
	else
		GetSizeInTwips(size);
	stream<< size;
	// 
	// write the source and close
	stream<< iSource;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	aStreamDict.AssignL(KUidApaDoorBaseStream,id);
	}



EXPORT_C TSize CApaDoorBase::InternalizeBaseStreamL(const CStreamStore& aStore,const CStreamDictionary& aStreamDict)
/** Internalises the information for this base class from a stream in the specified 
store, and returns the size of the icon or glass door.

The stream ID is extracted from the specified stream dictionary. The UID associated 
with the stream ID is the same as that used when externalising.

@publishedAll 
@released
@param aStore The store containing the stream.
@param aStreamDict The steam dictionary containing the stream ID.
@return The size of the icon or glass door, in twips. */
	{
	TStreamId id=aStreamDict.At(KUidApaDoorBaseStream);
	if (id==KNullStreamId)
		User::Leave(KErrCorrupt); //  there is no base stream - the file is not valid (used to panic EPanicNoBaseDoorStream)
	RStoreReadStream stream;
	stream.OpenLC(aStore,id);
	iFormat = (TFormat)stream.ReadInt8L();
	__ASSERT_DEBUG(iFormat==EIconic || iFormat==EGlassDoor,Panic(EDPanicIllegalDoorFormat));
	TSize size;
	stream>> size;
	stream>> iSource;
	CleanupStack::PopAndDestroy(); // stream
	return size;
	}

// Virtual functions from CPicture

EXPORT_C TStreamId CApaDoorBase::StoreL(CStreamStore& aStore) const
/** Stores the Embedded Document to the specified store.

@publishedAll 
@released
@param aStore The store containing the stream.
@return The ID of the (head) stream used to store the Embedded Document */
	{
	return CPicture::StoreL(aStore);
	}

EXPORT_C void CApaDoorBase::SetScaleFactor(TInt aScaleFactorWidth,TInt aScaleFactorHeight)
/** Sets the Embedded Document's scale factors

@publishedAll 
@released
@param aScaleFactorWidth The width scale factor, in percent
@param aScaleFactorHeight The height scale factor, in percent
*/
	{
	CPicture::SetScaleFactor(aScaleFactorWidth, aScaleFactorHeight);
	}

EXPORT_C void CApaDoorBase::SetCropInTwips(const TMargins& aMargins)
/** Sets the cropping margins of a Embedded Document in twips.
These are relative to the original unscaled size of the Embedded Document.

@publishedAll 
@released
@param aMargins The cropping margins of the Embedded Document, in twips
*/	
	{
	CPicture::SetCropInTwips(aMargins);
	}
	
EXPORT_C TPictureCapability CApaDoorBase::Capability() const
/** Gets the Embedded Document's capabilities.
These include whether it is scalable and croppable.

@publishedAll 
@released
@return The capabilities of the Embedded Document
*/
	{
	return CPicture::Capability();
	}
	
EXPORT_C void CApaDoorBase::GetCropInTwips(TMargins& aMargins) const 
/** Gets the cropping margins of a Embedded Document in twips.
These margins are relative to the original unscaled size of the Embedded Document.

@publishedAll 
@released
@param aMargins The cropping margins of the Embedded Document, in twips 
*/
	{
	CPicture::GetCropInTwips(aMargins);
	}

EXPORT_C TInt CApaDoorBase::ScaleFactorWidth() const
/** Gets the Embedded Document's width scale factor.

@publishedAll 
@released
@return The width scale factor, in percent
*/	
	{
	return CPicture::ScaleFactorWidth();
	}

EXPORT_C TInt CApaDoorBase::ScaleFactorHeight() const
/** Gets the Embedded Document height scale factor.

@publishedAll 
@released
@return The height scale factor, in percent
*/
	{
	return CPicture::ScaleFactorHeight();
	}

EXPORT_C TBool CApaDoorBase::LineBreakPossible(TUint aClass,TBool aBeforePicture,TBool aHaveSpaces) const
/** States whether a line break is possible, either before or after an Embedded Document.
The default implementation returns ETrue, implying that there is a break opportunity both before and after the Embedded Document, whether or not a space is present.
This may be overridden for special types of Embedded Documents.

@publishedAll 
@released
@param aClass The line breaking class of the adjacent character
@param aBeforePicture ETrue, if the adjacent character is before the Embedded Document; EFalse, if the adjacent character is afterwards
@param aHaveSpaces ETrue, if spaces occur between the adjacent character and the Embedded Document; EFalse, otherwise
@return ETrue, if a line break is possible; EFalse, otherwise. 
*/	
	{
	return CPicture::LineBreakPossible(aClass, aBeforePicture, aHaveSpaces);
	}

EXPORT_C TBool CApaDoorBase::NativePixelSize(TSize& aPixelSize)
/** Returns the native pixel size of the bitmap. 

Derived classes might be implemented as bitmaps, in that case it might be interesting to know this.
@publishedAll 
@released
@param aPixelSize The pixel size
@return TBool ETrue, if the derived classes are implemented as bitmaps; EFalse, otherwise.
*/	
	{
	return CPicture::NativePixelSize(aPixelSize);
	}

/** Reserved for future use */
EXPORT_C void CApaDoorBase::CApaDoorBase_Reserved1()
	{
	}

/** Reserved for future use */
EXPORT_C void CApaDoorBase::CApaDoorBase_Reserved2()
	{
	}

///////////////////////////////////
// TApaModelDoorFactory
///////////////////////////////////

EXPORT_C TApaModelDoorFactory::TApaModelDoorFactory(const MApaModelHeaderFactory* aFactory)
	:iHeaderFactory(aFactory)
/** Constructs a door factory object.

@param aFactory A pointer to a factory object for constructing the application 
model wrapper object, also known as the application model header, a CApaModelHeader 
type. The application model wrapper is provided by the application model (not 
the application UI), and supplies the knowledge for internalizing the application 
model data from an embedded store.
@see CApaModelHeader
@see MApaModelHeaderFactory */
	{}


EXPORT_C void TApaModelDoorFactory::NewPictureL(TPictureHeader& aPictureHeader,const CStreamStore& aPictureStore)const
/** Constructs and restores an application's door (picture) from a stream in the 
specified store.

The restored door is a CApaModelDoor type object.

@param aPictureHeader The header identifying the door to be restored. The 
UID identifying the door must be KUidPictureTypeDoor, otherwise the function 
leaves with KErrNotSupported. On entry, the door picture must be represented 
by a stream ID, otherwise the function leaves with KErrBadHandle; on return, 
the door picture is represented by a pointer to an internalized CApaModelDoor 
object.
@param aPictureStore The store from which the door will be restored.
@see TPictureHeader::iPicture */
	{
	if (aPictureHeader.iPictureType!=KUidPictureTypeDoor)
		User::Leave(KErrNotSupported); // wrong type
	if (!aPictureHeader.iPicture.IsId())
		User::Leave(KErrBadHandle); // not an id - can't restore
	//
	// create and restore the door
	TStreamId id = aPictureHeader.iPicture.AsId();
	aPictureHeader.iPicture = CApaModelDoor::NewL(aPictureStore,id,iHeaderFactory);
	}


///////////////////////////////////
// CApaModelDoor
///////////////////////////////////


EXPORT_C CApaModelDoor* CApaModelDoor::NewL(CApaModelHeader* aHeader)
/** Creates a model door object.

@param aHeader A pointer to an existing concrete application model wrapper 
object.
@return A pointer to the new model door object. */
	{
	CApaModelDoor* self = new(ELeave) CApaModelDoor(aHeader);
	return self;
	}


EXPORT_C CApaModelDoor* CApaModelDoor::NewLC(CApaModelHeader* aHeader)
/** Creates a model door object, and puts a pointer to it onto the cleanup stack.

@param aHeader A pointer to an existing concrete application model wrapper 
object.
@return A pointer to the new model door object. */
	{
	CApaModelDoor* self = CApaModelDoor::NewL(aHeader);
	CleanupStack::PushL(self);
	return self;
	}


EXPORT_C CApaModelDoor* CApaModelDoor::NewL(const CStreamStore& aStore,TStreamId aHeadStreamId,const MApaModelHeaderFactory* aFactory)
/** Creates a model door object and restores it from an embedded store within the 
specified store.

@param aStore The store from which the model door is to be restored.
@param aHeadStreamId The head stream ID. This is the ID of the stream containing 
the stream dictionary which is restored as part of the process of creating 
this model door object. The stream dictionary contains the ID of the stream 
hosting the embedded store.
@param aFactory A pointer to a factory object for constructing the application 
model wrapper object, also known as the application model header, a CApaModelHeader 
type.
@return A pointer to the new model door object.
@see MApaModelHeaderFactory */
	{
	CApaModelDoor* self = new(ELeave) CApaModelDoor();
	CleanupStack::PushL(self);
	self->RestoreL(aStore,aHeadStreamId,aFactory);
	CleanupStack::Pop();
	return self;
	}


CApaModelDoor::CApaModelDoor()
	{__DECLARE_NAME(_S("CApaModelDoor"));}


CApaModelDoor::CApaModelDoor(CApaModelHeader* aHeader)
	:iModelHeader(aHeader)
	{}


EXPORT_C CApaModelDoor::~CApaModelDoor()
/** Destructor.

Frees all resources owned by the object, prior to its destruction. */
	{
	delete iModelHeader;
	delete iStore;
	delete iStoreHost;
	}


EXPORT_C TStreamId CApaModelDoor::StoreL(CStreamStore& aTargetStore)const
/** Stores the model data in the specified store as an embedded store.

The function stores the model data, if the model exists in memory, otherwise, 
it simply copies the stream containing the embedded document into the specified 
store.

@param aStore The store in which the model data is to be stored.
@return The stream ID of the head stream for the embedded model data. This 
stream contains the stream dictionary through which the embedded data and 
its door can be restored. */
	{
	// create stream dictionary
	CStreamDictionary* streamDic = CStreamDictionary::NewLC();
	//
	// stream out door's state
	ExternalizeBaseStreamL(aTargetStore,*streamDic);
	//
	// store the model 
	TStreamId id;
	RStoreWriteStream stream;
	if (iModelHeader)
		{
		// create an embedded store in a new write stream
		id = stream.CreateL(aTargetStore);
		CEmbeddedStore* target=CEmbeddedStore::NewLC(stream); // takes ownership of stream
		streamDic->AssignL(KUidApaDoorDocStream,id);
		// store the model
		CStreamDictionary* rootDict=CStreamDictionary::NewLC();
		iModelHeader->StoreL(*target,*rootDict);
		CApaProcess::WriteRootStreamL(*target,*rootDict,iModelHeader->AppId());
		CleanupStack::PopAndDestroy(); // rootDict
		// close the new embedded store
		target->CommitL();
		CleanupStack::PopAndDestroy(); // target
		}
	else if (iStore)
		{
		RStoreWriteStream trg;
		id = trg.CreateLC(aTargetStore);
		CopyStoreL(*iStore,trg);
		CleanupStack::PopAndDestroy(); // trg
		streamDic->AssignL(KUidApaDoorDocStream,id);
		}
	else 
		Panic(EPanicNoModelHeaderWhenStoring); // impossible situation
	//
	// store the stream dictionary and return its stream id
	id = stream.CreateLC(aTargetStore);
	stream<< *streamDic;
	stream.CommitL();
	CleanupStack::PopAndDestroy(2); // stream,streamDic
	return id;
	}


void CApaModelDoor::CopyStoreL(const CEmbeddedStore& aSourceStore,RWriteStream& aTargetStream)
// static method
// copies an embedded store containing a doc to aTargetStream
//
	{
	// read the contents of aSourceStore's rootstream (so I can write it out in a mo')
	CStreamDictionary* root=ReadStreamDictionaryLC(aSourceStore,aSourceStore.Root());
	//
	// copy the source store directly
	MStreamBuf* host=aSourceStore.Host();
	TStreamPos pos=aSourceStore.Position(aSourceStore.Root());
	host->SeekL(host->ERead,EStreamBeginning);
	RReadStream stream(host);
	aTargetStream.WriteL(stream,pos.Offset());
	//
	// write the root stream
	aTargetStream<< *root;
	aTargetStream.CommitL();
	CleanupStack::PopAndDestroy(); // root
	}


EXPORT_C void CApaModelDoor::RestoreL(const CStreamStore& aSourceStore,TStreamId aHeadStreamId,const MApaModelHeaderFactory* aFactory)
/** Restores the embedded model data from the specified store.

@param aStore The store from which the embedded model data is to be restored.
@param aHeadStreamId The stream ID of the head stream for the embedded model 
data. This stream contains the stream dictionary through which the embedded 
model data and its door can be restored.
@param aFactory A pointer to a factory object for constructing the application 
model wrapper object, also known as the application model header, a CApaModelHeader 
type. The application model wrapper is provided by the application model (not 
the application UI), and supplies the knowledge for internalizing the application 
model data from an embedded store. The pointer must not be null, otherwise 
the function raises an APPARC 22 panic. */
	{
	__ASSERT_ALWAYS(aFactory,Panic(EPanicNoFactory));
	//
	delete iStore;
	delete iStoreHost;
	iStore=NULL;
	iStoreHost = NULL;
	//
	// internalize the streamDic from the headstream
	CStreamDictionary* streamDic=ReadStreamDictionaryLC(aSourceStore,aHeadStreamId);
	//
	// internalize the door's state
	TSize size=InternalizeBaseStreamL(aSourceStore,*streamDic);
	SetSizeInTwips(size);
	//
	// internalize the embedded store
	RStoreReadStream src;
	src.OpenL(aSourceStore,streamDic->At(KUidApaDoorDocStream));
	CleanupStack::PopAndDestroy(); // streamDic
	streamDic = NULL;
	iStore = CEmbeddedStore::FromL(src);
	//
	// check for a security stream, then internalize the model
	TRAP_IGNORE(InternalizeModelL(*aFactory)); //lint !e613 Possible use of null pointer - Asserted above
	// ignore any leave, we still have the data in the store to fall back on
	}


void CApaModelDoor::InternalizeModelL(const MApaModelHeaderFactory& aFactory)
// internalizes the model if the data is not encrypted
// if the factory does not recognize the doc type, NewHeaderL() will leave (probably KErrNotSupported)
	{	
	CStreamDictionary* streamDic = ReadStreamDictionaryLC(*iStore,iStore->Root());
	if (streamDic->At(KUidSecurityStream)==KNullStreamId)
		{// not encrypted, so internalize the model
		TApaAppIdentifier appId = CApaProcess::ReadAppIdentifierL(*iStore,*streamDic);
		iModelHeader = aFactory.NewHeaderL(*iStore,*streamDic,appId);
		}
	CleanupStack::PopAndDestroy(); // streamDic
	}


CStreamDictionary* CApaModelDoor::ReadStreamDictionaryLC(const CStreamStore& aSourceStore,TStreamId aStreamId)
// static method
//
	{
	// read the stream dic from the doc's root stream
	CStreamDictionary* streamDic=CStreamDictionary::NewLC();
	RStoreReadStream stream;
	stream.OpenLC(aSourceStore,aStreamId);
	stream>> *streamDic;
	CleanupStack::PopAndDestroy(); // root
	return streamDic;
	}


TSize CApaModelDoor::GlassDoorSize()const
	{
	TSize size;
	GetSizeInTwips(size);
	return size;
	}


EXPORT_C void CApaModelDoor::DetachFromStoreL(TDetach aDegree)
/** Restores the model to the specified degree.

@param aDegree The degree to which restoration is needed. */
	{
	if (iModelHeader)
		{
		iModelHeader->DetachFromStoreL(aDegree);
		if (!iStoreHost)
			{
			delete iStore;
			iStore = NULL;
			}
		}
	else if (!iStoreHost)
		{
		if (aDegree==EDetachDraw)
			{
			delete iStore;
			iStore = NULL;
			// now all I can do is draw as I am, any attempt to change me will result in a panic
			}
		else
			{
			__ASSERT_ALWAYS(iStore,Panic(EPanicNoStoreOnDetach));
			// instantiate the mem buffer, and a stream to write to it
			CBufSeg* bufSeg = CBufSeg::NewL(KHugeGranularity);
			CleanupStack::PushL(bufSeg);
			HBufBuf* buf=HBufBuf::NewL(*bufSeg,0);
			RWriteStream writeStream(buf);
			writeStream.PushL();
			// write the store to the mem buffer
			CopyStoreL(*iStore,writeStream);
			CleanupStack::Pop(2); // bufSeg,writeStream
			//
			// set iStoreHost as host for the embedded store
			MStreamBuf* host=iStore->Host();
			__ASSERT_ALWAYS(host!=NULL,Panic(EDPanicNoHostForStore));
			iStore->Detach();
			host->Release(); //lint !e613 Possible use of null pointer - Asserted above
			iStore->Reattach(buf);
			iStoreHost = bufSeg;
			}
		}
	}



EXPORT_C void CApaModelDoor::Draw(CGraphicsContext& /*aGc*/,const TPoint& /*aTopLeft*/,const TRect& /*aClipRect*/,
						MGraphicsDeviceMap* /*aMap*/)const
/**
Not supported; if called, raises an APPARC 17 panic.
@removed
*/
	{
	Panic(EPanicNotSupported);
	}




EXPORT_C void CApaModelDoor::ExternalizeL(RWriteStream& /*aStream*/)const
/**
Not supported; if called, raises an APPARC 17 panic. 
@removed
*/
	{
	Panic(EPanicNotSupported);
	}


EXPORT_C void CApaModelDoor::GetOriginalSizeInTwips(TSize& aSize)const
/** Gets the door's original size, in twips.

@param aSize This size, in twips. */
	{
	aSize = TSize(500,500); // ?
	}


EXPORT_C void CApaModelDoor::SetScaleFactor(TInt aScaleFactorWidth,TInt aScaleFactorHeight)
/** Sets the door's scale factors.

@param aScaleFactorWidth The width scale factor, in percent.
@param aScaleFactorHeight The height scale factor, in percent. */
	{
	iScaleFactor.iWidth = aScaleFactorWidth;
	iScaleFactor.iHeight = aScaleFactorHeight;
	}


EXPORT_C TInt CApaModelDoor::ScaleFactorWidth()const
/** Gets the door's width scale factor.

@return The width scale factor, in percent. */
	{
	return iScaleFactor.iWidth;
	}


EXPORT_C TInt CApaModelDoor::ScaleFactorHeight()const
/** Gets the door's height scale factor.

@return The height scale factor, in percent. */
	{
	return iScaleFactor.iHeight;
	}

// CApaModelHeader

/** Constructor for CApaModelHeader */
EXPORT_C CApaModelHeader::CApaModelHeader()
	{
	}

/** Reserved for future use */
EXPORT_C void CApaModelHeader::Reserved_1()
	{}

/** Reserved for future use */	
EXPORT_C void CApaModelHeader::Reserved_2()
	{}

// MApaModelHeaderFactory

/** Constructor for MApaModelHeaderFactory */
EXPORT_C MApaModelHeaderFactory::MApaModelHeaderFactory()
	{}

/** Reserved for future use */
EXPORT_C void MApaModelHeaderFactory::MApaModelHeaderFactory_Reserved1()
	{}

/** Reserved for future use */
EXPORT_C void MApaModelHeaderFactory::MApaModelHeaderFactory_Reserved2()
	{}
