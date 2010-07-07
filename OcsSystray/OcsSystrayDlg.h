//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsSystrayDlg.h : header file
//

#include "MyHyperLink.h"
#include "SystemTray.h"
#include "OCSInventory Front.h"
#include <winsvc.h>
#include "Config.h"

#pragma once

// Message used for Systray icon notifycations
#define WM_SYSTRAY_ICON_NOTIFY			WM_USER+1
#define ID_SYSTRAY_ICON					5000


// COcsSystrayDlg dialog
class COcsSystrayDlg : public CDialog
{
// Construction
public:
	COcsSystrayDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OCSSYSTRAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	// Hyperlink to OCS website
	CMyHyperLink m_HLink;
	// Icon in the systray
	CSystemTray	 m_iconTray;

	// To check service status or send messages to service
	BOOL IsServiceRunning( CString &csStatusText);
	BOOL ServiceSendMessage( DWORD dwMsg);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	// Update service status every 5 seconds
	afx_msg void OnTimer( UINT nIDEvent);
	// Show version information dialogbox
	afx_msg void OnDisplay();
	// Ask servive to start agent now
	afx_msg void OnRunAgent();
	// Show inventory now
	afx_msg void OnShowInventory();
	// Hide version information dialogbox
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()
};
