//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsProcess.cpp: implementation of the CProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sysinfo.h"
#include "OcsProcess.h"
#include <TlHelp32.h>
#include "DebugLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Process Status Helper functions for Windows NT 3.5X/4.0/2000 in PSAPI.DLL
BOOL (__stdcall *lpfnEnumProcesses) ( DWORD *lpidProcesses, DWORD cb, LPDWORD cbNeeded);
BOOL (__stdcall *lpfnEnumProcessModules) ( HANDLE hProcess, HMODULE *lphModules, DWORD cb, LPDWORD cbNeeded);
DWORD (__stdcall *lpfnGetModuleFileNameEx) ( HANDLE hProcess, HMODULE hModules, LPTSTR lpBaseName, DWORD nSize);

// ToolHelp 32 Process functions for Windows 9X/Me in KERNEL32.DLL
HANDLE (__stdcall *lpfnCreateToolhelp32Snapshot) ( DWORD dwFlags, DWORD th32ProcessID);
BOOL (__stdcall *lpfnProcess32First) ( HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
BOOL (__stdcall *lpfnProcess32Next) ( HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
BOOL (__stdcall *lpfnModule32First) ( HANDLE hSnapshot, LPMODULEENTRY32 lpme);
BOOL (__stdcall *lpfnModule32Next) ( HANDLE hSnapshot, LPMODULEENTRY32 lpme);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcess::CProcess()
{

}

CProcess::~CProcess()
{

}

BOOL CProcess::IsProcessRunning(LPCTSTR lpstrProcessName)
{
	OSVERSIONINFO osVersion;

	osVersion.dwOSVersionInfoSize = sizeof( OSVERSIONINFO);
	if (!GetVersionEx( &osVersion))
	{
		AddLog( _T( "Retrieving Process <%s> status...Failed in call to <GetVersionEx> function !\n"), lpstrProcessName);
		return FALSE;
	}
	switch (osVersion.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Windows NT 3.5X/4/2000
		return IsProcessRunningNT( lpstrProcessName); 
	case VER_PLATFORM_WIN32_WINDOWS:
		// Windows 9X/Me
		return IsProcessRunning9X( lpstrProcessName); 
	default:
		// Unknown
		AddLog( _T( "Retrieving Process <%s> status...Failed because unsupported OS !\n"), lpstrProcessName);
		return FALSE;
	}
}

BOOL CProcess::IsProcessRunning9X(LPCTSTR lpstrProcessName)
{
	CString		csFindedModuleName,
				csModuleNameToFind = lpstrProcessName;
	BOOL		bFound = FALSE;
	HANDLE			hSnapProcess,
					hSnapModule;
	PROCESSENTRY32	peProcess;
	MODULEENTRY32	meModule;
	BOOL			bNextProcess,
					bNextModule;


	AddLog( _T( "\t9X Retrieving Process <%s> status...\n"), lpstrProcessName);
	csModuleNameToFind.MakeLower();
	// Load the CreateToolhelp32Snapshot function
	if ((*(FARPROC*)&lpfnCreateToolhelp32Snapshot = GetProcAddress( GetModuleHandle(_T( "KERNEL32.DLL")), "CreateToolhelp32Snapshot")) == NULL)
	{
		// Tell the user that we could not find a usable function 
		AddLog( _T( "\t9X Retrieving Process: Failed to load <CreateToolhelp32Snapshot> function from KERNEL32.dll !\n"));
		return FALSE;
	}
	// Load the Process32First function
	if ((*(FARPROC*)&lpfnProcess32First = GetProcAddress( GetModuleHandle(_T( "KERNEL32.DLL")), "Process32First")) == NULL)
	{
		// Tell the user that we could not find a usable function 
		AddLog( _T( "\t9X Retrieving Process: Failed to load <Process32First> function from KERNEL32.dll !\n"));
		return FALSE;
	}
	// Load the Process32Next function
	if ((*(FARPROC*)&lpfnProcess32Next = GetProcAddress( GetModuleHandle(_T( "KERNEL32.DLL")), "Process32Next")) == NULL)
	{
		// Tell the user that we could not find a usable function 
		AddLog( _T( "\t9X Retrieving Process: Failed to load <Process32Next> function from KERNEL32.dll !\n"));
		return FALSE;
	}
	// Load the Module32First function
	if ((*(FARPROC*)&lpfnModule32First = GetProcAddress( GetModuleHandle(_T( "KERNEL32.DLL")), "Module32First")) == NULL)
	{
		// Tell the user that we could not find a usable function 
		AddLog( _T( "\t9X Retrieving Process: Failed to load <Module32First> function from KERNEL32.dll !\n"));
		return FALSE;
	}
	// Load the Module32Next function
	if ((*(FARPROC*)&lpfnModule32Next = GetProcAddress( GetModuleHandle(_T( "KERNEL32.DLL")), "Module32Next")) == NULL)
	{
		// Tell the user that we could not find a usable function 
		AddLog( _T( "\t9X Retrieving Process: Failed to load <Module32Next> function from KERNEL32.dll !\n"));
		return FALSE;
	}
	// Create a Toolhelp32 snapshot for processes
	if ((hSnapProcess = lpfnCreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0)) != NULL)
	{
		// OK => get the first process
		peProcess.dwSize = sizeof( PROCESSENTRY32);
		bNextProcess = lpfnProcess32First( hSnapProcess, &peProcess);
		while ((!bFound) && bNextProcess)
		{
			/// Create a Toolhelp32 snapshot for modules of current process
			if ((hSnapModule = lpfnCreateToolhelp32Snapshot( TH32CS_SNAPMODULE, peProcess.th32ProcessID)) != NULL)
			{
				// OK => get the first process module
				meModule.dwSize = sizeof( MODULEENTRY32);
				bNextModule = lpfnModule32First( hSnapModule, &meModule);
				while ((!bFound) && bNextModule)
				{
					csFindedModuleName = meModule.szModule;
					csFindedModuleName.MakeLower();
					if (csFindedModuleName.Find( csModuleNameToFind) >= 0)
						// Finded
						bFound = TRUE;
					else
						// Get the next process module
						bNextModule = lpfnModule32Next( hSnapModule, &meModule);
				}
				CloseHandle( hSnapModule);
			}
			// Get the next process
			bNextProcess = lpfnProcess32Next( hSnapProcess, &peProcess);
		}
		CloseHandle( hSnapProcess);
		AddLog( _T( "\t9X Retrieving Process: OK (%s).\n"), bFound ? _T( "running") : _T( "NOT running"));
	}
	else
		AddLog( _T( "\t9X Retrieving Process: Failed in call to <CreateToolhelp32Snapshot> function from KERNEL32.dll (error #%lu) !\n"),
				GetLastError());
	return bFound;
}

BOOL CProcess::IsProcessRunningNT(LPCTSTR lpstrProcessName)
{
	TCHAR		*szModuleName;
	CString		csFindedModuleName,
				csModuleNameToFind = lpstrProcessName;
	BOOL		bFound = FALSE;
	DWORD		lpidProcess[MAX_OBJECTS],
				cbSize = MAX_OBJECTS * sizeof( DWORD),
				cbNeeded;
	HANDLE		hProcess;
	HMODULE		lphModule[MAX_OBJECTS],
				hDll;

	AddLog( _T( "\tNT Retrieving Process <%s> status...\n"), lpstrProcessName);
	csModuleNameToFind.MakeLower();
	// Use PSAPI.DLL
	if ((hDll = LoadLibrary( _T( "PSAPI.DLL"))) == NULL)
	{
	 	AddLog( _T( "\tNT Retrieving Process: Failed to load <PSAPI.dll> !\n"));
		return FALSE;
	}
	// Load the EnumProcesses function
	if ((*(FARPROC*)&lpfnEnumProcesses = GetProcAddress( hDll, "EnumProcesses")) == NULL)
	{
		// Tell the user that we could not find a usable 
		// PSAPI DLL.                                  
		AddLog( _T( "\tNT Retrieving Process: Failed to load <EnumProcesses> function from PSAPI.dll !\n"));
		FreeLibrary( hDll);
		return FALSE;
	}
	// Load the EnumProcessModules function
	if ((*(FARPROC*)&lpfnEnumProcessModules = GetProcAddress( hDll, "EnumProcessModules")) == NULL)
	{
		// Tell the user that we could not find a usable 
		// PSAPI DLL.                                  
		AddLog( _T( "\tNT Retrieving Process: Failed to load <EnumProcessModules> function from PSAPI.dll !\n"));
		FreeLibrary( hDll);
		return FALSE;
	}
	// Load the GetModuleFileNameEx function
#ifndef _UNICODE
	if ((*(FARPROC*)&lpfnGetModuleFileNameEx = GetProcAddress( hDll, "GetModuleFileNameExA")) == NULL)
	{
		// Tell the user that we could not find a usable 
		// PSAPI DLL.                                  
		AddLog( _T( "\tNT Retrieving Process: Failed to load <GetModuleFileNameExA> function from PSAPI.dll !\n"));
		FreeLibrary( hDll);
		return FALSE;
	}
#else
	if ((*(FARPROC*)&lpfnGetModuleFileNameEx = GetProcAddress( hDll, "GetModuleFileNameExW")) == NULL)
	{
		// Tell the user that we could not find a usable 
		// PSAPI DLL.                                  
		AddLog( _T( "\tNT Retrieving Process: Failed to load <GetModuleFileNameExW> function from PSAPI.dll !\n"));
		FreeLibrary( hDll);
		return FALSE;
	}
#endif
	// Enum all running processes
	if (lpfnEnumProcesses( lpidProcess, cbSize, &cbNeeded))
	{
		// OK
		LONG	lProcessNumber = cbNeeded / sizeof( DWORD),
				lProcessCpt;

		// For each process, enum all loaded modules
		for (lProcessCpt=0; (lProcessCpt<lProcessNumber) && (!bFound); lProcessCpt++)
		{
			if ((hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, lpidProcess[ lProcessCpt])) == NULL)
			{
				AddLog( _T( "\t\tFailed in call to <OpenProcess> function (error #%lu)!\n"),
						GetLastError());
				continue;
			}
			// Retrieve the number of bytes needed
			cbSize = MAX_OBJECTS * sizeof( HMODULE);
			//lpfnEnumProcessModules( hProcess, lphModule, cbSize, &cbNeeded);
			// Retrieve the process modules
			if (lpfnEnumProcessModules( hProcess, lphModule, cbSize, &cbNeeded))
			{
				LONG	lModuleNumber = cbNeeded / sizeof( HMODULE),
						lCptModule;

				// Find if a process module as the required name
				for (lCptModule=0; (lCptModule<lModuleNumber) && (!bFound); lCptModule++)
				{
					csFindedModuleName.Empty();
					szModuleName = csFindedModuleName.GetBuffer( _MAX_PATH+1);
					cbSize = _MAX_PATH;
					lpfnGetModuleFileNameEx( hProcess, lphModule[ lCptModule], szModuleName, cbSize);
					csFindedModuleName.ReleaseBuffer( (int) _tcslen( szModuleName)+1);
					csFindedModuleName.MakeLower();
					if (csFindedModuleName.Find( csModuleNameToFind) >= 0)
						// Finded
						bFound = TRUE;
				}
			}
			else
				AddLog( _T( "\t\tFailed in call to <EnumProcessModules> function (error #%lu)!\n"),
						GetLastError());
			CloseHandle( hProcess);
		}
		AddLog( _T( "\tNT Retrieving Process: OK (%s).\n"), bFound ? _T( "running") : _T( "NOT running"));
	}
	else
		AddLog( _T( "\tNT Retrieving Process: Failed in call to <EnumProcesses> function from KERNEL32.dll (error #%lu) !\n"),
				GetLastError());
	FreeLibrary( hDll);
	return bFound;
}
