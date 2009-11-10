// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/*
Quietus is Medieval Latin for discharged from an obligation, which comes form the Latin 
(he is) at rest. So its a synonym for death :). See teh references below.
Which is exaclt what this object is all about - it starts the process that will kill any
process based around a CActiveScheduler.

from Dictionary.com

qui·e·tus

   1. Something that serves to suppress, check, or eliminate.
   2. Release from life; death.
   3. A final discharge, as of a duty or debt.
[Short for Middle English quietus (est), (he is) discharged (of an obligation),
from Medieval Latin quitus (est), from Latin, (he is) at rest. See quiet.]

from thesaurus.com
Main Entry:  	death
Part of Speech:  noun
Definition:  	end
Synonyms:  	afterlife, annihilation, bereavement, casualty, cessation, curtains*, darkness, decease, demise, departure, destruction, dissolution, downfall, dying, end, ending, eradication, eternal rest, euthanasia, exit, expiration, extermination, extinction, fatality, finis*, finish, future home, grave, grim reaper*, heaven, loss, mortality, necrosis, obliteration, oblivion, paradise, parting, passing, passing over, quietus, release, repose, ruin, ruination, silence, sleep, termination, tomb, un
Antonyms:  	being, birth, existence, life, living, nascency
Source:  	Roget's New Millennium™ Thesaurus, First Edition (v 1.2.1)
Copyright © 2006 by Lexico Publishing Group, LLC. All rights reserved.
* = informal or slang

*/

#ifndef __LBSQUIETUS_H__
#define __LBSQUIETUS_H__

#include <e32base.h>

/** 
@internalTechnology
@released

CActive object, starts the process to kill any
process based around a CActiveScheduler.
*/

class CQuietus : public CActive
	{
public:
	static CQuietus* NewL();
	~CQuietus();
	void CoupDeGrace();
private:
	CQuietus(); // make this private.
	// from CActive
	void RunL();
	void DoCancel(); 
	};
	
#endif // __LBSQUIETUS_H__
