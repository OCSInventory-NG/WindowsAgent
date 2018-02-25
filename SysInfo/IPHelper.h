//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// IPHelper.h: interface for the CIPHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPHELPER_H__609EE787_2041_44FB_863D_75B36F337728__INCLUDED_)
#define AFX_IPHELPER_H__609EE787_2041_44FB_863D_75B36F337728__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CIPHelper  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CIPHelper();
	virtual ~CIPHelper();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////
	static BOOL GetNetworkAdapters( CNetworkAdapterList *pList);
};
#endif // !defined(AFX_IPHELPER_H__609EE787_2041_44FB_863D_75B36F337728__INCLUDED_)

