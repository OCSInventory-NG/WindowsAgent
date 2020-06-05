//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Software.cpp: implementation of the CSoftware class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Software.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoftware::CSoftware()
{
	Clear();
}

CSoftware::~CSoftware()
{
	Clear();
}

LPCTSTR CSoftware::GetPublisher()
{
	return m_csPublisher;
}

LPCTSTR CSoftware::GetName()
{
	return m_csName;
}

LPCTSTR CSoftware::GetVersion()
{
	return m_csVersion;
}

LPCTSTR CSoftware::GetFolder()
{
	return m_csFolder;
}

LPCTSTR CSoftware::GetComments()
{
	return m_csComments;
}

LPCTSTR CSoftware::GetGUID()
{
	return m_csGUID;
}

LPCTSTR CSoftware::GetLanguage()
{
	return m_csLanguage;
}

LPCTSTR CSoftware::GetFilename()
{
	return m_csFilename;
}

ULONG CSoftware::GetFilesize()
{
	return m_ulFilesize;
}

BOOL CSoftware::IsFromRegistry()
{
	return m_bFromRegistry;
}

LPCTSTR CSoftware::GetSource()
{
	if (m_bFromRegistry)
		return _T( "Registry");
	return _T( "Disk");
}

LPCTSTR CSoftware::GetInstallDate()
{
	return m_csInstallDate;
}

DWORD CSoftware::GetMemoryAddressWidth()
{
	return m_dwAddressWidth;
}

void CSoftware::Set( LPCTSTR lpstrPublisher, LPCTSTR lpstrName, LPCTSTR lpstrVersion, LPCTSTR lpstrFolder, LPCTSTR lpstrComments, LPCTSTR lpstrFilename, ULONG ulSize, BOOL bFromRegistry)
{
	m_csPublisher = lpstrPublisher;
	StrForSQL( m_csPublisher);
	m_csName = lpstrName;
	StrForSQL( m_csName);
	m_csVersion = lpstrVersion;
	StrForSQL( m_csVersion);
	m_csFolder = lpstrFolder;
	StrForSQL( m_csFolder);
	m_csComments = lpstrComments;
	StrForSQL( m_csComments);
	m_csFilename = lpstrFilename;
	StrForSQL( m_csFilename);
	m_ulFilesize = ulSize;
	m_bFromRegistry = bFromRegistry;
}

void CSoftware::SetPublisher(LPCTSTR lpstrPublisher)
{
	m_csPublisher = lpstrPublisher;
	StrForSQL( m_csPublisher);
}

void CSoftware::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CSoftware::SetVersion(LPCTSTR lpstrVersion)
{
	m_csVersion = lpstrVersion;
	StrForSQL( m_csVersion);
}

void CSoftware::SetFolder(LPCTSTR lpstrFolder)
{
	m_csFolder = lpstrFolder;
	StrForSQL( m_csFolder);
}

void CSoftware::SetComments(LPCTSTR lpstrComments)
{
	m_csComments = lpstrComments;
	StrForSQL( m_csComments);
}

void CSoftware::SetGUID(LPCTSTR lpstrGUID)
{
	m_csGUID = lpstrGUID;
	StrForSQL(m_csGUID);
}

void CSoftware::SetLanguage(LPCTSTR lpstrLanguage)
{
	m_csLanguage = lpstrLanguage;
	StrForSQL( m_csLanguage);
}

void CSoftware::SetFilename(LPCTSTR lpstrFilename)
{
	m_csFilename = lpstrFilename;
	StrForSQL( m_csFilename);
}

void CSoftware::SetFilesize(ULONG ulSize)
{
	m_ulFilesize = ulSize;
}

void CSoftware::SetFromRegistry( BOOL bFromRegistry)
{
	m_bFromRegistry = bFromRegistry;
}

void CSoftware::SetInstallDate( LPCTSTR lpstrDate, BOOL bFormat)
{
	if ((lpstrDate == NULL) ||(_tcslen( lpstrDate) < 8) || (!bFormat))
		m_csInstallDate = lpstrDate;
	else
	{
		// Format date in YYYY/MM/DD
		m_csInstallDate = lpstrDate;
		m_csInstallDate.Insert( 4, '/');
		m_csInstallDate.Insert( 7, '/');
	}
}

void CSoftware::SetMemoryAddressWidth( DWORD dwWidth)
{
	m_dwAddressWidth = dwWidth;
}

void CSoftware::Clear()
{
	m_csPublisher.Empty();	
	m_csName.Empty();		
	m_csVersion.Empty();	
	m_csFolder.Empty();		
	m_csComments.Empty();	
	m_csGUID.Empty();	
	m_csLanguage.Empty();	
	m_csFilename.Empty();	
	m_ulFilesize = 0;	
	m_bFromRegistry = FALSE;
	m_csInstallDate = _T( "0000/00/00");
	m_dwAddressWidth = 0;
}

int CSoftware::operator==(CSoftware cObject) const
{
	return ((m_csPublisher == cObject.GetPublisher()) &&
		(m_csName == cObject.GetName()) &&
		(m_csVersion == cObject.GetVersion()) &&
		(m_csFolder == cObject.GetFolder()) &&	
		(m_csComments == cObject.GetComments()) &&
		(m_csGUID == cObject.GetGUID()) &&
		(m_csLanguage == cObject.GetLanguage()) &&
		(m_csFilename == cObject.GetFilename()) &&
		(m_ulFilesize == cObject.GetFilesize()) &&
		(m_bFromRegistry == cObject.IsFromRegistry()) &&
		(m_csInstallDate == cObject.GetInstallDate()) &&
		(m_dwAddressWidth == cObject.GetMemoryAddressWidth())
		);
}
