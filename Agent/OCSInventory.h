//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OCSInventory.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define	OCS_LABEL_FILE			_T( "label")

#include "log.h"
#include "Plugins.h"
#include "PrologRequest.h"
#include "InventoryRequest.h"
#include "PrologResponse.h"
#include "InventoryResponse.h"
#include "ConnexionAbstract.h"
#include "ConnexionLocal.h"
#include "ComProvider.h"
#include "CapRegistry.h"
#include "CapIpdiscover.h"
#include "CapSnmp.h"
#include "CapDownload.h"
#include "CapExecute.h"
#include "NotifyUser.h"

// Application exit codes
#define OCS_APP_NO_ERROR				0
#define OCS_APP_GENERIC_ERROR			1
#define OCS_APP_ALREADY_RUNNING_ERROR	2
#define OCS_APP_COM_PROVIDER_ERROR		3
#define OCS_APP_NETWORK_ERROR			4

// COCSInventoryApp:
// See OCSInventory.cpp for the implementation of this class
//
class COCSInventoryApp : public CWinApp
{
public:
	COCSInventoryApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// Parse command line arguments
	BOOL parseCommandLine();
	// Retrieve the agent version from exe
	CString getVersion();

// Implementation
	DECLARE_MESSAGE_MAP()

public: //Attributes
	CLog	*m_pLogger;

protected: //Attributes
	CPlugins *m_pPlugins;
	CConfig *m_pConfig;
	int		m_nExitCode;		// Application exit code
};

extern COCSInventoryApp theApp;