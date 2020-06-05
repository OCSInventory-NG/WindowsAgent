//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================


// OcsNotifyUser.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OcsNotifyUser.h"
#include "OCSInventory Front.h"
#include "NotifyUser.h"
#include "DownloadDlg.h"
#include "TagInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COcsNotifyUserApp

BEGIN_MESSAGE_MAP(COcsNotifyUserApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// COcsNotifyUserApp construction

COcsNotifyUserApp::COcsNotifyUserApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_nExitCode = OCS_NOTIFY_APP_GENERIC_ERROR;
	m_pLogger = NULL;
}


// The one and only COcsNotifyUserApp object

COcsNotifyUserApp theApp;


// COcsNotifyUserApp initialization

BOOL COcsNotifyUserApp::InitInstance()
{
	HANDLE hMutexOneInstance = NULL;

	try
	{
		// InitCommonControlsEx() is required on Windows XP if an application
		// manifest specifies use of ComCtl32.dll version 6 or later to enable
		// visual styles.  Otherwise, any window creation will fail.
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(InitCtrls);
		// Set this to include all the common control classes you want to use
		// in your application.
		InitCtrls.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		if (!CWinApp::InitInstance())
			return FALSE; // terminates the application

		// Logger
		CTime	cStartTime;		// Start time of the inventory check
		cStartTime = CTime::GetCurrentTime();
		m_pLogger			= getOcsLogger();
		m_pLogger->setApplication( AfxGetAppName());

		/*****
		*
		* Parse command line
		*
		****/
		if (!parseCommandLine())
			return FALSE;

		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "=============================================================================="));
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "Starting OCS Inventory NG User Notification Tool on %s."), cStartTime.Format( _T( "%#c")));

		/*****
		 *
		 *	Checks wether another instance of ocsinventory.exe is 
		 *	already running.
		 *
		 ****/

		hMutexOneInstance = ::CreateMutex( NULL, TRUE, _T("OCSNOTIFYUSER-088FA840-B10D-11D3-BC36-006067709674"));
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			m_nExitCode = OCS_NOTIFY_APP_ALREADY_RUNNING_ERROR;
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T("Notification Tool => already running"));
			return FALSE; // terminates the application
		}

		/*****
		 *
		 *	Gets Download version
		 *
		 ****/
		CString csMessage = getVersion();
		if (csMessage == _T( ""))
		{
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T("Notification Tool => Failed to retrieve Notification Tool version from file. Version 0.0.0.1 assumed"));
			csMessage=_T( "0.0.0.1");
		}
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T("Notification Tool => Running OCS Inventory NG Notification Tool Version %s"), csMessage);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T("Notification Tool => Using OCS Inventory NG FrameWork Version %s"), getFrameworkVersion());

		/*****
		 *
		 *	Main initinstance block 
		 *
		 ****/		
		AfxEnableControlContainer();

		switch (m_uNotifcation)
		{
		case NOTIFY_TYPE_PREINSTALL:
			// Display preinstallation dialogbox
			if (!displayPreinstallDialogBox())
				m_pLogger->log( LOG_PRIORITY_DEBUG, _T("Notification Tool => failed to display preinstall DialogBox")); 
			break;
		case NOTIFY_TYPE_POSTINSTALL:
			// Display postinstallation dialogbox
			if (!displayPostinstallDialogBox())
				m_pLogger->log( LOG_PRIORITY_DEBUG, _T("Notification Tool => failed to display postinstall DialogBox")); 
			break;
		case NOTIFY_TYPE_MSGBOX:
			// Display standard messagebox
			if (!displayMessageBox())
				m_pLogger->log( LOG_PRIORITY_DEBUG, _T("Notification Tool => failed to display standard MessageBox!")); 
			break;
		case NOTIFY_TYPE_ASKTAG:
			// Display standard messagebox
			if (!askTagDialogBox())
				m_pLogger->log( LOG_PRIORITY_DEBUG, _T("Notification Tool => failed to display AskTag DialogBox!")); 
			break;
		default:
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T("Notification Tool => wrong notification type!")); 
			break;
		}
	}
	catch( CException *pEx)
	{
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "Notification Tool => %s"), LookupError( pEx)); 
		pEx->Delete();
	}	
	
	/*****
	 *
	 *	Releasing mutex and terminate
	 *
	 ****/
	if (hMutexOneInstance != NULL)
	{
        ::ReleaseMutex( hMutexOneInstance);
    }
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int COcsNotifyUserApp::ExitInstance() 
{
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "Notification Tool => Exit code is %d"), m_nExitCode);
	CWinApp::ExitInstance();
	return m_nExitCode;
}

CString COcsNotifyUserApp::getVersion()
{
	CFileVersion fileVer;
	CString myVer;

	// Get application path	
	if (GetModuleFileName( AfxGetInstanceHandle(), myVer.GetBuffer( 4*_MAX_PATH+1), 4*_MAX_PATH) == 0)
		return _T( "");
	myVer.ReleaseBuffer();
	// Open application file to get version from file
	if (fileVer.Open( myVer))
	{
		myVer = fileVer.GetProductVersion();
		myVer.Remove( ' ');
		myVer.Replace( _T( ","), _T( "."));
		fileVer.Close();
	}
	else
		myVer.Empty();
	return myVer;
}

BOOL COcsNotifyUserApp::parseCommandLine()
{
	CString csTimeOut;

	// /DEBUG 
	if (isRequired( m_lpCmdLine, _T( "debug")))
		m_pLogger->setLogLevel( LOG_PRIORITY_DEBUG);
	// Notification type
	if (isRequired( m_lpCmdLine, _T( "preinstall")))
	{
		// Show preinstall dialog box /PREINSTALL
		m_uNotifcation = NOTIFY_TYPE_PREINSTALL;
	}
	else if (isRequired( m_lpCmdLine, _T( "asktag")))
	{
		// Show askTag dialog box /ASKTAG
		m_uNotifcation = NOTIFY_TYPE_ASKTAG;
		// Ensure /FILE is provided
		if (isRequired( m_lpCmdLine, _T( "file")))
			m_csFile = getParamValue( m_lpCmdLine, _T( "file"));
		else
			return FALSE;
	}
	else if (isRequired( m_lpCmdLine, _T( "postinstall")))
	{
		// Show postinstall dialog box /POSTINSTALL
		m_uNotifcation = NOTIFY_TYPE_POSTINSTALL;
	}
	else
	{
		// Show default messagebox /MSGBOX
		m_uNotifcation = NOTIFY_TYPE_MSGBOX;
	}
	// /MSG=message (mandatory)
	if (isRequired( m_lpCmdLine, _T( "msg")))
		m_csMessage = getParamValue( m_lpCmdLine, _T( "msg"));
	else
		return FALSE;
	// /NOCANCEL 
	if (isRequired( m_lpCmdLine, _T( "nocancel")))
		m_bCancel = FALSE;
	else
		m_bCancel = TRUE;
	// /DELAY
	if (isRequired( m_lpCmdLine, _T( "delay")))
		m_bDelay = TRUE;
	else
		m_bDelay = FALSE;
	// /REBOOT
	if (isRequired( m_lpCmdLine, _T( "reboot")))
		m_bReboot = TRUE;
	else
		m_bReboot = FALSE;
	// /TIMEOUT[=seconds]
	if (isRequired( m_lpCmdLine, _T( "timeout")))
		csTimeOut = getParamValue( m_lpCmdLine, _T( "timeout"));
	else 
		csTimeOut.Empty();
	if (csTimeOut.IsEmpty())
		m_uTimeOut = 0;
	else
		m_uTimeOut = _ttol( csTimeOut);
	return TRUE;
}

BOOL COcsNotifyUserApp::displayPreinstallDialogBox()
{
	CDownloadDlg cDlg;

	cDlg.setAbortAllowed( m_bCancel);
	cDlg.setDelayAllowed( m_bDelay);
	cDlg.setTimeOut( m_uTimeOut);
	cDlg.setNotification( m_csMessage);
	switch (cDlg.DoModal())
	{
	case IDCANCEL:
		// Canceled by user
		m_nExitCode = OCS_NOTIFY_APP_CANCEL;
		break;
	case IDOK:
		if (cDlg.isDelayed())
			m_nExitCode = OCS_NOTIFY_APP_DELAY;
		else
			m_nExitCode = OCS_NOTIFY_APP_OK;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


BOOL COcsNotifyUserApp::displayPostinstallDialogBox()
{
	CDownloadDlg cDlg;

	cDlg.setAbortAllowed( FALSE);
	cDlg.setDelayAllowed( m_bDelay);
	cDlg.setRebootRequired( m_bReboot);
	cDlg.setTimeOut(30);
	cDlg.setNotification( m_csMessage);
	switch (cDlg.DoModal())
	{
	case IDCANCEL:
		// Canceled by user
		m_nExitCode = OCS_NOTIFY_APP_CANCEL;
		break;
	case IDOK:
		if (cDlg.isDelayed())
			m_nExitCode = OCS_NOTIFY_APP_DELAY;
		else
			m_nExitCode = OCS_NOTIFY_APP_OK;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL COcsNotifyUserApp::displayMessageBox()
{
	if (m_bCancel)
	{
		// Cancel is allowed, so asking question
		switch (AfxMessageBox( m_csMessage, MB_OKCANCEL|MB_ICONQUESTION|MB_SYSTEMMODAL))
		{
		case IDOK:
			m_nExitCode = OCS_NOTIFY_APP_OK;
			break;
		case IDCANCEL:
			m_nExitCode = OCS_NOTIFY_APP_CANCEL;
			break;
		default:
			return FALSE;
		}
	}
	else
	{
		// Cancel not allowed, only display information message
		if (AfxMessageBox( m_csMessage, MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL) == IDOK)
			m_nExitCode = OCS_NOTIFY_APP_OK;
		else
			return FALSE;
	}
	return TRUE;
}


BOOL COcsNotifyUserApp::askTagDialogBox()
{
	CTagInputDlg cDlg;
	CStdioFile	 cFile;

	cDlg.setLabelText( m_csMessage);
	if (cDlg.DoModal() == IDOK)
		m_nExitCode = OCS_NOTIFY_APP_OK;
	else
		return FALSE;
	// Store result into file
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "Notification Tool => Writing Tag value <%s> to file <%s>"), cDlg.getTagValue(), m_csFile); 
	if (!cFile.Open( m_csFile, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		return FALSE;
	cFile.WriteString( cDlg.getTagValue());
	cFile.Close();
	return TRUE;
}
