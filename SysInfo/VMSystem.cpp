//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#include "stdafx.h"
#include "SysInfo.h"
#include "VMSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CVMSystem::CVMSystem(void)
{
}

CVMSystem::~CVMSystem(void)
{
}

BOOL CVMSystem::FindHypervisor( LPCTSTR lpstrValue, CString &csVMSystem)
{
	ASSERT( lpstrValue);
	CString csSearch = lpstrValue;

	if (csSearch.Find( _T( "VMware")) >= 0)
	{
		csVMSystem = _T( "VMware");
		return TRUE;
	}
	if (csSearch.Find( _T( "Xen")) >= 0)
	{
		csVMSystem = _T( "Xen");
		return TRUE;
	}
	if (csSearch.Find( _T( "QEMU")) >= 0)
	{
		csVMSystem = _T( "QEMU");
		return TRUE;
	}
	if ((csSearch.Find( _T( "VirtualBox")) >= 0) ||
		(csSearch.Find( _T( "VBOX")) >= 0))
	{
		csVMSystem = _T( "VirtualBox");
		return TRUE;
	}
	if ((csSearch.Find( _T( "Virtual Machine")) >= 0) ||
		(csSearch.Find( _T( "Virtual HD")) >= 0) ||
		(csSearch.Find( _T( "Virtual CD")) >= 0))
	{
		csVMSystem = _T( "Virtual Machine");
		return TRUE;
	}
	return FALSE;
}

BOOL CVMSystem::DetectVM( CBios *pBios, CString &csVMSystem)
{
	ASSERT( pBios);

	// First, search in System Manufacturer
	if (FindHypervisor( pBios->GetSystemManufacturer(), csVMSystem))
		return TRUE;
	// Next, search in system Model
	if (FindHypervisor( pBios->GetSystemModel(), csVMSystem))
		return TRUE;
	// Last, search in Bios manufacturer
	if (FindHypervisor( pBios->GetBiosManufacturer(), csVMSystem))
		return TRUE;
	return FALSE;
}


BOOL CVMSystem::DetectVM( CSystemControllerList *pMyList, CString &csVMSystem)
{
	CSystemController myObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		pos = pMyList->GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			myObject = pMyList->GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			if (FindHypervisor( myObject.GetName(), csVMSystem))
				return TRUE;
			if (FindHypervisor( myObject.GetManufacturer(), csVMSystem))
				return TRUE;
			if (pos != NULL)
				myObject = pMyList->GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return FALSE;
}

BOOL CVMSystem::DetectVM( CStoragePeripheralList *pMyList, CString &csVMSystem)
{
	CStoragePeripheral myObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		pos = pMyList->GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			myObject = pMyList->GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			if (FindHypervisor( myObject.GetName(), csVMSystem))
				return TRUE;
			if (FindHypervisor( myObject.GetManufacturer(), csVMSystem))
				return TRUE;
			if (FindHypervisor( myObject.GetModel(), csVMSystem))
				return TRUE;
			if (pos != NULL)
				myObject = pMyList->GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return FALSE;
}


BOOL CVMSystem::DetectVM( CVideoAdapterList *pMyList, CString &csVMSystem)
{
	CVideoAdapter	myObject;
	POSITION		pos;
	BOOL			bContinue;

	try
	{
		pos = pMyList->GetHeadPosition();
		bContinue = (pos != NULL);
		if (bContinue)
			// There is one record => get the first
			myObject = pMyList->GetNext( pos);
		while (bContinue)
		{
			bContinue = (pos != NULL);
			if (FindHypervisor( myObject.GetName(), csVMSystem))
				return TRUE;
			if (FindHypervisor( myObject.GetChipset(), csVMSystem))
				return TRUE;
			if (pos != NULL)
				myObject = pMyList->GetNext( pos);
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		return FALSE;
	}
	return FALSE;
}
