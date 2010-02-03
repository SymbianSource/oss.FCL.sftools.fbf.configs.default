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

#if (!defined __SMOKETEST_PROPERTY_H__)
#define __SMOKETEST_PROPERTY_H__

// EPOC includes
#include <test/testexecutestepbase.h>
#include <e32property.h>

/**
 This class contains generic utility method to set system wide variable usign publish and subscribe
*/
class CTestProperty : public CBase
{
public:
	IMPORT_C CTestProperty(CTestStep& aTestStep);
	IMPORT_C ~CTestProperty();

	/**
	* Set a property
	*
	* \param aCategory catagory of the RProperty
	* \param aKey key of the catagory
	* \param aValue value of the key
	*/
	IMPORT_C TInt SetPropertyL(const TUid aCategory, const TInt aKey, const TInt aValue);

	/**
	* Set a property
	*
	* \param aCategory catagory of the RProperty
	* \param aKey key of the catagory
	* \param aValue value of the key
	*/
	IMPORT_C TInt SetPropertyL(const TUid aCategory, const TInt aKey, const TDesC8& aValue);

	/**
	* Set a property
	*
	* \param aCategory catagory of the RProperty
	* \param aKey key of the catagory
	* \param aValue value of the key
	*/
	IMPORT_C TInt SetPropertyL(const TUid aCategory, const TInt aKey, const TDesC16& aValue);

	/**
	* Get a property
	*
	* \param aCategory catagory of the RProperty
	* \param aKey key of the catagory
	* \param aValue value of the key returned
	*/
	IMPORT_C TInt GetPropertyL(const TUid aCategory, const TInt aKey, TInt& aValue);

	/**
	* Get a property
	*
	* \param aCategory catagory of the RProperty
	* \param aKey key of the catagory
	* \param aValue value of the key returned
	*/
	IMPORT_C TInt GetPropertyL(const TUid aCategory, const TInt aKey, TDes8& aValue);

	/**
	* Get a property
	*
	* \param aCategory catagory of the RProperty
	* \param aKey key of the catagory
	* \param aValue value of the key returned
	*/
	IMPORT_C TInt GetPropertyL(const TUid aCategory, const TInt aKey, TDes16& aValue);

private:
	CTestStep&	iTestStep;
	RProperty	iProperty;
};

#endif /* __SMOKETEST_PROPERTY_H__ */
