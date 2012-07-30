//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
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
	UINT getPriority();
	// Get Action
	LPCTSTR getAction();
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
	BOOL getExecTry( UINT *puTry);
	// Set execute try number to "exec_try" file
	BOOL setExecTry( UINT uTry);
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
	UINT execute( UINT uCommandTimeOut);

	// User notification
	BOOL isNotifyUserRequired();
	BOOL isAbortAllowed();
	BOOL isDelayAllowed();
	UINT getNotifyCountdown();
	LPCTSTR getNotifyText();
	BOOL isDoneNotifyUserRequired();
	LPCTSTR getDoneNotifyText();

protected:
	// Verify package signature. Return
	BOOL checkSignature();
	// Delete fragament files
	BOOL deleteFragement();
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

protected:
	CString m_csID;
	UINT m_uPriority;
	CString m_csAction;
	CString m_csName;
	CString m_csDigest;
	CString m_csLocation;
	CString m_csProtocol;
	UINT m_uFrags;
	CString m_csDigestAlgo;
	CString m_csDigestEncode;
	CString m_csPath;
	CString m_csCommand;
	CString m_csGardeFou;
	time_t	m_tTimePack;

	/* User notification */
	BOOL m_bNotifyUser;
	UINT m_uNotifyCountdown;
	CString m_csNotifyText;
	BOOL m_bNotifyCanAbort;
	BOOL m_bNotifyCanDelay;
	BOOL m_bNeedDoneAction;
	CString m_csNeedDoneActionText;
};

#endif // _PACKAGE_H_INCLUDED_
