//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsService.h : OCS Inventory Service processes
//

#ifndef _OCSSERVICE_H_INCLUDED_
#define _OCSSERVICE_H_INCLUDED_

#pragma once

#include "ntservice.h"
#include "ntservicemsg.h"

#define OCS_SERVICE_DESCRIPTION		_T( "Service starting periodically OCS Inventory NG Agent for Windows")
#define OCS_SERVICE_DEPENDANCIES	_T( "RPCSS\0EventLog\0WMI\0Tcpip\0\0")

#define	OCS_SERVICE_RAND_FILE		_T( "rand")
#define	OCS_SERVICE_TTO_WAIT		_T( "TTO_WAIT")
#define	OCS_SERVICE_OLD_PROLOG_FREQ	_T( "OLD_PROLOG_FREQ")
#define	RUN_OCS						_T( "ocsinventory.exe")
#define	PROLOG_FREQ_UNIT			3600 // seconds
#define	DEFAULT_PROLOG_FREQ			10	// * PROLOG_FREQ_UNIT
#define	WRITE_TTOWAIT_EACH			60	// seconds
// System must have an uptime less than 2 minutes for considering it is starting
#define SYSTEM_START_MAX_UPTIME		120000 // milliseconds

class COcsService :
	public CNTService
{
public:
	COcsService();
	~COcsService();
	BOOL OnInit();
	void OnStop();
	void Run();
	// Process messages send by user (run inventory now, show inventory)
	BOOL OnUserControl( DWORD dwOpcode);
	// Parse command line args
	//	-install to register service into Windows Service Manager
	//	-uninstall to unregister service
	//	-? to show version
    BOOL ParseCommandLine( int argc, LPTSTR argv[]);

protected:
	// Generate random number between 0 and nMax
	int generateRandNumber(int nMax);
	// Open file lpstrFile in directory lpstrFolder for denying write and store opened object into m_tHandles
	BOOL protectFile(LPCTSTR lpstrFolder, LPCTSTR lpstrFile);
	// Open required OCS files to prevent delete
	BOOL protectFiles();
	// Close all file object referenced into m_tHandles
	BOOL UnprotectFiles();
	// Load service configuration from file
	BOOL loadConfig();
	// Store service configuration to file (if not full, just write TTO_WAIT)
	BOOL writeConfig( BOOL bFull = TRUE);
	// Check if something changed in inventory
	BOOL CheckInventoryState();
	// Launch OCS Inventory Agent, using notify mode or not
	BOOL runAgent( BOOL bNotify = FALSE);
	// Show inventory informations
	BOOL showInventory();

protected:
	int			m_iTToWait;				// Time to wait before next agent run (in seconds)
	int			m_iPrologFreq;			// Time between 2 agent run
	int			m_iOldPrologFreq;		// Last time between 2 agent run
	int			m_iWriteIniLatency;		// Time between TTO_WAIT write to file to save it
	CObArray	m_tHandles;				// Handle of opened files to protect them from being deleted
};

#endif // _OCSSERVICE_H_INCLUDED_