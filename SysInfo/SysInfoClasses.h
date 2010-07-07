//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SysInfoClasses.h
//    
//   This file uses #pragma's and #define's that are shared between all 
//   SysInfo classes. In order to link statically, #define 
//   SYSINFO_LINKSTATIC in your project's settings. 
// 



#ifndef SYSINFOCLASSES_H_
#define SYSINFOCLASSES_H_

#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>


#define NOT_AVAILABLE	_T( "N/A")
#define ONE_KILOBYTE	1024
#define ONE_MEGABYTE	1048576 // (1024 * 1024)
#define MAX_OBJECTS		128		// Max number of processes and modules we can check


//------------------------------------------------------
// this allows us to export/import a class
// to link with the dll, do nothing; 
//

#if defined(SYSINFO_EXPORTS)
#   undef SYSINFO_API
#   define SYSINFO_API __declspec(dllexport)
#else 
#   undef SYSINFO_API
#   define SYSINFO_API __declspec(dllimport)
#endif


inline void StrForSQL( CString &csMyStr)
{
	csMyStr.Replace( _T( "'"), _T( "''"));
	csMyStr.Replace( _T( "\\"), _T( "/"));
	csMyStr.Replace( _T( "\n"), _T( " "));
	csMyStr.TrimLeft();
	csMyStr.TrimRight();
}


inline BOOL isLocalDrive( LPCTSTR lpstrDrive)
{
	return (GetDriveType( lpstrDrive) == DRIVE_FIXED);
}

#endif