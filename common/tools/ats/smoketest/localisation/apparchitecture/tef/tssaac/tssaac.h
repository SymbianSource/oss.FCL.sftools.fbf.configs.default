// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @test
 @internalComponent  Internal Symbian test code
*/

#if (!defined __TSSAAC_H__)
#define __TSSAAC_H__

#include <domainmember.h>

//Write a result file for the only non-paniccing test case
_LIT(KSsaacResultFile, "c:\\SysStartApparc_TestNonPanic.txt");


/**
The class registers with the Domain Manager, listens for state transitions, and performs the test object
ro run the test cases.
In the end, the transition is always passed.
*/
class CApparcFuncInSystart : public CDmDomain
	{
public:
	static CApparcFuncInSystart* NewL(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId);
	static CApparcFuncInSystart* NewLC(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId);
	~CApparcFuncInSystart();
	void Start();

private:
	CApparcFuncInSystart(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId);
	void RunL();
	void DoCancel();
	};

#endif
