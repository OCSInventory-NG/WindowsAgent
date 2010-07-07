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
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
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
	CString csCertFile, csId, csLoc, csType, csTmpValue;
	COptDownloadPackage* pOptDownloadPackage;
	LONG lPackNum = 0;

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
	CMapStringToString *pmConf = m_pPrologResp->getDownloadParameters();
	if ( pmConf->GetCount() == 0 )
	{
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => No download parameter available"));
		unlockDownload();
		resumeDownload();
		return FALSE;
	}
	pmConf->Lookup( _T( "FRAG_LATENCY"), m_csDownloadFragLatency);
	pmConf->Lookup( _T( "CYCLE_LATENCY"), m_csDownloadCycleLatency);
	pmConf->Lookup( _T( "PERIOD_LATENCY"), m_csDownloadPeriodLatency);
	pmConf->Lookup( _T( "PERIOD_LENGTH"), m_csDownloadPeriodLength);
	pmConf->Lookup( _T( "TIMEOUT"), m_csDownloadTimeout);
	pmConf->Lookup( _T( "ON"), m_csDownloadOn);
	writeConfig();
	
	// Now get each package information
	CMapStringToString *pmPack = m_pPrologResp->getDownloadPackages();
	while (pmPack[lPackNum].GetCount())
	{
		pmPack[lPackNum].Lookup( _T( "ID"), csTmpValue);
		// Try to find if package was not previously downloaded, parsing package history file
		CString csHistBuf;
		BOOL	bFound = FALSE;
		cFileHistory.SeekToBegin();
		while (cFileHistory.ReadPackage( csHistBuf))
		{
			if( csHistBuf.Find( csTmpValue) != -1 )
			{
				// Package ID found in history
				bFound = TRUE;
				break;
			}
		}
		pOptDownloadPackage = new COptDownloadPackage( this);
		pOptDownloadPackage->setId( csTmpValue);
		// If CERT_PATH or CERT_FILE option is provided
		pmPack[lPackNum].Lookup( _T( "CERT_PATH"), csTmpValue);
		pOptDownloadPackage->setCertPath( csTmpValue);
		pmPack[lPackNum].Lookup( _T( "CERT_FILE"), csTmpValue);
		pOptDownloadPackage->setCertFile( csTmpValue);
		// Set URL where to download INFO metadata
		pmPack[lPackNum].Lookup( _T( "INFO_LOC"), csTmpValue);
		pOptDownloadPackage->setInfoLocation( csTmpValue);
		// Set URL where to download fragment
		pmPack[lPackNum].Lookup( _T( "PACK_LOC"), csTmpValue);
		pOptDownloadPackage->setPackLocation( csTmpValue);
		if (bFound)				
		{
			// Package ID found in history, do not download
			m_pLogger->log(LOG_PRIORITY_NOTICE,  _T( "DOWNLOAD => Will not download package <%s>, already in the package history"), csTmpValue);
			sendMessage( csTmpValue, ERR_ALREADY_SETUP);
			delete pOptDownloadPackage;
		}
		else
			// Package not already downloaded, put it in the download queue
			m_tPackages.Add(pOptDownloadPackage);
		lPackNum++;							
	}	
	cFileHistory.Close();
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
	lPackNum = 0;
	while (lPackNum < m_tPackages.GetSize())
	{
		pOptDownloadPackage = (COptDownloadPackage*) (m_tPackages[lPackNum]);
		if (pOptDownloadPackage->makeDirectory() && !fileExists( pOptDownloadPackage->getLocalMetadataFilename()))
			// Download metadata from deployment server
			if (pOptDownloadPackage->downloadInfoFile())
				m_pLogger->log(LOG_PRIORITY_NOTICE,  _T( "DOWNLOAD => Package <%s> added to download queue"), pOptDownloadPackage->getId());
		lPackNum++;
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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COptDownloadPackage::COptDownloadPackage( CCapDownload * pC)
{
	pM=pC; 
	m_pLogger=getOcsLogger();
};


void COptDownloadPackage::setId(LPCTSTR lpstrId)
{
	m_csId = lpstrId;
}

void COptDownloadPackage::setCertPath(LPCTSTR lpstrCertPath)
{
	m_csCertPath = lpstrCertPath;
	// Expand INSTALL_PATH if needed
	m_csCertPath.Replace(  _T( "INSTALL_PATH"),  getDataFolder());
	m_csCertPath.Replace(  _T( "/"),  _T( "\\") );
}

void COptDownloadPackage::setCertFile(LPCTSTR lpstrCertFile)
{
	m_csCertFile = lpstrCertFile;
	// Expand INSTALL_PATH if needed
	m_csCertFile.Replace(  _T( "INSTALL_PATH"),  getDataFolder());
	m_csCertFile.Replace(  _T( "/"),  _T( "\\") );
}

void COptDownloadPackage::setInfoLocation(LPCTSTR lpstrInfoLoc)
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
	if (!LoadFileToText( csBuffer, getLocalMetadataFilename()))
	{
		m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DOWNLOAD => Cannot read Metadata file <%s>"), getLocalMetadataFilename());
		return FALSE;
	}
	if( !xml.SetDoc( csBuffer ))
	{
		m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DOWNLOAD => Metadata file <%s> is not XML"), getLocalMetadataFilename());
		return FALSE;
	}
	xml.FindElem( _T( "DOWNLOAD"));
	xml.SetAttrib( _T( "LOC"), m_csRemotePackLoc);
	if (!WriteTextToFile( xml.GetDoc(), xml.GetDoc().GetLength(), getLocalMetadataFilename()))
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Can't update Metadata file <%s>"), getLocalMetadataFilename());
		return FALSE;
	}
	// Now create a timestamp 
	csBuffer.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csId, OCS_DOWNLOAD_TIMESTAMP);
	_ltot( time( NULL), szDummy, 10);
	if (!WriteTextToFile( szDummy, _tcslen( szDummy), csBuffer))
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Can't create timestamp file <%s>"), csBuffer);
	m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DOWNLOAD => Retrieve info file...OK (pack %s)"), m_csId );
    return TRUE;
}
