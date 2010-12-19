//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Software.h: interface for the CSoftware class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOFTWARE_H__C4903B6B_8BDE_11D4_8077_0040055338AF__INCLUDED_)
#define AFX_SOFTWARE_H__C4903B6B_8BDE_11D4_8077_0040055338AF__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

#define MICROSOFT_CORP_STRING	 _T( "Microsoft Corporation")

class SYSINFO_API CSoftware  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CSoftware();
	virtual ~CSoftware();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	// Get the app publisher
	LPCTSTR GetPublisher();
	// Get the app friendly name
	LPCTSTR GetName();
	// Get the app version
	LPCTSTR GetVersion();
	// Get the app folder location C:\MyAppFolder
	LPCTSTR GetFolder();
	// Get the app comments
	LPCTSTR GetComments();
	// Get the app GUID
	LPCTSTR GetGUID();
	// Get the app language
	LPCTSTR GetLanguage();
	// Get the app filename
	LPCTSTR GetFilename();
	// Get the app filezie
	ULONG GetFilesize();
	// Is app extracted from registry
	BOOL IsFromRegistry();
	// Get the source in string format
	LPCTSTR GetSource();
	// Get the installation date
	LPCTSTR GetInstallDate();
	// Get Memory Address Width (32/64 bits)
	DWORD GetMemoryAddressWidth();


	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	// Set the Software attributes
	void Set( LPCTSTR lpstrPublisher, LPCTSTR lpstrName, LPCTSTR lpstrVersion, LPCTSTR lpstrFolder, LPCTSTR lpstrComments, LPCTSTR lpstrFilename = NOT_AVAILABLE, ULONG ulSize = 0, BOOL bFromRegistry = FALSE);
	// Set the publisher name
	void SetPublisher( LPCTSTR lpstrPublisher);
	// Set the app name
	void SetName( LPCTSTR lpstrName);
	// Set the app version
	void SetVersion( LPCTSTR lpstrVersion);
	// Set the app folder location
	void SetFolder( LPCTSTR lpstrFolder);
	// Set the app comments
	void SetComments( LPCTSTR lpstrComments);
	// Set the app GUID
	void SetGUID( LPCTSTR lpstrGUID);
	// Set the app language
	void SetLanguage( LPCTSTR lpstrLanguage);
	// Set the app filename
	void SetFilename( LPCTSTR lpstrFilename);
	// Set the app filesize
	void SetFilesize( ULONG ulSize);
	// Set the installation date, and insert '/' to format it as YYYY/MM/DD f asked
	void SetInstallDate( LPCTSTR lpstrDate, BOOL bFormat = FALSE);
	// Set Memory Address Width (32/64 bits)
	void SetMemoryAddressWidth( DWORD dwWidth);
	// Set if the app is extracted from the registry
	void SetFromRegistry( BOOL bFromRegistry = FALSE);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CSoftware cObject) const;

protected: // Attributes
	CString m_csPublisher;	// Publisher Microsoft
	CString m_csName;		// Name Word
	CString m_csVersion;	// Version
	CString m_csFolder;		// File folder C:\MyAppDir
	CString m_csComments;	// Comments
	CString m_csGUID;		// GUID of app
	CString m_csLanguage;	// Language of app
	CString m_csFilename;	// Filename of app
	ULONG	m_ulFilesize;	// Size in bytes of file app
	DWORD	m_dwAddressWidth; // Memory address width (32/64 bits)
	CString m_csInstallDate; // Software installation date
	BOOL	m_bFromRegistry;// Extracted from registry ?
};
#endif // !defined(AFX_SOFTWARE_H__C4903B6B_8BDE_11D4_8077_0040055338AF__INCLUDED_)

