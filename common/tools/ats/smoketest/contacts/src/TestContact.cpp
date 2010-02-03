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

#include <cntfldst.h>

#include "TestContact.h"

CTestContact* CTestContact::NewL(CContactItem& aSeedContact)
	{
	CTestContact* self=CTestContact::NewLC(aSeedContact);
	CleanupStack::Pop();
	return self;
	}

CTestContact* CTestContact::NewLC(CContactItem& aSeedContact)
	{
	CTestContact* self=new(ELeave) CTestContact();
	CleanupStack::PushL(self);
	self->ConstructL(aSeedContact);
	return self;
	}

CTestContact* CTestContact::NewL(CContactItem* aSeedContact)
	{
	CTestContact* self=CTestContact::NewLC(aSeedContact);
	CleanupStack::Pop();
	return self;
	}

CTestContact* CTestContact::NewLC(CContactItem* aSeedContact)
	{
	CTestContact* self=new(ELeave) CTestContact();
	CleanupStack::PushL(self);
	self->ConstructL(aSeedContact);
	return self;
	}

CTestContact::CTestContact() : iDestroyOnExit(EFalse)
	{
	}

void CTestContact::ConstructL(CContactItem& aSeedContact)
	{
	iDestroyOnExit=ETrue;
	iContactItem=CContactCard::NewL(&aSeedContact);
	}

void CTestContact::ConstructL(CContactItem* aSeedContact)
	{
	iDestroyOnExit=EFalse;
	iContactItem=aSeedContact;
	}

CTestContact::~CTestContact()
	{
	if (iDestroyOnExit)
		delete iContactItem;
	iContactItem=NULL;
	}

CContactItem& CTestContact::ContactItem()
	{
	return *iContactItem;
	}

const TPtrC CTestContact::FirstNameL()
	{
	return TextFieldL(KUidContactFieldGivenName);
	}

const TPtrC CTestContact::LastNameL()
	{
	return TextFieldL(KUidContactFieldFamilyName);
	}

const TPtrC CTestContact::CompanyNameL()
	{
	return TextFieldL(KUidContactFieldCompanyName);
	}

const TPtrC CTestContact::JobTitleL()
	{
	return TextFieldL(KUidContactFieldJobTitle);
	}

const TPtrC CTestContact::WorkPhoneL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}

const TPtrC CTestContact::WorkMobileL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}

const TPtrC CTestContact::HomePhoneL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}

const TPtrC CTestContact::HomeMobileL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}

const TPtrC CTestContact::FaxL()
	{
	return TextFieldL(KUidContactFieldFax);
	}

const TPtrC CTestContact::EmailAddressL()
	{
	return TextFieldL(KUidContactFieldEMail);
	}

const TPtrC CTestContact::InternetAddressL()
	{
	return TextFieldL(KUidContactFieldUrl);
	}

const TPtrC CTestContact::StreetL()
	{
	return TextFieldL(KUidContactFieldAddress);
	}

const TPtrC CTestContact::TownL()
	{
	return TextFieldL(KUidContactFieldLocality);
	}

const TPtrC CTestContact::CountyL()
	{
	return TextFieldL(KUidContactFieldRegion);
	}

const TPtrC CTestContact::PostCodeL()
	{
	return TextFieldL(KUidContactFieldPostcode);
	}

const TPtrC CTestContact::CountryL()
	{
	return TextFieldL(KUidContactFieldCountry);
	}

const TPtrC CTestContact::NotesL()
	{
	return TextFieldL(KUidContactFieldNote);
	}

const TPtrC CTestContact::NickNameL()
	{
	return TextFieldL(KUidContactFieldSecondName);
	}

const TPtrC CTestContact::PictureL()
	{
	return TextFieldL(KUidContactFieldPicture);
	}

void CTestContact::SetFirstNameL(const TDesC& aFirstName)
	{
 	SetTextFieldL(KUidContactFieldGivenName, aFirstName, KUidContactFieldVCardMapUnusedN);
	}

void CTestContact::SetLastNameL(const TDesC& aLastName)
	{
 	SetTextFieldL(KUidContactFieldFamilyName, aLastName, KUidContactFieldVCardMapUnusedN);
	}

void CTestContact::SetCompanyNameL(const TDesC& aCompanyName)
	{
	SetTextFieldL(KUidContactFieldCompanyName, aCompanyName, KUidContactFieldVCardMapORG);
	}

void CTestContact::SetJobTitleL(const TDesC& aJobTitle)
	{
	SetTextFieldL(KUidContactFieldJobTitle,aJobTitle);
	}

void CTestContact::SetWorkPhoneL(const TDesC& aWorkPhone)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	SetTextFieldL(*contentType,aWorkPhone);
	CleanupStack::PopAndDestroy(); // contentType.
	}

void CTestContact::SetWorkMobileL(const TDesC& aWorkMobile)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	SetTextFieldL(*contentType,aWorkMobile);
	CleanupStack::PopAndDestroy(); // contentType.
	}

void CTestContact::SetHomePhoneL(const TDesC& aHomePhone)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	SetTextFieldL(*contentType,aHomePhone);
	CleanupStack::PopAndDestroy(); // contentType.
	}

void CTestContact::SetHomeMobileL(const TDesC& aHomeMobile)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	SetTextFieldL(*contentType,aHomeMobile);
	CleanupStack::PopAndDestroy(); // contentType.
	}

void CTestContact::SetFaxL(const TDesC& aFax)
	{
	SetTextFieldL(KUidContactFieldFax,aFax);
	}

void CTestContact::SetHomeEmailAddressL(const TDesC& aEmailAddress)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldEMail);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	SetTextFieldL(*contentType,aEmailAddress);
	CleanupStack::PopAndDestroy(contentType);
	}

void CTestContact::SetWorkEmailAddressL(const TDesC& aEmailAddress)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldEMail);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	SetTextFieldL(*contentType,aEmailAddress);
	CleanupStack::PopAndDestroy(contentType);
	}


void CTestContact::SetInternetAddressL(const TDesC& aInternetAddress)
	{
	SetTextFieldL(KUidContactFieldUrl,aInternetAddress);
	}

void CTestContact::SetStreetL(const TDesC& aStreet)
	{
	SetTextFieldL(KUidContactFieldAddress,aStreet);
	}

void CTestContact::SetTownL(const TDesC& aTown)
	{
	SetTextFieldL(KUidContactFieldLocality,aTown);
	}

void CTestContact::SetCountyL(const TDesC& aCounty)
	{
	SetTextFieldL(KUidContactFieldRegion,aCounty);
	}

void CTestContact::SetPostCodeL(const TDesC& aPostCode)
	{
	SetTextFieldL(KUidContactFieldPostcode,aPostCode);
	}

void CTestContact::SetCountryL(const TDesC& aCountry)
	{
	SetTextFieldL(KUidContactFieldCountry,aCountry);
	}

void CTestContact::SetNotesL(const TDesC& aNotes)
	{
	SetTextFieldL(KUidContactFieldNote,aNotes);
	}

void CTestContact::SetNickNameL(const TDesC& aNickName)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldSecondName);
	TRAPD(err, SetTextFieldL(*contentType,aNickName));
        if (!err) { }
	CleanupStack::PopAndDestroy(); // contentType.
	}

void CTestContact::SetPictureL(const TDesC8& aPicture)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPicture);
	SetPictureFieldL(aPicture);
	CleanupStack::PopAndDestroy(contentType);
	}

const TPtrC CTestContact::TextFieldL(TFieldType aFieldType) const
	{
	CContactItemField& field=FindFieldL(aFieldType);
	ASSERT(field.StorageType()==KStorageTypeText);
	return STATIC_CAST(CContactTextField*,(field.Storage()))->Text();
	}

const TPtrC CTestContact::TextFieldL(const CContentType& aTypesToMatch) const
	{
	CContactItemField& field=FindFieldL(aTypesToMatch);
	ASSERT(field.StorageType()==KStorageTypeText);
	return STATIC_CAST(CContactTextField*,field.Storage())->Text();
	}

void CTestContact::InsertTextFieldL(TFieldType aID, const TDesC& aStringOfStuff )
	{
 	CContactItemField* itemFieldPtr = NULL;
 	CContactTextField* textStoragePtr = NULL;

 	itemFieldPtr = CContactItemField::NewLC( KStorageTypeText, aID );
 	textStoragePtr = itemFieldPtr->TextStorage(); // we don't have ownership here, hence no stack.

 	textStoragePtr->SetTextL( aStringOfStuff );
 	textStoragePtr = NULL; // we've finished with this ptr now.

 	iContactItem->AddFieldL( *itemFieldPtr ); // hand ownership over here
 	CleanupStack::Pop( itemFieldPtr ); // and pop,
	}

void CTestContact::SetTextFieldL(TFieldType aFieldType,const TDesC& aText)
	{
 	TInt	fieldIndex=FindField(aFieldType);

 	if ( fieldIndex > KErrNotFound )
		{
 		CContactItemField&	field=FindFieldL(aFieldType);
 		ASSERT(field.StorageType()==KStorageTypeText);
 		STATIC_CAST(CContactTextField*,field.Storage())->SetText(aText.AllocL());
		}
 	else
		{
 		InsertTextFieldL(aFieldType, aText);
		}
	}

void CTestContact::SetTextFieldL(const CContentType& aTypesToMatch,const TDesC& aText) const
	{
	CContactItemField& field=FindFieldL(aTypesToMatch);
	ASSERT(field.StorageType()==KStorageTypeText);
	STATIC_CAST(CContactTextField*,field.Storage())->SetText(aText.AllocL());
	}

CContactItemField& CTestContact::FindFieldL(TFieldType aFieldType) const
	{
	CContactItemFieldSet& fields=iContactItem->CardFields();
	TInt index=fields.Find(aFieldType);
	User::LeaveIfError(index);
	return fields[index];
	}

void CTestContact::SetTextFieldL(TFieldType aFieldType, const TDesC& aText, TUid aAddOnContentType)
	{
	CContactItemFieldSet& fields = iContactItem->CardFields();
	CContactTextField* textField = NULL;
	TInt fieldIndex = 0;

	fieldIndex = FindField( aFieldType );

	if (fieldIndex == KErrNotFound )
		{
		InsertTextFieldL( aFieldType, aText );
		fieldIndex = FindField( aFieldType );
		CContactItemField& field = fields[ fieldIndex ];
		field.AddFieldTypeL( aAddOnContentType );
		}
		else if (fieldIndex >= KErrNone )
		{
		CContactItemField& field = fields[ fieldIndex ];
		ASSERT(field.StorageType()==KStorageTypeText);
		textField = static_cast<CContactTextField*>(field.Storage());
		textField->SetText( aText.AllocL() );
		field.AddFieldTypeL( aAddOnContentType );
		}
	}

TInt CTestContact::FindField(TFieldType aFieldType) const
	{
	return iContactItem->CardFields().Find( aFieldType );
	}

CContactItemField& CTestContact::FindFieldL(const CContentType& aTypesToMatch) const
	{
	CContactItemFieldSet& fields=iContactItem->CardFields();
	const TInt numTypesToMatch(aTypesToMatch.FieldTypeCount());
	const TFieldType mainFieldType(aTypesToMatch.FieldType(0));
	TInt index=-1;
	FOREVER
		{
		index=fields.FindNext(mainFieldType,index+1);
		User::LeaveIfError(index);

		CContactItemField& thisPhoneField=fields[index];
		const CContentType& thisPhoneFieldTypes=thisPhoneField.ContentType();
		const TInt numFieldTypes(thisPhoneFieldTypes.FieldTypeCount());

		if (numFieldTypes!=numTypesToMatch)
			{
			continue;
			}

		TInt numTypesLeftToMatch=numTypesToMatch;
		for (TInt ii=0;ii<numFieldTypes;++ii)
			{
			TFieldType thisFieldType(thisPhoneFieldTypes.FieldType(ii));
			for (TInt jj=0;jj<numTypesToMatch;++jj)
				{
				TFieldType thisFieldTypeToMatch(aTypesToMatch.FieldType(jj));
				if (thisFieldType==thisFieldTypeToMatch)
					{
					if (--numTypesLeftToMatch<=0)
						{
						return thisPhoneField;
						}

					break;
					}
				}
			}
		}
	}

void CTestContact::SetPictureFieldL(const TDesC8& aPicture) const
	{

	CContactItemFieldSet& fieldSet=iContactItem->CardFields();
	TInt pos;
	pos=fieldSet.Find(KUidContactFieldPicture);
	User::LeaveIfError(pos);
	fieldSet[pos].StoreStorage()->SetThingL(aPicture);

	}
