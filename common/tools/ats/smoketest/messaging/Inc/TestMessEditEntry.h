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
// This contains CTestMessEditEntry which edits the entry characterstics
// 
//


#if (!defined __TEST_MESS_EDIT_ENTRY_H__)
#define __TEST_MESS_EDIT_ENTRY_H__

#include "TestMessProcessExistingMsg.h"

/**
 This class edits the entry characteristics
*/
class CTestMessEditEntry : public CTestMessProcessExistingMsg
{
public:
	CTestMessEditEntry() { SetTestStepName(_L("EditEntry")); }

	virtual	void ProcessMessageL(CBaseMtm& aBaseMtm, TMsvId aMsgId);

	//Move the message to different folder
	void MoveMessageL(TMsvId aMessageId,TMsvId aFolderId);

	/**
	 This class is used for asynchronous operation for move
	 message operation
	*/
	class CMoveOperation: public CActive
		{
	public:
		//Pure virtual implementation of CActive
		void RunL();

		//Pure virtual implementation of CActive
		void DoCancel();
		
		//Set the operation as active
		void SetOperation();

		//Two phase construction
		static CMoveOperation* NewL();
	private:
		//Second phase construction
		void ConstructL();

		//Constructor
		CMoveOperation();
		};
};

#endif /* __TEST_MESS_EDIT_ENTRY_H__ */
