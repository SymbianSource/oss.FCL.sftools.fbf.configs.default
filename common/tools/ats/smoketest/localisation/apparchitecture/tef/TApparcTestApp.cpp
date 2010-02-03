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
// @file 
// @internalComponent - Internal Symbian test code
// This is a support app used by Apparc\tef\TApparcTestServer (TApparc4Step in particular)
// This application covers the following tests for the static objects
// Test 1 : Existing CCoeStatic derived objects using the 2 parameter
// constructor must be destroyed before the app ui.
// Way to test this,
// Case 1: Get hold of a particular static using CCoeEnv::Static(TUid aUid) 
// in an application's override of CEikAppUi::PrepareToExit() and confirm the 
// returned pointer is not null. 
// Case 2: Then, the same thing is done in the app ui's destructor to check that 
// the pointer is now null.
// Test 2: A static constructed with a negative priority should be accessible from the 
// app ui's destructor, but then destroyed before another static that has a lower priority value.
// Way to test this,
// Case 1: Construct a static (A) using the 3 parameter constructor, specifying a negative priority.
// Then construct another static (B) specifying a priority (A's priority - 1). 
// Check that static A is still non-null when accessed from the app ui's destructor. 
// Case 2: Then check that static A is null when accessed from B's destructor.
// Test 3: Given two statics with different positive priorities,
// the higher priority one should be destroyed first.
// Way to test this,
// Case 1: If (for example) static A has priority EDefaultDestructionPriority,
// and static B has EDefaultDestructionPriority-1, A's destructor should be able to access B,
// Case 2: But B's destructor shouldn't be able to access A.
// Test 4: Statics with priorities immediately either side of the pre/post-destroy app ui should be destroyed before/after the app ui. Should also test that 
// the first negative priority static is destroyed and not "skipped over" when the code calculates
// which static should be destroyed next after the app ui has been destroyed.
// Way to test this,
// Case 1: We could create statics A, B and C with priority zero, -1 and -2 respectively. 
// Then check in the app ui destructor that A is null, and B and C are both non-null.
// Case 2: Then in B's destructor check that C is still non-null.
// Case 3: And in C's destructor check that B is now null.
// 
//



#include <coeccntx.h>
#include <apgtask.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <f32file.h>
#include <eikon.hrh>
#include <eikstart.h> ////TKAS added for exe-app

#include <TApparcTestApp.rsg>

// File which stores the test results
_LIT(KApparcTestResultsFileName, "c:\\system\\Apparctest\\ApparcTestResultsFile.txt");
_LIT(KApparcTestDir, "c:\\system\\Apparctest\\");

_LIT8(KTestPass ,"PASS\r\n");
_LIT8(KTestFail,"FAIL\r\n");

_LIT8(KApparcTest1,"Test1:");
_LIT8(KApparcTest2,"Test2:");
_LIT8(KApparcTest3,"Test3:");
_LIT8(KApparcTest4,"Test4:");
_LIT8(KApparcTestCase1,"Case1:");
_LIT8(KApparcTestCase2,"Case2:");
_LIT8(KApparcTestCase3,"Case3:");

const TUid KUidApparcTestApp = {0x100048F3};

// Uids for diffent classes for static 
const TUid KUidTestStatic		 = 	{0x13003ACE}; 
const TUid KUidTestStaticNegativePrioA = {0x13004ACE}; 
const TUid KUidTestStaticNegativePrioB = {0x13005ACE}; 
const TUid KUidTestStaticPosPriA =	{0x13006ACE};
const TUid KUidTestStaticPosPriB = 	{0x13007ACE};
const TUid KUidTestStaticPriZeroA=	{0x13009ACE};
const TUid KUidTestStaticPriOneB =	{0x1300AACE};
const TUid KUidTestStaticPriTwoC =	{0x1300BACE};

// Priorities for static objects
enum {ENegativePriortyStaticA = -3};
enum {EPriorityTwoStaticC= -2,EPriorityOneStaticB = -1,EPriorityZeroStaticA= 0};

// static class for Test case 1  
class CTApparcTestStatic : public CCoeStatic
	{
public:
	inline CTApparcTestStatic(RFile& aFile);
	inline static CTApparcTestStatic* Self();
	~CTApparcTestStatic();
	RFile iFile;
	};

CTApparcTestStatic::~CTApparcTestStatic()
	{
	}

// Static class with negative priority for Test case 2.
class CTApparcTestNegativePrioStaticA : public CCoeStatic
	{
public:
	inline CTApparcTestNegativePrioStaticA(RFile& aFile);
	inline static CTApparcTestNegativePrioStaticA* Self();
	~CTApparcTestNegativePrioStaticA();
	
	RFile& iFile;
	};

// Static class with negative priority for Test case 2. (priority less than CTApparcTestNegativePrioStaticA)
class CTApparcTestNegativePrioStaticB : public CCoeStatic
	{
public:
	inline CTApparcTestNegativePrioStaticB();
	inline static CTApparcTestNegativePrioStaticB* Self();
	~CTApparcTestNegativePrioStaticB();
	
	RFile iFile;
	RFs iFs;
	};

CTApparcTestNegativePrioStaticA::~CTApparcTestNegativePrioStaticA()
	{
	}

CTApparcTestNegativePrioStaticB::~CTApparcTestNegativePrioStaticB()
	{
	// Test 2, Case 2
	CTApparcTestNegativePrioStaticA* testCoeStaticA = CTApparcTestNegativePrioStaticA::Self();
	iFile.Write(KApparcTest2);
	iFile.Write(KApparcTestCase2);
	
	if(testCoeStaticA == NULL)
		iFile.Write(KTestPass);
	else
		iFile.Write(KTestFail);
		
	// CTApparcTestNegativePrioStaticB class object is the most negative prioritised
	// static objects .Hence the file and session handles are closed here as no
	// more logging is required hereafter. 	
	iFile.Close();
	iFs.Close();	
	}

// Static class with positive priority for Test case 3:
class CTApparcTestPosPrioStaticA : public CCoeStatic
	{
public:
	inline CTApparcTestPosPrioStaticA(RFile& aFile);
	inline static CTApparcTestPosPrioStaticA* Self();
	~CTApparcTestPosPrioStaticA();
	
	RFile& iFile;
	};

// static class with positive priority less than CTApparcTestPosPrioStaticA for Test case 3.
class CTApparcTestPosPrioStaticB : public CCoeStatic
	{
public:
	inline CTApparcTestPosPrioStaticB(RFile& aFile);
	inline static CTApparcTestPosPrioStaticB* Self();
	~CTApparcTestPosPrioStaticB();
	
	RFile& iFile;
	};

CTApparcTestPosPrioStaticA::~CTApparcTestPosPrioStaticA()
	{
	// Test 3 , Case 1
	CTApparcTestPosPrioStaticB* testPosPrioStaticB = CTApparcTestPosPrioStaticB::Self();
	iFile.Write(KApparcTest3);
	iFile.Write(KApparcTestCase1);
	
	if(testPosPrioStaticB != NULL)
		iFile.Write(KTestPass);
	else
		iFile.Write(KTestFail);
	}
	
CTApparcTestPosPrioStaticB::~CTApparcTestPosPrioStaticB()
	{
	// Test 3 , Case 2
	CTApparcTestPosPrioStaticA* testPosPrioStaticA = CTApparcTestPosPrioStaticA::Self();
	iFile.Write(KApparcTest3);
	iFile.Write(KApparcTestCase2);
	
	if(testPosPrioStaticA == NULL)
		iFile.Write(KTestPass);
	else
		iFile.Write(KTestFail);
	}

// static class with  priority 0 for test case 4
class CTApparcTestPrioZeroStaticA : public CCoeStatic
	{
public:
	inline CTApparcTestPrioZeroStaticA(RFile& aFile);
	inline static CTApparcTestPrioZeroStaticA* Self();
	~CTApparcTestPrioZeroStaticA();
	
	RFile& iFile;
	};

// static class with priority -1 for test case 4
class CTApparcTestPrioOneStaticB : public CCoeStatic
	{
public:
	inline CTApparcTestPrioOneStaticB(RFile& aFile);
	inline static CTApparcTestPrioOneStaticB* Self();
	~CTApparcTestPrioOneStaticB();
	
	RFile& iFile;
	};

// static class with priority -2 for test case 4	
class CTApparcTestPrioTwoStaticC : public CCoeStatic
	{
public:
	inline CTApparcTestPrioTwoStaticC(RFile& aFile);
	inline static CTApparcTestPrioTwoStaticC* Self();
	~CTApparcTestPrioTwoStaticC();
	
	RFile& iFile;
	};


CTApparcTestPrioZeroStaticA::~CTApparcTestPrioZeroStaticA()
	{
	}

CTApparcTestPrioOneStaticB::~CTApparcTestPrioOneStaticB()
	{
	// Test 4 , Case 2
	CTApparcTestPrioTwoStaticC* testPrioTwoCoeStaticC = CTApparcTestPrioTwoStaticC::Self();
	iFile.Write(KApparcTest4);
	iFile.Write(KApparcTestCase2);

	if(testPrioTwoCoeStaticC != NULL)
		iFile.Write(KTestPass);
	else
		iFile.Write(KTestFail);
	}

CTApparcTestPrioTwoStaticC::~CTApparcTestPrioTwoStaticC()
	{
	// Test 4 , Case 3
	CTApparcTestPrioOneStaticB* testPrioOneCoeStaticB = CTApparcTestPrioOneStaticB::Self();
	iFile.Write(KApparcTest4);
	iFile.Write(KApparcTestCase3);

	if(testPrioOneCoeStaticB == NULL)
		iFile.Write(KTestPass);
	else
		iFile.Write(KTestFail);
	}

////////////////////////////////////////////////////////////////////////
//
// CTApparcTestAppView
//
////////////////////////////////////////////////////////////////////////
class CTApparcTestAppView : public CCoeControl
    {
public:
	static CTApparcTestAppView* NewL(const TRect& aRect);
	CTApparcTestAppView();
	~CTApparcTestAppView();
    void ConstructL(const TRect& aRect);
private:
    // Inherited from CCoeControl
	void Draw(const TRect& /*aRect*/) const;
private:
	HBufC*  iApparcTestText;
    };

CTApparcTestAppView::CTApparcTestAppView()
	{
	}

CTApparcTestAppView* CTApparcTestAppView::NewL(const TRect& aRect)
	{
	CTApparcTestAppView* self = new(ELeave) CTApparcTestAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

CTApparcTestAppView::~CTApparcTestAppView()
	{
	delete iApparcTestText;
	}

void CTApparcTestAppView::ConstructL(const TRect& aRect)
    {
	iApparcTestText = iEikonEnv->AllocReadResourceL(R_TAPPARCTEST_TEXT_TITLE);
	CreateWindowL();
	SetRect(aRect);
	ActivateL();
	}

void CTApparcTestAppView::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();
	TRect      drawRect = Rect();
	const CFont*  fontUsed = iEikonEnv->TitleFont();
	
	gc.Clear();
	gc.UseFont(fontUsed);
	TInt   baselineOffset=(drawRect.Height() - fontUsed->HeightInPixels())/2; 
	gc.DrawText(*iApparcTestText,drawRect,baselineOffset,CGraphicsContext::ECenter, 0);
	gc.DiscardFont();
	}

////////////////////////////////////////////////////////////////////////
//
// CTApparcTestAppUi
//
////////////////////////////////////////////////////////////////////////

class CTApparcTestAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CTApparcTestAppUi();
private:
    // Inherirted from class CEikAppUi
	void PrepareToExit();
	void HandleCommandL(TInt aCommand);

private:
	CCoeControl*	iAppView;
	};
	
	
void CTApparcTestAppUi::ConstructL()
    {
    BaseConstructL();
	iAppView = CTApparcTestAppView::NewL(ClientRect());
	
	// Constructs the static object for tests
	CTApparcTestNegativePrioStaticB* testCoeStaticB = new(ELeave)CTApparcTestNegativePrioStaticB();
	
	CTApparcTestStatic* testCoeStatic = new (ELeave)CTApparcTestStatic(testCoeStaticB->iFile);
	CTApparcTestNegativePrioStaticA* testCoeStaticA  = new(ELeave)CTApparcTestNegativePrioStaticA(testCoeStaticB->iFile);
	CTApparcTestPrioTwoStaticC* testPrioTwoStaticC = new(ELeave) CTApparcTestPrioTwoStaticC(testCoeStaticB->iFile);
    CTApparcTestPosPrioStaticA* testPosPriStaticA = new(ELeave) CTApparcTestPosPrioStaticA(testCoeStaticB->iFile);
	CTApparcTestPosPrioStaticB* testPosPriStaticB = new(ELeave) CTApparcTestPosPrioStaticB(testCoeStaticB->iFile);
	CTApparcTestPrioZeroStaticA* testPrioZeroStaticA = new(ELeave) CTApparcTestPrioZeroStaticA(testCoeStaticB->iFile);
	CTApparcTestPrioOneStaticB* testPrioOneStaticB = new(ELeave) CTApparcTestPrioOneStaticB(testCoeStaticB->iFile);
	
	// to get rid of compiler warnings
	if(testCoeStatic==NULL  || testCoeStaticA == NULL ||  testCoeStaticB == NULL|| testPosPriStaticA ==NULL ||
		testPosPriStaticB == NULL || testPrioZeroStaticA == NULL || testPrioOneStaticB == NULL|| testPrioTwoStaticC == NULL)
		 return;	
	}

CTApparcTestAppUi::~CTApparcTestAppUi()
	{
	CTApparcTestNegativePrioStaticB* testNegStaticB = CTApparcTestNegativePrioStaticB::Self();
	// Test 1, Case 2
	CTApparcTestStatic* testCoeStatic = CTApparcTestStatic::Self();
	testNegStaticB->iFile.Write(KApparcTest1);
	testNegStaticB->iFile.Write(KApparcTestCase2);
	
	if(testCoeStatic == NULL)
		testNegStaticB->iFile.Write(KTestPass);
	else
		testNegStaticB->iFile.Write(KTestFail);
		
	// Test 2, Case 1	 
	CTApparcTestNegativePrioStaticA* testCoeStaticA = CTApparcTestNegativePrioStaticA::Self();
	testNegStaticB->iFile.Write(KApparcTest2);
	testNegStaticB->iFile.Write(KApparcTestCase1);

	if(testCoeStaticA != NULL)
		testNegStaticB->iFile.Write(KTestPass);
	else
		testNegStaticB->iFile.Write(KTestFail);

	// Test 4, Case 1
	CTApparcTestPrioZeroStaticA* testPrioZeroCoeStaticA = CTApparcTestPrioZeroStaticA::Self();
	CTApparcTestPrioOneStaticB* testPrioOneCoeStaticB = CTApparcTestPrioOneStaticB::Self();
	CTApparcTestPrioTwoStaticC* testPrioTwoCoeStaticC = CTApparcTestPrioTwoStaticC::Self();
	testNegStaticB->iFile.Write(KApparcTest4);
	testNegStaticB->iFile.Write(KApparcTestCase1);

	if(testPrioZeroCoeStaticA == NULL && testPrioOneCoeStaticB != NULL && testPrioTwoCoeStaticC != NULL)
		testNegStaticB->iFile.Write(KTestPass);
	else
		testNegStaticB->iFile.Write(KTestFail);

	delete iAppView;
	}

void CTApparcTestAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
 	case EEikCmdExit: 
		Exit();
		break;
		}
	}

void CTApparcTestAppUi::PrepareToExit()
	{
	CEikAppUi::PrepareToExit();
	// Test 1, Case 1
	CTApparcTestStatic* testCoeStatic = CTApparcTestStatic::Self();
	
	testCoeStatic->iFile.Write(KApparcTest1);
	testCoeStatic->iFile.Write(KApparcTestCase1);

	if(testCoeStatic != NULL)
		testCoeStatic->iFile.Write(KTestPass);
	else
		testCoeStatic->iFile.Write(KTestFail);
	}

////////////////////////////////////////////////////////////////////////
//
// CTApparcTestDocument
//
////////////////////////////////////////////////////////////////////////

class CTApparcTestDocument : public CEikDocument
	{
public:
	static CTApparcTestDocument* NewL(CEikApplication& aApp);
	CTApparcTestDocument(CEikApplication& aApp);
	void ConstructL();
private: 
	// Inherited from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

CTApparcTestDocument::CTApparcTestDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}

CEikAppUi* CTApparcTestDocument::CreateAppUiL()
	{
    return new(ELeave) CTApparcTestAppUi;
	}

////////////////////////////////////////////////////////////////////////
//
// CTApparcTestApplication
//
////////////////////////////////////////////////////////////////////////

class CTApparcTestApplication : public CEikApplication
	{
private: 
	// Inherited from class CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
private:
	CApaDocument* CreateDocumentL(CApaProcess* a) { return CEikApplication::CreateDocumentL(a); }
	};

TUid CTApparcTestApplication::AppDllUid() const
	{
	return KUidApparcTestApp;
	}

CApaDocument* CTApparcTestApplication::CreateDocumentL()
	{
	return new (ELeave) CTApparcTestDocument(*this);
	}

EXPORT_C CApaApplication* NewApplication()
	{
	return new CTApparcTestApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

		

//
// inline functions 
//

// inline function for CTApparcTestStatic
inline CTApparcTestStatic::CTApparcTestStatic(RFile& aFile)
	: CCoeStatic(KUidTestStatic),iFile(aFile)
	{
	}

inline CTApparcTestStatic* CTApparcTestStatic::Self()
	{
	return STATIC_CAST(CTApparcTestStatic*,CCoeEnv::Static(KUidTestStatic));
	}
	
// inline functions for CTApparcTestNegativePrioStaticA
inline CTApparcTestNegativePrioStaticA::CTApparcTestNegativePrioStaticA(RFile& aFile)
	: CCoeStatic(KUidTestStaticNegativePrioA,ENegativePriortyStaticA),iFile(aFile)
	{
	}

inline CTApparcTestNegativePrioStaticA* CTApparcTestNegativePrioStaticA::Self()
	{
	return STATIC_CAST(CTApparcTestNegativePrioStaticA*,CCoeEnv::Static(KUidTestStaticNegativePrioA));
	}
	
// inline functions for CTApparcTestNegativePrioStaticB	
inline CTApparcTestNegativePrioStaticB::CTApparcTestNegativePrioStaticB()
	: CCoeStatic(KUidTestStaticNegativePrioB,ENegativePriortyStaticA - 1)
	{
	User::LeaveIfError(iFs.Connect());
	TInt err = iFs.MkDirAll(KApparcTestDir);
	
	err = iFile.Create(iFs,KApparcTestResultsFileName,EFileWrite | EFileShareAny);
	if(err == KErrAlreadyExists)
		iFile.Open(iFs,KApparcTestResultsFileName,EFileWrite | EFileShareAny);
	}

inline CTApparcTestNegativePrioStaticB* CTApparcTestNegativePrioStaticB::Self()
	{
	return STATIC_CAST(CTApparcTestNegativePrioStaticB*,CCoeEnv::Static(KUidTestStaticNegativePrioB));
	}

// inline functions for CTApparcTestPosPrioStaticA	
inline CTApparcTestPosPrioStaticA::CTApparcTestPosPrioStaticA(RFile& aFile)
	: CCoeStatic(KUidTestStaticPosPriA,EDefaultDestructionPriority),iFile(aFile)
	{
	}

inline CTApparcTestPosPrioStaticA* CTApparcTestPosPrioStaticA::Self()
	{
	return STATIC_CAST(CTApparcTestPosPrioStaticA*,CCoeEnv::Static(KUidTestStaticPosPriA));
	}

// inline functions for CTApparcTestPosPrioStaticB		
inline CTApparcTestPosPrioStaticB::CTApparcTestPosPrioStaticB(RFile& aFile)
	: CCoeStatic(KUidTestStaticPosPriB,EDefaultDestructionPriority-1),iFile(aFile)
	{
	}

inline CTApparcTestPosPrioStaticB* CTApparcTestPosPrioStaticB::Self()
	{
	return STATIC_CAST(CTApparcTestPosPrioStaticB*,CCoeEnv::Static(KUidTestStaticPosPriB));
	}
	
// inline functions for CTApparcTestPrioZeroStaticA			
inline CTApparcTestPrioZeroStaticA::CTApparcTestPrioZeroStaticA(RFile& aFile)
	: CCoeStatic(KUidTestStaticPriZeroA,EPriorityZeroStaticA),iFile(aFile)
	{
	}

inline CTApparcTestPrioZeroStaticA* CTApparcTestPrioZeroStaticA::Self()
	{
	return STATIC_CAST(CTApparcTestPrioZeroStaticA*,CCoeEnv::Static(KUidTestStaticPriZeroA));
	}
	
// inline functions for CTApparcTestPrioOneStaticB					
inline CTApparcTestPrioOneStaticB::CTApparcTestPrioOneStaticB(RFile& aFile)
	: CCoeStatic(KUidTestStaticPriOneB,EPriorityOneStaticB),iFile(aFile)
	{	
	}

inline CTApparcTestPrioOneStaticB* CTApparcTestPrioOneStaticB::Self()
	{
	return STATIC_CAST(CTApparcTestPrioOneStaticB*,CCoeEnv::Static(KUidTestStaticPriOneB));
	}

// inline functions for CTApparcTestPrioTwoStaticC						
inline CTApparcTestPrioTwoStaticC::CTApparcTestPrioTwoStaticC(RFile& aFile)
	: CCoeStatic(KUidTestStaticPriTwoC,EPriorityTwoStaticC),iFile(aFile)
	{
	}

inline CTApparcTestPrioTwoStaticC* CTApparcTestPrioTwoStaticC::Self()
{
	return STATIC_CAST(CTApparcTestPrioTwoStaticC*,CCoeEnv::Static(KUidTestStaticPriTwoC));
}
