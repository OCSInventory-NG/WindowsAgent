//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DMI.h: interface for the CDMI class.
//
// This code is derived from SMBIOS Peek code sample by wjfrancis
// http://www.codeproject.com/KB/system/SMBIOS_Peek.aspx
//////////////////////////////////////////////////////////////////////

#ifndef _DMI_HEADER_INCLUDED_
#define _DMI_HEADER_INCLUDED_

#pragma once

#include "SysInfoClasses.h"

// See DMTF standard SMBios specification reference
// Currently, version 2.6.1 is supported (see DSP0134_2.6.1.pdf)
// 
// As field values are used either from DMI tables or WMI, 
// methods to associate strings to numerical value are defined in components
// classes (CSystemSlot, CSystemPort...)

// Header of one DMI table
typedef struct _DmiHeader
{
	UCHAR Type;
	UCHAR Length;
	WORD  Handle;
}DmiHeader;

// Supported DMI table types
#define DMI_BIOS_INFORMATION		0
#define DMI_SYSTEM_INFORMATION		1
#define DMI_BASEBOARD_INFORMATION	2
#define DMI_SYSTEM_ENCLOSURE		3
#define DMI_SYSTEM_PORTS			8
#define DMI_SYSTEM_SLOTS			9
#define DMI_MEMORY_ARRAY			16
#define DMI_MEMORY_DEVICE			17

class SYSINFO_API CDMI
{
public:
	CDMI(void);
	~CDMI(void);

	//////////////////////////////////
	// Connection methods
	//////////////////////////////////
	BOOL Connect( LPCTSTR lpstrDevice = NULL);
	BOOL IsConnected();
	BOOL Disconnect();

	// Get information from DMI tables
	BOOL GetBiosInfo( CBios *pMyBios);
	BOOL GetBios( CString &csVendor, CString &csVersion, CString &csDate);
	BOOL GetSystemInformation( CString &csVendor, CString &csModel, CString &csSN);
	BOOL GetBaseBoard( CString &csVendor, CString &csProduct, CString &csVersion, CString &csSN, CString &csAssetTag);
	BOOL GetSystemEnclosure( CString &csVendor, CString &csType, CString &csSN, CString &csAssetTag);
	BOOL GetSystemPorts( CSystemPortList *pMyList);
	BOOL GetSystemSlots( CSystemSlotList *pMyList);
	BOOL GetMemorySlots( CMemorySlotList *pMyList);
	BOOL GetUUID( CString &csUUID);

protected:
	// Get string from DMI table field id
	LPCTSTR DmiString(DmiHeader* dmi, UCHAR id);
	// Get string from BYTE, WORD or DWORD value
	LPCTSTR DmiStringB(BYTE b);
	LPCTSTR DmiStringW(WORD w);
	LPCTSTR DmiStringD(DWORD d);
	// Get string with hex value from BYTE, WORD or DWORD value
	// Type 1 = format %04Xh (%d), type 2 = %04X, type 3 = %04Xh
	LPCTSTR DmiStringBX(BYTE b, DWORD type = 0);
	LPCTSTR DmiStringWX(WORD w, DWORD type = 0);
	LPCTSTR DmiStringDX(DWORD d, DWORD type = 0);
	// Verify SMBios header checksum
	BOOL CheckSum(const BYTE *buf, int length);
	// Get length of DMI table at position nPosition
	int GetTableLength( int nPosition);
	// Get next DMI table of type uType, and start seraching form begining
	// of table, or from current position
	UCHAR *GetNextTable( UCHAR uType, BOOL bStart = FALSE);

protected:
	// Raw SMBios data
	UCHAR *m_pTables;
	// Raw SMBios data length
	int m_nStructureLength;
	// SMBios Version
	int m_nSMBiosVersionMajor, m_nSMBiosVersionMinor;
};

#endif // _DMI_HEADER_INCLUDED_
