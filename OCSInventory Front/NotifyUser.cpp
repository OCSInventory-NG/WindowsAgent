//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#include "NotifyUser.h"
#include <WtsApi32.h>
#include <Userenv.h>
#include "Config.h"
#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CNotifyUser::CNotifyUser(void)
{
	OSVERSIONINFO osVersion;

	if (GetVersionEx( &osVersion))
	{
		if ((osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
			(osVersion.dwMajorVersion > 5))
			// Windows Vista, 2008, Seven...
			m_bIsVistaOrHigher = TRUE;
		else
			// Windows 2000, XP or 2003
			m_bIsVistaOrHigher = FALSE;
	}
	else
		// Assuming it is a recent OS
		m_bIsVistaOrHigher = TRUE;
	// There is no session console
	m_hUserToken = NULL;
	// Default user notification timeout is infinite
	m_dwTimeOut = INFINITE;
}

CNotifyUser::~CNotifyUser(void)
{
	if (m_hUserToken != NULL)
	{
		CloseHandle( m_hUserToken);
		m_hUserToken = NULL;
	}
}

BOOL CNotifyUser::getActiveConsoleSessionID()
{
	PWTS_SESSION_INFO pSessionInfo = NULL;
	DWORD dwCount,
		  dwSessionID;

	// WTSQueryUserToken function, from Wtsapi32.dll
	typedef BOOL(WINAPI * pWTSQueryUserToken) (
        IN ULONG SessionId,
        OUT PHANDLE phToken);
    HINSTANCE			hDll = NULL;
    pWTSQueryUserToken	functionWTSQueryUserToken = NULL;

	dwSessionID = OCS_NO_WTS_SESSION_ID;
	if (m_hUserToken != NULL)
	{
		CloseHandle( m_hUserToken);
		m_hUserToken = NULL;
	}
	// Get list of session opened on server
	if (!WTSEnumerateSessions(	WTS_CURRENT_SERVER_HANDLE, // Server handle
								0,				// Reserved
								1,				// WTS version, must be 1
								&pSessionInfo,	// Array of WTS_SESSION_INFO
								&dwCount)		// Number of WTS_SESSION_INFO returned 
		|| (dwCount < 1))
	{
		getOcsLogger()->log(  LOG_PRIORITY_DEBUG, _T( "LOG_PRIORITY_ERROR => Failed to enumerate WTS sessions (%s) ?"), LookupError( GetLastError()));
		return FALSE;
	}
	// Search for active console session
	for (DWORD dwIndex=0; (dwIndex<dwCount) && (dwSessionID == OCS_NO_WTS_SESSION_ID); dwIndex++)
	{
		if (pSessionInfo[dwIndex].State == WTSActive)
			// This is the first active session, probably the console one, so save it
			dwSessionID = pSessionInfo[dwIndex].SessionId;
	}					
	WTSFreeMemory( pSessionInfo);
	// Retrieve the active session ID user token. This is the token that
	// will be used for creating the interactive UI process. 
    if ((hDll = LoadLibrary( _T( "Wtsapi32.dll"))) < (HINSTANCE) HINSTANCE_ERROR) 
	{
		// Cannot load WTS API => Windows 2000 ?
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to load Wtsapi32.dll (%s)"), LookupError( GetLastError()));
        return FALSE;
    }
    if ((functionWTSQueryUserToken = (pWTSQueryUserToken) GetProcAddress(hDll, "WTSQueryUserToken")) == NULL)
	{
		// Cannot load WTSQueryUserToken function => Windows 2000 ?
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to load WTSQueryUserToken function (%s)"), LookupError( GetLastError()));
		FreeLibrary( hDll);
        return FALSE;
    }
	if (!functionWTSQueryUserToken( dwSessionID, &m_hUserToken))
	{
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to call WTSQueryUserToken (%s)"), LookupError( GetLastError()));
		if (m_hUserToken != NULL)
			CloseHandle( m_hUserToken);
		m_hUserToken = NULL;
		FreeLibrary( hDll);
		return FALSE;
	}
	FreeLibrary( hDll);
	return TRUE;
}

UINT CNotifyUser::displayAsConnectedUser( CString &csCommand)
{
	STARTUPINFO			si = {0};
	PROCESS_INFORMATION pi = {0};
	HANDLE				hDuplicatedToken = NULL;
	LPVOID				lpEnvironment = NULL;
	DWORD				dwExitCode,
						dwResult;

	ASSERT( m_hUserToken);

	si.cb = sizeof(si);
	// Duplicate the token so that it can be used to create a process
	if (!DuplicateTokenEx( m_hUserToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDuplicatedToken))
	{
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to call DuplicateTokenEx (%s)"), LookupError( GetLastError()));
		if (hDuplicatedToken != NULL)
			CloseHandle( hDuplicatedToken);
		m_uResult = OCS_NOTIFY_APP_GENERIC_ERROR;
		return m_uResult;
	}
	// Create an environment block for the interactive process
	if (!CreateEnvironmentBlock( &lpEnvironment, hDuplicatedToken, FALSE))
	{
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to call CreateEnvironmentBlock (%s)"), LookupError( GetLastError()));
		if (lpEnvironment != NULL)
			DestroyEnvironmentBlock( lpEnvironment);
		if (hDuplicatedToken != NULL)
			CloseHandle( hDuplicatedToken);
		m_uResult = OCS_NOTIFY_APP_GENERIC_ERROR;
		return m_uResult;
	}
	// Create the process under the target user’s context
	if (!CreateProcessAsUser( hDuplicatedToken, NULL,
							csCommand.GetBuffer(), NULL, NULL, FALSE, 
							NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | 
							CREATE_UNICODE_ENVIRONMENT, lpEnvironment, 
							getInstallFolder(), &si, &pi))
	{
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to call CreateProcessAsUser (%s)"), LookupError( GetLastError()));
		csCommand.ReleaseBuffer();
		if (lpEnvironment != NULL)
			DestroyEnvironmentBlock( lpEnvironment);
		if (hDuplicatedToken != NULL)
			CloseHandle( hDuplicatedToken);
		m_uResult = OCS_NOTIFY_APP_GENERIC_ERROR;
		return m_uResult;
	}
	csCommand.ReleaseBuffer();
	CloseHandle( pi.hThread);
	// Wait for process to terminate
	if ((dwResult = WaitForSingleObject( pi.hProcess, m_dwTimeOut)) != WAIT_OBJECT_0)
	{
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to call WaitForSingleObject (%s)"), LookupError( GetLastError()));
		if (dwResult == WAIT_TIMEOUT)
			// Kill process
			TerminateProcess(  pi.hProcess, WAIT_TIMEOUT);
		CloseHandle( pi.hProcess);
		if (lpEnvironment != NULL)
			DestroyEnvironmentBlock( lpEnvironment);
		if (hDuplicatedToken != NULL)
			CloseHandle( hDuplicatedToken);
		m_uResult = OCS_NOTIFY_APP_ALREADY_RUNNING_ERROR;
		return m_uResult;
	}
	// Get process exit code
	if (GetExitCodeProcess( pi.hProcess, &dwExitCode)) 
		m_uResult = dwExitCode;
	else
	{
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to call GetExitCodeProcess (%s)"), LookupError( GetLastError()));
		CloseHandle( pi.hProcess);
		if (lpEnvironment != NULL)
			DestroyEnvironmentBlock( lpEnvironment);
		if (hDuplicatedToken != NULL)
			CloseHandle( hDuplicatedToken);
		m_uResult = OCS_NOTIFY_APP_GENERIC_ERROR;
		return m_uResult;
	}
	CloseHandle( pi.hProcess);
	if (lpEnvironment != NULL)
		DestroyEnvironmentBlock( lpEnvironment);
	if (hDuplicatedToken != NULL)
		CloseHandle( hDuplicatedToken);
	getOcsLogger()->log(  LOG_PRIORITY_DEBUG, _T( "NOTIFY_USER => OcsNotifyUser exit code is %u"), m_uResult);
	return m_uResult;
}

UINT CNotifyUser::displayAsDefaultUser( CString &csCommand)
{
	STARTUPINFO			si = {0};
	PROCESS_INFORMATION pi = {0};
	DWORD				dwExitCode,
						dwResult;

	si.cb = sizeof(si);
	// Create the process using the current user’s context
	if (CreateProcess( NULL, csCommand.GetBuffer(), 
						NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
						NULL, getInstallFolder(), &si, &pi) == FALSE)
	{
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to call CreateProcess (%s)"), LookupError( GetLastError()));
		csCommand.ReleaseBuffer();
		m_uResult = OCS_NOTIFY_APP_GENERIC_ERROR;
		return m_uResult;
	}
	csCommand.ReleaseBuffer();
	CloseHandle( pi.hThread);
	// Wait for process to terminate
	if ((dwResult = WaitForSingleObject( pi.hProcess, m_dwTimeOut)) != WAIT_OBJECT_0)
	{
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to call WaitForSingleObject (%s)"), LookupError( GetLastError()));
		if (dwResult == WAIT_TIMEOUT)
			// Kill process
			TerminateProcess(  pi.hProcess, WAIT_TIMEOUT);
		CloseHandle( pi.hProcess);
		m_uResult = OCS_NOTIFY_APP_ALREADY_RUNNING_ERROR;
		return m_uResult;
	}
	// Get process exit code
	if (GetExitCodeProcess( pi.hProcess, &dwExitCode)) 
		m_uResult = dwExitCode;
	else
	{
		getOcsLogger()->log(  LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to call GetExitCodeProcess (%s)"), LookupError( GetLastError()));
		CloseHandle( pi.hProcess);
		m_uResult = OCS_NOTIFY_APP_GENERIC_ERROR;
		return m_uResult;
	}
	CloseHandle( pi.hProcess);
	getOcsLogger()->log(  LOG_PRIORITY_DEBUG, _T( "NOTIFY_USER => OcsNotifyUser exit code is %u"), m_uResult);
	return m_uResult;
}

UINT CNotifyUser::display( CString &csCommand)
{
	// Retrieve active console session
	if (!getActiveConsoleSessionID())
	{
		// Error enumerating sessions
		getOcsLogger()->log(  LOG_PRIORITY_NOTICE, _T( "NOTIFY_USER => Assuming not running under System service account, but under connected user account, and setting user answer timeout to <%u> minutes"), USER_ANSWER_TIME_OUT);
		m_dwTimeOut = USER_ANSWER_TIME_OUT * 60 * 1000;
		if (m_hUserToken != NULL)
		{
			CloseHandle( m_hUserToken);
			m_hUserToken = NULL;
		}
	}
	if (m_hUserToken == NULL)
	{
		// No session console, but service can display even if nobody logged on
		// Or running under user account
		return displayAsDefaultUser( csCommand);
	}
	// User connected on session console
	return displayAsConnectedUser( csCommand);
}

UINT CNotifyUser::AskQuestion( LPCTSTR lpstrText)
{
	CString csCommand;

	ASSERT( lpstrText);

	// Generate command
	csCommand.Format( _T( "\"%s\\%s\" /MSG=\"%s\""), getInstallFolder(), OCS_NOTIFY_USER_COMMAND, lpstrText);
	if (getAgentConfig()->isDebugRequired() >= OCS_DEBUG_MODE_TRACE)
		// Enable debugging
		csCommand.Append( _T( " /DEBUG"));
	return display( csCommand);
}

UINT CNotifyUser::ShowInformation( LPCTSTR lpstrText)
{
	CString csCommand;

	ASSERT( lpstrText);

	// Generate command
	csCommand.Format( _T( "\"%s\\%s\" /MSGBOX /NOCANCEL /MSG=\"%s\""), getInstallFolder(), OCS_NOTIFY_USER_COMMAND, lpstrText);
	if (getAgentConfig()->isDebugRequired() >= OCS_DEBUG_MODE_TRACE)
		// Enable debugging
		csCommand.Append( _T( " /DEBUG"));
	return display( csCommand);
}

UINT CNotifyUser::ShowPreInstall( LPCTSTR lpstrText, BOOL bCancelAllowed, BOOL bDelayAllowed, UINT uTimeOut)
{
	CString csCommand;

	ASSERT( lpstrText);

	// Generate command
	csCommand.Format( _T( "\"%s\\%s\" /PREINSTALL"), getInstallFolder(), OCS_NOTIFY_USER_COMMAND);
	if (getAgentConfig()->isDebugRequired() >= OCS_DEBUG_MODE_TRACE)
		// Enable debugging
		csCommand.Append( _T( " /DEBUG"));
	// Is cancel denied
	if (!bCancelAllowed)
		csCommand.Append( _T( " /NOCANCEL"));
	// Is delay allowed
	if (bDelayAllowed)
		csCommand.Append( _T( " /DELAY"));
	// Is timeout set
	if (uTimeOut > 0)
		csCommand.AppendFormat( _T( " /TIMEOUT=%u"), uTimeOut);
	// Message to display
	csCommand.AppendFormat( _T( " /MSG=\"%s\""), lpstrText);
	return display( csCommand);
}

UINT CNotifyUser::ShowPostInstall( LPCTSTR lpstrText, BOOL bRebootRequired, BOOL bDelayAllowed, UINT uTimeOut)
{
	CString csCommand;

	ASSERT( lpstrText);

	// Generate command
	csCommand.Format( _T( "\"%s\\%s\" /POSTINSTALL"), getInstallFolder(), OCS_NOTIFY_USER_COMMAND);
	if (getAgentConfig()->isDebugRequired() >= OCS_DEBUG_MODE_TRACE)
		// Enable debugging
		csCommand.Append( _T( " /DEBUG"));
	// Is reboot required
	if (bRebootRequired)
		csCommand.Append( _T( " /REBOOT"));
	// Cancel is always denied for end notification
	csCommand.Append( _T( " /NOCANCEL"));
	// Is delay allowed
	if (bDelayAllowed)
		csCommand.Append( _T( " /DELAY"));
	// Is timeout set
	if (uTimeOut > 0)
		csCommand.AppendFormat( _T( " /TIMEOUT=%u"), uTimeOut);
	// Message to display
	csCommand.AppendFormat( _T( " /MSG=\"%s\""), lpstrText);
	return display( csCommand);
}

UINT CNotifyUser::AskTag( LPCTSTR lpstrText, CString &csTag)
{
	CString		csCommand, csFile;
	TCHAR		szPath[ _MAX_PATH+1];
	CStdioFile	cFile;

	ASSERT( lpstrText);

	if (!GetTempPath( _MAX_PATH, szPath))
	{
		m_uResult = OCS_NOTIFY_APP_GENERIC_ERROR;
		return m_uResult;
	}
	csFile.Format( _T( "%s\\OCS_ASK_TAG_TO_USER.txt"), szPath);
	// Generate command
	csCommand.Format( _T( "\"%s\\%s\" /ASKTAG /MSG=\"%s\" /FILE=\"%s\""), getInstallFolder(), OCS_NOTIFY_USER_COMMAND, lpstrText, csFile);
	if (getAgentConfig()->isDebugRequired() >= OCS_DEBUG_MODE_TRACE)
		// Enable debugging
		csCommand.Append( _T( " /DEBUG"));
	if (display( csCommand))
		// Cancel, delay or error
		return m_uResult;
	// Read file result content
	if (!cFile.Open( csFile, CFile::modeRead | CFile::typeText))
	{
		getOcsLogger()->log( LOG_PRIORITY_ERROR, _T( "NOTIFY_USER => Failed to read Tag value from file <%s>: %s"), csTag, LookupError( GetLastError())); 
		m_uResult = OCS_NOTIFY_APP_GENERIC_ERROR;
		return m_uResult;
	}
	cFile.ReadString( csTag);
	cFile.Close();
	cFile.Remove( csFile);
	getOcsLogger()->log( LOG_PRIORITY_DEBUG, _T( "NOTIFY_USER => Read Tag value <%s> from file <%s>"), csTag, csFile); 
	return m_uResult;
}

