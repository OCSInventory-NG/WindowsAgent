//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// TagInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ocsinventory.h"
#include "TagInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTagInputDlg dialog


CTagInputDlg::CTagInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTagInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTagInputDlg)
	m_csLabelText = _T("");
	m_csTagValue = _T("");
	//}}AFX_DATA_INIT
}


void CTagInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTagInputDlg)
	DDX_Control(pDX, IDC_VAL, m_controlTag);
	DDX_Text(pDX, IDC_LABEL_TEXT, m_csLabelText);
	DDX_Text(pDX, IDC_VAL, m_csTagValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTagInputDlg, CDialog)
	//{{AFX_MSG_MAP(CTagInputDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTagInputDlg message handlers

BOOL CTagInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTagInputDlg::OnOK() 
{
	CDialog::OnOK();
}

void CTagInputDlg::setLabelText(LPCTSTR lpstrLabelText)
{
	m_csLabelText = lpstrLabelText;
}

LPCTSTR CTagInputDlg::getTagValue()
{
	return m_csTagValue;
}
