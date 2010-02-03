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
// This contains CTestContactsAddEntries
// 
//

// Epoc include
#include <TestExecuteLog.h>

// User include
#include "TestContactsAddEntries.h"
#include "TestContact.h"

/*@{*/
/// Literal constants
_LIT(KCardTemplateName,	"TestTemplate");
_LIT(KCount,			"count");
_LIT(KFirstName,		"firstname%d");
_LIT(KLastName,			"lastname%d");
_LIT(KCompanyName,		"company%d");
_LIT(KJobTitle,			"job%d");
_LIT(KWorkPhone,		"workphone%d");
_LIT(KWorkMobile,		"workmobile%d");
_LIT(KHomePhone,		"homephone%d");
_LIT(KHomeMobile,		"homemobile%d");
_LIT(KFax,				"fax%d");
_LIT(KWorkEmailAddress,	"workemail%d");
_LIT(KInternetAddress,	"internet%d");
_LIT(KStreet,			"street%d");
_LIT(KTown,				"town%d");
_LIT(KCounty,			"county%d");
_LIT(KPostCode,			"postcode%d");
_LIT(KCountry,			"country%d");
_LIT(KNotes,			"notes%d");
_LIT(KNickName,			"nickName%d");
_LIT(KPictureName,		"picturename%d");
_LIT(KGroupName,		"groupname%d");
_LIT(KTemplateName,		"templatename%d");
_LIT(KOneFieldTemplate,	"OneFieldTemplate");
/*@}*/


CTestContactsAddEntries::CTestContactsAddEntries()
/**
Default Constructor
 */
	{
	SetTestStepName(_L("AddEntries"));
	}

CTestContactsAddEntries::~CTestContactsAddEntries()
/**
Destructor
 */
	{
	}

// Each test step must supply a implementation for doTestStepL
enum TVerdict CTestContactsAddEntries::doTestStepL()
/**
invoked by the test execute framework for exercising the
functionality under test
@return	EPass: The test passed succesfully;
		EFail: The test failed;
		EInconclusive: The result of the test was inconclusive;
		ETestSuiteError: Error in the test suite;
		EAbort: The test was aborted;
		EIgnore: The test was unexecuted
@leave system wide error codes
*/
	{

	// Printing to the console and log file
	INFO_PRINTF1(_L("Contacts Add Entries Test"));

	OpenDatabase();
	if ( TestStepResult() == EPass )
		{
		TRAPD(r, AddEntriesL());
		if (r!=KErrNone)
			{
			ERR_PRINTF2(_L("Test step failed with error code = %d"), r);
			SetTestStepResult(EFail);
			}
		}
	CleanupDatabase();

	// test steps return a result
	return TestStepResult();
	}

void CTestContactsAddEntries::AddEntriesL()
/**
This function adds contact entries to the database depending on entry type.
If the entry contains the group name then the contact will be added to the
particular group specified in the entry. If the entry contains the template
name then the contact item depending on the specified template name will be
created and added to the database.

@leave system wide error code
 */
	{
	TBool	groupExists = EFalse;
	TBool	templateFound = EFalse;
	TInt	count=1;

	if ( !GetIntFromConfig(ConfigSection(), KCount, count) )
		{
		count=1;
		}

	INFO_PRINTF2(_L("Number of Entries to Add = %d"), count);

	TBuf<KMaxScriptLineLength>	tempStore;
	TPtrC						ptrString;

	/*******************************************************************
	 To Create a contact card template depending upon the contact item
	 on which the new template's field set is based. The template name
	 on which the new template is based is read from the ini file.
	 *******************************************************************/

	for (TInt entry=0; entry<count; )
		{
		CContactItem* item = NULL;

		// The entry contains the template name
		tempStore.Format(KTemplateName(), ++entry);
		if (!GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("Entry %d has no Template Name"), entry);
			item = iDatabase->CreateContactCardTemplateLC(KCardTemplateName);
			}
		else
			{
			INFO_PRINTF2(_L("Template Name = %S"), &ptrString);
			CContactIdArray* templateList = NULL;
			CContactItem* tempTemplate = NULL;

			// Returns a copy of the database's template ID list
			templateList = iDatabase->GetCardTemplateIdListL();
			CleanupStack::PushL(templateList);

			// Returns the number of contact card templates that currently exist in the database.
			TInt tCount = iDatabase->TemplateCount();
			INFO_PRINTF2(_L("Number of existing templates = %d"), tCount);

			// Search for template in the template ID list
			for(TInt i = 0; i<tCount && templateFound == EFalse;i++)
				{
				// Reads a contact item
				tempTemplate = STATIC_CAST(CContactItem*, iDatabase->ReadContactLC((*templateList)[i]));

				// Gets the label for a contact card template.
				TPtrC tempLabel = STATIC_CAST(CContactCardTemplate*,tempTemplate)->GetTemplateLabelL();

							// Compare the template label
				if(!(tempLabel.Compare(ptrString)))
					{
					INFO_PRINTF2(_L("Template %S found"), &tempLabel);

					// Creates a contact card template and adds it to the database
					item = iDatabase->CreateContactCardTemplateLC(tempTemplate,tempLabel);

					// Compare the template label with OneFieldTemplate and add field to it.
					if(!(tempLabel.Compare(KOneFieldTemplate)))
						{
						// Allocates and constructs a contact item field
						CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldGivenName);
						item->AddFieldL(*field);
						CleanupStack::Pop(field); // field
						}
					CleanupStack::Pop(item);
					templateFound = ETrue;
					}
				  CleanupStack::PopAndDestroy(tempTemplate);
				}

			if (!templateFound)
				{
				ERR_PRINTF2(_L("Ini Template %S does not match existing templates"), &ptrString);
				SetTestStepResult(EFail);
				}

			// Set templateFound to false for next entry
			templateFound = EFalse;

			CleanupStack::PopAndDestroy(templateList);
			CleanupStack::PushL(item);
			}

		CTestContact*	contact=CTestContact::NewLC(*item);

		tempStore.Format(KFirstName(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("firstname = %S"), &ptrString);
			contact->SetFirstNameL(ptrString);
			}

		tempStore.Format(KNickName(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("NickName = %S"), &ptrString);
			TRAPD(err, contact->SetNickNameL(ptrString));
			if ( err != KErrNone )
				{
				ERR_PRINTF2(_L("SetNickNameL Leave = %d"), err);
				SetTestStepResult(EFail);
				}
			}

		tempStore.Format(KLastName(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("lastname = %S"), &ptrString);
			contact->SetLastNameL(ptrString);
			}

		tempStore.Format(KCompanyName(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("company = %S"), &ptrString);
			contact->SetCompanyNameL(ptrString);
			}

		tempStore.Format(KJobTitle(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("job = %S"), &ptrString);
			contact->SetJobTitleL(ptrString);
			}

		tempStore.Format(KWorkPhone(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("work phone = %S"), &ptrString);
			contact->SetWorkPhoneL(ptrString);
			}

		tempStore.Format(KWorkMobile(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("work mobile = %S"), &ptrString);
			contact->SetWorkMobileL(ptrString);
			}

		tempStore.Format(KHomePhone(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("home phone = %S"), &ptrString);
			contact->SetHomePhoneL(ptrString);
			}

		tempStore.Format(KHomeMobile(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("home mobile = %S"), &ptrString);
			contact->SetHomeMobileL(ptrString);
			}

		tempStore.Format(KFax(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("fax = %S"), &ptrString);
			contact->SetFaxL(ptrString);
			}

		tempStore.Format(KWorkEmailAddress(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("work email = %S"), &ptrString);
			contact->SetWorkEmailAddressL(ptrString);
			}

		tempStore.Format(KInternetAddress(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("internet = %S"), &ptrString);
			contact->SetInternetAddressL(ptrString);
			}

		tempStore.Format(KStreet(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("street = %S"), &ptrString);
			contact->SetStreetL(ptrString);
			}

		tempStore.Format(KTown(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("town = %S"), &ptrString);
			contact->SetTownL(ptrString);
			}

		tempStore.Format(KCounty(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("county = %S"), &ptrString);
			contact->SetCountyL(ptrString);
			}

		tempStore.Format(KPostCode(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("post code = %S"), &ptrString);
			contact->SetPostCodeL(ptrString);
			}

		tempStore.Format(KCountry(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("country = %S"), &ptrString);
			contact->SetCountryL(ptrString);
			}

		tempStore.Format(KNotes(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("notes = %S"), &ptrString);
			contact->SetNotesL(ptrString);
			}

		tempStore.Format(KPictureName(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("Picture Name = %S"), &ptrString);

			HBufC8*	pictureName8 = HBufC8::NewLC(ptrString.Length());
			pictureName8->Des().Copy(ptrString);

			contact->SetPictureL(*pictureName8);

			CleanupStack::PopAndDestroy(pictureName8);
			}

		tempStore.Format(KGroupName(), entry);
		if (GetStringFromConfig(ConfigSection(), tempStore, ptrString))
			{
			INFO_PRINTF2(_L("Group Name = %S"), &ptrString);

			// Flag for adding contacts to group.
			groupExists = ETrue;
			}

		const TContactItemId id = iDatabase->AddNewContactL(contact->ContactItem());

		/********************************************************************
		This is for adding the contact entry to the specified group.
		********************************************************************/
		if(groupExists)
			{
			INFO_PRINTF1(_L("Adding Item "));
			CContactIdArray* groupList = NULL;

			// Returns a copy of the database's group ID list
			groupList = iDatabase->GetGroupIdListL();
			CleanupStack::PushL(groupList);

			// Returns the number of groups that currently exist in the database
			TInt groupCount = iDatabase->GroupCount();


			// Search for the group
			for(TInt i = 0; i<groupCount;i++)
				{
				// Reads a contact item
				CContactGroup* group = STATIC_CAST(CContactGroup*, iDatabase->ReadContactLC((*groupList)[i]));

				// Gets the contact item's ID.
				const TContactItemId groupId1 = group->Id();

				// Gets the group label.
				TPtrC	label = group->GetGroupLabelL();
				// Compare the group label with group name read from ini file.
				if(!(label.Compare(ptrString)))
					{
					/**
					Sets a contact item which already exists in the database
					to be a member of a contact group
					 */
					iDatabase->AddContactToGroupL(id, groupId1);
					INFO_PRINTF1(_L("Item added successfully"));
					groupExists = EFalse;
					}
				CleanupStack::PopAndDestroy(group);

				}
			CleanupStack::PopAndDestroy(groupList);
			}

		CleanupStack::PopAndDestroy(contact);
		CleanupStack::PopAndDestroy(item);

		}
	}
