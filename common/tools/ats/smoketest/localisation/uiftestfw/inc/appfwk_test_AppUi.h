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



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __APPFWK_TEST_APPUI_H__
#define __APPFWK_TEST_APPUI_H__

#include <e32std.h>
#include <e32base.h>
#include <coeaui.h>
#include <eikappui.h>

#include <test/testexecutestepbase.h>


// user panic descriptors
_LIT(KPanicNullPointer,"Null pointer");

//user panic errors
const TInt KNullPointer = 1;


/********************************************************************/
/*																	*/
/*																	*/
/*		MAutoTestApp												*/
/*																	*/
/*																	*/
/********************************************************************/

class MAutoTestApp
	{
public:
	virtual void RunTestStepL(TInt aNextStep)=0; // this function needs to be implemented by the derived class of either CTestCoeAppUi or CTestAppUi
	};


/********************************************************************/
/*																	*/
/*																	*/
/*		CAutoTestManager											*/
/*																	*/
/*																	*/
/********************************************************************/

/**
  The test code manager, provides functions to set active object  with lowest priority for running test in auto mode.
*/
class CAutoTestManager : public CActive
	{
public:
	enum TTestResult
		{
		EPass,
		EFailed,
		EUnknown,
		};
public:
	IMPORT_C ~CAutoTestManager();
	IMPORT_C static CAutoTestManager* NewL(MAutoTestApp* aAutoTestApp);
	IMPORT_C void StartAutoTest(); //init active object and set it active
	IMPORT_C void FinishAllTestCases(TTestResult aTestResult);	//every test program should call this function in order to 
						 										//stop the consequence of test steps by 
																//closing active sheduler
	IMPORT_C void SetTestCaseNumber(TInt aNumber);
	inline TTestResult	GetTestResult() const;
private: 
	IMPORT_C CAutoTestManager(MAutoTestApp* aAutoTestApp);
	void RunL();
	void DoCancel();
private:
	MAutoTestApp* iAutoTestApp;
	TInt iNumStep;
	TTestResult	iResult;
	};

inline CAutoTestManager::TTestResult	CAutoTestManager::GetTestResult() const
	{
	return iResult;
	}


/********************************************************************/
/*																	*/
/*																	*/
/*		CTestCoeAppUi												*/
/*																	*/
/*																	*/
/********************************************************************/

/**
  The abstract class is base for test programs, incapsulates the CAutoTestManager member.
  The class doesn't handles work with resources.

  It is supposed that a derived class in its ConstructL(); function first of all has to calls the ConstructL() function of the parent class.
*/

class CTestCoeAppUi : public CCoeAppUi, public MAutoTestApp
	{
public:
	IMPORT_C CTestCoeAppUi(CTestStep*		aStep);
	IMPORT_C ~CTestCoeAppUi();
	IMPORT_C void ConstructL(); // should be called from ConstructL of the child class
	CTestExecuteLogger&	Logger() {return iStep->Logger();}
	inline void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine) 
		{
		iStep -> testBooleanTrue(aCondition, aFile, aLine);
		}
	void testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine);
	void testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine );
	void testBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine );
	void TestCheckPointCompareL(TInt aVal, TInt aExpectedVal, const TDesC& aText, const TText8* aFile, TInt aLine);

protected:
	inline CAutoTestManager& AutoTestManager() const;
	
private:
	CAutoTestManager* iAutoTestManager;
	CTestStep		*iStep;
	};

CAutoTestManager& CTestCoeAppUi::AutoTestManager() const
	{
	return *iAutoTestManager;
	}


class CEikButtonGroupContainer;
class CEikMenuBar;

/********************************************************************/
/*																	*/
/*																	*/
/*		CTestAppUi													*/
/*																	*/
/*																	*/
/********************************************************************/

/**
  The abstract class is base for test programs, incapsulates the CAutoTestManager member.
  The class handles work with resources.

  It is supposed that a derived class in its ConstructL(); function first of all has to calls the ConstructL() function of the parent class.
*/
class CTestAppUi : public CEikAppUi, public MAutoTestApp
	{
public:
	IMPORT_C CTestAppUi(CTestStep*	aStep, const TDesC& aRes, TInt aResourceHotKeysId = 0, TInt aResourceMenuBarId = 0, TInt aResourceToolBarId = 0);
	IMPORT_C ~CTestAppUi();
	IMPORT_C void ConstructL(); // should be called from ConstructL of the child class
	CTestExecuteLogger&	Logger() {return iStep->Logger();}
	inline void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine) 
		{
		iStep -> testBooleanTrue(aCondition, aFile, aLine, ETrue);
		}

protected:
	inline CAutoTestManager& AutoTestManager() const;
	inline CTestStep& TestStep() const;
	IMPORT_C void ReduceRect(TRect& aRect) const;	//reduce client rectangle if toolbar or/and menu exist hide menu from screen
	IMPORT_C void ProcessCommandL(TInt aCommand);

private:
	CAutoTestManager* iAutoTestManager;

	TInt iResourceFileFlag;
	CEikButtonGroupContainer *iToolBar;
	CEikMenuBar	*iMenuBar;
	
	TInt iResourceHotKeysId;
	TInt iResourceMenuBarId;
	TInt iResourceToolBarId;
	TFileName iRes;
	CTestStep *iStep;
	};

CAutoTestManager& CTestAppUi::AutoTestManager() const
	{
	return *iAutoTestManager;
	}

CTestStep& CTestAppUi::TestStep() const
	{
	return *iStep;
	}

//to prevent from alloc memory error in the User::__DbgMarkEnd 
//we need to call this function if we are using  CCoeEnv and memory heap macros
IMPORT_C void PreallocateHALBuffer();

//to prevent test fail due to panic dialog on screen 
//we need to call this function if we the test we are running is sensitive to dialogs being on screen.
IMPORT_C void CloseAllPanicWindowsL();

#endif	// __APPFWK_TEST_APPUI_H__
