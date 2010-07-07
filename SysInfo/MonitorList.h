//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// MonitorList.h: interface for the CMonitorList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONITORLIST_H__6DCE229B_71C0_4A4D_B442_FA2BE2606853__INCLUDED_)
#define AFX_MONITORLIST_H__6DCE229B_71C0_4A4D_B442_FA2BE2606853__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CMonitorList : public CList<CMonitor, CMonitor&>  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CMonitorList();
	virtual ~CMonitorList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_MONITORLIST_H__6DCE229B_71C0_4A4D_B442_FA2BE2606853__INCLUDED_)

