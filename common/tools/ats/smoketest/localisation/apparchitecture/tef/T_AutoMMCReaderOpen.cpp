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
// This is a defect fix for the following defect	
// INC040672 - MMC Card door open causes wrong mime type instead of error code	
// This test is fully automated and performs the following task
// Tests to access a file which is on MMC and the MMC card is removed at the time of access.
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#include "testableapalssession.h"
#include "T_AutoMMCReaderOpen.h"

const TInt KDirLength = 5; // No need of length more than this , hence length is 5

CTestAutoMMCReaderStep::~CTestAutoMMCReaderStep()
	{
	}

CTestAutoMMCReaderStep::CTestAutoMMCReaderStep()
	{
	SetTestStepName(KTestAutoMMCReaderOpenStep);
	}

TVerdict CTestAutoMMCReaderStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestAutoMMCReaderStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}

TVerdict CTestAutoMMCReaderStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Apparch...T_AutoMMCReaderOpen"));
		
	OpenMMCCardReaderDrive();
	INFO_PRINTF1(_L("\nT_AutoMMCReaderOpen Completed."));
	return TestStepResult();
	}

void CTestAutoMMCReaderStep::OpenMMCCardReaderDrive()
	{
	INFO_PRINTF1(_L("Testing fix for INC040672 - MMC Card door open causes wrong mime type instead of error code"));
	
	_LIT(KValidFileName, "ValidFileName.jpg");
		
	RFs fs;
	TInt ret = fs.Connect();
	TEST(ret == KErrNone);
	RTestableApaLsSession ls;
	ret = ls.Connect();
	TEST(ret == KErrNone);
		
//	The following steps are to reproduce the removal of MMC card
// 	Loop through drive letters Y -> D to access a file on that drive (any filename)
//	Find a drive letter that returns KErrNotReady , 
//	setup a valid filename with that drive. 

	for (TInt drive = EDriveY; drive >= EDriveD ; drive--)
		{
		TDriveUnit driveUnit(drive);	
		TDriveName driveName = driveUnit.Name();

		TBuf<KDirLength> bufDirName(driveName);
		bufDirName.Append(_L("\\"));
		
		ret = fs.MkDirAll(bufDirName);
		if (ret == KErrNotReady)
			{
			TFileName bufValidFileName(bufDirName);
			bufValidFileName.Append(KValidFileName);
			TDataType dataType;
			TUid uid = KNullUid;
			
			HEAP_TEST_LS_SESSION(ls, 0, 0, ret = ls.AppForDocument(bufValidFileName, uid, dataType), NO_CLEANUP);
			TEST(ret == KErrNotReady);
			TEST(uid == KNullUid);
			TEST(dataType == TDataType());
			break;
			}
		}
	TEST(ret == KErrNotReady);
	ls.Close();
	fs.Close();
	}
