// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalComponent - Internal Symbian test code
*/

#if !defined(__RECUPGRADECONSTANTS_H__)
#define __RECUPGRADECONSTANTS_H__


_LIT(KUpgDocPath, "Z:\\system\\data\\TApsRecogUpgradeTest.upg");
_LIT(KUpgRscTargetDir,"C:\\resource\\plugins\\");
_LIT(KUpgDllTargetDir,"C:\\sys\\bin\\");
_LIT(KUpgRscSourcePath, "Z:\\system\\data\\recupgrade.rsc");
_LIT(KUpgRscTargetPath,"C:\\resource\\plugins\\recupgrade.rsc");
_LIT(KUpgDllTargetPath, "C:\\sys\\bin\\recupgrade.dll");
_LIT(KUpgDllSourcePathWinscw, "Z:\\system\\data\\recupgrade_winscw_rel.dll");
_LIT(KUpgDllSourcePathArmv5, "Z:\\system\\data\\recupgrade_armv5_rel.dll");
_LIT8(KMimeUpgType,"mime/upg");

#define KUidUpgradeRecognizerValue 0xA000B461

const TUid KUidUpgradeRecognizer={KUidUpgradeRecognizerValue};
const TInt KNumUpgradeTypes=1;
	
#endif
