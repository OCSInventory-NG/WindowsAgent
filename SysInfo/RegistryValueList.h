//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// RegistryValueList.h: interface for the CRegistryValueList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRYVALUELIST_H__373129CF_544C_4DC3_9536_313B900BC615__INCLUDED_)
#define AFX_REGISTRYVALUELIST_H__373129CF_544C_4DC3_9536_313B900BC615__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CRegistryValueList : public CList<CRegistryValue, CRegistryValue&>    
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CRegistryValueList();
	virtual ~CRegistryValueList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_REGISTRYVALUELIST_H__373129CF_544C_4DC3_9536_313B900BC615__INCLUDED_)

