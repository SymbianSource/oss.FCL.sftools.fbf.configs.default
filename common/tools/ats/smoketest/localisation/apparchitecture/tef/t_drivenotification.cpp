// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Only in Armv5 platform,
// C: Drive is set to be monitored using KApaDrivesToMonitor. The value of 
// KApaDrivesToMonitor is set to 4. It is computed in the following way.
// ZY XWVU TSRQ PONM LKJI HGFE DCBA
// 00 0000 0000 0000 0000 0000 0100. 
// If the value is Set then the corresponding drive has to be monitored.
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32test.h>
#include "appfwk_test_utils.h"
#include "t_drivenotification.h"

_LIT(KResourceFileSourceZ, "z:\\system\\data\\tnotifydrivesapp_reg.rsc");

void CDriveTestObserver::HandleAppListEvent(TInt /*aEvent*/)
	{
	iNotified++;
	if (iNotifier)
		iNotifier->Cancel();
	CActiveScheduler::Stop();
	}

CT_DriveNotificationStep::~CT_DriveNotificationStep()
	{	
	}

CT_DriveNotificationStep::CT_DriveNotificationStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_DriveNotificationStep);
	}
	
/**
   @SYMTestCaseID	APPFWK-APPARC-0083
  
   @SYMCR 			CR1035
  
   @SYMTestCaseDesc Test for the drive's notification.
   
   @SYMTestPriority High 
  
   @SYMTestStatus 	Implemented
   
   @SYMTestActions 	In armv5 platform, the patchable constant KApaDrivesToMonitor is set to monitor C: drive. 
   Hence the notifications from other drives are ignored. The test copies the tnotifydrivesapp_reg.rsc to either 
   D: or E: drive and also to C:. There will be no notification from D: or E: drives and the applist wont be 
   updated whereas from C: notifications will be issued and applist will be updated.
   In Winscw platform, all the drives will be monitored by default and notifications will be issued when the test
   copies tnotifydrivesapp_reg.rsc to C: drive and W: drive and the applist will be updated.
   
   @SYMTestExpectedResults The app is not added to the applist when no notification is issued and applist is 
   updated when notifications are issued.
*/    
void CT_DriveNotificationStep::TestDrivesNotificationL()
	{
	INFO_PRINTF1(_L("Start TestDriveNotification()"));
	
	TApaAppInfo appInfo;
	TUid appUid = {0xA0003376};
	RApaLsSession theLs;
	User::LeaveIfError(theLs.Connect());
	CleanupClosePushL(theLs);
	
	// Wait for applist to be updated.... 
	RPointerArray<TDesC> dummy;
	User::LeaveIfError(theLs.ForceRegistration(dummy));
	
	//Check whether app is not present in the applist
	TInt ret = theLs.GetAppInfo(appInfo,appUid);
	INFO_PRINTF3(_L(" Expected value is %d, Call to GetAppInfo returned : %d  "), KErrNotFound, ret);
	TEST(ret==KErrNotFound);
	
	//Copy the registration file to C: drive.
	CopyRegFileL(EDriveC);
	
	CDriveTestObserver* obs = new(ELeave) CDriveTestObserver();
	CleanupStack::PushL(obs);
	CApaAppListNotifier* notif = CApaAppListNotifier::NewL(obs, CActive::EPriorityHigh);
	CleanupStack::PushL(notif);
	
	obs->iNotifier = notif;
		
	CActiveScheduler::Start();
	//Since c:\\private\\10003a3f\\Import\\apps\\ path is Monitored, a notification is issued and applist is updated.
	TEST(obs->iNotified > 0);	
	INFO_PRINTF2(_L("Received %d notifications"),obs->iNotified);
	
	//Check whether applist is updated and app is present in the applist.
	ret = theLs.GetAppInfo(appInfo,appUid);
	INFO_PRINTF3(_L(" Expected value is %d, Call to GetAppInfo returned : %d  "), KErrNone, ret);
	TEST(ret==KErrNone);
	
	//Deleting the rsc file that is present in c:\\private\\10003a3f\\Import\\apps\\ path.
	DeleteRegFileL(EDriveC);
	
	// Wait for applist to be updated.... 
	User::LeaveIfError(theLs.ForceRegistration(dummy));
	
	//Check whether applist is updated and app is absent in the applist.
	ret = theLs.GetAppInfo(appInfo,appUid);
	INFO_PRINTF3(_L(" Expected value is %d, Call to GetAppInfo returned : %d  "), KErrNotFound, ret);
	TEST(ret==KErrNotFound);
		
	//Armv5 platform, GetMMCDriveL() returns either the D: (in case of NAND rom) or E: (in case of Normal rom).
	//Winscw platform, GetMMCDriveL() returns W:.
	TDriveNumber drive = GetMMCDriveL();
	
	//Copy the registration file to drive specified.
	CopyRegFileL(drive);
	
	// Wait for applist to be updated.... 
	User::LeaveIfError(theLs.ForceRegistration(dummy));
	
	//Check whether applist is updated and app is present in the applist.
#ifdef __EABI__
	ret = theLs.GetAppInfo(appInfo,appUid);
	TEST (ret == KErrNotFound);
	INFO_PRINTF3(_L(" Expected value is %d, Call to GetAppInfo returned : %d  "), KErrNotFound, ret);
	//Deleting the rsc file.
	DeleteRegFileL(drive);
#else	
	ret = theLs.GetAppInfo(appInfo,appUid);
	TEST(ret==KErrNone);
	INFO_PRINTF3(_L(" Expected value is %d, Call to GetAppInfo returned : %d  "),KErrNone, ret);
	//Deleting the rsc file.
	DeleteRegFileL(drive);
#endif
	CleanupStack::PopAndDestroy(3, &theLs);
	
	INFO_PRINTF1(_L("End TestDrivesNotificationL()"));
	}
/*
Copy the tnotifydrivesapp_reg.rsc file from source to destination. 
In Winscw platform, the registration file is copied to C: and W: drive.
In Armv5 platform, the registration file is copied to C: and D: or E: drive. 
*/

void CT_DriveNotificationStep::CopyRegFileL(TDriveNumber aDriveNumber)
	{
	_LIT(KResourceFileTarget, ":\\private\\10003a3f\\Import\\apps\\tnotifydrivesapp_reg.rsc");	
	_LIT(KResourceAppFolder, ":\\private\\10003a3f\\Import\\apps\\");
	RBuf tempPathToBeCopied, tempFolderToBeCreated;
	RFs fs;
	TChar driveLetter;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	TInt maxSizeofFileName = KResourceFileTarget().Length() + 1;
	TInt maxSizeofFolderName = KResourceAppFolder().Length() + 1;
	tempPathToBeCopied.CreateL(maxSizeofFileName);
	CleanupClosePushL(tempPathToBeCopied);
	tempFolderToBeCreated.CreateL(maxSizeofFolderName);
	CleanupClosePushL(tempFolderToBeCreated);
	fs.DriveToChar(aDriveNumber,driveLetter);
	tempPathToBeCopied.Append(driveLetter);
	tempPathToBeCopied.Append(KResourceFileTarget);
	tempFolderToBeCreated.Append(driveLetter);
	tempFolderToBeCreated.Append(KResourceAppFolder);
	RSmlTestUtils smlServer;
	CleanupClosePushL(smlServer);
	TInt ret = smlServer.Connect();
	TEST(ret == KErrNone);
	ret = smlServer.CreateDirectoryL(tempFolderToBeCreated);
	if (ret != KErrNone && ret != KErrAlreadyExists)
		{
		User::LeaveIfError(ret);
		}
	ret = smlServer.CopyFileL(KResourceFileSourceZ, tempPathToBeCopied);
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("Copied Registration file. Finished with the value : %d "), ret);
	CleanupStack::PopAndDestroy(4, &fs);
	}



void CT_DriveNotificationStep::DeleteRegFileL(TDriveNumber aDriveNumber)
	{
	_LIT(KResourceFileTarget, ":\\private\\10003a3f\\Import\\apps\\tnotifydrivesapp_reg.rsc");	
	RBuf tempPathToBeDeleted;
	RFs fs;
	TChar driveLetter;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	TInt maxSizeofFileName = KResourceFileTarget().Length() + 1;
	tempPathToBeDeleted.CreateL(maxSizeofFileName);
	CleanupClosePushL(tempPathToBeDeleted);
	fs.DriveToChar(aDriveNumber,driveLetter);
	tempPathToBeDeleted.Append(driveLetter);
	tempPathToBeDeleted.Append(KResourceFileTarget);
	RSmlTestUtils smlServer;
	CleanupClosePushL(smlServer);
	TInt ret = smlServer.Connect();
	TEST(ret == KErrNone);
	ret = smlServer.SetReadOnly(tempPathToBeDeleted, 0)	;
	TEST(ret == KErrNone);
	//delete the file
	ret = smlServer.DeleteFileL(tempPathToBeDeleted);	
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("Deleted Registration file. Finished with value : %d"), ret);	
	CleanupStack::PopAndDestroy(3,&fs);
	}
	

TDriveNumber CT_DriveNotificationStep::GetMMCDriveL()
	{
	TDriveNumber drive = EDriveW;	
#ifdef __EABI__
	RFs	fs;
 	User::LeaveIfError(fs.Connect());
	// The removable media is expected at D: on NAND ROM and at E: on normal ROMs.
	//The following code works on techview but not guaranteed to work on all platforms. 
	TDriveInfo driveInfo;
	TInt err = fs.Drive(driveInfo, EDriveD);
	if(err == KErrNone && ((driveInfo.iDriveAtt & KDriveAttRemovable) != 0))
		{
	 	 // Use drive D
	 	drive = EDriveD;
	 	}
	 else
	 	{
		err = fs.Drive(driveInfo, EDriveE);
		if(err == KErrNone && ((driveInfo.iDriveAtt & KDriveAttRemovable) != 0))
			{
		 	// Use drive E
		 	drive = EDriveE;
		 	}
		}
	fs.Close();
#else
	drive = EDriveW;
#endif	
	return drive;
	}


TVerdict CT_DriveNotificationStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing the Notifications ... T_DriveNotification"));
	
	// start an active scheduler
	iScheduler=new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	
	// run the testcode (inside an alloc heaven harness)
 	__UHEAP_MARK;
 	 	
 	TRAPD(ret,TestDrivesNotificationL());
	TEST(ret==KErrNone);
	INFO_PRINTF2(_L("TestDrivesNotificationL finished with code '%d'\n"), ret);
 	
  	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("T_DriveNotification Completed."));
	return TestStepResult();
	}
