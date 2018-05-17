//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DownloadDlg.h: interface for the CDownloadDlg class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "SimpleBrowser.h"

// CDownloadDlg dialog
class CDownloadDlg : public CDialog
{
// Construction
public:
	CDownloadDlg(CWnd* pParent = NULL);	// standard constructor

	// Set package informations
	// Is user allowed to abort action
	void setAbortAllowed( BOOL bAbort = FALSE);
	// Is user allowed to delay action
	void setDelayAllowed( BOOL bDelay = FALSE);
	// Is reboot action required after package setup (will display automatically rebooting instead of validating)
	void setRebootRequired( BOOL bReboot = FALSE);
	// Timeout before validating default action OK
	void setTimeOut( UINT uTimeOut = 0);
	// Notification text to display to user
	void setNotification( LPCTSTR lpstrText);
	// Return user choice if delaying action is allowed (if user press "delay" button or not)
	BOOL isDelayed();

// Dialog Data
	enum { IDD = IDD_DOWNLOAD_DIALOG };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCancel();
	afx_msg void OnDelay();
	DECLARE_MESSAGE_MAP()

// Implementation
protected:
	HICON m_hIcon;
	BOOL m_bAbortAllowed;	// Is user allowed to abort action
	BOOL m_bDelayAllowed;	// Is user allowed to delay action
	BOOL m_bReboot;			// Is reboot action required
	BOOL m_bDelayed;		// User choose to delay action or not (if allowed)
	UINT m_uNotifyCountdown;// Timeout before validating default action OK
	UINT m_uWaited;			// Current cowntdown
	CString	m_csEdit;

	CButton	m_buttonOK;
	CButton	m_buttonCancel;
	CButton	m_buttonWait;
	SimpleBrowser m_Browser;// HTML display
};
