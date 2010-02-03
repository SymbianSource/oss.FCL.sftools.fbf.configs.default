// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ENDTASKTESTAPP_H
#define __ENDTASKTESTAPP_H


/**
Application class
*/
#include <eikapp.h>
class CEndTaskTestApplication : public CEikApplication
	{
public:
	static CApaApplication* NewApplication();
	~CEndTaskTestApplication();
	
private:
	CEndTaskTestApplication();
	
	// from CApaApplication
	TUid AppDllUid() const;
	CApaDocument* CreateDocumentL();
	};


/**
Document class
*/
#include <eikdoc.h>
class CEikAppUi;
class CEikApplication;
class CEndTaskTestDocument : public CEikDocument
	{
public:
	static CEndTaskTestDocument* NewL(CEikApplication& aApp);
	~CEndTaskTestDocument();
	
private:
	CEndTaskTestDocument(CEikApplication& aApp);
	
	// from CEikDocument
	CEikAppUi* CreateAppUiL();
	};


/**
Application UI class, root of all graphical user interface in this application
*/
#include <eikappui.h>
class CEndTaskTestAppUi : public CEikAppUi
    {
public:
	CEndTaskTestAppUi();
	~CEndTaskTestAppUi();
	
	// from CEikAppUi
	void ConstructL();
	void HandleCommandL(TInt aCommand);
	
	// from CCoeAppUi
	void HandleApplicationSpecificEventL(TInt aType,const TWsEvent& aEvent);
	};


#endif // __ENDTASKTESTAPP_H
