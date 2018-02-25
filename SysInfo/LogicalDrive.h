//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// LogicalDrive.h: interface for the CLogicalDrive class.
//
// Class storing informations about one computer logical drive.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICALDRIVE_H__A1610561_ACE7_11D4_8F74_00600889DFA5__INCLUDED_)
#define AFX_LOGICALDRIVE_H__A1610561_ACE7_11D4_8F74_00600889DFA5__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CLogicalDrive  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CLogicalDrive();
	virtual ~CLogicalDrive();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	// Return the free space in mega bytes of the logical drive
	LONG GetFreeMB();
	// Return the physical size in mega bytes of the logical drive
	LONG GetTotalMB();
	// Return the letter associated by the OS to the logical drive (A, C, D...)
	LPCTSTR GetDriveLetter();
	// Return the operating system volume name of the logical drive
	LPCTSTR GetVolumName();
	// Return the drive type the logical drive (Hard drive, CD-rom...)
	LPCTSTR GetDriveType();
	// Return the file system of the logical drive (FAT, FAT32, NTFS...)
	LPCTSTR GetFileSystem();
	// Return the number of files found on the logical drive
	LONG GetFilesNumber();
	// Return a string containing percantage of free space
	LPCTSTR GetPercentFree();
	// Format informations in a XML string

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetType( DWORD dwType);
	void SetDriveType( LPCTSTR lpstrType);
	void SetVolumName( LPCTSTR lpstrVolumn);
	void SetFileSystem( LPCTSTR lpstrFileSystem);
	void SetDriveLetter( LPCTSTR lpstrLetter);
	void SetTotalSize( LONG lSize);
	void SetFreeSpace( LONG lFree);
	void SetFilesNumber( LONG lNum);
	// Retrieve all other logical drive information
	BOOL RetrieveDriveInfo( LPCTSTR lpstrDrive);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CLogicalDrive cObject) const;

protected: // Methods
	ULONG RetrieveFreeSpace( LPCTSTR lpstrDrive);
	ULONG RetrieveTotalSize( LPCTSTR lpstrDrive);

protected: // Attributes
	CString m_csDriveType;	// Drive type (HD, FD, CDRom, REMOVABLE, NETWORK)
	CString m_csVolumName;	// OS volum name
	CString m_csFileSystem;	// File system type (FAT, FAT32, NTFS...)
	CString m_csDriveLetter;// OS drive letter
	LONG	m_lTotalSize;	// Total size in MB
	LONG	m_lFreeSpace;	// Free space in MB
	LONG	m_lNumberOfFiles;// Total number of files
};
#endif // !defined(AFX_LOGICALDRIVEINFO_H__A1610561_ACE7_11D4_8F74_00600889DFA5__INCLUDED_)

