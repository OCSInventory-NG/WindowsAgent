//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CapExecute.cpp: implementation of the CCapExecute class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CapExecute.h"
#include "ExecCommand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCapExecute::CCapExecute(void)
{
}

CCapExecute::~CCapExecute(void)
{
}

BOOL CCapExecute::executePlugins( LPCTSTR lpstrPath)
{
	return (execute( TRUE, lpstrPath) && execute( FALSE, lpstrPath));
}

BOOL CCapExecute::execute( BOOL bScript, LPCTSTR lpstrPath)
{
	CString	csPath,
			csCommand;
	CExecCommand cmProcess;
	BOOL	bFound = FALSE;
	CMarkup myXml;

	try
	{
		CFileFind			cFinder;

		if ((lpstrPath == NULL) || (_tcslen( lpstrPath) == 0))
			// Use standard install path
			csPath.Format( _T( "%s\\plugins"), getInstallFolder());
		else
			// Use provided path to search for plugins
			csPath = lpstrPath;
		// Search for DLL into path
		if (bScript)
		{
			m_pLogger->log( LOG_PRIORITY_DEBUG,  _T( "EXECUTABLE PLUGIN => Searching for VBS script(s) in folder <%s>"), csPath);
			csCommand.Format( _T( "%s\\*.vbs"), csPath);
		}
		else
		{
			m_pLogger->log( LOG_PRIORITY_DEBUG,  _T( "EXECUTABLE PLUGIN => Searching for executable(s) in folder <%s>"), csPath);
			csCommand.Format( _T( "%s\\*.exe"), csPath);
		}
		bFound = cFinder.FindFile( csCommand);
		while (bFound)
		{
			// One script found, try to load it
			bFound = cFinder.FindNextFile();
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "EXECUTABLE PLUGIN => Found executable plugin <%s>"), cFinder.GetFileName());
			// Execute script
			if (bScript)
				csCommand.Format( _T( "cscript /nologo \"%s\""), cFinder.GetFilePath());
			else
				csCommand.Format( _T( "\"%s\""), cFinder.GetFilePath());
			switch (cmProcess.execWait( csCommand, csPath))
			{
			case EXEC_ERROR_START_COMMAND:
				m_pLogger->log( LOG_PRIORITY_WARNING, _T( "EXECUTABLE PLUGIN => Failed to start executable plugin <%s> (%s)"), cFinder.GetFilePath(), cmProcess.getOutput());
				continue;
			case EXEC_ERROR_WAIT_COMMAND:
				m_pLogger->log( LOG_PRIORITY_WARNING, _T( "EXECUTABLE PLUGIN => Failed to get executable plugin <%s> output"), cFinder.GetFilePath());
				continue;
			default:
				// Success
				break;
			}
			// Now get output and ensure XML well formed
			m_pLogger->log( LOG_PRIORITY_TRACE, _T( "%s"), CA2T( cmProcess.getOutput()));
			// First, assume plugin XML output is XML UTF-8 well formed
			if (!myXml.SetDoc( cmProcess.getOutput()))
			{
				// Not XML well formed, or not UTF-8 encoded, try to UTF-8 encode
				m_pLogger->log( LOG_PRIORITY_WARNING, _T( "EXECUTABLE PLUGIN => Unable to parse executable plugin <%s> XML output, perhaps not UTF-8 encoded. Trying to UTF-8 encode"), cFinder.GetFilePath());
				if (!myXml.SetDoc( CA2CA( cmProcess.getOutput(), CP_UTF8)))
				{
					m_pLogger->log( LOG_PRIORITY_WARNING, _T( "EXECUTABLE PLUGIN => Executable plugin <%s> output is not an XML document"), cFinder.GetFilePath());
					continue;
				}
			}
			// Copy XML content to inventory, node <content>
			if (! m_pInventory->getXmlPointerContent()->AddXml( &myXml))
			{
				m_pLogger->log( LOG_PRIORITY_WARNING, _T( "EXECUTABLE PLUGIN => Failed adding plugin <%s> output to inventory"), cFinder.GetFilePath());
				continue;
			}
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "EXECUTABLE PLUGIN => Plugin <%s> executed successfully"), cFinder.GetFileName());
		}
		cFinder.Close();
		return TRUE;
	}
	catch (CException *pEx)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "EXECUTABLE PLUGIN => Error while parsing Plug-in directory <%s>"), LookupError( pEx));
		pEx->Delete();
		return FALSE;
	}
}
