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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

	unProtectConfigFiles();
	csConfigFile.Format( _T( "%s\\%s"), getDataFolder(), OCS_CONFIG_FILENAME);
	if (bFull)
	{
		csValue.Format( _T( "%d"), m_iPrologFreq);
		if (!WritePrivateProfileString( OCS_SERVICE_SECTION, OCS_SERVICE_PROLOG_FREQ, csValue, csConfigFile))
		{
			csValue.Format( _T( "Failed to write new PROLOG Frequency: %s"), LookupError( GetLastError())); 
			LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csValue);
			bResult = FALSE;
		}
		csValue.Format( _T( "%d"), m_iOldPrologFreq);
		WritePrivateProfileString( OCS_SERVICE_SECTION, OCS_SERVICE_OLD_PROLOG_FREQ, csValue, csConfigFile);
	}
	csValue.Format( _T( "%d"), m_iTToWait);
	if (!WritePrivateProfileString( OCS_SERVICE_SECTION, OCS_SERVICE_TTO_WAIT, csValue, csConfigFile))
	{
		csValue.Format( _T( "Failed to write new TTO Wait: %s"), LookupError( GetLastError())); 
		LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csValue);
		bResult = FALSE;
	}
	protectConfigFiles();
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
	// Portect OCS files to prevent delete
	protectCommonFiles();
	protectConfigFiles();
	return TRUE;
}

CFile *COcsService::protectFile(LPCTSTR lpstrFolder, LPCTSTR lpstrFile)
{
	CString csFile;
	CFile	*pFile = NULL;

	try
	{
		csFile.Format( _T( "%s\\%s"), lpstrFolder, lpstrFile);
		pFile = new CFile();
		if (fileExists( csFile))
		{
			if (pFile->Open( csFile, CFile::modeRead|CFile::shareDenyWrite))
				// Successfully opened denying write
				return pFile;
			// Unable to open file for denying write
			delete pFile;
			return NULL;
		}
		// file does not exists => assume protected
		pFile->m_hFile = CFile::hFileNull;
		return pFile;
	}
	catch (CException *pEx)
	{
		if (pFile)
			delete pFile;
		pEx->Delete();
		return NULL;
	}
}

BOOL COcsService::unProtectFile( CFile *pFile)
{
	try
	{
		if (pFile != NULL)
		{
			if (pFile->m_hFile != CFile::hFileNull)
				pFile->Close();
			delete pFile;
		}
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

BOOL COcsService::protectCommonFiles()
{
	CFile *pFile;

	if ((pFile = protectFile( getInstallFolder(), _T( "zlib1.dll"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <zlib1.dll>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "libeay32.dll"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <libeay32.dll>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "ssleay32.dll"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <ssleay32.dll>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "libcurl.dll"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <libcurl.dll>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "ZipArchive.dll"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <ZipArchive.dll>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "ComHTTP.dll"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <ComHTTP.dll>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "OcsWmi.dll"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <OcsWmi.dll>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "Sysinfo.dll"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <Sysinfo.dll>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "OCSInventory Front.dll"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <OCSInventory Front.dll>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "Download.exe"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <Download.exe>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "Ocsinventory.exe"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <Ocsinventory.exe>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getInstallFolder(), _T( "OcsSystray.exe"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <OcsSystray.exe>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getDataFolder(), _T( "last_state"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <last_state>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getDataFolder(), _T( "ocsinventory.dat"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <ocsinventory.dat>"));
	else
		m_tCommonHandles.Add( pFile);
	if ((pFile = protectFile( getDataFolder(), _T( "admininfo.conf"))) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <admininfo.conf>"));
	else
		m_tCommonHandles.Add( pFile);
	return TRUE;
}

BOOL COcsService::unProtectCommonFiles()
{
	CFile *pFile;

	for (INT_PTR cptHandle=0; cptHandle < m_tCommonHandles.GetSize(); cptHandle++)
	{
		pFile = ((CFile*)m_tCommonHandles.GetAt(cptHandle));
		if (!unProtectFile( pFile))
		{
			CString csMessage;

			csMessage.Format( _T( "Unable to release write protection on file <%s>"), pFile->GetFilePath());
			LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, csMessage);
		}
	}
	m_tCommonHandles.RemoveAll();
	return TRUE;
}

BOOL COcsService::protectConfigFiles()
{
	CFile *pFile;

	if ((pFile = protectFile( getDataFolder(), OCS_CONFIG_FILENAME)) == NULL)
		LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, _T( "Unable to set write protection on file <ocsinventory.ini>"));
	else
		m_tConfigHandles.Add( pFile);
	return TRUE;
}

BOOL COcsService::unProtectConfigFiles()
{
	CFile *pFile;

	for (int cptHandle=0; cptHandle < m_tConfigHandles.GetSize(); cptHandle++)
	{
		pFile = ((CFile*)m_tConfigHandles.GetAt(cptHandle));
		if (!unProtectFile( pFile))
		{
			CString csMessage;

			csMessage.Format( _T( "Unable to release write protection on file <%s>"), pFile->GetFilePath());
			LogEvent( EVENTLOG_WARNING_TYPE, EVMSG_GENERIC_MESSAGE, csMessage);
		}
	}
	m_tConfigHandles.RemoveAll();
	return TRUE;
}

void COcsService::OnStop()
{
	// Unprotect all files
	unProtectCommonFiles();
	unProtectConfigFiles();
}

void COcsService::Run()
{
	BOOL bNotifyInventory = FALSE;
	CString csStatus;

	// Latency for launching agent when agent launch fails
	int nLatencyAgentLaunch = m_iWriteIniLatency ? m_iWriteIniLatency : WRITE_TTOWAIT_EACH;

	csStatus.Format( _T( "Service start parameters FREQ: %i, OLD_FREQ: %i, TTO_WAIT: %i"), m_iPrologFreq, m_iOldPrologFreq, m_iTToWait);
	LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_GENERIC_MESSAGE, csStatus);

	// Check if system starting (system uptime < 2 min)
	if (GetTickCount() < SYSTEM_START_MAX_UPTIME)
	{
		// Start download for package to be installed at boot time
	}

#ifdef _DEBUG
	showInventory();

	for (int i=0; i<10*WRITE_TTOWAIT_EACH+10; i++)
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
				if (!bNotifyInventory && isInventoryStateChanged())
				{
					// Inventory state changed, force inventory immediatly
					m_iTToWait = 0;
					bNotifyInventory = TRUE;
				}
			}
		}
		if( m_iTToWait <= 0 )
		{
			UINT vOld = m_iOldPrologFreq;
			if (!runAgent( bNotifyInventory))
			{
				// Agent launch failed => do not wait for next prolog freq
				// Try to launch it next WRITE_TTOWAIT_EACH, but if antother error occurs
				// increase laucnh latency, and so on at each each error.
				nLatencyAgentLaunch = nLatencyAgentLaunch * 2;
				if (nLatencyAgentLaunch > (m_iPrologFreq*PROLOG_FREQ_UNIT))
					// At least, launch agent once a PROLOG_FREQ
					nLatencyAgentLaunch = m_iPrologFreq*PROLOG_FREQ_UNIT;
				// Compute TTO_WAIT inside the latency, not PROLOG_FREQ, because of error
				m_iTToWait = generateRandNumber( nLatencyAgentLaunch);
			}
			else
			{
				// Agent launch successful, restore initial latency
				nLatencyAgentLaunch = m_iWriteIniLatency ? m_iWriteIniLatency : WRITE_TTOWAIT_EACH;
				// Read new parameters
				loadConfig();
				if (!bNotifyInventory)
				{
					// Compute new TTO_WAIT inside PROLOG_FREQ only if not in NOTIFY mode
					if( m_iPrologFreq != m_iOldPrologFreq )
						m_iTToWait = generateRandNumber(m_iPrologFreq*PROLOG_FREQ_UNIT);			
					else
						m_iTToWait = m_iPrologFreq*PROLOG_FREQ_UNIT;
				}
				bNotifyInventory = FALSE;
				m_iOldPrologFreq = m_iPrologFreq;
				writeConfig();
				csStatus.Format( _T( "OCS Inventory NG Agent executed successfully.\n\nNew service parameters: FREQ: %i, OLD_FREQ: %i, TTO_WAIT: %i"), m_iPrologFreq, vOld, m_iTToWait);
				LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_GENERIC_MESSAGE, csStatus);
			}
		}
		Sleep(1000);
		m_iTToWait--;
	}
	// Report to the event log that the service has stopped successfully
	LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED, m_csServiceName);
}


BOOL COcsService::isInventoryStateChanged()
{
	try
	{
		CNetworkAdapterList myList;
		CSysInfo	mySysInfo;
		CString		csBuffer;
		COCSInventoryState myState;

		csBuffer.Format( _T( "%s\\%s"), getDataFolder(), OCS_LAST_STATE_FILE);
		if (!myState.ReadFromFile( csBuffer))
		{
			// File open or parsing error => assume no changes
			LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, _T( "Failed to load/parse last inventory state"));
			return FALSE;
		}
		// Get network adapter infos
		mySysInfo.getNetworkAdapters( &myList);
		// Checking if networks changes
		csBuffer = myList.GetHash();
		if (csBuffer.CompareNoCase( myState.GetNetworks()) != 0)
		{
			// Changed => ask to force inventory in NOTIFY mode
			CString csMessage;

			csMessage.Format( _T( "Inventory state change detected on:\n- network adapters\n\nOCS Inventory NG Agent will be launched in NOTIFY mode"));
			LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_GENERIC_MESSAGE, csMessage);
			return TRUE;
		}
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
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
	// Unprotect files to allow agent change them if needed
	unProtectCommonFiles();
	unProtectConfigFiles();
	switch (cmProcess.execWait( csCmd, getDataFolder(), FALSE))
	{
		case EXEC_ERROR_START_COMMAND:
		csMessage.Format( _T( "Can't launch OCS Inventory NG Agent (%s)"), cmProcess.getOutput());
		LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
		break;
	case EXEC_ERROR_WAIT_COMMAND:
		csMessage.Format( _T( "Can't get OCS Inventory NG Agent exit code"));
		LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
		break;
	default:
		// Success, get exit code
		nExitCode = cmProcess.getExitValue();
		switch (nExitCode)
		{
		case -1:
			// Exit code not available
			csMessage.Format( _T( "Can't get OCS Inventory NG Agent exit code (%s)"), cmProcess.getOutput());
			LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
			break;
		case 0:
			// Success
			bReturn = TRUE;
			break;
		case 1:
			// Exit code is 1 => Generic agent Error
			csMessage.Format( _T( "OCS Inventory NG Agent encounter an error (exit code is %d => Generic Agent error)"), nExitCode);
			LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
			break;
		case 2:
			// Exit code is 2 => Agent already running
			csMessage.Format( _T( "OCS Inventory NG Agent encounter an error (exit code is %d => Agent is already running)"), nExitCode);
			LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
			break;
		case 3:
			// Exit code is 3 => Failed to load/initialize Communication Provider
			csMessage.Format( _T( "OCS Inventory NG Agent encounter an error (exit code is %d => Failed to load/initialize Communication Provider)"), nExitCode);
			LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
			break;
		case 4:
			// Exit code is 4 => Network error
			csMessage.Format( _T( "OCS Inventory NG Agent encounter an error (exit code is %d => Failed to talk with Communication Server)"), nExitCode);
			LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
			break;
		default:
			// Exit code is not a success
			csMessage.Format( _T( "OCS Inventory NG Agent encounter an error (exit code is %d => Unknown code !)"), nExitCode);
			LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_GENERIC_ERROR, csMessage);
			break;
		}
		break;
	}
	// Reprotect files
	protectCommonFiles();
	protectConfigFiles();
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
		LogEvent( EVENTLOG_INFORMATION_TYPE, EVMSG_GENERIC_MESSAGE, _T( "User manually ask displaying local inventory informations"));
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
			delete pRequest;
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