//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CMemory.cpp
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Windows.h"
#include "Winbase.h" 
#include "SysInfoClasses.h"

#include "Memory.h"

// Under Windows 2K or higher, use MEMORYSTATUSEX
BOOL (__stdcall *lpfnGlobalMemoryStatusEx) ( LPMEMORYSTATUSEX lpBuffer);

CMemory::CMemory()
{
   determineMemory();
}

CMemory::CMemory( const CMemory& source)
{
   assign(source);
}

CMemory& CMemory::operator=( const CMemory& right)
{
   if (this != &right)
   {
      assign(right);
   }

   return (*this);
}

CMemory::~CMemory()
{
   // nothing to do yet
}

void CMemory::determineMemory(void)
{
	OSVERSIONINFO	osVersion;

	osVersion.dwOSVersionInfoSize = sizeof( OSVERSIONINFO);
	if (!GetVersionEx( &osVersion))
	{
		// Unable to know if it is W2K or higher
		determineMemory9X_NT();
		return;
	}
	if ((osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
		(osVersion.dwMajorVersion >= 5))
	{
		// Windows 2K or higher => use MEMORYSTATUSEX
		if (!determineMemory2K())
			determineMemory9X_NT();
	}
	else
	{
		// Windows 9X/Me/NT => use MEMORYSTATUS
		determineMemory9X_NT();
	}
}

BOOL CMemory::determineMemory9X_NT(void)
{
	// Windows 9X/Me/NT => use MEMORYSTATUS
	MEMORYSTATUS	pMemStatus;
	GlobalMemoryStatus( &pMemStatus);

	m_ulTotalRam = pMemStatus.dwTotalPhys + 655360;
	m_ulAvailRam = pMemStatus.dwAvailPhys + 655360;
	m_ulTotalPageFile = pMemStatus.dwTotalPageFile + 655360;
	m_ulAvailPageFile = pMemStatus.dwAvailPageFile + 655360;
	m_ulTotalVirtual = pMemStatus.dwTotalVirtual + 655360;
	m_ulAvailVirtual = pMemStatus.dwAvailVirtual + 655360;
	return TRUE;
}

BOOL CMemory::determineMemory2K(void)
{
	// Windows 2K or higher => use MEMORYSTATUSEX
	MEMORYSTATUSEX pMemStatus;

	if ((*(FARPROC*)&lpfnGlobalMemoryStatusEx = GetProcAddress( GetModuleHandle(_T( "KERNEL32.DLL")), "GlobalMemoryStatusEx")) == NULL)
		return FALSE;

	pMemStatus.dwLength = sizeof( MEMORYSTATUSEX);
	if (!lpfnGlobalMemoryStatusEx (&pMemStatus))
		return FALSE;

	m_ulTotalRam = pMemStatus.ullTotalPhys + 655360;
	m_ulAvailRam = pMemStatus.ullAvailPhys + 655360;
	m_ulTotalPageFile = pMemStatus.ullTotalPageFile + 655360;
	m_ulAvailPageFile = pMemStatus.ullAvailPageFile + 655360;
	m_ulTotalVirtual = pMemStatus.ullTotalVirtual + 655360;
	m_ulAvailVirtual = pMemStatus.ullAvailVirtual + 655360;
	return TRUE;
}

void CMemory::assign(const CMemory& source)
{
   m_ulTotalRam = source.m_ulTotalRam;
   m_ulAvailRam = source.m_ulAvailRam;
   m_ulTotalPageFile = source.m_ulTotalPageFile;
   m_ulAvailPageFile = source.m_ulAvailPageFile;
   m_ulTotalVirtual = source.m_ulTotalVirtual;
   m_ulAvailVirtual = source.m_ulAvailVirtual;
}

ULONGLONG CMemory::getTotalRAM() const
{
	return m_ulTotalRam;
}

ULONGLONG CMemory::getAvailRAM() const
{
	return m_ulAvailRam;
}

ULONGLONG CMemory::getTotalPageFile() const
{
	return m_ulTotalPageFile;
}

ULONGLONG CMemory::getAvailPageFile() const
{
	return m_ulAvailPageFile;
}

ULONGLONG CMemory::getTotalVirtual() const
{
	return m_ulTotalVirtual;
}

ULONGLONG CMemory::getAvailVirtual() const
{
	return m_ulAvailVirtual;
}

