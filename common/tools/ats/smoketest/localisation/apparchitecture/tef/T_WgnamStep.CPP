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
// Test accessing window group name.\n
// Tests accessing & modifier functionality of CApaWindowGroupName API.\n
// 
//



/**
 @file
 @internalComponent - Internal Symbian test code 
*/

#include <e32test.h>
#include <apgwgnam.h>
#include <w32std.h>
#include "T_WgnamStep.h"

/**
  Auxiliary Fn for Test Case ID T-WgnamStep-testWgNamesL
 
  This function accepts current window group name as descriptor and prints it.\n
  
*/
void CT_WgnamStep::PrintWindowGroupName(const TPtrC& aWndGroupName)
	{
	_LIT(KWindowGroupName, "Current name=\"%S\"\n");
	TFileName buffer;
	buffer.Format(KWindowGroupName, &aWndGroupName);
	INFO_PRINTF1(buffer);
	}


/**
   @SYMTestCaseID T-WgnamStep-testWgNamesL
  
   @SYMPREQ
  
   @SYMTestCaseDesc Test setting and accessing functionality of Window group name. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions The test checks the following functionality of CApaWindowGroupName\n
   (1) Creation of window group name object.\n
   (2) Setting the task to be a System task.\n
   (3) Setting the task to be Busy.\n
   (4) Setting document name as filename.\n
   (5) Setting the task's capability to shutdown.\n
   (6) Setting the task's capability to switch between files.\n
   (7) Setting the tasks Uid.\n
   (8) Setting the tasks caption.\n
   (9) Setting the tasks Document name.\n
   (10) Setting the task to be ready.\n
   (11) Setting a name to a window group name.\n
   \n
   In each of the cases the setter function is called to set a particular
   attribute. The attribute is cross checked by verifying with its 
   corresponding getter / accessor function. After setting each of the attributes
   the test tries to analyze the impact of the attribute changed, on the windows
   group name.\n
   API Calls:\n	
   CApaWindowGroupName::NewL(const RWsSession& aWsSession)\n
   CApaWindowGroupName::SetSystem(TBool aSystem)\n
   CApaWindowGroupName::IsSystem() const\n
   CApaWindowGroupName::SetBusy(TBool aBusy)\n
   CApaWindowGroupName::IsBusy() const\n
   CApaWindowGroupName::SetDocNameIsAFile(TBool aDocNameIsAFile)\n
   CApaWindowGroupName::DocNameIsAFile() const\n
   CApaWindowGroupName::SetRespondsToShutdownEvent(TBool aRespondsToShutdownEvent)\n
   CApaWindowGroupName::RespondsToShutdownEvent() const\n
   CApaWindowGroupName::SetRespondsToSwitchFilesEvent(TBool aRespondsToSwitchFilesEvent)\n
   CApaWindowGroupName::RespondsToSwitchFilesEvent() const\n
   CApaWindowGroupName::SetAppUid(TUid aAppUid);\n
   CApaWindowGroupName::AppUid() const\n
   CApaWindowGroupName::SetCaptionL(const TDesC& aCaption)\n
   CApaWindowGroupName::Caption() const\n
   CApaWindowGroupName::SetDocNameL(const TDesC& aDocName);\n
   CApaWindowGroupName::DocName() const\n
   CApaWindowGroupName::SetAppReady(TBool aIsReady)\n
   CApaWindowGroupName::IsAppReady()\n
   CApaWindowGroupName::SetWindowGroupName(HBufC* aWgName)\n
   CApaWindowGroupName::WindowGroupName() const\n
   CApaWindowGroupName::FindByCaption(const TDesC& aCaption, RWsSession& aWsSession, TInt& aPrevWgId) const\n
   
   @SYMTestExpectedResults Test checks results against expected values.
    
 */
void CT_WgnamStep::testWgNamesL()
	{
	_LIT(KWindowGroupName1, "WindowGroupName 1");
	_LIT(KWindowGroupName2, "WindowGroupName 2");
	_LIT(KBadName,"00\x00\x31\x31\x31\x31\x31\x31\x31\x31x31\x0\x0");
	
	INFO_PRINTF1(_L("Starting testWgNamesL()"));
	//
	// Instantiate a blank name
	CApaWindowGroupName* apaWindowGroupName=NULL;
	TRAPD(ret, apaWindowGroupName=CApaWindowGroupName::NewL(iWsSession));
	TEST(ret==KErrNone);
	User::LeaveIfError(iWsSession.Connect());
	TEST(apaWindowGroupName!=NULL);

	// test setting system
	INFO_PRINTF1(_L("Block 1"));
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(!apaWindowGroupName->IsSystem());
	INFO_PRINTF1(_L("Setting system\n"));
	apaWindowGroupName->SetSystem(ETrue);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(apaWindowGroupName->IsSystem());
	INFO_PRINTF1(_L("Setting not system\n"));
	apaWindowGroupName->SetSystem(EFalse);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(!apaWindowGroupName->IsSystem());
	INFO_PRINTF1(_L("\n"));
	// User::After(2000000);

	// test setting busy
	INFO_PRINTF1(_L("Block 2"));
	INFO_PRINTF1(_L("Test setting busy"));
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(!apaWindowGroupName->IsBusy());
	INFO_PRINTF1(_L("Setting busy\n"));
	apaWindowGroupName->SetBusy(ETrue);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(apaWindowGroupName->IsBusy());
	INFO_PRINTF1(_L("Setting not busy\n"));
	apaWindowGroupName->SetBusy(EFalse);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(!apaWindowGroupName->IsBusy());
	INFO_PRINTF1(_L("\n"));
	// User::After(2000000);

	// test setting docname not a file
	INFO_PRINTF1(_L("Block 3"));
	INFO_PRINTF1(_L("Test setting doc name is not a file"));
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(apaWindowGroupName->DocNameIsAFile());
	INFO_PRINTF1(_L("Setting doc name not a file\n"));
	apaWindowGroupName->SetDocNameIsAFile(EFalse);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(!apaWindowGroupName->DocNameIsAFile());
	INFO_PRINTF1(_L("Setting doc name is a file\n"));
	apaWindowGroupName->SetDocNameIsAFile(ETrue);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(apaWindowGroupName->DocNameIsAFile());
	INFO_PRINTF1(_L("\n"));
	// User::After(2000000);

	// RDebug::Print(_L("Block 3"));
	INFO_PRINTF1(_L("Test setting does not respond to shutdown event"));
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(apaWindowGroupName->RespondsToShutdownEvent());
	INFO_PRINTF1(_L("Setting does not respond to shutdown event\n"));
	apaWindowGroupName->SetRespondsToShutdownEvent(EFalse);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(!apaWindowGroupName->RespondsToShutdownEvent());
	INFO_PRINTF1(_L("Setting does respond to shutdown event\n"));
	apaWindowGroupName->SetRespondsToShutdownEvent(ETrue);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(apaWindowGroupName->RespondsToShutdownEvent());
	INFO_PRINTF1(_L("\n"));
	// User::After(2000000);

	// test setting does not respond to switch files event
	// RDebug::Print(_L("Block 3"));
	INFO_PRINTF1(_L("Test setting does not respond to switch files event"));
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(apaWindowGroupName->RespondsToSwitchFilesEvent());
	INFO_PRINTF1(_L("Setting does not respond to switch files event\n"));
	apaWindowGroupName->SetRespondsToSwitchFilesEvent(EFalse);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(!apaWindowGroupName->RespondsToSwitchFilesEvent());
	INFO_PRINTF1(_L("Setting does respond to switch files event\n"));
	apaWindowGroupName->SetRespondsToSwitchFilesEvent(ETrue);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(apaWindowGroupName->RespondsToSwitchFilesEvent());
	INFO_PRINTF1(_L("\n"));
	// User::After(2000000);

	// test setting app uid
	// RDebug::Print(_L("Block 5"));
	INFO_PRINTF1(_L("Test setting appUid"));
	const TUid uid={ 0x10000111 };
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	INFO_PRINTF2(_L("Setting app uid to %x\n"), uid.iUid);
	apaWindowGroupName->SetAppUid(uid);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(apaWindowGroupName->AppUid()==uid);
	INFO_PRINTF1(_L("\n"));
	// User::After(2000000);

	// test setting caption
	// RDebug::Print(_L("Block 6"));
	INFO_PRINTF1(_L("Test setting caption"));
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TPtrC caption=_L("App caption");
	INFO_PRINTF2(_L("Setting app caption to %S\n"), &caption);
	apaWindowGroupName->SetCaptionL(caption);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(!(apaWindowGroupName->Caption().Compare(caption)));
	INFO_PRINTF1(_L("\n"));
	// User::After(2000000);
	
	// testing FindByCaption
	INFO_PRINTF1(_L("Test Window group ID"));
	_LIT(KCaption, "App caption");
	TInt aWgId=0;
	apaWindowGroupName->FindByCaption(KCaption, iWsSession, aWgId);
	TEST(aWgId==KErrNotFound);
	INFO_PRINTF3(_L("Expected value is KErrNotfound : %d ,Value Obtained is   : %d \n"),KErrNotFound, aWgId);

	// test setting document name
	// RDebug::Print(_L("Block 7"));
	INFO_PRINTF1(_L("Test setting document name"));
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TPtrC docName=_L("Doc name");
	INFO_PRINTF2(_L("Setting doc name to %S\n"), &docName);
	apaWindowGroupName->SetDocNameL(docName);
	PrintWindowGroupName(apaWindowGroupName->WindowGroupName());
	TEST(!(apaWindowGroupName->DocName().Compare(docName)));
	INFO_PRINTF1(_L("\n"));

	// test AppReady flag
	INFO_PRINTF1(_L("Testing AppReady flag"));
	INFO_PRINTF1(_L("Setting AppReady flag to false"));
	apaWindowGroupName->SetAppReady(EFalse);
	TEST(apaWindowGroupName->IsAppReady()==EFalse);
	INFO_PRINTF1(_L("Setting AppReady flag to true"));
	apaWindowGroupName->SetAppReady(ETrue);
	TEST(!apaWindowGroupName->IsAppReady()==EFalse);
	INFO_PRINTF1(_L("\n"));


	// Testing SetWindowGroup Name
	INFO_PRINTF1(_L("Testing SetWindowGroupName"));
	CApaWindowGroupName* apaWindGroupName = NULL;
	
	TRAP(ret, apaWindGroupName=CApaWindowGroupName::NewL(iWsSession, apaWindowGroupName->WindowGroupName()));
	TEST(ret==KErrNone);
	apaWindGroupName->SetWindowGroupNameL(KWindowGroupName1);
	TEST(apaWindGroupName->WindowGroupName().Compare(KWindowGroupName1)==0);
	HBufC* windGroupName = KWindowGroupName2().AllocLC();
	apaWindGroupName->SetWindowGroupName(windGroupName);
	TEST(apaWindGroupName->WindowGroupName().Compare(KWindowGroupName2)==0);
	
	apaWindGroupName->SetWindowGroupNameL(KBadName);
	const TUid retUid = apaWindGroupName->AppUid();
	TEST(retUid == KNullUid);
	INFO_PRINTF2(_L("Expected Uid: 0, Actual uid = %d\n"), retUid);
	delete apaWindGroupName;

	INFO_PRINTF1(_L("Cleaning up"));
	delete apaWindowGroupName; // apaWindowGroupName

	CleanupStack::Pop();	// windGroupName
	iWsSession.Close();

	INFO_PRINTF1(_L("Finished testWgNamesL()"));
	}



CT_WgnamStep::~CT_WgnamStep()
/**
   Destructor
 */
	{
	}

CT_WgnamStep::CT_WgnamStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_WgnamStep);
	}

TVerdict CT_WgnamStep::doTestStepPreambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_WgnamStep::doTestStepPostambleL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	return TestStepResult();
	}


TVerdict CT_WgnamStep::doTestStepL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
{
	INFO_PRINTF1(_L("Testing window group names"));
	//
	// run the testcode (inside an alloc heaven harness)

	__UHEAP_MARK;
	TRAPD(r,testWgNamesL());
		TEST(r==KErrNone);
	__UHEAP_MARKEND;

	return TestStepResult();
}
