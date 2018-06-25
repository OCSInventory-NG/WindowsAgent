//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// VideoAdapter.h: interface for the CVideoAdapter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOADAPTER_H__DE761F14_8847_11D5_B278_0040055338AF__INCLUDED_)
#define AFX_VIDEOADAPTER_H__DE761F14_8847_11D5_B278_0040055338AF__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CVideoAdapter
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CVideoAdapter();
	virtual ~CVideoAdapter();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	// Return Video adapter name
	LPCTSTR GetName();
	// Retrun Video adapter chipset
	LPCTSTR GetChipset();
	// Return Video adapter memory
	LPCTSTR GetMemory();
	// Return screen resolution
	LPCTSTR GetScreenResolution();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetName( LPCTSTR lpstrDescription);
	void SetChipset( LPCTSTR lpstrChipset);
	void SetMemory( LPCTSTR lpstrMemory);
	void SetMemory( LONG lMemory);
	void SetScreenResolution( LPCTSTR lpstrResolution);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CVideoAdapter cObject) const;

protected: // Attributes
	CString	m_csName;			// Video adpater name
	CString m_csChipset;		// Video adapter chipset
	CString m_csMemory;			// Video adapter memory
	CString	m_csResolution;		// Screen resolution
};

#endif // !defined(AFX_VIDEOADAPTER_H__DE761F14_8847_11D5_B278_0040055338AF__INCLUDED_)

