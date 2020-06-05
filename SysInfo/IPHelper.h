//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
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

#define ETHERNET_ADAPTER	_T( "Ethernet")
#define TOKENBUS_ADAPTER	_T( "Token Bus")
#define TOKENRING_ADAPTER	_T( "Token Ring")
#define ATM_ADAPTER			_T( "ATM")
#define DIALUP_ADAPTER		_T( "Dialup")
#define LOOPBACK_ADAPTER	_T( "Loopback")
#define OTHER_ADAPTER		_T( "Other (see MIB)")


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
	static BOOL GetNetworkAdaptersJustMAC(CNetworkAdapterList *pList);

	// 
	static LPCTSTR GetIfType(UINT uType);
	static BOOL IsEthernet(UINT uType);
	static BOOL IsTokenRing(UINT uType);
	static BOOL IsTokenBus(UINT uType);
	static BOOL IsISDN(UINT uType);
	static BOOL IsDialup(UINT uType);
	static BOOL IsATM(UINT uType);
	static BOOL IsLAN(UINT uType);
	static BOOL IsDSL(UINT uType);
	static BOOL IsLoopback(UINT uType);
	static LPCTSTR GetAdapterType(UINT uType);
};
#endif // !defined(AFX_IPHELPER_H__609EE787_2041_44FB_863D_75B36F337728__INCLUDED_)

