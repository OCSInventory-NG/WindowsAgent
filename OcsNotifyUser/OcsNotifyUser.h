
// OcsNotifyUser.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

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

	// Parse command line arguments
	BOOL parseCommandLine();

// Implementation
	DECLARE_MESSAGE_MAP()

protected: //Attributes
	UINT	m_uNotifcation;		// Notifcation type
	CString m_csMessage;		// Message to display to user
	BOOL	m_bCancel;			// Is cancel allowed or not
	BOOL	m_bDelay;			// Is delaying action allowed or not
	UINT	m_uTimeOut;			// Timeout for displaying message to user (OK assumed)
	int		m_nExitCode;		// Application exit code
};

extern COcsNotifyUserApp theApp;