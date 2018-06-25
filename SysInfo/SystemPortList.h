//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SystemPortList.h: interface for the CSystemPortList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMPORTLIST_H__2A8DF271_A4B9_47A1_ACD7_455B1E6C24B5__INCLUDED_)
#define AFX_SYSTEMPORTLIST_H__2A8DF271_A4B9_47A1_ACD7_455B1E6C24B5__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CSystemPortList : public CList<CSystemPort, CSystemPort&> 
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CSystemPortList();
	virtual ~CSystemPortList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_SYSTEMPORTLIST_H__2A8DF271_A4B9_47A1_ACD7_455B1E6C24B5__INCLUDED_)

