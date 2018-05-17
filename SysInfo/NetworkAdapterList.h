//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// NetworkAdapterList.h: interface for the CNetworkAdapterList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKADAPTERLIST_H__DE761F11_8847_11D5_B278_0040055338AF__INCLUDED_)
#define AFX_NETWORKADAPTERLIST_H__DE761F11_8847_11D5_B278_0040055338AF__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CNetworkAdapterList : public CList<CNetworkAdapter, CNetworkAdapter&> 
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CNetworkAdapterList();
	virtual ~CNetworkAdapterList();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////
	CString GetMaskByGateway(CString gtw);
	CString GetIpByGateway(CString gtw);
	CString GetMaskByNetNumber(CString nbr);

	// Set the IP address and net mask for the adapter lIfIndex in the list
	BOOL SetIpAddrEntry( LONG lIfIndex, LPCTSTR lpstrIPAddr, LPCTSTR lpstrIPNetMask, LPCTSTR lpstrNetNumber);
	// Didier LIROULET 2005-10-17 BEGIN
	// Set the gateway for the adapter lIfIndex in the list
	BOOL SetIpRouteEntry( LONG lIfIndex, LPCTSTR lpstrGateway);
	// Didier LIROULET 2005-10-17 END
	// Set the adapter infos IP @, net mask, gateways and DCHP server by the adapter MAC @ in the list
	BOOL SetIpAddrEntry(LPCTSTR lpstrMAC, LPCTSTR lpstrIPAddr, LPCTSTR lpstrIPNetMask, LPCTSTR lpstrGateway, LPCTSTR lpstrDhcpServer, LPCTSTR lpstrNetNumber);
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_NETWORKADAPTERLIST_H__DE761F11_8847_11D5_B278_0040055338AF__INCLUDED_)

