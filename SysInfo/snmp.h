//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SNMP.h: interface for the CSNMP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNMP_H__3CFE0AD8_CC5A_4A0C_8BED_28A055A3C960__INCLUDED_)
#define AFX_SNMP_H__3CFE0AD8_CC5A_4A0C_8BED_28A055A3C960__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CSNMP  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CSNMP();
	virtual ~CSNMP();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	static BOOL GetNetworkAdapters( CNetworkAdapterList *pList);
};
#endif // !defined(AFX_SNMP_H__3CFE0AD8_CC5A_4A0C_8BED_28A055A3C960__INCLUDED_)

