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
 * Header file for OcsinventoryFront Classes
 *
 */
#ifndef OCSINVENTORY_CLASSES_INCLUDED
#define OCSINVENTORY_CLASSES_INCLUDED

#pragma once

#ifdef OCSINVENTORYFRONT_EXPORTS
#define OCSINVENTORYFRONT_API __declspec(dllexport)
#else
#define OCSINVENTORYFRONT_API __declspec(dllimport)
#endif // OCSINVENTORYFRONT_EXPORTS

class CLog;
class CDeviceid;
class CConfig;
class CComProvider;

// Get Framework version
CString OCSINVENTORYFRONT_API getFrameworkVersion();
// Get application install folder
LPCTSTR OCSINVENTORYFRONT_API getInstallFolder();
// Get application data storage folder (All Users)
LPCTSTR OCSINVENTORYFRONT_API getDataFolder();
// Get pointer to agent's default Logging machine, or agent default
CLog OCSINVENTORYFRONT_API *getOcsLogger();
// Get pointer to computer unique ID
CDeviceid OCSINVENTORYFRONT_API *getOcsDeviceID();
// Get Agent config
CConfig OCSINVENTORYFRONT_API *getAgentConfig();
// Get Communication Provider to use with Communication server
CComProvider OCSINVENTORYFRONT_API *getComServerProvider();
// Get error message from system
CString OCSINVENTORYFRONT_API LookupError( DWORD dwErr);
// Get error message from C++ exception
CString OCSINVENTORYFRONT_API LookupError( CException *pEx);
// Retrieve if a command line option is on (/XML for example)
BOOL OCSINVENTORYFRONT_API isRequired( LPCTSTR lpstrCommandLine, CString csOption);
// Retrieve value from a command line option ("/XML:path_to_file.xml")
CString OCSINVENTORYFRONT_API getParamValue( LPCTSTR lpstrCommandLine, CString csParam);

// Get application data storage folder (All Users)
BOOL OCSINVENTORYFRONT_API setDataFolder( LPCTSTR lpstrDir);

#endif // OCSINVENTORY_CLASSES_INCLUDED