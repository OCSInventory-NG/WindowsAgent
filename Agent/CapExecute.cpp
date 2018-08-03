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
	return (execute(VBS, lpstrPath) && execute(PWSHELL, lpstrPath));
}

BOOL CCapExecute::execute( const int bScript, LPCTSTR lpstrPath)
{
	CString	csPath,
			csCommand,
			csOutputFile;
	CFileStatus rStatus;
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
		if (bScript == PWSHELL)
		{
			m_pLogger->log( LOG_PRIORITY_DEBUG,  _T( "EXECUTABLE PLUGIN => Searching for PS1 script(s) in folder <%s>"), csPath);
			csCommand.Format( _T( "%s\\*.ps1"), csPath);
		}
		else if (bScript == VBS){
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T("EXECUTABLE PLUGIN => Searching for VBS script(s) in folder <%s>"), csPath);
			csCommand.Format(_T("%s\\*.vbs"), csPath);
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
			// Save plugin ouput to file
			csOutputFile.Format( _T( "%s\\%s.xml"), getDataFolder(), cFinder.GetFileName());
			cmProcess.setOutputFile( csOutputFile);
			// Execute script
			if (bScript == PWSHELL){
				csCommand.Format(_T("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe -ExecutionPolicy Bypass -Command \"& {& '%s'; [Environment]::Exit(1)}\""), cFinder.GetFilePath());
			}
			else if (bScript == VBS){
				csCommand.Format(_T("cscript /nologo \"%s\""), cFinder.GetFilePath());
			}
			else{
				csCommand.Format(_T("\"%s\""), cFinder.GetFilePath());
			}

			switch (cmProcess.execWait( csCommand, csPath))
			{
			case EXEC_ERROR_START_COMMAND:
				m_pLogger->log( LOG_PRIORITY_ERROR, _T( "EXECUTABLE PLUGIN => Failed to start executable plugin <%s> (%s)"), cFinder.GetFilePath(), cmProcess.getOutput());
				// Delete plugin XML output
				DeleteFile( csOutputFile);
				continue;
			case EXEC_ERROR_WAIT_COMMAND:
				m_pLogger->log( LOG_PRIORITY_ERROR, _T( "EXECUTABLE PLUGIN => Failed to get executable plugin <%s> output"), cFinder.GetFilePath());
				// Delete plugin XML output
				DeleteFile( csOutputFile);
				continue;
			default:
				// Success
				break;
			}
			// Now get output and ensure XML well formed
			m_pLogger->log( LOG_PRIORITY_TRACE, _T( "%s"), CA2T( cmProcess.getOutput()));
			// First, ensure output is not empty
			if (CFile::GetStatus( csOutputFile, rStatus) && (rStatus.m_size == 0))
			{
				// No output
				m_pLogger->log( LOG_PRIORITY_WARNING, _T( "EXECUTABLE PLUGIN => Plugin <%s> does not produce any output"), cFinder.GetFilePath());
				// Delete plugin XML output
				DeleteFile( csOutputFile);
				continue;
			}
			// Then, try to assume plugin XML output is XML UTF-8 well formed
			if (!myXml.LoadFile( csOutputFile))
			{
				// Not XML well formed, or not UTF-8 encoded
				m_pLogger->log( LOG_PRIORITY_WARNING, _T( "EXECUTABLE PLUGIN => Executable plugin <%s> output is not UTF-8 encoded or an XML document, trying to UTF-8 encode"), cFinder.GetFilePath());
				if (!encodeFileToUTF8( csOutputFile))
				{
					m_pLogger->log( LOG_PRIORITY_ERROR, _T( "EXECUTABLE PLUGIN => Failed to UTF-8 encode executable plugin <%s> output"), cFinder.GetFilePath());
					// Do not delete plugin output, to allow diagnostic
					continue;
				}
				m_pLogger->log( LOG_PRIORITY_WARNING, _T( "EXECUTABLE PLUGIN => Executable plugin <%s> output encoded to UTF-8, trying to reload XML document"), cFinder.GetFilePath());
				// UTF-8 encode successful, try to reload
				if (!myXml.LoadFile( csOutputFile))
				{
					m_pLogger->log( LOG_PRIORITY_ERROR, _T( "EXECUTABLE PLUGIN => Executable plugin <%s> output is not a valid XML document"), cFinder.GetFilePath());
					// Do not delete plugin output, to allow diagnostic
					continue;
				}
			}
			// Copy XML content to inventory, node <content>
			if (! m_pInventory->getXmlPointerContent()->AddXml( &myXml))
			{
				m_pLogger->log( LOG_PRIORITY_ERROR, _T( "EXECUTABLE PLUGIN => Failed adding plugin <%s> output to inventory"), cFinder.GetFilePath());
				// Delete plugin XML output
				DeleteFile( csOutputFile);
				continue;
			}
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "EXECUTABLE PLUGIN => Plugin <%s> executed successfully"), cFinder.GetFileName());
			// Delete plugin XML output
			DeleteFile( csOutputFile);
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

BOOL CCapExecute::encodeFileToUTF8( LPCTSTR lpstrFile)
{
	CString csText;

	if (!LoadFileToText( csText, lpstrFile))
		return FALSE;
	return WriteTextToUTF8File( csText, lpstrFile);
}
