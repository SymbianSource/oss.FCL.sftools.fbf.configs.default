// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32svr.h>
#include <u32hal.h>
#include <bautils.h>
#include "APSSERV.H"
#include "APFREC.H"
#include "APSSES.H"
#include "APSSTD.H"
#include "../apfile/aprfndr.h"
#include "APGAPLST.H"
#include "APSSCAN.H"
#include "APSSTD.H"
#include "APASVST.H"
#include "datastor.h"
#include "APSRECCACHE.h"
#include "apsnnapps.h"
#include "../apfile/apuninstallmonitor.h"
#include "../apgrfx/apprivate.h"
#include "apsiconcaptionoverride.h"

_LIT(KAppArcServerSemaphore,"AppArcServerSemaphore");
_LIT(KAppArcServerThread,"AppArcServerThread");
_LIT(KAppRegistrationFileImportLocation, "?:\\private\\10003a3f\\import\\apps\\");
_LIT(KAppResourceAppsLocation, "?:\\resource\\apps\\");
_LIT(KNonNativeApplicationTypeRegistry, ":\\private\\10003a3f\\NonNativeTypes.dat");

/*
 * patchable const data values defined in ApsConstData.cpp
 */

IMPORT_C extern const TInt KApaLoadDataRecognizersOnDemand;
IMPORT_C extern const TInt KApaUnloadRecognizersTimeout;
IMPORT_C extern const TInt KApaDrivesToMonitor;
IMPORT_C extern const TInt KApaLoadMbmIconsOnDemand;

const TUint8 KPolicyElementWriteDeviceData = 0;

//To monitor all drives.
const TInt KApaMonitorAllDrives = 0x3FFFFFF;

const TUint KRangeCount = 3; 

const TInt KAppListServRanges[KRangeCount] = 
	{	
	EFirstUnrestrictedOpcodeInAppListServ,
	EFirstOpcodeNeedingWriteDeviceDataInAppListServ, 
	EAppListFirstUnusedOpcode,
	};

const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::EAlwaysPass,			//Always passing no capability required [0-99]
	KPolicyElementWriteDeviceData,		//Requires WriteDeviceData				 [100-(EAppListFirstUnusedOpcode-1)]
	CPolicyServer::ENotSupported, 		//Not Supported		[EAppListFirstUnusedOpcode-End]
	};

const CPolicyServer::TPolicyElement KPolicyElements[] = 
	{ 
	{_INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData), CPolicyServer::EFailClient} 
	};

const CPolicyServer::TPolicy KApaServPolicy =
	{
	CPolicyServer::EAlwaysPass, 
	KRangeCount,
	KAppListServRanges,
	KElementsIndex, 	
	KPolicyElements 	
	};
 	


//////////////////////////////
// CApaAppListServer
//////////////////////////////

const TInt KAppListServerPriority=CActive::EPriorityStandard;
/**
NameApaServStartSemaphore

@internalTechnology
*/
EXPORT_C TPtrC NameApaServStartSemaphore()
	{
	TPtrC nameApaServStartSemaphore(KAppArcServerSemaphore);
	return nameApaServStartSemaphore;
	}

/**
NameApaServServerThread

@internalTechnology 
*/
EXPORT_C TPtrC NameApaServServerThread()
	{
	TPtrC nameApaServServerThread(KAppArcServerThread);
	return nameApaServServerThread;
	}

EXPORT_C CApaAppListServer* CApaAppListServer::Self()
	{ // static
	return static_cast<CApaAppListServer*>(Dll::Tls());
	}

EXPORT_C CApaAppListServer* CApaAppListServer::NewL(MApaAppStarter* aAppStarter)
// Create a new CApaAppListServer which owns it's own resources
	{
	CApaAppListServer* self=new(ELeave) CApaAppListServer(KAppListServerPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aAppStarter);
	CleanupStack::Pop();
	return self;
	}

CApaAppListServer::CApaAppListServer(TInt aPriority)
	: CPolicyServer(aPriority,KApaServPolicy),
	iAppList(0),
	iTypeStoreModified(0),
	iLoadRecognizersOnDemand(KApaLoadDataRecognizersOnDemand),
	iLoadMbmIconsOnDemand(KApaLoadMbmIconsOnDemand)
	{
	
	#ifdef __WINS__
	// KApaLoadDataRecognizersOnDemand and KApaloadIconsOnDemand are Rom patchable constants,
	// so need an emulator equivalent
	// if WINS then read value from epoc.ini
	// requires licencees to set property in epoc.ini

	TInt halValue = 0;
	if (UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty, (TAny*)"patchdata_apserv_dll_KApaLoadDataRecognizersOnDemand", &halValue) == KErrNone)
		{
		iLoadRecognizersOnDemand = halValue;
		}
	if (UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty, (TAny*)"patchdata_apserv_dll_KApaLoadMbmIconsOnDemand", &halValue) == KErrNone)
		{
		iLoadMbmIconsOnDemand = halValue;
		}
	#endif

	__DECLARE_NAME(_S("CApaAppListServer"));
	}

void CApaAppListServer::ConstructL(MApaAppStarter* aAppStarter)
	{
	StartL(KAppListServerName);
	User::LeaveIfError(Dll::SetTls(this));
	User::LeaveIfError(iFs.Connect());
	
	iScanningFileRecognizer=CApaScanningFileRecognizer::NewL(iFs,aAppStarter);
		
	// Get the idle timeout delay from the commandline if specified. The default is 50000ms
	const TInt cmdLineLen = User::CommandLineLength();
	TInt idlePeriodicDelay=50000; //default value
	if(cmdLineLen)
		{
		_LIT(KIdleTimeout,"IDLE_TIMEOUT_PERIODIC_DELAY_");
		
		// Extract the command line into a buffer
		HBufC* cmdLine = HBufC::NewLC(cmdLineLen);
		TPtr cmdLinePtr = cmdLine->Des();
		User::CommandLine(cmdLinePtr);
		cmdLinePtr.UpperCase();
				
		// Check if there is an idle timeout value given
		TInt idleTimeOutArgPos = cmdLinePtr.Find(KIdleTimeout);
		if(KErrNotFound != idleTimeOutArgPos)
			{
			// Extract the value out of the command line argument
			const TInt idleTimeOutValuePos = idleTimeOutArgPos + KIdleTimeout().Length();
			TInt i = idleTimeOutValuePos;
			while (i < cmdLineLen)
				{
				TChar c(cmdLinePtr[i]);
				if (!c.IsDigit())
					{
					break;
					}
				i++;
				}
			TLex idleTimeOutParser(cmdLinePtr.Mid(idleTimeOutValuePos, (i-idleTimeOutValuePos)));
		  	User::LeaveIfError(idleTimeOutParser.Val(idlePeriodicDelay));	  					  	
			}
							
		// Check if the recognition cache should be used or not. By default the cache is used
		iRecognitionCache = 0;
		_LIT(KWithoutRecognitionCache,"WITHOUT_RECOGNITION_CACHE");
		if(KErrNotFound == cmdLinePtr.Find(KWithoutRecognitionCache))
			{
			iRecognitionCache = new (ELeave) CApsRecognitionCache(iFs);
			}
			
		_LIT(KTextShellMode,"TEXT_SHELL_MODE");
		if (KErrNotFound == cmdLinePtr.Find(KTextShellMode))
			{
			// We are NOT running in text shell mode so connect to wserv session
			User::LeaveIfError(iWsSession.Connect());
			iRuleBasedPlugIns = CApaScanningRuleBasedPlugIns::NewL();
			}

		CleanupStack::PopAndDestroy(cmdLine);		
		}
	else
		{
		// no arguments so cache is used
		iRecognitionCache = new (ELeave) CApsRecognitionCache(iFs);
		// and rule based plugins are enabled
		User::LeaveIfError(iWsSession.Connect());
		iRuleBasedPlugIns = CApaScanningRuleBasedPlugIns::NewL();
		}
	
	CApaAppRegFinder* appRegFinder=CApaAppRegFinder::NewL(iFs); //non-standard transfer of ownership, no need for CleanupStack
	iAppList=CApaAppList::NewL(iFs, appRegFinder, iLoadMbmIconsOnDemand, idlePeriodicDelay); // takes ownership of scanner

	// If the phone rebooted halfway through processing updates, there will be a log file left
	// look for one and recover if neccessary
	CApsNonNativeApplicationsManager::RecoverFromUpdateLogL(iFs);

	iDataRecognizer=CApaScanningDataRecognizer::NewL(iFs, !iLoadRecognizersOnDemand);

	ConstructPathsToMonitorL();	

	if ( iAppFsMonitor )
		{
		iAppFsMonitor->Start(ENotifyFile);
		iAppFsMonitor->SetBlocked(ETrue);			
		}
	TRAP_IGNORE(iAppList->InitListL(this));
	
#ifdef USING_ECOM_RECOGS
	iRecEcomMonitor=CApaEComMonitor::NewL(TCallBack(&PlugInNotifyCallBack,this));
	iRecEcomMonitor->Start();
#endif
	iTypeStoreManager=CTypeStoreManager::NewL(iFs);
	TRAPD(err,iTypeStoreManager->RestoreL());
	if(err==KErrNone)
		{
		User::LeaveIfError(iFs.Modified(iTypeStoreManager->IniFileName(),iTypeStoreModified));
		}
	iTypeStoreMonitor=CApaFsMonitor::NewL(iFs,iTypeStoreManager->IniFileName(),TCallBack(&TypeStoreNotifyCallback,this));
	iTypeStoreMonitor->Start(ENotifyWrite); // this presumably needs to be ENotifyWrite rather than ENotifyFile (the latter being used or the other CApaFsMonitor objects) because CTypeStoreManager internally uses CDictionaryFileStore::OpenL, which presumably itself uses RFile::Open, which isn't covered by ENotifyFile according to its documentation
	TypeStoreNotifyCallback(this);
	iBaBackupSessionWrapper=CBaBackupSessionWrapper::NewL();
	iBaBackupSessionWrapper->RegisterBackupOperationObserverL(*((MBackupOperationObserver*)this));

	TChar sysDrive = RFs::GetSystemDriveChar();
	TInt maxSizeofFileName = KNonNativeApplicationTypeRegistry().Length() + 1;
	iNonNativeApplicationTypeRegistry.CreateL(maxSizeofFileName);
	iNonNativeApplicationTypeRegistry.Append(sysDrive);
	iNonNativeApplicationTypeRegistry.Append(KNonNativeApplicationTypeRegistry());

	TRAP_IGNORE(InternalizeNonNativeApplicationTypeArrayL());	// We don't want a corrupt file to prevent from starting
	if(iLoadRecognizersOnDemand)
		{
		iRecognizerUnloadTimer=CPeriodic::NewL(EPriorityNormal);
		}
	iAppUnInstallMonitor = CApaAppUnInstallMonitor::NewL(this);
	iAppUnInstallMonitor->Start();

	iIconCaptionOverrides = new(ELeave) CApaIconCaptionOverrides();
	iIconCaptionObserver = CApaIconCaptionCenrepObserver::NewL(*iIconCaptionOverrides);
	iIconCaptionObserver->LoadOverridesL();
	}
	
void CApaAppListServer::ConstructPathsToMonitorL()
	{
	TInt drivesToMonitor = KApaDrivesToMonitor;
	#ifdef __WINS__
	// KApaDrivesToMonitor is a Rom patchable constant, so need an emulator equivalent
	// if WINS then read value from epoc.ini requires licencees to set property in epoc.ini
	// Usage: In epoc.ini patchdata_apserv_dll_KApaDrivesToMonitor 4
	
	TInt valueOfKApaDrivesToMonitor = 0;
	if (UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"patchdata_apserv_dll_KApaDrivesToMonitor",&valueOfKApaDrivesToMonitor) == KErrNone)
		{
		drivesToMonitor = valueOfKApaDrivesToMonitor;
		}
	#endif
	
	if ( drivesToMonitor != KApaMonitorAllDrives )
		{
		// 1. Create and add CApaFsNotifier for all locations belonging to a drive if the drive is set to monitor.
		// 2. Do this for all the drives which are required to monitor.
		TInt maskBit = 1;
		TChar driveLetter;
		//Only bits from 0(EDriveA)- 25(EDriveZ) are valid other bits are ignored from 32-bit KApaDrivesToMonitor.
		for ( TInt i = EDriveA; i <= EDriveZ; i++ )
			{
			if ( drivesToMonitor & maskBit )
				{
				RFs::DriveToChar(i,driveLetter);
				RBuf pathToBeMonitored;
				TBuf<1> drive;
				drive.Append(driveLetter);
				
				//Creating or Adding <driveLetter>:\private\10003a3f\import\apps\ path to monitor.
				pathToBeMonitored.CreateL(KAppRegistrationFileImportLocation());				
				CleanupClosePushL(pathToBeMonitored);
				pathToBeMonitored.Replace(0,1,drive);				
				if ( iAppFsMonitor == NULL )
					{
					iAppFsMonitor=CApaFsMonitor::NewL(iFs, pathToBeMonitored, TCallBack(&AppFsNotifyCallBack, this));
					}
				else
					{
					iAppFsMonitor->AddLocationL(pathToBeMonitored);		
					}
				CleanupStack::PopAndDestroy(&pathToBeMonitored);
			
				//Adding <driveLetter>:\resource\apps\ path to monitor.
				pathToBeMonitored.CreateL(KAppResourceAppsLocation());
				CleanupClosePushL(pathToBeMonitored);
				pathToBeMonitored.Replace(0,1,drive);			
				iAppFsMonitor->AddLocationL(pathToBeMonitored);
				CleanupStack::PopAndDestroy(&pathToBeMonitored);
				}
			maskBit = maskBit << 1;
			}	
		}
	else
		{
		iAppFsMonitor=CApaFsMonitor::NewL(iFs, KAppRegistrationFileImportLocation, TCallBack(&AppFsNotifyCallBack, this));
		iAppFsMonitor->AddLocationL(KAppResourceAppsLocation);
		}	
	}
	
EXPORT_C CApaAppListServer::~CApaAppListServer()
	{
	delete iIconCaptionObserver;
	delete iIconCaptionOverrides;
	if(iBaBackupSessionWrapper)
		iBaBackupSessionWrapper->DeRegisterBackupOperationObserver(*this);
	delete iAppUnInstallMonitor;
	delete iScanningFileRecognizer;
	delete iAppList; // deletes scanners
	delete iDataRecognizer;
	delete iTypeStoreManager;
	delete iAppFsMonitor;	
	delete iTypeStoreMonitor;
	delete iBaBackupSessionWrapper;
	delete iRecognitionCache;
#ifdef USING_ECOM_RECOGS
	delete iRecEcomMonitor;
#else
	iRecEcomMonitor = NULL;
#endif
	iFs.Close();
	iAppCmdLine=NULL;
	iWsSession.Close();	
	delete 	iRuleBasedPlugIns;
	for (TInt i=iNonNativeApplicationTypeArray.Count()-1; i>=0; --i)
		{
		delete iNonNativeApplicationTypeArray[i].iNativeExecutable;
		}
	iNonNativeApplicationTypeArray.Close();
	iCustomAppList.ResetAndDestroy();
	iCustomAppList.Close();
	delete iRecognizerUnloadTimer;
	iNonNativeApplicationTypeRegistry.Close();
	}

EXPORT_C void CApaAppListServer::HandleStartUninstallEvent()
	{
	if ( iAppFsMonitor )
		{
		iAppFsMonitor->SetBlocked(ETrue);	
		}
	AppList().StopScan();
	}
EXPORT_C void CApaAppListServer::HandleEndUninstallEventL()
	{
	if ( iAppFsMonitor )
		{
		iAppFsMonitor->SetBlocked(EFalse);	
		}
	AppList().RestartScanL();
	}

CSession2* CApaAppListServer::NewSessionL(const TVersion& aVersion,const RMessage2&/* aMessage*/) const

// Create a new server session.
	{
	// check we're the right version
	TVersion v(KAppListServMajorVersionNumber,KAppListServMinorVersionNumber,KAppListServBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);
	// make new session
	return CApaAppListServSession::NewL(const_cast<CApaAppListServer&>(*this), const_cast<RFs&>(iFs));
	}

//
// scanning code here
//

EXPORT_C TCallBack CApaAppListServer::RescanCallBack()
	{
	return TCallBack(&AppFsNotifyWithForcedRegistrationsResetCallBack,this);
	}

TInt CApaAppListServer::AppFsNotifyWithForcedRegistrationsResetCallBack(TAny* aPtr)
	{
	ASSERT(aPtr);
	reinterpret_cast<CApaAppListServer*>(aPtr)->AppList().ResetForcedRegistrations();
	return AppFsNotifyCallBack(aPtr);
	}

TInt CApaAppListServer::AppFsNotifyCallBack(TAny* aObject)
	{
	ASSERT(aObject);
	reinterpret_cast<CApaAppListServer*>(aObject)->UpdateApps();
	return KErrNone;
	}

TInt CApaAppListServer::PlugInNotifyCallBack(TAny* aObject)
	{
	//update the pre-Platform-security style  recognizers and rule-based plug-ins
	ASSERT(aObject);
	reinterpret_cast<CApaAppListServer*>(aObject)->UpdatePlugIns();
	return KErrNone;
	}

TInt CApaAppListServer::TypeStoreNotifyCallback(TAny* aObject)
	{
	ASSERT(aObject);
	reinterpret_cast<CApaAppListServer*>(aObject)->UpdateTypeStore();
	return KErrNone;
	}

void CApaAppListServer::UpdateApps()
// update the list
	{
	// File system change, rescan
	TRAP_IGNORE(iAppList->StartIdleUpdateL (this));
	}

void CApaAppListServer::NotifyUpdate(TInt aReason)
// tell all sessions to update their clients
	{
	// Updates the applist with the icon caption details from the Central Repository.
	TRAP_IGNORE(UpdateAppListByIconCaptionOverridesL());
	// The short caption value sets through the API has got the highest precedence over the
	// values found in either central repository or resource file.
	TRAP_IGNORE(UpdateAppListByShortCaptionL());
		
    //Delete any data mappings in service registry related to uninstalled applications.
    CArrayFixFlat<TUid>* uninstalledApps=iAppList->UninstalledAppArray();
    TBool modificationStatus=EFalse;
    TBool modified;
    
    if(uninstalledApps)
        {
        TInt count=uninstalledApps->Count();
        for(int index=0;index<count;index++)
            {
            modified=iTypeStoreManager->DeleteApplicationDataMappings((*uninstalledApps)[index]);
        
            //Set modificationStatus if any data mapping is removed from the service registry
            if(modified)
                modificationStatus=ETrue;
            }
        
        delete uninstalledApps;
        
        //If service registry is modified, store it to a file
        if(modificationStatus)
            TRAP_IGNORE(iTypeStoreManager->StoreL());
        }
    
	// iterate through sessions
	iSessionIter.SetToFirst();
	CApaAppListServSession* ses=static_cast<CApaAppListServSession*>(&(*iSessionIter++));
	while (ses!=NULL)
		{
		ses->NotifyClients(aReason);	
		ses=static_cast<CApaAppListServSession*>(&(*iSessionIter++));
		}
	}

void CApaAppListServer::UpdatePlugIns()
// update the pre-Platform-security style, ecom style recognizers and rule-based plug-ins
	{
	TRAP_IGNORE(iScanningFileRecognizer->ScanForRecognizersL());
	//we want the recognizers to be loaded either if:
	// 1) they are not loaded on demand
	// 2) they are loaded on demand and actively used
	// 3) they are loaded on demand but waiting for the timer to be unloaded
	if(!iLoadRecognizersOnDemand || iRecognizerUsageCount > 0 || (iRecognizerUnloadTimer && iRecognizerUnloadTimer->IsActive()))
		{
		TRAP_IGNORE(iDataRecognizer->LoadRecognizersL());
		}
	if (iRecognitionCache)
		{	// RecognitionCache is flushed if there are any changes in plugins, i.e.,if:
			// 1. New ECom Plugin is installed.
			// 2. Existing ECom Plugin is uninstalled.
		iRecognitionCache->Flush();
		}

	TRAP_IGNORE(iRuleBasedPlugIns->ScanForRuleBasedPlugInsL());
	}

void CApaAppListServer::UpdateTypeStore()
// Update the internal type store if things have changed
	{
	TRAP_IGNORE(DoUpdateTypeStoreL());
	}

void CApaAppListServer::DoUpdateTypeStoreL()
	{
	TTime modified;
	TInt err=iFs.Modified(iTypeStoreManager->IniFileName(),modified);
	// check the time stamp to see if we are interested in an update
	if (modified>iTypeStoreModified && err==KErrNone)
		{
		CTypeStoreManager* manager=CTypeStoreManager::NewL(iFs);
		CleanupStack::PushL(manager);
		manager->RestoreL();
		iTypeStoreModified=modified;
		delete iTypeStoreManager;
		iTypeStoreManager=manager;
		CleanupStack::Pop(manager); 
		}
	for (iSessionIter.SetToFirst(); ; iSessionIter++) //lint !e1757 prefix operator not defined for TDblQueIter
		{
     	CSession2* const session=iSessionIter;
     	if (session==NULL)
			{
  			break;
			}
     	static_cast<CApaAppListServSession*>(session)->NotifyClientOfDataMappingChange();
		}
	}

void CApaAppListServer::HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)
//
// Handle a signal from the backup server to indicate that a backup has started or finished.
//
	{
	switch(aBackupOperationAttributes.iOperation)
		{
	case MBackupOperationObserver::ENone:
	case MBackupOperationObserver::EAbort:
		break;
	case MBackupOperationObserver::EStart:
		if ( iAppFsMonitor )
			{
			iAppFsMonitor->SetBlocked(ETrue);	
			}
		break;
	case MBackupOperationObserver::EEnd:
		if ( iAppFsMonitor )
			{
			iAppFsMonitor->SetBlocked(EFalse);	
			}
		break;
	default:
		Panic(EEventFromBackupObserverError);
		break;
		}
	}

void CApaAppListServer::InitialListPopulationComplete()
	{
	if ( iAppFsMonitor )
		{
		iAppFsMonitor->SetBlocked(EFalse);	
		}
	
	// notify clients (whoever is interested) that initial population of list is completed
	iSessionIter.SetToFirst();
	CApaAppListServSession* ses=static_cast<CApaAppListServSession*>(&(*iSessionIter++));
	// iterate through sessions
	while (ses!=NULL)
		{
		ses->NotifyClientForCompletionOfListPopulation();	
		ses=static_cast<CApaAppListServSession*>(&(*iSessionIter++));
		}
	}

void CApaAppListServer::RegisterNonNativeApplicationTypeL(TUid aApplicationType, const TDesC& aNativeExecutable)
	{
	for (TInt i=iNonNativeApplicationTypeArray.Count()-1; i>=0; --i)
		{
		if (iNonNativeApplicationTypeArray[i].iTypeUid.iUid==aApplicationType.iUid)
			{
			User::Leave(KErrAlreadyExists);
			}
		}
	SNonNativeApplicationType nonNativeApplicationType;
	nonNativeApplicationType.iTypeUid.iUid=aApplicationType.iUid;
	nonNativeApplicationType.iNativeExecutable=aNativeExecutable.AllocLC();
	iNonNativeApplicationTypeArray.AppendL(nonNativeApplicationType);
	CleanupStack::Pop(nonNativeApplicationType.iNativeExecutable);
	CleanupStack::PushL(TCleanupItem(DeleteLastNonNativeApplicationType, this));
	ExternalizeNonNativeApplicationTypeArrayL();
	CleanupStack::Pop(this); // the TCleanupItem
	}

void CApaAppListServer::DeregisterNonNativeApplicationTypeL(TUid aApplicationType)
	{
	TInt i;
	for (i=iNonNativeApplicationTypeArray.Count()-1; i>=0; --i)
		{
		if (iNonNativeApplicationTypeArray[i].iTypeUid.iUid==aApplicationType.iUid)
			{
			break;
			}
		}
	if (i>=0)
		{
		ExternalizeNonNativeApplicationTypeArrayL(i);
		delete iNonNativeApplicationTypeArray[i].iNativeExecutable;
		iNonNativeApplicationTypeArray[i].iNativeExecutable = NULL;
		iNonNativeApplicationTypeArray.Remove(i);
		}
	}

void CApaAppListServer::InternalizeNonNativeApplicationTypeArrayL()
	{
	RFile file;
	CleanupClosePushL(file);
	const TInt error=file.Open(iFs, iNonNativeApplicationTypeRegistry, EFileShareReadersOnly|EFileStream|EFileRead);
	if (error==KErrNone) // don't leave if the file can't be opened (because it doesn't exist, or because the directory we're looking for it in doesn't exist)
		{
		RFileReadStream sourceStream;
		sourceStream.Attach(file); // file gets closed by this call, but that's okay, we don't need it any more (sourceStream has its own copy of this RFile object that it owns)
		CleanupClosePushL(sourceStream);
		TCardinality arrayCount;
		arrayCount.InternalizeL(sourceStream);
		for (TInt i=0; i<TInt(arrayCount); ++i)
			{
			SNonNativeApplicationType nonNativeApplicationType;
			nonNativeApplicationType.iTypeUid.iUid=sourceStream.ReadUint32L();
			nonNativeApplicationType.iNativeExecutable=HBufC::NewLC(sourceStream, KMaxFileName);
			iNonNativeApplicationTypeArray.AppendL(nonNativeApplicationType);
			CleanupStack::Pop(nonNativeApplicationType.iNativeExecutable);
			}
		CleanupStack::PopAndDestroy(&sourceStream);
		}
	CleanupStack::PopAndDestroy(&file);
	}

void CApaAppListServer::ExternalizeNonNativeApplicationTypeArrayL(TInt aIndexToIgnore/*=-1*/) const
	{
	RFs& fs=const_cast<RFs&>(iFs);
	fs.MkDirAll(iNonNativeApplicationTypeRegistry); // ignore any error
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Replace(fs, iNonNativeApplicationTypeRegistry, EFileShareExclusive|EFileStream|EFileWrite));
	RFileWriteStream targetStream;
	targetStream.Attach(file); // file gets closed by this call, but that's okay, we don't need it any more (targetStream has its own copy of this RFile object that it owns)
	CleanupClosePushL(targetStream);
	const TInt arrayCount(iNonNativeApplicationTypeArray.Count());
	TInt arrayCountToExternalize=arrayCount;
	if (aIndexToIgnore>=0)
		{
		--arrayCountToExternalize;
		}
	TCardinality(arrayCountToExternalize).ExternalizeL(targetStream);
	for (TInt i=0; i<arrayCount; ++i)
		{
		if (i!=aIndexToIgnore)
			{
			const SNonNativeApplicationType& nonNativeApplicationType=iNonNativeApplicationTypeArray[i];
			targetStream.WriteUint32L(nonNativeApplicationType.iTypeUid.iUid);
			targetStream << *nonNativeApplicationType.iNativeExecutable;
			}
		}
	targetStream.CommitL();
	CleanupStack::PopAndDestroy(2, &file);
	}

TPtrC CApaAppListServer::NativeExecutableL(TUid aNonNativeApplicationType) const
	{
	for (TInt i=iNonNativeApplicationTypeArray.Count()-1; i>=0; --i)
		{
		const SNonNativeApplicationType& nonNativeApplicationType=iNonNativeApplicationTypeArray[i];
		if (nonNativeApplicationType.iTypeUid.iUid==aNonNativeApplicationType.iUid)
			{
			return *nonNativeApplicationType.iNativeExecutable;
			}
		}
	User::Leave(KErrNotSupported); // not KErrNotFound
	return KNullDesC();
	}

void CApaAppListServer::DeleteLastNonNativeApplicationType(TAny* aThis)
	{ // static
	CApaAppListServer& self=*static_cast<CApaAppListServer*>(aThis);
	const TInt arrayIndex=self.iNonNativeApplicationTypeArray.Count()-1;
	delete self.iNonNativeApplicationTypeArray[arrayIndex].iNativeExecutable;
	self.iNonNativeApplicationTypeArray[arrayIndex].iNativeExecutable = NULL;
	self.iNonNativeApplicationTypeArray.Remove(arrayIndex);
	}

void CApaAppListServer::NotifyScanComplete()
	{
	// Updates the applist with the icon caption details from the Central Repository.
	TRAP_IGNORE(UpdateAppListByIconCaptionOverridesL());
	// The short caption value sets through the API has got the highest precedence over the
	// values found in either central repository or resource file.		
	TRAP_IGNORE(UpdateAppListByShortCaptionL());

	// iterate through sessions
	iSessionIter.SetToFirst();
	CApaAppListServSession* ses=static_cast<CApaAppListServSession*>(&(*iSessionIter++));
	while (ses!=NULL)
		{
		ses->NotifyScanComplete();	
		ses=static_cast<CApaAppListServSession*>(&(*iSessionIter++));
		}
	}

void CApaAppListServer::AddCustomAppInfoInListL(CCustomAppInfoData* aCustomAppInfo)
	{
	TBool replaced=EFalse;
	for (TInt i=iCustomAppList.Count()-1; i>=0; --i)
		{
		if ((aCustomAppInfo->Uid()==iCustomAppList[i]->Uid()) && (aCustomAppInfo->Language()==iCustomAppList[i]->Language()))
			{
			CCustomAppInfoData* oldAppInfo = iCustomAppList[i];
			iCustomAppList[i]=aCustomAppInfo;
			delete oldAppInfo;
			replaced=ETrue;
			break;
			}
		}
	//Add ELangNone items at the end and others at the beginning
	if(!replaced)
		{
		if(aCustomAppInfo->Language()==ELangNone)
			{
			iCustomAppList.AppendL(aCustomAppInfo);
			}
		else
			{
			iCustomAppList.InsertL(aCustomAppInfo,0);
			}
		}
	
	} //lint !e818 Suppress pointer parameter could be declared as pointing to const - this method takes ownership


void CApaAppListServer::UpdateAppListByShortCaptionL()
	{
	CApaAppData* app=NULL;
	//Items with ELangNone are always found at the end and other languages at the beginning
	//While iterating from end to beginning, we overwrite short caption set by ELangNone if there is any provided for the application language
	for (TInt i=iCustomAppList.Count()-1; i>=0; --i)
		{
		const CCustomAppInfoData* const customAppInfo=iCustomAppList[i];
		app=iAppList->AppDataByUid(customAppInfo->Uid());
		if ( app && ((app->ApplicationLanguage()==customAppInfo->Language()) || (customAppInfo->Language()==ELangNone)))
			{
			app->SetShortCaptionL(*customAppInfo->ShortCaption());
			}
		}
	}

void CApaAppListServer::UpdateAppListByIconCaptionOverridesL()
	{
	// get the language downgrade path, so that most appropriate language can be used
	RArray<TLanguage> languageDowngradePath;
	CleanupClosePushL(languageDowngradePath);
	BaflUtils::GetDowngradePathL(iFs, User::Language(), languageDowngradePath);
	for (CApaAppData* app = iAppList->FirstApp(); app; app = iAppList->NextApp(app))
		{
		CApaIconCaptionOverridesForApp* appOverride = iIconCaptionOverrides->OverrideForApp(app->AppEntry().iUidType[2]);
		if (appOverride)
			{
			TApaIconCaptionOverrideReader overrideReader = appOverride->Reader(languageDowngradePath);
			const TDesC* shortCaption = overrideReader.ShortCaption();
			if (shortCaption)
				app->SetShortCaptionL(*shortCaption);
			const TDesC* caption = overrideReader.Caption();
			if (caption)
				app->SetCaptionL(*caption);
			const TDesC* iconFileName = overrideReader.IconFileName();
			if (overrideReader.NumIconsSet() && iconFileName)
				app->SetIconsL(*iconFileName, overrideReader.NumIcons());
			}
		}
	CleanupStack::PopAndDestroy(&languageDowngradePath);
	}

CCustomAppInfoData* CCustomAppInfoData::NewL(TUid aAppUid, TLanguage aLanguage, const TDesC& aShortCaption)
	{
	CCustomAppInfoData* self=new(ELeave) CCustomAppInfoData(aAppUid, aLanguage);
    CleanupStack::PushL(self);
	self->ConstructL(aShortCaption);
	CleanupStack::Pop(self);
	return self;
	}

CCustomAppInfoData::~CCustomAppInfoData()
	{
	delete iShortCaption;
	}

CCustomAppInfoData::CCustomAppInfoData(TUid aAppUid, TLanguage aLanguage)
	:iUid(aAppUid),
	iLanguage(aLanguage)
	{
	}

void CCustomAppInfoData::ConstructL(const TDesC& aShortCaption)
	{
	iShortCaption=aShortCaption.AllocL();
	}

/*
 * Data Recognizer calls
 */
 
TBool CApaAppListServer::CachedRecognitionResult(const TParseBase& aParser, TDataRecognitionResult& aResult) const
	{
	if(iRecognitionCache && aParser.PathPresent() && aParser.NamePresent())
		{
		return iRecognitionCache->Get(aParser.DriveAndPath(), aParser.NameAndExt(), aResult);
		}
	return EFalse;
	}

/**
N.B. The @c CRecognitionResult object is reference counted so it must be closed!
*/
CRecognitionResult* CApaAppListServer::CachedRecognitionResult(const RFile& aFile, const TParseBase& aParser) const
	{
	if(iRecognitionCache && aParser.PathPresent() && aParser.NamePresent())
		{
		return iRecognitionCache->Get(aFile, aParser.DriveAndPath(), aParser.NameAndExt());
		}
	return NULL;
	}

void CApaAppListServer::CacheRecognitionResultL(const TParseBase& aParser, const TDataRecognitionResult& aResult)
	{
	if(iRecognitionCache && aParser.PathPresent() && aParser.NamePresent())
		{
		iRecognitionCache->AddL(aParser.DriveAndPath(), aParser.NameAndExt(), aResult);
		}
	}

void CApaAppListServer::CacheRecognitionResultL(const RFile& aFile, const TParseBase& aParser, const TDataRecognitionResult& aResult)
	{
	if(iRecognitionCache && aParser.PathPresent() && aParser.NamePresent())
		{
		iRecognitionCache->AddL(aFile, aParser.DriveAndPath(), aParser.NameAndExt(), aResult);
		}
	}

TDataRecognitionResult CApaAppListServer::RecognizeDataL(const TDesC& aName, const TDesC8& aBuffer)
	{
	TParsePtrC parser(iFs.IsValidName(aName) ? aName : KNullDesC);
	TDataRecognitionResult result;
	
	// check cache	
	if(!CachedRecognitionResult(parser, result))
		{
		// recognize
		if(iLoadRecognizersOnDemand)
			{
			LoadRecognizersLC();
			}
		result = iDataRecognizer->RecognizeL(aName, aBuffer);
		if(iLoadRecognizersOnDemand)
			{
			CleanupStack::PopAndDestroy();
			}

		// add to cache
		CacheRecognitionResultL(parser, result);
		}
	
	return result;	
	}

TDataRecognitionResult CApaAppListServer::RecognizeDataL(RFile& aFile, TInt aPreferredBufSize)
	{
	CRecognitionResult* result = RecognizeDataAsCRecognitionResultL(aFile, aPreferredBufSize);
	TDataRecognitionResult ret;
	result->Get(ret);
	result->Close();
	return ret;
	}

/**
Same as @c RecognizeDataL(RFile&, TInt) but returns a @c CRecognitionResult 
instead of a @c TDataRecognitionResult.

N.B. The @c CRecognitionResult object is reference counted so it must be closed!
*/
CRecognitionResult* CApaAppListServer::RecognizeDataAsCRecognitionResultL(RFile& aFile, TInt aPreferredBufSize)
	{
	CRecognitionResult* result = NULL;
	
	TFileName fileName;
	User::LeaveIfError(aFile.FullName(fileName));
	TParsePtrC parser(fileName); //fileName is valid since it comes from RFile

	//check cache
	result = CachedRecognitionResult(aFile,parser);
	if(!result)
		{
		// recognize
		if(iLoadRecognizersOnDemand)
			{
			LoadRecognizersLC();
			}
		const TDataRecognitionResult recResult = iDataRecognizer->RecognizeL(aFile, aPreferredBufSize);
		if(iLoadRecognizersOnDemand)
			{
			CleanupStack::PopAndDestroy();
			}
		
		//add to cache
		CacheRecognitionResultL(aFile, parser, recResult);

		result = CRecognitionResult::NewL(parser.NameAndExt(), recResult);
		}
	
	return result;
	}

TBool CApaAppListServer::RecognizeDataL(const TDesC& aName, const TDesC8& aBuffer, const TDataType& aDataType)
	{
	if(iLoadRecognizersOnDemand)
		{
		LoadRecognizersLC();
		}
	const TBool ret = iDataRecognizer->RecognizeL(aName,aBuffer,aDataType);
	if(iLoadRecognizersOnDemand)
		{
		CleanupStack::PopAndDestroy();
		}
	return ret;
	}

TBool CApaAppListServer::RecognizeDataL(RFile& aFile, TInt aPreferredBufSize, const TDataType& aDataType)
	{
	if(iLoadRecognizersOnDemand)
		{
		LoadRecognizersLC();
		}
	const TBool ret = iDataRecognizer->RecognizeL(aFile,aPreferredBufSize,aDataType);
	if(iLoadRecognizersOnDemand)
		{
		CleanupStack::PopAndDestroy();
		}
	return ret;
	}

TInt CApaAppListServer::DataRecognizerPreferredBufSizeL()
	{
	if(iLoadRecognizersOnDemand)
		{
		LoadRecognizersLC();
		}
	const TInt ret = iDataRecognizer->PreferredBufSize();
	if(iLoadRecognizersOnDemand)
		{
		CleanupStack::PopAndDestroy();
		}
	return ret;
	}

void CApaAppListServer::DataTypeL(CDataTypeArray& aArray)
	{
	if(iLoadRecognizersOnDemand)
		{
		LoadRecognizersLC();
		}
	iDataRecognizer->DataTypeL(aArray);
	if(iLoadRecognizersOnDemand)
		{
		CleanupStack::PopAndDestroy();
		}
	}

/*
 * Recognizer loading/unloading code
 */

void CApaAppListServer::LoadRecognizersLC()
	{
	ASSERT(iLoadRecognizersOnDemand);

	LoadRecognizersL();
	TCleanupItem cleanup(CApaAppListServer::RecognizerCleanup, this);
	CleanupStack::PushL(cleanup);
	}

void CApaAppListServer::RecognizerCleanup(TAny* aSelf)
	{
	if (aSelf)
		{
		static_cast<CApaAppListServer*>(aSelf)->UnloadRecognizers();
		}
	}

void CApaAppListServer::LoadRecognizersL()
	{
	ASSERT(iLoadRecognizersOnDemand);

	if(iRecognizerUnloadTimer->IsActive())
		{
		__ASSERT_DEBUG(iRecognizerUsageCount==0,Panic(EReferenceCountingError1));
		iRecognizerUnloadTimer->Cancel();
		}
	else if(iRecognizerUsageCount==0)
		{
		iDataRecognizer->LoadRecognizersL();
		}

	++iRecognizerUsageCount;
	}

TInt CApaAppListServer::UnloadRecognizers()
	{
	ASSERT(iLoadRecognizersOnDemand);

	--iRecognizerUsageCount;
	__ASSERT_DEBUG(iRecognizerUsageCount>=0,Panic(EReferenceCountingError2));
	if (iRecognizerUsageCount==0)
		{
		iRecognizerUnloadTimer->Start(KApaUnloadRecognizersTimeout,0,TCallBack(CApaAppListServer::DoUnloadRecognizersCallback,this));
		}
	return KErrNone;
	}
	
TInt CApaAppListServer::DoUnloadRecognizersCallback(TAny* aSelf)
	{
	TInt ret=KErrNone;
	if (aSelf)
		{
		ret = static_cast<CApaAppListServer*>(aSelf)->DoUnloadRecognizers();
		}
	return ret;
	}

TInt CApaAppListServer::DoUnloadRecognizers()
	{
	ASSERT(iLoadRecognizersOnDemand);

	// need to cancel the periodic timer since we only want a oneshot timer
	iRecognizerUnloadTimer->Cancel();
	iDataRecognizer->UnloadRecognizers();
	return KErrNone;
	}
	
void CApaAppListServer::GetAppByDataType(const TDataType& aDataType, TUid& aUid) const
	{
	iTypeStoreManager->GetAppByDataType(aDataType, aUid);
	}
	
void CApaAppListServer::GetAppByDataType(const TDataType& aDataType, TUid aServiceUid, TUid& aUid) const
	{
	iTypeStoreManager->GetAppByDataType(aDataType, aServiceUid, aUid);
	}
	
TBool CApaAppListServer::InsertAndStoreIfHigherL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid)
	{
	return iTypeStoreManager->InsertAndStoreIfHigherL(aDataType, aPriority, aUid);
	}
	
void CApaAppListServer::InsertAndStoreDataMappingL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid, TUid aServiceUid)
	{
	iTypeStoreManager->InsertAndStoreDataMappingL(aDataType, aPriority, aUid, aServiceUid);
	}
	
void CApaAppListServer::DeleteAndStoreDataMappingL(const TDataType& aDataType, TUid aServiceUid)
	{
	iTypeStoreManager->DeleteAndStoreDataMappingL(aDataType, aServiceUid);
	}
	
CApaFileRecognizer* CApaAppListServer::FileRecognizer()
	{ 
	return static_cast<CApaFileRecognizer*>(iScanningFileRecognizer); 
	}

TBool CApaAppListServer::LoadMbmIconsOnDemand() const
	{
	return iLoadMbmIconsOnDemand;
	}

#ifdef _DEBUG

/**
Flushes the recognition cache.

Useful for debugging.
*/
void CApaAppListServer::FlushRecognitionCache()
	{
	if(iRecognitionCache)
		iRecognitionCache->Flush();
	}

/**
Sets whether or not recognizers should be loaded when they are needed.

Useful for debugging.
*/	
void CApaAppListServer::SetLoadRecognizersOnDemandL(TBool aLoadRecognizersOnDemand)
	{
	if(iLoadRecognizersOnDemand == aLoadRecognizersOnDemand)
		return;
	
	CPeriodic* newUnloadTimer;
	if(aLoadRecognizersOnDemand)
		{
		ASSERT(!iRecognizerUnloadTimer);
		newUnloadTimer = CPeriodic::NewL(EPriorityNormal);
		}
	else
		{
		ASSERT(iRecognizerUnloadTimer);
		newUnloadTimer = NULL;
		}
	CleanupStack::PushL(newUnloadTimer);
	
	CApaScanningDataRecognizer* newDataRecognizer = CApaScanningDataRecognizer::NewL(iFs,!aLoadRecognizersOnDemand);
	delete iDataRecognizer;
	iDataRecognizer = newDataRecognizer;
	
	delete iRecognizerUnloadTimer;
	iRecognizerUnloadTimer = newUnloadTimer;
	CleanupStack::Pop(newUnloadTimer);
	
	iRecognizerUsageCount = 0;
	iLoadRecognizersOnDemand = aLoadRecognizersOnDemand;	
	}

/**
If recognizers are set to be loaded on demand this method can be used to perform
the unloading synchronously, instead of waiting for the unloading timer to go off.

Useful for debugging.
*/
void CApaAppListServer::PerformOutstandingRecognizerUnloading()
	{
	if(iLoadRecognizersOnDemand && iRecognizerUnloadTimer->IsActive())
		{
		__ASSERT_DEBUG(iRecognizerUsageCount==0,Panic(EReferenceCountingError3));
		DoUnloadRecognizers();
		}
	}

#endif //_DEBUG
