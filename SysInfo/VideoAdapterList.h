//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// VideoAdapterList.h: interface for the CVideoAdapterList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOADAPTERLIST_H__DE761F15_8847_11D5_B278_0040055338AF__INCLUDED_)
#define AFX_VIDEOADAPTERLIST_H__DE761F15_8847_11D5_B278_0040055338AF__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CVideoAdapterList : public CList<CVideoAdapter, CVideoAdapter&>  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CVideoAdapterList();
	virtual ~CVideoAdapterList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_VIDEOADAPTERLIST_H__DE761F15_8847_11D5_B278_0040055338AF__INCLUDED_)

