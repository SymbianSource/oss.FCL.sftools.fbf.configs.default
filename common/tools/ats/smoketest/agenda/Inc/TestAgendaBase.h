// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestAgendaBase which is the base class for all
// the Psd Agx suite test steps
// 
//

#ifndef TEST_AGENDA_BASE_H
#define TEST_AGENDA_BASE_H

#include <TestExecuteStepBase.h>
#include <calentryview.h>
#include <gdi.h>



//forward declarations
class CCalSession;

#define KMaxDateStringLength	30
#define KMaxTimeStringLength	30

#define KMaxDateStringLength	30
#define KMaxTimeStringLength	30

class CTestAgendaPicture :public MPictureFactory
/**
*  Picture class for the agenda model
*/
{
/**************************************************************
* An object of this class is used for setting the picture factory
* of an Agenda model, although our code does not use any of the
* functionality provided by this class, it is created with empty
* implementation and set to satisfy the compiler as it uses
* an assert macro to check if this object is set before saving
* the agenda model entries into a file
*************************************************************/

public:
	CTestAgendaPicture();
	virtual ~CTestAgendaPicture();
	virtual void NewPictureL(TPictureHeader& aHeader,const CStreamStore& aDeferredPictureStore)const;
};


class CTestAgendaBase : public CTestStep
{
public:
	CTestAgendaBase();
	~CTestAgendaBase();

	virtual enum TVerdict	doTestStepPreambleL();
	virtual enum TVerdict	doTestStepPostambleL();

	CCalSession& CTestAgendaBase::CalSession();
	void SetCalEntryView(CCalEntryView* calEntryViewPtr);

protected:
	void	CreateDatabaseL();
	void	OpenDatabaseL();
	void	CleanupDatabase();

private:
	CActiveScheduler*	iSchedular;
	TBool				iEnteredBlocked;
	TBool				iExitedBlocked;

protected:
    RFs					iFs;
	TBool				iSessionConnected;
	CCalEntryView*      iCalEntryViewBase;
	/**
	To store the models entries
	*/
	CFileStore*			iStore;
	CCalSession*        iCalSession;
};

#endif /* TEST_AGENDA_BASE_H */
