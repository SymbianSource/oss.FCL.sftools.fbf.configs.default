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

#include <s32stor.h>
#include <fbs.h>

#include "APGDOOR.H"
#include "APGICNFL.H"
#include "APGCLI.H"
#include "APACLN.H"
#include "APGSTD.H"
#include "APGPRIV.H"
#include "APFDEF.H"

#include "../apparc/TRACE.H"

const TUid KUidApaDoorStateStream={0x1000012a}; // build invariant
const TInt KHugeGranularity=4096; // 4k granularity for the door's host buffer
// default icon size only used if door was created by a model door, set to glass, but does not support glass
#define KDefaultIconSizeInTwips TSize(500,500)

///////////////////////////////////////////
// CApaDoor
///////////////////////////////////////////

EXPORT_C CApaDoor* CApaDoor::NewLC(RFs& aFs, CApaDocument& aDoc,const TSize& aDefaultIconSizeInTwips)
/** Creates a new wrapper for the specified embedded document and places a
pointer to it onto the cleanup stack.

The door is of the specified size. The wrapper takes ownership of the document; 
if creation of the wrapper fails, the document object is destroyed.

@param aFs A file server session.
@param aDoc The document for which the door is to be created.
@param aDefaultIconSizeInTwips The size of the door in twips.
@return  The new embedded document wrapper object. */
	{
	CApaDoor* self=new CApaDoor(aFs, aDoc,aDefaultIconSizeInTwips);
	if (!self)
		{
		__ASSERT_DEBUG(aDoc.Process(), Panic(EDPanicNoProcess));
		aDoc.Process()->DestroyDocument(&aDoc);
		User::LeaveNoMemory();
		}
	CleanupStack::PushL(self);
	self->ConstructL(); //lint !e613 Possible use of null pointer - Lint is wrong, see User::LeaveNoMemory() above
	return self;
	}


EXPORT_C CApaDoor* CApaDoor::NewL(RFs& aFs, CApaDocument& aDoc,const TSize& aDefaultIconSizeInTwips)
/** Creates a new wrapper for the specified embedded document.

The door is of the specified size. The wrapper takes ownership of the document; 
if creation of the wrapper fails, the document object is destroyed.

@param aFs A file server session.
@param aDoc The document for which the door is to be created.
@param aDefaultIconSizeInTwips The size of the door in twips.
@return  The new embedded document wrapper object. */
	{
	CApaDoor* self = CApaDoor::NewLC(aFs, aDoc,aDefaultIconSizeInTwips);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CApaDoor* CApaDoor::NewL(RFs& aFs, const CStreamStore& aStore,TStreamId aStreamId,CApaProcess& aProcess)
// Creates a door and restores it
// Should only be called by the TApaPictureFactory
//
	{
	CApaDoor* self=new(ELeave) CApaDoor(aFs, aProcess);
	CleanupStack::PushL(self);
	self->RestoreL(aStore,aStreamId);
	CleanupStack::Pop();
	return self;
	}


CApaDoor::CApaDoor(RFs& aFs, CApaDocument& aDoc,const TSize& aDefaultIconSizeInTwips)
	:iFs(aFs),
	iApaProcess(aDoc.Process()),
	iApaDoc(&aDoc),
	iIconSizeInTwips(aDefaultIconSizeInTwips)
	{__DECLARE_NAME(_S("CApaDoor"));}


CApaDoor::CApaDoor(RFs& aFs, CApaProcess& aProcess)
	:iFs(aFs),
	iApaProcess(&aProcess)
	{__DECLARE_NAME(_S("CApaDoor"));}


EXPORT_C CApaDoor::~CApaDoor()
/** Destructor.

Frees all resources owned by the object, prior to its destruction. In particular, 
it destroys the document, removing all references to it from the application 
process. */
	{
	delete iPicture;
	if (iApaDoc)
		iApaProcess->DestroyDocument(iApaDoc); // removes it from the process and deletes it
	delete iAppCaption;
	delete iStore;
	delete iStoreHost;
	iApaProcess = NULL;
	iApaDoc = NULL;
	}


void CApaDoor::ConstructL()
	{
	__SHOW_TRACE(_L("Starting CApaDoor::ConstructL"));
	__APA_PROFILE_START(3);
	__ASSERT_ALWAYS(iApaDoc,Panic(EPanicNoDocumentOnConstruction));
	//
	// check that the doc supports embedding
	TApaAppCapabilityBuf buf;
	__ASSERT_DEBUG(iApaDoc->Application(), Panic(EDPanicNoApplication));
	iApaDoc->Application()->Capability(buf);
	if (buf().iEmbeddability==TApaAppCapability::ENotEmbeddable)
		User::Leave(KErrNotSupported);
	//
	__SHOW_TRACE(_L("...doc is embeddable"));
	//
	// set up the icon
	SetFormatToIconL();
	//
	__PROFILE_END(3);
	}

TStreamId CApaDoor::StoreL(CStreamStore& aTargetStore)const
/** Stores the embedded document in the specified store as an embedded store.

This function saves the format of the door. It also stores the document, if 
the document exists in memory, otherwise, it simply copies the stream containing 
the embedded document into the specified store.

@param aStore The store into which the embedded document is to be stored.
@return The stream ID of the head stream for the embedded document. This stream 
contains the stream dictionary through which the embedded document and its 
door can be restored. */
	{
	__SHOW_TRACE(_L("Starting CApaDoor::StoreL"));
	__APA_PROFILE_START(4);
	// create stream dictionary
	CStreamDictionary* streamDic = CStreamDictionary::NewLC();
	//
	// stream out door's state
	ExternalizeBaseStreamL(aTargetStore,*streamDic);
	ExternalizeStateStreamL(aTargetStore,*streamDic);
	//
	// store the doc if it exists, otherwise copy the persistent data directly
	TStreamId id;
	RStoreWriteStream stream;
	if (iApaDoc)
		{
		// create an embedded store in a new write stream
		id = stream.CreateL(aTargetStore);
		CEmbeddedStore* target=CEmbeddedStore::NewLC(stream); // takes ownership of stream
		streamDic->AssignL(KUidApaDoorDocStream,id);
		StoreDocL(*target);
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
		Panic(EPanicNoDocOrStoreWhenStoring); // impossible situation
	//
	// store the stream dictionary and return its stream id
	id = stream.CreateLC(aTargetStore);
	stream<< *streamDic;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	//
	// tidy up
	CleanupStack::PopAndDestroy(); // streamDic
	__PROFILE_END(4);
	return id;
	}


void CApaDoor::CopyStoreL(const CEmbeddedStore& aSourceStore,RWriteStream& aTargetStream)
// static method
// copies an embedded store containing a doc to aTargetStream
//
	{
	__SHOW_TRACE(_L("Starting CApaDoor::CopyStoreL"));
	// read the contents of aSourceStore's rootstream (so I can write it out in a mo')
	CStreamDictionary* root=ReadStreamDictionaryLC(aSourceStore,aSourceStore.Root());
	//
	// copy the source store directly
	MStreamBuf* host=aSourceStore.Host();
	__ASSERT_DEBUG(host, Panic(EDPanicNoHostForStore));
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

EXPORT_C void CApaDoor::RestoreL(const CStreamStore& aSourceStore,TStreamId aStreamId)
/** Restores the embedded document from the specified store.

The format of the door is set to iconic if the embedded document is password 
protected.

@param aStore The store from which the embedded document is to be restored. 
@param aHeadStreamId The stream ID of the head stream for the embedded document. 
This stream contains the stream dictionary through which the embedded document 
and its door can be restored. */
	{
	__SHOW_TRACE(_L("Starting CApaDoor::RestoreL"));
	__APA_PROFILE_START(5);
	__ASSERT_DEBUG(iApaProcess,Panic(EDPanicNoProcess));
	//
	if (iApaDoc)
		{
		iApaProcess->DestroyDocument(iApaDoc);
		iApaDoc = NULL;
		}
	delete iStore;
	delete iStoreHost;
	iStore=NULL;
	iStoreHost = NULL;
	//
	// internalize the streamDic from the headstream
	CStreamDictionary* streamDic=ReadStreamDictionaryLC(aSourceStore,aStreamId);
	//
	// internalize the door's state
	__APA_PROFILE_START(13);
	TSize currentSize=InternalizeBaseStreamL(aSourceStore,*streamDic);
	InternalizeStateStreamL(aSourceStore,*streamDic,currentSize);
	__APA_PROFILE_END(13);
	//
	// internalize the embedded store
	__APA_PROFILE_START(14);
	RStoreReadStream src;
	src.OpenL(aSourceStore,streamDic->At(KUidApaDoorDocStream));
	iStore = CEmbeddedStore::FromL(src);
	CleanupStack::PopAndDestroy(); // streamDic
	streamDic = NULL;
	__APA_PROFILE_END(14);
	//
	// internalize the doc's stream dict
	streamDic = ReadStreamDictionaryLC(*iStore,iStore->Root());
	//
	// set the door's format 
	if (iFormat==EIconic || (streamDic->At(KUidSecurityStream)!=KNullStreamId))
		// iconify automatically if a password is required for access
		SetFormatToIconL();
	else
		{
		TRAPD(ret, SetFormatToGlassL() );
		if (ret==KErrNone)
			{
			__ASSERT_DEBUG(iPicture, Panic(EDPanicNoPictureOnDrawing));
			iPicture->SetSizeInTwips(currentSize);
			}
		else if (ret!=KErrNoMemory)
			// problem loading app/doc - just iconify it for now...
			SetFormatToIconL();
		else
			User::Leave(ret);
		}	
	CleanupStack::PopAndDestroy(); // streamDic
	__PROFILE_END(5);
	}


void CApaDoor::StoreDocL(CPersistentStore& aTargetStore)const
// stores the doc if it's in memory, otherwise panics!
// aStore should be protected before calling this method
//
	{
	__SHOW_TRACE(_L("Starting CApaDoor::StoreDocL"));
	__ASSERT_ALWAYS(iApaDoc,Panic(EPanicNoDocumentOnStore)); // the doc must be in memory to be stored
	//
	// create a stream dic
	CStreamDictionary* streamDic=CStreamDictionary::NewLC();
	// store the doc
	iApaDoc->StoreL(aTargetStore,*streamDic);
	// write store's root stream
	CApaProcess::WriteRootStreamL(aTargetStore,*streamDic,*iApaDoc->Application());
	// tidy up
	CleanupStack::PopAndDestroy(); // streamDic
	}


void CApaDoor::RestoreDocL(const CPersistentStore& aSourceStore)
// restores the document from the embedded store
// leaves with KErrNotFound if the app dll cant be located
//
	{
	__SHOW_TRACE(_L("Starting CApaDoor::RestoreDocL"));
	__ASSERT_ALWAYS(!iApaDoc,Panic(EPanicDocAlreadyExists));
	//
	// read the stream dic from the doc's root stream
	CStreamDictionary* streamDic=ReadStreamDictionaryLC(aSourceStore,aSourceStore.Root());
	//
	// read the app id from the store
	TApaAppIdentifier appId = CApaProcess::ReadAppIdentifierL(aSourceStore,*streamDic);
	//
	__ASSERT_DEBUG(iApaProcess,Panic(EDPanicNoProcess));
	// if the app exists find it, load it and create a doc, else leave if the correct app cannot be found
	CApaDocument* doc = // create an unrestored doc and adds it to the process list
	iApaProcess->AddNewDocumentL(appId.iAppUid);
	TApaDocCleanupItem cleanup(iApaProcess,doc);
	CleanupStack::PushL(cleanup);
	doc->RestoreL(aSourceStore,*streamDic); // restores the doc
	iApaDoc = doc;
	CleanupStack::Pop(); // doc
	CleanupStack::PopAndDestroy(); // streamDic
	}


CStreamDictionary* CApaDoor::ReadStreamDictionaryLC(const CStreamStore& aSourceStore,TStreamId aStreamId)
// static method
//
	{
	__APA_PROFILE_START(12);
	// read the stream dic from the doc's root stream
	CStreamDictionary* streamDic=CStreamDictionary::NewLC();
	RStoreReadStream stream;
	stream.OpenLC(aSourceStore,aStreamId);
	stream>> *streamDic;
	CleanupStack::PopAndDestroy(); // root
	__APA_PROFILE_END(12);
	return streamDic;
	}


void CApaDoor::ExternalizeL(RWriteStream& /*aStream*/)const
	{
	Panic(EPanicMethodNotSupported);
	}


void CApaDoor::ExternalizeStateStreamL(CStreamStore& aStore,CStreamDictionary& aStreamDict)const
	{
	__SHOW_TRACE(_L("Starting CApaDoor::ExternalizeStateStreamL"));
	__ASSERT_ALWAYS(iAppCaption,Panic(EPanicNoCaption));
	__ASSERT_ALWAYS(iPicture,Panic(EPanicNoPictureInDoor));
	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(aStore);
	//
	stream<< *iAppCaption;
	TSize size;
	if (iFormat==EIconic || iFormat==ETemporarilyIconic)
		GetSizeInTwips(size);
	else 
		size = iIconSizeInTwips;
	stream<< size;
	//
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	aStreamDict.AssignL(KUidApaDoorStateStream,id);
	}


void CApaDoor::InternalizeStateStreamL(const CStreamStore& aStore,const CStreamDictionary& aStreamDict,TSize aDefaultIconSize)
	{
	__SHOW_TRACE(_L("Starting CApaDoor::InternalizeStateStreamL"));
	TStreamId id=aStreamDict.At(KUidApaDoorStateStream);
	if (id!=KNullStreamId)
		{
		RStoreReadStream stream;
		stream.OpenLC(aStore,id);
		TApaAppCaption caption;
		stream>> caption;
		delete iAppCaption;
		iAppCaption = NULL;
		iAppCaption = caption.AllocL();
		stream>> iIconSizeInTwips;
		CleanupStack::PopAndDestroy(); // stream
		}
	else
		{// use default settings
		delete iAppCaption;
		iAppCaption = NULL;
		iAppCaption = HBufC::NewL(0);
		if (iFormat==EIconic)
			iIconSizeInTwips = aDefaultIconSize;
		else
			iIconSizeInTwips = KDefaultIconSizeInTwips;
		}
	}


void CApaDoor::DetachFromStoreL(TDetach aDegree)
/** Detaches the door from its store, restoring any unrestored elements of the picture, 
if necessary.

@param aDegree Degree to which picture is detached.
@see CApaDocument::DetachFromStoreL() */
	{
	__SHOW_TRACE(_L("Starting CApaDoor::DetachFromStoreL"));
	if (iApaDoc)
		{
		iApaDoc->DetachFromStoreL(aDegree);
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
			host->Release(); //lint !e613 Suppress possible use of null pointer
			iStore->Reattach(buf);
			iStoreHost = bufSeg;
			}
		}
	}


EXPORT_C CApaDocument* CApaDoor::DocumentL(TBool aCheckPassword)
// leaves with KErrNotFound if the doc needs to be restored but the app dll cannot be found
//
/** Returns a pointer to the embedded document represented by this wrapper.

If necessary, the document is restored from its embedded store.

Note that if the wrapper does not have a reference to the embedded document 
store, then the function raises a APGRFX 13 panic. Constructing this wrapper 
through a TApaPictureFactory or storing the embedded document through CApaDoor::StoreL() 
ensures that this wrapper has a reference to the embedded document store.

@param aCheckPassword If ETrue, any password is checked before returning a 
pointer to the document. If EFalse, the password is not checked.
@return A pointer to the embedded document.
@see TApaPictureFactory
@see CApaDoor::StoreL()
@see CApaDocument::ValidatePasswordL() */
	{
	__SHOW_TRACE(_L("Starting CApaDoor::DocumentL"));
	//
	if (!iApaDoc)
		{
		__ASSERT_ALWAYS(iStore,Panic(EPanicNoStoreOnRestore));
		RestoreDocL(*iStore);
		}
	else if (aCheckPassword)
		iApaDoc->ValidatePasswordL();
	//
	return iApaDoc;
	}

EXPORT_C void CApaDoor::SetFormatToTemporaryIconL(TBool aEnabled)
// if the door is currently iconic do nothing
// if the door is glass switch it's format to iconic, but ensure that when externalized the format will be persisted as glass
//
/** Switches the format of the door between temporarily iconic and glass.

If the door is iconic, then the function does nothing.

@param aEnabled If ETrue and the format is currently glass, then the format 
switches to temporarily iconic; this is the default. If EFalse and the format 
is currently temporarily iconic, then the format switches to glass. */
	{
	__SHOW_TRACE(_L("Starting CApaDoor::SetFormatToTemporaryIconL"));
	if (aEnabled && iFormat==EGlassDoor) 
		{
		TSize glassSize;
		GetSizeInTwips(glassSize);
		SetFormatToIconL();
		iFormat = ETemporarilyIconic;
		iIconSizeInTwips = glassSize; //abuse it!
		}
	else if (!aEnabled && iFormat==ETemporarilyIconic)
		SetFormatToGlassL();
	}

#ifdef __VC32__
#pragma optimize("g", off) // Disable due to problem with MSVC
#endif
EXPORT_C void CApaDoor::SetFormatToIconL()
/** Sets the format of the door to iconic.

The application's icon is used, or, if this cannot be found, the default icon 
is used instead. The function leaves only if construction of the default icon 
object fails. */
	{
	__SHOW_TRACE(_L("Starting CApaDoor::SetFormatToIconL"));
	__APA_PROFILE_START(6);
	if (iFormat==ETemporarilyIconic && iPicture)
		{
		GetSizeInTwips(iIconSizeInTwips);
		iFormat = EIconic;
		}
	else if (iFormat!=EIconic || !iPicture)
		{
		TUid appUid;
		if (iApaDoc)
			{
			__ASSERT_DEBUG(iApaDoc->Application(), Panic(EDPanicNoApplication));
			appUid = iApaDoc->Application()->AppDllUid();
			}
		else
			{
			appUid = AppUidFromStreamL();
			}

		TInt ret=KErrNone;
		if (!iAppCaption)
			{
			RApaLsSession ls;
			CleanupClosePushL(ls);
			ret=ls.Connect();
			if (ret==KErrNone)
				{
				TApaAppInfo info;
				ret=ls.GetAppInfo(info,appUid);
				if (ret==KErrNone)
					{
					iAppCaption = info.iCaption.AllocL();
					}
				}
			CleanupStack::PopAndDestroy(&ls);
			}

		if (!iAppCaption)	// no caption found
			iAppCaption = HBufC::NewL(0);

		CPicture* icon=CApaIconPicture::NewL(iIconSizeInTwips, appUid);
		delete iPicture;
		iPicture = icon;
		iFormat = EIconic;
		}
	__PROFILE_END(6);
	}
#ifdef __VC32__
#pragma optimize("g", on)
#endif


TUid CApaDoor::AppUidFromStreamL() const
	{
	__ASSERT_DEBUG(iStore,Panic(EDPanicNoStoreOnIconify));
	__APA_PROFILE_START(15);
	CStreamDictionary* streamDic=ReadStreamDictionaryLC(*iStore,iStore->Root());
	TApaAppIdentifier appId=CApaProcess::ReadAppIdentifierL(*iStore,*streamDic);
	CleanupStack::PopAndDestroy(streamDic);
	__APA_PROFILE_END(15);
	return appId.iAppUid;
	}
	

EXPORT_C void CApaDoor::SetFormatToGlassL()
/** Sets the format of the door to glass.

The function asks the document to create a fresh copy of the door and destroys 
any existing copy. If the process of creating the door completes without leaving, 
but returns a zero pointer, then the function raises an APGRFX 17 panic.

The function leaves with:

KErrNotSupported, if the document does not support being represented by a 
glass door.

KErrNotFound, if the application DLL cannot be found.

If the function leaves, the format remains unchanged.

@see CApaDocument::GlassPictureL() */
	{
	__SHOW_TRACE(_L("Starting CApaDoor::SetFormatToGlassL"));
	__APA_PROFILE_START(7);
	if (iFormat!=EGlassDoor || !iPicture)
		{
		if (!iApaDoc)
			{
			__ASSERT_DEBUG(iStore,Panic(EDPanicNoStoreOnGlassing));
			RestoreDocL(*iStore);
			}
		if (iApaDoc->Capability().CanDrawGlass())
			{
			CPicture* glass = iApaDoc->GlassPictureL();
			__ASSERT_ALWAYS(glass,Panic(EPanicNoGlassPicture));
			if (iPicture)
				iPicture->GetSizeInTwips(iIconSizeInTwips); // store the current icon size
			delete iPicture;
			iPicture = glass;
			iFormat = EGlassDoor;
			}
		else
			User::Leave(KErrNotSupported); // glass pic's not supported
		}
	__PROFILE_END(7);
	}


EXPORT_C TUid CApaDoor::AppUidL()const
/** Gets the application specific UID associated with the embedded document.

@return The application specific UID. */
	{
	if (iApaDoc)
		{
		__ASSERT_DEBUG(iApaDoc->Application(), Panic(EDPanicNoApplication));
		return iApaDoc->Application()->AppDllUid();
		}
	//
	__ASSERT_ALWAYS(iStore,Panic(EPanicNoStoreOnAppUid));
	//
	// read uid from store's headstream
	CStreamDictionary* streamDic = ReadStreamDictionaryLC(*iStore,iStore->Root());
	TApaAppIdentifier appId = CApaProcess::ReadAppIdentifierL(*iStore,*streamDic);
	CleanupStack::PopAndDestroy(); // streamDic
	return appId.iAppUid;
	}
	

void CApaDoor::Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
						MGraphicsDeviceMap* aMap)const
/** Draws the door either as glass or as an icon depending on the format.

@param aGc The graphics context.
@param aTopLeft The co-ordinates where the top left corner pixel of the picture 
should be placed. Note that whether this is actually drawn depends on the 
clipping area defined.
@param aClipRect A clipping rectangle.
@param aMap The device map for the graphics device. */
	{
	__ASSERT_DEBUG(iPicture,Panic(EDPanicNoPictureOnDrawing));
	//
	iPicture->Draw(aGc,aTopLeft,aClipRect,aMap);
	}

void CApaDoor::GetOriginalSizeInTwips(TSize& aSize)const
/** Get the door's original size, in twips.

@param aSize The size, in twips. */
	{
	__ASSERT_DEBUG(iPicture,Panic(EDPanicNoPicture));
	iPicture->GetOriginalSizeInTwips(aSize);
	}

void CApaDoor::SetScaleFactor(TInt aScaleFactorWidth,TInt aScaleFactorHeight)
/** Sets the door's scale factors.

@param aScaleFactorWidth The width scale factor, in percent.
@param aScaleFactorHeight The height scale factor, in percent. */
	{
	__ASSERT_DEBUG(iPicture,Panic(EDPanicNoPicture));
	iPicture->SetScaleFactor(aScaleFactorWidth,aScaleFactorHeight);
	}

TInt CApaDoor::ScaleFactorWidth()const
/** Gets the door's width scale factor.

@return The width scale factor, in percent. */
	{
	__ASSERT_DEBUG(iPicture,Panic(EDPanicNoPicture));
	return iPicture->ScaleFactorWidth();
	}

TInt CApaDoor::ScaleFactorHeight()const
/** Gets the door's height scale factor.

@return The height scale factor, in percent. */
	{
	__ASSERT_DEBUG(iPicture,Panic(EDPanicNoPicture));
	return iPicture->ScaleFactorHeight();
	}

void CApaDoor::SetCropInTwips(const TMargins& aMargins)
/** Sets the cropping margins of a picture in twips.

These are relative to the original unscaled size of the picture.

@param aMargins The cropping margins, in twips. */
	{
	__ASSERT_DEBUG(iPicture,Panic(EDPanicNoPicture));
	iPicture->SetCropInTwips(aMargins);
	}

void CApaDoor::GetCropInTwips(TMargins& aMargins)const
/** Gets the cropping margins of the door in twips.

These margins are relative to the original unscaled size of the picture.

@param aMargins The cropping margins, in twips. */
	{
	__ASSERT_DEBUG(iPicture,Panic(EDPanicNoPicture));
	iPicture->GetCropInTwips(aMargins);
	}

TPictureCapability CApaDoor::Capability()const
/** Gets the picture's capabilities.

These include whether it is scalable and croppable.

@return The capabilities of the picture. */
	{
	__ASSERT_DEBUG(iPicture,Panic(EDPanicNoPicture));
	return iPicture->Capability();
	}

TSize CApaDoor::GlassDoorSize()const
	{
	TSize size;
	if (iFormat==EGlassDoor)
		GetSizeInTwips(size);
	else if (iFormat==ETemporarilyIconic)
		{
		if (!iApaDoc)
			size = iIconSizeInTwips;
		else
			{
			// there's a doc, so get a glass door from it just in case it's size has changed since I changed format
			CPicture* glass = NULL;
			TRAP_IGNORE(glass = iApaDoc->GlassPictureL()); 
			__ASSERT_ALWAYS(glass,Panic(EPanicNoGlassPicture));
			glass->GetSizeInTwips(size); //lint !e613 Possible use of null pointer - Asserted above
			delete glass;
			}
		}
	else
		Panic(EIllegalCallToGlassDoorSize);
	return size;
	}


void CApaDoor::SetIconSizeInTwips(TSize aSize)
// for use of factory
	{
	if (iFormat==EGlassDoor)
		iIconSizeInTwips = aSize;
	else
		SetSizeInTwips(aSize);
	}


///////////////////////////////////
// TApaPictureFactory
///////////////////////////////////

#define KDoNotApplyIconSize TSize(-1,-1)

/** Constructor for TApaPictureFactory */
EXPORT_C TApaPictureFactory::TApaPictureFactory()
	:iApaProcess(NULL),
	iIconSize(TSize(0,0))
	{
	}

EXPORT_C TApaPictureFactory::TApaPictureFactory(CApaProcess* aAppProcess)
	:iApaProcess(aAppProcess),
	iIconSize(KDoNotApplyIconSize)
/** Constructs a door factory object for the specified application process.

@param aAppProcess The application process. */
	{}

EXPORT_C void TApaPictureFactory::NewPictureL(TPictureHeader& aPictureHeader,const CStreamStore& aPictureStore)const
// called (by the containing doc) to restore an app door from its header
//
/** Constructs and restores an application's door (picture) from a stream in the 
specified store.

The restored door is a CApaDoor type object.

Note that the function can leave with KErrNoMemory if creation of the CApaDoor 
object fails.

@param aPictureHeader The header identifying the door to be restored. The 
UID identifying the door must be KUidPictureTypeDoor, otherwise the function 
leaves with KErrNotSupported. On entry, the door picture must be represented 
by a stream ID, otherwise the function leaves with KErrBadHandle; on return, 
the door picture is represented by a pointer to an internalized CApaDoor object.
@param aPictureStore The store from which the door will be restored.
@see TPictureHeader
@see TPictureHeader::iPicture */
	{
	__SHOW_TRACE(_L("Starting TApaPictureFactory::NewPictureL"));
	if (aPictureHeader.iPictureType!=KUidPictureTypeDoor)
		User::Leave(KErrNotSupported); // wrong type
	if (!aPictureHeader.iPicture.IsId())
		User::Leave(KErrBadHandle); // not an id - can't restore
	//
	// create and restore the door
	TStreamId id = aPictureHeader.iPicture.AsId();
//	RFs fs;
//	User::LeaveIfError(fs.Connect());
//	CleanupClosePushL(fs);
	__ASSERT_DEBUG(iApaProcess, Panic(EDPanicNoProcess));
	if(iApaProcess)
		{
		CApaDoor* door = CApaDoor::NewL(iApaProcess->FsSession()/*fs*/,aPictureStore,id,*CONST_CAST(CApaProcess*,iApaProcess));
		aPictureHeader.iPicture = door;
		//
		// set the icon size if requested
		if (iIconSize!=KDoNotApplyIconSize)
			door->SetIconSizeInTwips(iIconSize);
		}
//	CleanupStack::PopAndDestroy(); // fs - it's not needed any more as the base class doesn't use it
	}


////////////////////////////////////
// HBufBuf
////////////////////////////////////

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
