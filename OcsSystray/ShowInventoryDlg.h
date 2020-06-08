//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyleft 2010 OCS Inventory Team
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#pragma once

#include "resource.h"
#include "MyHyperLink.h"
#include "SimpleBrowser.h"

// CShowInventoryDlg dialog

class CShowInventoryDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowInventoryDlg)

public:
	CShowInventoryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CShowInventoryDlg();

	void SetInventory( LPCTSTR lpstrFilename);

// Dialog Data
	enum { IDD = IDD_SHOW_INVENTORY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	HICON m_hIcon;
	// Hyperlink to OCS website
	CMyHyperLink m_HLink;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

private:
	CString m_csXml;
	SimpleBrowser m_Browser;
};
