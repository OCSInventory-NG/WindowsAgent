//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Download.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "config.h"
#include "log.h"
#include "ConnexionAbstract.h"
#include "ComProvider.h"
#include "BlackList.h"
#include "Package.h"
#include "resource.h"		// main symbols


// CDownloadApp:
// See Download.cpp for the implementation of this class
//

class CDownloadApp : public CWinApp
{
public:
	CDownloadApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// Read download configuration stored by agent
	BOOL readConfig();
	// Parse command line arguments
	BOOL parseCommandLine();
	// Retrieve the agent version from exe
	CString getVersion();
	// Wait agent to terminate
	BOOL waitAgent();

// Implementation
	DECLARE_MESSAGE_MAP()
protected: // Methods
	// Parse download directory to find suitable packages metadata
	// Return the number of package to process
	UINT findPackages();
	// Run a period, to download a fragment of each package, or execute package 
	BOOL runPeriod();
	// Read package result from "done" file, and send it to server
	BOOL setFinished( CPackage *pPack);
	// "Really" send package result code to server and optionally command output
	BOOL sendResult( LPCTSTR lpstrPackID, LPCTSTR lpstrCode, LPCTSTR lpstrOuput = NULL);
	// Download one package fragment
	BOOL downloadFragment( CPackage *pPack);
	// All fragment are doanloaded, create build.zip, verify signature, unzip and execute command
	BOOL executePackage( CPackage *pPack);

protected: //Attributes
	CLog	*m_pLogger;
	CConfig	*m_pConfig;
	CConnexionAbstract	*m_pConnexion;
	UINT64	m_uDownloadFragLatency;		// Time to wait between fragment downloaded
	UINT64	m_uDownloadCycleLatency;	// Time to wait between download cycle
	UINT64	m_uDownloadPeriodLatency;	// Time to wait between each cycle priority ( = period)
	UINT64	m_uDownloadPeriodLength;	// Number of period in a download cycle
	UINT64	m_uDownloadTimeout;			// Package deployment timeout in days
	UINT64	m_uCommandTimeout;			// Command Execution Timeout in minutes
	BOOL	m_bDownloadOn;				// Is download enabled
	CBlackList m_BlackList;				// Blacklist of package IDs, package to not process during the period
	CObArray m_listPack;				// Array of package to process
};

extern CDownloadApp theApp;
