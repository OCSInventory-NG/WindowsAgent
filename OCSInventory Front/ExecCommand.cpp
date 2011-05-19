//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ExecCommand.cpp: implementation for the CExecCommand class.
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ExecCommand.h"
#include "OcsUtils.h"
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <Tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if ( _MSC_VER >= 1300 )
// Visual studio .NET or higher
#include <stddef.h>
#define TO_INTPTR(x) ((intptr_t)(x))
#else // Visual Studio 6
#define TO_INTPTR(x) ((long)(x))
#endif

// Utility class describing a process running in memory 
class CProcessProps : public CObject
{
public:
	CProcessProps();
	~CProcessProps();

	void set( DWORD dwProcessID, DWORD dwParentID, LPCTSTR lpstrImagePath);
	void setRunning( BOOL bRunning = TRUE);

	DWORD getProcessID();
	DWORD getParentID();
	LPCTSTR getImagePath();
	BOOL isRunning();

protected:
	DWORD m_dwProcessID;
	DWORD m_dwParentID;
	CString m_csImagePath;
	BOOL	m_bRunning;
};


CProcessProps::CProcessProps()
{
	m_dwProcessID = -1;
	m_dwParentID = -1;
	m_csImagePath.Empty();
	m_bRunning = FALSE;
}

CProcessProps::~CProcessProps()
{
}

void CProcessProps::set( DWORD dwProcessID, DWORD dwParentID, LPCTSTR lpstrImagePath)
{
	m_dwProcessID = dwProcessID;
	m_dwParentID = dwParentID;
	m_csImagePath = lpstrImagePath;
	m_bRunning = TRUE;
}

void CProcessProps::setRunning( BOOL bRunning)
{
	m_bRunning = bRunning;
}

DWORD CProcessProps::getProcessID()
{
	return m_dwProcessID;
}

DWORD CProcessProps::getParentID()
{
	return m_dwParentID;
}

LPCTSTR CProcessProps::getImagePath()
{
	return m_csImagePath;
}

BOOL CProcessProps::isRunning()
{
	return m_bRunning;
}

CExecCommand::CExecCommand()
{
    initialize();
}

CExecCommand::~CExecCommand()
{
	closeHandles();
}

void CExecCommand::initialize()
{
	m_hProcessHandle = NULL;
	m_hChildStdinRd = NULL;
	m_hChildStdinWr = NULL;
	m_hChildStdoutRd = NULL;
	m_hChildStdoutWr = NULL;
	m_hChildStderrRd = NULL;
	m_hChildStderrWr = NULL;
	m_hChildStdinWrDup = NULL;
	m_hChildStdoutRdDup = NULL;
	m_hChildStderrRdDup = NULL;
    m_nExitValue    = -1;
    m_fdStdIn		= -1;
    m_fdStdOut		= -1;
    m_fdStdErr		= -1;
	m_csOutput.Empty();
	m_dwTimeout = INFINITE;
	m_bComspec = TRUE;
}

void CExecCommand::setTimeout( DWORD dwTimeout)
{
	m_dwTimeout = dwTimeout;
}

void CExecCommand::useComspec( BOOL bUse)
{
	m_bComspec = bUse;
}

LPCSTR CExecCommand::getOutput()
{
	return m_csOutput;
}

int CExecCommand::getExitValue()
{ 
	return m_nExitValue;
}

BOOL CExecCommand::closeHandles()
{
	BOOL bRet = TRUE;

	if (m_fdStdErr != -1)
	{
		// This will close this as well: this->hChildStderrRdDup
		_close( m_fdStdErr);
		m_fdStdErr = -1;
		m_hChildStderrRdDup = NULL;
	}
	if (m_fdStdIn != -1)
	{
		// This will close this as well: this->hChildStdinWrDup
		_close( m_fdStdIn);
		m_fdStdIn = -1;
		m_hChildStdinWrDup = NULL;
	}
	if (m_fdStdOut != -1)
	{
		// This will close this as well: this->hChildStdoutRdDup
		_close( m_fdStdOut);
		m_fdStdOut = -1;
		m_hChildStdoutRdDup = NULL;
	}
	if (m_hChildStdinRd && !CloseHandle( m_hChildStdinRd))
	{
		bRet = FALSE;
	}
	m_hChildStdinRd = NULL;
	if (m_hChildStdinWr && !CloseHandle( m_hChildStdinWr))
	{
		bRet = FALSE;
	}
	m_hChildStdinWr = NULL;
	if (m_hChildStdoutRd && !CloseHandle( m_hChildStdoutRd))
	{
		bRet = FALSE;
	}
	m_hChildStdoutRd = NULL;
	if (m_hChildStdoutWr && !CloseHandle( m_hChildStdoutWr))
	{
		bRet = FALSE;
	}
	m_hChildStdoutWr = NULL;
	if (m_hChildStderrRd && !CloseHandle( m_hChildStderrRd))
	{
		bRet = FALSE;
	}
	m_hChildStderrRd = NULL;
	if (m_hChildStderrWr && !CloseHandle( m_hChildStderrWr))
	{
		bRet = FALSE;
	}
	m_hChildStderrWr = NULL;
	return bRet;
}

DWORD CExecCommand::realCreateProcess(LPCTSTR lpstrCommand, LPCTSTR lpstrPath, BOOL bCapture)
{
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO		siStartInfo;
	CString			csComspec,
					csCommand;
	static DWORD	dwProcessID = 0;

	ASSERT( lpstrCommand);

	if (m_bComspec)
	{
		// Find COMSPEC environnement variable to start process
		if (!csComspec.GetEnvironmentVariable( _T( "COMSPEC")))
		{
			/*
			* Oh gag, we're on Win9x or using COMMAND.COM. Not supported
			*/
			m_csOutput.AppendFormat( "Get COMSPEC Error: %s", GetAnsiFromUnicode( LookupError( GetLastError())));
			return 0;
		}
		csCommand.Format( _T( "\"%s\" /c %s"), csComspec, lpstrCommand);
	}
	else
		// Do not use COMSPEC to start process => full path for command must be provided
		csCommand = lpstrCommand;

	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	if (bCapture)
	{
		// Capture stdin/stdout/stderr
		siStartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		siStartInfo.hStdInput = m_hChildStdinRd;
		siStartInfo.hStdOutput = m_hChildStdoutWr;
		siStartInfo.hStdError = m_hChildStdoutWr;
	}
	else
		// No capture
		siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
	siStartInfo.wShowWindow = SW_HIDE;

	if (!CreateProcess( NULL, csCommand.GetBuffer(), NULL, NULL, TRUE,
                    0, //CREATE_NEW_CONSOLE,
                    NULL, lpstrPath, &siStartInfo, &piProcInfo)) 
    {
		m_csOutput.AppendFormat( "CreateProcess Error: %s", GetAnsiFromUnicode( LookupError( GetLastError())));
		return 0;
	}
	// Close the handles now so anyone waiting is woken.
	CloseHandle(piProcInfo.hThread);
	// Return process handle
	m_hProcessHandle = piProcInfo.hProcess;
	dwProcessID = piProcInfo.dwProcessId;
	return dwProcessID;
}

BOOL CExecCommand::startProcessCapture(LPCTSTR lpstrCommand, LPCTSTR lpstrPath)
{
	SECURITY_ATTRIBUTES saAttr;
	int fd1, fd2, fd3;

	try
	{
		ASSERT( lpstrCommand);
		initialize();

		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;

		fd1 = 0;
		fd2 = 0;
		fd3 = 0;

		////////////////////////////////////////////////////////////////
		// Capture stdin
		if (!CreatePipe(&m_hChildStdinRd, &m_hChildStdinWr, &saAttr, 0))
		{
			m_csOutput.AppendFormat( "CreatePipe Error: %s", GetAnsiFromUnicode( LookupError( GetLastError())));
			return FALSE;
		}
		/* Create new output read handle and the input write handle. Set
		* the inheritance properties to FALSE. Otherwise, the child inherits
		* the these handles; resulting in non-closeable handles to the pipes
		* being created. */
		if (!DuplicateHandle( GetCurrentProcess(), m_hChildStdinWr, GetCurrentProcess(), &m_hChildStdinWrDup, 0,
			 FALSE, DUPLICATE_SAME_ACCESS))
		{
			m_csOutput.AppendFormat( "DuplicateHandle Error");
			return FALSE;
		}

		// Close the inheritable version of ChildStdin that we're using.
		CloseHandle( m_hChildStdinWr);
		m_hChildStdinWr = NULL;

		////////////////////////////////////////////////////////////////
		// Capture stdout
		if (!CreatePipe(&m_hChildStdoutRd, &m_hChildStdoutWr, &saAttr, 0))
		{
			m_csOutput.AppendFormat( "CreatePipe Error: %s", GetAnsiFromUnicode( LookupError( GetLastError())));
			return FALSE;
		}
		if (!DuplicateHandle( GetCurrentProcess(), m_hChildStdoutRd, GetCurrentProcess(), &m_hChildStdoutRdDup, 0,
			 FALSE, DUPLICATE_SAME_ACCESS))
		{
			m_csOutput.AppendFormat( "DuplicateHandle Error");
			return FALSE;
		}

		// Close the inheritable version of ChildStdout that we're using.
		CloseHandle( m_hChildStdoutRd);
		m_hChildStdoutRd = NULL;

		////////////////////////////////////////////////////////////////
		// Capture stderr
		if (!CreatePipe( &m_hChildStderrRd, &m_hChildStderrWr, &saAttr, 0))
		{
			m_csOutput.AppendFormat( "CreatePipe Error: %s", GetAnsiFromUnicode( LookupError( GetLastError())));
			return FALSE;
		}
		if (!DuplicateHandle( GetCurrentProcess(), m_hChildStderrRd, GetCurrentProcess(), &m_hChildStderrRdDup, 0,
			  FALSE, DUPLICATE_SAME_ACCESS))
		{
			m_csOutput.AppendFormat( "DuplicateHandle Error");
			return FALSE;
		}
		// Close the inheritable version of ChildStdErr that we're using.
		CloseHandle( m_hChildStderrRd);
		m_hChildStderrRd = NULL;

		fd1 = _open_osfhandle(TO_INTPTR(m_hChildStdinWrDup), _O_RDONLY|_O_TEXT);
		fd2 = _open_osfhandle(TO_INTPTR(m_hChildStdoutRdDup), _O_RDONLY|_O_TEXT);
		fd3 = _open_osfhandle(TO_INTPTR(m_hChildStderrRdDup), _O_RDONLY|_O_TEXT);

		if (realCreateProcess( lpstrCommand, lpstrPath, TRUE) == 0)
		{
			if(fd1 >= 0)
			{
				_close(fd1);
			}
			if(fd2 >= 0)
			{
				_close(fd2);
			}
			if(fd3 >= 0)
			{
				_close(fd3);
			}
			return FALSE;
		}

		/*
		* Insert the files we've created into the process dictionary
		* all referencing the list with the process handle and the
		* initial number of files (see description below in _PyPclose).
		* Since if _PyPclose later tried to wait on a process when all
		* handles weren't closed, it could create a deadlock with the
		* child, we spend some energy here to try to ensure that we
		* either insert all file handles into the dictionary or none
		* at all.  It's a little clumsy with the various popen modes
		* and variable number of files involved.
		*/

		/* Child is launched. Close the parents copy of those pipe
		* handles that only the child should have open.  You need to
		* make sure that no handles to the write end of the output pipe
		* are maintained in this process or else the pipe will not close
		* when the child process exits and the ReadFile will hang. */
		if ( fd1 >= 0 )
		{
			m_fdStdIn = fd1;
		}
		if ( fd2 >= 0 )
		{
			m_fdStdOut = fd2;
		}
		if ( fd3 >= 0 )
		{
			m_fdStdErr = fd3;
		}
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

int CExecCommand::execWait( LPCTSTR lpstrCommand, LPCTSTR lpstrPath)
{
	if (!startProcessCapture( lpstrCommand, lpstrPath))
		return EXEC_ERROR_START_COMMAND;
	if (!waitCapture())
		return EXEC_ERROR_WAIT_COMMAND;
	return EXEC_SUCCESSFULL;
}

int CExecCommand::execNoWait( LPCTSTR lpstrCommand, LPCTSTR lpstrPath)
{
	try
	{
		ASSERT( lpstrCommand);
		initialize();
		
		// Start process
		if (realCreateProcess( lpstrCommand, lpstrPath) == 0) 
		{
			return EXEC_ERROR_START_COMMAND;
		}
		return EXEC_SUCCESSFULL;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return EXEC_ERROR_START_COMMAND;
	}
}

BOOL  CExecCommand::waitCapture()
{
	int		nResult = -1;
	DWORD	dwExitCode;
	char	bBuffer[1024];
	int		nLength;

	m_csOutput = "";
	BOOL bDone = false;

	try
	{
		while(!bDone)
		{
			Sleep(10);
			BOOL bHaveSome = false;
			struct _stat fsout;
			struct _stat fserr;
			int rOut = _fstat( m_fdStdOut, &fsout);
			int rErr = _fstat( m_fdStdErr, &fserr);
			if ( rOut && rErr )
			{
				break;
			}
			if (fserr.st_size > 0)
			{
				nLength = _read( m_fdStdErr, bBuffer, 1023);
				bBuffer[nLength] = 0;
				m_csOutput.AppendFormat( "%s", bBuffer);
				bHaveSome = TRUE;
			}
			if (fsout.st_size > 0)
			{
				nLength = _read( m_fdStdOut, bBuffer, 1023);
				bBuffer[nLength] = 0;
				m_csOutput.AppendFormat( "%s", bBuffer);
				bHaveSome = TRUE;
			}
			if (!bHaveSome)
			{
				GetExitCodeProcess( m_hProcessHandle, &dwExitCode);
				if (dwExitCode != STILL_ACTIVE)
				{
					break;
				}
			}
		}  

		if ((WaitForSingleObject( m_hProcessHandle, m_dwTimeout) != WAIT_FAILED) &&
			GetExitCodeProcess( m_hProcessHandle, &dwExitCode)) 
		{
			nResult = dwExitCode;
		} 
		else 
		{
			m_csOutput.Format( "GetExitCode Error: %s", GetAnsiFromUnicode( LookupError( GetLastError())));
			nResult = -1;
		}

		// Free up the native handle at this point
		CloseHandle( m_hProcessHandle);
		m_hProcessHandle = NULL;
		m_nExitValue = nResult;
		if (nResult < 0 || !closeHandles())
		{
			return FALSE;
		}
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

int CExecCommand::execWaitForAllChilds( LPCTSTR lpstrCommand, LPCTSTR lpstrPath)
{
	CObArray		myProcessList;
	CProcessProps	*pProcess;
	DWORD			dwExitCode,
					dwProcessID;

	try
	{
		ASSERT( lpstrCommand);
		initialize();

		// Start process
		if ((dwProcessID = realCreateProcess( lpstrCommand, lpstrPath)) == 0) 
		{
			return EXEC_ERROR_START_COMMAND;
		}
		// We need high priority on OS to follow thread/process created by main command
		SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS);
		// Store first process
		pProcess = new CProcessProps();
		pProcess->set( dwProcessID, GetCurrentProcessId(), lpstrCommand);
		myProcessList.Add( pProcess);
		// While there is running processes
		while (myProcessList.GetCount() > 0)
		{
			// Parse memory processes for new childs process or terminated processes
			if (!parseRunningProcesses( &myProcessList))
			{
				m_csOutput.AppendFormat( "Parse running processes Error: %s", GetAnsiFromUnicode( LookupError( GetLastError())));
				SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
				return EXEC_ERROR_WAIT_COMMAND;
			}
			Sleep( EXEC_WAIT_CHECK_LATENCY);
		}
		// Now return to normal prioity
		SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
		// Get exit code
		if (GetExitCodeProcess( m_hProcessHandle, &dwExitCode)) 
		{
			m_nExitValue = dwExitCode;
		}
		else
		{
			m_nExitValue = -1;
			m_csOutput.AppendFormat( "GetExitCode Error: %s", GetAnsiFromUnicode( LookupError( GetLastError())));
			return EXEC_ERROR_WAIT_COMMAND;
		}
		return EXEC_SUCCESSFULL;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return EXEC_ERROR_START_COMMAND;
	}
}

BOOL CExecCommand::parseRunningProcesses( CObArray *pProcessList)
{
    HANDLE			hSnapshot = NULL; 
    BOOL			bRet      = FALSE; 
    PROCESSENTRY32	pe32; 
	INT_PTR 		uIndex;
	CProcessProps	*pProc;

	try
	{
		// Take a snapshot of running processes
		if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) 
			return FALSE; 
		pe32.dwSize = sizeof(PROCESSENTRY32); 
		// By default, set all already listed processes as not running
		for (uIndex=0; uIndex<pProcessList->GetCount(); uIndex++)
		{
			pProc = (CProcessProps *) pProcessList->GetAt( uIndex);
			pProc->setRunning( FALSE);
		}
		// Check each process in memory
		for (bRet = Process32First(hSnapshot, &pe32); bRet; bRet = Process32Next(hSnapshot, &pe32))
		{
			// Check if process is already listed and set it is running
			if (!isProcessListed( pProcessList, pe32.th32ProcessID))
			{
				// This is not a listed process, is the parent listed
				if (isProcessListed( pProcessList, pe32.th32ParentProcessID))
				{
					// Parent process is listed, so register we have to wait for this process
					pProc = new CProcessProps;
					pProc->set( pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile);
					pProcessList->Add( pProc);
				}
			}
		}
		CloseHandle( hSnapshot);
		hSnapshot = NULL;
		// Now, remove from list all processes not running
		for (uIndex=0; uIndex<pProcessList->GetCount(); uIndex++)
		{
			pProc = (CProcessProps *) pProcessList->GetAt( uIndex);
			if (!pProc->isRunning())
			{
				pProcessList->RemoveAt( uIndex);
				delete pProc;
			}
		}
		return TRUE;
	}
	catch (CException *pEx)
	{
		if (hSnapshot != NULL)
			CloseHandle( hSnapshot);
		pEx->Delete();
		return FALSE;
	}
}

BOOL CExecCommand::isProcessListed( CObArray *pProcessList, DWORD dwProcessID)
{
	INT_PTR 		uIndex;
	CProcessProps	*pProc;

	for (uIndex=0; uIndex<pProcessList->GetCount(); uIndex++)
	{
		pProc = (CProcessProps *) pProcessList->GetAt( uIndex);
		if (pProc->getProcessID() == dwProcessID)
		{
			pProc->setRunning();
			return TRUE;
		}
	}
	return FALSE;
}
