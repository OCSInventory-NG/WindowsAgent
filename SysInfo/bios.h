//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// BIOSInfo.h: interface for the CBios class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BIOS_H__0769B90F_185D_424A_A8E9_121705E35122__INCLUDED_)
#define AFX_BIOS_H__0769B90F_185D_424A_A8E9_121705E35122__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CBios  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CBios();
	virtual ~CBios();

	static LPCTSTR ParseChassisType( DWORD dwType);

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	// Return the System manufacturer
	LPCTSTR GetSystemManufacturer();
	// Return the System model
	LPCTSTR GetSystemModel();
	// Return the System S/N
	LPCTSTR GetSystemSerialNumber();
	// Return the BIOS machine type
	LPCTSTR GetMachineType();
	// Return the BIOS manufacturer
	LPCTSTR GetBiosManufacturer();
	// Return the BIOS Version
	LPCTSTR GetBiosVersion();
	// Return the BIOS Date
	LPCTSTR GetBiosDate();
	// Return the Motherboard manufacturer
	LPCTSTR GetMotherboardManufacturer();
	// Return the Motherboard model
	LPCTSTR GetMotherboardModel();
	// Return the Motherboard S/N
	LPCTSTR GetMotherboardSerialNumber();
	// Return the Asset Tag
	LPCTSTR GetAssetTag();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();

	BOOL IsValidSystemManufacturer();
	BOOL IsValidSystemModel();
	BOOL IsValidSystemSerialNumber();
	BOOL IsValidAssetTag();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	// Clear BIOS info
	void Clear();
	// Set all BIOSInfo attributes
	void Set( LPCTSTR lpstrSystemManufacturer, LPCTSTR lpstrSystemModel, LPCTSTR lpstrSystemSerialNumber, LPCTSTR lpstrMachineType,
			  LPCTSTR lpstrBiosManufacturer, LPCTSTR lpstrBiosVersion, LPCTSTR lpstrBiosDate,
			  LPCTSTR lpstrMotherboardManufacturer, LPCTSTR lpstrMotherboardModel, LPCTSTR lpstrMotherboardSerialNumber);
	// Set System manufacturer
	void SetSystemManufacturer( LPCTSTR lpstrManufacturer);
	// Set System model
	void SetSystemModel( LPCTSTR lpstrModel);
	// Set System serial number
	void SetSystemSerialNumber( LPCTSTR lpstrSerialNumber);
	// Set Motherboard manufacturer
	void SetMotherboardManufacturer( LPCTSTR lpstrManufacturer);
	// Set Motherboard model
	void SetMotherboardModel( LPCTSTR lpstrModel);
	// Set Motherboard serial number
	void SetMotherboardSerialNumber( LPCTSTR lpstrSerialNumber);
	// Set BIOS machine type
	void SetMachineType( LPCTSTR lpstrMachineType);
	// Set BIOS manufacturer
	void SetBiosManufacturer( LPCTSTR lpstrManufacturer);
	// Set BIOS version
	void SetBiosVersion( LPCTSTR lpstrBiosVersion);
	// Set BIOS date
	void SetBiosDate( LPCTSTR lpstrBiosDate);
	void SetBiosDateFormat( LPCTSTR lpstrBiosDate);
	// Set ASSET TAG
	void SetAssetTag( LPCTSTR lpstrTag);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CBios cObject) const;

protected: // Attributes
	CString		m_csSystemManufacturer;	// Device manufacturer
	CString		m_csSystemModel;		// Device model
	CString		m_csSystemSerialNumber;	// Device serial number
	CString		m_csMotherboardManufacturer; // Motherboard manufacturer
	CString		m_csMotherboardModel;	// Motherboard model
	CString		m_csMotherboardSerialNumber; // Motherboard serial number
	CString		m_csMachineType;		// Device type (tower, mini-tower, pizza box...)
	CString		m_csBiosManufacturer;	// BIOS manufacturer
	CString		m_csBiosVersion;		// BIOS version
	CString		m_csBiosDate;			// BIOS Date
	CString		m_csAssetTag;			// Asset Tag
};
#endif // !defined(AFX_BIOS_H__0769B90F_185D_424A_A8E9_121705E35122__INCLUDED_)

