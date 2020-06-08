//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DownloadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OcsNotifyUser.h"
#include "DownloadDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////
// CDownloadDlg dialog

CDownloadDlg::CDownloadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownloadDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_csEdit.Empty();
	m_bAbortAllowed = FALSE;
	m_bDelayAllowed = FALSE;
	m_bReboot = FALSE;
	m_uNotifyCountdown = 0;
}

void CDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_buttonOK);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_DELAY, m_buttonWait);
}

BEGIN_MESSAGE_MAP(CDownloadDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DELAY, OnDelay)
END_MESSAGE_MAP()


// CDownloadDlg message handlers

BOOL CDownloadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString csMessage;

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if( m_uNotifyCountdown)
		SetTimer( WM_TIMER, 1000, NULL );	
	m_bDelayed = FALSE;
	m_uWaited = 0;	

	OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);

	if ((osvi.dwMajorVersion < 5) ||
		((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0)))
	{
		// Windows 2000 or previous => Display as plain text
		SetDlgItemText( IDC_EDIT1, m_csEdit);
	}
	else
	{
		// Windows XP/2003/Vista/2008/Seven or + => Display HTML message using SimpleBrowser
		if (m_Browser.CreateFromControl( this, IDC_EDIT1))
		{
			csMessage.Format( _T( "<HTML><BODY><HEAD><TITLE>OCS Inventory Deployment Status text</TITLE></HEAD><P>%s</P></BODY></HTML>"),
								m_csEdit);
			m_Browser.Write( csMessage);
		}
	}
	// Set window title if reboot required
	if (m_bReboot)
	{
		csMessage.Format( IDS_REBOOT_REQUIRED_STRING);
		SetDlgItemText( IDC_STATUS, csMessage);
		csMessage.Insert( 0, _T( "OCS Inventory - "));
		SetWindowText( csMessage);
	}
	// Bring window to front
	BringWindowToTop();
	return TRUE;  // return TRUE  unless you set the focus to a control
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
		CRect fen, ok, cancel, wait;
		CRect v_fen, v_ok, v_cancel, v_wait;
		int xok,xcancel,xwait,y;	
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

void CDownloadDlg::OnTimer(UINT_PTR nIDEvent) 
{
	CString csMessage;

	m_uWaited++;
	if (m_uWaited >= m_uNotifyCountdown)
		OnOK();
	
	TCHAR szUseless[255];
	_itot_s( m_uNotifyCountdown - m_uWaited , szUseless, 255, 10);
	if (m_bReboot)
		csMessage.Format( IDS_AUTO_REBOOT_STRING, szUseless);
	else
		csMessage.Format( IDS_AUTO_VALIDATE_STRING, szUseless);
	SetDlgItemText( IDC_STATUS, csMessage);
	csMessage.Insert( 0, _T( "OCS Inventory - "));
	SetWindowText( csMessage);
	BringWindowToTop();
	SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
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

void CDownloadDlg::setAbortAllowed( BOOL bAbort)
{
	m_bAbortAllowed = bAbort;
}

void CDownloadDlg::setDelayAllowed( BOOL bDelay)
{
	m_bDelayAllowed = bDelay;
	m_bDelayed = FALSE;
}

void CDownloadDlg::setRebootRequired( BOOL bReboot)
{
	m_bReboot = bReboot;
}

void CDownloadDlg::setTimeOut( UINT uTimeOut)
{
	m_uNotifyCountdown = uTimeOut;
	m_uWaited = 0;
}

void CDownloadDlg::setNotification( LPCTSTR lpstrText)
{
	m_csEdit = lpstrText;
}

BOOL CDownloadDlg::isDelayed()
{
	return m_bDelayed;
}
