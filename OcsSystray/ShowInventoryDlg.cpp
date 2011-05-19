//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyleft 2010 OCS Inventory NG Team
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ShowInventoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShowInventoryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CShowInventoryDlg dialog

IMPLEMENT_DYNAMIC(CShowInventoryDlg, CDialog)

CShowInventoryDlg::CShowInventoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowInventoryDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_csXml.Empty();
}

CShowInventoryDlg::~CShowInventoryDlg()
{
}

void CShowInventoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WEB_URL, m_HLink);
}


BEGIN_MESSAGE_MAP(CShowInventoryDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CShowInventoryDlg message handlers

BOOL CShowInventoryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// Set link to OCS Web site
	m_HLink.SetLinkUrl( _T( "http://www.ocsinventory-ng.org"));
	// Display XML using SimpleBrowser
	m_Browser.CreateFromControl( this, IDC_BROWSER);
	m_Browser.Navigate( m_csXml);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CShowInventoryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CShowInventoryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CShowInventoryDlg::SetInventory( LPCTSTR lpstrFilename)
{
	m_csXml = lpstrFilename;
}