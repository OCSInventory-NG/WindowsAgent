//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SystemControllerList.h: interface for the CSystemControllerList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMCONTROLLERLIST_H__09308B4F_199B_4D87_AEF9_AE219D44B044__INCLUDED_)
#define AFX_SYSTEMCONTROLLERLIST_H__09308B4F_199B_4D87_AEF9_AE219D44B044__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CSystemControllerList : public CList<CSystemController, CSystemController&>   
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CSystemControllerList();
	virtual ~CSystemControllerList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_SYSTEMCONTROLLERLIST_H__09308B4F_199B_4D87_AEF9_AE219D44B044__INCLUDED_)

