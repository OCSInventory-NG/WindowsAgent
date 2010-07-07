//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// StoragePeripheralList.h: interface for the CStoragePeripheralList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGEPERIPHERALLIST_H__21D2FFA5_E1B2_460A_AA3E_56712989CAC3__INCLUDED_)
#define AFX_STORAGEPERIPHERALLIST_H__21D2FFA5_E1B2_460A_AA3E_56712989CAC3__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CStoragePeripheralList : public CList<CStoragePeripheral, CStoragePeripheral&>  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CStoragePeripheralList();
	virtual ~CStoragePeripheralList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_STORAGEPERIPHERALLIST_H__21D2FFA5_E1B2_460A_AA3E_56712989CAC3__INCLUDED_)

