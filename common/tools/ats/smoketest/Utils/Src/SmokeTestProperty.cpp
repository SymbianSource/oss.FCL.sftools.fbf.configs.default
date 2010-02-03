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
// This contains CTestProperty
// 
//

#include "SmokeTestProperty.h"

// EPOC32 include
#include <e32base.h>

EXPORT_C CTestProperty::CTestProperty(CTestStep& aTestStep)
:	CBase()
,	iTestStep(aTestStep)
	{
	}

EXPORT_C CTestProperty::~CTestProperty()
	{
	}

EXPORT_C TInt CTestProperty::SetPropertyL(const TUid aCategory, const TInt aKey, const TInt aValue)
	{
	//Define the property, in case it is being used for the first time.
	TInt	ret=RProperty::Define(aCategory, aKey, RProperty::EInt);
	if ( (ret==KErrNone) || (ret== KErrAlreadyExists) )
		{
		//Attach to the property so that you can track any chnages to it.
		ret=iProperty.Attach(aCategory, aKey);
		if ( ret==KErrNone )
			{
			//Set the new test number and wait until the change is in effect.
			TRequestStatus	status;
			iProperty.Subscribe(status);
			ret=iProperty.Set(aCategory, aKey, aValue);
			if ( ret==KErrNone )
				{
				User::WaitForRequest(status);
				ret=status.Int();
				if ( ret != KErrNone )
					{
					iTestStep.INFO_PRINTF2(_L("request complete error=%d"), ret);
					}
				}
			else
				{
				iTestStep.INFO_PRINTF2(_L("RProperty::Set error=%d"), ret);
				}
			iProperty.Close();
			}
		else
			{
			iTestStep.INFO_PRINTF2(_L("RProperty::Attach error=%d"), ret);
			}
		}
	else
		{
		iTestStep.INFO_PRINTF2(_L("RProperty::Define error=%d"), ret);
		}

	return ret;
	}

EXPORT_C TInt CTestProperty::SetPropertyL(const TUid aCategory, const TInt aKey, const TDesC8& aValue)
	{
	//Define the property, in case it is being used for the first time.
	TInt	ret=RProperty::Define(aCategory, aKey, RProperty::EByteArray);
	if ( (ret==KErrNone) || (ret== KErrAlreadyExists) )
		{
		//Attach to the property so that you can track any chnages to it.
		ret=iProperty.Attach(aCategory, aKey);
		if ( ret==KErrNone )
			{
			//Set the new test number and wait until the change is in effect.
			TRequestStatus	status;
			iProperty.Subscribe(status);
			ret=iProperty.Set(aCategory, aKey, aValue);
			if ( ret==KErrNone )
				{
				User::WaitForRequest(status);
				ret=status.Int();
				}
			iProperty.Close();
			}
		}

	return ret;
	}

EXPORT_C TInt CTestProperty::SetPropertyL(const TUid aCategory, const TInt aKey, const TDesC16& aValue)
	{
	//Define the property, in case it is being used for the first time.
	TInt	ret=RProperty::Define(aCategory, aKey, RProperty::EText);
	if ( (ret==KErrNone) || (ret== KErrAlreadyExists) )
		{
		//Attach to the property so that you can track any chnages to it.
		ret=iProperty.Attach(aCategory, aKey);
		if ( ret==KErrNone )
			{
			//Set the new test number and wait until the change is in effect.
			TRequestStatus	status;
			iProperty.Subscribe(status);
			ret=iProperty.Set(aCategory, aKey, aValue);
			if ( ret==KErrNone )
				{
				User::WaitForRequest(status);
				ret=status.Int();
				}
			iProperty.Close();
			}
		}

	return ret;
	}

EXPORT_C TInt CTestProperty::GetPropertyL(const TUid aCategory, const TInt aKey, TInt& aValue)
	{
	return RProperty::Get(aCategory, aKey, aValue);
	}

EXPORT_C TInt CTestProperty::GetPropertyL(const TUid aCategory, const TInt aKey, TDes8& aValue)
	{
	return RProperty::Get(aCategory, aKey, aValue);
	}

EXPORT_C TInt CTestProperty::GetPropertyL(const TUid aCategory, const TInt aKey, TDes16& aValue)
	{
	return RProperty::Get(aCategory, aKey, aValue);
	}
