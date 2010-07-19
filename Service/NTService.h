//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// NTService.h : A simple Windows Service
//

#ifndef _NTSERVICE_H_INCLUDED_
#define _NTSERVICE_H_INCLUDED_

#pragma once

#include <winsvc.h>
#include "resource.h"

// Registry under HKEY_LOCAL_MACHINE where service is registered
#define HKEY_NT_SERVICE			_T( "SYSTEM\\CurrentControlSet\\Services")
// Registry Subkey for service parameters
#define HKEY_NT_SERVICE_PARAMETER _T( "Parameters")
// Registry Subkey for service status
#define HKEY_NT_SERVICE_STATUS	_T( "Status")
// Registry key under HKEY_LOCAL_MACHINE where service event log is registered
#define HKEY_NT_LOG_EVENT		_T( "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application")

// First service control message id
#define SERVICE_CONTROL_USER	128

class CNTService
{
public:
	// Service constructor with service key and display names
    CNTService(LPCTSTR lpstrServiceName);
	// Destructor
	virtual ~CNTService();

	// Log event into Windows Application event log. Args are
	//	- Event type (EVENTLOG_INFORMATION_TYPE, EVENTLOG_WARNING_TYPE, EVENTLOG_ERROR_TYPE)
	//	- Event ID
	//	- Parameter strings
    void LogEvent(WORD wType, DWORD dwID, 
				  LPCTSTR pszS1 = NULL,
                  LPCTSTR pszS2 = NULL,
                  LPCTSTR pszS3 = NULL);
	// Initialize service (Do not overload, called by Service Manager)
    BOOL Initialize();
	// Start service (Do not overload, called by Service Manager)
    BOOL StartService();
	// Main code of service (to overload)
    virtual void Run();
	// Overload to process specific before service start
	virtual BOOL OnInit();
	// Overload to process specific actions before service stop 
    virtual void OnStop();
	// Overload to process service status
    virtual void OnInterrogate();
	// Overload to process specific actions before service pause 
	virtual void OnPause();
	// Overload to process specific actions before service resume 
    virtual void OnContinue();
	// Overload to process specific actions before system shutdown 
    virtual void OnShutdown();
	// Overload to process specific service control messages
    virtual BOOL OnUserControl(DWORD dwOpcode);
	// Is service running
	BOOL IsRunning();
	// Return code of specific service control message
	DWORD GetWin32ExitCode();
	// Parse command line args (at least, parse standard args 
	// to install/remove service or show version)
    virtual BOOL ParseCommandLine( int argc, LPTSTR argv[]);

   
    // static member functions
    static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI Handler(DWORD dwOpcode);
	// Debugging support
    void DebugMsg( LPCTSTR pszFormat, ...);

protected:
	// Is service already regsitered
    BOOL IsInstalled();
	// Register service into Windows Service Manager
    BOOL Install( LPCTSTR lpstrDescription = NULL, LPCTSTR lpstrDependancies = NULL);
	// Unregister service into Windows Service Manager
    BOOL Uninstall();
	// Set service status
    void SetStatus(DWORD dwState);

    // data members
    HANDLE	m_hEventSource;		// Event log handle
    CString m_csServiceName;	// Service name
    SERVICE_STATUS_HANDLE m_hServiceStatus;	// Service status handle
    SERVICE_STATUS m_Status;	// Service status
    BOOL m_bIsRunning;			// Is service running
    // static data
    static CNTService* m_pThis; // nasty hack to get object into callback function
};

#endif // _NTSERVICE_H_INCLUDED
