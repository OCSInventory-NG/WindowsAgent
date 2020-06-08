//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// LogicalDriveList.h: interface for the CLogicalDriveList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICALDRIVELIST_H__859CC4DB_15B4_4DEE_A9D9_0EA772FC76A9__INCLUDED_)
#define AFX_LOGICALDRIVELIST_H__859CC4DB_15B4_4DEE_A9D9_0EA772FC76A9__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CLogicalDriveList : public CList<CLogicalDrive, CLogicalDrive&>   
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CLogicalDriveList();
	virtual ~CLogicalDriveList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_LOGICALDRIVELIST_H__859CC4DB_15B4_4DEE_A9D9_0EA772FC76A9__INCLUDED_)

