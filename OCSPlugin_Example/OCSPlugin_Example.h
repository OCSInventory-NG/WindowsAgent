//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyleft 2007 OCS Inventory NG Team
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence Version 2.0
// http://www.gnu.org/ or Licence.txt
//====================================================================================

// OCSPlugin_Example.h : main header file for the OCSPLUGIN_EXAMPLE DLL
//

#if !defined(AFX_OCSPLUGIN_EXAMPLE_H__329976AF_C781_439B_A1EC_2AA1F0A115DE__INCLUDED_)
#define AFX_OCSPLUGIN_EXAMPLE_H__329976AF_C781_439B_A1EC_2AA1F0A115DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "PluginApi.h"

/////////////////////////////////////////////////////////////////////////////
// COCSPlugin_ExampleApp
// See OCSPlugin_Example.cpp for the implementation of this class
//

class COCSPlugin_ExampleApp : public CWinApp
{
public:
	COCSPlugin_ExampleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COCSPlugin_ExampleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(COCSPlugin_ExampleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OCSPLUGIN_EXAMPLE_H__329976AF_C781_439B_A1EC_2AA1F0A115DE__INCLUDED_)
