//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// HTTP.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "HTTP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CHTTPApp
BEGIN_MESSAGE_MAP(CHTTPApp, CWinApp)
END_MESSAGE_MAP()


// CHTTPApp construction
CHTTPApp::CHTTPApp()
{
}


// The one and only CHTTPApp object
CHTTPApp theApp;


// CHTTPApp initialization
BOOL CHTTPApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


CServerConfig * CHTTPApp::createServerConfig( LPCTSTR lpstrFile, LPCTSTR lpstrSection)
{
	CHTTPConfig *pConfig;

    if ((pConfig = new CHTTPConfig( lpstrFile, lpstrSection)) == NULL)
		return NULL;
	return pConfig;
}

BOOL CHTTPApp::writeServerConfig( CServerConfig *pConfig, LPCTSTR lpstrFile, LPCTSTR lpstrSection)
{
	CHTTPConfig *pHttpConfig = (CHTTPConfig *) pConfig;

	return pHttpConfig->save( lpstrFile, lpstrSection);
}

void CHTTPApp::destroyServerConfig(CServerConfig *pConfig)
{
    CHTTPConfig *pHTTP = (CHTTPConfig *) pConfig;

    if (pHTTP)
        delete pHTTP;
}

CConnexionAbstract * CHTTPApp::createServerConnexion( CServerConfig *pConfig)
{
	ASSERT( pConfig);

	CHTTPConfig *pHTTP = (CHTTPConfig *)pConfig;

    return new CHTTPConnexion( pHTTP);
}

void CHTTPApp::destroyServerConnexion(CConnexionAbstract *pCon)
{
    CHTTPConnexion *pHTTP = (CHTTPConnexion *) pCon;

    if (pHTTP)
        delete pHTTP;
}

///////////////////////////////////////////////////////////////////////////////
// Class wrapper functions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// CreateServerConnexion - create an instance of the class CConnexionAbstract
OCS_PROVIDER_API CConnexionAbstract * newServerConnexion( CServerConfig *pConfig)
{
	// normal function body here
	return theApp.createServerConnexion( pConfig);
}

///////////////////////////////////////////////////////////////////////////////
// DestroyServerConnexion - free the memory for the class instance 
OCS_PROVIDER_API void deleteServerConnexion(CConnexionAbstract *pCon)
{
	// normal function body here
	theApp.destroyServerConnexion( pCon);
}

///////////////////////////////////////////////////////////////////////////////
// CreateServerConfig - create an instance of the class CServerConfig
OCS_PROVIDER_API CServerConfig * newServerConfig( LPCTSTR lpstrFile, LPCTSTR lpstrSection)
{
	// normal function body here
	return theApp.createServerConfig( lpstrFile, lpstrSection);
}

///////////////////////////////////////////////////////////////////////////////
// saveServerConfig - save an instance of the class CServerConfig
OCS_PROVIDER_API BOOL saveServerConfig( CServerConfig *pConfig, LPCTSTR lpstrFile, LPCTSTR lpstrSection)
{
	// normal function body here
	return theApp.writeServerConfig( pConfig, lpstrFile, lpstrSection);
}

///////////////////////////////////////////////////////////////////////////////
// deleteServerConfig - free the memory for the class instance 
OCS_PROVIDER_API void deleteServerConfig( CServerConfig *pConfig)
{
	// normal function body here
	theApp.destroyServerConfig( pConfig);
}
