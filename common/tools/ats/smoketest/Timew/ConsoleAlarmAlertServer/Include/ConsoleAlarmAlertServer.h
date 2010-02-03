// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TALARMALERTSERVER.H
// 
//

#ifndef __CONSOLEALARMALERTSERVER_H__
#define __CONSOLEALARMALERTSERVER_H__

// System includes
#include <e32base.h>

// User includes

// Type definitions

// Constants

// Enumerations

// Classes referenced
class CTestAlarmAlertConsole;
class CConsoleAlarmAlertSession;

///////////////////////////////////////////////////////////////////////////////////////
// ----> CConsoleAlarmAlertServer (header)
///////////////////////////////////////////////////////////////////////////////////////
class CConsoleAlarmAlertServer : public CPolicyServer
	{
///////////////////////////////////////////////////////////////////////////////////////
public:										// STATIC CONSTRUCT / DESTRUCT
///////////////////////////////////////////////////////////////////////////////////////
	static CConsoleAlarmAlertServer*		NewLC();
	~CConsoleAlarmAlertServer();

public:
	inline void								AddResponseArray(TAny* aResponseArray);
	void									SetAttachment(HBufC8* aData);
	inline HBufC8*							Attachment() const;
	inline TAny*							ResponseArray() const;
	inline CConsoleAlarmAlertSession*		WaitingSession() const;
	inline void								SetNotifying(TBool aIsNotifying);
	inline TBool							IsNotifying() const;
///////////////////////////////////////////////////////////////////////////////////////
private:									// INTERNAL CONSTRUCT
///////////////////////////////////////////////////////////////////////////////////////
	CConsoleAlarmAlertServer();
	void									ConstructL();

///////////////////////////////////////////////////////////////////////////////////////
public:										// FROM CServer2
///////////////////////////////////////////////////////////////////////////////////////
	CSession2*								NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

///////////////////////////////////////////////////////////////////////////////////////
private:									// FROM CActive
///////////////////////////////////////////////////////////////////////////////////////
	TInt									RunError(TInt aError);

///////////////////////////////////////////////////////////////////////////////////////
private:									// MEMBER DATA
///////////////////////////////////////////////////////////////////////////////////////
	TAny*									iInstructionSet;
	HBufC8*									iAttachment;
	CConsoleAlarmAlertSession*				iWaitingSession;
	TBool									iIsNotifying;
	};

// inline functions
void CConsoleAlarmAlertServer::AddResponseArray(TAny* aResponseArray)		{ iInstructionSet = aResponseArray; }
HBufC8* CConsoleAlarmAlertServer::Attachment() const						{ return iAttachment; }
TAny* CConsoleAlarmAlertServer::ResponseArray() const						{ return iInstructionSet; }
CConsoleAlarmAlertSession* CConsoleAlarmAlertServer::WaitingSession() const	{ return iWaitingSession; }
void CConsoleAlarmAlertServer::SetNotifying(TBool aIsNotifying)				{ iIsNotifying = aIsNotifying; }
TBool CConsoleAlarmAlertServer::IsNotifying()	const						{ return iIsNotifying; }
#endif
