//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SoundDeviceList.h: interface for the CSoundDeviceList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDDEVICELIST_H__C69CD4E2_060A_488D_982C_CE8626CFD27B__INCLUDED_)
#define AFX_SOUNDDEVICELIST_H__C69CD4E2_060A_488D_982C_CE8626CFD27B__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CSoundDeviceList : public CList<CSoundDevice, CSoundDevice&>  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CSoundDeviceList();
	virtual ~CSoundDeviceList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_SOUNDDEVICELIST_H__C69CD4E2_060A_488D_982C_CE8626CFD27B__INCLUDED_)

