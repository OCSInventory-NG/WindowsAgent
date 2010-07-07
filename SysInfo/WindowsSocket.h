//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// WindowsSocket.h: interface for the CWindowsSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWSSOCKET_H__91E29D3E_FE3D_49BC_A386_611A6718999E__INCLUDED_)
#define AFX_WINDOWSSOCKET_H__91E29D3E_FE3D_49BC_A386_611A6718999E__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CWindowsSocket  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CWindowsSocket();
	virtual ~CWindowsSocket();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetIPAddress();

protected: // Attributes
	CString m_csIP;
};
#endif // !defined(AFX_WINDOWSSOCKET_H__91E29D3E_FE3D_49BC_A386_611A6718999E__INCLUDED_)

