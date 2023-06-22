//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Package.h: interface for the CPackage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _PACKAGE_H_INCLUDED_
#define _PACKAGE_H_INCLUDED_

#pragma once

class CPackage : public CObject
{
public:
	CPackage(void);
	~CPackage(void);

	// Load Package information from metadata file "info"
	BOOL load( LPCTSTR lpstrFile);
	// Get package ID (i.e timestamp)
	LPCTSTR getID();
	// Get package priority
	UINT64 getPriority();
	// Get Action
	LPCTSTR getAction();
	// Get if we have to schedule package setup at specified date
	LPCTSTR getSchedule();
	// Get package timestamp
	time_t getTimeStamp();
	// Check if package timestamp is older than now - tTimeOut hours
	BOOL isExpired( UINT uTimeOut);
	// Remove package directory
	BOOL clean();
	static BOOL clean( LPCTSTR lpstrID);
	// Is task file exists
	BOOL existTask();
	// Is done file exists
	BOOL existDone(); 
	// Create "task" file  and populate it with fragment to download
	BOOL createTask();
	// Get task result code from "done" file
	BOOL getDone( CString &csCode);
	// Get task result code, task output from "done" file
	BOOL getDone( CString &csCode, CString &csOutput);
	// Set task result code into "done" file, with optional command output
	BOOL setDone( LPCTSTR lpstrCode, LPCTSTR lpstrOutput = _T( ""));
	// Get execute try number from "exec_try" file
	BOOL getExecTry( UINT64 *puTry);
	// Set execute try number to "exec_try" file
	BOOL setExecTry( UINT64 uTry);
	// Is there any more frag to download (task filesize > 0)
	BOOL isFragTodownload();
	// Get next frag ID to download from task file (the first one)
	BOOL getFragToDownload( CString &csFragID);
	// Set frag downloaded into task file
	BOOL setFragDownloaded( LPCTSTR lpstrFragID);
	// Get URL where to download Fragment
	LPCTSTR getFragURL( LPCTSTR lpstrFragID);
	// Get path where to store fragment
	LPCTSTR getFragLocalPath( LPCTSTR lpstrFragID);
	// check if package not already built (is frag files exists, is built.zip exists)
	BOOL isBuilt();
	// Concatenete fragment to create build.zip, verify signature, delete fragment
	BOOL build();
	// Execute package using provided time out in minutes
	UINT64 execute( UINT64 uCommandTimeOut);
	// Execute post package execution command using provided time out in minutes
	UINT64 executePostCmd( UINT64 uCommandTimeOut);

	// User notification
	BOOL isNotifyUserRequired();
	BOOL isAbortAllowed();
	BOOL isDelayAllowed();
	UINT getNotifyCountdown();
	LPCTSTR getNotifyText();
	BOOL isDoneNotifyUserRequired();
	LPCTSTR getDoneNotifyText();

	// Scheduler
	BOOL isTimeToSetup();
	BOOL setScheduler();
	BOOL deleteScheduler();

protected:
	// Verify package signature. Return
	BOOL checkSignature();
	// Delete fragament files
	BOOL deleteFragment();
	// Unzip package
	BOOL unZip();
	// Is Digest base64 encoded
	BOOL isBase64();
	// Is Digest hexadecimal encoded
	BOOL isHexadecimal();
	// Read package digest from registry
	BOOL regReadPackageDigest( CString &csDigest);
	// Delete package digest from registry
	BOOL regDeletePackageDigest();
	// Is execution result successful
	BOOL isExecSuccessful( int nExitCode, CString &csStatus);

protected:
	CString m_csID;				// Package ID
	UINT m_uPriority;			// Package priority
	CString m_csAction;			// Action to perform (STORE, LAUNCH, EXECUTE)
	CString m_csName;			// Package name
	CString m_csDigest;			// Package digest
	CString m_csLocation;		// Where to download package fragments
	CString m_csProtocol;		// Protocol to use for fragment downloading
	UINT m_uFrags;				// Number of fragments
	CString m_csDigestAlgo;		// Digest algorithm
	CString m_csDigestEncode;	// Digest encoding algorithm
	CString m_csPath;			// Path for local package storage
	CString m_csStorePath;		// Path for store destination package
	CString m_csGardeFou;		// Command to validate package before install		
	CString m_csCommand;		// Command to execute when LAUNCH or EXECUTE action
	CString	m_csSchedule;		// Download fragment, but schedule setup at specified date and time
	CString	m_csPostCmd;		// Command to execute after package success
	time_t	m_tTimePack;		// Timestamp of package processing start

	/* User notification */
	BOOL m_bNotifyUser;			// Do we have to notify user before laucnhing action (but after downloading fragments)
	UINT m_uNotifyCountdown;	// Number of seconds before auto validation of user notification 
	CString m_csNotifyText;		// Before package action user notification text
	BOOL m_bNotifyCanAbort;		// Is user able to abort package setup
	BOOL m_bNotifyCanDelay;		// Is user able to report package setup at next agent execution
	BOOL m_bNeedDoneAction;		// Do we have to notify user after package successful execution
	CString m_csNeedDoneActionText; // After package success notification text
};

#endif // _PACKAGE_H_INCLUDED_
