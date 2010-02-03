// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SM_STD_H__
#define __SM_STD_H__

// System includes
#include <e32base.h>
#include <t32wld.h>
#include <asclisession.h>
#include <asshdalarm.h>

// User includes
#include "timewmod.h"

class CModelResponder : public CActive
	{
public:
	void Start();
	virtual void RunL();
	virtual void DoStart()=0;
	virtual void DoCancel()=0;
protected:
	CModelResponder(CModelBase &aModel,TInt aPriority);
private:
	void NotifyModel(TInt aNotification);
private:
	CModelBase *iModel;
	};

class CAlmModel;
class CAlmResponder : public CModelResponder
	{
public:
	CAlmResponder(RASCliSession& aSvr,CAlmModel& aModel,TInt aResponderPriority);
public:
	virtual void DoStart();
	virtual void DoCancel();
private:
	TAlarmId iAlarmId;
	RASCliSession* iAlmSvr;
	};

class CWldModel;
class CWldResponder : public CModelResponder
	{
public:
	CWldResponder(RWorldServer& aSvr,CWldModel& aModel,TInt aResponderPriority);
public:
	virtual void DoStart();
	virtual void DoCancel();
protected:
	RWorldServer* iWldSvr;
	};

#endif
