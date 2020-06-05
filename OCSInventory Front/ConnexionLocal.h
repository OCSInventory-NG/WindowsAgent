//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ConnexionLocal.h: interface for the CConnexionLocal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNEXIONLOCAL_H__BAEDADE0_7E2F_439A_A189_32BE4071AC65__INCLUDED_)
#define AFX_CONNEXIONLOCAL_H__BAEDADE0_7E2F_439A_A189_32BE4071AC65__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "ConnexionAbstract.h"

class OCSINVENTORYFRONT_API CConnexionLocal : public CConnexionAbstract
{

public://Methods
	CConnexionLocal( LPCTSTR lpstrFilename);
	virtual ~CConnexionLocal();
	// Send a request and return a response
	CByteArray * sendRequest(CRequestAbstract *pRequest);
	// A simple get that get a binary content into a file
	BOOL getFile( LPCTSTR lpstrURL, LPCTSTR lpstrFilename);
	BOOL getLabel( LPCTSTR lpstrFilename);

	// Get Status Code or Error Code
	DWORD getErrorStatus();
	LPCTSTR getErrorString();


private: // Attributes
	// The .ocs filename
	CString m_csFileName;
	// Return code
	DWORD		m_dwErrorCode;
	CString		m_csErrorString;
};

#endif // !defined(AFX_CONNEXIONLOCAL_H__BAEDADE0_7E2F_439A_A189_32BE4071AC65__INCLUDED_)
