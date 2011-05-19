//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ServiceApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ServiceApp.h"
#include "OcsService.h"
#include "NTServiceMsg.h" // Event message ids
#include "OCSInventory Front.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	COcsService	MyService; 

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		MyService.LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_FAILED_INIT);
		nRetCode = 1;
	}
	else
	{
#ifdef _DEBUG
		if (MyService.Initialize())
		{
			MyService.Run();
			MyService.OnStop();
		}
		return MyService.GetWin32ExitCode();
#endif
		// Parse for standard arguments (install, uninstall, version etc.)
		if (!MyService.ParseCommandLine( argc, argv))
		{
			// Didn't find any standard args so start the service
			// Uncomment the DebugBreak line below to enter the debugger
			// when the service is started.
			//DebugBreak();
			MyService.StartService();
		}
		// When we get here, the service has been stopped
		nRetCode = MyService.GetWin32ExitCode();
	}
	return nRetCode;
}
