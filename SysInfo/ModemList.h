//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ModemList.h: interface for the CModemList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODEMLIST_H__D965CF4B_51B1_474D_B615_3D4150A16F44__INCLUDED_)
#define AFX_MODEMLIST_H__D965CF4B_51B1_474D_B615_3D4150A16F44__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CModemList : public CList<CModem, CModem&>
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CModemList();
	virtual ~CModemList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_MODEMLIST_H__D965CF4B_51B1_474D_B615_3D4150A16F44__INCLUDED_)

