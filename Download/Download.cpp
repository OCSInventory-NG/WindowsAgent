//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Download.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Download.h"
#include "NotifyUser.h"
#include "commonDownload.h"
#include "OcsUtils.h"
#include "DownloadRequest.h"
#include "DownloadResponse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDownloadApp

BEGIN_MESSAGE_MAP(CDownloadApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDownloadApp construction

CDownloadApp::CDownloadApp()
{
	m_pLogger = NULL;
	m_pConfig = NULL;
	m_pConnexion = NULL;
}


// The one and only CDownloadApp object

CDownloadApp theApp;


// CDownloadApp initialization

BOOL CDownloadApp::InitInstance()
{
	HANDLE hMutexOneInstance = NULL;

	try
	{
		// InitCommonControlsEx() is required on Windows XP if an application
		// manifest specifies use of ComCtl32.dll version 6 or later to enable
		// visual styles.  Otherwise, any window creation will fail.
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(InitCtrls);
		// Set this to include all the common control classes you want to use
		// in your application.
		InitCtrls.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		if (!CWinApp::InitInstance())
		{
			return FALSE; // terminates the application
		}

		// Initialize COM.
		if (FAILED( CoInitializeEx( 0, COINIT_MULTITHREADED)))
		{
			AfxMessageBox( _T( "Failed to initialize COM"));
			return FALSE; // terminates the application
		}
		// Set general COM security levels --------------------------
		if (FAILED( CoInitializeSecurity(	NULL, 
										-1,                          // COM authentication
										NULL,                        // Authentication services
										NULL,                        // Reserved
										RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
										RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
										NULL,                        // Authentication info
										EOAC_NONE,                   // Additional capabilities 
										NULL                         // Reserved
										)))
		{
			AfxMessageBox( _T( "Failed to initialize COM Security"));
			CoUninitialize();
			return FALSE; // terminates the application
		}

		/*****
		 *
		 *	Checks wether another instance of ocsinventory.exe is 
		 *	already running.
		 *
		 ****/

		hMutexOneInstance = ::CreateMutex( NULL, TRUE, _T("OCSINVENTORY-DOWNLOAD"));
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			return FALSE; // terminates the application
		}

		/*****
		 *
		 *	Main initinstance block 
		 *
		 ****/		
		AfxEnableControlContainer();

		// Standard initialization
		// If you are not using these features and wish to reduce the size
		// of your final executable, you should remove from the following
		// the specific initialization routines you do not need

		CTime	cStartTime;		// Start time of the inventory check
		CTime	cEndTime;		// End time of the inventory check
		CString	csMessage;		// User message
		CPackage *pPack;		// Package being processed
		// Begin time
		cStartTime = CTime::GetCurrentTime();

		// Logger
		m_pLogger			= getOcsLogger();
		m_pLogger->setApplication( AfxGetAppName());
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "=============================================================================="));
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "Starting OCS Inventory NG Package Download and Setup Tool on %s."), cStartTime.Format( _T( "%#c")));

		// Agent Configuration
		m_pConfig			= getAgentConfig();

		/*****
		*
		* Parse command line
		*
		****/
		parseCommandLine();


		/*****
		*
		* Set the required loglevel if any
		*
		****/
		if (m_pConfig->isDebugRequired())
		{
			m_pLogger->setLogLevel( LOG_PRIORITY_DEBUG);
		}

		/*****
		 *
		 *	Gets Download version
		 *
		 ****/
		csMessage = getVersion();
		if (csMessage == _T( ""))
		{
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T("DOWNLOAD => Failed to retrieve Download version from file. Version 0001 assumed"));
			csMessage=_T( "0.0.0.1");
		}
		m_pConfig->setVersion( csMessage);
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T("DOWNLOAD => Running OCS Inventory NG Download Version %s"), csMessage);
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T("DOWNLOAD => Using OCS Inventory NG FrameWork Version %s"), getFrameworkVersion());

		/*****
		 *
		 *	Communication provider loader
		 *
		 ****/
		CComProvider		*pProvider = getComServerProvider();
		CServerConfig		*pServerConfig = NULL;

		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Using network connection with Communication Server"));
		// Load communication provider interface
		if (!pProvider->load( m_pConfig->getCommunicationProvider()))
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed loading Communication Provider <%s>"), m_pConfig->getCommunicationProvider());
			goto CLEAN_AND_EXIT;
		}
		// Load default configuration
		if ((pServerConfig = pProvider->newConfig()) == NULL)
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed initializing Communication Provider settings for <%s>"), m_pConfig->getCommunicationProvider());
			goto CLEAN_AND_EXIT;
		}
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Using Communication Provider <%s> Version <%s>"), pProvider->getName(), pProvider->getVersion());
		// Parse command line arguments
		pServerConfig->parseCommandLine( m_lpCmdLine);
		// Create provider connection object
		if ((m_pConnexion = pProvider->newConnexion( pServerConfig)) == NULL)
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed creating connection for Communication Provider <%s>"), m_pConfig->getCommunicationProvider());
			goto CLEAN_AND_EXIT;
		}
		while (TRUE)
		{
			// Ensure agent is not running
			waitAgent();
			// Trying to get exclusive access to download
			if (!lockDownload())
			{
				m_pLogger->log( LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Cannot  lock directory <%s>"), getDownloadFolder());
				goto CLEAN_AND_EXIT;
			}
			// Read default download configuration pulled from server by agent
			readConfig();
			if (!m_bDownloadOn)
			{
				m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Download option turned off, so exiting"));
				unlockDownload();
				goto CLEAN_AND_EXIT;
			}
			m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Starting new period of %u cycles"), m_uDownloadPeriodLength);
			// Clean package list
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Flushing package queue"), m_uDownloadPeriodLength);
			for (INT_PTR nIndex=0; nIndex<m_listPack.GetCount(); nIndex++)
			{
				pPack = (CPackage *) m_listPack.GetAt( nIndex);
				delete pPack;
			}
			m_listPack.RemoveAll();
			// Parse download sub directories for packages
			m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Parsing directory <%s> for packages"), getDownloadFolder());
			if (findPackages() == 0)
			{
				// No package to process
				m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => No package found, exiting"));
				unlockDownload();
				goto CLEAN_AND_EXIT;
			}
			// Process 
			if (!runPeriod())
			{
				// No package to process
				m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => All package action failed during this session, exiting"));
				unlockDownload();
				goto CLEAN_AND_EXIT;
			}
			unlockDownload();
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Pausing for period latency (%u seconds)"), m_uDownloadPeriodLatency);
			Sleep( m_uDownloadPeriodLatency * 1000 );
			// At least, wait 1 second
			Sleep( 1000);
		}

CLEAN_AND_EXIT:
		/*****
		 *
		 * Free connexion and server config if needed
		 *
		 ****/
		m_pLogger->log(LOG_PRIORITY_NOTICE,  _T( "DOWNLOAD => Unloading communication provider"));
		if (m_pConnexion != NULL)
		{
			// Use provider to delete connexion and server config
			pProvider->deleteConnexion( m_pConnexion);
			if (pServerConfig != NULL)
				pProvider->deleteConfig( pServerConfig);
		}
		// Try to see if OCS Agent installer done file exists
		csMessage.Format( _T( "%s\\%s"), getDownloadFolder(), OCS_AGENT_SETUP_DONE);
		if (fileExists( csMessage))
		{
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Deleting OCS Inventory Agent Setup result file <%s>"), csMessage);
			DeleteFile( csMessage);
		}

		/*****
		 *
		 *	Get end time and compute execution duration
		 *
		 ****/
		cEndTime = CTime::GetCurrentTime();
		CTimeSpan	cTimeExec;				// Time of execution
		cTimeExec = cEndTime - cStartTime;
		csMessage.Format( _T( "%.02d:%.02d:%.02d"), cTimeExec.GetHours(), cTimeExec.GetMinutes(), cTimeExec.GetSeconds());
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Execution duration: %s.\n"), csMessage);

	}
	catch( CException *pEx)
	{
#ifdef _DEBUG
		AfxMessageBox( LookupError( pEx), MB_OK|MB_ICONSTOP);
#else
		_tprintf( LookupError( pEx));
#endif
		pEx->Delete();
	}	
	
	/*****
	 *
	 *	Releasing mutex and terminate
	 *
	 ****/
	if (hMutexOneInstance != NULL)
	{
        ::ReleaseMutex(hMutexOneInstance);
    }

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

CString CDownloadApp::getVersion()
{
	CFileVersion fileVer;
	CString myVer;

	// Get application path	
	if (GetModuleFileName( AfxGetInstanceHandle(), myVer.GetBuffer( 4*_MAX_PATH+1), 4*_MAX_PATH) == 0)
		return _T( "");
	myVer.ReleaseBuffer();
	// Open application file to get version from file
	if (fileVer.Open( myVer))
	{
		myVer = fileVer.GetProductVersion();
		myVer.Remove( ' ');
		myVer.Replace( _T( ","), _T( "."));
		fileVer.Close();
	}
	else
		myVer.Empty();
	return myVer;
}

BOOL CDownloadApp::parseCommandLine()
{
	//****************************************
	// Core configuration options
	//****************************************

	// /CONF:path_to_file
	if (isRequired( m_lpCmdLine, _T( "conf")))
	{
		// Load new config file before parsing other args
		m_pConfig->load( getParamValue( m_lpCmdLine, _T( "conf")));
	}
	// /DEBUG 
	if (isRequired( m_lpCmdLine, _T( "debug")))
		m_pConfig->setDebugRequired();

	return TRUE;
}


BOOL CDownloadApp::readConfig()
{
	CString csFileName;
	BOOL	bResult = TRUE;

	csFileName.Format( _T( "%s\\%s"), getDownloadFolder(), OCS_CONFIG_FILENAME);

	m_uDownloadFragLatency = GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "FragLatency"), _ttoi( FRAG_LATENCY_DEFAULT), csFileName);
	m_uDownloadCycleLatency = GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "CycleLatency"), _ttoi( CYCLE_LATENCY_DEFAULT), csFileName);
	m_uDownloadPeriodLatency = GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "PeriodLatency"), _ttoi( PERIOD_LATENCY_DEFAULT), csFileName);
	m_uDownloadPeriodLength = GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "PeriodLength"), _ttoi( PERIOD_LENGTH_DEFAULT), csFileName);
	m_uDownloadTimeout = GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "Timeout"), _ttoi( TIMEOUT_DEFAULT), csFileName);
	m_uCommandTimeout = GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "CommandTimeout"), _ttoi( COMMAND_TIMEOUT_DEFAULT), csFileName);
	m_bDownloadOn = (GetPrivateProfileInt( OCS_AGENT_SECTION, _T( "On"), 0, csFileName) != 0);
	return TRUE;
}

BOOL CDownloadApp::waitAgent()
{
	while (isDownloadSuspended())
	{
		m_pLogger->log(LOG_PRIORITY_WARNING, _T( "DOWNLOAD => Agent is still running, waiting 10 seconds..."));
		Sleep(10000);
		continue;
	}
	return TRUE;
}

BOOL CDownloadApp::sendResult( LPCTSTR lpstrPackID, LPCTSTR lpstrCode, LPCTSTR lpstrOuput)
{
	ASSERT( lpstrPackID);
	ASSERT( lpstrCode);
	ASSERT( m_pConnexion);

	// Send the download request
	m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Sending result code <%s> for package <%s>"), lpstrCode, lpstrPackID);
	CDownloadRequest	cRequest;
	CDownloadResponse	*pResponse = NULL;

	cRequest.setPackageResult( lpstrPackID, lpstrCode, lpstrOuput);
	pResponse = new CDownloadResponse( m_pConnexion->sendRequest( &cRequest));
	pResponse->setErrorStatus( m_pConnexion->getErrorStatus());
	pResponse->setErrorString( m_pConnexion->getErrorString());
	if (pResponse->isSuccess())
	{
		m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Result code request successfully sent"));
		cRequest.setSuccess();
	}
	else
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed to send result code request for package <%s> (%s)"), lpstrPackID, pResponse->getErrorString());
		delete pResponse;
		return FALSE;
	}
	delete pResponse;
	return TRUE;
}

UINT CDownloadApp::findPackages()
{
	CFileFind	cFinder;
	CPackage	*pPack;
	CString		csMessage;
	INT_PTR		nHighPriority = 0;

	csMessage.Format( _T( "%s\\*"), getDownloadFolder());
	BOOL bWorking = cFinder.FindFile( csMessage);
	while (bWorking)
	{
		bWorking = cFinder.FindNextFile();
		// Process only directory except . and ..
		if (cFinder.IsDots() ||!cFinder.IsDirectory())
			continue;
		// Check if Package is not blacklisted
		if (m_BlackList.IsListed( cFinder.GetFileName()))
		{
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Package <%s> is blacklisted, skipping"), cFinder.GetFilePath());
			continue;
		}
		m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Verifying package <%s>"), cFinder.GetFilePath());
		// Try to load metadata file
		pPack = new CPackage();
		csMessage.Format( _T( "%s\\%s"), cFinder.GetFilePath(), OCS_DOWNLOAD_METADATA);
		if (!pPack->load( csMessage))
		{
			// Error loading metadata
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Error loading metadata file <%s>, removing package <%s>"), csMessage, cFinder.GetFileName());
			if (!pPack->clean( cFinder.GetFileName()))
				m_pLogger->log(LOG_PRIORITY_WARNING, _T( "DOWNLOAD => Error removing corrupted package <%s>"), cFinder.GetFileName());
			delete pPack;
			continue;
		}
		// Verify package ID
		if (_tcsicmp( pPack->getID(), cFinder.GetFileName()) != 0)
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => ID <%s> differs from the directory <%s>"), pPack->getID(), cFinder.GetFileName());
			if (sendResult( cFinder.GetFileName(), ERR_BAD_ID))
				// Server successfully notified => remove package
				if (!pPack->clean( cFinder.GetFileName()))
					m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed to remove directory <%s> for package <%s>"), cFinder.GetFilePath(), pPack->getID());
			delete pPack;
			continue;
		}
		// Verify package timestamp
		if (pPack->isExpired( m_uDownloadTimeout))
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Package <%s> timed out (now:%lu, since:%lu, Timeout:%lu)"), pPack->getID(), time( NULL), pPack->getTimeStamp(), m_uDownloadTimeout);
			if (sendResult( cFinder.GetFileName(), ERR_TIMEOUT))
				// Server successfully notified => remove package
				if (!pPack->clean( cFinder.GetFileName()))
					m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed to remove timed out package <%s>"), pPack->getID());
			delete pPack;
			continue;
		}
		// Create task file if not already done
		if (!pPack->existTask() && !pPack->createTask())
		{
			m_pLogger->log(LOG_PRIORITY_WARNING, _T( "DOWNLOAD => Cannot create file <%s> for package <%s>"), OCS_DOWNLOAD_TASK, pPack->getID());
			delete pPack;
			continue;
		}
		// Verify if package already downloaded, but scheduled later
		if (pPack->isBuilt() && !pPack->isTimeToSetup())
		{
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Package <%s> already downloaded, but execution scheduled on <%s>, skipping"), pPack->getID(), pPack->getSchedule());
			delete pPack;
			continue;
		}
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Package <%s> verified and added to process queue"), pPack->getID());
		m_listPack.Add( pPack);
		if (pPack->getPriority() == 0)
			nHighPriority++;
	}
	cFinder.Close();
	if (nHighPriority > 0)
	{
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => High priority 0 package found, removing lower priority package from queue"), pPack->getID());
		// There is high priority package, only process these packages
		for (nHighPriority=0; nHighPriority<m_listPack.GetCount();)
		{
			pPack = (CPackage *) m_listPack[nHighPriority];
			if (pPack->getPriority() != 0)
			{
				// Remove not high priority package from the queue
				m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Removing package <%s> from queue"), pPack->getID());
				m_listPack.RemoveAt( nHighPriority);
				delete pPack;
			}
			else
				// Next element
				nHighPriority++;
		}
	}
	return m_listPack.GetCount();
}

BOOL CDownloadApp::setFinished( CPackage *pPack)
{
	CString csExitCode, csOutput;

	// First, check if package execution not already done, but send result failed
	if (pPack->existDone())
	{
		if (!pPack->getDone( csExitCode, csOutput))
			csExitCode = ERR_DONE_FAILED;
		if (csExitCode.Compare( CODE_SUCCESS) == 0)
		{
			// Package execute successful
			if (!CFilePackageHistory::AddPackage( getPackageHistoryFilename(), pPack->getID()))
				m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Cannot add package <%s> to History file <%s>"), pPack->getID(), getPackageHistoryFilename());
		}
		else
		{
			// Package execute failed
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Will not register package <%s> in history: result <%s> not a success"), pPack->getID(), csExitCode);
		}
		if (!sendResult( pPack->getID(), csExitCode, csOutput))
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Cannot send event message for package <%s>. Will retry later"), pPack->getID());
			return FALSE;
		}
		// Result successfully sent, clean package
		pPack->clean();
		m_BlackList.Add( pPack->getID());
		return TRUE;
	}
	return FALSE;
}

BOOL CDownloadApp::runPeriod()
{
	CPackage	*pPack;
	INT_PTR		nIndex;
	UINT		uPeriod,
				uFragToDownload = 0,// Number of fragment to download
				uError = 0;			// Number of download error
	CTime		cCycleTime;			// Start time of the cycle

	// Start period length cycle of download
	for (uPeriod=1; uPeriod<=m_uDownloadPeriodLength; uPeriod++)
	{
		// Start a cycle to download one fragament of each package of prioity uPeriod
		cCycleTime = CTime::GetCurrentTime();
		m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Processing packages for cycle %u on %s"), uPeriod, cCycleTime.Format( _T( "%#c")));
		for (nIndex=0; nIndex< m_listPack.GetCount(); nIndex++)
		{
			pPack = (CPackage*)m_listPack.GetAt( nIndex);
			if (m_BlackList.IsListed( pPack->getID())) 
				continue;
			if ((pPack->getPriority() != 0) && (uPeriod % pPack->getPriority() != 0))
				// This is not a high priority package or period is not a multiple of package priority
				continue;
			// Check if package execution finished, and only missing to send result to server
			if (!setFinished( pPack))
			{
				// Package not yet fully done, download one fargment or execute 
				if (pPack->isFragTodownload())
				{
					// There is one or more fragment to download
					uFragToDownload++;
					if (!downloadFragment( pPack))
						uError++;
				}
				else
					// all fragment downloaded, unzip and execute
					if (!pPack->existDone() && executePackage( pPack))
						setFinished( pPack);
			}
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Pausing for fragment latency (%u seconds)"), m_uDownloadFragLatency);
			Sleep( m_uDownloadFragLatency * 1000 );
		}
		m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Pausing for cycle latency (%u seconds)"), m_uDownloadCycleLatency);
		Sleep( m_uDownloadCycleLatency * 1000 );
	}
	// Return error if 2/3 of package download fails
	if ((uFragToDownload > 0) && (uError > 2*uFragToDownload/3))
		return FALSE;
	return TRUE;
}

BOOL CDownloadApp::downloadFragment( CPackage *pPack)
{
	CString csFragID;
	UINT	uIndex;

	ASSERT( m_pConnexion);

	// Read next fragment to download
	if (!pPack->getFragToDownload( csFragID))
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "DOWNLOAD => Failed to get next package fragment to download for package <%s>. Will retry later"), pPack->getID());
		return FALSE;
	}
	m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Downloading package fragment <%s>"), csFragID);
	uIndex = 0;
	while (!m_pConnexion->getFile( pPack->getFragURL( csFragID), pPack->getFragLocalPath( csFragID)))
	{
		uIndex ++;
		if (uIndex > MAX_ERROR_COUNT)
		{
			m_pLogger->log(LOG_PRIORITY_WARNING, _T( "DOWNLOAD =>  Max error count (%u) reached while downloading package fragment <%s> (%s)"), MAX_ERROR_COUNT, csFragID, m_pConnexion->getErrorString());
			return FALSE;
		}
		else
			m_pLogger->log(LOG_PRIORITY_WARNING, _T( "DOWNLOAD => Failed to download package fragment <%s> (%s). Retrying"), csFragID, m_pConnexion->getErrorString());
	}
	// Mark fragment downloaded
	if (!pPack->setFragDownloaded( csFragID))
	{
		m_pLogger->log(LOG_PRIORITY_WARNING, _T( "DOWNLOAD => Failed to set package fragment <%s> as downloaded"), csFragID);
		return FALSE;
	}
	return TRUE;
}

BOOL CDownloadApp::executePackage( CPackage *pPack)
{
	CNotifyUser cDlg;

	// Create ZIP if not already done
	if (!pPack->isBuilt())
	{
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Building package <%s>"), pPack->getID());
		if (!pPack->build())
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed to build package <%s>"), pPack->getID());
			return FALSE;
		}
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Package <%s> built successfully"), pPack->getID());
	}
	// Ensure package execution is not schedue in the future
	if (!pPack->isTimeToSetup())
	{
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Package <%s> execution is scheduled on <%s>. Scheduling package and skipping execution"), 
						pPack->getID(), pPack->getSchedule());
		if (!pPack->setScheduler())
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed to schedule package <%s>"), pPack->getID());
		return TRUE;
	}
	// Notify user if needed
	if (pPack->isNotifyUserRequired())
	{
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Notifying user for package <%s>"), pPack->getID());
		switch (cDlg.ShowPreInstall( pPack->getNotifyText(), pPack->isAbortAllowed(),
									pPack->isDelayAllowed(), pPack->getNotifyCountdown()))
		{
		case OCS_NOTIFY_APP_OK:
			// Ok, install
			break;
		case OCS_NOTIFY_APP_CANCEL:
			// Canceled by user
			pPack->setDone( ERR_ABORTED);
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Package <%s> cancelled by user"), pPack->getID());
			return TRUE;
		case OCS_NOTIFY_APP_DELAY:
			// Package delayed
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Package <%s> delayed by user"), pPack->getID());
			m_BlackList.Add( pPack->getID());
			return TRUE;
		default:
			m_pLogger->log( LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Error notifying user for package <%s>, executing default action after %u seconds"), 
				pPack->getID(), pPack->getNotifyCountdown());
			if (pPack->getNotifyCountdown())
				Sleep( pPack->getNotifyCountdown()*1000);
		}
	}
	// Execute package
	m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => Executing action <%s> for package <%s>"), pPack->getAction(), pPack->getID());
	if (!pPack->execute( m_uCommandTimeout))
	{
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Failed to execute action <%s> for package <%s>"), pPack->getAction(), pPack->getID());
		return FALSE;
	}
	// Notify user if needed
	if (pPack->isDoneNotifyUserRequired())
	{
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "DOWNLOAD => Notifying user for package <%s> action end"), pPack->getID());
		switch (cDlg.ShowPostInstall( pPack->getDoneNotifyText()))
		{
		case OCS_NOTIFY_APP_OK:
			// Ok, install
			break;
		case OCS_NOTIFY_APP_CANCEL:
			// Canceled by user
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => End Action for package <%s> cancelled by user"), pPack->getID());
			return TRUE;
		case OCS_NOTIFY_APP_DELAY:
			// Package delayed
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "DOWNLOAD => End Action for package <%s> delayed by user"), pPack->getID());
			return TRUE;
		default:
			m_pLogger->log( LOG_PRIORITY_ERROR, _T( "DOWNLOAD => Error notifying user for package <%s> end action"), 
				pPack->getID());
		}
	}
	// Excute post command if needed
	pPack->executePostCmd( m_uCommandTimeout);
	// Execution finished
	return TRUE;
}

int CDownloadApp::ExitInstance() 
{
	// Free COM
	CoUninitialize();
	return CWinApp::ExitInstance();
}
