//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#if !defined(AFX_TAGINPUTDLG_H__088A24B2_8338_4520_872E_31781BCE7337__INCLUDED_)
#define AFX_TAGINPUTDLG_H__088A24B2_8338_4520_872E_31781BCE7337__INCLUDED_

#pragma once

#include "Resource.h"

// TagInputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTagInputDlg dialog

class CTagInputDlg : public CDialog
{
// Construction
public:
	LPCTSTR getTagValue();
	void setLabelText( LPCTSTR lpstrLabelText);
	CTagInputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTagInputDlg)
	enum { IDD = IDD_CU };
	CEdit	m_controlTag;
	CString	m_csLabelText;
	CString	m_csTagValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTagInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTagInputDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAGINPUTDLG_H__088A24B2_8338_4520_872E_31781BCE7337__INCLUDED_)
