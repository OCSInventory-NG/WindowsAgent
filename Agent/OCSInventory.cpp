//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OCSInventory.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OCSInventory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#include "vld.h"
#endif

// COCSInventoryApp

BEGIN_MESSAGE_MAP(COCSInventoryApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// COCSInventoryApp construction

COCSInventoryApp::COCSInventoryApp()
{
	m_pLogger = NULL;
	m_pConfig = NULL;
	m_pPlugins = NULL;
	m_nExitCode = OCS_APP_NO_ERROR;
}


// The one and only COCSInventoryApp object

COCSInventoryApp theApp;


// COCSInventoryApp initialization

BOOL COCSInventoryApp::InitInstance()
{
	HANDLE hMutexOneInstance = NULL;

	try
	{
/*		// InitCommonControlsEx() is required on Windows XP if an application
		// manifest specifies use of ComCtl32.dll version 6 or later to enable
		// visual styles.  Otherwise, any window creation will fail.
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(InitCtrls);
		// Set this to include all the common control classes you want to use
		// in your application.
		InitCtrls.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx( &InitCtrls);
*/
		if (!CWinApp::InitInstance())
		{
			m_nExitCode = OCS_APP_GENERIC_ERROR;
			return FALSE; // terminates the application
		}


		/*****
		 *
		 *	Checks wether another instance of ocsinventory.exe is 
		 *	already running.
		 *
		 ****/

		hMutexOneInstance = ::CreateMutex( NULL, TRUE, _T("OCSINVENTORY-088FA840-B10D-11D3-BC36-006067709674"));
		if ( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			m_nExitCode = OCS_APP_ALREADY_RUNNING_ERROR;
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
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.

		CTime				cStartTime;		// Start time of the inventory check
		CTime				cEndTime;		// End time of the inventory check
		CString				csMessage;		// User message

		// Begin time
		cStartTime = CTime::GetCurrentTime();

		BOOL				bWriteState = FALSE;
		CString				csLabelText;

		// Logger
		m_pLogger			= getOcsLogger();
		m_pLogger->setApplication( AfxGetAppName());
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "=============================================================================="));
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "Starting OCS Inventory NG Agent on %s."), cStartTime.Format( _T( "%#c")));

		// Agent Configuration
		m_pConfig			= getAgentConfig();

		// Communication provider loader
		CComProvider		*pProvider = getComServerProvider();
		CServerConfig		*pServerConfig = NULL;
		// Server connection object
		CConnexionAbstract	*pConnexion = NULL;

		// Capacities
		CCapRegistry		cCapRegistry;
		CCapIpdiscover		cCapIpdiscover;
		CCapDownload		cCapDownload;
		CCapExecute			cCapExec;

		// Phases objects
		CPrologRequest		*pProlog = NULL;
		CInventoryRequest	*pInventory = NULL;
		CPrologResponse		*pPrologResp = NULL;
		CInventoryResponse	*pInventoryResponse = NULL;

		// If we want to store new settings, clear old settings
		if (isRequired( m_lpCmdLine, OCS_AGENT_SAVE_SETTINGS))
		{
			m_pConfig->Clear();
		}

		/*****
		*
		* Parse command line
		*
		****/
		if (!parseCommandLine())
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed initializing using provided command line parameters <%s>"), m_lpCmdLine);
			m_nExitCode = OCS_APP_GENERIC_ERROR;
			goto CLEAN_AND_EXIT;
		}

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
		 *	Gets agent version
		 *
		 ****/
		csMessage = getVersion();
		if (csMessage == _T( ""))
		{
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T("AGENT => Failed to retrieve Agent version from file. Version 0001 assumed"));
			csMessage=_T( "0001");
		}
		m_pConfig->setVersion( csMessage);
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T("AGENT => Running OCS Inventory NG Agent Version %s"), csMessage);
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T("AGENT => Using OCS Inventory NG FrameWork Version %s"), getFrameworkVersion());

		/*****
		 *
		 *	Load Plugins
		 *
		 ****/
		m_pLogger->log(LOG_PRIORITY_NOTICE,  _T( "AGENT => Loading plug-in(s)"));
		m_pPlugins = new CPlugins();

		/*****
		 *
		 *	Connection type, local mode or with network connection to server
		 *
		 ****/
		if (m_pConfig->isLocalRequired())
		{
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "AGENT => Local mode actived by /LOCAL argument: No network connection"));
			csMessage.Format( _T( "%s\\%s.ocs"), m_pConfig->getLocalInventoryFolder(), getOcsDeviceID()->getDeviceID());
			// Create local connection object
			if ((pConnexion = new CConnexionLocal( csMessage)) == NULL)
			{
				m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed creating Local Provider connexion"));
				m_nExitCode = OCS_APP_GENERIC_ERROR;
				goto CLEAN_AND_EXIT;
			}

		}
		else
		{
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "AGENT => Using network connection with Communication Server"));
			// Load communication provider interface
			if (!pProvider->load( m_pConfig->getCommunicationProvider()))
			{
				m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed loading Communication Provider <%s>"), m_pConfig->getCommunicationProvider());
				m_nExitCode = OCS_APP_COM_PROVIDER_ERROR;
				goto CLEAN_AND_EXIT;
			}
			// Load default configuration
			if ((pServerConfig = pProvider->newConfig()) == NULL)
			{
				m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed initializing Communication Provider settings for <%s>"), m_pConfig->getCommunicationProvider());
				m_nExitCode = OCS_APP_COM_PROVIDER_ERROR;
				goto CLEAN_AND_EXIT;
			}
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "AGENT => Using Communication Provider <%s> Version <%s>"), pProvider->getName(), pProvider->getVersion());
			// If we want to store new settings, clear old settings
			if (isRequired( m_lpCmdLine, OCS_AGENT_SAVE_SETTINGS))
			{
				pServerConfig->Clear();
			}
			// Parse command line arguments
			if (!pServerConfig->parseCommandLine( m_lpCmdLine))
				m_pLogger->log( LOG_PRIORITY_ERROR, _T( "AGENT => Failed parsing Communication Provider command line arguments"));
			// Create provider connection object
			if ((pConnexion = pProvider->newConnexion( pServerConfig)) == NULL)
			{
				m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed creating connection for Communication Provider <%s>"), m_pConfig->getCommunicationProvider());
				m_nExitCode = OCS_APP_COM_PROVIDER_ERROR;
				goto CLEAN_AND_EXIT;
			}
		}

		/*****
		 *
		 *	Get the tag value from command line if provided
		 *
		 ****/
		if (m_pConfig->isTagTextProvided())
		{
			csMessage.Format( _T( "%s\\%s"), getDataFolder(), OCS_ACCOUNTINFO_FILENAME);
			if (!m_pConfig->writeAccountInfos( _T( "TAG"), m_pConfig->getTagText()))
				m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Cannot write TAG <%s>"), LookupError( GetLastError()));
			else
				m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "AGENT => Tag forced by /TAG, value is <%s>"), m_pConfig->getTagText()); 
		}

		/*****
		 *
		 *	Check if writing config file is required in command line
		 *
		 ****/
		if (isRequired( m_lpCmdLine, OCS_AGENT_SAVE_SETTINGS))
		{
			// Save parameters to config file
			m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "AGENT => Writing configuration to file <%s>"), m_pConfig->getConfigFile());
			if (!m_pConfig->save())
				m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed writing agent settings to file <%s>"), m_pConfig->getConfigFile());
			if (!pProvider->saveConfig( pServerConfig))
				m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed writing Communication Provider settings to file <%s>"), m_pConfig->getConfigFile());
			// Exit
			goto CLEAN_AND_EXIT;
		}

		/*****
		 *
		 *	Shows the tag requesting dialog, only if the "notag" option was not provided
		 *  Or the TAG is provided in command line
		 *
		 ****/
		if ((!m_pConfig->isNoTagRequired()) && !m_pConfig->isTagTextProvided())
		{
			CString csLabelFile, csAccountInfoFile, csLabelText;
			BOOL labelOpened = FALSE;
			BOOL adminOpened = FALSE;

			csLabelFile.Format( _T( "%s\\%s"), getDataFolder(), OCS_LABEL_FILE);
			csAccountInfoFile.Format( _T( "%s\\%s"), getDataFolder(), OCS_ACCOUNTINFO_FILENAME);
			// Is Admininfo.conf file existing
			if (fileExists( csAccountInfoFile))
			{
				// Account info file "admininfo.conf" already exist, TAG already asked
				adminOpened = TRUE;
			}
			else
			{
				// Account info file "admininfo.conf" do not exist (agent first run), 
				// try to download LABEL file to ask user for TAG
				if (!m_pConfig->isLocalRequired())
				{
					m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "AGENT => Sending getLabel request")); 
					if (!pConnexion->getLabel( csLabelFile))
					{
						m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "AGENT => Failed to retrieve Label <%s>"), pConnexion->getErrorString()); 
						DeleteFile( csLabelFile);
					}
					else
						m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "AGENT => Label sucessfully retrieved")); 
				}

			}
			// Is LABEL file existing
			if (LoadFileToText( csLabelText, csLabelFile))
				labelOpened = TRUE;
			// If LABEL exists, but admininfo.conf does not exists
			if (labelOpened && !adminOpened)
			{
				m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "AGENT => Prompting user for TAG value")); 
				if (csLabelText.IsEmpty())
					csLabelText = _T( "Please, enter TAG value");
				CTagInputDlg tagDlg;
				tagDlg.setLabelText( csLabelText);
				if (tagDlg.DoModal() == IDOK)
				{
					// Write tag
					if (!m_pConfig->writeAccountInfos( _T( "TAG"), tagDlg.getTagValue()))
					{
						m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Cannot write TAG <%s>"), LookupError( GetLastError()));
					}
				}
			}
		}

		/*****
		 *
		 *	Start hooks from Plugins
		 *
		 ****/
		m_pPlugins->startHook();

		/*****
		 *
		 *	Create prolog object and send it
		 *
		 ****/
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "AGENT => Sending Prolog"));
		pProlog = new CPrologRequest();

		/*****
		 *
		 *	PrologWrite hooks from Plugins
		 *
		 ****/
		m_pPlugins->prologWriteHook( pProlog);

		/*****
		 *
		 *	Initialize prolog in capacities
		 *
		 ****/
		cCapRegistry.setProlog( pProlog);
		cCapIpdiscover.setProlog( pProlog);
		cCapDownload.setProlog( pProlog);
		cCapExec.setProlog( pProlog);

		/*****
		 *
		 *	Sending prolog
		 *
		 ****/
		// Send the prolog
		pPrologResp = new CPrologResponse( pConnexion->sendRequest( pProlog ));
		pPrologResp->setErrorStatus( pConnexion->getErrorStatus());
		pPrologResp->setErrorString( pConnexion->getErrorString());
		if (pPrologResp->isSuccess())
		{
			m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "AGENT => Prolog successfully sent"));
			pProlog->setSuccess();
		}
		else
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed to send Prolog <%s>"), pPrologResp->getErrorString());
			m_nExitCode = OCS_APP_NETWORK_ERROR;
			goto CLEAN_AND_EXIT;
		}
		// Show support log message if needed
		if (pPrologResp->getSupportLog( csMessage))
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "SUPPORT => %s"), csMessage);
		else
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "SUPPORT => No support detected, Registration key : N/A"));

		/*****
		 *
		 *	PrologResp hooks from Plugins
		 *
		 ****/
		m_pPlugins->prologRespHook( pPrologResp);

		/*****
		 *
		 *	Initialize prologResp in capacities
		 *
		 ****/
		cCapRegistry.setPrologResp( pPrologResp);
		cCapIpdiscover.setPrologResp( pPrologResp);
		cCapDownload.setPrologResp( pPrologResp);
		cCapExec.setPrologResp( pPrologResp);

		/*****
		 *
		 *	Write Prolog Freq in service file
		 *
		 ****/
		if (!WritePrivateProfileString( OCS_SERVICE_SECTION, OCS_SERVICE_PROLOG_FREQ, pPrologResp->getPrologFreq(), m_pConfig->getConfigFile()))
		{
			m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed to write Prolog Frequency <%s>"), LookupError( GetLastError()));
		}
		else
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "AGENT => Prolog Frequency set to %s hour(s)"), pPrologResp->getPrologFreq());


		if (pPrologResp->isInventoryRequired() || m_pConfig->isNotifyRequired() || m_pConfig->isForceInventoryRequired())
		{
			if (m_pConfig->isForceInventoryRequired())		
				m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "AGENT => Inventory forced by /FORCE option"));
			else
			{
				if (m_pConfig->isNotifyRequired())		
					m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "AGENT => Inventory change forced by /NOTIFY option"));
				else
					m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "AGENT => Inventory required"));
			}

			/*****
			 *
			 *	Launch inventory
			 *
			 ****/
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "AGENT => Launching hardware and software checks"));
			pInventory = new CInventoryRequest( m_pConfig->isNotifyRequired());

			/*****
			 *
			 *	Initialize Inventory in capacities (not in notify mode)
			 *
			 ****/
			if (!m_pConfig->isNotifyRequired())
			{
				cCapRegistry.setInventory( pInventory);
				cCapIpdiscover.setInventory( pInventory);
				cCapDownload.setInventory( pInventory);
				cCapExec.setInventory( pInventory);
			}
			/*****
			 *
			 *	Inventory hooks from Plugins
			 *
			 ****/
			m_pPlugins->inventoryHook( pInventory );

			/*****
			 *
			 *	Using capacities on inventory (not in notify mode)
			 *
			 ****/
			// Feed inventory with required registry keys (not in notify mode)
			if (!m_pConfig->isNotifyRequired() && pPrologResp->isRegistryRequired())
			{
				m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "AGENT =>  Communication Server ask for Registry Query"));
				cCapRegistry.retrieveKeys();
			}
			// Ipdiscover (not in notify mode)
			if (!m_pConfig->isNotifyRequired() && (pPrologResp->isIpdiscoverRequired() || m_pConfig->isIpDiscoverRequired()))
			{
				if (m_pConfig->isIpDiscoverRequired())
				{
					cCapIpdiscover.setLocalNetwork( m_pConfig->getIpDiscoverNetwork());
					m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "AGENT => IpDiscover required by /IPDISC on network %s"), m_pConfig->getIpDiscoverNetwork());
				}
				else
					m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "AGENT => Communication Server ask for IpDiscover"));

				if (m_pConfig->isIpDiscoverLatencyProvided())
				{
					cCapIpdiscover.setLatency( m_pConfig->getIpDiscoverLatency());
					m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "AGENT => IpDiscover Latency forced by /IPDISC_LAT to %s"), m_pConfig->getIpDiscoverLatency());
				}

				cCapIpdiscover.scanLocalNetwork();
			}
			// Script or binary plugins (not in notify mode)
			if (!m_pConfig->isNotifyRequired())
				cCapExec.executePlugins();
			
			/*****
			 *
			 *	Send inventory
			 *
			 ****/
			m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "AGENT => Sending Inventory"));
			pInventoryResponse = new CInventoryResponse( pConnexion->sendRequest( pInventory ));
			pInventoryResponse->setErrorStatus( pConnexion->getErrorStatus());
			pInventoryResponse->setErrorString( pConnexion->getErrorString());
			if (pInventoryResponse->isSuccess())
			{
				m_pLogger->log(LOG_PRIORITY_NOTICE, _T( "AGENT => Inventory successfully sent"));
				pInventory->setSuccess();
			}
			else
			{
				m_pLogger->log(LOG_PRIORITY_ERROR, _T( "AGENT => Failed to send Inventory <%s>"), pInventoryResponse->getErrorString());
				m_nExitCode = OCS_APP_NETWORK_ERROR;
			}
			
			/*****
			 *
			 *	Initialize Inventory response in capacities (not in notify mode)
			 *
			 ****/
			if (!m_pConfig->isNotifyRequired())
			{
				cCapRegistry.setInventoryResp( pInventoryResponse);
				cCapIpdiscover.setInventoryResp( pInventoryResponse);
				cCapDownload.setInventoryResp( pInventoryResponse);
				cCapExec.setInventoryResp( pInventoryResponse);
			}
			/*****
			 *
			 *	Writes the plain xml file on disk (for debugging purpose)
			 *	 only if the "/xml" option was provided
			 *
			 ****/
			if (m_pConfig->isXmlRequired())
			{
				csMessage.Format(_T( "%s\\%s.xml"), m_pConfig->getXmlFolder(), getOcsDeviceID()->getDeviceID());
				m_pLogger->log(LOG_PRIORITY_DEBUG, _T( "AGENT => Writing standard XML file <%s> required by argument /XML"), csMessage);
				WriteVoidToFile( pInventory->getMessage(), pInventory->getMessage().GetLength(), csMessage);
			}
			
			/*****
			 *
			 * Free inventory sent to server and inventory response received from server
			 *
			 ****/
			if (pInventory != NULL)
				delete pInventory;
			if (pInventoryResponse != NULL)
				delete pInventoryResponse;
		}
		/*****
		 *
		 *	Download (not in notify mode)
		 *
		 ****/
		if (!m_pConfig->isNotifyRequired() && pPrologResp->isDownloadRequired())
		{
			m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "AGENT =>  Communication Server ask for Package Download"));
			// Check if OCS Agent Setup was previously downloaded and run
			// This is a special work because OCS Agent Setup kill download process. So we loose package infos
			// such as package ID and temporary unzip folder. As a workaround, Agent setup create a result file 
			// OCSNG-Windows-Agent-Setup_done into Agent's download folder to store package ID and result.  
			// We have to read this file if exists, and move result file to package directory 
			// before laucnhing download tool, to avoid running Agent setup in loop ! 
			cCapDownload.checkOcsAgentSetupResult();
			// Check for new package to download
			cCapDownload.retrievePackages();
			// Start download tool for already existing packages and new ones
			cCapDownload.launch();
		}
		/*****
		 *
		 *	End hooks from Plugins
		 *
		 ****/
		m_pPlugins->endHook();

CLEAN_AND_EXIT:
		/*****
		 *
		 * Free prolog sent to server
		 *
		 ****/
		if (pProlog != NULL)
			delete pProlog;

		/*****
		 *
		 * Free prolog response received from server
		 *
		 ****/
		if (pPrologResp != NULL)
			delete pPrologResp;

		/*****
		 *
		 * Free connexion and server config if needed
		 *
		 ****/
		m_pLogger->log(LOG_PRIORITY_NOTICE,  _T( "AGENT => Unloading communication provider"));
		if (pConnexion != NULL)
		{
			if (m_pConfig->isLocalRequired())
				// Local connexion object
				delete pConnexion;
			else
			{
				// Use provider to delete connexion and server config
				pProvider->deleteConnexion( pConnexion);
				if (pServerConfig != NULL)
					pProvider->deleteConfig( pServerConfig);
			}
		}

		/*****
		 *
		 *	Unload Plugins
		 *
		 ****/
		m_pLogger->log(LOG_PRIORITY_NOTICE,  _T( "AGENT => Unloading plug-in(s)"));
		if (m_pPlugins != NULL)
			delete m_pPlugins;

		/*****
		 *
		 *	Get end time and compute execution duration
		 *
		 ****/
		cEndTime = CTime::GetCurrentTime();
		CTimeSpan	cTimeExec;				// Time of execution
		cTimeExec = cEndTime - cStartTime;
		csMessage.Format( _T( "%.02d:%.02d:%.02d"), cTimeExec.GetHours(), cTimeExec.GetMinutes(), cTimeExec.GetSeconds());
		m_pLogger->log( LOG_PRIORITY_NOTICE, _T( "AGENT => Execution duration: %s.\n"), csMessage);

	}
	catch( CException *pEx)
	{
		AfxMessageBox( LookupError( pEx), MB_OK|MB_ICONSTOP);
		pEx->Delete();
		m_nExitCode = OCS_APP_GENERIC_ERROR;
	}	
	
	/*****
	 *
	 *	Releasing mutex and terminate
	 *
	 ****/
	if (hMutexOneInstance != NULL)
	{
        ::ReleaseMutex( hMutexOneInstance);
    }
	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}

int COCSInventoryApp::ExitInstance() 
{
	if (CWinApp::ExitInstance() != 0)
		m_nExitCode = OCS_APP_GENERIC_ERROR;
	return m_nExitCode;
}


CString COCSInventoryApp::getVersion()
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

BOOL COCSInventoryApp::parseCommandLine()
{
	//****************************************
	// Core configuration options
	//****************************************

	// /WORK_DIR:path_to_folder
	if (isRequired( m_lpCmdLine, _T( "work_dir")))
	{
		if (setDataFolder( getParamValue( m_lpCmdLine, _T( "work_dir"))))
			// Load new config file before parsing other args
			m_pConfig->load();
		else
			return FALSE;
	}
	// /DEBUG[=level] 
	if (isRequired( m_lpCmdLine, _T( "debug")))
	{
		CString csLevel = getParamValue( m_lpCmdLine, _T( "debug"));
		// Enable default level debuging if no value provided
		if (csLevel.IsEmpty())
			csLevel.Format( _T( "%u"), OCS_DEBUG_MODE_STANDARD);
		m_pConfig->setDebugRequired( _ttoi( csLevel));
	}
	// /NOSOFTWARE
	if (isRequired( m_lpCmdLine, _T( "nosoftware")))
		m_pConfig->setNoSoftwareRequired();
	// /NOTAG
	if (isRequired( m_lpCmdLine, _T( "notag")))
		m_pConfig->setNoTagRequired();
	// Get TAG value if needed (/TAG:"my value"
	m_pConfig->setTagText( getParamValue( m_lpCmdLine, _T( "tag")));
	// /LOCAL[=path_to_folder]
	if (isRequired( m_lpCmdLine, _T( "local")))
		// Get path to folder
		m_pConfig->setLocalInventory( getParamValue( m_lpCmdLine, _T( "local")));
	// /XML[=path_to_folder]
	if (isRequired( m_lpCmdLine, _T( "xml"))) 
		// Get path to folder
		m_pConfig->setXmlFolder( getParamValue( m_lpCmdLine, _T( "xml")));
	// /FORCE
	if (isRequired( m_lpCmdLine, _T( "force")))
		m_pConfig->setForceInventoryRequired();
	// /IPDISC=network
	if (isRequired( m_lpCmdLine, _T( "ipdisc"))) 
		// get network address
		m_pConfig->setIpDiscoverRequired( getParamValue( m_lpCmdLine, _T( "ipdisc")));
	// /IPDISC_LAT=num_secs
	if (isRequired( m_lpCmdLine, _T( "ipdisc_lat"))) 
		// get number of seconds
		m_pConfig->setIpDiscoverLatency( getParamValue( m_lpCmdLine, _T( "ipdisc_lat")));
	// /FASTIP
	if (isRequired( m_lpCmdLine, _T( "fastip")))
		m_pConfig->setFastIpRequired();
	// /HKCU
	if (isRequired( m_lpCmdLine, _T( "hkcu")))
		m_pConfig->setHkcuRequired();
	// /UID
	if (isRequired( m_lpCmdLine, _T( "uid")))
		m_pConfig->setNewUID();
	// /NOTIFY
	if (isRequired( m_lpCmdLine, _T( "notify")))
		m_pConfig->setNotify( TRUE);
	return TRUE;
}
