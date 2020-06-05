//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// HTTPConnexion.h: interface for the CHTTPConnexion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPCONNEXION_H__6687A2BD_1658_4CA3_93AC_DC725FE752C9__INCLUDED_)
#define AFX_HTTPCONNEXION_H__6687A2BD_1658_4CA3_93AC_DC725FE752C9__INCLUDED_

#pragma once

#include "Provider Classes.h"
#include "OCSInventory Front.h"
#include "ConnexionAbstract.h"
#include "HTTPConfig.h"
#include "curl/curl.h"

class OCS_PROVIDER_API CHTTPConnexion : public CConnexionAbstract
{
public: // Methods
	// The constructor
	CHTTPConnexion( CHTTPConfig *pConfig);
	//Standard destructor
	virtual ~CHTTPConnexion();

	// Send a request and return a response
	virtual CByteArray * sendRequest(CRequestAbstract *pRequest);
	// A simple get that get a binary content into a file
	BOOL getFile( LPCTSTR lpstrURL, LPCTSTR lpstrFilename);
	BOOL getLabel( LPCTSTR lpstrFilename);

	// A simple get that get a binary content (allocated in memory !!!)
	CByteArray * simpleGet( LPCTSTR lpstrURL);
	// A simple post (text data) that get a binary content (allocated in memory !!!)
	CByteArray * simplePost( LPCTSTR lpstrURL, LPCTSTR lpstrPost);
	// Get HTTP Status Code or Error Code
	DWORD getErrorStatus();
	LPCTSTR getErrorString();

private: // Methods
	// Initialize cURL library
	BOOL initLibCurl();
	// Set proxy credentials if needed
	BOOL setProxyAuth();
	// Retrieve HTTP status code from response headers
	DWORD getHttpCode( CString &csHeader);


private: // Attributes
	// Communication server config
	CHTTPConfig	*m_pConfig;
	// cURL handler
    CURL 		*m_pCurl;
	// General headers
	CString		m_csUserAgent;
	// HTTP return code
	DWORD		m_dwErrorCode;
	CString		m_csErrorString;
};

#endif // !defined(AFX_HTTPCONNEXION_H__6687A2BD_1658_4CA3_93AC_DC725FE752C9__INCLUDED_)
