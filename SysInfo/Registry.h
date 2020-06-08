//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Registry.h: interface for the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRY_H__599ACCD2_83FA_4ED3_9006_8C315E043D08__INCLUDED_)
#define AFX_REGISTRY_H__599ACCD2_83FA_4ED3_9006_8C315E043D08__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

#define HIVE_WOW32_32KEY	0
#define HIVE_WOW64_32KEY	1
#define HIVE_WOW64_64KEY	2

class SYSINFO_API CRegistry  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CRegistry();
	virtual ~CRegistry();

	//////////////////////////////////
	// Connection methods
	//////////////////////////////////

	BOOL Connect( LPCTSTR lpstrDevice = NULL);
	void SetAddressWidthOS( DWORD dwAddressWidth);
	BOOL Disconnect();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	BOOL GetBiosInfo( CBios *pMyBios);
	BOOL GetDeviceDescription( CString &csDescription);
	BOOL GetDomainOrWorkgroup( CString &csDomain);
	BOOL GetInputDevices( CInputDeviceList *pList);
	BOOL GetLoggedOnUser( CString &csUser);
	BOOL GetLastLoggedUser(CString &csLastLoggedUser);
	BOOL GetModems( CModemList *pMyList);
	BOOL GetMonitors( CMonitorList *pMyList);
	BOOL GetSystemPorts( CSystemPortList *pMyList);
	BOOL GetPrinters( CPrinterList *pMyList);
	DWORD GetProcessors( CString &csProcType, CString &csProcSpeed);
	DWORD GetCPU( CCpuList *pMyList);
	BOOL GetCPUName( UINT uCPUID, CString &csName);
	BOOL GetRegistryApplications( CSoftwareList *pList, BOOL hkcu);
	BOOL GetSoundDevices( CSoundDeviceList *pMyList);
	BOOL GetStoragePeripherals( CStoragePeripheralList *pMyList);
	BOOL GetSystemControllers( CSystemControllerList *pMyList);
	BOOL GetVideoAdapters( CVideoAdapterList *pMyList);
	BOOL GetWindowsRegistration( CString &csCompany, CString &csOwner, CString &csProductID);
	BOOL GetWindowsProductKey( CString &csProductKey );
	BOOL IsNotebook();
	BOOL GetRegistryValue( UINT uKeyTree, LPCTSTR lpstrSubKey, LPCTSTR lpstrValue, CString &csResult);
	BOOL GetRegistryMultipleValues(LPCTSTR lpstrQueryName, UINT uKeyTree, LPCTSTR lpstrSubKey, CRegistryValueList *pMyList);
	BOOL GetUser(CString &csUser);

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

protected: // Methods
	BOOL GetLoggedOnUserNT( CString &csUser);
	BOOL GetLoggedOnUserVista( CString &csUser);
	BOOL GetLastLoggedUserNT(CString &csLastLoggedUser);
	BOOL GetLastLoggedUserVista(CString &csLastLoggedUser);
	BOOL GetDeviceDescriptionNT( CString &csDescription);
	BOOL GetDomainOrWorkgroupNT( CString &csDomain);
	BOOL GetBiosInfoNT( CBios *pMyBios);
	BOOL GetBiosInfoVista( CBios *pMyBios);
	BOOL GetInputDevicesNT( CInputDeviceList *pList);
	BOOL GetModemsNT( CModemList *pMyList);
	BOOL GetMonitorsNT( CMonitorList *pList);
	BOOL GetSystemPortsNT( CSystemPortList *pMyList);
	DWORD GetProcessorsNT( CString &csProcType, CString &csProcSpeed);
	BOOL GetRegistryApplicationsNT( CSoftwareList *pList, HKEY hHive, UINT uHiveType = HIVE_WOW32_32KEY);
	BOOL GetSoundDevicesNT( CSoundDeviceList *pList);
	BOOL GetStoragePeripheralsNT( CStoragePeripheralList *pMyList);
	BOOL GetSystemControllersNT( CSystemControllerList *pMyList);
	BOOL GetVideoAdaptersNT_2K( CVideoAdapterList *pMyList);
	BOOL GetVideoAdaptersXP( CVideoAdapterList *pMyList);
	BOOL GetWindowsRegistrationNT( CString &csCompany, CString &csOwner, CString &csProductID);
	BOOL IsNotebookNT();
	// Get screen resolution
	BOOL GetScreenResolution( CString &csResolution);
	// Parse multi string registry value
	LPCTSTR ParseMultiSZ( LPCTSTR lpstrCurrent = NULL);
	// Get data from registry value (same return as RegQueryValueEx)
	LONG GetValue( HKEY hKey, LPCTSTR lpstrValue, CString &csData);
	LONG GetValue( HKEY hKey, LPCTSTR lpstrValue, DWORD *dwData);

protected: // Attributes
	BOOL m_bRemoteRegistry;	// TRUE if connecting to a remote computer
	HKEY m_hKey;			// Handle to HKLM key on computer
	DWORD m_dwPlatformId;	// OS Platform ID to determine if Win9X or NT
	BOOL  m_bIsXP;			// TRUE if Win XP or higher
	BOOL  m_bIsVista;		// TRUE if Win Vista or higher
	DWORD m_dwAddressWidth; // OS Address Width (32, 64 bits)

};
#endif // !defined(AFX_REGISTRY_H__599ACCD2_83FA_4ED3_9006_8C315E043D08__INCLUDED_)

