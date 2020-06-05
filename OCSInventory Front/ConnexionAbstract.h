//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Connexion.h: interface for the CConnexion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNEXION_H__2B46142F_9B28_46F8_BA85_6407579D824A__INCLUDED_)
#define AFX_CONNEXION_H__2B46142F_9B28_46F8_BA85_6407579D824A__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "Log.h"
#include "Config.h"
#include "RequestAbstract.h"
#include "ResponseAbstract.h"

class OCSINVENTORYFRONT_API CConnexionAbstract
{

public: // Methods

	// Standard destructor and constructor
	CConnexionAbstract();
	virtual ~CConnexionAbstract();

	// Send a request and return a response
	virtual CByteArray * sendRequest(CRequestAbstract*) = NULL;
	// Download and store LABEL file
	virtual BOOL getLabel( LPCTSTR lpstrFilename) = NULL;
	// A simple get that get a binary content into a file
	virtual BOOL getFile( LPCTSTR lpstrURL, LPCTSTR lpstrFilename) = NULL;

	// Get Status Code or Error Code
	virtual DWORD getErrorStatus() = OCS_RESPONSE_ERROR_SUCCESS;
	virtual LPCTSTR getErrorString() = NULL;

protected: // Methods
	// Object logger
	CLog		*m_pLogger;
};

#endif // !defined(AFX_CONNEXION_H__2B46142F_9B28_46F8_BA85_6407579D824A__INCLUDED_)
