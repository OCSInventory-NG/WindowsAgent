//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#pragma once

#include "SysInfoClasses.h"

#define VMSYSTEM_PHYSYCAL	_T( "Physical")

class SYSINFO_API CVMSystem
{
public:
	CVMSystem(void);
	~CVMSystem(void);

	// Try to detect hypervisor we're running on using Bios
	BOOL DetectVM( CBios *pBios, CString &csVMSystem);
	// Try to detect hypervisor we're running on using System controller
	BOOL DetectVM( CSystemControllerList *pMyList, CString &csVMSystem);
	// Try to detect hypervisor we're running on using Storage Peripherals
	BOOL DetectVM( CStoragePeripheralList *pMyList, CString &csVMSystem);
	// Try to detect hypervisor we're running on using Video Adapter
	BOOL DetectVM( CVideoAdapterList *pMyList, CString &csVMSystem);
protected:
	// Try to find Hypervisor signature into a string
	BOOL FindHypervisor( LPCTSTR lpstrValue, CString &csVMSystem);
};
