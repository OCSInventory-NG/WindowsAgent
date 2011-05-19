//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsSystrayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OcsSystray.h"
#include "OcsSystrayDlg.h"
#include "OCSInventory Front Classes.h"
#include "FileVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// COcsSystrayDlg dialog
COcsSystrayDlg::COcsSystrayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COcsSystrayDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COcsSystrayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WEB_URL, m_HLink);
}

BEGIN_MESSAGE_MAP(COcsSystrayDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND( IDC_SYSTRAY_DISPLAY, OnDisplay)
	ON_COMMAND( IDC_SYSTRAY_RUN_AGENT, OnRunAgent)
	ON_COMMAND( IDC_SYSTRAY_SHOW_INVENTORY, OnShowInventory)
END_MESSAGE_MAP()


// COcsSystrayDlg message handlers

BOOL COcsSystrayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Set link to OCS Web site
	m_HLink.SetLinkUrl( _T( "http://www.ocsinventory-ng.org"));
	// Set agent version
	CFileVersion fileVer;
	CString csMessage;

	// Service version
	csMessage.Format( _T( "%s\\ocsservice.exe"), getInstallFolder());
	if (fileVer.Open( csMessage))
	{
		csMessage.Format( _T( "%s version %s"), fileVer.GetFileDescription(), fileVer.GetFixedFileVersion());
		fileVer.Close();
	}
	else
		csMessage = _T( "Service version unknown");
	SetDlgItemText( IDC_VERSION_SERVICE, csMessage);
	// Agent version
	csMessage.Format( _T( "%s\\ocsinventory.exe"), getInstallFolder());
	if (fileVer.Open( csMessage))
	{
		csMessage.Format( _T( "%s version %s"), fileVer.GetFileDescription(), fileVer.GetFixedFileVersion());
		fileVer.Close();
	}
	else
		csMessage = _T( "Agent version unknown");
	SetDlgItemText( IDC_VERSION_AGENT, csMessage);
	// Download version
	csMessage.Format( _T( "%s\\download.exe"), getInstallFolder());
	if (fileVer.Open( csMessage))
	{
		csMessage.Format( _T( "%s version %s"), fileVer.GetFileDescription(), fileVer.GetFixedFileVersion());
		fileVer.Close();
	}
	else
		csMessage = _T( "Download and Setup Tool version unknown");
	SetDlgItemText( IDC_VERSION_DOWNLOAD, csMessage);
	// Framework version
	csMessage.Format( _T( "%s\\OCSInventory Front.dll"), getInstallFolder());
	if (fileVer.Open( csMessage))
	{
		csMessage.Format( _T( "%s version %s"), fileVer.GetFileDescription(), fileVer.GetFixedFileVersion());
		fileVer.Close();
	}
	else
		csMessage = _T( "Framework Functions version unknown");
	SetDlgItemText( IDC_VERSION_FRAMEWORK, csMessage);
	// Sysinfo version
	csMessage.Format( _T( "%s\\sysinfo.dll"), getInstallFolder());
	if (fileVer.Open( csMessage))
	{
		csMessage.Format( _T( "%s version %s"), fileVer.GetFileDescription(), fileVer.GetFixedFileVersion());
		fileVer.Close();
	}
	else
		csMessage = _T( "System Functions version unknown");
	SetDlgItemText( IDC_VERSION_SYSINFO, csMessage);
	// WMI version
	csMessage.Format( _T( "%s\\ocswmi.dll"), getInstallFolder());
	if (fileVer.Open( csMessage))
	{
		csMessage.Format( _T( "%s version %s"), fileVer.GetFileDescription(), fileVer.GetFixedFileVersion());
		fileVer.Close();
	}
	else
		csMessage = _T( "WMI Functions version unknown");
	SetDlgItemText( IDC_VERSION_WMI, csMessage);
	// Com Provider version
	csMessage.Format( _T( "%s\\%s"), getInstallFolder(), OCS_DEFAULT_PROVIDER);
	if (fileVer.Open( csMessage))
	{
		csMessage.Format( _T( "%s version %s"), fileVer.GetFileDescription(), fileVer.GetFixedFileVersion());
		fileVer.Close();
	}
	else
		csMessage = _T( "Communication Functions version unknown");
	SetDlgItemText( IDC_VERSION_COM_PROVIDER, csMessage);
	// Get service status
	IsServiceRunning( csMessage);
	SetDlgItemText( IDC_SERVICE_STATUS, csMessage);
	// Set tray icon
	csMessage.LoadString( AFX_IDS_APP_TITLE);
	m_iconTray.Create( NULL, WM_SYSTRAY_ICON_NOTIFY, csMessage, m_hIcon, IDR_MAINFRAME, FALSE);
	m_iconTray.SetCallbackMessage( IDC_SYSTRAY_DISPLAY);
	CSystemTray::MinimiseToTray( this);
	// Start timer to refresh service status
	SetTimer( ID_SYSTRAY_ICON, 5000, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COcsSystrayDlg::OnPaint()
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
HCURSOR COcsSystrayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COcsSystrayDlg::OnTimer( UINT nIDEvent)
{
	// Get service status
	CString csMessage;
	IsServiceRunning( csMessage);
	SetDlgItemText( IDC_SERVICE_STATUS, csMessage);
	CDialog::OnTimer( nIDEvent);
}

void COcsSystrayDlg::OnDisplay()
{
	CSystemTray::MaximiseFromTray( this);
	m_iconTray.HideIcon();
}

void COcsSystrayDlg::OnCancel()
{
	CSystemTray::MinimiseToTray( this);
	m_iconTray.ShowIcon();
}

void COcsSystrayDlg::OnRunAgent()
{
	CWaitCursor myCursor;

	if (ServiceSendMessage( OCS_SERVICE_CONTROL_RUN_INVENTORY_NOW))
		AfxMessageBox( _T( "Start inventory notification successfully sent to OCS Inventory NG Service"),
						MB_OK|MB_ICONINFORMATION);
}

void COcsSystrayDlg::OnShowInventory()
{
	CWaitCursor myCursor;
	CString csXml;
	CShowInventoryDlg cDlg;

	if (!ServiceSendMessage( OCS_SERVICE_CONTROL_SHOW_INVENTORY))
		return;
	// Now, XML must be generated, try to display it
	csXml.Format( _T( "%s\\OCSInventory.xml"), getDataFolder());
	if (!fileExists( csXml))
	{
		AfxMessageBox( _T( "Cannot display inventory informations !\n\nService did not generate XML file."), MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	cDlg.SetInventory( csXml);
	if (cDlg.DoModal() != IDCANCEL)
	{
		AfxMessageBox( _T( "Cannot display inventory informations !"), MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	DeleteFile( csXml);
}

BOOL COcsSystrayDlg::IsServiceRunning( CString &csStatusText)
{
    SERVICE_STATUS ss;
	SC_HANDLE hSCM, hService;

	// open the service control manager
    hSCM = OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, GENERIC_READ);
    if(!hSCM)
 	{
		csStatusText.Format( IDS_FAILED_CONNECT_SERVICE, _T( "Service Control Manager"), LookupError( GetLastError()));
		return FALSE;
	}
    // open the service
    hService = OpenService( hSCM, OCS_SERVICE_SECTION, GENERIC_READ);
    if(!hService)
	{
		csStatusText.Format( IDS_FAILED_CONNECT_SERVICE, OCS_SERVICE_SECTION, LookupError( GetLastError()));
		CloseServiceHandle( hSCM);
		return FALSE;
	}
    // Get the current status
    memset( &ss, 0, sizeof( ss));
    if (!QueryServiceStatus( hService, &ss))
	{
		csStatusText.Format( IDS_FAILED_CONNECT_SERVICE, OCS_SERVICE_SECTION, LookupError( GetLastError()));
		CloseServiceHandle( hService);
		CloseServiceHandle( hSCM);
		return FALSE;
	}
    // close the service handle
    CloseServiceHandle( hService);
    // close the service control manager handle
    CloseServiceHandle( hSCM);
    // See what we got
	switch (ss.dwCurrentState) 
	{
    case SERVICE_START_PENDING:
    case SERVICE_CONTINUE_PENDING:
		csStatusText.Format( IDS_SERVICE_STARTING, OCS_SERVICE_SECTION);
        break;
    case SERVICE_RUNNING:
		csStatusText.Format( IDS_SERVICE_STARTED, OCS_SERVICE_SECTION);
        break;
    case SERVICE_STOP_PENDING:
    case SERVICE_PAUSE_PENDING:
		csStatusText.Format( IDS_SERVICE_STOPPING, OCS_SERVICE_SECTION);
        break;
    case SERVICE_STOPPED:
    case SERVICE_PAUSED:
		csStatusText.Format( IDS_SERVICE_STOPPED, OCS_SERVICE_SECTION);
        break;
	default:
 		csStatusText.Format( IDS_FAILED_CONNECT_SERVICE, OCS_SERVICE_SECTION, LookupError( GetLastError()));
		break;
   }
	return TRUE;
}

BOOL COcsSystrayDlg::ServiceSendMessage( DWORD dwMsg)
{
    SERVICE_STATUS ss;
	SC_HANDLE hSCM, hService;
	CString csMessage;

	// open the service control manager
    hSCM = OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE, GENERIC_READ);
    if(!hSCM)
 	{
		csMessage.Format( IDS_FAILED_CONNECT_SERVICE, _T( "Service Control Manager"), LookupError( GetLastError()));
		AfxMessageBox( csMessage, MB_ICONSTOP|MB_OK);
		return FALSE;
	}
    // open the service
    hService = OpenService( hSCM, OCS_SERVICE_SECTION, GENERIC_READ|SERVICE_USER_DEFINED_CONTROL);
    if(!hService)
	{
		csMessage.Format( IDS_FAILED_CONNECT_SERVICE, OCS_SERVICE_SECTION, LookupError( GetLastError()));
		AfxMessageBox( csMessage, MB_ICONSTOP|MB_OK);
		CloseServiceHandle( hSCM);
		return FALSE;
	}
    // Send the service control message
    memset( &ss, 0, sizeof( ss));
    if (!ControlService( hService, dwMsg, &ss))
	{
		csMessage.Format( IDS_FAILED_SEND_MESSAGE, OCS_SERVICE_SECTION, LookupError( GetLastError()));
		AfxMessageBox( csMessage, MB_ICONSTOP|MB_OK);
		CloseServiceHandle( hService);
		CloseServiceHandle( hSCM);
		return FALSE;
	}
    // close the service handle
    CloseServiceHandle( hService);
    // close the service control manager handle
    CloseServiceHandle( hSCM);
	return TRUE;
}
