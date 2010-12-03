//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CSysInfo.h : main header file for the CSysInfo DLL
//

#ifndef _SYSINFO_H_INCLUDED_
#define  _SYSINFO_H_INCLUDED_

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "OcsWmi.h"
#include "SetupApi.h"
#include "SysInfoClasses.h"
#include "AccessLog.h"
#include "Bios.h"
#include "FileVersion.h"
#include "InputDevice.h"
#include "InputDeviceList.h"
#include "LogicalDrive.h"
#include "LogicalDriveList.h"
#include "Memory.h"
#include "MemorySlot.h"
#include "MemorySlotList.h"
#include "Monitor.h"
#include "MonitorList.h"
#include "Modem.h"
#include "ModemList.h"
#include "NetworkAdapter.h"
#include "NetworkAdapterList.h"
#include "Printer.h"
#include "PrinterList.h"
#include "RegistryValue.h"
#include "RegistryValueList.h"
#include "Software.h"
#include "SoftwareList.h"
#include "StoragePeripheral.h"
#include "StoragePeripheralList.h"
#include "SoundDevice.h"
#include "SoundDeviceList.h"
#include "SystemController.h"
#include "SystemControllerList.h"
#include "SystemPort.h"
#include "SystemPortList.h"
#include "SystemSlot.h"
#include "SystemSlotList.h"
#include "VideoAdapter.h"
#include "VideoAdapterList.h"
#include "WindowsSocket.h"
#include "DeviceProperties.h"
#include "Wmi.h"
#include "EDID.h"
#include "Registry.h"
#include "DMI.h"

// CSysInfoApp
// See CSysInfo.cpp for the implementation of this class
//

class CSysInfoApp : public CWinApp
{
public:
	CSysInfoApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


class SYSINFO_API CSysInfo
{
public: // Methods
	BOOL getWindowsProductKey(CString &productKey);
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////

	// Create a CSysInfo object, enabling or not logging into lpstrFolder or current one
	// Log file will have <CSysInfo.log> name
	CSysInfo( BOOL bEnableLog = FALSE, LPCTSTR lpstrFolder = NULL);
	virtual ~CSysInfo();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	// OS information
	BOOL getOS( CString &csName, CString &csVersion, CString &csComment, CString &csDescription);
	DWORD getAddressWidthOS();
	BOOL getDomainOrWorkgroup( CString &csDomain);
	BOOL getUserDomain( CString &csUserDomain);

	// Memory information
	BOOL getMemory( ULONG *m_ulPhysicalMemory, ULONG *m_ulSwapSize);

	// Hardware informations
	BOOL getBiosInfo( CBios *pMyBios);
	BOOL getInputDevices( CInputDeviceList *pList);
	LPCTSTR getLocalIP();
	BOOL getLogicalDrives( CLogicalDriveList *pMyList);
	BOOL getMemorySlots( CMemorySlotList *pMyList);
	BOOL getModems( CModemList *pMyList);
	BOOL getMonitors( CMonitorList *pMyList);
	BOOL getNetworkAdapters( CNetworkAdapterList *pMyList);
	BOOL getSystemPorts( CSystemPortList *pMyList);
	BOOL getPrinters( CPrinterList *pMyList);
	DWORD getProcessors( CString &csProcType, CString &csProcSpeed);
	BOOL getSoundDevices( CSoundDeviceList *pMyList);
	BOOL getStoragePeripherals( CStoragePeripheralList *pMyList);
	BOOL getSystemControllers( CSystemControllerList *pMyList);
	BOOL getSystemSlots( CSystemSlotList *pMyList);
	BOOL getVideoAdapters( CVideoAdapterList *pMyList);
	BOOL getWindowsRegistration( CString &csCompany, CString &csOwner, CString &csProductID);
	BOOL isNotebook();

	// User informations
	BOOL getUserName( CString &csUserName);

	// Last user logged in the system
	BOOL getLastLoggedUser( CString &csLastLoggedUser);

	// Software informations
	BOOL getRegistryApplications( CSoftwareList *pList, BOOL hkcu);

	// Registry informations
	BOOL getRegistryValue( UINT uKeyTree, LPCTSTR lpstrSubKey, LPCTSTR lpstrValue, CString &csResult);
	BOOL getRegistryMultipleValues( LPCTSTR lpstrQueryName, UINT uKeyTree, LPCTSTR lpstrSubKey, CRegistryValueList *pMyList);

protected: // Attributes
	SYSTEM_INFO m_SystemInfo;	// System informations
	CWmi		m_wmiInfo;		// Class to get informations from WMI
	CDMI		m_dmiInfo;		// Class to get informations from SMBIOS/DMI
	CRegistry	m_registryInfo;	// Class to get informations from registry
	CWindowsSocket	m_Sock;		// Class to get informations from Windows Sockets
	DWORD		m_dwAddressWidth;// OS Address Width (32, 64 bits)
};

inline BOOL getFileVersion( LPCTSTR lpstrFile, CString &csPublisher, CString &csName, CString &csVersion, CString &csComment, CString &csLanguage)
{
	CFileVersion myFileVersion;

	try
	{
		if (!myFileVersion.Open( lpstrFile))
		{
			csPublisher = NOT_AVAILABLE;
			csName = NOT_AVAILABLE;
			csVersion = NOT_AVAILABLE;
			csComment = NOT_AVAILABLE;
			csLanguage = NOT_AVAILABLE;
			return FALSE;
		}
		csPublisher = myFileVersion.GetCompanyName();
		csPublisher.Replace( _T( "\r"), _T( " "));
		csName = myFileVersion.GetProductName();
		csName.Replace( _T( "\r"), _T( " "));
		if (csName.IsEmpty())
			csName = myFileVersion.GetInternalName();
		csVersion = myFileVersion.GetFileVersion();
		csVersion.Replace( _T( "\r"), _T( " "));
		csComment = myFileVersion.GetFileDescription();
		csComment.Replace( _T( "\r"), _T( " "));
		csLanguage = myFileVersion.GetProductLanguage();
		myFileVersion.Close();
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		csPublisher = NOT_AVAILABLE;
		csName = NOT_AVAILABLE;
		csVersion = NOT_AVAILABLE;
		csComment = NOT_AVAILABLE;
		csLanguage = NOT_AVAILABLE;
		return FALSE;
	}
	return TRUE;
}

#endif // _SYSINFO_H_INCLUDED_
