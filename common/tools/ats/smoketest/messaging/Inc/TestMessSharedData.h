// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestMessSharedData
// 
//

#if (!defined __TEST_MESS_SHARED_DATA_H__)
#define __TEST_MESS_SHARED_DATA_H__

#include <msvapi.h>
#include <mtclreg.h>
#include <ImapConnectionObserver.h>
#include <TestExecuteStepBase.h>

class  CTestMessSharedData : public CBase, private MMsvSessionObserver
{
public:
	static CTestMessSharedData*	NewL();
	~CTestMessSharedData();

	void	IMAP4ConnectAndSyncCompleteAfterDisconnectL(TUid aMtmId, TMsvId aMsvId, CTestStep& aStep);
	TInt	DeleteEntryL(CTestStep& aStep);

	void	DoProcessL();

protected:
	CTestMessSharedData();

private:
	void	ConstructL();

	//	MMsvSessionObserver implementation
	virtual void HandleSessionEventL(TMsvSessionEvent, TAny*, TAny*, TAny*) {}

private:
	class CMsgEntry : public CActive
	{
	public:
		~CMsgEntry();

		virtual void	DoCancel();
		virtual void	RunL();
		virtual void	DoProcessL(CClientMtmRegistry& aMtmRegistry) = 0;
		virtual void	DoCancelProcessL() = 0;

	protected:
		CMsgEntry(CMsgEntry* aList);

		void	ConstructL(CTestStep& aStep);

	public:
		TName		iName;
		CMsgEntry*	iNext;
		TBool		iReadyToBeDeleted;
	};
	class CMsgIMAP4ConnectAndSyncComplete : public CMsgEntry, public MMsvImapConnectionObserver
	{
	public:
		static CMsgIMAP4ConnectAndSyncComplete* NewL(TUid aMtmId, TMsvId aMsvId, CTestStep& aStep, CMsgEntry* aList);
		~CMsgIMAP4ConnectAndSyncComplete();

		virtual void	RunL();
		virtual void	DoProcessL(CClientMtmRegistry& aMtmRegistry);
		virtual void	DoCancelProcessL();

	protected:
		CMsgIMAP4ConnectAndSyncComplete(TUid aMtmId, TMsvId aMsvId, CMsgEntry* aList);

	private:		
		//	MMsvImapConnectionObserver implementation
		void	HandleImapConnectionEvent(TImapConnectionEvent aConnectionEvent);

	private:
		TUid			iMtmId;
		TMsvId			iMsvId;
		CBaseMtm*		iBaseMtm;
		CMsvOperation*	iOperation;
		
	};

public:
	CMsvSession*		iSession;
	CClientMtmRegistry*	iMtmRegistry;
	CMsgEntry*			iList;

private:
	CMsgEntry*			iAnyPostStepToDo;
	TBool				iDeleteTestStep;
};

#endif /* __TEST_MESS_SHARED_DATA_H__ */
