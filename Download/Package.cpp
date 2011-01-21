//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Package.cpp: implementation of the CPackage class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCSInventory Front.h"
#include "Markup.h"
#include "commonDownload.h"
#include "OcsUtils.h"
#include "Package.h"
#include "ZipArchive.h"
#include "ExecCommand.h"

CPackage::CPackage(void)
{
	m_uPriority = 0;
	UINT m_uFrags = 0;
	m_tTimePack = 0;

	/* User notification */
	BOOL m_bNotifyUser = FALSE;
	UINT m_uNotifyCountdown = 60;
	BOOL m_bNotifyCanAbort = FALSE;
	BOOL m_bNotifyCanDelay = FALSE;
	BOOL m_bNeedDoneAction = FALSE;
}

CPackage::~CPackage(void)
{
}

BOOL CPackage::load( LPCTSTR lpstrFile)
{
	CString csBuffer;
	CMarkup myXml;
	TCHAR	cPath[_MAX_PATH], 
			cCommand[255];

	try
	{
		// Load "info" file content
		if (!LoadFileToText( csBuffer, lpstrFile))
			return FALSE;
		// Parse XML
		if (!myXml.SetDoc( GetAnsiFromUnicode( csBuffer)))
			return FALSE;
		myXml.ResetPos();
		if (!myXml.FindFirstElem( _T( "DOWNLOAD")))
			return FALSE;
		m_csID = myXml.GetAttrib( _T( "ID"));
		csBuffer = myXml.GetAttrib( _T( "PRI"));
		if (csBuffer.IsEmpty())
			return FALSE;
		m_uPriority = _ttoi( csBuffer);
		m_csAction = myXml.GetAttrib( _T( "ACT"));
		m_csName = myXml.GetAttrib( _T( "NAME"));
		m_csDigest = myXml.GetAttrib( _T( "DIGEST"));
		m_csLocation = myXml.GetAttrib( _T( "LOC"));
		m_csProtocol = myXml.GetAttrib( _T( "PROTO"));
		csBuffer = myXml.GetAttrib( _T( "FRAGS"));
		if (csBuffer.IsEmpty())
			return FALSE;
		m_uFrags = _ttoi( csBuffer);
		m_csDigestAlgo = myXml.GetAttrib( _T( "DIGEST_ALGO"));
		m_csDigestAlgo.MakeLower();
		m_csDigestEncode = myXml.GetAttrib( _T( "DIGEST_ENCODE"));
		m_csDigestEncode.MakeLower();

		if (m_csAction == OCS_DOWNLOAD_ACTION_STORE)
		{
			// Store only action, get path where to store data
			if( !ExpandEnvironmentStrings( myXml.GetAttrib( _T( "PATH")), cPath, _MAX_PATH ))
			{
				return FALSE;
			}
			else
			{
				m_csPath = cPath;
				m_csPath.Replace( _T( "/"), _T( "\\"));
				m_csPath.Replace( _T( "INSTALL_PATH"), getInstallFolder());
			}
		}
		else
			// Put into tmp folder to unzip package into path
			m_csPath.Format( _T( "%s\\%s\\tmp"), getDownloadFolder(), m_csID);

		if (m_csAction == OCS_DOWNLOAD_ACTION_LAUNCH)
		{
			// In LAUNCH, command is in the NAME atribute
			if( !ExpandEnvironmentStrings( m_csName, cCommand, 255 ))
			{
				return FALSE;
			}
			else
			{
				m_csCommand = cCommand;
				m_csCommand.Replace( _T( "INSTALL_PATH"), getInstallFolder());
			}
		}
		else
		{
			// In Store or Execute mode, command is in the COMMAND attribute
			if( !ExpandEnvironmentStrings( myXml.GetAttrib( _T( "COMMAND")), cCommand, 255 ))
			{
				return FALSE;
			}
			else
			{
				m_csCommand = cCommand;
				m_csCommand.Replace( _T( "INSTALL_PATH"), getInstallFolder());
			}
		}
		m_csGardeFou = myXml.GetAttrib( _T( "GARDEFOU"));
		
		/* User notification */
		m_bNotifyUser = (_ttoi( myXml.GetAttrib( _T( "NOTIFY_USER"))) != 0);
		if (m_bNotifyUser)
		{
			m_uNotifyCountdown = _ttoi( myXml.GetAttrib( _T( "NOTIFY_COUNTDOWN")));
			m_csNotifyText = myXml.GetAttrib( _T( "NOTIFY_TEXT"));
			m_bNotifyCanAbort = ( _ttoi( myXml.GetAttrib( _T( "NOTIFY_CAN_ABORT"))) != 0);
			m_bNotifyCanDelay = ( _ttoi( myXml.GetAttrib( _T( "NOTIFY_CAN_DELAY"))) != 0);
		}
		m_bNeedDoneAction = (_ttoi( myXml.GetAttrib( _T( "NEED_DONE_ACTION"))) != 0);
		if (m_bNeedDoneAction)
		{
			m_csNeedDoneActionText = myXml.GetAttrib( _T( "NEED_DONE_ACTION_TEXT"));
		}
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

time_t CPackage::getTimeStamp()
{
	CString csFilename, csTimestamp;

	// Check if not already retrieved from file
	if (m_tTimePack != 0)
		return m_tTimePack;

	try
	{
		// Load "since" file content
		csFilename.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_TIMESTAMP);
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

BOOL CPackage::clean()
{
	CString csPath;

	csPath.Format( _T( "%s\\%s"), getDownloadFolder(), m_csID);
	return directoryDelete( csPath);
}

BOOL CPackage::clean( LPCTSTR lpstrID)
{
	CString csPath;

	ASSERT( lpstrID);

	csPath.Format( _T( "%s\\%s"), getDownloadFolder(), lpstrID);
	return directoryDelete( csPath);
}

BOOL CPackage::isExpired( UINT uTimeOut)
{
	time_t	tTimeNow;

	tTimeNow = time( NULL);
	return (((tTimeNow - getTimeStamp())/86400) >  uTimeOut);
}


LPCTSTR CPackage::getID()
{
	return m_csID;
}

UINT CPackage::getPriority()
{
	return m_uPriority;
}

LPCTSTR CPackage::getAction()
{
	return m_csAction;
}

BOOL CPackage::isBase64()
{
	return (m_csDigestEncode.CompareNoCase( _T( "base64")) == 0);
}

BOOL CPackage::isHexadecimal()
{
	return (m_csDigestEncode.CompareNoCase( _T( "hexa")) == 0);
}

BOOL CPackage::existTask()
{
	CString csTask;

	csTask.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_TASK);
	return fileExists( csTask);
}

BOOL CPackage::existDone()
{
	CString csTaskDone;

	csTaskDone.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_DONE);
	return fileExists( csTaskDone);;
}

BOOL CPackage::createTask()
{
	CString csTask, csBuffer;
	CStdioFile myFile;

	csTask.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_TASK);
	try
	{
		if (!myFile.Open( csTask, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareDenyWrite))
			return FALSE;
		for (UINT uIndex=1; uIndex<=m_uFrags; uIndex++)
		{
			csBuffer.Format( _T( "%s-%u\n"), m_csID, uIndex);
			myFile.WriteString( csBuffer);
		}
		myFile.Close();
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		myFile.Abort();
		DeleteFile( csTask);
		return FALSE;
	}
	return TRUE;
}

BOOL CPackage::getDone( CString &csCode)
{
	CString csFile;
	CStdioFile myFile;

	csCode = ERR_DONE_FAILED;
	try
	{
		csFile.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_DONE);
		if (!myFile.Open( csFile, CFile::modeRead|CFile::typeText|CFile::shareDenyNone))
			return FALSE;
		// Read only first line to get OCS result code
		myFile.ReadString( csCode);
		myFile.Close();
		return TRUE;
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		myFile.Abort();
		csCode = ERR_DONE_FAILED;
		return FALSE;
	}
}

BOOL CPackage::getDone( CString &csCode, CString &csOutput)
{
	CString csFile, csBuffer;
	CStdioFile myFile;

	csCode = ERR_DONE_FAILED;
	try
	{
		csFile.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_DONE);
		if (!myFile.Open( csFile, CFile::modeRead|CFile::typeText|CFile::shareDenyNone))
			return FALSE;
		// Read first line to get result code
		myFile.ReadString( csCode);
		// Following lines are command ouput
		while (myFile.ReadString( csBuffer))
			csOutput.AppendFormat( _T( "%s\n"), csBuffer);
		myFile.Close();
		return TRUE;
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		myFile.Abort();
		csCode = ERR_DONE_FAILED;
		return FALSE;
	}
}

BOOL CPackage::setDone( LPCTSTR lpstrCode, LPCTSTR lpstrOutput)
{
	CStdioFile myFile;
	CString	   csFile, csResult;

	csFile.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_DONE);
	try
	{
		if (!myFile.Open( csFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareDenyWrite))
			return FALSE;
		// First line is result code
		myFile.WriteString( lpstrCode);
		// Following lines are command ouput
		myFile.WriteString( lpstrOutput);
		myFile.Close();
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		myFile.Abort();
		DeleteFile( csFile);
		return FALSE;
	}
	return TRUE;
}


BOOL CPackage::isFragTodownload()
{
	CString csTask, csTempTask;
	CFileStatus rStatus;

/*
	// Check if build.zip exists
	csTask.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_BUILD);
	if (fileExists( csTask))
		return FALSE;
*/
	csTask.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_TASK);
	// Check if task.tmp exists (task save before update). If so, use it
	csTempTask.Format( _T( "%s\\%s\\%s.tmp"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_TASK);
	if (fileExists( csTempTask))
	{
		DeleteFile( csTask);
		MoveFile( csTempTask, csTask);
	}
	// If task filesize > 0, there is frag to download
	if (CFile::GetStatus( csTask, rStatus))
		return (rStatus.m_size > 0);
	return FALSE;
}

BOOL CPackage::getFragToDownload( CString &csFragID)
{
	CStdioFile myFile;
	CString csTask;

	csFragID.Empty();
	try
	{
		csTask.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_TASK);
		if (!myFile.Open( csTask, CFile::modeRead|CFile::typeText|CFile::shareDenyNone))
			return FALSE;
		myFile.ReadString( csFragID);
		myFile.Close();
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		myFile.Abort();
		return FALSE;
	}
	return TRUE;
}

BOOL CPackage::setFragDownloaded( LPCTSTR lpstrFragID)
{
	CString		csTask,
				csTempTask,
				csBuffer;
	CStdioFile	fileTask,
				fileTempTask;
	UINT		uIndex = 0;

	ASSERT( lpstrFragID);

	try
	{
		csTask.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_TASK);
		// First, create a backup copy of task file
		csTempTask.Format( _T( "%s\\%s\\%s.tmp"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_TASK);
		if (!CopyFile( csTask, csTempTask, FALSE))
			return FALSE;
		// Now, write task backup content to task, except when lpstrID
		if (!fileTask.Open( csTask, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareDenyWrite))
			return FALSE;
		if (!fileTempTask.Open( csTempTask, CFile::modeRead|CFile::typeText|CFile::shareDenyNone))
		{
			fileTask.Abort();
			return FALSE;
		}
		while (fileTempTask.ReadString( csBuffer))
		{
			if (csBuffer.CompareNoCase( lpstrFragID) != 0)
			{
				// This is not our fragment ID
				fileTask.WriteString( csBuffer);
				fileTask.WriteString( _T( "\n"));
			}
		}
		fileTask.Close();
		fileTempTask.Close();
		// Success, so delete backup file
		return DeleteFile( csTempTask);
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		fileTempTask.Abort();
		fileTask.Abort();
		return FALSE;
	}
}

LPCTSTR CPackage::getFragURL( LPCTSTR lpstrFragID)
{
	static CString csBuffer;

	ASSERT( lpstrFragID);
	csBuffer.Format( _T( "%s://%s/%s/%s"), m_csProtocol, m_csLocation, m_csID, lpstrFragID);
	return csBuffer;
}

LPCTSTR CPackage::getFragLocalPath( LPCTSTR lpstrFragID)
{
	static CString csBuffer;

	ASSERT( lpstrFragID);
	csBuffer.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, lpstrFragID);
	return csBuffer;
}

BOOL CPackage::build()
{
	CString		csFile, csZipFile;
	CFileStatus cfStatus;
	__int64		i64BuildSize = 0;
	UINT		uIndex;
	CLog		*pLog = getOcsLogger();
	CFile		fileZip, fileFrag;

	if (m_uFrags == 0)
	{
		pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => No fragment files for package <%s>, nothing to build"), m_csID);
		return TRUE;
	}
	// Ensure all fragment are available
	pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => Verifying fragment files of package <%s>"), m_csID);
	for (uIndex=1; uIndex<=m_uFrags; uIndex++)
	{
		csFile.Format( _T( "%s\\%s\\%s-%u"), getDownloadFolder(), m_csID, m_csID, uIndex);
		if (!CFile::GetStatus( csFile, cfStatus))
		{
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Fragment file <%s> is missing or unreadable"), csFile);
			setDone( ERR_BUILD);
			return FALSE;
		}
		i64BuildSize += cfStatus.m_size;
	}
	pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => Checking free disk space for package <%s>"), m_csID);
	GetCurrentDirectory( 4*_MAX_PATH, csFile.GetBuffer( 4*_MAX_PATH));
	csFile.ReleaseBuffer();
	if (GetDiskFree( csFile) < (3*i64BuildSize))
	{
		pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Free disk space missing for package <%s> (required at least %I64d bytes)"), m_csID, 3*i64BuildSize);
		setDone( ERR_OUT_OF_SPACE);
		return FALSE;
	}
	// Concatenate each fragment file into build.zip
	pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => Building ZIP for package <%s>"), m_csID);
	csZipFile.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_BUILD);
	try
	{
		if (!fileZip.Open( csZipFile, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
		{
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Cannot create Zip <%s> from fragment files"), csZipFile);
			setDone( ERR_BUILD);
			return FALSE;
		}
		for (uIndex=1; uIndex<=m_uFrags; uIndex++)
		{
			// Read data in frag file and write it in zip file
			csFile.Format( _T( "%s\\%s\\%s-%u"), getDownloadFolder(), m_csID, m_csID, uIndex);
			if (!fileFrag.Open( csFile, CFile::modeRead|CFile::shareDenyNone))
			{
				pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Cannot read Fragment file <%s>"), csFile);
				setDone( ERR_BUILD);
				return FALSE;
			}
			UINT uLength;
			BYTE pBuffer[1024];
			BOOL bContinue = TRUE;
			while (bContinue)
			{
				uLength = fileFrag.Read( pBuffer, 1024);
				fileZip.Write( pBuffer, uLength);
				// Stop if read less than 1024 bytes
				if (uLength < 1024)
					bContinue = FALSE;
			}
			fileFrag.Close();
		}
		fileZip.Close();	
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		fileZip.Abort();
		fileFrag.Abort();
		DeleteFile( csZipFile);
		pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Cannot read Fragment file <%s>"), csFile);
		setDone( ERR_BUILD);
		return FALSE;
	}
	// Verify signature
	pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => Verifying ZIP signature for package <%s>"), m_csID);
	if (!checkSignature())
	{
		pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Failed verifying signature for Package <%s>"), m_csID);
		setDone( ERR_BAD_DIGEST);
		return FALSE;
	}
	// Delete frag files, no more needed
	deleteFragement();
	return TRUE;
}

BOOL CPackage::checkSignature()
{
	CString csZipFile,
			csHash;
	BOOL	bRes;

	// Compute digest on build.zip file
	csZipFile.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_BUILD);
	if (isBase64())
		bRes = fileDigest( csZipFile, csHash, m_csDigestAlgo);
	else if (isHexadecimal())
		bRes = fileDigest( csZipFile, csHash, m_csDigestAlgo, FALSE);
	else
		// Unsupported encoding
		return FALSE;
	// Digest compute successfully
	if (bRes && (m_csDigest.Compare( csHash) == 0))
		return TRUE;
	return FALSE;
}

BOOL CPackage::isBuilt()
{
	CString csFile;

	// If there is no fragement, assume package built
	if (m_uFrags == 0)
		// No fragment
		return TRUE;
	// Check if first fragment exist
	csFile.Format( _T( "%s\\%s\\%s-1"), getDownloadFolder(), m_csID, m_csID);
	if (fileExists( csFile))
		// Package not built, or previous build unsuccessfull
		return FALSE;
	// Check if ZIP exist
	csFile.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_BUILD);
	if (!fileExists( csFile))
		// build.zip not existing
		return FALSE;
	return TRUE;
}

BOOL CPackage::deleteFragement()
{
	CString csFile;

	for (UINT uIndex=1; uIndex<=m_uFrags; uIndex++)
	{
		csFile.Format( _T( "%s\\%s\\%s-%u"), getDownloadFolder(), m_csID, m_csID, uIndex);
		DeleteFile( csFile);
	}
	return TRUE;
}

BOOL CPackage::unZip()
{
	CZipArchive cZip;
	CString		csFile;

	csFile.Format( _T( "%s\\%s\\%s"), getDownloadFolder(), m_csID, OCS_DOWNLOAD_BUILD);
	try
	{
		cZip.Open( csFile);
		for(ZIP_INDEX_TYPE i=0; i<cZip.GetCount();i++)
			cZip.ExtractFile(i, m_csPath);
		cZip.Close();
	}
	catch (CException * pE)
	{		
		pE->Delete();
		cZip.Close( CZipArchive::afAfterException);
		return FALSE;			
	}
	catch (std::exception *pEx)
	{
		cZip.Close( CZipArchive::afAfterException);
		return FALSE;			
	}
	return TRUE;
}

UINT CPackage::execute()
{
	CLog *pLog = getOcsLogger();
	CExecCommand cmProcess;
	CString csBuffer;

	if (m_csAction == OCS_DOWNLOAD_ACTION_LAUNCH)
	{
		// We need to wait for all threads/processes started
		// First, extrac build.zip to tmp folder
		if (!directoryCreate( m_csPath) || !unZip())
		{
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Failed to unzip package <%s> to folder <%s>"), m_csID, m_csPath);
			setDone( ERR_UNZIP);
			return FALSE;
		}
		pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => Launching command <%s> for package <%s>"), m_csCommand, m_csID);
		if (m_csCommand.IsEmpty())
		{
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => No command provided for <%s> action of package <%s>"), OCS_DOWNLOAD_ACTION_STORE, m_csID);
			setDone( ERR_BAD_PARAM);
			return FALSE;
		}
		switch (cmProcess.execWaitForAllChilds( m_csCommand, m_csPath))
		{
		case EXEC_ERROR_START_COMMAND:
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Failed to launch command <%s> for package <%s> (%s)"), m_csCommand, m_csID, cmProcess.getOutput());
			setDone( ERR_EXECUTE);
			return FALSE;
		case EXEC_ERROR_WAIT_COMMAND:
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Failed to get command <%s> result code for package <%s> (%s)"), m_csCommand, m_csID, cmProcess.getOutput());
			csBuffer = ERR_EXECUTE_PACK;
			break;
		default:
			pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => Package <%s> successfully launched. Command exit code is <%d>"), m_csID, cmProcess.getExitValue());
			if (cmProcess.getExitValue() != 0)
				// Command result code is not a success
				csBuffer.Format( _T( "%s%d"), ERR_EXIT_CODE, cmProcess.getExitValue());
			else
				csBuffer = CODE_SUCCESS;
			break;
		}
		setDone( csBuffer);
		return TRUE;
	}
	if (m_csAction == OCS_DOWNLOAD_ACTION_EXECUTE)
	{
		// We only need to wait for cmmand result
		// First, extrac build.zip to tmp folder
		if (!directoryCreate( m_csPath) || !unZip())
		{
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Failed to unzip package <%s> to folder <%s>"), m_csID, m_csPath);
			setDone( ERR_UNZIP);
			return FALSE;
		}
		pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => Executing command <%s> for package <%s>"), m_csCommand, m_csID);
		if (m_csCommand.IsEmpty())
		{
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => No command provided for <%s> action of package <%s>"), OCS_DOWNLOAD_ACTION_STORE, m_csID);
			setDone( ERR_BAD_PARAM);
			return FALSE;
		}
		switch (cmProcess.execWait( m_csCommand, m_csPath))
		{
		case EXEC_ERROR_START_COMMAND:
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Failed to execute command <%s> for package <%s> (%s)"), m_csCommand, m_csID, cmProcess.getOutput());
			setDone( ERR_EXECUTE);
			return FALSE;
		case EXEC_ERROR_WAIT_COMMAND:
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Failed to get command <%s> result code for package <%s> (%s)"), m_csCommand, m_csID, cmProcess.getOutput());
			csBuffer = ERR_EXECUTE_PACK;
			break;
		default:
			pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => Package <%s> successfully executed. Command exit code is <%d>"), m_csID, cmProcess.getExitValue());
			if (cmProcess.getExitValue() != 0)
				// Command result code is not a success
				csBuffer.Format( _T( "%s%d"), ERR_EXIT_CODE, cmProcess.getExitValue());
			else
				csBuffer = CODE_SUCCESS;
			break;
		}
		setDone( csBuffer, cmProcess.getOutput());
		return TRUE;
	}
	if (m_csAction == OCS_DOWNLOAD_ACTION_STORE)
	{
		// We just want to unzip data to specified folder
		if (m_csPath.IsEmpty())
		{
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => No path provided for <%s> action of package <%s>"), OCS_DOWNLOAD_ACTION_STORE, m_csID);
			setDone( ERR_BAD_PARAM);
			return FALSE;
		}
		if (!directoryCreate( m_csPath) || !unZip())
		{
			pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Failed to unzip package <%s> to folder <%s>"), m_csID, m_csPath);
			setDone( ERR_UNZIP);
			return FALSE;
		}
		pLog->log( LOG_PRIORITY_DEBUG, _T( "PACKAGE => Package <%s> successfully stored to folder <%s>"), m_csID, m_csPath);
		setDone( CODE_SUCCESS);
		return TRUE;
	}
	// Unknown action
	pLog->log( LOG_PRIORITY_WARNING, _T( "PACKAGE => Unknown action <%s> for package <%s>"), m_csAction, m_csID);
	setDone( ERR_BAD_PARAM);
	return FALSE;
}

// User notification
BOOL CPackage::isNotifyUserRequired()
{
	return (m_bNotifyUser);
}

BOOL CPackage::isAbortAllowed()
{
	return (m_bNotifyUser && m_bNotifyCanAbort);
}

BOOL CPackage::isDelayAllowed()
{
	return (m_bNotifyUser && m_bNotifyCanDelay);
}

UINT CPackage::getNotifyCountdown()
{
	return m_uNotifyCountdown;
}
LPCTSTR CPackage::getNotifyText()
{
	return m_csNotifyText;
}

BOOL CPackage::isDoneNotifyUserRequired()
{
	return (m_bNeedDoneAction);
}

LPCTSTR CPackage::getDoneNotifyText()
{
	return m_csNeedDoneActionText;
}

