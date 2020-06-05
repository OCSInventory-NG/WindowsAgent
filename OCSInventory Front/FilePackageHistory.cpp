//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// FilePackageHistory.cpp: implementation of the CFilePackageHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilePackageHistory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilePackageHistory::CFilePackageHistory()
{
	m_bOpened = FALSE;
}

CFilePackageHistory::~CFilePackageHistory()
{
	// Thanks Goneri
	if (m_bOpened)
		m_File.Close();
	m_bOpened = FALSE;
}

BOOL CFilePackageHistory::Open(LPCTSTR lpstrFile, BOOL bForWriting, BOOL bExclusive)
{
	BOOL bReturn = FALSE;

	try
	{
		UINT nOpenFlags = CFile::modeCreate|CFile::modeNoTruncate|CFile::typeText;
		
		if (bForWriting)
			nOpenFlags |= CFile::modeWrite;
		else
			nOpenFlags |= CFile::modeRead;
		if (bExclusive)
			nOpenFlags |= CFile::shareExclusive;
		else
			nOpenFlags |= CFile::shareDenyNone;
		bReturn = m_File.Open( lpstrFile, nOpenFlags);
	}
	catch ( CException *pEx)
	{
		pEx->Delete();
	}
	if (bReturn)
		m_bOpened = TRUE;
	return bReturn;
}

void CFilePackageHistory::SeekToBegin()
{
	if (m_bOpened)
		m_File.SeekToBegin();
}

void CFilePackageHistory::SeekToEnd()
{
	if (m_bOpened)
		m_File.SeekToEnd();
}

BOOL CFilePackageHistory::AddPackage(LPCTSTR lpstrPackageID)
{
	try
	{
		CString csBuffer;

		m_File.SeekToEnd();
		csBuffer.Format( _T( "%s\n"), lpstrPackageID);
		m_File.WriteString( csBuffer);
		return TRUE;
	}
	catch ( CException *pEx)
	{
		pEx->Delete();
	}
	return FALSE;
}

BOOL CFilePackageHistory::ReadPackage(CString &csPackageID)
{
	try
	{
		return m_File.ReadString( csPackageID);
	}
	catch ( CException *pEx)
	{
		pEx->Delete();
	}
	return FALSE;
}

void CFilePackageHistory::Close()
{
	try
	{
		if (m_bOpened)
			m_File.Close();
		m_bOpened = FALSE;
	}
	catch ( CException *pEx)
	{
		pEx->Delete();
	}
}

void CFilePackageHistory::Abort()
{
	try
	{
		if (m_bOpened)
			m_File.Abort();
		m_bOpened = FALSE;
	}
	catch ( CException *pEx)
	{
		pEx->Delete();
	}
}

int CFilePackageHistory::CleanDuplicates(LPCTSTR lpstrFile, BOOL bForce)
{
	CString				csBackFile,	// Old history file backup name
						csNewFile,	// Newly created history file, which is cleaned for duplicates
						csPackageID, // One package ID
						csPackageList; // List of all package IDs
	CFilePackageHistory	cFileTmp,	// Old history backup file
						cFile;		// Current/new history file

	try
	{
		csBackFile.Format( _T( "%s.bak"), lpstrFile);
		csNewFile.Format( _T( "%s.new"), lpstrFile);
		// Check if backup file exists
		if ((GetFileAttributes( csBackFile) != 0xFFFFFFFF) && !bForce)
			// File exists, but clean not forced
			return 2;
		// Open current file for reading
		if (!cFile.Open( lpstrFile))
			return 0;
		// Open new file for writing (ensure not existing)
		DeleteFile( csNewFile);
		if (!cFileTmp.Open( csNewFile, TRUE))
		{
			cFile.Close();
			return 0;
		}
		while (cFile.ReadPackage( csPackageID))
		{
			// Ensure package not already listed
			if (csPackageList.Find( csPackageID) == -1)
			{
				// Package not already listed in history
				if (!cFileTmp.AddPackage( csPackageID))
				{
					cFile.Close();
					cFileTmp.Abort();
					return 0;
				}
				// Add package to list
				csPackageList += (csPackageID + _T( " "));
			}
		}
		cFileTmp.Close();
		cFile.Close();
		// Make a backup of current history file
		if (!CopyFile( lpstrFile, csBackFile, FALSE))
		{
			// Remove newly created file
			DeleteFile( csNewFile);
			DeleteFile( csBackFile);
			return 0;
		}
		// Copy new file to old one
		if (!CopyFile( csNewFile, lpstrFile, FALSE))
		{
			// Remove newly created file
			DeleteFile( csNewFile);
			// Restore old file
			if (CopyFile( csBackFile, lpstrFile, FALSE))
				DeleteFile( csBackFile);
			return 0;
		}
		// Delete newly created temp file
		DeleteFile( csNewFile);
		DeleteFile( csBackFile);
		return 1;
	}
	catch ( CException *pEx)
	{
		pEx->Delete();
		cFile.Abort();
		cFileTmp.Abort();
		DeleteFile( csNewFile);
	}
	return 0;
}

BOOL CFilePackageHistory::AddPackage(LPCTSTR lpstrFile, LPCTSTR lpstrPackageID)
{
	CFilePackageHistory myFile;

	try
	{
		if (!myFile.Open( lpstrFile, TRUE))
			return FALSE;
		if (!myFile.AddPackage( lpstrPackageID))
		{
			myFile.Abort();
			return FALSE;
		}
		myFile.Close();
		return TRUE;
	}
	catch ( CException *pEx)
	{
		pEx->Delete();
		myFile.Abort();
	}
	return FALSE;
}
