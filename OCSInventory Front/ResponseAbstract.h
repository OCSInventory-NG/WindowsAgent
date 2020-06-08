//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ResponseAbstract.h: interface for the CResponseAbstract class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESPONSEABSTRACT_H__0AF24ACC_A77B_44B8_959F_9D62088A26F1__INCLUDED_)
#define AFX_RESPONSEABSTRACT_H__0AF24ACC_A77B_44B8_959F_9D62088A26F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ocsinventory Front Classes.h"
#include "Markup.h"
#include "Zip.h"
#include "OcsUtils.h"

#define OCS_RESPONSE_ERROR_SUCCESS	0
#define OCS_RESPONSE_ERROR_FILE		1
#define OCS_RESPONSE_ERROR_CURL		2
#define OCS_RESPONSE_ERROR_ZLIB		3

class OCSINVENTORYFRONT_API CResponseAbstract  
{
public: // Methods

	// Standard constructor and destructor
	CResponseAbstract();
	CResponseAbstract(CByteArray*);
	virtual ~CResponseAbstract();

	// Return the xml content in CMarkup format
	CMarkup *getXmlContent();
	// Return the xml content in CString format
	CStringA getStringContent();
	// Return the error status (the OCS_RESPONSE_ERROR_SUCCESS is the good one)
	DWORD getErrorStatus();
	BOOL isSuccess();
	// Usually set by the connexion layer
	void setErrorStatus( DWORD );
	void setErrorString( LPCTSTR );
	// Retrieve the error string if error status != OCS_RESPONSE_ERROR_SUCCESS
	CString getErrorString();

protected: // Methods

	// Generic function to retrieve parameters
	BOOL	getParamValue();

protected: // Attributes

	CByteArray	*m_pRawData;
	CMarkup		m_cmXml;
	CStringA	m_csContent;
	DWORD		m_dwErrorStatus;
	CString		m_csErrorString;

};

#endif // !defined(AFX_RESPONSEABSTRACT_H__0AF24ACC_A77B_44B8_959F_9D62088A26F1__INCLUDED_)
