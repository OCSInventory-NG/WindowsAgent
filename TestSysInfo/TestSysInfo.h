//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// TestSysInfo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTestSysInfoApp:
// See TestSysInfo.cpp for the implementation of this class
//

class CTestSysInfoApp : public CWinApp
{
public:
	CTestSysInfoApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTestSysInfoApp theApp;