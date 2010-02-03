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
// This contains CTestAgendaServerLaunchProgress
// 
//

#ifndef TEST_AGENDA_SERVER_LAUNCH_PROGRESS_H
#define TEST_AGENDA_SERVER_LAUNCH_PROGRESS_H

#include "TestAgendabase.h"
#include <e32base.h>
#include <calprogresscallback.h>


//forward declaration
//class CTestAgendaBase;
class CCalEntryView;


class CTestAgendaServerLaunchProgress : public CActive, private MCalProgressCallBack
/**
* This class provides an implementationfor the pure virtual functions
* declared in MAgnProgressCallBack. The framework  makes calls to the
* functions indidcating the status of the operation.
*/
{
public:
	static CTestAgendaServerLaunchProgress* NewL(CTestAgendaBase* aAgendaBase);
	virtual ~CTestAgendaServerLaunchProgress();
	void	Start();

	//from CActive
	virtual void	RunL();
	virtual void	DoCancel();

private:
	CTestAgendaServerLaunchProgress(CTestAgendaBase* aAgendaBase);
	void	ConstructL();

	//from MAgnProgressCallBack
	virtual void	Completed(TInt aError = KErrNone);
	virtual void	Progress(TInt aPercentageCompleted);
	virtual TBool NotifyProgress();

private:
	/**
	pointer to the CTestAgendaBase object.
	*/
	CTestAgendaBase*			iAgendaBase;
    CCalEntryView*              iCalEntryView;

	/**
	file in which the entries are present
	*/
	TPtrC						iFileName;

	/**
	file server session
	*/
	RFs							iFs;

};

#endif TEST_AGENDA_SERVER_LAUNCH_PROGRESS_H
