//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// InputDeviceList.h: interface for the CInputDeviceList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTDEVICELIST_H__91B9F0C4_E2FD_4855_B89B_888B2CCE0616__INCLUDED_)
#define AFX_INPUTDEVICELIST_H__91B9F0C4_E2FD_4855_B89B_888B2CCE0616__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CInputDeviceList : public CList<CInputDevice, CInputDevice&>  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CInputDeviceList();
	virtual ~CInputDeviceList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};

#endif // !defined(AFX_INPUTDEVICELIST_H__91B9F0C4_E2FD_4855_B89B_888B2CCE0616__INCLUDED_)

