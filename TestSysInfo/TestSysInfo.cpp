//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// TestSysInfo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TestSysInfo.h"
#include "TestSysInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#include "vld.h"
#endif


// CTestSysInfoApp

BEGIN_MESSAGE_MAP(CTestSysInfoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestSysInfoApp construction

CTestSysInfoApp::CTestSysInfoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTestSysInfoApp object

CTestSysInfoApp theApp;


// CTestSysInfoApp initialization

BOOL CTestSysInfoApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Standard initialization

	// Initialize COM.
	if (FAILED( CoInitializeEx( 0, COINIT_MULTITHREADED)))
	{
		AfxMessageBox( _T( "Failed to initialize COM"));
		return FALSE; // terminates the application
	}
	// Set general COM security levels --------------------------
	if (FAILED( CoInitializeSecurity(	NULL, 
									-1,                          // COM authentication
									NULL,                        // Authentication services
									NULL,                        // Reserved
									RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
									RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
									NULL,                        // Authentication info
									EOAC_NONE,                   // Additional capabilities 
									NULL                         // Reserved
									)))
	{
		AfxMessageBox( _T( "Failed to initialize COM Security"));
		CoUninitialize();
		return FALSE; // terminates the application
	}

	CTestSysInfoDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CTestSysInfoApp::ExitInstance() 
{
	// Free COM
	CoUninitialize();
	return CWinApp::ExitInstance();
}
