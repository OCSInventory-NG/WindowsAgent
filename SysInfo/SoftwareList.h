//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SoftwareList.h: interface for the CSoftwareList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOFTWARELIST_H__E36BC909_5DB5_4FD4_BB2F_260128477168__INCLUDED_)
#define AFX_SOFTWARELIST_H__E36BC909_5DB5_4FD4_BB2F_260128477168__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CSoftwareList : public CList<CSoftware, CSoftware&>   
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CSoftwareList();
	virtual ~CSoftwareList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_SOFTWARELIST_H__E36BC909_5DB5_4FD4_BB2F_260128477168__INCLUDED_)

