//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyleft 2010 OCS Inventory NG Team
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsService.cpp : Implements OCS Inventory Service processes
//

#include "stdafx.h"
#include "OcsService.h"
#include <openssl/rand.h>
#include "OCSInventory Front.h"
#include "sysinfo.h"
#include "OCSInventoryState.h"
#include "OcsUtils.h"
#include "ExecCommand.h"
#include "Config.h"
#include "ServerConfig.h"
#include "ComProvider.h"
#include "InventoryRequest.h"

COcsService::COcsService():CNTService( OCS_SERVICE_SECTION)
{
}

COcsService::~COcsService()
{
}

BOOL COcsService::ParseCommandLine(int argc, LPTSTR argv[])
{
	CString csMessage,
			csParam;

    // See if we have any command line args we recognise
	if (argc <= 1)
		return FALSE;

	csParam = argv[1];
	if ((csParam.CompareNoCase( _T( "-?")) == 0) || (csParam.CompareNoCase( _T( "-h")) == 0) ||
		(csParam.CompareNoCase( _T( "/?")) == 0) || (csParam.CompareNoCase( _T( "/h")) == 0))
	{
        // Spit out version info
		csMessage.Format( _T( "Service <%s> %s registered into Windows Service Control Manager.\n\n%s\n%s\n%s\n%s"),
				m_csServiceName, IsInstalled() ? _T( "is") : _T( "IS NOT"),
				_T( "You can use the following command line parameters :"),
				_T( "   [-?] or [/?] or [-h] or [/h] to show this help."),
				_T( "   [-install] or [/install] to register service autostart at system startup."),
				_T( "   [-uninstall] or [/uninstall]  or [-remove] or [/remove] to unregister service."));
		AfxMessageBox( csMessage, MB_OK|MB_ICONINFORMATION);
        return TRUE; // say we processed the argument
    } 
	if ((csParam.CompareNoCase( _T( "-install")) == 0) || (csParam.CompareNoCase( _T( "/install")) == 0))
	{
        // Request to install
        if (IsInstalled()) 
		{
			csMessage.Format( _T( "Service <%s> is already registered."), m_csServiceName);
			AfxMessageBox( csMessage, MB_OK|MB_ICONEXCLAMATION);
        } 
		else if (!Install( OCS_SERVICE_DESCRIPTION, OCS_SERVICE_DEPENDANCIES))
		{
			csMessage.Format( _T( "Failed to register Service <%s> into Windows Service Manager."), m_csServiceName);
			AfxMessageBox( csMessage, MB_OK|MB_ICONSTOP);
        }
        return TRUE; // say we processed the argument
    } 
	if ((csParam.CompareNoCase( _T( "-uninstall")) == 0) || (csParam.CompareNoCase( _T( "/uninstall")) == 0) ||
		(csParam.CompareNoCase( _T( "-remove")) == 0) || (csParam.CompareNoCase( _T( "/remove")) == 0))
	{
        // Request to uninstall.
        if (IsInstalled()) 
		{
            // Try and remove the copy that's installed
            if (!Uninstall()) 
			{
				csMessage.Format( _T( "Failed to unregister Service <%s> from Windows Service Manager."), m_csServiceName);
				AfxMessageBox( csMessage, MB_OK|MB_ICONSTOP);
            }
        }
        return TRUE; // say we processed the argument
    }
    // Don't recognise the args
    return FALSE;
}

int COcsService::generateRandNumber(int nMax)
{
	CString csNamePath;
	csNamePath.Format( _T( "%s\\%s"), getDataFolder(), OCS_SERVICE_RAND_FILE);
	
	RAND_write_file( GetAnsiFromUnicode( csNamePath));
	BYTE* randBuf= new BYTE[sizeof(UINT)];
	CFile fRand;
	CFileException e;

	BOOL errorOccured = FALSE;
	while ( ! fRand.Open( csNamePath, CFile::modeRead|CFile::shareDenyNone,&e) )
	{		
		if( ! ( e.m_cause == CFileException::sharingViolation) ) 
		{
			LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, _T( "Can't open rand file, will seed with timestamp."));
			errorOccured = TRUE;
			break;
		}		
	}

	if( ! errorOccured )
	{
		fRand.Read( randBuf, 4);
		fRand.Close();
		DeleteFile( csNamePath);
		UINT dest;
		memcpy( &dest, randBuf, sizeof(UINT));
		srand(dest);
	}
	else {
		srand ( UINT (time(NULL)) );
	}
	delete [] randBuf;
	return UINT((nMax*rand())/RAND_MAX);
}

BOOL COcsService::loadConfig()
{
	CString csConfigFile;

	csConfigFile.Format( _T( "%s\\%s"), getDataFolder(), OCS_CONFIG_FILENAME);

	m_iPrologFreq = GetPrivateProfileInt( OCS_SERVICE_SECTION, OCS_SERVICE_PROLOG_FREQ, DEFAULT_PROLOG_FREQ, csConfigFile);
	m_iOldPrologFreq = GetPrivateProfileInt( OCS_SERVICE_SECTION, OCS_SERVICE_OLD_PROLOG_FREQ, DEFAULT_PROLOG_FREQ, csConfigFile);
	m_iTToWait = GetPrivateProfileInt( OCS_SERVICE_SECTION, OCS_SERVICE_TTO_WAIT, -1, csConfigFile);
	m_iWriteIniLatency = GetPrivateProfileInt( OCS_SERVICE_SECTION, _T( "WRITE_INI_LATENCY"), WRITE_TTOWAIT_EACH, csConfigFile);
	return TRUE;
}

BOOL COcsService::writeConfig( BOOL bFull)
{
	CString csConfigFile,
			csValue;
	BOOL	bResult = TRUE;

	csConfigFile.Format( _T( "%s\\%s"), getDataFolder(), OCS_CONFIG_FILENAME);

	if (bFull)
	{
		csValue.Format( _T( "%d"), m_iPrologFreq);
		bResult = bResult && WritePrivateProfileString( OCS_SERVICE_SECTION, OCS_SERVICE_PROLOG_FREQ, csValue, csConfigFile);
		csValue.Format( _T( "%d"), m_iOldPrologFreq);
		bResult = bResult && WritePrivateProfileString( OCS_SERVICE_SECTION, OCS_SERVICE_OLD_PROLOG_FREQ, csValue, csConfigFile);
	}
	csValue.Format( _T( "%d"), m_iTToWait);
	bResult = bResult && WritePrivateProfileString( OCS_SERVICE_SECTION, OCS_SERVICE_TTO_WAIT, csValue, csConfigFile);
	return bResult;
}

BOOL COcsService::OnInit()
{
	/*****
	 *
	 *	Checks wether another instance of ocsservice.exe is 
	 *	already running. (win98 and win95 in debug mode)
	 *
	 ****/
    ::CreateMutex( NULL, TRUE, _T("OCSSERVICE-088FA840-B10D-11D3-BC36-006067709674") );
	int Err = GetLastError();
	if ( Err == ERROR_ALREADY_EXISTS || Err == ERROR_ACCESS_DENIED ) {

	    ExitProcess(0); // terminates the application
	}
	// Load service configuration
	loadConfig();
	if( m_iPrologFreq <= 0 )
		m_iPrologFreq = DEFAULT_PROLOG_FREQ;
	if( m_iPrologFreq != m_iOldPrologFreq )
	{
		m_iTToWait = generateRandNumber( m_iPrologFreq*PROLOG_FREQ_UNIT);
		m_iOldPrologFreq = m_iPrologFreq;
		writeConfig();
	}
	// Rotate log files
	rotateLogs();
	// Open required OCS files to prevent delete
	protectFiles();
	return TRUE;
}

BOOL COcsService::protectFile(LPCTSTR lpstrFolder, LPCTSTR lpstrFile)
{
	CString csFile;
	CFile	*pFile;

	csFile.Format( _T( "%s\\%s"), lpstrFolder, lpstrFile);
	pFile = new CFile();
	pFile->Open( csFile, CFile::modeRead|CFile::shareDenyWrite);
	m_tHandles.Add( pFile);
	return TRUE;
}

BOOL COcsService::protectFiles()
{
	protectFile( getInstallFolder(), _T( "Zlib1.dll"));
	protectFile( getInstallFolder(), _T( "libeay32.dll"));
	protectFile( getInstallFolder(), _T( "ssleay32.dll"));
	protectFile( getInstallFolder(), _T( "libcurl.dll"));
	protectFile( getInstallFolder(), _T( "ZipArchive.dll"));
	protectFile( getInstallFolder(), _T( "ComHTTP.dll"));
	protectFile( getInstallFolder(), _T( "OcsWmi.dll"));
	protectFile( getInstallFolder(), _T( "Sysinfo.dll"));
	protectFile( getInstallFolder(), _T( "OCSInventory Front.dll"));
	protectFile( getInstallFolder(), _T( "Download.exe"));
	protectFile( getInstallFolder(), _T( "Ocsinventory.exe"));
	protectFile( getInstallFolder(), _T( "OcsSystray.exe"));
	protectFile( getDataFolder(), _T( "last_state"));
	protectFile( getDataFolder(), _T( "ocsinventory.dat"));
	protectFile( getDataFolder(), _T( "admininfo.conf"));
	return TRUE;
}

BOOL COcsService::unProtectFiles()
{
	CFile *pFile;

	for (int cptHandle=0; cptHandle < m_tHandles.GetSize(); cptHandle++)
	{
		pFile = ((CFile*)m_tHandles.GetAt(cptHandle));
		pFile->Close();
		delete( pFile);		
	}
	m_tHandles.RemoveAll();
	return TRUE;
}

void COcsService::OnStop()
{
	// Close all file object referenced into m_tHandles
	unProtectFiles();
}

void COcsService::Run()
{
	BOOL bNotifyInventory = FALSE;

	// Latency for launching agent when agent launch fails
	int nLatencyAgentLaunch = m_iWriteIniLatency ? m_iWriteIniLatency : WRITE_TTOWAIT_EACH;

	CString csStatus;
	csStatus.Format( _T( "Service start parameters FREQ: %i, OLD_FREQ: %i, TTO_WAIT: %i"), m_iPrologFreq, m_iOldPrologFreq, m_iTToWait);
	LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_GENERIC_MESSAGE, csStatus);

	// Check if system starting (system uptime < 2 min)
	if (GetTickCount() < SYSTEM_START_MAX_UPTIME)
	{
		// Start download for package to be installed at boot time
	}

#ifdef _DEBUG
	showInventory();

	for (int i=0; i<10; i++)
#else
	while (IsRunning())
#endif
	{
		if( m_iTToWait % (m_iWriteIniLatency ? m_iWriteIniLatency:WRITE_TTOWAIT_EACH) == 0 )
		{
			// Write time to wait before next run
			writeConfig( FALSE);
			// Check inventory state for changes, and launch agent if needed
			if (m_iTToWait > m_iWriteIniLatency)
			{
				if (!bNotifyInventory && checkInventoryState())
				{
					// Inventory state changed, force inventory immediatly
					LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Inventory state change detected, OCS Inventory NG Agent launched in NOTIFY mode"));
					m_iTToWait = 0;
					bNotifyInventory = TRUE;
				}
			}
		}
		if( m_iTToWait <= 0 )
		{
			// Unprotect files to allow agent change them if needed
			unProtectFiles();
			UINT vOld = m_iOldPrologFreq;
			if (!runAgent( bNotifyInventory))
			{
				// Agent launch failed
				if (nLatencyAgentLaunch < (m_iPrologFreq*PROLOG_FREQ_UNIT))
					// At least, launch agent once a PROLOG_FREQ
					nLatencyAgentLaunch = nLatencyAgentLaunch * 2;
				m_iTToWait = generateRandNumber( nLatencyAgentLaunch);
			}
			else
			{
				bNotifyInventory = FALSE;
				// Agent launch successful, restore initial latency
				nLatencyAgentLaunch = m_iWriteIniLatency ? m_iWriteIniLatency : WRITE_TTOWAIT_EACH;
				// Read new parameters
				loadConfig();
				// Compute new TTO_WAIT
				if( m_iPrologFreq != m_iOldPrologFreq )
					m_iTToWait = generateRandNumber(m_iPrologFreq*PROLOG_FREQ_UNIT);			
				else
					m_iTToWait = m_iPrologFreq*PROLOG_FREQ_UNIT;
				
				m_iOldPrologFreq = m_iPrologFreq;
				writeConfig();

				csStatus.Format( _T( "OCS Inventory NG Agent launched successfully. New service parameters: FREQ: %i, OLD_FREQ: %i, TTO_WAIT: %i"), m_iPrologFreq, vOld, m_iTToWait);
				LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_GENERIC_MESSAGE, csStatus);
			}
			// Reportect files
			protectFiles();
		}
		Sleep(1000);
		m_iTToWait--;
	}
	// Report to the event log that the service has stopped successfully
	LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED, m_csServiceName);
}


BOOL COcsService::checkInventoryState()
{
	CStdioFile cFile;

	try
	{
		CNetworkAdapterList myList;
		CSysInfo	mySysInfo;
		CString		csBuffer;
		COCSInventoryState myState;

		csBuffer.Format( _T( "%s\\%s"), getDataFolder(), OCS_LAST_STATE_FILE);
		if (!myState.ReadFromFile( csBuffer))
		{
			// File open or parsing error
			return FALSE;
		}
		// Get network adapter infos
		mySysInfo.getNetworkAdapters( &myList);
		// Checking if networks changes
		csBuffer = myList.GetHash();
		if (csBuffer.CompareNoCase( myState.GetNetworks()) != 0)
			// Changed, ask forced inventory
			return TRUE;
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		cFile.Abort();
	}
	return FALSE;
}


BOOL COcsService::runAgent( BOOL bNotify)
{
	CString csCmd,
			csMessage;
	BOOL	bReturn = FALSE;
	int		nExitCode;
	CExecCommand cmProcess;

	Sleep( generateRandNumber( m_iWriteIniLatency ? m_iWriteIniLatency : WRITE_TTOWAIT_EACH) );

	if (bNotify)
		// Force agent to notify inventory, even if server does not request it
		// Used when inventory state change detected
		csCmd.Format( _T( "\"%s\\%s\" /NOTIFY"), getInstallFolder(), RUN_OCS);
	else
		csCmd.Format( _T( "\"%s\\%s\""), getInstallFolder(), RUN_OCS);

	switch (cmProcess.execWait( csCmd, getDataFolder()))
	{
		case EXEC_ERROR_START_COMMAND:
		csMessage.Format( _T( "Can't launch OCS Inventory NG Agent (%s)"), cmProcess.getOutput());
		LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
		return FALSE;
	case EXEC_ERROR_WAIT_COMMAND:
		csMessage.Format( _T( "Cant't get OCS Inventory NG Agent exit code"));
		LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
		return FALSE;
	default:
		// Success
		break;
	}
	nExitCode = cmProcess.getExitValue();
	switch (nExitCode)
	{
	case -1:
		// Exit code not available
		csMessage.Format( _T( "Cant't get OCS Inventory NG Agent exit code (%s)"), cmProcess.getOutput());
		LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
		break;
	case 0:
		// Success
		bReturn = TRUE;
		break;
	default:
		// Exit code is not a success
		csMessage.Format( _T( "OCS Inventory NG Agent encounter an error (exit code is %d)"), nExitCode);
		LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
		break;
	}
	return bReturn;
}

BOOL COcsService::OnUserControl( DWORD dwOpcode)
{
    switch (dwOpcode) 
	{
    case OCS_SERVICE_CONTROL_RUN_INVENTORY_NOW:
        // Run inventory in 2 units (seconds by default)
		m_iTToWait = 2;
		// Also, force randomize for next launch
		m_iOldPrologFreq = 999;
		LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_GENERIC_MESSAGE, _T( "User manually request Agent launch"));
        return TRUE;
    case OCS_SERVICE_CONTROL_SHOW_INVENTORY:
        // Show inventory using XSLT
		LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_GENERIC_MESSAGE, _T( "User manually ask local inventory informations"));
		showInventory();
        return TRUE;
    default:
        break;
    }
    return FALSE; // say not handled
}

BOOL COcsService::showInventory()
{
	try
	{
		CRequestAbstract *pRequest;
		CStringA		 csXml, csXsl;
		CString			 csFile;
		int				 nIndex;

		// Generate XML file name
		csFile.Format( _T( "%s\\OCSInventory.xml"), getDataFolder());
		// Create inventory
		if ((pRequest = new CInventoryRequest()) == NULL)
		{
			LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, _T( "Unable to retreive inventory informations"));
			return FALSE;
		}
		if (!pRequest->final())
		{
			LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, _T( "Unable to generate XML of inventory informations"));
			return FALSE;
		}
		// Get inventory XML
		csXml = pRequest->getMessage();
		// Delete inventory
		delete pRequest;
		// Add XSL transform to XM
		csXsl.Format( "\n<?xml-stylesheet type=\"text/xsl\" href=\"%s\\OCS-Transform.xsl\" ?>", GetAnsiFromUnicode( getInstallFolder()));
		if ((nIndex = csXml.Find( "?>")) == -1)
		{
			LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, _T( "Unable to add XSL stylesheet to inventory informations"));
			return FALSE;
		}
		csXml.Insert( nIndex+2, csXsl);
		// Write XML to data directory
		if (!WriteVoidToFile( csXml, csXml.GetLength(), csFile))
		{
			LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, _T( "Unable to write XML inventory informations to file"));
			return FALSE;
		}
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, _T( "An unknown error occured while trying to generate inventory informations"));
		return FALSE;
	}
}

BOOL COcsService::rotateLogs()
{
	try
	{
		CString		csFile;
		CFileFind	myFinder;
		BOOL		bWorking;

		csFile.Format( _T( "%s\\*.log"), getDataFolder());
		bWorking = myFinder.FindFile( csFile);
		while (bWorking)
		{
			bWorking = myFinder.FindNextFile();
			// There is log file => rename each with .bak extension and delete original
			csFile.Format( _T("%s.bak"), myFinder.GetFilePath());
			if (!CopyFile( myFinder.GetFilePath(), csFile, FALSE) ||
				!DeleteFile( myFinder.GetFilePath()))
			{
				csFile.Format( _T( "Failed to rotate log file <%s>"), myFinder.GetFilePath());
				LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csFile);
				continue;
			}
		}
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, _T( "An unknown error occured while rotating log files"));
		return FALSE;
	}
	return TRUE;
}