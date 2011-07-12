//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

/* 
 *
 * Header file for OcsinventoryFront Communication Provider Classes
 *
 */
#ifndef _OCS_PROVIDER_CLASSES_INCLUDED_
#define _OCS_PROVIDER_CLASSES_INCLUDED_

#pragma once

#ifdef OCS_PROVIDER_EXPORTS
#define OCS_PROVIDER_API __declspec(dllexport)
#else
#define OCS_PROVIDER_API __declspec(dllimport)
#endif // OCS_HTTP_EXPORTS

#define OCS_HTTP_PROVIDER_SECTION					_T( "HTTP")

class CServerConfig;
class CConnexionAbstract;

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// CreateServerConfig - create an instance of the class CServerConfig
OCS_PROVIDER_API CServerConfig * newServerConfig( LPCTSTR lpstrFile = NULL, LPCTSTR lpstrSection = NULL);
///////////////////////////////////////////////////////////////////////////////
// DestroyServerConfig - free the memory for the class instance 
OCS_PROVIDER_API void deleteServerConfig( CServerConfig *pConfig);
///////////////////////////////////////////////////////////////////////////////
// DestroyServerConnexion - free the memory for the class instance 
OCS_PROVIDER_API BOOL saveServerConfig( CServerConfig *pConfig, LPCTSTR lpstrFile = NULL, LPCTSTR lpstrSection = NULL);
///////////////////////////////////////////////////////////////////////////////
// CreateServerConnexion - create an instance of the class CConnexionAbstract
OCS_PROVIDER_API CConnexionAbstract * newServerConnexion( CServerConfig *pConfig);
///////////////////////////////////////////////////////////////////////////////
// DestroyServerConnexion - free the memory for the class instance 
OCS_PROVIDER_API void deleteServerConnexion(CConnexionAbstract *pCon);

#ifdef __cplusplus
};
#endif

#endif // _OCS_PROVIDER_CLASSES_INCLUDED_