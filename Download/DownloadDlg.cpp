//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DownloadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Download.h"
#include "Package.h"
#include "DownloadDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////
// CDownloadDlg dialog




CDownloadDlg::CDownloadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownloadDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_csEdit.Empty();
	m_bAbortAllowed = FALSE;
	m_bDelayAllowed = FALSE;
	m_uNotifyCountdown = 0;
}

void CDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_buttonOK);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_DELAY, m_buttonWait);
//	DDX_Control(pDX, IDC_EDIT1, m_editText);
//	DDX_Text( pDX, IDC_EDIT1, m_csEdit);
}

BEGIN_MESSAGE_MAP(CDownloadDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DELAY, OnDelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CDownloadDlg message handlers

BOOL CDownloadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if( m_uNotifyCountdown)
		SetTimer( WM_TIMER, 1000, NULL );	
	m_bDelayed = FALSE;
	m_uWaited = 0;	

/*	CRect cr, win;
	int nbl = m_editText.GetLineCount();
	if( nbl > MAX_LINES ) nbl = MAX_LINES;
	if( nbl < MIN_LINES ) nbl = MIN_LINES;

	m_editText.GetWindowRect(&cr);

	GetWindowRect(&win);

	long double ldTemp = nbl*cr.Height() / 3;
	int newHeight = int( ceil( ldTemp));
	m_editText.SetWindowPos( &CWnd::wndTop, 0, 0, cr.Width() , newHeight, SWP_NOMOVE );

	SetWindowPos( &CWnd::wndTopMost, 0, 0, win.Width() , newHeight + win.Height() - cr.Height() + 10, SWP_NOMOVE );

	OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);

	if (osvi.dwMajorVersion < 6)
	{
		// Windows 2000/XP/2003 => Display as plain text
*/		
		SetDlgItemText( IDC_EDIT1, m_csEdit);
/*	}
	else
	{
		// Windows Vista/2008/Seven or + => Display HTML message using SimpleBrowser
		if (m_Browser.CreateFromControl( this, IDC_EDIT1))
		{
			CString csMessage;
			csMessage.Format( _T( "<HTML><BODY><HEAD><TITLE>OCS Inventory NG Deployment Status text</TITLE></HEAD><P>%s</P></BODY></HTML>"),
								m_csEdit);
			m_Browser.Write( csMessage);
		}
	}
*/	// Bring window to front
	BringWindowToTop();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDownloadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDownloadDlg::OnPaint()
{
	CString csMessage;

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
		// Enable and/or center button
		m_buttonOK.SetFocus();
//		m_editText.SetSel(-1);
		CRect fen, ok, cancel, wait;
		CRect v_fen, v_ok, v_cancel, v_wait;
		int xok,xcancel,xwait,y;	
		// Load language dependant button message
/*		csMessage.LoadString( IDS_CANCEL_STRING);
		m_buttonCancel.SetWindowText( csMessage);
		csMessage.LoadString( IDS_DELAY_STRING);
		m_buttonWait.SetWindowText( csMessage);
*/
		GetClientRect(&fen);
		m_buttonOK.GetClientRect(&ok);
		m_buttonCancel.GetClientRect(&cancel);
		m_buttonWait.GetClientRect(&wait);

		GetWindowRect(&v_fen);
		m_buttonOK.GetWindowRect(&v_ok);
		m_buttonCancel.GetWindowRect(&v_cancel);
		m_buttonWait.GetWindowRect(&v_wait);

		m_buttonCancel.ShowWindow( m_bAbortAllowed );
		m_buttonWait.ShowWindow( m_bDelayAllowed );

		if( ! m_bAbortAllowed && ! m_bDelayAllowed ) { // OK only => center it
			xok = (fen.Width() - ok.Width() ) / 2;
			y = fen.bottom - 10 - ok.bottom ;
			m_buttonOK.SetWindowPos( &CWnd::wndTop, xok, y, 0, 0, SWP_NOSIZE );
		}
		else if( !m_bDelayAllowed ) {// ok and cancel => center them
			xok     = ((  fen.Width()/2) -     ok.Width() ) / 2;
			xcancel = (( (fen.Width()/2) - cancel.Width() ) / 2) + fen.Width()/2 ;
			y = fen.bottom - 10 - ok.bottom ;
			m_buttonOK.SetWindowPos( &CWnd::wndTop, xok, y, 0, 0, SWP_NOSIZE );
			m_buttonCancel.SetWindowPos( &CWnd::wndTop, xcancel, y, 0, 0, SWP_NOSIZE );
		}
		else if( ! m_bAbortAllowed ) { // ok and delay => center them
			xok     = ((  fen.Width()/2) -     ok.Width() ) / 2;
			xwait = (( (fen.Width()/2) - wait.Width() ) / 2) + fen.Width()/2 ;
			y = fen.bottom - 10 - ok.bottom ;
			m_buttonOK.SetWindowPos( &CWnd::wndTop, xok, y, 0, 0, SWP_NOSIZE );
			m_buttonWait.SetWindowPos( &CWnd::wndTop, xwait, y, 0, 0, SWP_NOSIZE );
		}
		else { // All 3 button
			y = fen.bottom - 10 - ok.bottom ;

			xok     = ((  fen.Width()/3) -     ok.Width() ) / 2;
			xcancel = (( (fen.Width()/3) - cancel.Width() ) / 2) + fen.Width()/3 ;
			xwait = (( (fen.Width()/3) - wait.Width() ) / 2) + (fen.Width()/3)*2 ;

			m_buttonOK.SetWindowPos( &CWnd::wndTop, xok , y, 0, 0, SWP_NOSIZE );
			m_buttonWait.SetWindowPos( &CWnd::wndTop, xcancel, y, 0, 0, SWP_NOSIZE );
			m_buttonCancel.SetWindowPos( &CWnd::wndTop, xwait, y, 0, 0, SWP_NOSIZE );
		}
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDownloadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDownloadDlg::OnTimer(UINT nIDEvent) 
{
	CString csMessage;

	m_uWaited++;
	if (m_uWaited >= m_uNotifyCountdown)
		OnOK();
	
	TCHAR szUseless[255];
	_itot_s( m_uNotifyCountdown - m_uWaited , szUseless, 255, 10);
	csMessage.Format( IDS_AUTO_VALIDATE_STRING, szUseless);
	SetWindowText( csMessage);
	SetDlgItemText( IDC_STATUS, csMessage);
	BringWindowToTop();
	CDialog::OnTimer( nIDEvent);
}

void CDownloadDlg::OnCancel() 
{
	if( m_bAbortAllowed)
		CDialog::OnCancel();
}

void CDownloadDlg::OnDelay() 
{
	m_bDelayed = TRUE;
	CDialog::OnOK();
}

void CDownloadDlg::setPackage( CPackage *pPack)
{
	m_bAbortAllowed = pPack->isAbortAllowed();
	m_bDelayAllowed = pPack->isDelayAllowed();
	m_uNotifyCountdown = pPack->getNotifyCountdown();
	m_csEdit = pPack->getNotifyText();
	m_uWaited = 0;
	m_bDelayed = FALSE;
}

BOOL CDownloadDlg::isDelayed()
{
	return m_bDelayed;
}
