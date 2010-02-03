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
// This contains CTestContact
// 
//

#ifndef TEST_CONTACT_H
#define TEST_CONTACT_H

#include <cntitem.h>

class MTestContactInstrumentation
{
public:
	virtual void			RaiseInstrumentationEventNotificationL(TInt aEventTag) = 0;
};

class CTestContact : public CBase
	{
public:
	static CTestContact*	NewL(CContactItem& aSeedContact);
	static CTestContact*	NewLC(CContactItem& aSeedContact);
	static CTestContact*	NewL(CContactItem* aSeedContact);
	static CTestContact*	NewLC(CContactItem* aSeedContact);
	~CTestContact();

	CContactItem&	ContactItem();
	const TPtrC		FirstNameL();
	const TPtrC		LastNameL();
	const TPtrC		CompanyNameL();
	const TPtrC		JobTitleL();
	const TPtrC		WorkPhoneL();
	const TPtrC		WorkMobileL();
	const TPtrC		HomePhoneL();
	const TPtrC		HomeMobileL();
	const TPtrC		FaxL();
	const TPtrC		EmailAddressL();
	const TPtrC		InternetAddressL();
	const TPtrC		StreetL();
	const TPtrC		TownL();
	const TPtrC		CountyL();
	const TPtrC		PostCodeL();
	const TPtrC		CountryL();
	const TPtrC		NotesL();
	const TPtrC		NickNameL();
	const TPtrC		PictureL();

	void	SetFirstNameL(const TDesC& aFirstName);
	void	SetLastNameL(const TDesC& aLastName);
	void	SetCompanyNameL(const TDesC& aCompanyName);
	void	SetJobTitleL(const TDesC& aJobTitle);
	void	SetWorkPhoneL(const TDesC& aWorkPhone);
	void	SetWorkMobileL(const TDesC& aWorkMobile);
	void	SetHomePhoneL(const TDesC& aHomePhone);
	void	SetHomeMobileL(const TDesC& aHomeMobile);
	void	SetFaxL(const TDesC& aFax);
	void	SetHomeEmailAddressL(const TDesC& aEmailAddress);
	void	SetWorkEmailAddressL(const TDesC& aEmailAddress);
	void	SetInternetAddressL(const TDesC& aInternetAddress);
	void	SetStreetL(const TDesC& aStreet);
	void	SetTownL(const TDesC& aTown);
	void	SetCountyL(const TDesC& aCounty);
	void	SetPostCodeL(const TDesC& aPostCode);
	void	SetCountryL(const TDesC& aCountry);
	void	SetNotesL(const TDesC& aNotes);
	void	SetNickNameL(const TDesC& aNickName);
	void 	SetPictureL(const TDesC8& aPicture);

private:
	CTestContact();
	void				ConstructL(CContactItem& aSeedContact);
	void				ConstructL(CContactItem* aSeedContact);
	const TPtrC			TextFieldL(TFieldType aFieldType) const;
	const TPtrC			TextFieldL(const CContentType& aTypesToMatch) const;
  	void 				SetTextFieldL(const CContentType& aTypesToMatch,const TDesC& aText) const;
  	void 				SetTextFieldL(TFieldType aFieldType, const TDesC& aText);
  	void 				SetTextFieldL(TFieldType aFieldType, const TDesC& aText, TUid aAddOnContentType);
	CContactItemField&	FindFieldL(TFieldType aFieldType) const;
	CContactItemField&	FindFieldL(const CContentType& aTypesToMatch) const;
	void SetPictureFieldL(const TDesC8& aPicture) const;
	TInt FindField(TFieldType aFieldType) const;
  	void InsertTextFieldL(TUid aID, const TDesC& aStringOfStuff);

private:
	CContactItem*	iContactItem;
	TBool			iDestroyOnExit;
	};

#endif /* __TEST_CONTACT_H__ */
