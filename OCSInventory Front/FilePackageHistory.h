//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// FilePackageHistory.h: interface for the CFilePackageHistory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEPACKAGEHISTORY_H__A4BA1C08_4CE4_4C45_BDC0_4EEBA344E0E0__INCLUDED_)
#define AFX_FILEPACKAGEHISTORY_H__A4BA1C08_4CE4_4C45_BDC0_4EEBA344E0E0__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"

class OCSINVENTORYFRONT_API CFilePackageHistory 
{
public:
	CFilePackageHistory();
	virtual ~CFilePackageHistory();

	// Open history file lpstrFile
	BOOL Open( LPCTSTR lpstrFile, BOOL bForWriting = FALSE, BOOL bExclusive = FALSE);
	// Read a package to an history file opened for reading (from current position)
	BOOL ReadPackage( CString &csPackageID);
	// Add a package to an history file opened for writing, to end of file
	BOOL AddPackage( LPCTSTR lpstrPackageID);
	// Close file when no error, saving changes
	void Close();
	// Close file when error, discarding changes
	void Abort();
	// Positionning
	void SeekToEnd();
	void SeekToBegin();

	// Remove duplicates package from an history file
	// Create backup file history.bak to store history file before modifications
	// If history.bak file already exists, clean ignored until bForce = TRUE
	// Return 0 if error, 1 if file cleaned successfully, 2 if clean not required
	static int CleanDuplicates( LPCTSTR lpstrFile, BOOL bForce = FALSE);
	// Add package to an history file (will be opened and then closed)
	static BOOL AddPackage( LPCTSTR lpstrFile, LPCTSTR lpstrPackageID);

protected:
	// An history file is just a special text file
	CStdioFile m_File;
	// Is history file already opened ?
	BOOL	   m_bOpened;
};

#endif // !defined(AFX_FILEPACKAGEHISTORY_H__A4BA1C08_4CE4_4C45_BDC0_4EEBA344E0E0__INCLUDED_)
