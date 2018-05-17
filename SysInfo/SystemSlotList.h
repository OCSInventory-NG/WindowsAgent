//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SystemSlotList.h: interface for the CSystemSlotList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMSLOTLIST_H__206470BA_A221_41A4_B55E_4BD618E23911__INCLUDED_)
#define AFX_SYSTEMSLOTLIST_H__206470BA_A221_41A4_B55E_4BD618E23911__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CSystemSlotList : public CList<CSystemSlot, CSystemSlot&>  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CSystemSlotList();
	virtual ~CSystemSlotList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_SYSTEMSLOTLIST_H__206470BA_A221_41A4_B55E_4BD618E23911__INCLUDED_)

