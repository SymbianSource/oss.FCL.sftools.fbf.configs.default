// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The following tests are performed to test the Child behaviour on its Parent's termination.
// 
//



/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "T_ProcStep.h"
const TInt KTProcTerminatingChildI = 1246;
const TInt KTProcTerminatingChildII = 1247;
const TInt KTProcTerminatingChildIII = 1248;
const TInt KTProcTerminatingParent = 1249;

CChildProcess* CChildProcess::NewL(RProcess& aParentProcess,TProcessId aChildProcessId)
	{
	CChildProcess* self= new(ELeave) CChildProcess(aParentProcess,aChildProcessId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CChildProcess::~CChildProcess()
/**
 * Destructor
 */
	{
	Cancel();
	}

void CChildProcess::ConstructL()
/**
 * Sets active, so that control goes into RunL() 
 * where parent is terminated and child process logged on.
 */	
 	{
	User::LeaveIfError(iChildProcess.Open(iChildProcessId));
	SetActive();
	TRequestStatus *status=&iStatus;
	User::RequestComplete(status,KErrNone);
	CActiveScheduler::Start();
	}

CChildProcess::CChildProcess(RProcess& aParentProcess,TProcessId aChildProcessId):CActive(0),iChildProcessId(aChildProcessId),iParentProcess(aParentProcess),iCount(0)
	{
	CActiveScheduler::Add(this);
	}

void CChildProcess::RunL()
/**
 * Opens the parent process, on failure leaves.
 * Logs on the child process status, sets it active and terminates the parent process.
 * When child terminates scheduler is stopped.
 */
	{
	switch(iCount)
		{
		case 0:
			iCount++;
			iChildProcess.Logon(iStatus);
			SetActive();
			iParentProcess.Terminate(KErrNone);
			break;
		case 1:
			CActiveScheduler::Stop();
		default:
			break;
		}
	}

void CChildProcess::DoCancel()
	{
	switch(iCount)
		{
		case 0:
			iChildProcess.LogonCancel(iStatus);
			iCount++;
			break;
		case 1:
			iChildProcess.LogonCancel(iStatus);
			break;
		default:
			break;
		}
	}


////////////////////////////////////////////////////////////////////////////
//
// T_ProcStep.cpp 
// ------------
//
// Implements the test cases to test Child behaviour on Parent termination.
//
////////////////////////////////////////////////////////////////////////////

CT_ProcStep::CT_ProcStep()
/**
 * Constructor
 */
	{
	}


CT_ProcStep::~CT_ProcStep()
/**
 * Destructor
 */
	{
	}

/**
 @SYMTestCaseID	APPFWK-APPARC-0015

 @SYMPREQ	PREQ1123

 @SYMTestCaseDesc The Test determines that the Child dies when its Parent is terminated.

 @SYMTestPriority Critical

 @SYMTestStatus Implemented

 @SYMTestActions. Creates and launches a process (parent). Creates another process (child) passing the first(parent) process ID.
 Launches the child process. Terminates parent and checks the existance of child process. The child should die.
 API Calls:\n
 RProcess::Create(const TDesC &aFileName, const TDesC &aCommand, TOwnerType aType=EOwnerProcess);\n
 RProcess::Resume();
 RProcess::ExitType() const;
 RProcess::ExitReason() const;
 RProcess::Id() const;
 CApaCommandLine::SetProcessEnvironmentL(RProcess &aProcess) const;
 CApaCommandLine::NewLC();
 CApaCommandLine::SetParentProcessId(TProcessId);

 @SYMTestExpectedResults Termination of child process automatically.\n

 */
void CT_ProcStep::testChildSetToTerminateL(void)
	{
	TInt ret(0);

	//commandline for parent process
	CApaCommandLine* parentProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Parent Process created "));

	//parent process
	RProcess parentProc;
	ret = parentProc.Create(KParentExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(parentProc);
	INFO_PRINTF2(_L("	Create Parent Process returned : %d"),ret);

	//attach commandline to parent process
	TRAP(ret,parentProcCmdln->SetProcessEnvironmentL(parentProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	INFO_PRINTF1(_L("	Run Parent Process "));
	parentProc.Resume();
	//Time for the parent process to launch itself
	User::After(500000);

	//commandline for child process
	//Get parent process ID here
	TUint64 parentProcId = parentProc.Id();
	INFO_PRINTF2(_L("	Parent Process Id = 0x%lx "),parentProcId);

	CApaCommandLine* childProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child Process created "));

	//setting the parent process ID to child
	childProcCmdln->SetParentProcessId(parentProcId);
	INFO_PRINTF1(_L("	Set ParentProcessId to Child "));

	//child process
	RProcess childProc;
	ret = childProc.Create(KChildOneExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProc);
	
	INFO_PRINTF2(_L("	Create Child Process returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcCmdln->SetProcessEnvironmentL(childProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	INFO_PRINTF1(_L("	Run Child Process "));
	childProc.Resume();
	//Time for the child process to launch itself
	User::After(500000);

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcId = childProc.Id();
	INFO_PRINTF2(_L("	Child Process Id = 0x%lx "),childProcId);

	CChildProcess* childProcess=NULL;
	TRAP(ret, childProcess=CChildProcess::NewL(parentProc,childProcId));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupStack::PushL(childProcess);

	TExitType exitType = parentProc.ExitType();
	TEST(exitType == EExitTerminate);
	TInt exitReason = parentProc.ExitReason();
	TEST(exitReason == 0);
	if(exitType==EExitTerminate && exitReason==0)
		{
		INFO_PRINTF1(_L("	Parent process is Terminated"));
		}
	
	exitType = childProc.ExitType();
	TEST(exitType == EExitTerminate);
	exitReason = childProc.ExitReason();
	TEST(exitReason == 0);
	if(exitType == EExitTerminate && exitReason==0)
		{
		INFO_PRINTF1(_L("	The child process is killed automatically ... "));
		}

	CleanupStack::PopAndDestroy(childProcess);
	CleanupStack::PopAndDestroy(&childProc);
	CleanupStack::PopAndDestroy(childProcCmdln);
	CleanupStack::PopAndDestroy(&parentProc);
	CleanupStack::PopAndDestroy(parentProcCmdln);
	}

/**
 @SYMTestCaseID APPFWK-APPARC-0016

 @SYMPREQ PREQ1123

 @SYMTestCaseDesc Test determines that the Child remains when its Parent is terminated.

 @SYMTestPriority Critical

 @SYMTestStatus Implemented

 @SYMTestActions. Creates and launches a process (parent). Creates another process (child) without passing the first(parent) process ID.
 Launches the child process. Terminates parent and checks the existance of child process. The child process should be alive.
 API Calls:\n
 RProcess::Create(const TDesC &aFileName, const TDesC &aCommand, TOwnerType aType=EOwnerProcess);\n
 RProcess::Resume();
 RProcess::ExitType() const;
 RProcess::ExitReason() const;
 RProcess::Id() const;
 RProcess::Terminate(TInt aReason);
 CApaCommandLine::SetProcessEnvironmentL(RProcess &aProcess) const;
 CApaCommandLine::NewLC();

 @SYMTestExpectedResults Existence of child process.\n

 */
void CT_ProcStep::testChildSetToRemainL(void)
	{
	TInt ret(0);

	//commandline for parent process
	CApaCommandLine* parentProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Parent Process created "));

	//parent process
	RProcess parentProc;
	ret = parentProc.Create(KParentExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(parentProc);
	INFO_PRINTF2(_L("	Create Parent Process returned : %d"),ret);

	//attach commandline to parent process
	TRAP(ret,parentProcCmdln->SetProcessEnvironmentL(parentProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	INFO_PRINTF1(_L("	Run Parent Process "));
	parentProc.Resume();
	//Time for the parent process to launch itself
	User::After(500000);

	//commandline for child process
	//Get parent process ID here
	TUint64 parentProcId = parentProc.Id();
	INFO_PRINTF2(_L("	Parent Process Id = 0x%lx "),parentProcId);

	CApaCommandLine* childProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child Process created "));

	//child process
	RProcess childProc;
	ret = childProc.Create(KChildOneExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProc);

	INFO_PRINTF2(_L("	Create Child Process returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcCmdln->SetProcessEnvironmentL(childProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	INFO_PRINTF1(_L("	Run Child Process "));
	childProc.Resume();
	//Time for the child process to launch itself
	User::After(500000);

	parentProc.Terminate(KTProcTerminatingParent);
		INFO_PRINTF1(_L("	Kill Parent Process ... "));
	TExitType exitType = parentProc.ExitType();
	TEST(exitType == EExitTerminate);
	TInt exitReason = parentProc.ExitReason();
	TEST(exitReason == KTProcTerminatingParent);

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcId = childProc.Id();
	INFO_PRINTF2(_L("	Child Process Id = 0x%lx "),childProcId);

	//Wait for child to terminate ... if it really does
	User::After(10000000);

	exitType = childProc.ExitType();
	TEST(exitType == EExitPending);
	if(exitType == EExitPending)
		{
		INFO_PRINTF1(_L("	Child process is still running "));
		INFO_PRINTF1(_L("	so terminating it manually ..."));
		childProc.Terminate(KTProcTerminatingChildI);
		exitType = childProc.ExitType();
		TEST(exitType == EExitTerminate);
		exitReason = childProc.ExitReason();
		TEST(exitReason == KTProcTerminatingChildI);
		}

	CleanupStack::PopAndDestroy(&childProc);
	CleanupStack::PopAndDestroy(childProcCmdln);
	CleanupStack::PopAndDestroy(&parentProc);
	CleanupStack::PopAndDestroy(parentProcCmdln);
	}

/**
 @SYMTestCaseID APPFWK-APPARC-0017

 @SYMPREQ PREQ1123

 @SYMTestCaseDesc Test determines that one Child remains and another terminates when their Parent terminates, based on their creation.

 @SYMTestPriority High

 @SYMTestStatus Implemented

 @SYMTestActions. Creates and launches a process (parent). Creates a process (child I) passing the first (parent) process ID.
 Creates a second process (child II) without passing the first (parent) process ID. Launches both the child processes.
 Terminates parent and checks the existance of both the child processes. Child I should die and Child II should remain alive.
 API Calls:\n
 RProcess::Create(const TDesC &aFileName, const TDesC &aCommand, TOwnerType aType=EOwnerProcess);\n
 RProcess::Resume();
 RProcess::ExitType() const;
 RProcess::ExitReason() const;
 RProcess::Id() const;
 RProcess::Terminate(TInt aReason);
 CApaCommandLine::SetProcessEnvironmentL(RProcess &aProcess) const;
 CApaCommandLine::NewLC();
 CApaCommandLine::SetParentProcessId(TProcessId);
 
 @SYMTestExpectedResults Termination of first child process and existence of the second.\n

 */
void CT_ProcStep::testTwoChildsOneToTerminateAndOtherToRemainL(void)
	{
	TInt ret(0);
	TExitType exitType;

	//commandline for parent process
	CApaCommandLine* parentProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Parent Process created "));

	//parent process
	RProcess parentProc;
	ret = parentProc.Create(KParentExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(parentProc);
	INFO_PRINTF2(_L("	Create Parent Process returned : %d"),ret);

	//attach commandline to parent process
	TRAP(ret,parentProcCmdln->SetProcessEnvironmentL(parentProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	INFO_PRINTF1(_L("	Run Parent Process "));
	parentProc.Resume();
	//Time for the parent process to launch itself
	User::After(500000);

	//commandline for child process
	//Get parent process ID here
	TUint64 parentProcId = parentProc.Id();
	INFO_PRINTF2(_L("	Parent Process Id = 0x%lx"),parentProcId);

	//For Child ONE
	CApaCommandLine* childProcOneCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child One created "));

	//setting the parent process ID to child I
	childProcOneCmdln->SetParentProcessId(parentProcId);
	INFO_PRINTF1(_L("	Set ParentProcessId to Child One "));

	//child process ONE
	RProcess childProcOne;
	ret = childProcOne.Create(KChildOneExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProcOne);
	
	INFO_PRINTF2(_L("	Create Child One returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcOneCmdln->SetProcessEnvironmentL(childProcOne));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine of Child One to its Process "));

	INFO_PRINTF1(_L("	Run Child One "));
	childProcOne.Resume();
	//Time for the child one to launch itself
	User::After(500000);

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcOneId = childProcOne.Id();
	INFO_PRINTF2(_L("	Child One Id = 0x%lx "),childProcOneId);

	//For Child TWO
	CApaCommandLine* childProcTwoCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child Two created "));

	//child process TWO
	RProcess childProcTwo;
	ret = childProcTwo.Create(KChildTwoExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProcTwo);
	
	INFO_PRINTF2(_L("	Create Child Two returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcTwoCmdln->SetProcessEnvironmentL(childProcTwo));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine of Child Two to its Process "));

	INFO_PRINTF1(_L("	Run Child Two "));
	childProcTwo.Resume();
	//Time for the child one to launch itself
	User::After(500000);

	//child II process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcTwoId = childProcTwo.Id();
	INFO_PRINTF2(_L("	Child Two Id = 0x%lx "),childProcTwoId);

	CChildProcess* childProcess=NULL;
	TRAP(ret, childProcess=CChildProcess::NewL(parentProc,childProcOneId));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupStack::PushL(childProcess);

	exitType = parentProc.ExitType();
	TEST(exitType == EExitTerminate);
	TInt exitReason = parentProc.ExitReason();
	TEST(exitReason == 0);
	if(exitType==EExitTerminate && exitReason==0)
		{
		INFO_PRINTF1(_L("	Parent process is Terminated"));
		}
	
	exitType = childProcOne.ExitType();
	TEST(exitType == EExitTerminate);
	exitReason = childProcOne.ExitReason();
	TEST(exitReason == 0);
	if(exitType==EExitTerminate && exitReason==0)
		{
		INFO_PRINTF1(_L("	Child I is killed automatically ... "));
		}

	//Wait and see if child II terminates automatically...
	User::After(10000000);

	exitType = childProcTwo.ExitType();
	TEST(exitType == EExitPending);
	if(exitType==EExitPending)
		{
		INFO_PRINTF1(_L("	Child II running successfully"));
		childProcTwo.Terminate(KTProcTerminatingChildII);
		exitType = childProcTwo.ExitType();
		TEST(exitType==EExitTerminate);
		exitReason = childProcTwo.ExitReason();
		TEST(exitReason == KTProcTerminatingChildII);
		INFO_PRINTF1(_L("	So Terminated it manually ..."));
		}

	CleanupStack::PopAndDestroy(childProcess);
	CleanupStack::PopAndDestroy(&childProcTwo);
	CleanupStack::PopAndDestroy(childProcTwoCmdln);
	CleanupStack::PopAndDestroy(&childProcOne);
	CleanupStack::PopAndDestroy(childProcOneCmdln);
	CleanupStack::PopAndDestroy(&parentProc);
	CleanupStack::PopAndDestroy(parentProcCmdln);
	}

/**
 @SYMTestCaseID APPFWK-APPARC-0018

 @SYMPREQ PREQ1123

 @SYMTestCaseDesc The Test determines that a process is launched and terminated without any problem.\n

 @SYMTestPriority Low

 @SYMTestStatus Implemented

 @SYMTestActions. Creates and launches a process. No Child is created. Parent should launch properly, and on termination should die.
 API Calls:\n
 RProcess::Create(const TDesC &aFileName, const TDesC &aCommand, TOwnerType aType=EOwnerProcess);\n
 RProcess::Resume();
 RProcess::ExitType() const;
 RProcess::ExitReason() const;
 RProcess::Id() const;
 RProcess::Terminate(TInt aReason);
 
 @SYMTestExpectedResults Proper creation and termination.\n

 */
void CT_ProcStep::testParentWithoutAChildL(void)
	{
	TInt ret(0)	;

	//process
	RProcess parentProc;
	ret = parentProc.Create(KParentExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(parentProc);
	INFO_PRINTF2(_L("	Create Parent Process returned : %d"),ret);

	TUint64 parentProcId=parentProc.Id();
	INFO_PRINTF2(_L("	Process Id = 0x%lx "),parentProcId);

	INFO_PRINTF1(_L("	Run the Process "));
	parentProc.Resume();
	//Time for the parent process to launch itself
	User::After(5000000);

	TExitType exitType = parentProc.ExitType();
	TEST(exitType == EExitKill||exitType == EExitPending);
	if(exitType==EExitPending)
		{
		INFO_PRINTF1(_L("	Process running normally "));
		parentProc.Terminate(KTProcTerminatingParent);
		exitType = parentProc.ExitType();
		TEST(exitType==EExitTerminate);
		TInt exitReason = parentProc.ExitReason();
		TEST(exitReason == KTProcTerminatingParent);
		INFO_PRINTF1(_L("	Terminating the process "));
		}
	CleanupStack::PopAndDestroy(&parentProc);
	}

/**
 @SYMTestCaseID APPFWK-APPARC-0019

 @SYMPREQ PREQ1123

 @SYMTestCaseDesc The Test determines that more than one Child for a parent terminate on their Parent's termination.

 @SYMTestPriority Medium

 @SYMTestStatus Implemented

 @SYMTestActions. Creates and launches a process (parent). Creates 3 processes (child I,II,III) passing the first (parent) process ID.
 Launches all the 3 processes. Terminates parent and checks the existance of the child processes. All 3 children should die.
 API Calls:\n
 RProcess::Create(const TDesC &aFileName, const TDesC &aCommand, TOwnerType aType=EOwnerProcess);\n
 RProcess::Resume();
 RProcess::ExitType() const;
 RProcess::ExitReason() const;
 RProcess::Id() const;
 RProcess::Terminate(TInt aReason);
 CApaCommandLine::SetProcessEnvironmentL(RProcess &aProcess) const;
 CApaCommandLine::NewLC();
 CApaCommandLine::SetParentProcessId(TProcessId);
 
 @SYMTestExpectedResults Termination of all child processes automatically.\n

 */
void CT_ProcStep::testAllChildsSetToTerminateL(void)
	{
	TInt ret(0);

	//commandline for parent process
	CApaCommandLine* parentProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Parent Process created "));

	//parent process
	RProcess parentProc;
	ret = parentProc.Create(KParentExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(parentProc);
	INFO_PRINTF2(_L("	Create Parent Process returned : %d"),ret);

	//attach commandline to parent process
	TRAP(ret,parentProcCmdln->SetProcessEnvironmentL(parentProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	INFO_PRINTF1(_L("	Run Parent Process "));
	parentProc.Resume();
	//Time for the parent process to launch itself
	User::After(500000);

	//Get parent process ID here
	TUint64 parentProcId = parentProc.Id();
	INFO_PRINTF2(_L("	Parent Process Id = 0x%lx "),parentProcId);

	//Child I
	CApaCommandLine* childProcOneCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child One created "));

	//setting the parent process ID to child
	childProcOneCmdln->SetParentProcessId(parentProcId);
	INFO_PRINTF1(_L("	Set ParentProcessId to Child One "));

	//Child process I
	RProcess childProcOne;
	ret = childProcOne.Create(KChildOneExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProcOne);
	
	INFO_PRINTF2(_L("	Create Child One returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcOneCmdln->SetProcessEnvironmentL(childProcOne));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine of Child One to its Process "));

	INFO_PRINTF1(_L("	Run Child One "));
	childProcOne.Resume();
	//Time for the child process to launch itself
	User::After(1000000);

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcOneId = childProcOne.Id();
	INFO_PRINTF2(_L("	Child One Id = 0x%lx "),childProcOneId);

	//Child II
	CApaCommandLine* childProcTwoCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child Two created "));

	//setting the parent process ID to child
	childProcTwoCmdln->SetParentProcessId(parentProcId);
	INFO_PRINTF1(_L("	Set ParentProcessId to Child Two "));

	//child process II
	RProcess childProcTwo;
	ret = childProcTwo.Create(KChildTwoExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProcTwo);
	
	INFO_PRINTF2(_L("	Create Child Two returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcTwoCmdln->SetProcessEnvironmentL(childProcTwo));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine of Child Two to its Process "));

	INFO_PRINTF1(_L("	Run Child Two "));
	childProcTwo.Resume();
	//Time for the child process to launch itself
	User::After(1000000);

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcTwoId = childProcTwo.Id();
	INFO_PRINTF2(_L("	Child Two Id = 0x%lx "),childProcTwoId);

	//Child III
	CApaCommandLine* childProcThreeCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child Three created "));

	//setting the parent process ID to child
	childProcThreeCmdln->SetParentProcessId(parentProcId);
	INFO_PRINTF1(_L("	Set ParentProcessId to Child Three "));

	//child process III
	RProcess childProcThree;
	ret = childProcThree.Create(KChildThreeExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProcThree);
	
	INFO_PRINTF2(_L("	Create Child Three returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcThreeCmdln->SetProcessEnvironmentL(childProcThree));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine of Child Three to its Process "));

	childProcThree.Resume();

	//Time for the child process to launch itself
	User::After(1000000);

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcThreeId = childProcThree.Id();
	INFO_PRINTF2(_L("	Child Three Id = 0x%lx "),childProcThreeId);

	CChildProcess* childProcess=NULL;
	TRAP(ret, childProcess=CChildProcess::NewL(parentProc,childProcOneId));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupStack::PushL(childProcess);

	TExitType exitType = parentProc.ExitType();
	TEST(exitType == EExitTerminate);
	TInt exitReason = parentProc.ExitReason();
	TEST(exitReason == 0);
	if(exitType == EExitTerminate && exitReason == 0)
		{
		INFO_PRINTF1(_L("	Parent process is Terminated "));
		}

	exitType = childProcOne.ExitType();
	TEST(exitType == EExitTerminate);
	exitReason = childProcOne.ExitReason();
	TEST(exitReason == 0);
	if(exitType == EExitTerminate && exitReason == 0)
		{
		INFO_PRINTF1(_L("	Child I is killed "));
		}

	exitType = childProcTwo.ExitType();
	TEST(exitType == EExitTerminate);
	exitReason = childProcTwo.ExitReason();
	TEST(exitReason == 0);
	if(exitType == EExitTerminate && exitReason == 0)
		{
		INFO_PRINTF1(_L("	Child II is killed "));
		}

	//Wait 1sec to close the child process
	User::After(1000000);
	exitType = childProcThree.ExitType();
	TEST(exitType == EExitTerminate);
	exitReason = childProcThree.ExitReason();
	TEST(exitReason == 0);
	if(exitType == EExitTerminate && exitReason == 0)
		{
		INFO_PRINTF1(_L("	Child III is killed "));
		}

	CleanupStack::PopAndDestroy(childProcess);
	CleanupStack::PopAndDestroy(&childProcThree);
	CleanupStack::PopAndDestroy(childProcThreeCmdln);
	CleanupStack::PopAndDestroy(&childProcTwo);
	CleanupStack::PopAndDestroy(childProcTwoCmdln);
	CleanupStack::PopAndDestroy(&childProcOne);
	CleanupStack::PopAndDestroy(childProcOneCmdln);
	CleanupStack::PopAndDestroy(&parentProc);
	CleanupStack::PopAndDestroy(parentProcCmdln);
	}

/**
 @SYMTestCaseID APPFWK-APPARC-0020

 @SYMPREQ PREQ1123

 @SYMTestCaseDesc The Test determines that more than one Child for a Parent remain alive on their parent's termination.

 @SYMTestPriority Medium

 @SYMTestStatus Implemented

 @SYMTestActions. Creates and launches a process (parent). Creates 3 processes (child I,II,III) without passing the first (parent) process ID.
 Launches all the 3 processes. Terminates parent and checks the existance of the child processes. All 3 children should remain alive.
 API Calls:\n
 RProcess::Create(const TDesC &aFileName, const TDesC &aCommand, TOwnerType aType=EOwnerProcess);\n
 RProcess::Resume();
 RProcess::ExitType() const;
 RProcess::ExitReason() const;
 RProcess::Id() const;
 RProcess::Terminate(TInt aReason);
 CApaCommandLine::SetProcessEnvironmentL(RProcess &aProcess) const;
 CApaCommandLine::NewLC();
 
 @SYMTestExpectedResults Existence of all child processes.\n

 */
void CT_ProcStep::testNoChildSetToTerminateL(void)
	{
	TInt ret(0);

	//commandline for parent process
	CApaCommandLine* parentProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Parent Process created "));

	//parent process
	RProcess parentProc;
	ret = parentProc.Create(KParentExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(parentProc);
	INFO_PRINTF2(_L("	Create Parent Process returned : %d"),ret);

	//attach commandline to parent process
	TRAP(ret,parentProcCmdln->SetProcessEnvironmentL(parentProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	INFO_PRINTF1(_L("	Run Parent Process "));
	parentProc.Resume();
	//Time for the parent process to launch itself
	User::After(500000);

	//commandline for child process
	//Get parent process ID here
	TUint64 parentProcId = parentProc.Id();
	INFO_PRINTF2(_L("	Parent Process Id = 0x%lx "),parentProcId);

	//Child I
	CApaCommandLine* childProcOneCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child One created "));

	//child process
	RProcess childProcOne;
	ret = childProcOne.Create(KChildOneExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProcOne);
	
	INFO_PRINTF2(_L("	Create Child One returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcOneCmdln->SetProcessEnvironmentL(childProcOne));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine of Child One to its Process "));

	childProcOne.Resume();

	//Time for the child process to launch itself
	User::After(1000000);

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcOneId = childProcOne.Id();
	INFO_PRINTF2(_L("	Child Process Id = 0x%lx "),childProcOneId);

	//Child II
	CApaCommandLine* childProcTwoCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child Two created "));

	//child process
	RProcess childProcTwo;
	ret = childProcTwo.Create(KChildTwoExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProcTwo);
	
	INFO_PRINTF2(_L("	Create Child Two returned : %d "),ret);

	//attach commandline to child process
	TRAP(ret,childProcTwoCmdln->SetProcessEnvironmentL(childProcTwo));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine of Child Two to its Process "));

	INFO_PRINTF1(_L("	Run Child Two "));
	childProcTwo.Resume();
	//Time for the child process to launch itself
	User::After(1000000);

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcTwoId = childProcTwo.Id();
	INFO_PRINTF2(_L("	Child Two Id = 0x%lx "),childProcTwoId);

	//Child III
	CApaCommandLine* childProcThreeCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child Three created "));

	//child process
	RProcess childProcThree;
	ret = childProcThree.Create(KChildThreeExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProcThree);
	
	INFO_PRINTF2(_L("	Create Child Three returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcThreeCmdln->SetProcessEnvironmentL(childProcThree));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine of Child Three to its Process "));

	childProcThree.Resume();

	//Time for the child process to launch itself
	User::After(1000000);

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcThreeId = childProcThree.Id();
	INFO_PRINTF2(_L("	Child Three Id = 0x%lx "),childProcThreeId);

	parentProc.Terminate(KTProcTerminatingParent);
	TExitType exitType = parentProc.ExitType();
	TEST(exitType == EExitTerminate);
	TInt exitReason = parentProc.ExitReason();
	TEST(exitReason == KTProcTerminatingParent);
	INFO_PRINTF1(_L("	Terminating parent process "));

	//Time for the child processes to terminate if they really do
	User::After(500000);

	exitType = childProcOne.ExitType();
	TEST(exitType == EExitPending);
	if(exitType == EExitPending)
		{
		INFO_PRINTF1(_L("	Child I is still running "));
		INFO_PRINTF1(_L("	So Terminating it manually ... "));
		childProcOne.Terminate(KTProcTerminatingChildI);
		exitType = childProcOne.ExitType();
		TEST(exitType==EExitTerminate);
		exitReason = childProcOne.ExitReason();
		TEST(exitReason == KTProcTerminatingChildI);
		}

	exitType = childProcTwo.ExitType();
	TEST(exitType == EExitPending);
	if(exitType == EExitPending)
		{
		INFO_PRINTF1(_L("	Child II is still running "));
		INFO_PRINTF1(_L("	So Terminating it manually ... "));
		childProcTwo.Terminate(KTProcTerminatingChildII);
		exitType = childProcTwo.ExitType();
		TEST(exitType==EExitTerminate);
		exitReason = childProcTwo.ExitReason();
		TEST(exitReason == KTProcTerminatingChildII);
		}

	exitType = childProcThree.ExitType();
	TEST(exitType == EExitPending);
	if(exitType == EExitPending)
		{
		INFO_PRINTF1(_L("	Child III is still running "));
		INFO_PRINTF1(_L("	So Terminating it manually ... "));
		childProcThree.Terminate(KTProcTerminatingChildIII);
		exitType = childProcThree.ExitType();
		TEST(exitType==EExitTerminate);
		exitReason = childProcThree.ExitReason();
		TEST(exitReason == KTProcTerminatingChildIII);
		}

	CleanupStack::PopAndDestroy(&childProcThree);
	CleanupStack::PopAndDestroy(childProcThreeCmdln);
	CleanupStack::PopAndDestroy(&childProcTwo);
	CleanupStack::PopAndDestroy(childProcTwoCmdln);
	CleanupStack::PopAndDestroy(&childProcOne);
	CleanupStack::PopAndDestroy(childProcOneCmdln);
	CleanupStack::PopAndDestroy(&parentProc);
	CleanupStack::PopAndDestroy(parentProcCmdln);
	}

/**
 @SYMTestCaseID APPFWK-APPARC-0021

 @SYMPREQ PREQ1123

 @SYMTestCaseDesc The Test determines that the child receives its parent process ID correctly, when set during its creation.

 @SYMTestPriority Medium

 @SYMTestStatus Implemented

 @SYMTestActions. Creates and launches a process (parent). Creates another process (child) setting the first(parent) process ID.
 Parent Process Id is passed to child through SetParameter API. Launches the child process. Child obtains the parent process Id from within.
 Child compares both the Id's and writes the results to a file. The Id's should match each other.
 API Calls:\n
 RProcess::Create(const TDesC &aFileName, const TDesC &aCommand, TOwnerType aType=EOwnerProcess);\n
 RProcess::Resume();
 RProcess::ExitType() const;
 RProcess::ExitReason() const;
 RProcess::Id() const;
 RProcess::SetParameter(TInt aSlot, TInt aData);
 CApaCommandLine::SetProcessEnvironmentL(RProcess &aProcess) const;
 CApaCommandLine::NewLC();
 CApaCommandLine::SetParentProcessId(TProcessId);
 RFile::Open(RFs &aFs, const TDesC &aName, TUint aFileMode);
 RFile::Read(TInt aPos, TDes8 &aDes) const;
 RFile::Close();
 RFs::Connect(TInt aMessageSlots=KFileServerDefaultMessageSlots);
 RFs::Delete(const TDesC &aName);
 RFs::Close();
 @SYMTestExpectedResults Id received by child process should match its parent process Id.\n

 */
void CT_ProcStep::testIdAvailableToChildL(void)
	{
	TInt ret(0);

	//commandline for parent process
	CApaCommandLine* parentProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Parent process created "));

	//parent process
	RProcess parentProc;
	ret = parentProc.Create(KParentExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(parentProc);
	INFO_PRINTF2(_L("	Create Parent Process returned : %d"),ret);

	//attach commandline to parent process
	TRAP(ret,parentProcCmdln->SetProcessEnvironmentL(parentProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	INFO_PRINTF1(_L("	Run Parent Process "));
	parentProc.Resume();
	//Time for the parent process to launch itself
	User::After(500000);

	//commandline for child process
	//Get parent process ID here
	TUint64 parentProcId = parentProc.Id();
	INFO_PRINTF2(_L("	Parent Process Id = 0x%lx "),parentProcId);

	CApaCommandLine* childProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child process created "));

	//setting the parent process ID to child
	childProcCmdln->SetParentProcessId(parentProcId);
	INFO_PRINTF1(_L("	Set ParentProcessId to Child "));
	
	//child process
	RProcess childProc;
	ret = childProc.Create(KChildThreeExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProc);
	
	INFO_PRINTF2(_L("	Create Child Process returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcCmdln->SetProcessEnvironmentL(childProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

//	Setting the parent process Id in an environment slot for the child to receive.
	ret = childProc.SetParameter(12,parentProcId);
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("	Set the Parent Process Id - 0x%lx to Child through SetParameter API in Slot 12 "),parentProcId);

	INFO_PRINTF1(_L("	Run Child Process "));
	childProc.Resume();
	//Time for the child process to launch itself
	User::After(1000000);

	RFs fs;
	RFile file;
	ret=fs.Connect();
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Create File server session "));

	ret = file.Open(fs,KFilePath,EFileWrite | EFileShareAny);
	TEST(ret == KErrNone);
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("	File opened successfully "));
		TBuf8<5> readData;
		file.Read(0,readData);
		TBuf8<5> result(KTResultPass); 
		TEST(result==readData);
		if(result==readData)
			{
			INFO_PRINTF1(_L("	Child received the parent process ID correctly ..."));
			}
		else
			{
			INFO_PRINTF1(_L("	Child Failed to receive the parent process ID ..."));
			}
		file.Close();
		fs.Delete(KFilePath);
		fs.Close();
		INFO_PRINTF1(_L("	File Close & Delete and Session Close "));
		}

	//child process Id is reqd to monitor if it gets killed on its parent's termination
	TUint64 childProcId = childProc.Id();
	INFO_PRINTF2(_L("	Child Process Id = 0x%lx "),childProcId);

	CChildProcess* childProcess=NULL;
	TRAP(ret, childProcess=CChildProcess::NewL(parentProc,childProcId));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupStack::PushL(childProcess);
	
	TExitType exitType = parentProc.ExitType();
	TEST(exitType == EExitTerminate);
	TInt exitReason = parentProc.ExitReason();
	TEST(exitReason == 0);
	if(exitType==EExitTerminate && exitReason == 0)
		{
		INFO_PRINTF1(_L("	Parent process is Terminated"));
		}
		
	exitType = childProc.ExitType();
	TEST(exitType == EExitTerminate);
	exitReason = childProc.ExitReason();
	TEST(exitReason == 0);
	if(exitType==EExitTerminate && exitReason == 0)
		{
		INFO_PRINTF1(_L("	The child process is killed "));
		}
	CleanupStack::PopAndDestroy(childProcess);
	CleanupStack::PopAndDestroy(&childProc);
	CleanupStack::PopAndDestroy(childProcCmdln);
	CleanupStack::PopAndDestroy(&parentProc);
	CleanupStack::PopAndDestroy(parentProcCmdln);
	}

/**
 @SYMTestCaseID APPFWK-APPARC-0022

 @SYMPREQ PREQ1123

 @SYMTestCaseDesc The Test determines that the child doesn't receive its parent process ID correctly, when not set during its creation.

 @SYMTestPriority Medium

 @SYMTestStatus Implemented

 @SYMTestActions. Creates and launches a process (parent). Creates another process (child) without setting any parent.
 Parent Process Id is passed to child through SetParameter API. Launches the child process. Child obtains the parent process Id from within.
 Child compares both the Id's. The Id's should not match each other.
 API Calls:\n
 RProcess::Create(const TDesC &aFileName, const TDesC &aCommand, TOwnerType aType=EOwnerProcess);\n
 RProcess::Resume();
 RProcess::ExitType() const;
 RProcess::ExitReason() const;
 RProcess::Id() const;
 RProcess::SetParameter(TInt aSlot, TInt aData);
 CApaCommandLine::SetProcessEnvironmentL(RProcess &aProcess) const;
 CApaCommandLine::NewLC();
 RFile::Open(RFs &aFs, const TDesC &aName, TUint aFileMode);
 RFile::Read(TInt aPos, TDes8 &aDes) const;
 RFile::Close();
 RFs::Connect(TInt aMessageSlots=KFileServerDefaultMessageSlots);
 RFs::Delete(const TDesC &aName);
 RFs::Close();
 @SYMTestExpectedResults Id received by child process should not match its parent process Id.\n

 */
void CT_ProcStep::testIdNotAvailableToChildL(void)
	{
	TInt ret(0);
	TInt exitReason(0);
	
	//commandline for parent process
	CApaCommandLine* parentProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Parent Process created "));

	//parent process
	RProcess parentProc;
	ret = parentProc.Create(KParentExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(parentProc);
	INFO_PRINTF2(_L("	Create Parent Process returned : %d"),ret);

	//attach commandline to parent process
	TRAP(ret,parentProcCmdln->SetProcessEnvironmentL(parentProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	INFO_PRINTF1(_L("	Run Parent Process "));
	parentProc.Resume();
	//Time for the parent process to launch itself
	User::After(500000);

	//commandline for child process
	//Get parent process ID here
	TUint64 parentProcId = parentProc.Id();
	INFO_PRINTF2(_L("	Parent Process Id = 0x%lx "),parentProcId);

	CApaCommandLine* childProcCmdln=CApaCommandLine::NewLC();
	INFO_PRINTF1(_L("	CommandLine for Child Process created "));
	
	//child process
	RProcess childProc;
	ret = childProc.Create(KChildThreeExe,KNullDesC);
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	CleanupClosePushL(childProc);
	
	INFO_PRINTF2(_L("	Create Child Process returned : %d"),ret);

	//attach commandline to child process
	TRAP(ret,childProcCmdln->SetProcessEnvironmentL(childProc));
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Attach CommandLine to Process "));

	//Setting the parent process Id in an environment slot for the child to receive.
	ret = childProc.SetParameter(12,parentProcId);
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("	Set the Parent Process Id - 0x%lx to Child through SetParameter API in Slot 12 "),parentProcId);

	INFO_PRINTF1(_L("	Run Child Process "));
	childProc.Resume();
	//Time for the child process to launch itself
	User::After(1000000);

	RFs fs;
	RFile file;
	ret = fs.Connect();
	TEST(ret == KErrNone);
	User::LeaveIfError(ret);
	INFO_PRINTF1(_L("	Create File server session "));

	ret = file.Open(fs,KFilePath,EFileWrite | EFileShareAny);
	TEST(ret == KErrNone);
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("	File opened successfully "));
		TBuf8<5> readData;
		file.Read(0,readData);
		TBuf8<5> result(KTResultFail); 
		TEST(result==readData);
		if(result==readData)
			{
			INFO_PRINTF1(_L("	Child did not receive the parent process ID as intended..."));
			}
		else
			{
			INFO_PRINTF1(_L("	Child received the Wrong parent process ID ..."));
			}
		file.Close();
		fs.Delete(KFilePath);
		fs.Close();
		INFO_PRINTF1(_L("	File Close & Delete and Session Close "));
		}

	TExitType exitType = parentProc.ExitType();
	TEST(exitType == EExitPending);
	if(exitType == EExitPending)
		{
		INFO_PRINTF1(_L("	Parent is still running "));
		INFO_PRINTF1(_L("	So Terminating it manually ... "));
		parentProc.Terminate(KTProcTerminatingParent);
		exitType = parentProc.ExitType();
		TEST(exitType==EExitTerminate);
		exitReason = parentProc.ExitReason();
		TEST(exitReason == KTProcTerminatingParent);
		}

	exitType = childProc.ExitType();
	TEST(exitType == EExitPending);
	if(exitType == EExitPending)
		{
		INFO_PRINTF1(_L("	Child is still running "));
		INFO_PRINTF1(_L("	So Terminating it manually ... "));
		childProc.Terminate(KTProcTerminatingChildIII);
		exitType = childProc.ExitType();
		TEST(exitType==EExitTerminate);
		exitReason = childProc.ExitReason();
		TEST(exitReason == KTProcTerminatingChildIII);
		}

	CleanupStack::PopAndDestroy(&childProc);
	CleanupStack::PopAndDestroy(childProcCmdln);
	CleanupStack::PopAndDestroy(&parentProc);
	CleanupStack::PopAndDestroy(parentProcCmdln);
	}

void CT_ProcStep::testChildExistsL(void)
/**
 * Calls other functions which implement the test cases.
 */
	{
	INFO_PRINTF1(_L("Begin - testChildSetToTerminateL ----------- "));
	TRAPD(ret,testChildSetToTerminateL());
	TEST(ret==KErrNone);
	INFO_PRINTF1(_L("End - testChildSetToTerminateL ----------- \n"));

	INFO_PRINTF1(_L("Begin - testChildSetToRemainL ----------- "));
	TRAP(ret,testChildSetToRemainL());
	TEST(ret==KErrNone);
	INFO_PRINTF1(_L("End - testChildSetToRemainL ----------- \n"));

	INFO_PRINTF1(_L("Begin - testTwoChildsOneToTerminateAndOtherToRemainL ----------- "));
	TRAP(ret,testTwoChildsOneToTerminateAndOtherToRemainL());
	TEST(ret==KErrNone);
	INFO_PRINTF1(_L("End - testTwoChildsOneToTerminateAndOtherToRemainL ----------- \n"));

	INFO_PRINTF1(_L("Begin - testParentWithoutAChildL ----------- "));
	TRAP(ret,testParentWithoutAChildL());
	TEST(ret==KErrNone);
	INFO_PRINTF1(_L("End - testParentWithoutAChildL ----------- \n"));

	INFO_PRINTF1(_L("Begin - testAllChildsSetToTerminateL ----------- "));
	TRAP(ret,testAllChildsSetToTerminateL());
	TEST(ret==KErrNone);
	INFO_PRINTF1(_L("End - testAllChildsSetToTerminateL ----------- \n"));

	INFO_PRINTF1(_L("Begin - testNoChildSetToTerminateL ----------- "));
	TRAP(ret,testNoChildSetToTerminateL());
	TEST(ret==KErrNone);
	INFO_PRINTF1(_L("End - testNoChildSetToTerminateL ----------- \n"));

	INFO_PRINTF1(_L("Begin - testIdAvailableToChildL ----------- "));
	TRAP(ret,testIdAvailableToChildL());
	TEST(ret==KErrNone);
	INFO_PRINTF1(_L("End - testIdAvailableToChildL ----------- \n"));

	INFO_PRINTF1(_L("Begin - testIdNotAvailableToChildL ----------- "));
	TRAP(ret,testIdNotAvailableToChildL());
	TEST(ret==KErrNone);
	INFO_PRINTF1(_L("End - testIdNotAvailableToChildL ----------- \n"));
	}

TVerdict CT_ProcStep::doTestStepL()
/**
   @return - TVerdict code
   Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test child process existence when parent terminates/exists begins here ------- \n"));

	__UHEAP_MARK;

	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	TRAPD(ret,testChildExistsL());
	TEST(ret==KErrNone);

	CleanupStack::PopAndDestroy(scheduler);

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("Test child process existence when parent terminates/exists ends here ------- \n"));

	return TestStepResult();
	}
