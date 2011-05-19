//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// NTService.cpp : Implements a simple Windows Service
//

#include "stdafx.h"
#include "NTService.h"
#include "NTServiceMsg.h" // Event message ids

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// static variables
CNTService* CNTService::m_pThis = NULL;

CNTService::CNTService(LPCTSTR lpstrServiceName)
{
     // copy the address of the current object so we can access it from
    // the static member callback functions. 
    // WARNING: This limits the application to only one CNTService object. 
    m_pThis = this;
    
   // Set the default service name and version
    m_csServiceName = lpstrServiceName;
    m_hEventSource = NULL;

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_Status.dwCurrentState = SERVICE_STOPPED;
    m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_Status.dwWin32ExitCode = 0;
    m_Status.dwServiceSpecificExitCode = 0;
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    m_bIsRunning = FALSE;
}

CNTService::~CNTService()
{
    if (m_hEventSource) 
	{
        ::DeregisterEventSource( m_hEventSource);
    }
}

BOOL CNTService::ParseCommandLine(int argc, LPTSTR argv[])
{

	CString csMessage,
			csParam;

    // See if we have any command line args we recognise
	if (argc <= 1)
		return FALSE;

	csParam = argv[1];
	if ((csParam.CompareNoCase( _T( "-?")) == 0) || (csParam.CompareNoCase( _T( "-h")) == 0) ||
		(csParam.CompareNoCase( _T( "/?")) == 0) || (csParam.CompareNoCase( _T( "/h")) == 0))
	{
        // Spit out version info
		csMessage.Format( _T( "Service <%s> %s registered into Windows Service Control Manager.\n\n%s\n%s\n%s\n%s"),
				m_csServiceName, IsInstalled() ? _T( "is") : _T( "IS NOT"),
				_T( "You can use the following command line parameters :"),
				_T( "   [-?] or [/?] or [-h] or [/h] to show this help."),
				_T( "   [-install] or [/install] to register service autostart at system startup."),
				_T( "   [-uninstall] or [/uninstall]  or [-remove] or [/remove] to unregister service."));
		AfxMessageBox( csMessage, MB_OK|MB_ICONINFORMATION);
        return TRUE; // say we processed the argument
    } 
	if ((csParam.CompareNoCase( _T( "-install")) == 0) || (csParam.CompareNoCase( _T( "/install")) == 0))
	{
        // Request to install
        if (IsInstalled()) 
		{
			csMessage.Format( _T( "Service <%s> is already registered."), m_csServiceName);
			AfxMessageBox( csMessage, MB_OK|MB_ICONEXCLAMATION);
        } 
		else if (!Install())
		{
			csMessage.Format( _T( "Failed to register Service <%s> into Windows Service Manager."), m_csServiceName);
			AfxMessageBox( csMessage, MB_OK|MB_ICONSTOP);
        }
        return TRUE; // say we processed the argument
    } 
	if ((csParam.CompareNoCase( _T( "-uninstall")) == 0) || (csParam.CompareNoCase( _T( "/uninstall")) == 0) ||
		(csParam.CompareNoCase( _T( "-remove")) == 0) || (csParam.CompareNoCase( _T( "/remove")) == 0))
	{
        // Request to uninstall.
        if (IsInstalled()) 
		{
            // Try and remove the copy that's installed
            if (!Uninstall()) 
			{
				csMessage.Format( _T( "Failed to unregister Service <%s> from Windows Service Manager."), m_csServiceName);
				AfxMessageBox( csMessage, MB_OK|MB_ICONSTOP);
            }
        }
        return TRUE; // say we processed the argument
    }
    // Don't recognise the args
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////
// Install/uninstall routines

// Test if the service is currently installed
BOOL CNTService::IsInstalled()
{
    BOOL bResult = FALSE;

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     GENERIC_READ); // Read access only
    if (hSCM) 
	{
        // Try to open the service
        SC_HANDLE hService = ::OpenService(hSCM,
                                           m_csServiceName,
                                           SERVICE_QUERY_CONFIG);
        if (hService) 
		{
            bResult = TRUE;
            ::CloseServiceHandle( hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

BOOL CNTService::Install( LPCTSTR lpstrDescription, LPCTSTR lpstrDependancies)
{
    TCHAR szFilePath[_MAX_PATH+1];
	CString csKey;
	HKEY hKey;

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager( NULL, // local machine
                                      NULL, // ServicesActive database
                                      SC_MANAGER_ALL_ACCESS); // Full access
    if (!hSCM) 
		return FALSE;
    // Get the executable file path
    if (::GetModuleFileName( NULL, szFilePath, sizeof( szFilePath)) == 0)
		return FALSE;
    // Create the service
    SC_HANDLE hService = ::CreateService(hSCM,
                                         m_csServiceName,			// Service key name
                                         m_csServiceName,			// Service display name
                                         SERVICE_ALL_ACCESS,		// Desired access
                                         SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, // Runs in its own process with desktop interaction
                                         SERVICE_AUTO_START,        // Autostart at system startup
                                         SERVICE_ERROR_NORMAL,		// Normal error reporting
                                         szFilePath,				// Main binary
                                         NULL,						// Load order group
                                         NULL,						// Tag ID to order service start in group
                                         lpstrDependancies,			// Service dependancies
                                         NULL,						// Service runs under Local system account,
                                         NULL);						// so no user and password
    if (!hService) 
	{
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }
	// Set service description
	if (lpstrDescription != NULL)
	{
		SERVICE_DESCRIPTION sd;
		sd.lpDescription = (LPTSTR) lpstrDescription;
		if (!::ChangeServiceConfig2( hService, SERVICE_CONFIG_DESCRIPTION, &sd))
		{
			::CloseServiceHandle(hService);
			::CloseServiceHandle(hSCM);
			return FALSE;
		}
	}
    // Create registry entries for service parameters
	csKey.Format( _T( "%s\\%s\\%s"), HKEY_NT_SERVICE, m_csServiceName, HKEY_NT_SERVICE_PARAMETER);
    if (::RegCreateKey(HKEY_LOCAL_MACHINE, csKey, &hKey) != ERROR_SUCCESS) 
	{
        ::CloseServiceHandle(hService);
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }
	RegCloseKey( hKey);
    // Create registry entries to support logging messages
    // Add the source name as a subkey under the Application key in the EventLog service portion of the registry.
    csKey.Format( _T( "%s\\%s"), HKEY_NT_LOG_EVENT, m_csServiceName);
    if (::RegCreateKey( HKEY_LOCAL_MACHINE, csKey, &hKey) != ERROR_SUCCESS) 
	{
        ::CloseServiceHandle(hService);
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }
    // Add the Event ID message-file name to the 'EventMessageFile' subkey.
    ::RegSetValueEx( hKey, _T( "EventMessageFile"), 0, REG_SZ, (CONST BYTE*) szFilePath, (_tcslen( szFilePath)+1)*sizeof( TCHAR));     
    // Set the supported types flags.
    DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
    ::RegSetValueEx( hKey, _T( "TypesSupported"), 0, REG_DWORD, (CONST BYTE*)&dwData, sizeof(DWORD));
    ::RegCloseKey(hKey);
    LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED, m_csServiceName);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

BOOL CNTService::Uninstall()
{
	HKEY hKey;
    BOOL bResult = TRUE;

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager( NULL, // local machine
                                      NULL, // ServicesActive database
                                      SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) 
		return FALSE;
    SC_HANDLE hService = ::OpenService( hSCM, m_csServiceName, DELETE);
    if (!hService)
	{
	    ::CloseServiceHandle( hSCM);
		return FALSE;
	}
	// Stop service if started
	if (m_bIsRunning)
	{
		SERVICE_STATUS pStatus;

		if (!ControlService( hService, SERVICE_CONTROL_STOP, &pStatus))
		{
			// Service cannot be stopped
			LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_FAILED_STOP, m_csServiceName);
			bResult = FALSE;
			goto UNINSTALL_CLEAN;
		}
		else
		{
			// Wait 10 s
			Sleep(10000);
			LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED, m_csServiceName);
		}
	}
	// Unregister service
    if (::DeleteService(hService)) 
	{
        LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_REMOVED, m_csServiceName);
    } 
	else 
	{
		// Service cannot be unregistered
        LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILED_REMOVE, m_csServiceName);
		bResult = FALSE;
		goto UNINSTALL_CLEAN;
    }
    // Delete registry entries for logging message support
	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, HKEY_NT_LOG_EVENT, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		::RegDeleteKey( hKey, m_csServiceName);
		RegCloseKey( hKey);
	}

UNINSTALL_CLEAN:
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return bResult;
}

///////////////////////////////////////////////////////////////////////////////////////
// Logging functions

// This function makes an entry into the application event log
void CNTService::LogEvent(WORD wType, DWORD dwID,
                          LPCTSTR pszS1,
                          LPCTSTR pszS2,
                          LPCTSTR pszS3)
{
    LPCTSTR ps[3];
    ps[0] = pszS1;
    ps[1] = pszS2;
    ps[2] = pszS3;

    int iStr = 0;
    for (int i = 0; i < 3; i++) 
	{
        if (ps[i] != NULL) iStr++;
    }
    // Check the event source has been registered and if
    // not then register it now
    if (!m_hEventSource) 
	{
        m_hEventSource = ::RegisterEventSource(NULL,  // local machine
                                               m_csServiceName); // source name
    }
    if (m_hEventSource) 
	{
        ::ReportEvent(m_hEventSource,
                      wType,
                      0,
                      dwID,
                      NULL, // sid
                      iStr,
                      0,
                      ps,
                      NULL);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Service startup and registration

BOOL CNTService::StartService()
{
    SERVICE_TABLE_ENTRY st[] = 
	{
        {_T( ""), ServiceMain},
        {NULL, NULL}
    };
    BOOL b = ::StartServiceCtrlDispatcher(st);
    return b;
}

// static member function (callback)
void CNTService::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Get a pointer to the C++ object
    ASSERT( m_pThis);
    
    // Register the control request handler
    m_pThis->m_Status.dwCurrentState = SERVICE_START_PENDING;
    m_pThis->m_hServiceStatus = RegisterServiceCtrlHandler( m_pThis->m_csServiceName,
                                                           Handler);
    if (m_pThis->m_hServiceStatus == NULL) 
	{
        m_pThis->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILED_CTRLHANDLER, m_pThis->m_csServiceName);
        return;
    }
    // Start the initialisation
    if (m_pThis->Initialize()) 
	{
        // Do the real work. 
        // When the Run function returns, the service has stopped.
        m_pThis->m_bIsRunning = TRUE;
        m_pThis->m_Status.dwWin32ExitCode = 0;
        m_pThis->m_Status.dwCheckPoint = 0;
        m_pThis->m_Status.dwWaitHint = 0;
        m_pThis->Run();
    }
    // Tell the service manager we are stopped
    m_pThis->SetStatus(SERVICE_STOPPED);
}

///////////////////////////////////////////////////////////////////////////////////////////
// status functions

void CNTService::SetStatus(DWORD dwState)
{
    m_Status.dwCurrentState = dwState;
    ::SetServiceStatus( m_hServiceStatus, &m_Status);
}

///////////////////////////////////////////////////////////////////////////////////////////
// Service initialization

BOOL CNTService::Initialize()
{
    // Start the initialization
    SetStatus( SERVICE_START_PENDING);
    // Perform the actual initialization
    BOOL bResult = OnInit(); 
    // Set final state
    m_Status.dwWin32ExitCode = GetLastError();
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    if (!bResult) 
	{
        LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILED_INIT, m_csServiceName);
        SetStatus(SERVICE_STOPPED);
        return FALSE;    
    }
    LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STARTED, m_csServiceName);
    SetStatus(SERVICE_RUNNING);
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// main function to do the real work of the service

// This function performs the main work of the service. 
// When this function returns the service has stopped.
void CNTService::Run()
{
    while (IsRunning()) 
	{
        Sleep(5000);
    }
    // nothing more to do
}

//////////////////////////////////////////////////////////////////////////////////////
// Control request handlers

// static member function (callback) to handle commands from the
// service control manager
void CNTService::Handler(DWORD dwOpcode)
{
    switch (dwOpcode) 
	{
    case SERVICE_CONTROL_STOP: // 1
        m_pThis->SetStatus(SERVICE_STOP_PENDING);
        m_pThis->OnStop();
        m_pThis->m_bIsRunning = FALSE;
        break;
    case SERVICE_CONTROL_PAUSE: // 2
        m_pThis->OnPause();
        break;
    case SERVICE_CONTROL_CONTINUE: // 3
        m_pThis->OnContinue();
        break;
    case SERVICE_CONTROL_INTERROGATE: // 4
        m_pThis->OnInterrogate();
        break;
    case SERVICE_CONTROL_SHUTDOWN: // 5
        m_pThis->OnShutdown();
        break;
    default:
        if (dwOpcode >= SERVICE_CONTROL_USER) 
		{
            if (!m_pThis->OnUserControl(dwOpcode)) 
			{
                m_pThis->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BAD_REQUEST, m_pThis->m_csServiceName);
            }
        } 
		else 
		{
            m_pThis->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BAD_REQUEST, m_pThis->m_csServiceName);
        }
        break;
    }
    // Report current status
    ::SetServiceStatus( m_pThis->m_hServiceStatus, &(m_pThis->m_Status));
}
        
// Called when the service is first initialized
BOOL CNTService::OnInit()
{
	return TRUE;
}

// Called when the service control manager wants to stop the service
void CNTService::OnStop()
{
}

// called when the service is interrogated
void CNTService::OnInterrogate()
{
}

// called when the service is paused
void CNTService::OnPause()
{
}

// called when the service is continued
void CNTService::OnContinue()
{
}

// called when the service is shut down
void CNTService::OnShutdown()
{
}

// called when the service gets a user control message
BOOL CNTService::OnUserControl(DWORD dwOpcode)
{
    return FALSE; // say not handled
}

BOOL CNTService::IsRunning()
{
	return m_bIsRunning;
}

DWORD CNTService::GetWin32ExitCode()
{
	return m_Status.dwWin32ExitCode;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Debugging support

void CNTService::DebugMsg( LPCTSTR pszFormat, ...)
{
    CString csBuffer, csMessage;
	va_list arglist;
 
	va_start( arglist, pszFormat);
	csBuffer.FormatV( pszFormat, arglist);
	va_end(arglist);
    csBuffer.Format( _T( "[%s](%lu): %s\n"), m_csServiceName, GetCurrentThreadId(), csMessage);
    OutputDebugString( csBuffer);
}
