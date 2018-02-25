//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DeviceProperties.h: interface for the CDeviceProperties class.
//
// Class storing informations about Hardware and OS of a device
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICEPROPERTIES_H__02015421_D9B0_11D4_8F75_00600889DFA5__INCLUDED_)
#define AFX_DEVICEPROPERTIES_H__02015421_D9B0_11D4_8F75_00600889DFA5__INCLUDED_

#pragma once

#include "SysInfoClasses.h"
class SysInfo;

class SYSINFO_API CDeviceProperties  
{
// Methods
public:
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CDeviceProperties();
	virtual ~CDeviceProperties();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	// Return a string with the device unique ID
	LPCTSTR GetDeviceID();
	// Return a string with the device NetBIOS name
	LPCTSTR GetDeviceName();
	// Return a string with the operating system name
	LPCTSTR GetOSName();
	// Return a string with the operating system version
	LPCTSTR GetOSVersion();
	// Return a string with the operating system comments
	LPCTSTR GetOSComment();
	// Return TRUE if 64 bits OS
	BOOL Is64bitsOS();
	// Return a string with the processor type
	LPCTSTR GetProcessorType();
	// Return a string with the processor speed
	LPCTSTR GetProcessorSpeed();
	// Return the number of processors
	DWORD GetNumberOfProcessors();
	// Return the physical memory size in mega bytes
	ULONG GetPhysicalMemory();
	// Return the paging file size in mega bytes
	ULONG GetPageFileSize();
	// Return a string with the network primary IP address
	LPCTSTR GetIPAddress();
	// Return a string with the last check date
	LPCTSTR GetLastCheckDate();
	// Return a string with the execution duration
	LPCTSTR GetExecutionDuration();
	// Return a string with the logged on user when device has been checked
	LPCTSTR GetLoggedOnUser();
	// Return a string with the last user who'd been logged in the system
	LPCTSTR GetLastLoggedUser();
	// Return a string with the device description extracted from OS
	LPCTSTR GetDescription();
	// Return the NT domain name or workgroup
	LPCTSTR GetDomainOrWorkgroup();
	// return the NT user domain 
	LPCTSTR GetUserDomain();
	// Return Windows registered company
	LPCTSTR GetWindowsRegisteredCompany();
	// Return Windows registered owner
	LPCTSTR GetWindowsRegisteredOwner();
	// Return Windows product ID
	LPCTSTR GetWindowsProductID();
	// Return Windows product key
	LPCTSTR GetWindowsProductKey();
	// Get computer or VM UUID
	LPCTSTR GetUUID();
	// Get name of hypervisor we're running on (Physical if not)
	LPCTSTR GetVMSystem();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
	// Get current checksum
	ULONG GetChecksum();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	// Set the device unique ID
	void SetDeviceID( LPCTSTR lpstrDeviceID);
	// Set the device NetBIOS name
	void SetDeviceName( LPCTSTR lpstrName);
	// Set the operating system informations
	void SetOS( LPCTSTR lpstrName, LPCTSTR lpstrVersion, LPCTSTR lpstrComment);
	// Set OS memory addressing bits (32 bits, 64 bits...)
	void SetAddressWidthOS( UINT uBits = 32);
	// Set the processors informations
	void SetProcessor( LPCTSTR lpstrName, LPCTSTR lpstrSpeed, DWORD dwNumber);
	// Set the physical and paging file size in mega bytes
	void SetMemory( ULONG ulPhysical, ULONG ulPageFile);
	// Set the IP Address
	void SetIPAddress( LPCTSTR lpstrIP);
	// Set the last check date
	void SetLastCheckDate( LPCTSTR lpstrDate);
	// Compute and store the execution duration
	void SetExecutionDuration( CTime &cBeginTime, CTime &cEndTime);
	// Set the execution duration from string
	void SetExecutionDuration( LPCTSTR lpstrDuration);
	// Set the user
	void SetLoggedOnUser( LPCTSTR lpstrUser);
	// Set the last user who'd been logged in
	void SetLastLoggedUser( LPCTSTR lpstrLastLoggedUser);
	// Set the device description extracted from OS
	void SetDescription( LPCTSTR lpstrDescription);
	// Set the NT Domain name or workgroup
	void SetDomainOrWorkgroup( LPCTSTR lpstrDomain);
	// Set the NT User Domain
	void SetUserDomain( LPCTSTR lpstrUserDomain);
	// Set informations about Windows registration
	void SetWindowsRegistration( LPCTSTR lpstrCompany, LPCTSTR lpstrOwner, LPCTSTR lpstrProductID);
	// Set informations about Windows key
	void SetWindowsProductKey( LPCTSTR lpstrProductKey);
	// Set computer or VM UUID
	void SetUUID( LPCTSTR lpstrUUID);
	// Set name of hypervisor we're running on
	void SetVMSystem( LPCTSTR lpstrVM);
	// Set checksum
	void SetChecksum( ULONG ulchecksum);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==(CDeviceProperties &cObjet);

protected: // Attributes
	CString	m_csDeviceID;		// Device unique ID
	CString	m_csDeviceName;		// Device netbios or DNS name
	CString	m_csOSName;			// Operating system name (ex "Windows NT")
	CString	m_csOSVersion;		// OS Version  (ex "4.0 Build 1381")
	CString	m_csOSComment;		// OS Comment (ex "Service Pack 6")
	UINT	m_uBitsOS;			// OS memory addressing bits ( 32, 64, 128...)
	CString	m_csProcessorType;	// First Processor type of the device (ex "Intel Pentium II Model 1280 Stepping 2")
	CString	m_csProcessorSpeed; // Processor speed
	DWORD	m_dwNumberOfProcessor; // Number of processor of the device
	ULONG	m_ulPhysicalMemory;	// Physical memory of the device
	ULONG	m_ulSwapSize;		// Swap Size of the device
	CString	m_csIPAddress;		// IP Address of the device if available (ex "192.3.4.1" or "Unavailable")
	CString	m_csLastCheckDate;	// Last check date when reading from CSV
	CString	m_csExecutionDuration; // Execution duration in string format
	CString	m_csLoggedOnUser;	// Logged on username when device has been checked
	CString m_csLastLoggedUser; // Last user who'd been logged in
	CString	m_csDescription;	// Description extracted from OS
	CString	m_csDomain;			// NT Domain name or workgroup
	CString	m_csUserDomain		;// NT user Domain name 
	CString	m_csWinRegCompany;	// Windows registered company
	CString	m_csWinRegOwner;	// Windows registered owner
	CString m_csWinRegProductID;// Windows product ID
	CString m_csWinProductKey;	// Windows product key
	CString m_csUUID;			// Computer of VM UUID
	CString m_csVMSystem;		// Physical or hypervisor we're running on
	ULONG	m_ulChecksum;		// Checksum for detecting changes
};
#endif // !defined(AFX_DEVICEPROPERTIES_H__02015421_D9B0_11D4_8F75_00600889DFA5__INCLUDED_)

