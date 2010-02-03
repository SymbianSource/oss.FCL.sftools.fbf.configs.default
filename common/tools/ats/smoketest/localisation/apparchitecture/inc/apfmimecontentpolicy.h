/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef __APFMIMECONTENTPOLICY_H__
#define __APFMIMECONTENTPOLICY_H__

#include <e32base.h>

class CApfMimeContentPolicyImpl;
class RFile;
class RFs;

/**
CApfMimeContentPolicy is utility class for checking MIME types and 
file extensions against closed content list. It also provides a function 
for checking if a given file is a DRM envelope, and a convinience function
which combines all the checks mentioned above. 

Usage:

@code	
  TFileName fileName = <name of the file to be checked>
  TUid uid;
  TDataType recData;

  CApfMimeContentPolicy *ccp = CApfMimeContentPolicy::NewLC();

  if (ccp->IsDRMEnvelope( fileName ))	
	{
     // Whatever needs to be done if file is DRM envelope.
	}

  // Check file extension.	
  TParse parser;
  parser.Set(fileName, NULL, NULL);
  if (IsClosedExtension(parser.Ext()))
	{
    // Do what needs to be done if closed content.
	}

  iLs.AppForDocument(fileName, uid, recData);  // Get MIME type for file.

  if (ccp->IsClosedType(recData.Des())   // Check if this mime type is closed.
	{
     // Whatever needs to be done if type is closed.
	}
	
  // Or you can achieve same result calling a convinience function
  // which combines all the steps above:
  if (cpp->IsClosedFileL( fileName );
 	{
    // Do whatever has to be done if file is closed.	
	}

  // Don't forget to clean up.
	CleanupStack::PopAndDestroy();  // ccp	
@endcode

@publishedPartner
@released
*/
class CApfMimeContentPolicy : public CBase
	{
public: // Constructors and destructor
	IMPORT_C static CApfMimeContentPolicy* NewL();
	IMPORT_C static CApfMimeContentPolicy* NewLC();
	IMPORT_C static CApfMimeContentPolicy* NewL(RFs& aFs);
	IMPORT_C static CApfMimeContentPolicy* NewLC(RFs& aFs);
	IMPORT_C ~CApfMimeContentPolicy();
public:  // New functions
	IMPORT_C TBool IsClosedType(const TDesC& aMimeType);
	IMPORT_C TBool IsClosedExtension(const TDesC& aFileExtension);
	IMPORT_C TBool IsClosedFileL(const TDesC& aFileName);
	IMPORT_C TBool IsDRMEnvelopeL(const TDesC& aFileName);
	IMPORT_C TBool IsClosedFileL(RFile& aFileHandle);
	IMPORT_C TBool IsDRMEnvelopeL(RFile& aFileHandle);
private:
	CApfMimeContentPolicy();
	void ConstructL();
	void ConstructL(RFs& aFs);			
private:
	CApfMimeContentPolicyImpl* iImpl;  // Implementation.
	};


#endif  // __APFMIMECONTENTPOLICY_H__
