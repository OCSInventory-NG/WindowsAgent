//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// StoragePeripheral.h: interface for the CStoragePeripheral class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGEPERIPHERAL_H__6D006CB8_0A72_4960_93A3_89E3D1C9A400__INCLUDED_)
#define AFX_STORAGEPERIPHERAL_H__6D006CB8_0A72_4960_93A3_89E3D1C9A400__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

#define  STORAGE_MAX_LENGTH_SERIAL 20

class SYSINFO_API CStoragePeripheral  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CStoragePeripheral();
	virtual ~CStoragePeripheral();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetDescription();
	LPCTSTR GetModel();
	LPCTSTR GetName();
	LPCTSTR GetType();
	unsigned __int64 GetSize();
	LPCTSTR GetSizeString();
	LPCTSTR GetManufacturer();
	LPCTSTR GetSN();
	LPCTSTR GetFirmware();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetDescription( LPCTSTR lpstrDescription);
	void SetModel( LPCTSTR lpstrModel);
	// Is provided Model valid (check already stored Model if null)
	BOOL IsValidModel( LPCTSTR lpstrModel = NULL);
	void SetName( LPCTSTR lpstrName);
	void SetType( LPCTSTR lpstrType);
	void SetSize( unsigned __int64 u64Size);
	void SetManufacturer( LPCTSTR lpstrManufacturer);
	void SetSN( LPCTSTR lpstrSN);
	// Is provided serial number valid (check already stored SN if null)
	BOOL IsValidSN( LPCTSTR lpstrSN = NULL);
	void SetFirmware( LPCTSTR lpstrRev);
	// Is provided Firmware valid (check already stored Firmware if null)
	BOOL IsValidFirmware( LPCTSTR lpstrRev = NULL);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CStoragePeripheral cObject) const;
private:
	// Check wether a string is hex encoded or not
	BOOL is_hex( LPCTSTR lpstrString);
	// Check wether a string is printable or not
	BOOL is_printable( CString myString);

protected: // Attributes
	CString m_csType;			// Storage type (floppy, disk, tape...)
	CString m_csManufacturer;	// Manufacturer
	CString m_csName;			// Name
	CString m_csModel;			// Model
	CString m_csDescription;	// Description
	CString m_csSN;				// Serial number
	CString m_csFirmware;		// Firmware revision
	unsigned __int64 m_u64Size;	// Total size in MB
};
#endif // !defined(AFX_STORAGEPERIPHERAL_H__6D006CB8_0A72_4960_93A3_89E3D1C9A400__INCLUDED_)

