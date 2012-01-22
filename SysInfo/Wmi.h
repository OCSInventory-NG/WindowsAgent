//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Wmi.h: interface for the CWmi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WMI_H__73831499_4F80_4168_AB5B_6CB71A878DE1__INCLUDED_)
#define AFX_WMI_H__73831499_4F80_4168_AB5B_6CB71A878DE1__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CWmi  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CWmi();
	virtual ~CWmi();

	//////////////////////////////////
	// Connection methods
	//////////////////////////////////

	BOOL Connect( LPCTSTR lpstrDevice = NULL);
	BOOL IsConnected();
	BOOL Disconnect();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	BOOL GetBiosInfo( CBios *pMyBios);
	BOOL GetBios( CString &csVendor, CString &csVersion, CString &csDate, CString &csSN);
	BOOL GetSystemInformation( CString &csVendor, CString &csModel);
	BOOL GetBaseBoard( CString &csVendor, CString &csProduct, CString &csSN);
	BOOL GetSystemEnclosure( CString &csVendor, CString &csType, CString &csSN, CString &csAssetTag);
	BOOL GetOS( CString &csName, CString &csVersion, CString &csComment, CString &csDescription, CString &csInstallDate);
	DWORD GetAddressWidthOS();
	BOOL GetDomainOrWorkgroup(CString &csDomain);
	BOOL GetInputDevices( CInputDeviceList *pMyList);
	BOOL GetSystemPorts( CSystemPortList *pMyList);
	BOOL GetPrinters( CPrinterList *pMyList);
	DWORD GetProcessors( CString &csProcType, CString &csProcSpeed);
	DWORD GetCPU( CCpuList *pMyList, CRegistry *pReg);
	BOOL GetLogicalDrives( CLogicalDriveList *pMyList);
	BOOL GetMemorySlots( CMemorySlotList *pMyList);
	BOOL GetModems( CModemList *pMyList);
	BOOL GetMonitors( CMonitorList *pMyList);
	BOOL GetNetworkAdapters( CNetworkAdapterList *pMyList);
	BOOL GetSoundDevices( CSoundDeviceList *pMyList);
	BOOL GetStoragePeripherals( CStoragePeripheralList *pMyList);
	BOOL GetSystemControllers( CSystemControllerList *pMyList);
	BOOL GetSystemSlots( CSystemSlotList *pMyList);
	BOOL GetVideoAdapters( CVideoAdapterList *pMyList);
	BOOL GetWindowsRegistration( CString &csCompany, CString &csUser, CString &csSN);
	BOOL IsNotebook();
	BOOL GetHotFixes( CSoftwareList *pMyList);
	BOOL GetUUID( CString &csUUID);

protected: // Methods
	BOOL ParseChassisType( CString &csType);
	BOOL IsDeviceReallyConnected( DWORD dwStatus);
	LPCTSTR GetArchitecture( DWORD dwArch);
	LPCTSTR GetVoltage( DWORD dwVolts);

protected: // Attributes
	COcsWmi m_dllWMI;	// Class to access WMI
	BOOL m_bConnected;	// Do we are already connected to WMI namespace
};
#endif // !defined(AFX_WMI_H__73831499_4F80_4168_AB5B_6CB71A878DE1__INCLUDED_)

