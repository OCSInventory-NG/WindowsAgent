//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Download.cpp: implementation of the CDownload class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CapDownload.h"
#include "CapacityAbstract.h"
#include "ConnexionAbstract.h"
#include "DownloadRequest.h"
#include "DownloadResponse.h"
#include "ExecCommand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCapDownload::CCapDownload()
{
}

CCapDownload::~CCapDownload()
{
	// Empty list of package to download
	LONG l = 0;
	while (l < m_tPackages.GetSize())
	{
		delete m_tPackages[l];
		l++;
	}	
}

BOOL CCapDownload::retrievePackages()
{
	CString					csCertFile,
							csId,
							csValue;
	COptDownloadPackage		*pOptDownloadPackage;
	CMapStringToStringArray	*pMapArray = NULL;
	CMapStringToString		*pMap = NULL;
	INT_PTR					nPack = 0;

	/***
	*
	* Environnement checking
	*
	***/

	// Working directory
	if (directoryCreate( getDownloadFolder()) == FALSE)
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Cannot create working directory (%s)"), LookupError( GetLastError()));
		return FALSE;
	}
	// Open package history file, create it if needed
	CFilePackageHistory cFileHistory;
	if (! cFileHistory.Open( getPackageHistoryFilename(), FALSE, TRUE))
	{
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Cannot create history file <%>"), getPackageHistoryFilename());
		return FALSE;
	}
	if (! m_pPrologResp->isDownloadRequired())
	{
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => No package available for download"));
		return FALSE;
	}
	// Trying to create suspend Download tool
	if (!suspendDownload())
	{
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Cannot suspend Download and Setup Tool using <%s> file"), OCS_DOWNLOAD_SUSPEND);
		return FALSE;
	}
	// Trying to get exclusive access to download
	if (!lockDownload())
	{
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Cannot lock directory <%s>"), getDownloadFolder());
		resumeDownload();
		return FALSE;
	}
	// Get generic download parameters and write them for using with download tool
	pMapArray = m_pPrologResp->getDownloadParameters();
	if ((pMapArray == NULL) || pMapArray->IsEmpty())
	{
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => No download parameter available"));
		unlockDownload();
		resumeDownload();
		return FALSE;
	}
	// There is only one record for download parameters
	pMap = pMapArray->GetAt(0);
	pMap->Lookup( _T( "FRAG_LATENCY"), m_csDownloadFragLatency);
	pMap->Lookup( _T( "CYCLE_LATENCY"), m_csDownloadCycleLatency);
	pMap->Lookup( _T( "PERIOD_LATENCY"), m_csDownloadPeriodLatency);
	pMap->Lookup( _T( "PERIOD_LENGTH"), m_csDownloadPeriodLength);
	pMap->Lookup( _T( "TIMEOUT"), m_csDownloadTimeout);
	pMap->Lookup( _T( "ON"), m_csDownloadOn);
	writeConfig();
	delete pMapArray;
	pMapArray = NULL;
	
	// Now get each package information
	pMapArray = m_pPrologResp->getDownloadPackages();
	for (nPack=0; (pMapArray!=NULL) && (nPack<pMapArray->GetCount()); nPack++)
	{
		if (((pMap = pMapArray->GetAt(nPack)) == NULL) || pMap->IsEmpty())
			continue;
		csId.Empty();
		pMap->Lookup( _T( "ID"), csId);
		// Try to find if package was not previously downloaded, parsing package history file
		CString csHistBuf;
		BOOL	bFound = FALSE;
		cFileHistory.SeekToBegin();
		while (cFileHistory.ReadPackage( csHistBuf))
		{
			if( csHistBuf.Find( csId) != -1 )
			{
				// Package ID found in history
				bFound = TRUE;
				break;
			}
		}
		pOptDownloadPackage = new COptDownloadPackage( this);
		pOptDownloadPackage->setId( csId);
		// If CERT_PATH or CERT_FILE option is provided
		pMap->Lookup( _T( "CERT_PATH"), csValue);
		pOptDownloadPackage->setCertPath( csValue);
		pMap->Lookup( _T( "CERT_FILE"), csValue);
		pOptDownloadPackage->setCertFile( csValue);
		// Set URL where to download INFO metadata
		pMap->Lookup( _T( "INFO_LOC"), csValue);
		pOptDownloadPackage->setInfoLocation( csValue);
		// Set URL where to download fragment
		pMap->Lookup( _T( "PACK_LOC"), csValue);
		pOptDownloadPackage->setPackLocation( csValue);
		if (bFound)				
		{
			// Package ID found in history, do not download
			m_pLogger->log(LOG_PRIORITY_NOTICE,  _T( "DOWNLOAD => Will not download package <%s>, already in the package history"), csId);
			sendMessage( csId, ERR_ALREADY_SETUP);
			// Delete already download directory if needed 
			pOptDownloadPackage->clean();
			delete pOptDownloadPackage;
		}
		else
			// Package not already downloaded, put it in the download queue
			m_tPackages.Add(pOptDownloadPackage);
	}	
	cFileHistory.Close();
	delete pMapArray;
	// Cleaning file history for duplicates
	switch (CFilePackageHistory::CleanDuplicates( getPackageHistoryFilename()))
	{
	case 1:
		m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DOWNLOAD => Package history file successfully cleaned for duplicate IDs"));
		break;
	case 2:
		m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DOWNLOAD => Package history file cleaning not required"));
		break;
	default:
		m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DOWNLOAD => Failed to clean Package history file for duplicate IDs"));
		break;
	}

	// Now, prepare directories and download instructions for download tool
	for (nPack=0; nPack<m_tPackages.GetSize(); nPack++)
	{
		pOptDownloadPackage = (COptDownloadPackage*) (m_tPackages[nPack]);
		// Check if package is not expired
		if (pOptDownloadPackage->isExpired( m_csDownloadTimeout))
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Package <%s> timed out (now:%lu, since:%lu, Timeout:%s)"), pOptDownloadPackage->getId(), time( NULL), pOptDownloadPackage->getTimeStamp(), m_csDownloadTimeout);
			if (sendMessage( pOptDownloadPackage->getId(), ERR_TIMEOUT))
				pOptDownloadPackage->clean();
		}
		else
		{
			// Check if package not already added to download queue
			if (pOptDownloadPackage->makeDirectory() && !fileExists( pOptDownloadPackage->getLocalMetadataFilename()))
			{
				// Download metadata from deployment server
				if (pOptDownloadPackage->downloadInfoFile())
					m_pLogger->log(LOG_PRIORITY_NOTICE,  _T( "DOWNLOAD => Package <%s> added to download queue"), pOptDownloadPackage->getId());
				else
					// Error dowloading metadata => remove package directory to avoid error message into download tool
					pOptDownloadPackage->clean();
			}
		}
	}
	// Now, allow Download tool
	unlockDownload();
	resumeDownload();
	return TRUE;
}

BOOL CCapDownload::writeConfig()
{
	CString csFileName;
	BOOL	bResult = TRUE;

	csFileName.Format( _T( "%s\\%s"), getDownloadFolder(), OCS_CONFIG_FILENAME);

	bResult = WritePrivateProfileString( OCS_AGENT_SECTION, _T( "FragLatency"), m_csDownloadFragLatency, csFileName);
	bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "CycleLatency"), m_csDownloadCycleLatency, csFileName);
	bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "PeriodLatency"), m_csDownloadPeriodLatency, csFileName);
	bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "PeriodLength"), m_csDownloadPeriodLength, csFileName);
	bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "Timeout"), m_csDownloadTimeout, csFileName);
	bResult = bResult && WritePrivateProfileString( OCS_AGENT_SECTION, _T( "On"), m_csDownloadOn, csFileName);
	return bResult;
}

BOOL CCapDownload::launch()
{
	CExecCommand cmProcess;
	CString csCmd;

	if( m_csDownloadOn ==  _T( "0"))
	{
		m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DOWNLOAD => Download disabled by server"));
		return TRUE;
	}
	// Launch download tool with agent provided command line args
	csCmd.Format( _T( "\"%s\\download.exe\" %s"), getInstallFolder(), AfxGetApp()->m_lpCmdLine);
	if (cmProcess.execNoWait( csCmd, getDataFolder()) != EXEC_SUCCESSFULL)
	{
		m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DOWNLOAD => Can't create OCS Inventory NG Download process (%s)"), cmProcess.getOutput());
		return FALSE;
	}
	m_pLogger->log(LOG_PRIORITY_NOTICE,  _T( "DOWNLOAD => Download and setup tool successfully started"));
	return TRUE;
} 

BOOL CCapDownload::sendMessage( LPCTSTR lpstrPackID, LPCTSTR lpstrCode)
{
	CConfig				*pAgentConfig = getAgentConfig();
	CComProvider		*pProvider = getComServerProvider();
	CServerConfig		*pServerConfig = pProvider->newConfig();
	CConnexionAbstract	*pConnexion = NULL;

	ASSERT( lpstrPackID);
	ASSERT( lpstrCode);
	ASSERT( pAgentConfig);
	ASSERT( pProvider);
	ASSERT( pServerConfig);

	// Create provider connection object using default Provider configuration
	if ((pConnexion = pProvider->newConnexion( pServerConfig)) == NULL)
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed creating connection for Communication Provider <%s>"), pAgentConfig->getCommunicationProvider());
		pProvider->deleteConfig( pServerConfig);
		return FALSE;
	}
	// Download metadata info file
	m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Sending result code request <%s> for package <%s>"), lpstrCode, lpstrPackID);
	CDownloadRequest	cRequest;
	CDownloadResponse*	pResponse = NULL;
	// Send the download request
	cRequest.setPackageResult( lpstrPackID, lpstrCode);
	pResponse = new CDownloadResponse( pConnexion->sendRequest( &cRequest));
	pResponse->setErrorStatus( pConnexion->getErrorStatus());
	pResponse->setErrorString( pConnexion->getErrorString());
	if (pResponse->isSuccess())
	{
		m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Result code request successfully sent"));
		cRequest.setSuccess();
	}
	else
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed to send result code request for package <%s> (%s)"), lpstrPackID, pResponse->getErrorString());
		delete pResponse;
		pProvider->deleteConnexion( pConnexion);
		pProvider->deleteConfig( pServerConfig);
		return FALSE;
	}
	delete pResponse;
	m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DOWNLOAD => Unloading communication provider"));
	// Use provider to delete connexion and server config
	pProvider->deleteConnexion( pConnexion);
	pProvider->deleteConfig( pServerConfig);

	return TRUE;

}

BOOL CCapDownload::checkOcsAgentSetupResult()
{
	CString csFile, csCode = ERR_DONE_FAILED, csID;
	CStdioFile myFile;

	csFile.Format( _T( "%s\\%s"), getDownloadFolder(), OCS_AGENT_SETUP_DONE);
	if (!fileExists( csFile))
		// No OCS Agent Setup done file
		return TRUE;
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Found OCS Inventory Agent Setup result file <%s>"), csFile);
	// Open OCS Agent Setup done file to read exit code and package ID
	try
	{
		if (!myFile.Open( csFile, CFile::modeRead|CFile::typeText|CFile::shareDenyNone))
			return FALSE;
		// First line contains result code and seconf line contains package ID
		myFile.ReadString( csCode);
		myFile.ReadString( csID);
		myFile.Close();
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		myFile.Abort();
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed reading OCS Inventory Agent Setup result file <%s>"), csFile);
		return FALSE;
	}
	if (csID.IsEmpty())
	{
		// Upgrading from agent 1.X or previous to 2.0.0.22 ?
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Found result code <%s> for OCS Inventory Agent Setup package but no package ID specified, so unable to send result code"), csCode);
		return FALSE;
	}
	// All information available => try to register package and send result code
	if (csCode.Compare( CODE_SUCCESS) == 0)
	{
		// Package execute successful
		if (!CFilePackageHistory::AddPackage( getPackageHistoryFilename(), csID))
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Cannot add OCS Inventory Agent Setup package <%s> to History file <%s>"), csID, getPackageHistoryFilename());
	}
	else
	{
		// Package execute failed
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Will not register OCS Inventory Agent Setup package <%s> in history: result <%s> not a success"), csID, csCode);
	}
	m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Sending result code <%s> for OCS Inventory Agent Setup package <%s>"), csCode, csID);
	if (sendMessage( csID, csCode))
	{
		// Agent setup result code successfully sent => delete result file and registry package digest
		DeleteFile( csFile);
	}
	// Delete download directory of package, to avoid Download tool running setup another time
	if (!COptDownloadPackage::clean( csID))
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed to delete directory of package <%s>"), csID);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COptDownloadPackage::COptDownloadPackage( CCapDownload * pC)
{
	pM=pC; 
	m_pLogger=getOcsLogger();
};


void COptDownloadPackage::setId( LPCTSTR lpstrId)
{
	m_csId = lpstrId;
}

void COptDownloadPackage::setCertPath( LPCTSTR lpstrCertPath)
{
	m_csCertPath = lpstrCertPath;
	// Expand INSTALL_PATH if needed
	m_csCertPath.Replace(  _T( "INSTALL_PATH"),  getDataFolder());
	m_csCertPath.Replace(  _T( "/"),  _T( "\\") );
}

void COptDownloadPackage::setCertFile( LPCTSTR lpstrCertFile)
{
	m_csCertFile = lpstrCertFile;
	// Expand INSTALL_PATH if needed
	m_csCertFile.Replace(  _T( "INSTALL_PATH"),  getDataFolder());
	m_csCertFile.Replace(  _T( "/"),  _T( "\\") );
}

void COptDownloadPackage::setInfoLocation( LPCTSTR lpstrInfoLoc)
{
	m_csRemoteInfoLoc.Format( _T( "https://%s/%s/%s"), lpstrInfoLoc, m_csId, OCS_DOWNLOAD_METADATA);
	m_csLocalInfoLoc.Format( _T( "%s\\%s\\%s\\%s"), getDataFolder(), OCS_DOWNLOAD_FOLDER, m_csId, OCS_DOWNLOAD_METADATA);
}

void COptDownloadPackage::setPackLocation( LPCTSTR lpstrPackLoc)
{
	m_csRemotePackLoc = lpstrPackLoc;
	m_csLocalPackLoc.Format( _T( "%s\\%s\\%s"), getDataFolder(), OCS_DOWNLOAD_FOLDER, m_csId);
}

BOOL COptDownloadPackage::makeDirectory()
{
	if (!directoryCreate( m_csLocalPackLoc))
	{
		m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DOWNLOAD => Cannot create package <%s> directory (%s)"), m_csId, LookupError( GetLastError()));
		return FALSE;
	}
	return TRUE;
}

CString COptDownloadPackage::getId()
{
	return m_csId;
}

LPCTSTR COptDownloadPackage::getLocalMetadataFilename()
{
	return m_csLocalInfoLoc;
}

LPCTSTR COptDownloadPackage::getRemoteMetadataURL()
{
	return m_csRemoteInfoLoc;
}

LPCTSTR COptDownloadPackage::getLocalPackFolder()
{
	return m_csLocalPackLoc;
}

LPCTSTR COptDownloadPackage::getRemotePackURL()
{
	return m_csRemotePackLoc;
}

int COptDownloadPackage::downloadInfoFile()
{
	CConfig				*pAgentConfig = getAgentConfig();
	CComProvider		*pProvider = getComServerProvider();
	CServerConfig		*pServerConfig = pProvider->newConfig();
	CConnexionAbstract	*pConnexion = NULL;
	TCHAR				szDummy[_MAX_PATH+1];

	ASSERT( pAgentConfig);
	ASSERT( pProvider);
	ASSERT( pServerConfig);

	// Create provider connection object using default Provider configuration
	if ((pConnexion = pProvider->newConnexion( pServerConfig)) == NULL)
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed creating connection for Communication Provider <%s>"), pAgentConfig->getCommunicationProvider());
		pProvider->deleteConfig( pServerConfig);
		return FALSE;
	}
	// Download metadata info file
	m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Metadata file <info> for package <%s> is located at <%s>"), m_csId, getRemoteMetadataURL());
	if (!pConnexion->getFile( getRemoteMetadataURL(), getLocalMetadataFilename()))
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed to download Metadata file <%s> to <%s>"), getRemoteMetadataURL(), getLocalMetadataFilename());
		pProvider->deleteConnexion( pConnexion);
		pProvider->deleteConfig( pServerConfig);
		return FALSE;
	}
	m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DOWNLOAD => Unloading communication provider"));
	// Use provider to delete connexion and server config
	pProvider->deleteConnexion( pConnexion);
	pProvider->deleteConfig( pServerConfig);

	// Open metadata file to add fragment location
	CString csBuffer;
	CMarkup xml;
	if(!xml.LoadFile( getLocalMetadataFilename()))
	{
		m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DOWNLOAD => Cannot read or parse Metadata file <%s>"), getLocalMetadataFilename());
		return FALSE;
	}
	// Add fragment location to meta data
	xml.FindFirstElem( _T( "DOWNLOAD"));
	xml.SetAttrib( _T( "LOC"), m_csRemotePackLoc);
	// Write meta data file
	if (!xml.SaveFile( getLocalMetadataFilename()))
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Can't update Metadata file <%s>"), getLocalMetadataFilename());
		return FALSE;
	}
	// Compute digest on meta data and add it to Registry
	if (!fileDigest( getLocalMetadataFilename(), csBuffer))
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Can't register package <%s> into Registry"), m_csId);
		DeleteFile( getLocalMetadataFilename());
		return FALSE;
	}
	if (!regAddPackageDigest( m_csId, csBuffer))
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Can't register package <%s> into Registry"), m_csId);
		DeleteFile( getLocalMetadataFilename());
		return FALSE;
	}
	// Now create a timestamp 
	csBuffer.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csId, OCS_DOWNLOAD_TIMESTAMP);
	if (!fileExists( csBuffer))
	{
		_ltot( time( NULL), szDummy, 10);
		if (!WriteTextToFile( szDummy, csBuffer))
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Can't create timestamp file <%s>"), csBuffer);
	}
	else
		m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Timestamp file <%s> already exists"), csBuffer);
	m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DOWNLOAD => Retrieve info file...OK (pack %s)"), m_csId );
    return TRUE;
}

BOOL COptDownloadPackage::regAddPackageDigest( LPCTSTR lpstrPackID, LPCTSTR lpstrDigest)
{
	HKEY  hKey;
	DWORD dwValue;

	if (RegCreateKeyEx( HKEY_LOCAL_MACHINE, OCS_DOWNLOAD_REGISTRY, 0, NULL, REG_OPTION_NON_VOLATILE,
						KEY_WRITE, NULL, &hKey, &dwValue) != ERROR_SUCCESS) 
		return FALSE;
	dwValue = _tcslen( lpstrDigest)*sizeof( TCHAR);
	if (RegSetValueEx( hKey, lpstrPackID, 0, REG_SZ, (LPBYTE) lpstrDigest, dwValue) != ERROR_SUCCESS)
	{
		RegCloseKey( hKey); 
		return FALSE;
	}
	RegCloseKey( hKey); 
	return TRUE;
}

BOOL COptDownloadPackage::regDeletePackageDigest( LPCTSTR lpstrPackID)
{
	HKEY  hKey;

	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, OCS_DOWNLOAD_REGISTRY, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
		return FALSE;
	if (RegDeleteValue( hKey, lpstrPackID) != ERROR_SUCCESS)
	{
		RegCloseKey( hKey);
		return FALSE;
	}
	RegCloseKey( hKey);
	return TRUE;
}

BOOL COptDownloadPackage::isExpired( LPCTSTR csTimeOut)
{
	time_t	tTimeNow;
	UINT	uTimeOut = _ttol( csTimeOut);

	tTimeNow = time( NULL);
	return (((tTimeNow - getTimeStamp())/86400) >  uTimeOut);
}

BOOL COptDownloadPackage::clean()
{
	return clean( m_csId);
}

BOOL COptDownloadPackage::clean( LPCTSTR lpstrID)
{
	CString csPath;

	ASSERT( lpstrID);

	// Delete tmp path folder where package was unzipped (not an eror if not existing)
	if (GetTempPath( _MAX_PATH, csPath.GetBufferSetLength( _MAX_PATH+1)) == 0)
		return FALSE;
	csPath.ReleaseBuffer();
	csPath.AppendFormat( _T( "\\%s.OCS"), lpstrID);
	directoryDelete( csPath);
	// Now, really delete package directory and registry signature
	csPath.Format( _T( "%s\\%s"), getDownloadFolder(), lpstrID);
	return (regDeletePackageDigest( lpstrID) && directoryDelete( csPath));
}

time_t COptDownloadPackage::getTimeStamp()
{
	CString csFilename, csTimestamp;

	// Check if not already retrieved from file
	if (m_tTimePack != 0)
		return m_tTimePack;

	try
	{
		// Load "since" file content
		csFilename.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csId, OCS_DOWNLOAD_TIMESTAMP);
		if (!LoadFileToText( csTimestamp, csFilename))
		{
			m_tTimePack = 0;
			return m_tTimePack;
		}
		m_tTimePack = (time_t) _ttol( csTimestamp);
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		m_tTimePack = 0;
	}
	return m_tTimePack;
}
