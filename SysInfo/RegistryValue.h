//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// RegistryValue.h: interface for the CRegistryValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREGISTRYVALUE_H__D469D12F_9CA7_4E5B_9553_BA13FAF327E5__INCLUDED_)
#define AFX_DREGISTRYVALUE_H__D469D12F_9CA7_4E5B_9553_BA13FAF327E5__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

#define HKCR_TREE	0	// HKEY_CLASSES_ROOT
#define HKCU_TREE	1	// HKEY_CURRENT_USER
#define HKLM_TREE	2	// HKEY_LOCAL_MACHINE
#define HKU_TREE	3	// HKEY_USERS
#define HKCC_TREE	4	// HKEY_CURRENT_CONFIG
#define HKDD_TRE	5	// HKEY_DYN_DATA (9X only)

#define REGISTRY_ALL_VALUES_OF_KEY	_T( "*") // Get all values of a key

class SYSINFO_API CRegistryValue  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CRegistryValue();
	virtual ~CRegistryValue();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetName();
	LPCTSTR GetValue();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void Set( LPCTSTR lpstrName, LPCTSTR lpstrValue);
	void SetValue( LPCTSTR lpstrValue);
	void SetName( LPCTSTR lpstrName);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CRegistryValue cObject) const;

protected: // Attributes
	CString m_csName;		// Registry value name
	CString m_csValue;		// Registry value
};
#endif // !defined(AFX_DREGISTRYVALUE_H__D469D12F_9CA7_4E5B_9553_BA13FAF327E5__INCLUDED_)

