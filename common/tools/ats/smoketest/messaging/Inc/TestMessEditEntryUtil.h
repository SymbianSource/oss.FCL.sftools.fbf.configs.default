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
// This contains CTestMessEditEntryUtil. A utility to edit an entry
// 
//

#if (!defined __TEST_MESS_EDIT_ENTRY_UTIL_H__)
#define __TEST_MESS_EDIT_ENTRY_UTIL_H__

#include "TestMessStoreProcessUtil.h"

/**
 This class is a utility to edit an entry
*/
class CTestMessEditEntryUtil : public CTestMessStoreProcessUtil
{
public:
	CTestMessEditEntryUtil(CTestMessBase& aTestStep, TBool aSimChecked, TBool aSimInDatabase, RMobilePhone::TMobileAddress aSimPhoneNumber);

protected:
	// Process the CImHeader information
	virtual void	ProcessImHeaderL(CImHeader& aHeader);

	// Process the CImMimeHeader information
	virtual void	ProcessImMimeHeaderL(CImMimeHeader& aHeader);

	// Process the CImIAPPreferences information
	virtual void	ProcessInternetAccessPreferencesL(CImIAPPreferences& aPreferences);

	// Process the CSmsHeader information
	virtual void	ProcessSmsHeaderStreamL(CSmsHeader& aHeader);


	// Process the CRichText information
	virtual void	ProcessRichTextBodyL(CRichText& aBodyText);

private:
	TBool							iSimChecked;
	TBool							iSimInDatabase;
	RMobilePhone::TMobileAddress	iSimPhoneNumber;
};

#endif /* __TEST_MESS_EDIT_ENTRY_UTIL_H__ */
