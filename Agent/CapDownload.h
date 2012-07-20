//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Download.h: interface for the CDownload class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNLOAD_H__13358678_594C_47B8_A870_09E6F2BBE410__INCLUDED_)
#define AFX_DOWNLOAD_H__13358678_594C_47B8_A870_09E6F2BBE410__INCLUDED_

#pragma once

#include "commonDownload.h"
#include "OCSInventory.h"


class CCapDownload : public CCapacityAbstract
{
public: // Methods

	CCapDownload();
	virtual ~CCapDownload();

	// Get download package informations from server
	BOOL retrievePackages();
	// Write download tool general configuration parameters
	BOOL writeConfig();
	// Launch download in another process, using the download tool
	BOOL launch();
	
	// Check for OCSNG Windows Agent Setup execution result, and send result to server if found
	// This is a special work because OCS Agent Setup kill download process. So we loose package infos
	// such as package ID and temporary unzip folder. As a workaround, Agent setup create a result file 
	// OCSNG-Windows-Agent-Setup_done into Agent's download folder to store package ID and result.  
	// We have to read this file if exists, and send result message to server.
	BOOL checkOcsAgentSetupResult();

protected: // Methods
	// Send package result code to server
	BOOL sendMessage(LPCTSTR lpstrPackID, LPCTSTR lpstrCode);

protected: // Attributes
	CObArray m_tPackages;				// Array of package to process
	CString m_csDownloadFragLatency;	// Time to wait betwen fragment downloaded
	CString m_csDownloadCycleLatency;	// Time to wait between download cycle
	CString m_csDownloadPeriodLatency;	// Time to wait between each cycle priority ( = period)
	CString m_csDownloadPeriodLength;	// Number of period in a download cycle
	CString m_csDownloadTimeout;		// Timeout
	CString m_csDownloadOn;				// Is download enabled
};


class COptDownloadPackage : public CObject
{
public: // Methods
	
	// Standard constructor
	COptDownloadPackage( CCapDownload * pC);

	// Set package ID
	void setId(LPCTSTR lpstrId);
	// set path to CA certificate directory storage
	void setCertPath(LPCTSTR lpstrCertPath);
	// Set path to CA certificate file
	void setCertFile(LPCTSTR lpstrCertFile);
	// Set URL of remote metadata file on server
	void setInfoLocation(LPCTSTR lpstrInfoLoc);
	// Set URL of remote fragment files on server
	void setPackLocation(LPCTSTR lpstrPackLoc);
	// Get package ID
	CString getId();
	// Create loca directory to receive downloaded metadata and fragments
	BOOL makeDirectory();
	// Get path of loca metadata file
	LPCTSTR getLocalMetadataFilename();
	// Get URL of remote metadata file on server
	LPCTSTR getRemoteMetadataURL();
	// Get path of local fragment files
	LPCTSTR getLocalPackFolder();
	// Get URL of remote fragment files on server
	LPCTSTR getRemotePackURL();
	// Download metadata from remote server to local
	BOOL downloadInfoFile();
	// Get package timestamp
	time_t getTimeStamp();
	// Check if package is expired
	BOOL isExpired( LPCTSTR csTimeOut);
	// Remove package directory
	BOOL clean();
	static BOOL clean( LPCTSTR lpstrID);

protected:
	// Add and delete package digest to registry
	BOOL regAddPackageDigest( LPCTSTR lpstrPackID, LPCTSTR lpstrDigest);
	static BOOL regDeletePackageDigest( LPCTSTR lpstrPackID);

protected: // Attributes
	CString		m_csId;				// Download package ID
	CString		m_csCertFile;		// Certificate file to use to validate server
	CString		m_csCertPath;		// Certificate path to use to validate server
	CString		m_csRemoteInfoLoc;	// Remote directory on server where to find metadata file
	CString		m_csLocalInfoLoc;	// Local directory where to store/find metadata file
	CString		m_csRemotePackLoc;	// Remote directory on server where to find fragments
	CString		m_csLocalPackLoc;	// Local directory where to store/find fragments
	time_t		m_tTimePack;
	CCapDownload *pM;
	CLog		*m_pLogger;
};

#endif // !defined(AFX_DOWNLOAD_H__13358678_594C_47B8_A870_09E6F2BBE410__INCLUDED_)
