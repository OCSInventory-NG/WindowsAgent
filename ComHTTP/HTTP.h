//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// HTTP.h : main header file for the HTTP DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "Provider Classes.h"
#include "HTTPConnexion.h"
#include "HTTPConfig.h"	// Added by ClassView


// CHTTPApp
// See HTTP.cpp for the implementation of this class
//

class CHTTPApp : public CWinApp
{
public:
	CHTTPApp();

// Overrides
public:
	virtual BOOL InitInstance();

	// Create an OCS server configuration object and load params from provided file and section
	// If file is NULL, uses agent's default configuration file
	// If section is NULL, uses provider default section name
	CServerConfig * createServerConfig( LPCTSTR lpstrFile = NULL, LPCTSTR lpstrSection = NULL);
	// Write OCS server configuration to file and section provided
	// If file is NULL, uses agent's default configuration file
	// If section is NULL, uses provider default section name
	BOOL writeServerConfig( CServerConfig *pConfig, LPCTSTR lpstrFile = NULL, LPCTSTR lpstrSection = NULL);
	// Delete an OCS server configuration object
	void destroyServerConfig( CServerConfig *pConfig);

	// Create and delete a connexion to OCS server, using configuration provided
	CConnexionAbstract * createServerConnexion( CServerConfig *pConfig);
	void destroyServerConnexion(CConnexionAbstract *pCon);

	DECLARE_MESSAGE_MAP()
};
