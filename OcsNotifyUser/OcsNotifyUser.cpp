
// OcsNotifyUser.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OcsNotifyUser.h"
#include "OCSInventory Front.h"
#include "NotifyUser.h"
#include "DownloadDlg.h"

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
		{
			m_nExitCode = OCS_NOTIFY_APP_GENERIC_ERROR;
#ifdef _DEBUG
			AfxMessageBox( _T( "Failed to initialize!"), MB_OK|MB_ICONSTOP);
#else
			_tprintf( _T( "Failed to initialize!")); 
#endif 
			return FALSE; // terminates the application
		}

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
#ifdef _DEBUG
			AfxMessageBox( _T( "Already running!"), MB_OK|MB_ICONSTOP);
#else
			_tprintf( _T( "Already running!")); 
#endif 
			return FALSE; // terminates the application
		}

		/*****
		 *
		 *	Main initinstance block 
		 *
		 ****/		
		AfxEnableControlContainer();

		CDownloadDlg cDlg;

		/*****
		*
		* Parse command line
		*
		****/
		if (!parseCommandLine())
		{
			m_nExitCode = OCS_NOTIFY_APP_GENERIC_ERROR;
#ifdef _DEBUG
			AfxMessageBox( _T( "Failed to parse command line!"), MB_OK|MB_ICONSTOP);
#else
			_tprintf( _T( "Failed to parse command line!")); 
#endif 
			goto CLEAN_AND_EXIT;
		}

		switch (m_uNotifcation)
		{
		case NOTIFY_TYPE_PREINSTALL:
			// Display preinstallation dialogbox
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
				m_nExitCode = OCS_NOTIFY_APP_GENERIC_ERROR;
#ifdef _DEBUG
				AfxMessageBox( _T( "Failed to display preinstall dialogbox!"), MB_OK|MB_ICONSTOP);
#else
				_tprintf( _T( "Failed to display preinstall dialogbox!")); 
#endif 
				return FALSE;
			}
			break;
		case NOTIFY_TYPE_MSGBOX:
			// Display standard messagebox
			if (m_bCancel)
			{
				// Cancel is allowed
				if (AfxMessageBox( m_csMessage, MB_OKCANCEL|MB_ICONQUESTION|MB_SYSTEMMODAL) == IDCANCEL)
					m_nExitCode = OCS_NOTIFY_APP_OK;
				else
					m_nExitCode = OCS_NOTIFY_APP_CANCEL;
			}
			else
			{
				// Cancel not allowed
				AfxMessageBox( m_csMessage, MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
				m_nExitCode = OCS_NOTIFY_APP_OK;
			}
			break;
		default:
			m_nExitCode = OCS_NOTIFY_APP_GENERIC_ERROR;
#ifdef _DEBUG
				AfxMessageBox( _T( "Wrong notification type!"), MB_OK|MB_ICONSTOP);
#else
				_tprintf( _T( "Wrong notification type!")); 
#endif 
			goto CLEAN_AND_EXIT;
		}

CLEAN_AND_EXIT:
		m_csMessage.Format( _T( "Exit code is %d"), m_nExitCode);
#ifdef _DEBUG
		AfxMessageBox( m_csMessage, MB_OK|MB_ICONSTOP);
#else
		_tprintf( m_csMessage); 
#endif 
	}
	catch( CException *pEx)
	{
#ifdef _DEBUG
		AfxMessageBox( LookupError( pEx), MB_OK|MB_ICONSTOP);
#else
		_tprintf( LookupError( pEx)); 
#endif 
		pEx->Delete();
		m_nExitCode = OCS_NOTIFY_APP_GENERIC_ERROR;
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
	if (CWinApp::ExitInstance() != 0)
		m_nExitCode = OCS_NOTIFY_APP_GENERIC_ERROR;
	return m_nExitCode;
}

BOOL COcsNotifyUserApp::parseCommandLine()
{
	CString csTimeOut;

	// /PREINSTALL
	if (isRequired( m_lpCmdLine, _T( "preinstall")))
		// Show preinstall dialog box
		m_uNotifcation = NOTIFY_TYPE_PREINSTALL;
	else
		// Show default messagebox
		m_uNotifcation = NOTIFY_TYPE_MSGBOX;
	// /MSG=message (mandatory)
	m_csMessage = getParamValue( m_lpCmdLine, _T( "msg"));
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
