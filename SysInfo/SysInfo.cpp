//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
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
#include <VersionHelpers.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	return CWinApp::InitInstance();
}


CSysInfo::CSysInfo( BOOL bEnableLog, LPCTSTR lpstrFolder)
{
	if (bEnableLog)
		OpenLog( lpstrFolder);
	GetSystemInfo( &m_SystemInfo);
	m_wmiInfo.Connect();
	m_registryInfo.Connect();
	m_dwAddressWidth = 0;
}

CSysInfo::~CSysInfo()
{
	// Nothing to do yet
	m_wmiInfo.Disconnect();
	m_registryInfo.Disconnect();
	CloseLog();
}

BOOL CSysInfo::getOS( CString &csName, CString &csVersion, CString &csComment, CString &csDescription, CString &csInstallDate)
{
	if (m_wmiInfo.GetOS( csName, csVersion, csComment, csDescription, csInstallDate))
		// WMI successful
		return TRUE;

	AddLog( _T( "dtWinVer GetOS...\n"));

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
				if (os.IsWindows95(&osvi, FALSE))
				{
					csName = _T( "Microsoft Windows 95");
					csComment.Empty();
				}
				else if (os.IsWindows95SP1(&osvi, FALSE))
				{
					csName = _T( "Microsoft Windows 95");
					csComment = _T( "Service Pack 1");
				}
				else if (os.IsWindows95B(&osvi, FALSE))
				{
					csName = _T( "Microsoft Windows 95");
					csComment = _T( "Operating System Release 2.0/2.1 (B)");
				}
				else if (os.IsWindows95C(&osvi, FALSE))
				{
					csName = _T( "Microsoft Windows 95");
					csComment = _T( "Operating System Release 2.5 (C)");
				}
				else if (os.IsWindows98(&osvi, FALSE))
				{
					csName = _T( "Microsoft Windows 98");
					csComment.Empty();
				}
				else if (os.IsWindows98SP1(&osvi, FALSE))
				{
					csName = _T( "Microsoft Windows 98");
					csComment = _T( "Service Pack 1");
				}
				else if (os.IsWindows98SE(&osvi, FALSE))
				{
					csName = _T( "Microsoft Windows 98");
					csComment = _T( "Second Edition");
				}
				else if (os.IsWindowsME(&osvi, FALSE))
				{
					csName = _T( "Microsoft Windows Millennium");
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
				if (os.IsNTPreWin2k(&osvi, FALSE))
				{
					csName.Format( _T( "Microsoft Windows NT %d.%d"),
									osvi.dwUnderlyingMajorVersion,
									osvi.dwUnderlyingMinorVersion);

					if (os.IsNTWorkstation(&osvi, FALSE))
						csName += _T(" Workstation");
					else if (os.IsNTDatacenterServer(&osvi, FALSE))
						csName += _T(" Datacenter");
					else if (os.IsNTEnterpriseServer(&osvi, FALSE))
						csName += _T(" Enterprise");
					else 
						csName += _T(" Server");
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindows2000(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows 2000");

					if (os.IsWindows2000Professional(&osvi, FALSE))
						csName += _T(" Professional");
					else if (os.IsWindows2000DatacenterServer(&osvi, FALSE))
						csName += _T(" Datacenter");
					else if (os.IsWindows2000AdvancedServer(&osvi, FALSE))
						csName += _T(" Advanced Server");
					else 
						csName += _T(" Server");
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsXP(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows XP");

					if (os.IsWindowsXPPersonal(&osvi, FALSE))
						csName += _T(" Home Edition");                  
					else if (os.IsWindowsXPProfessional(&osvi, FALSE))
						csName += _T(" Professional");                  
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsServer2003(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Server 2003");

					if (os.IsDatacenterWindowsServer2003(&osvi, FALSE))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2003(&osvi, FALSE))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2003(&osvi, FALSE))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2003(&osvi, FALSE))
						csName += _T(" Standard Edition");
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsServer2003R2(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Server 2003 R2");

					if (os.IsDatacenterWindowsServer2003R2(&osvi, FALSE))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2003R2(&osvi, FALSE))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2003R2(&osvi, FALSE))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2003R2(&osvi, FALSE))
						csName += _T(" Standard Edition");
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsVista(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Vista");

					if (os.IsHomeBasic(&osvi))
						csName += _T(" Home Basic Edition");                  
					else if (os.IsHomeBasicPremium(&osvi))
						csName += _T(" Home Premium Edition");                  
					else if (os.IsBusiness(&osvi))
						csName += _T(" Business Edition");                  
					else if (os.IsProfessional(&osvi))
						csName += _T(" Professional Edition");                  
					else if (os.IsEnterprise(&osvi))
						csName += _T(" Enterprise Edition");                  
					else if (os.IsUltimate(&osvi))
						csName += _T(" Ultimate Edition");                  
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsServer2008(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Server 2008");

					if (os.IsDatacenterWindowsServer2008(&osvi, FALSE))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2008(&osvi, FALSE))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2008(&osvi, FALSE))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2008(&osvi, FALSE))
						csName += _T(" Standard Edition");
				}
				else if (os.IsWindows7(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows 7");

					if (os.IsHomeBasic(&osvi))
						csName += _T(" Home Basic Edition");                  
					else if (os.IsHomeBasicPremium(&osvi))
						csName += _T(" Home Premium Edition");                  
					else if (os.IsBusiness(&osvi))
						csName += _T(" Business Edition");                  
					else if (os.IsProfessional(&osvi))
						csName += _T(" Professional Edition");                  
					else if (os.IsEnterprise(&osvi))
						csName += _T(" Enterprise Edition");                  
					else if (os.IsUltimate(&osvi))
						csName += _T(" Ultimate Edition");                  
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsServer2008R2(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Server 2008 R2");

					if (os.IsDatacenterWindowsServer2008R2(&osvi, FALSE))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2008R2(&osvi, FALSE))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2008R2(&osvi, FALSE))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2008R2(&osvi, FALSE))
						csName += _T(" Standard Edition");
				}
				else if (os.IsWindows8(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows 8");

					if (os.IsHomeBasic(&osvi))
						csName += _T(" Home Basic Edition");                  
					else if (os.IsHomeBasicPremium(&osvi))
						csName += _T(" Home Premium Edition");                  
					else if (os.IsBusiness(&osvi))
						csName += _T(" Business Edition");                  
					else if (os.IsProfessional(&osvi))
						csName += _T(" Professional Edition");                  
					else if (os.IsEnterprise(&osvi))
						csName += _T(" Enterprise Edition");                  
					else if (os.IsUltimate(&osvi))
						csName += _T(" Ultimate Edition");                  
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsServer2012(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Server 2012");

					if (os.IsDatacenterWindowsServer2012(&osvi, FALSE))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2012(&osvi, FALSE))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2012(&osvi, FALSE))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2012(&osvi, FALSE))
						csName += _T(" Standard Edition");
				}
				else if (os.IsWindows8Point1(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Blue");

					if (os.IsHomeBasic(&osvi))
						csName += _T(" Home Basic Edition");                  
					else if (os.IsHomeBasicPremium(&osvi))
						csName += _T(" Home Premium Edition");                  
					else if (os.IsBusiness(&osvi))
						csName += _T(" Business Edition");                  
					else if (os.IsProfessional(&osvi))
						csName += _T(" Professional Edition");                  
					else if (os.IsEnterprise(&osvi))
						csName += _T(" Enterprise Edition");                  
					else if (os.IsUltimate(&osvi))
						csName += _T(" Ultimate Edition");                  
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWebWindowsServer2012(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Server Blue");

					if (os.IsDatacenterWindowsServer2012(&osvi, FALSE))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2012(&osvi, FALSE))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2012(&osvi, FALSE))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2012(&osvi, FALSE))
						csName += _T(" Standard Edition");
				}
				else if (os.IsWindows10(&osvi, FALSE)) {

					csName = _T("Microsoft Windows 10");

					if (os.IsHomeBasic(&osvi))
						csName += _T(" Home Basic Edition");
					else if (os.IsHomeBasicPremium(&osvi))
						csName += _T(" Home Premium Edition");
					else if (os.IsBusiness(&osvi))
						csName += _T(" Business Edition");
					else if (os.IsProfessional(&osvi))
						csName += _T(" Professional Edition");
					else if (os.IsEnterprise(&osvi))
						csName += _T(" Enterprise Edition");
					else if (os.IsUltimate(&osvi))
						csName += _T(" Ultimate Edition");
					csComment = osvi.szUnderlyingCSDVersion;
				}
				else if (os.IsWindowsServer2016(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Server 2016");

					if (os.IsDatacenterWindowsServer2016(&osvi, FALSE))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2016(&osvi, FALSE))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2016(&osvi, FALSE))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2016(&osvi, FALSE))
						csName += _T(" Standard Edition");
				}
				else if (os.IsWindowsServer2019(&osvi, FALSE))
				{
					csName = _T("Microsoft Windows Server 2019");

					if (os.IsDatacenterWindowsServer2019(&osvi, FALSE))
						csName += _T(" Datacenter Edition");
					else if (os.IsEnterpriseWindowsServer2019(&osvi, FALSE))
						csName += _T(" Enterprise Edition");
					else if (os.IsWebWindowsServer2019(&osvi, FALSE))
						csName += _T(" Web Edition");
					else if (os.IsStandardWindowsServer2019(&osvi, FALSE))
						csName += _T(" Standard Edition");
				}
/*				if (os.IsEmulated64Bit(&osvi))
					csComment += _T(" (64 Bits)");
*/				break;
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
		AddLog( _T( "\tFailed in call to GetVersion !\n"));
		csName = _T( "Unbknown OS");
		csVersion = NOT_AVAILABLE;
		csComment.Empty();
		return FALSE;
	}
	AddLog( _T( "\t\t<OS %s %s %s>\n\tOK\n"), csName, csVersion, csComment);
	m_registryInfo.GetDeviceDescription( csDescription);
	return TRUE;
}

BOOL CSysInfo::getMemory( ULONG *m_ulPhysicalMemory, ULONG *m_ulSwapSize)
{
	CMemory		memoryInfo;
	ULONGLONG	ulTotalRAM,
				ulTotalSwap;

	AddLog( _T( "getMemory...\n"));
	ulTotalRAM = memoryInfo.getTotalRAM() / ONE_MEGABYTE;
	*m_ulPhysicalMemory = (ULONG) ulTotalRAM;
	ulTotalSwap = memoryInfo.getTotalPageFile() / ONE_MEGABYTE;
	*m_ulSwapSize = (ULONG) ulTotalSwap;
	AddLog( _T( "\t\t<Physical: %I64u bytes><Swap: %I64u bytes>\n\tOK\n"), memoryInfo.getTotalRAM(), memoryInfo.getTotalPageFile());
	return TRUE;
}

LPCTSTR CSysInfo::getLocalIP()
{
	return m_Sock.GetIPAddress();
}

LPCTSTR CSysInfo::getLocalDNS()
{
	return CIPHelper::GetDefaultDNS();
}

LPCTSTR CSysInfo::getLocalGateway(CString csIpAdd)
{
	return CIPHelper::GetDefaultGateway(csIpAdd);
}

DWORD CSysInfo::getProcessors( CString &csProcType, CString &csProcSpeed, CCpuList *pMyList)
{
	static DWORD	dwRegNumber = 0,
					dwWmiNumber = 0;
	CCpu			myCpu;

	// First, check WMI combined to registry for bogus WMI CPU name
	if ((dwWmiNumber = m_wmiInfo.GetCPU( pMyList, &m_registryInfo)) > 0)	
	{
		myCpu = pMyList->GetHead();
		csProcType.Format( _T( "%s [%u core(s) %s]"), myCpu.GetName(), myCpu.GetNumberOfCores(), myCpu.GetArchitecture());
		csProcSpeed.Format( _T( "%u"), myCpu.GetMaxClockSpeed());
		return dwWmiNumber;
	}
	// Last, check only registry
	pMyList->RemoveAll();
	if ((dwRegNumber = m_registryInfo.GetCPU( pMyList)) > 0)
	{
		myCpu = pMyList->GetHead();
		csProcType.Format( _T( "%s [%u core(s) %s]"), myCpu.GetName(), myCpu.GetNumberOfCores(), myCpu.GetArchitecture());
		csProcSpeed.Format( _T( "%u"), myCpu.GetMaxClockSpeed());
	}
	else
	{
		csProcType = NOT_AVAILABLE;
		csProcSpeed = NOT_AVAILABLE;
	}
	return dwRegNumber;
}

DWORD CSysInfo::getCPU( CCpuList *pMyList)
{
	return m_wmiInfo.GetCPU( pMyList, &m_registryInfo);
}

DWORD CSysInfo::getAddressWidthOS()
{
	if (m_dwAddressWidth == 0)
		m_dwAddressWidth = m_wmiInfo.GetAddressWidthOS();
	return m_dwAddressWidth;
}

BOOL CSysInfo::getSystemPorts( CSystemPortList *pMyList)
{
	// First, try WMI
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
	CMemory		memoryInfo;
	ULONGLONG	ulTotalSlot, ulTotalRAM;

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
	return m_wmiInfo.GetSystemSlots( pMyList);
}

BOOL CSysInfo::getBiosInfo( CBios *pMyBios)
{
	// First, try WMI
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


BOOL CSysInfo::getUserName(CString &csUserName, tag_WBEM_GENERIC_FLAG_TYPE m_uFlag)
{
	TCHAR szUserName[255]; 
	DWORD dwUserName = 255; 

	// First, try to find user running explorer.exe process
	if (getUserNameFromExplorerProcess( csUserName))
		return TRUE;
	/* Then, try WMI
	May sometimes produce 100% CPU during many minutes

	Install these patchs from Microsoft solves the problem:
		For windows 2003 SP2: http://support.microsoft.com/kb/2257980/en
		For Winows 2008 : http://support.microsoft.com/kb/2464876/en
		For windows 2008 R2: http://support.microsoft.com/kb/2465990/en

	Anyway, I advise to run all this KB to fix WMI performance or errors:
	http://support.microsoft.com/kb/2591403

	See https://bugs.launchpad.net/ocsinventory-windows-agent/+bug/1300172
	*/
	if (m_wmiInfo.IsConnected() && m_wmiInfo.GetLoggedOnUser( csUserName, m_uFlag))
		return TRUE;
	// Call the GetUserName function.
	AddLog( _T( "GetUserName: Trying to find logged on User ID from current running process...\n"));
	if (GetUserName( szUserName, &dwUserName))
	{
		AddLog( _T( "\t\t<User: %s>\n\tOK.\n"), szUserName);
		csUserName = szUserName;
		return TRUE;
	}
	AddLog( _T( "\tFailed with error <%i> !\n"), GetLastError());
	// Last use registry
	return m_registryInfo.GetUser(csUserName);
}

BOOL CSysInfo::getUserNameFromExplorerProcess(CString &csUserName)
{
	TCHAR szUserName[255]; 
	DWORD dwName = 255; 
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof( PROCESSENTRY32 );
	HANDLE hToken;
	TOKEN_INFORMATION_CLASS TokenInformationClass = TokenUser;
	TCHAR szTokenInformation[255];
	DWORD dwTokenInformationLength = 255;//sizeof( TOKEN_OWNER );
	DWORD dwReturnLength=0;
	DWORD dwReferencedDomainName = 255;
	TCHAR szReferencedDomainName[255];
	SID_NAME_USE peUse;
	HANDLE hExplorer = NULL;
	HANDLE hSnapshot = NULL;
	HMODULE hAdv = NULL;
	BOOL bExplorerFound = FALSE;

	HANDLE	(WINAPI* pCreateToolhelp32Snapshot)	(DWORD, DWORD) = NULL; 
	BOOL	(WINAPI* pProcess32First)			(HANDLE, LPPROCESSENTRY32) = NULL; 
	BOOL	(WINAPI* pProcess32Next)			(HANDLE, LPPROCESSENTRY32)  = NULL; 
	HANDLE	(WINAPI* pOpenProcess)				(DWORD, BOOL, DWORD) = NULL; 
	BOOL	(WINAPI* pOpenProcessToken)			(HANDLE, DWORD, PHANDLE) = NULL; 
	BOOL	(WINAPI* pLookupAccountSid)			(LPCTSTR, PSID, LPTSTR, LPDWORD, LPTSTR, LPDWORD, PSID_NAME_USE ) = NULL; 
	BOOL	(WINAPI* pGetTokenInformation)		(HANDLE, TOKEN_INFORMATION_CLASS, LPVOID, DWORD, PDWORD) = NULL;

	AddLog( _T( "getUserNameFromExplorerProcess: Trying to find logged on User ID from <explorer.exe> process...\n"));

	// First, try to use Advapi.dll
	if( !(hAdv = LoadLibrary( _T( "Advapi32.dll"))))
	{
		AddLog( _T( "\tFailed to load AdvApi32 library !\n"));	
		return FALSE;
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
		AddLog( _T( "\tFailed to load AdvApi32 library with error <%i> !\n"), GetLastError());
		FreeLibrary( hAdv);
		return FALSE;
	}
	// Try to use kernel32 to enum process
	if(  !(*(FARPROC*)&pCreateToolhelp32Snapshot	= GetProcAddress( GetModuleHandle( _T("KERNEL32.DLL")), "CreateToolhelp32Snapshot") ) ||
#ifdef _UNICODE
		 !(*(FARPROC*)&pProcess32First			= GetProcAddress( GetModuleHandle( _T("KERNEL32.DLL")), "Process32FirstW") ) ||
		 !(*(FARPROC*)&pProcess32Next			= GetProcAddress( GetModuleHandle( _T("KERNEL32.DLL")), "Process32NextW") ) ) 
#else
		 !(*(FARPROC*)&pProcess32First			= GetProcAddress( GetModuleHandle( _T("KERNEL32.DLL")), "Process32First") ) ||
		 !(*(FARPROC*)&pProcess32Next			= GetProcAddress( GetModuleHandle( _T("KERNEL32.DLL")), "Process32Next") ) ) 
#endif
	{
		AddLog( _T( "\tFailed to load Kernel32 process access functions with error <%i> !\n"), GetLastError());	
		FreeLibrary( hAdv);
		return FALSE;
	}
	// Create snapshot of running processes
	if( (hSnapshot = pCreateToolhelp32Snapshot( TH32CS_SNAPALL ,0 )) == INVALID_HANDLE_VALUE )
	{
		AddLog( _T( "\tCreateToolhelp32Snapshot failed with error <%i> !\n"), GetLastError());
		FreeLibrary( hAdv);
		return FALSE;
	}
	// Trying to find explorer.exe in snapshot
	if( !pProcess32First( hSnapshot, &pe) )
	{
		AddLog( _T( "\tProcess32First failed with error <%i> !\n"), GetLastError());
		CloseHandle( hSnapshot );
		FreeLibrary( hAdv);
		return FALSE;
	}
	do
	{
		if( !CString(pe.szExeFile).CompareNoCase( _T( "explorer.exe")))
		{
			bExplorerFound = TRUE;
			break;
		}
		pe.dwSize = sizeof( PROCESSENTRY32 );
	}
	while (pProcess32Next( hSnapshot, &pe ));
	if (!bExplorerFound)
	{
		AddLog( _T( "\tCould not find <explorer.exe> process !\n"));
		CloseHandle( hSnapshot );
		FreeLibrary( hAdv);
		return FALSE;
	}
	// Retrieve a handle on explorer.exe process using ID */
	if( !(hExplorer = pOpenProcess( PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID )))
	{
		AddLog( _T( "\tFailed to open <explorer.exe> process with error <%i> !\n"), GetLastError());
		CloseHandle( hSnapshot );
		FreeLibrary( hAdv);
		return FALSE;
	}
	// Open token associated to explorer.exe to get information
	if( !pOpenProcessToken( hExplorer, TOKEN_READ, &hToken ) )
	{
		AddLog( _T( "\tOpenProcessToken failed with error <%i>\n"), GetLastError());
		CloseHandle( hExplorer );
		CloseHandle( hToken );
		CloseHandle( hSnapshot );
		FreeLibrary( hAdv);
		return FALSE;
	}
	if( !pGetTokenInformation( hToken, TokenInformationClass, &szTokenInformation, dwTokenInformationLength, &dwReturnLength))
	{
		AddLog( _T( "\tGetTokenInformation failed with error <%i>\n"), GetLastError());
		CloseHandle( hExplorer );
		CloseHandle( hToken );
		CloseHandle( hSnapshot );
		FreeLibrary( hAdv);
		return FALSE;
	}
	// Lookup user account running explorer.exe process
	if( !pLookupAccountSid( NULL, ((TOKEN_USER*)&szTokenInformation)->User.Sid, szUserName, &dwName, szReferencedDomainName, &dwReferencedDomainName, &peUse ) )
	{
		AddLog( _T( "\tLookupAccountSid failed with error <%i>\n"), GetLastError());
		CloseHandle( hExplorer );
		CloseHandle( hToken );
		CloseHandle( hSnapshot );
		FreeLibrary( hAdv);
		return FALSE;
	}
	CloseHandle( hExplorer );
	CloseHandle( hToken );
	CloseHandle( hSnapshot );
	FreeLibrary( hAdv );
	// Ensure username exists
	if( CString(szUserName) == _T( "") )
	{
		AddLog( _T( "\tFound empty user, so assuming failed !\n"));
		return FALSE;
	}
	AddLog( _T( "\t\t<User: %s>\n\tOK\n"), szUserName);
	csUserName = szUserName;
	return TRUE;
}

BOOL CSysInfo::getLastLoggedUser(CString &csLastLoggedUser)
{
	// Use registry
	return m_registryInfo.GetLastLoggedUser( csLastLoggedUser);
}

BOOL CSysInfo::getInstalledApplications(CSoftwareList *pList, BOOL hkcu)
{
	BOOL			bIsNT6orHigher = FALSE;

	if (IsWindowsVistaOrGreater())
	{
		bIsNT6orHigher = TRUE;
	}
	// Use registry
	m_registryInfo.SetAddressWidthOS( getAddressWidthOS());
	if (bIsNT6orHigher)
	{
		// Under Vista, 2008 or higher, Registry does not include Hotfix
		// We have to also query WMI for Hotfixes
		if (m_registryInfo.GetRegistryApplications( pList, hkcu))
		{
			// Don't care about hotfix
			m_wmiInfo.GetHotFixes( pList);
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		// 2000/XP/2003, Hotfixes are in the registry
		return m_registryInfo.GetRegistryApplications( pList, hkcu);
	}
}

BOOL CSysInfo::getDomainOrWorkgroup(CString &csDomain)
{
	// First, try WMI
	if (m_wmiInfo.GetDomainOrWorkgroup( csDomain))
		return TRUE;
	// Last, use registry
	return m_registryInfo.GetDomainOrWorkgroup( csDomain);
}

BOOL CSysInfo::getUserDomain(CString &csUserDomain, tag_WBEM_GENERIC_FLAG_TYPE m_uFlag)
{
	// First, try WMI
	if (m_wmiInfo.GetUserDomain( csUserDomain, m_uFlag))
		return TRUE;
	// Last, use registry
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

BOOL CSysInfo::getUUID( CString &csUUID)
{
	// Use WMI to retrieve UUID
	return m_wmiInfo.GetUUID( csUUID);
}
