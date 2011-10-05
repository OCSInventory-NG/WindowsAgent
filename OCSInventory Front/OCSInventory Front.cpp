//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OCSInventory Front.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "OCSInventory Front.h"
#include "OcsUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#include "vld.h"
#endif

// COCSInventoryFrontApp
BEGIN_MESSAGE_MAP(COCSInventoryFrontApp, CWinApp)
END_MESSAGE_MAP()


// COCSInventoryFrontApp construction
COCSInventoryFrontApp::COCSInventoryFrontApp()
{
	m_pComServerProvider = NULL;
	m_pAgentConfig = NULL;
	m_pDeviceID = NULL;
	m_pLogger = NULL;
}


// The one and only COCSInventoryFrontApp object
COCSInventoryFrontApp theApp;


// COCSInventoryFrontApp initialization
BOOL COCSInventoryFrontApp::InitInstance()
{
	try
	{
		if (!CWinApp::InitInstance())
		{
#ifdef _DEBUG
			AfxMessageBox( _T( "Unable to initialize OCS Inventory NG FrameWork instance!"), MB_ICONSTOP);
#endif
			return FALSE;
		}

		UINT uIndex;
	   /*****
		*
		* Get application installation path	
		*
		****/		
		if (GetModuleFileName( AfxGetInstanceHandle(), m_csInstallFolder.GetBuffer( _MAX_PATH*4+1), _MAX_PATH*4) == 0)
		{
#ifdef _DEBUG
			AfxMessageBox( _T( "Unable to initialize OCS Inventory NG installation path!"), MB_ICONSTOP);
#endif
			return FALSE;
		}
		m_csInstallFolder.ReleaseBuffer();
		if ((uIndex = m_csInstallFolder.ReverseFind( '\\')) > 0)
			m_csInstallFolder = m_csInstallFolder.Left( uIndex);

	   /*****
		*
		* Get application data storage path path	
		*
		****/		
		if (!SHGetSpecialFolderPath( NULL, m_csDataFolder.GetBuffer( _MAX_PATH+1), CSIDL_COMMON_APPDATA, TRUE))
		{
#ifdef _DEBUG
			AfxMessageBox( _T( "Unable to initialize OCS Inventory NG data storage path!"), MB_ICONSTOP);
#endif
			return FALSE;
		}
		m_csDataFolder.ReleaseBuffer();
		m_csDataFolder += OCSNG_COMMON_APPDATA;
		// Create it if not exists
		if (!directoryCreate( m_csDataFolder))
		{
#ifdef _DEBUG
			AfxMessageBox( _T( "Unable to initialize OCS Inventory NG data storage path!"), MB_ICONSTOP);
#endif
			return FALSE;
		}

	   /*****
		*
		* Initialise logging machine	
		*
		****/		
		m_pLogger = CLog::getInstance();
	   /*****
		*
		* Load default Agent config
		*
		****/		
		m_pAgentConfig = CConfig::getInstance();
	   /*****
		*
		* Load communication provider
		*
		****/
		m_pComServerProvider = new CComProvider();
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
#ifdef _DEBUG
		AfxMessageBox( _T( "Unhandled exception during OCSS Inventory NG FrameWork initialization!"), MB_ICONSTOP);
#endif
		return FALSE;
	}
}


BOOL COCSInventoryFrontApp::ExitInstance()
{
	if (m_pComServerProvider)
		delete m_pComServerProvider;
	if (m_pAgentConfig)
		delete m_pAgentConfig;
	if (m_pDeviceID)
		delete m_pDeviceID;
	if (m_pLogger)
		delete m_pLogger;
	return CWinApp::ExitInstance();
}

CLog * COCSInventoryFrontApp::getOcsLogger()
{
	return m_pLogger;
}

LPCTSTR COCSInventoryFrontApp::getInstallFolder()
{
	return m_csInstallFolder;
}

LPCTSTR COCSInventoryFrontApp::getDataFolder()
{
	return m_csDataFolder;
}

CDeviceid * COCSInventoryFrontApp::getDeviceID()
{
	if (m_pDeviceID == NULL)
		m_pDeviceID = CDeviceid::getInstance();
	return m_pDeviceID;
}

CConfig * COCSInventoryFrontApp::getAgentConfig()
{
	return m_pAgentConfig;
}

CComProvider * COCSInventoryFrontApp::getComServerProvider()
{
	return m_pComServerProvider;
}

CString COCSInventoryFrontApp::getFrameworkVersion()
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
		myVer.Remove( ',');
		fileVer.Close();
	}
	else
		myVer.Empty();
	return myVer;
}

BOOL COCSInventoryFrontApp::setDataFolder( LPCTSTR lpstrDir)
{
	m_csDataFolder = lpstrDir;
	// Create it if not exists
	if (!directoryCreate( m_csDataFolder))
		return FALSE;
	return TRUE;
}

LPCTSTR getInstallFolder()
{
	return theApp.getInstallFolder();
}

LPCTSTR getDataFolder()
{
	return theApp.getDataFolder();
}

CLog * getOcsLogger()
{
	return theApp.getOcsLogger();
}

CDeviceid * getOcsDeviceID()
{
	// normal function body here
	return theApp.getDeviceID();
}

CConfig * getAgentConfig()
{
	// normal function body here
	return theApp.getAgentConfig();
}

CComProvider * getComServerProvider()
{
	// normal function body here
	return theApp.getComServerProvider();
}

CString LookupError( DWORD dwErr)
{
	LPVOID lpMsgBuf;
	DWORD dwReturn;
	HMODULE hModule = NULL; // default to system source
	static CString csMsg; 
    //
    // if dwLastError is in the network range, load the message source
    //
//	if (Err >= NERR_BASE && Err <= MAX_NERR) 
//		hModule = LoadLibraryEx(TEXT("netmsg.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE);

	dwReturn = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM | // always consider system table
        ((hModule != NULL) ? FORMAT_MESSAGE_FROM_HMODULE : 0),
        hModule, // module to get message from (NULL == system)
        dwErr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL);

	if(hModule != NULL)
      FreeLibrary(hModule);

	if (dwReturn == 0)
		csMsg.Format( _T( "Error code %lu = Unspecified error"), dwErr);
	else
	{
		csMsg.Format( _T( "Error code %lu = %s"), dwErr, (LPCTSTR)lpMsgBuf);
		// Free the buffer.
		LocalFree( lpMsgBuf );
	}
	// Replace any new line character by space
	csMsg.Replace( _T( "\n"), _T( " "));
	csMsg.Replace( _T( "\r"), _T( " "));
	return csMsg;
}


CString LookupError( CException *pEx)
{
	static CString csMsg;
	CString csMessage;

	if (pEx == NULL)
		csMsg = _T( "Unspecified exception");
	else
		if (!pEx->GetErrorMessage( csMsg.GetBuffer( 4*256), 4*256))
			csMsg = _T( "Unspecified exception");
	// Replace any new line character by space
	csMsg.Replace( _T( "\n"), _T( " "));
	csMsg.Replace( _T( "\r"), _T( " "));
	return csMsg;
}

/**
 *	Returns TRUE if the option "option" was asked, FALSE otherwise
 */

BOOL isRequired( LPCTSTR lpstrCommandLine, CString csOption)
{
	CString	csCommand = lpstrCommandLine;
	CString csOpt, cs;
	
	csOption.MakeLower();
	csOpt.Format( _T( "/%s"),csOption);
	csCommand.MakeLower();
	int iRgOpt = csCommand.Find( csOpt);
	// Ensure we haven't find option name in another option
	if ((iRgOpt > 0) && (csCommand.GetAt( iRgOpt - 1) != ' '))
	{
		// Previous char is not a space, so this is not an option name
		return isRequired( csCommand.Right( iRgOpt+csOpt.GetLength()), csOption);
	}
	return (iRgOpt != -1);
}

CString getParamValue( LPCTSTR lpstrCommandLine, CString csParam)
{
	CString	csCommand = lpstrCommandLine,
			csOptionName,
			csValue,
			csSeparator;
	int iRngOpt,
		iOptionNameLength,
		iFin;

	// First, find option name position in command
	// (convert command line to lower case for this test only)
	csOptionName.Format( _T( "%s="), csParam);
	csSeparator = csCommand;
	csSeparator.MakeLower();
	iRngOpt = csSeparator.Find( csOptionName);
	if( iRngOpt == -1 )
	{
		// Option name not found
		return _T( "");
	}
	// Next, check option separator, space or "
	if (csCommand.GetAt( iRngOpt + csOptionName.GetLength()) == '\"')
	{
		// Option value is between double quotes
		csSeparator = _T( "\"");
		iOptionNameLength = csOptionName.GetLength() + 1;
	}
	else
	{
		// Option value is not between ", so use space as separator
		csSeparator = _T( " ");
		iOptionNameLength = csOptionName.GetLength();
	}
	// Find end of value name position
	if ((iFin = csCommand.Find( csSeparator, iRngOpt+iOptionNameLength)) == -1)
		// Separator not found, assume end of argument value is end on command line
		iFin = csCommand.GetLength();
	// Extract option value from command line
	csValue = csCommand.Mid( iRngOpt + iOptionNameLength, iFin - iRngOpt - iOptionNameLength );
	return csValue;
}

CString getFrameworkVersion()
{
	// normal function body here
	return theApp.getFrameworkVersion();
}

BOOL setDataFolder( LPCTSTR lpstrDir)
{
	// normal function body here
	return theApp.setDataFolder( lpstrDir);
}
