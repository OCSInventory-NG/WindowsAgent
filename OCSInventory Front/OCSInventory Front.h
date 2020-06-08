//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OCSInventory Front.h : main header file for the OCSInventory Front DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "Log.h"
#include "Deviceid.h"
#include "Config.h"
#include "ComProvider.h"

// COCSInventoryFrontApp
// See OCSInventory Front.cpp for the implementation of this class
//

class COCSInventoryFrontApp : public CWinApp
{
public:
	COCSInventoryFrontApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	CString getFrameworkVersion();
	// Get application install folder
	LPCTSTR getInstallFolder();
	// Get application data storage folder (working directory)
	LPCTSTR getDataFolder();
	// Get computer unique ID
	CDeviceid * getDeviceID();
	// Get logger object
	CLog * getOcsLogger();
	// Get Agent Config
	CConfig * getAgentConfig();
	// Get Communication Provider to use with Communication server
	CComProvider * getComServerProvider();

	// Set working directory
	BOOL setDataFolder( LPCTSTR lpstrDir);

protected: // Attributes
	// Application install folder
	CString m_csInstallFolder;
	// Application data storage folder
	CString m_csDataFolder;
	// A pointer to the logger
	CLog* m_pLogger;
	// Computer unique ID
	CDeviceid * m_pDeviceID;
	// Agent settings
	CConfig *m_pAgentConfig;
	// Communication Provider
	CComProvider *m_pComServerProvider;

	DECLARE_MESSAGE_MAP()
};
