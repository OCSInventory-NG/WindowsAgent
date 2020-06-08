//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// WindowsSocket.cpp: implementation of the CWindowsSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sysinfo.h"
#include <Winsock.h>
#include "WindowsSocket.h"
#include "DebugLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Winsock functions declaration for getting local IP Address
int (__stdcall *lpfnWSAStartup) (WORD wVersionRequested, WSADATA *wsaData);
int (__stdcall *lpfnGetHostName) (char *lpstrName, int nMaxSize);
struct hostent * (__stdcall *lpfnGetHostByName) (const char *lpstrName);
char * (__stdcall *lpfnInet_ntoa) (struct in_addr pInetAddr);
int (__stdcall *lpfnWSACleanup) (void);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWindowsSocket::CWindowsSocket()
{
	m_csIP = NOT_AVAILABLE;
}

CWindowsSocket::~CWindowsSocket()
{

}

LPCTSTR CWindowsSocket::GetIPAddress()
{
	struct hostent	*pHostEnt;
	struct in_addr	pInetAddr;
	WORD			wVersionRequested;
	WSADATA			wsaData;
	HINSTANCE	    hDll;
	char			szHostName[256];

	AddLog( _T( "WSA Get IPv4 address..."));
	// By default, Not available
	m_csIP = NOT_AVAILABLE;

	// Load the Winsock 32 bit DLL
	if ((hDll = LoadLibrary( _T( "wsock32.dll"))) == NULL)
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		AddLog( _T( "Failed to load <wsock32.dll> !\n"));
		return m_csIP;
	}
	// Load the WSAStartup function
	if ((*(FARPROC*)&lpfnWSAStartup = GetProcAddress( hDll, "WSAStartup")) == NULL)
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		AddLog( _T( "Failed to load <WSAStartup> function from wsock32.dll !\n"));
		FreeLibrary( hDll);
		return m_csIP;
	}
	// Load the gethostname function
	if ((*(FARPROC*)&lpfnGetHostName = GetProcAddress( hDll, "gethostname")) == NULL)
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		AddLog( _T( "Failed to load <gethostname> function from wsock32.dll !\n"));
		FreeLibrary( hDll);
		return m_csIP;
	}
	// Load the gethostbyname function
	if ((*(FARPROC*)&lpfnGetHostByName = GetProcAddress( hDll, "gethostbyname")) == NULL)
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		AddLog( _T( "Failed to load <gethostbyname> function from wsock32.dll !\n"));
		FreeLibrary( hDll);
		return m_csIP;
	}
	// Load the inet_ntoa function
	if ((*(FARPROC*)&lpfnInet_ntoa = GetProcAddress( hDll, "inet_ntoa")) == NULL)
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		AddLog( _T( "Failed to load <inet_ntoa> function from wsock32.dll !\n"));
		FreeLibrary( hDll);
		return m_csIP;
	}
	// Load the WSACleanup function
	if ((*(FARPROC*)&lpfnWSACleanup = GetProcAddress( hDll, "WSACleanup")) == NULL)
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		AddLog( _T( "Failed to load <WSACleanup> function from wsock32.dll !\n"));
		FreeLibrary( hDll);
		return m_csIP;
	}
	wVersionRequested = MAKEWORD( 1, 1 );
 	if (lpfnWSAStartup( wVersionRequested, &wsaData ) != 0)
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		AddLog( _T( "Failed in call to <WSAStartup> function from wsock32.dll !\n"));
		FreeLibrary( hDll);
		return m_csIP;
	}
	// Confirm that the WinSock DLL supports 1.1.
	// Note that if the DLL supports versions greater    
	// than 1.1 in addition to 1.1, it will still return 
	// 1.1 in wVersion since that is the version we      
	// requested.                                        
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
			HIBYTE( wsaData.wVersion ) != 1 ) 
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		AddLog( _T( "Failed because Winsock version < 1.1 !\n"));
		lpfnWSACleanup();
		FreeLibrary( hDll);
		return m_csIP; 
	}
 	if (lpfnGetHostName( szHostName, 256) == SOCKET_ERROR)
	{
		// Unable to get hostname
		AddLog( _T( "Failed in call to <gethostname> function from wsock32.dll !\n"));
		lpfnWSACleanup();
		FreeLibrary( hDll);
		return m_csIP; 
	}
 	if ((pHostEnt = lpfnGetHostByName( szHostName)) == NULL)
	{
		// Unable to get list of interfaces
		AddLog( _T( "Failed in call to <gethostbyname> function from wsock32.dll !\n"));
		lpfnWSACleanup( );
		FreeLibrary( hDll);
		return m_csIP; 
	}
	if (pHostEnt->h_addrtype != AF_INET)
	{
		// Not 32 bits IP adresses
		AddLog( _T( "Failed because no IPv4 Address !\n"));
		lpfnWSACleanup( );
		FreeLibrary( hDll);
		return m_csIP; 
	}
	else
	{
		// Computer as an IP Address => get the first one
		pInetAddr.s_addr = *(u_long *) pHostEnt->h_addr_list[0];
		m_csIP = lpfnInet_ntoa( pInetAddr);
	}
	// Unload the Winsock
	lpfnWSACleanup( );
	// Unload the Winsock Dll
	FreeLibrary( hDll);
	AddLog( _T( "OK (%s)\n"), m_csIP);
	return m_csIP;
}
