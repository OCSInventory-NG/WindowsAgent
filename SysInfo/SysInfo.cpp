//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CSysInfo.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SysInfo.h"
#include "IPHelper.h"
#include "SNMP.h"
#include "FileVersion.h"
#include "dtwinver.h"
#include <TlHelp32.h>
#include "DebugLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSysInfoApp

BEGIN_MESSAGE_MAP(CSysInfoApp, CWinApp)
END_MESSAGE_MAP()


// CSysInfoApp construction

CSysInfoApp::CSysInfoApp()
{
}


// The one and only CSysInfoApp object
CSysInfoApp theApp;


// CSysInfoApp initialization

BOOL CSysInfoApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


CSysInfo::CSysInfo( BOOL bEnableLog, LPCTSTR lpstrFolder)
{
	if (bEnableLog)
		OpenLog( lpstrFolder);
	GetSystemInfo( &m_SystemInfo);
	m_wmiInfo.Connect();
	m_dmiInfo.Connect();
	m_registryInfo.Connect();
	m_dwAddressWidth = 0;
}

CSysInfo::~CSysInfo()
{
	// Nothing to do yet
	m_wmiInfo.Disconnect();
	m_dmiInfo.Disconnect();
	m_registryInfo.Disconnect();
	CloseLog();
}

BOOL CSysInfo::getOS( CString &csName, CString &csVersion, CString &csComment, CString &csDescription)
{
	if (m_wmiInfo.GetOS( csName, csVersion, csComment, csDescription))
		// WMI successful
		return TRUE;

	AddLog( _T( "dtWinVer GetOS..."));

	COSVersion::OS_VERSION_INFO osvi;
	COSVersion					os;
	BOOL						bIsServer = FALSE;

	memset(&osvi, 0, sizeof(osvi));
	if (os.GetVersion(&osvi))
	{
		// Get OS name and comments
		switch (osvi.UnderlyingPlatform)
		{
			case COSVersion::Dos:               
			{
				csName = _T("DOS");
				csComment.Empty();
				break;
			}
			case COSVersion::Windows3x:         
			{
				csName = _T("Microsoft Windows 3.X");
				csComment.Empty();
				break;
			}
			case COSVersion::WindowsCE:            
			{
				csName = _T("Microsoft Windows CE");
				csComment.Empty();
				break;
			}
			case COSVersion::Windows9x:           
			{
				if (os.IsWindows95(&osvi))
				{
					csName = _T( "Microsoft Windows 95");
					csComment.Empty();
				}
				else if (os.IsWindows95SP1(&osvi))
				{
					csName = _T( "Microsoft Windows 95");
					csComment = _T( "Service Pack 1");
				}
				else if (os.IsWindows95B(&osvi))
				{
					csName = _T( "Microsoft Windows 95");
					csComment = _T( "Operating System Release 2.0/2.1 (B)");
				}
				else if (os.IsWindows95C(&osvi))
				{
					csName = _T( "Microsoft Windows 95");
					csComment = _T( "Operating System Release 2.5 (C)");
				}
				else if (os.IsWindows98(&osvi))
				{
					csName = _T( "Microsoft Windows 98");
					csComment.Empty();
				}
				else if (os.IsWindows98SP1(&osvi))
				{
					csName = _T( "Microsoft Windows 98");
					csComment = _T( "Service Pack 1");
				}
				else if (os.IsWindows98SE(&osvi))
				{
					csName = _T( "Microsoft Windows 98");
					csComment = _T( "Second Edition");
				}
				else if (os.IsWindowsME(&osvi))
				{
					csName = _T( "Microsoft Windows Millenium");
					csComment.Empty();
				}
				else
				{
					csName = _T( "Unknown OS");
					csComment.Empty();
				}
				break;
			}
			case COSVersion::WindowsNT:    
			{
				if (os.IsNTPreWin2k(&osvi))
				{
					csName.Format( _T( "Microsoft Windows NT %d.%d"),
									osvi.dwUnderlyingMajorVersion,
									osvi.dwUnderlyingMinorVersion);

					if (os.IsNTWorkstation(&osvi))
						csName += _T(" Workstation");
					else if (os.IsNTDatacenterServer(&osvi))
						csName += _T(" Datacenter");
					else if (os.IsNTEnterpriseServer(&osvi))
						csName += _T(" Enterprise");
					else 
						csName += _T(" Server");
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindows2000(&osvi))
				{
					csName = _T("Microsoft Windows 2000");

					if (os.IsWin2000Professional(&osvi))
						csName += _T(" Professional");
					else if (os.IsWin2000DatacenterServer(&osvi))
						csName += _T(" Datacenter");
					else if (os.IsWin2000AdvancedServer(&osvi))
						csName += _T(" Advanced Server");
					else 
						csName += _T(" Server");
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsXP(&osvi))
				{
					csName = _T("Microsoft Windows XP");

					if (os.IsXPPersonal(&osvi))
						csName += _T(" Home Edition");                  
					else if (os.IsXPProfessional(&osvi))
						csName += _T(" Professional");                  
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsServer2003(&osvi))
				{
					csName = _T("Microsoft Windows Server 2003");

					if (os.IsDatacenterWindowsServer2003(&osvi))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2003(&osvi))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2003(&osvi))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2003(&osvi))
						csName += _T(" Standard Edition");
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsServer2003R2(&osvi))
				{
					csName = _T("Microsoft Windows Server 2003 R2");

					if (os.IsDatacenterWindowsServer2003R2(&osvi))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2003R2(&osvi))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2003R2(&osvi))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2003R2(&osvi))
						csName += _T(" Standard Edition");
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsVista(&osvi))
				{
					csName = _T("Microsoft Windows Vista");

					if (os.IsHomeBasicInstalled(&osvi))
						csName += _T(" Home Basic Edition");                  
					else if (os.IsHomeBasicPremium(&osvi))
						csName += _T(" Home Premium Edition");                  
					else if (os.IsBusinessInstalled(&osvi))
						csName += _T(" Business Edition");                  
					else if (os.IsEnterpriseInstalled(&osvi))
						csName += _T(" Enterprise Edition");                  
					else if (os.IsUltimateInstalled(&osvi))
						csName += _T(" Ultimate Edition");                  
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsServer2008(&osvi))
				{
					csName = _T("Microsoft Windows Server 2008");

					if (os.IsDatacenterWindowsServer2008(&osvi))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2008(&osvi))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2008(&osvi))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2008(&osvi))
						csName += _T(" Standard Edition");
				}
				else if (os.IsWindows7(&osvi))
				{
					csName = _T("Microsoft Windows 7");

					if (os.IsHomeBasicInstalled(&osvi))
						csName += _T(" Home Basic Edition");                  
					else if (os.IsHomeBasicPremium(&osvi))
						csName += _T(" Home Premium Edition");                  
					else if (os.IsBusinessInstalled(&osvi))
						csName += _T(" Business Edition");                  
					else if (os.IsEnterpriseInstalled(&osvi))
						csName += _T(" Enterprise Edition");                  
					else if (os.IsUltimateInstalled(&osvi))
						csName += _T(" Ultimate Edition");                  
					csComment = osvi.szUnderlyingCSDVersion;
				}
				if (os.IsEmulated64Bit(&osvi))
					csComment += _T(" (64 Bits)");
				break;
			}
			default:                         
			{
				csName = _T("Unknown OS");
				csComment.Empty();
				break;
			}
		}
		// Format version and build number
		csVersion.Format( _T("%d.%d.%d"), osvi.dwUnderlyingMajorVersion,
											 osvi.dwUnderlyingMinorVersion,
											 osvi.dwUnderlyingBuildNumber);
	}
	else
	{
		AddLog( _T( "Failed in call to GetVersion !\n"));
		csName = _T( "Unbknown OS");
		csVersion = NOT_AVAILABLE;
		csComment.Empty();
		return FALSE;
	}
	AddLog( _T( "%s %s %s. OK\n"), csName, csVersion, csComment);
	m_registryInfo.GetDeviceDescription( csDescription);
	return TRUE;
}

BOOL CSysInfo::getMemory( ULONG *m_ulPhysicalMemory, ULONG *m_ulSwapSize)
{
	CMemory		memoryInfo;
	ULONGLONG	ulTotalRAM,
				ulTotalSwap;

	AddLog( _T( "getMemory..."));
	ulTotalRAM = memoryInfo.getTotalRAM() / ONE_MEGABYTE;
	*m_ulPhysicalMemory = (ULONG) ulTotalRAM;
	ulTotalSwap = memoryInfo.getTotalPageFile() / ONE_MEGABYTE;
	*m_ulSwapSize = (ULONG) ulTotalSwap;
	AddLog( _T( "Physical: %I64u bytes, Swap: %I64u bytes. OK\n"), memoryInfo.getTotalRAM(), memoryInfo.getTotalPageFile());
	return TRUE;
}

LPCTSTR CSysInfo::getLocalIP()
{
	return m_Sock.GetIPAddress();
}

DWORD CSysInfo::getProcessors( CString &csProcType, CString &csProcSpeed)
{
	CString csRegType = NOT_AVAILABLE,
			csRegSpeed = NOT_AVAILABLE;
	static DWORD	dwRegNumber = 0;

	// First, try WMI
	if ((dwRegNumber = m_wmiInfo.GetProcessors( csProcType, csProcSpeed)) > 0)	
	{
		AddLog( _T( "WMI GetProcessors: %lu processor(s) found.\n"), dwRegNumber);
		return dwRegNumber;
	}
	// Last, use registry
	return m_registryInfo.GetProcessors( csProcType, csProcSpeed);
}

DWORD CSysInfo::getAddressWidthOS()
{
	if (m_dwAddressWidth == 0)
		m_dwAddressWidth = m_wmiInfo.GetAddressWidthOS();
	return m_dwAddressWidth;
}

BOOL CSysInfo::getSystemPorts( CSystemPortList *pMyList)
{
	// First, try SMBios/DMI
	if (m_dmiInfo.GetSystemPorts( pMyList))
		return TRUE;
	// Next, try WMI
	if (m_wmiInfo.GetSystemPorts( pMyList))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetSystemPorts( pMyList);
}

BOOL CSysInfo::getPrinters( CPrinterList *pMyList)
{
	// First, try WMI
	if (m_wmiInfo.GetPrinters( pMyList))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetPrinters( pMyList);
}

BOOL CSysInfo::getSoundDevices( CSoundDeviceList *pMyList)
{
	// First, try WMI
	if (m_wmiInfo.GetSoundDevices( pMyList))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetSoundDevices( pMyList);
}

BOOL CSysInfo::getLogicalDrives( CLogicalDriveList *pMyList)
{
	CLogicalDrive	cLogicalDrive;
	UINT		uIndex;
	DWORD		dwValue;
	CString		csDrive;

	// First, try WMI
	if (m_wmiInfo.GetLogicalDrives( pMyList))
		return TRUE;
	// Last, use GetLogicalDrives API
	pMyList->RemoveAll();
	if ((dwValue = GetLogicalDrives()) != 0)
	{
		for (uIndex=0; uIndex<=26; uIndex++)
		{
			// Check if the logical drive uIndex really exists
			if (dwValue & 1)
			{
				// Yes => Construct the root directory
				csDrive.Format( _T( "%c:\\"), 'A'+uIndex);
				// Check if this is a local Hard Disk
				cLogicalDrive.RetrieveDriveInfo( csDrive);
				// Add the logical drive to the list
				pMyList->AddTail( cLogicalDrive);
			}
			// Bit shift the logical drives mask
			dwValue >>= 1;
		}
	}
	return TRUE;
}

BOOL CSysInfo::getMemorySlots( CMemorySlotList *pMyList)
{
	// First, try SMBios/DMI
	if (m_dmiInfo.GetMemorySlots( pMyList))
		return TRUE;
	// Last try WMI
	return m_wmiInfo.GetMemorySlots( pMyList);
}

BOOL CSysInfo::getModems( CModemList *pMyList)
{
	// First, try WMI
	if (m_wmiInfo.GetModems( pMyList))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetModems( pMyList);
}

BOOL CSysInfo::getMonitors( CMonitorList *pMyList)
{
	CEdid edidInfo;		// Class to get information for EDID enabled display

	// First try EDID
	if (edidInfo.Connect() && edidInfo.GetMonitors( pMyList))
	{
		edidInfo.Disconnect();
		return TRUE;
	}
	// Next, try WMI
	if (m_wmiInfo.GetMonitors( pMyList))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetMonitors( pMyList);
}

BOOL CSysInfo::getNetworkAdapters( CNetworkAdapterList *pMyList)
{
	// First try IPHelper
	if (CIPHelper::GetNetworkAdapters( pMyList))
		return TRUE;
	// Last, use SNMP
	return CSNMP::GetNetworkAdapters( pMyList);
}

BOOL CSysInfo::getVideoAdapters( CVideoAdapterList *pMyList)
{
	// First, try WMI
	if (m_wmiInfo.GetVideoAdapters( pMyList))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetVideoAdapters( pMyList);
}

BOOL CSysInfo::getSystemControllers( CSystemControllerList *pMyList)
{
	// First, try WMI
	if (m_wmiInfo.GetSystemControllers( pMyList))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetSystemControllers( pMyList);
}

BOOL CSysInfo::getStoragePeripherals( CStoragePeripheralList *pMyList)
{
	// First, try WMI
	if (m_wmiInfo.IsConnected() && m_wmiInfo.GetStoragePeripherals( pMyList))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetStoragePeripherals( pMyList);
}

BOOL CSysInfo::getSystemSlots( CSystemSlotList *pMyList)
{
	// First, try SMBios/DMI
	if (m_dmiInfo.GetSystemSlots( pMyList))
		return TRUE;
	// Last, try WMI
	return m_wmiInfo.GetSystemSlots( pMyList);
}

BOOL CSysInfo::getBiosInfo( CBios *pMyBios)
{
	// First, try SMBios/DMI
	if (m_dmiInfo.GetBiosInfo( pMyBios))
		return TRUE;
	// Next, try WMI
	if (m_wmiInfo.GetBiosInfo( pMyBios))
		return TRUE;
	// Last, try registry
	return m_registryInfo.GetBiosInfo( pMyBios);
}

BOOL CSysInfo::isNotebook()
{
	// First, try WMI
	if (m_wmiInfo.IsConnected())
		return m_wmiInfo.IsNotebook();
	// Last, try registry
	return m_registryInfo.IsNotebook();
}


BOOL CSysInfo::getUserName(CString &csUserName)
{
	TCHAR szUserName[255]; 
	DWORD dwUserName = 80; 
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof( PROCESSENTRY32 );
	HANDLE hToken;
	TOKEN_INFORMATION_CLASS TokenInformationClass = TokenUser;
	TCHAR szTokenInformation[255];
	DWORD dwTokenInformationLength = 255;//sizeof( TOKEN_OWNER );
	DWORD dwReturnLength=0;
	DWORD dwName = 255; 
	DWORD dwReferencedDomainName = 255;
	TCHAR szReferencedDomainName[255];
	SID_NAME_USE peUse;
	HANDLE hExplorer = NULL;
	HANDLE hSnapshot = NULL;
	HMODULE hAdv = NULL;
	HMODULE hPs = NULL;
	DWORD dwEnumProcessId[255];
	DWORD dwEnumCb = 255;
	DWORD dwEnumBytesReturned = 0;
	HANDLE hTmpProcess = NULL;
	TCHAR szBaseName[MAX_PATH];
	BOOL bExplorerFound = FALSE;


	HANDLE	(WINAPI* pCreateToolhelp32Snapshot)	(DWORD, DWORD) = NULL; 
	BOOL	(WINAPI* pProcess32First)			(HANDLE, LPPROCESSENTRY32) = NULL; 
	BOOL	(WINAPI* pProcess32Next)			(HANDLE, LPPROCESSENTRY32)  = NULL; 
	HANDLE	(WINAPI* pOpenProcess)				(DWORD, BOOL, DWORD) = NULL; 
	BOOL	(WINAPI* pOpenProcessToken)			(HANDLE, DWORD, PHANDLE) = NULL; 
	BOOL	(WINAPI* pLookupAccountSid)			(LPCTSTR, PSID, LPTSTR, LPDWORD, LPTSTR, LPDWORD, PSID_NAME_USE ) = NULL; 
	BOOL	(WINAPI* pGetTokenInformation)		(HANDLE, TOKEN_INFORMATION_CLASS, LPVOID, DWORD, PDWORD) = NULL;
	BOOL	(WINAPI* pEnumProcesses)			(DWORD*, DWORD, DWORD*) = NULL;
	BOOL	(WINAPI* pGetModuleBaseName)		(HANDLE, HMODULE, LPTSTR, DWORD) = NULL;

	AddLog( _T( "getUserName: Trying to get logged on User ID...\n"));

	// First, try to use Advapi.dll
	if( !(hAdv = LoadLibrary( _T( "Advapi32.dll"))))
	{
		AddLog( _T( "getUserName: Can't load AdvApi. Will use GetUserName()...\n"));	
		goto GETUSERNAME_DEFAULT;
	}
	if( !( (*(FARPROC*)&pOpenProcessToken			= GetProcAddress( hAdv , "OpenProcessToken" ) ) )||
		!( (*(FARPROC*)&pOpenProcess				= GetProcAddress( GetModuleHandle( _T( "KERNEL32.DLL")), "OpenProcess") ) )||
		!( (*(FARPROC*)&pGetTokenInformation		= GetProcAddress( hAdv , "GetTokenInformation") ) )||
#ifdef _UNICODE
		!( (*(FARPROC*)&pLookupAccountSid			= GetProcAddress( hAdv , "LookupAccountSidW") ) ) ) 
#else
		!( (*(FARPROC*)&pLookupAccountSid			= GetProcAddress( hAdv , "LookupAccountSidA") ) ) ) 
#endif
	{
		goto GETUSERNAME_DEFAULT;
	}

	// Try to use kernel32 to enum process
	if(  (*(FARPROC*)&pCreateToolhelp32Snapshot	= GetProcAddress( GetModuleHandle( _T("KERNEL32.DLL")), "CreateToolhelp32Snapshot") ) &&
		 (*(FARPROC*)&pProcess32First			= GetProcAddress( GetModuleHandle( _T("KERNEL32.DLL")), "Process32First") ) &&
		 (*(FARPROC*)&pProcess32Next			= GetProcAddress( GetModuleHandle( _T("KERNEL32.DLL")), "Process32Next") ) ) 
	{
		AddLog( _T( "getUserName: Using Process32 library...OK\n"));		
	}
	// If not available (NT4), using psapi embedded in installer
	else if( hPs = LoadLibrary( _T( "Psapi.dll")) ) 
	{
#ifdef _UNICODE 
		if( (*(FARPROC*)&pEnumProcesses = GetProcAddress( hPs, "EnumProcesses") ) && (*(FARPROC*)&pGetModuleBaseName = GetProcAddress( hPs, "GetModuleBaseNameW") )){
#else
		if( (*(FARPROC*)&pEnumProcesses = GetProcAddress( hPs, "EnumProcesses") ) && (*(FARPROC*)&pGetModuleBaseName = GetProcAddress( hPs, "GetModuleBaseNameA") )){
#endif
			AddLog( _T( "getUserName: Using PSAPI library...OK\n"));	
		}
		else{
			AddLog( _T( "getUserName: Cannot find functions entry point for PSAPI...Will using GetUserName()...\n"));		
			goto GETUSERNAME_DEFAULT;
		}
	}
	//If nothing is available, trying with getusername (user will be system is ocsservice launched ocs)
	else
	{
		AddLog( _T( "getUserName: No process access functions available...Will using GetUserName()...\n"));	
		goto GETUSERNAME_DEFAULT;
	}

	/* If we are here, either enumprocesses, either process32 are available */
	if( pEnumProcesses )
	{
		if( !pEnumProcesses( dwEnumProcessId, dwEnumCb, &dwEnumBytesReturned) )
		{
			goto GETUSERNAME_DEFAULT;
		}
		for( DWORD i=0; i<(dwEnumBytesReturned/sizeof(DWORD)); i++ )
		{
			if( !(hTmpProcess = pOpenProcess( PROCESS_ALL_ACCESS, FALSE, dwEnumProcessId[i] ) ) )
			{
				AddLog( _T( "ERROR: OpenProcess failed with error: %i\n"), GetLastError());
				continue;
			}
			if( !pGetModuleBaseName( hTmpProcess, NULL, szBaseName , MAX_PATH))
			{
				AddLog( _T( "ERROR: GetModuleBaseName failed with error: %i\n"), GetLastError());
				CloseHandle( hTmpProcess );
				hTmpProcess = NULL;
				continue;
			}
			if( !CString(szBaseName).CompareNoCase( _T( "explorer.exe")) ){
				hExplorer = hTmpProcess;
				break;
			}
			CloseHandle( hTmpProcess );
			hTmpProcess = NULL;
		}
		
	}else{
		if( (hSnapshot = pCreateToolhelp32Snapshot( TH32CS_SNAPALL ,0 )) == INVALID_HANDLE_VALUE ){
			AddLog( _T( "ERROR: CreateToolhelp32Snapshot failed with error: %i\n"), GetLastError());
			goto GETUSERNAME_DEFAULT;
		}

		/* Trying to find explorer.exe */
		if( !pProcess32First( hSnapshot, &pe) )
		{
			CloseHandle( hSnapshot );
			hSnapshot = NULL;
			goto GETUSERNAME_DEFAULT;
		}

		do {
			if( !CString(pe.szExeFile).CompareNoCase( _T( "explorer.exe")) )
			{
				bExplorerFound = TRUE;
				break;
			}
			pe.dwSize = sizeof( PROCESSENTRY32 );
		} while( pProcess32Next( hSnapshot, &pe ) );

		/* Retrieve a handle on explorer.exe process using ID */
		if( bExplorerFound )
		{
			if( !(hExplorer = pOpenProcess( PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID )))
			{
				CloseHandle( hSnapshot );
				hSnapshot = NULL;
				goto GETUSERNAME_DEFAULT;
			}
		}
	}

	if( hExplorer )
	{
		/* Open token associated to explorer.exe */
		if( !pOpenProcessToken( hExplorer, TOKEN_READ, &hToken ) ){
			AddLog( _T( "ERROR: OpenProcessToken failed with error: %i\n"), GetLastError());
			CloseHandle( hExplorer );
			hExplorer = NULL;
			CloseHandle( hToken );
			hToken = NULL;
			CloseHandle( hSnapshot );
			hSnapshot = NULL;
			goto GETUSERNAME_DEFAULT;
		}

		if( !pGetTokenInformation( hToken, TokenInformationClass, &szTokenInformation, dwTokenInformationLength, &dwReturnLength))
		{
			AddLog( _T( "ERROR: GetTokenInformation failed with error: %i\n"), GetLastError());
			CloseHandle( hExplorer );
			hExplorer = NULL;
			CloseHandle( hToken );
			hToken = NULL;
			CloseHandle( hSnapshot );
			hSnapshot = NULL;
			goto GETUSERNAME_DEFAULT;
		}

		if( !pLookupAccountSid( NULL, ((TOKEN_USER*)&szTokenInformation)->User.Sid, szUserName, &dwName, szReferencedDomainName, &dwReferencedDomainName, &peUse ) ){
			AddLog( _T( "ERROR: LookupAccountSid failed with error: %i\n"), GetLastError());
			CloseHandle( hExplorer );
			hExplorer = NULL;
			CloseHandle( hToken );
			hToken = NULL;
			CloseHandle( hSnapshot );
			hSnapshot = NULL;
			goto GETUSERNAME_DEFAULT;
		}
		CloseHandle( hExplorer );
		hExplorer = NULL;
		CloseHandle( hToken );
		hToken = NULL;
		CloseHandle( hSnapshot );
		hSnapshot = NULL;

		if( CString(szUserName) != _T( "") ){
			AddLog( _T( "User found (running process explorer): %s.\n"), szUserName);
			csUserName = szUserName;
			FreeLibrary( hAdv );
			hAdv = NULL;
			FreeLibrary( hPs );
			hPs = NULL;
			return TRUE;
		}
	}
	else{
		AddLog( _T( "ERROR: Explorer.exe not found. Will try with GetUserName..."));
	}
	// Call the GetUserName function.
GETUSERNAME_DEFAULT:
	if (hAdv != NULL)
		FreeLibrary( hAdv );
	if (hPs != NULL)
		FreeLibrary( hPs );
	if (GetUserName( szUserName, &dwUserName))
	{
		AddLog( _T( "User found (current thread): %s.\n"), szUserName);
		csUserName = szUserName;
		return TRUE;
	}
	// Last use registry
	AddLog( _T( "Failed !\n"));
	return m_registryInfo.GetLoggedOnUser( csUserName);	
}

BOOL CSysInfo::getLastLoggedUser(CString &csLastLoggedUser)
{
	// Use registry
	return m_registryInfo.GetLastLoggedUser( csLastLoggedUser);
}

BOOL CSysInfo::getRegistryApplications(CSoftwareList *pList, BOOL hkcu)
{
	// Use registry
	m_registryInfo.SetAddressWidthOS( getAddressWidthOS());
	return m_registryInfo.GetRegistryApplications( pList, hkcu);
}

BOOL CSysInfo::getDomainOrWorkgroup(CString &csDomain)
{
	// First, try WMI
	if (m_wmiInfo.GetDomainOrWorkgroup( csDomain))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetDomainOrWorkgroup( csDomain);
}

BOOL CSysInfo::getUserDomain(CString &csUserDomain)
{
	// Use registry
	return m_registryInfo.GetDomainOrWorkgroup( csUserDomain);
}

BOOL CSysInfo::getWindowsRegistration( CString &csCompany, CString &csOwner, CString &csProductID)
{
	// First, try WMI
	if (m_wmiInfo.GetWindowsRegistration( csCompany, csOwner, csProductID))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetWindowsRegistration( csCompany, csOwner, csProductID);
}

BOOL CSysInfo::getRegistryValue( UINT uKeyTree, LPCTSTR lpstrSubKey, LPCTSTR lpstrValue, CString &csResult)
{
	// Use registry
	return m_registryInfo.GetRegistryValue( uKeyTree, lpstrSubKey, lpstrValue, csResult);
}

BOOL CSysInfo::getRegistryMultipleValues( LPCTSTR lpstrQueryName, UINT uKeyTree, LPCTSTR lpstrSubKey, CRegistryValueList *pMyList)
{
	// Use registry
	return m_registryInfo.GetRegistryMultipleValues( lpstrQueryName, uKeyTree, lpstrSubKey, pMyList);
}

BOOL CSysInfo::getInputDevices( CInputDeviceList *pMyList)
{
	// First, try WMI
	if (m_wmiInfo.GetInputDevices( pMyList))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetInputDevices( pMyList);
}

BOOL CSysInfo::getWindowsProductKey(CString &productKey)
{
	// Use registry
	return m_registryInfo.GetWindowsProductKey( productKey );
}
