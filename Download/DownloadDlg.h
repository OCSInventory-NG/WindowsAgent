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

#define MAX_LINES 15
#define MIN_LINES 3

// CDownloadDlg dialog
class CDownloadDlg : public CDialog
{
// Construction
public:
	CDownloadDlg(CWnd* pParent = NULL);	// standard constructor

	// Set package informations
	void setPackage( CPackage *pPack);
	// Allow to check if user press "delay" button
	BOOL isDelayed();

// Dialog Data
	enum { IDD = IDD_DOWNLOAD_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	BOOL m_bAbortAllowed;
	BOOL m_bDelayAllowed;
	BOOL m_bDelayed;
	UINT m_uNotifyCountdown;
	UINT m_uWaited;
	CString	m_csEdit;

	CButton	m_buttonOK;
	CButton	m_buttonCancel;
	CButton	m_buttonWait;
	CEdit	m_editText;
	SimpleBrowser m_Browser;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCancel();
	afx_msg void OnDelay();
	DECLARE_MESSAGE_MAP()
};
