//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// MemorySlotList.h: interface for the CMemorySlotList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYSLOTLIST_H__C516FC9E_571E_4CFE_BB73_0E6D7C0ECA66__INCLUDED_)
#define AFX_MEMORYSLOTLIST_H__C516FC9E_571E_4CFE_BB73_0E6D7C0ECA66__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CMemorySlotList : public CList<CMemorySlot, CMemorySlot&> 
{
public: // Methods
	ULONG GetTotalMemory();
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CMemorySlotList();
	virtual ~CMemorySlotList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};

#endif // !defined(AFX_MEMORYSLOTLIST_H__C516FC9E_571E_4CFE_BB73_0E6D7C0ECA66__INCLUDED_)

