//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// LogicalDrive.cpp: implementation of the CLogicalDrive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "LogicalDrive.h"

#pragma comment(lib, "mpr.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogicalDrive::CLogicalDrive()
{
	Clear();
}

CLogicalDrive::~CLogicalDrive()
{
	Clear();
}

LPCTSTR CLogicalDrive::GetFileSystem()
{
	return m_csFileSystem;
}

LPCTSTR CLogicalDrive::GetDriveType()
{
	return m_csDriveType;
}

LPCTSTR CLogicalDrive::GetVolumName()
{
	return m_csVolumName;
}

LPCTSTR CLogicalDrive::GetDriveLetter()
{
	return m_csDriveLetter;
}

LONG CLogicalDrive::GetTotalMB()
{
	return m_lTotalSize;
}

LONG CLogicalDrive::GetFreeMB()
{
	return m_lFreeSpace;
}

LPCTSTR CLogicalDrive::GetPercentFree()
{
	static CString csMessage;

	if (m_lTotalSize > 0)
		csMessage.Format( _T( "%ld %%"), m_lFreeSpace*100/m_lTotalSize);
	else
		csMessage = NOT_AVAILABLE;
	return csMessage;
}

BOOL CLogicalDrive::RetrieveDriveInfo( LPCTSTR lpstrDrive)
{
	BOOL	bIsLocalDrive;				// TRUE if logical drive is a hard drive
	TCHAR	strName[255];				// Volume Name buffer
	DWORD	dwName = 255;				// Max size of Volume Name buffer
	TCHAR	strFSName[255];				// FS name buffer
	DWORD	dwFSName = 255;				// Max size of FS Name buffer
	DWORD	dwMaximumComponentLength;	// Max filename length for this FS
	DWORD	dwFSFlags;					// FS settings
	TCHAR	strRemoteShare[_MAX_PATH+1];// Remote share name
	DWORD	dwRemoteShare = _MAX_PATH+1;// Max size of remote share name
	CString	csDrive;					// Formatting the drive name for WNetGetConnection

	m_csDriveLetter = lpstrDrive;
	// Retrieve drive type
	switch (::GetDriveType( lpstrDrive))
		{
		case DRIVE_FIXED :		// Local HDD
			SetType( DRIVE_FIXED);
			bIsLocalDrive = TRUE;
			// Retrieve File System information
			if (GetVolumeInformation( lpstrDrive, strName, dwName, NULL, &dwMaximumComponentLength, &dwFSFlags, strFSName, dwFSName))
			{
				// OK
				m_csVolumName = strName;
				m_csFileSystem = strFSName;
			}
			else
			{
				// Error
				m_csVolumName = NOT_AVAILABLE;
				m_csFileSystem = NOT_AVAILABLE;
			}
			break;
		case DRIVE_REMOVABLE :	// Removable Drive
			SetType( DRIVE_REMOVABLE);
			bIsLocalDrive = FALSE;
			m_csVolumName = NOT_AVAILABLE;
			m_csFileSystem = NOT_AVAILABLE;
			break;
		case DRIVE_CDROM:		// CD-Rom drive
			SetType( DRIVE_CDROM);
			bIsLocalDrive = FALSE;
			m_csVolumName = NOT_AVAILABLE;
			m_csFileSystem = NOT_AVAILABLE;
			break;
		case DRIVE_REMOTE :		// Network drive
			SetType( DRIVE_REMOTE);
			bIsLocalDrive = FALSE;
			// Retrieve File System information
			if (GetVolumeInformation( lpstrDrive, strName, dwName, NULL, &dwMaximumComponentLength, &dwFSFlags, strFSName, dwFSName))
				// OK
				m_csFileSystem = strFSName;
			else
				// Error
				m_csFileSystem = NOT_AVAILABLE;
			// Get remote share
			csDrive = lpstrDrive;
			csDrive.Remove( '\\');
			if (WNetGetConnection( csDrive, strRemoteShare, &dwRemoteShare) == NO_ERROR)
				// OK
				m_csVolumName.Format( _T( "%s"), strRemoteShare);
			else
				// Error
				m_csVolumName.Format( _T( "%s"), NOT_AVAILABLE);
			break;
		case DRIVE_RAMDISK:		// RAM Disk
			SetType( DRIVE_RAMDISK);
			bIsLocalDrive = TRUE;
			// Retrieve File System information
			if (GetVolumeInformation( lpstrDrive, strName, dwName, NULL, &dwMaximumComponentLength, &dwFSFlags, strFSName, dwFSName))
			{
				// OK
				m_csVolumName = strName;
				m_csFileSystem = strFSName;
			}
			else
			{
				// Error
				m_csVolumName = NOT_AVAILABLE;
				m_csFileSystem = NOT_AVAILABLE;
			}
			break;
		case DRIVE_NO_ROOT_DIR:	// Disk does not have root dir
			SetType( DRIVE_NO_ROOT_DIR);
			bIsLocalDrive = FALSE;
			m_csVolumName = NOT_AVAILABLE;
			m_csFileSystem = NOT_AVAILABLE;
			break;
		case DRIVE_UNKNOWN:
			SetType( DRIVE_UNKNOWN);
			bIsLocalDrive = FALSE;
			m_csVolumName = NOT_AVAILABLE;
			m_csFileSystem = NOT_AVAILABLE;
			break;
		default :				// Unknown
			SetDriveType( NOT_AVAILABLE);
			bIsLocalDrive = FALSE;
			m_csVolumName = NOT_AVAILABLE;
			m_csFileSystem = NOT_AVAILABLE;
			break;
		}
	// Retrieve Total size and Free space
	if (bIsLocalDrive)
	{
		m_lTotalSize = RetrieveTotalSize( lpstrDrive);
		m_lFreeSpace = RetrieveFreeSpace( lpstrDrive);
	}
	else
	{
		// Total size and Free space not available
		m_lTotalSize = 0;
		m_lFreeSpace = 0;
	}
	StrForSQL( m_csDriveLetter);
	StrForSQL( m_csDriveType);
	StrForSQL( m_csFileSystem);
	StrForSQL( m_csVolumName);
	// Return TRUE if this is a hard drive
	return bIsLocalDrive;
}

LONG CLogicalDrive::GetFilesNumber()
{
	return m_lNumberOfFiles;
}

void CLogicalDrive::SetDriveType( LPCTSTR lpstrType)
{
	m_csDriveType = lpstrType;
	StrForSQL( m_csDriveType);
}

void CLogicalDrive::SetType( DWORD dwType)
{
	switch (dwType)
	{
	case DRIVE_REMOVABLE:
		m_csDriveType = _T( "Removable Drive");
		break;
	case DRIVE_FIXED:
		m_csDriveType = _T( "Hard Drive");
		break;
	case DRIVE_REMOTE:
		m_csDriveType = _T( "Network Drive");
		break;
	case DRIVE_CDROM:
		m_csDriveType = _T( "CD-Rom Drive");
		break;
	case DRIVE_RAMDISK:
		m_csDriveType = _T( "RAM Drive");
		break;
	case DRIVE_NO_ROOT_DIR:
		m_csDriveType = _T( "No root dir");
		break;
	case DRIVE_UNKNOWN:
		m_csDriveType = _T( "Unknown");
		break;
	default:
		m_csDriveType = NOT_AVAILABLE;
		break;
	}
	StrForSQL( m_csDriveType);
}

void CLogicalDrive::SetVolumName( LPCTSTR lpstrVolumn)
{
	m_csVolumName = lpstrVolumn;
	StrForSQL( m_csVolumName);
}

void CLogicalDrive::SetFileSystem( LPCTSTR lpstrFileSystem)
{
	m_csFileSystem = lpstrFileSystem;
	StrForSQL( m_csFileSystem);
}

void CLogicalDrive::SetDriveLetter( LPCTSTR lpstrLetter)
{
	m_csDriveLetter = lpstrLetter;
	StrForSQL( m_csDriveLetter);
}

void CLogicalDrive::SetTotalSize( LONG lSize)
{
	m_lTotalSize = lSize;
}

void CLogicalDrive::SetFreeSpace( LONG lFree)
{
	m_lFreeSpace = lFree;
}

void CLogicalDrive::SetFilesNumber(LONG lNum)
{
	m_lNumberOfFiles = lNum;
}

ULONG CLogicalDrive::RetrieveTotalSize(LPCTSTR lpstrDrive)
{
	DWORD	dwSectPerClust,
			dwBytesPerSect,
			dwFreeClusters,
			dwTotalClusters;

	unsigned __int64 i64FreeBytesToCaller,
                       i64TotalBytes,
                       i64FreeBytes;
    BOOL  fResult;

	typedef BOOL (WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER,
		PULARGE_INTEGER, PULARGE_INTEGER);

    P_GDFSE pGetDiskFreeSpaceEx = NULL;

//         Use GetDiskFreeSpaceEx if available; otherwise, use
//         GetDiskFreeSpace.
//
//         Note: Since GetDiskFreeSpaceEx is not in Windows 95 Retail, we
//         dynamically link to it and only call it if it is present.  We 
//         don't need to call LoadLibrary on KERNEL32.DLL because it is 
//         already loaded into every Win32 process's address space.

#ifndef _UNICODE
	pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress(
                               GetModuleHandle( _T( "KERNEL32.DLL")),
                                                "GetDiskFreeSpaceExA");
#else
	pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress(
                               GetModuleHandle( _T( "KERNEL32.DLL")),
                                                "GetDiskFreeSpaceExW");
#endif
    if (pGetDiskFreeSpaceEx)
    {
		fResult = pGetDiskFreeSpaceEx (lpstrDrive,
							(PULARGE_INTEGER)&i64FreeBytesToCaller,
                            (PULARGE_INTEGER)&i64TotalBytes,
                            (PULARGE_INTEGER)&i64FreeBytes);
    }
	else
	{
		// Cannot load GetDiskFreeSpaceEx => use GetDiskFreeSpace
        fResult = GetDiskFreeSpace (lpstrDrive, 
                             &dwSectPerClust,
                             &dwBytesPerSect, 
                             &dwFreeClusters,
                             &dwTotalClusters);

        if (fResult)
        {
            // force 64-bit math
			i64TotalBytes = (__int64)dwTotalClusters * dwSectPerClust *
                              dwBytesPerSect;
            i64FreeBytes = (__int64)dwFreeClusters * dwSectPerClust *
                              dwBytesPerSect;
        }
	}
	
	if (!fResult)
		// Error
		return -1;

    return (ULONG) (i64TotalBytes/ONE_MEGABYTE);
}

ULONG CLogicalDrive::RetrieveFreeSpace(LPCTSTR lpstrDrive)
{
	DWORD	dwSectPerClust,
			dwBytesPerSect,
			dwFreeClusters,
			dwTotalClusters;

	unsigned __int64 i64FreeBytesToCaller,
                     i64TotalBytes,
                     i64FreeBytes;
    BOOL	fResult;

	typedef BOOL (WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER,
		PULARGE_INTEGER, PULARGE_INTEGER);

    P_GDFSE pGetDiskFreeSpaceEx = NULL;

//         Use GetDiskFreeSpaceEx if available; otherwise, use
//         GetDiskFreeSpace.
//
//         Note: Since GetDiskFreeSpaceEx is not in Windows 95 Retail, we
//         dynamically link to it and only call it if it is present.  We 
//         don't need to call LoadLibrary on KERNEL32.DLL because it is 
//         already loaded into every Win32 process's address space.

#ifndef _UNICODE
	pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress(
                               GetModuleHandle( _T( "KERNEL32.DLL")),
                                                "GetDiskFreeSpaceExA");
#else
	pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress(
                               GetModuleHandle( _T( "KERNEL32.DLL")),
                                                "GetDiskFreeSpaceExW");
#endif
    if (pGetDiskFreeSpaceEx)
    {
		fResult = pGetDiskFreeSpaceEx (lpstrDrive,
							(PULARGE_INTEGER)&i64FreeBytesToCaller,
                            (PULARGE_INTEGER)&i64TotalBytes,
                            (PULARGE_INTEGER)&i64FreeBytes);
    }
	else
	{
		// Cannot load GetDiskFreeSpaceEx => use GetDiskFreeSpace
        fResult = GetDiskFreeSpace (lpstrDrive, 
                             &dwSectPerClust,
                             &dwBytesPerSect, 
                             &dwFreeClusters,
                             &dwTotalClusters);

        if (fResult)
        {
            // force 64-bit math
			i64TotalBytes = (__int64)dwTotalClusters * dwSectPerClust *
                              dwBytesPerSect;
            i64FreeBytes = (__int64)dwFreeClusters * dwSectPerClust *
                              dwBytesPerSect;
        }
	}
	
	if (!fResult)
		// Error
		return -1;

    return (ULONG) (i64FreeBytes/ONE_MEGABYTE);
}

void CLogicalDrive::Clear()
{
	m_csDriveType.Empty();	
	m_csVolumName.Empty();	
	m_csFileSystem.Empty();	
	m_csDriveLetter.Empty();
	m_lTotalSize = 0;	
	m_lFreeSpace = 0;	
	m_lNumberOfFiles = 0;
}

int CLogicalDrive::operator==(CLogicalDrive cObject) const
{
	return ((m_csDriveType == cObject.GetDriveType()) &&	
		(m_csVolumName == cObject.GetVolumName()) &&
		(m_csFileSystem == cObject.GetFileSystem()) &&
		(m_lTotalSize == cObject.GetTotalMB()) &&
		(m_lFreeSpace == cObject.GetFreeMB()) &&
		(m_lNumberOfFiles == cObject.GetFilesNumber()));
}
