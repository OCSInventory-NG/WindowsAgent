//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsProcess.h: interface for the CProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCSPROCESS_H__05D45087_83E3_482C_9927_E7423B6C0502__INCLUDED_)
#define AFX_OCSPROCESS_H__05D45087_83E3_482C_9927_E7423B6C0502__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class CProcess  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CProcess();
	virtual ~CProcess();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	static BOOL IsProcessRunning(LPCTSTR lpstrProcessName);

protected: // Methods
	static BOOL IsProcessRunning9X(LPCTSTR lpstrProcessName);
	static BOOL IsProcessRunningNT(LPCTSTR lpstrProcessName);
};
#endif // !defined(AFX_OCSPROCESS_H__05D45087_83E3_482C_9927_E7423B6C0502__INCLUDED_)

