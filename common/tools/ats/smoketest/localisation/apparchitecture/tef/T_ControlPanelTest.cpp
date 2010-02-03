// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests control panel Application Apparc base classes and utility functions to get application's data.\n
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#include <f32file.h>
#include <fbs.h>
#include <s32std.h> 
#include <s32stor.h> 
#include <s32file.h> 

#include <apaid.h>
#include "..\apparc\apadll.h"
#include <apgaplst.h>
#include <apgicnfl.h>
#include <apgdoor.h>
#include <apfrec.h>
#include <apfctlf.h>
#include <apgctl.h>
#include <apgaplst.h>
#include <apaflrec.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <apsserv.h>
#include <appfwk_test_AppUi.h>

#include "T_ControlPanelTest.h"
#include "tstapp.h"
#include <w32std.h>

#if !defined(__E32TEST_H__)
#include <e32test.h>
#endif

_LIT(KCompleted, "Completed.");


_LIT(KRSCDIR,"C:\\Resource\\apps\\");
_LIT(KRSCREGDIR,"C:\\private\\10003a3f\\import\\apps\\");
_LIT(KNEWCTLPATH,"C:\\sys\\bin\\app_CTRL2.exe");
_LIT(KSRCRESOURCEPATH,"Z:\\private\\10003a3f\\import\\apps\\App_CTRL2_reg.Rsc");
_LIT(KDESTRESOURCEPATH,"C:\\private\\10003a3f\\import\\apps\\App_CTRL2_reg.Rsc");

LOCAL_D TInt SimulateKey(TAny*)
	{
	User::After(3000000);
	RWsSession session;
	User::LeaveIfError(session.Connect());	
	//control down
	TRawEvent rawEvent;

	rawEvent.Set(TRawEvent::EKeyDown,EStdKeyLeftCtrl);
	session.SimulateRawEvent(rawEvent);

	// e down
	rawEvent.Set(TRawEvent::EKeyDown,'E');
	session.SimulateRawEvent(rawEvent);

	// e up
	rawEvent.Set(TRawEvent::EKeyUp,'E');
	session.SimulateRawEvent(rawEvent);

	//control up
	rawEvent.Set(TRawEvent::EKeyUp,EStdKeyLeftCtrl);
	session.SimulateRawEvent(rawEvent);

	session.Flush();
	
	session.Close();
	
	return KErrNone;
	}

void CT_ControlPanelTestStep::RemoveFilesFromCDrive()
	{
	TInt ret = iTestServ.SetReadOnly(KDESTRESOURCEPATH,0); //remove READ ONLY option
	TEST(ret==KErrNone);

	TRAP(ret,iTestServ.DeleteFileL(KDESTRESOURCEPATH));
	TEST(ret==KErrNone);
	}



/**
  Auxiliary Fn for Test Case ID T-ControlPanelStep-testControls1L,
  T-ControlPanelStep-testControls2L, T-ControlPanelStep-testControls3L
 
  The method finds the index of control app_ctrl2.exe in the control list.
  Depending on the Boolean value of argument aIsNewPath the search is
  done on ROM / RAM.\n
 
*/

TInt CT_ControlPanelTestStep::ControlIndexInList(CApaSystemControlList* aList, TBool aIsNewPath)
	{
	TFileName ctlPath=_L("Z:\\sys\\bin\\app_CTRL2.exe");
	TFileName newCtlPath=_L("c:\\sys\\bin\\app_ctrl2.exe");
	const TInt count = aList->Count();
	TInt retVal = KErrNotFound;
	for(TInt ii = 0; ii < count; ++ii)
		{		
		if(((aList->Control(ii)->FileName().CompareF(ctlPath)==0) && (!aIsNewPath)) ||
			((aList->Control(ii)->FileName().CompareF(newCtlPath)==0) && (aIsNewPath)))
			{
			retVal = ii;
			
			
			if(aList->Control(ii)->FileName().CompareF(ctlPath)==0)
				{
				INFO_PRINTF1(_L("***Found control on the z: drive"));
				}
			else
				{
				INFO_PRINTF1(_L("***Found control on the c: drive"));
				}
			break;
			}
		}
	INFO_PRINTF2(_L("Out [%d] controlIndexInList"),retVal);	
	return retVal;
	}


/**
   @SYMTestCaseID T-ControlPanelStep-testControls1L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaSystemControlList::UpdateL().
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Check the control list to find the number of controls in the
   list and the number of updates to the list. Call UpdateL(). Check the list
   again for no change in the count of controls and updates to the list.\n
   API Calls:\n	
   CApaSystemControlList::UpdateL()\n
   
   @SYMTestExpectedResults Test confirms that there is no change in the control
   list after updating.
 */
void CT_ControlPanelTestStep::testControls1L()
	{
	INFO_PRINTF1(_L("In testControls1L......"));
	
	iControlCount1 = iControlList->Count();
	TEST((iControlCount1>=1));
	
	TEST((iControlList->UpdateCount()>=1));//update count returns 1 if new list
	//
	// do an update - there should be no changes
	TInt ret;
	INFO_PRINTF1(_L("Updating the list ......"));
	TRAP(ret, iControlList->UpdateL());
	TEST(ret==KErrNone);
	iControlCount2 = iControlList->Count();
	TEST(iControlCount2 == iControlCount1);	
	TEST((iControlList->UpdateCount()>=1));
	iIndex = ControlIndexInList(iControlList,EFalse);
	TEST((iIndex >= 0) && (iIndex < iControlCount2));
	
	INFO_PRINTF1(_L("Out testControls1L......"));
	}


/**
   @SYMTestCaseID T-ControlPanelStep-testControls2L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaSystemControlList::UpdateL() and UpdateCount().
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Copy control from Z: drive to C: drive. Call UpdateL() to
   update the control list. Check if UpdateL() has incremented iUpdateCount
   by calling UpdateCount(). Check the filename of control to ensure that the
   control copied to C: drive has been updated in the list. Delete the newly
   copied control.\n
   API Calls:\n	
   CApaSystemControlList::UpdateL()\n
   CApaSystemControlList::UpdateCount()const\n
   
   @SYMTestExpectedResults The test shows updating of the copied control in control list.
 */

void CT_ControlPanelTestStep::testControls2L()
	{
	INFO_PRINTF1(_L("In testControls2L......"));	
	
	iTestServ.CreateDirectoryL(KRSCDIR);
	iTestServ.CreateDirectoryL(KRSCREGDIR);

	TInt ret=iTestServ.CopyFileL(KSRCRESOURCEPATH,KDESTRESOURCEPATH);
	TEST(ret==KErrNone);
	
	INFO_PRINTF1(_L("Files Copied to C Drive......"));
	INFO_PRINTF1(_L("Updating the list ......"));
	iControlCount=iControlList->UpdateCount();
	while(iControlList->UpdateCount()<=iControlCount)
		{
		TRAP(ret, iControlList->UpdateL());
		if(ret!=KErrNone)
			{
			break;				
			}
		User::After(100000);	
		}

	INFO_PRINTF2(_L("In testControls2L After Controllist update [%d]......"),ret);
	TEST(ret==KErrNone);
	iControlCount3 = iControlList->Count();
	TEST(iControlCount3 == iControlCount1);
	TEST((iControlList->UpdateCount()>=2));
	iIndex = ControlIndexInList(iControlList,ETrue);
	TEST((iIndex >= 0) && (iIndex < iControlCount3));

	TFileName name=iControlList->Control(iIndex)->FileName();
	TEST(name.CompareF(KNEWCTLPATH)==0);
	RemoveFilesFromCDrive();
	INFO_PRINTF1(_L("Removed the file from C Drive......"));
	INFO_PRINTF1(_L("Updating the list ......"));
	iControlCount=iControlList->UpdateCount();
	while(iControlList->UpdateCount()<=iControlCount)
		{
		TRAPD(ret, iControlList->UpdateL());
		if(ret!=KErrNone)
			{
			break;				
			}
		User::After(100000);
		}
	INFO_PRINTF1(_L("Out testControls2L......"));
	}

/**
   @SYMTestCaseID T-ControlPanelStep-testControls3L
  
   @SYMPREQ
  
   @SYMTestCaseDesc Tests CApaSystemControlList::Control() and CApaSystemControl APIs Type() and Caption().
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions Update the control list to ensure that the deleted control is
   removed from the list and the control on the Z: Drive is added to the list.
   Retrieve this control from the list using CApaSystemControlList::Control(). 
   Check the type and caption of the control calling Type() and Caption() methods.\n
   API Calls:\n	
   CApaSystemControlList::Control(TInt aIndex)const\n
   CApaSystemControl::Type()const\n
   CApaSystemControl::Caption()const\n
   
   @SYMTestExpectedResults The test shows the addition of the control present on 
   Z: drive in control list. Test confirms the retrieved info on the control
   obtained from the control list.
  
 */
void CT_ControlPanelTestStep::testControls3L()
	{
	INFO_PRINTF1(_L("In testControls3L......"));
	TFileName ctlPath=_L("Z:\\sys\\bin\\app_ctrl2.exe");
	TFileName ctlCaption=_L("app_ctrl2");
	// This method is called to close the panic dialog generated by previous test
	CloseAllPanicWindowsL();
	
	INFO_PRINTF1(_L("IN testControls3L Before Updating the list......"));
	TRAPD(ret, iControlList->UpdateL());
	
	TEST(ret==KErrNone);
	const TInt controlCount4 = iControlList->Count();
	TEST(controlCount4 == iControlCount1);	// no changes in the count
	TEST((iControlList->UpdateCount()>=3));

	iIndex = ControlIndexInList(iControlList,EFalse);
	TEST((iIndex >= 0) && (iIndex < controlCount4));
	TEST(iControlList->Control(iIndex)->FileName().CompareF(ctlPath)==0);
	
	INFO_PRINTF1(_L("Testing CApaSystemControl"));
	//
	//get the control from the list and check it's type and caption
	iControl=iControlList->Control(iIndex);
	TEST(iControl!=NULL);
	TFileName caption=iControl->Caption();
	TEST(caption.CompareF(ctlCaption)==0);
	TEST(iControl->Icon()!=NULL);
	
	RThread thread;
	TFullName name=_L("Control Panel Test");
	TInt r=thread.Create(name,SimulateKey,KDefaultStackSize,KDefaultStackSize,KDefaultStackSize,NULL);
	TEST(r==KErrNone);
	thread.Resume();
		
	// run the control
	TRAP(ret, iControl->CreateL()); // this call is synchronous
	TEST(ret==KErrNone);
	
	thread.Close();
	INFO_PRINTF1(_L("Out testControls3L......"));
	}



/**
   Auxiliary Fn for T-ControlPanelStep-testControls1L, T-ControlPanelStep-testControls2L, T-ControlPanelStep-testControls3L
   
   This method creates a control list by calling CApaSystemControlList::NewL() and 
   initiates subsequent tests on CApaSystemControlList and CApaSystemControl APIs.
    
 */
void CT_ControlPanelTestStep::testControlsL()
	{
	INFO_PRINTF1(_L("Testing CApaSystemControlList"));
	
	//Create a session with F & B server
	TInt ret=RFbsSession::Connect();
	TEST(!ret);

	TRAP(ret,iControlList=CApaSystemControlList::NewL(iFs));
	TEST(ret==KErrNone);

	TRAPD(err,testControls1L());
	TRAP(err,testControls2L());
	TRAP(err,testControls3L());

	delete iControlList;

	//Close the session F & B server.
	RFbsSession::Disconnect();
	}


/**
  Auxiliary Fn for all Test Cases.
 
  This method creates and installs an active scheduler and puts the
  test code on the scheduler as a CIdle object. The method initiates
  all tests by calling the static method CT_ControlPanelTestCallBack::CallBack().
 
*/
void CT_ControlPanelTestStep::DoStepTestsInCallbackL()
	{
	// create an active scheduler
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	// put the test code onto the scheduler as an idle object
	CIdle* idle=CIdle::NewL(-20);
	CleanupStack::PushL(idle);

	CT_ControlPanelTestCallBack* callBack = new(ELeave) CT_ControlPanelTestCallBack(this);
	CleanupStack::PushL(callBack);

	idle->Start(TCallBack(CT_ControlPanelTestCallBack::CallBack,callBack));
	// start the test code
	CActiveScheduler::Start();

	// all outstanding requests complete - kill the scheduler
	CleanupStack::PopAndDestroy(3); //scheduler, callBack, idle
	}




/**
  Auxiliary Fn for all Test Cases.
 
  The method initiates all tests to be performed.
 
*/
void CT_ControlPanelTestStep::DoStepTests()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Test Controls......"));
	TRAPD(r,testControlsL());
	TEST(r==KErrNone);
	INFO_PRINTF1(KCompleted);
	__UHEAP_MARKEND;
	}


CT_ControlPanelTestStep::~CT_ControlPanelTestStep()
/**
   Destructor
 */
	{
	}

CT_ControlPanelTestStep::CT_ControlPanelTestStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_ControlPanelTest);
	}

TVerdict CT_ControlPanelTestStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_ControlPanelTestStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}


TVerdict CT_ControlPanelTestStep::doTestStepL()
/**
  @return - TVerdict code
  Override of base class virtual
*/
	{
	INFO_PRINTF1(_L("Testing Apparc ...T_ControlPanel"));

	// set up the directory structure
	User::LeaveIfError(iFs.Connect());
	
	// connect to the test utils server
	User::LeaveIfError(iTestServ.Connect());
	
	// Run the tests...w	
	TRAPD(ret,DoStepTestsInCallbackL())
	TEST(ret==KErrNone);

	// Close everything
	iFs.Close();
	iTestServ.Close();

	INFO_PRINTF1(_L("T_ControlPanel Completed."));
	return TestStepResult();
	}


TInt CT_ControlPanelTestCallBack::CallBack(TAny* callBack /*aThis*/)
/**
  This static method is the callback function of CIdle object. The method
  calls the non-static method DoStepTests() which initiates all the tests. 
*/
	{
	
	//Call Test Step func

	((CT_ControlPanelTestCallBack *)callBack)->iTestStep->DoStepTests();
	
	CActiveScheduler::Stop();
	return EFalse; // don't call back again
	}


CT_ControlPanelTestCallBack::CT_ControlPanelTestCallBack(CT_ControlPanelTestStep* aTestStep)
/**
   Constructor
 */
	{
	iTestStep = aTestStep;
	}

CT_ControlPanelTestCallBack::~CT_ControlPanelTestCallBack()
/**
   Destructor
 */
	{

	}
