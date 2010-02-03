/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* This contains CTestMessStoreProcessUtil. Base class process what is in the message store
* 
*
*/





/**
 @file TestMessStoreProcessUtil.h
*/

#if (!defined __TEST_MESS_STORE_PROCESS_UTIL_H__)
#define __TEST_MESS_STORE_PROCESS_UTIL_H__

#include "TestMessBase.h"

class CTestMessStoreProcessUtil : public CBase
{
public:
	CTestMessStoreProcessUtil(CTestMessBase& aTestStep, TBool aReadOnly);

	//
	TInt	ProcessEntryL(TMsvId aMsvId);

protected:
	// Process the CImHeader information
	virtual void	ProcessImHeaderL(CImHeader& aHeader) = 0;

	// Process the CImMimeHeader information
	virtual void	ProcessImMimeHeaderL(CImMimeHeader& aHeader) = 0;

	// Process the CImIAPPreferences information
	virtual void	ProcessInternetAccessPreferencesL(CImIAPPreferences& aPreferences) = 0;

	// Process the CSmsHeader information
	virtual void	ProcessSmsHeaderStreamL(CSmsHeader& aHeader) = 0;


	// Process the CRichText information
	virtual void	ProcessRichTextBodyL(CRichText& aBodyText) = 0;

	// Convert Text to character set information
	TBool			ValidCharacterSetInfo(const TPtrC& aCharacterSetType, TUint &aCharSetVal);

private:
	// Process the CImHeader information
	void	ProcessImHeaderL(CMsvStore& aStore);

	// Process the CImMimeHeader information
	void	ProcessImMimeHeaderL(CMsvStore& aStore);


	// Process the CSmsHeader information
	void	ProcessSmsHeaderStreamL(CMsvStore& aStore);


	// Process the CRichText information
	void	ProcessRichTextBodyL(CMsvStore& aStore);

protected:
	CTestMessBase&	iTestStep;

private:
	TBool			iReadOnly;
};

#endif /* __TEST_MESS_STORE_PROCESS_UTIL_H__ */
