//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================


// OcsNotifyUser.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "log.h"
#include "resource.h"		// main symbols

// Dialog box type
#define NOTIFY_TYPE_PREINSTALL	1			// Notification before package installation
#define NOTIFY_TYPE_MSGBOX		2			// Default messagebox notification

// COcsNotifyUserApp:
// See OcsNotifyUser.cpp for the implementation of this class
//

class COcsNotifyUserApp : public CWinAppEx
{
public:
	COcsNotifyUserApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	DECLARE_MESSAGE_MAP()

protected: //Attributes
	// Retrieve the version from exe
	CString getVersion();
	// Parse command line arguments
	BOOL parseCommandLine();
	// Display preinstall DialogBox
	BOOL displayPreinstallDialogBox();
	// Display standard MessageBox
	BOOL displayMessageBox();

protected: //Attributes
	CLog	*m_pLogger;
	UINT	m_uNotifcation;		// Notifcation type
	CString m_csMessage;		// Message to display to user
	BOOL	m_bCancel;			// Is cancel allowed or not
	BOOL	m_bDelay;			// Is delaying action allowed or not
	UINT	m_uTimeOut;			// Timeout for displaying message to user (OK assumed)
	int		m_nExitCode;		// Application exit code
};

extern COcsNotifyUserApp theApp;