//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#include "stdafx.h"
#include "OCSInventory Front.h"
#include "commonDownload.h"
#include "OcsUtils.h"

LPCTSTR getPackageHistoryFilename()
{
	static CString csBuffer;

	csBuffer.Format( _T( "%s\\%s"), getDataFolder(), OCS_DOWNLOAD_HISTORY);
	return csBuffer;
}

LPCTSTR getDownloadFolder()
{
	static CString csBuffer;

	csBuffer.Format( _T( "%s\\%s"), getDataFolder(), OCS_DOWNLOAD_FOLDER);
	return csBuffer;
}


BOOL suspendDownload()
{
	CString csSuspend;
	CFile	fSuspend;

	try
	{
		csSuspend.Format( _T( "%s\\%s\\%s"), getDataFolder(), OCS_DOWNLOAD_FOLDER, OCS_DOWNLOAD_SUSPEND);
		if (!fSuspend.Open( csSuspend, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
			return FALSE;
		fSuspend.Close();
		return TRUE;
	}
	catch( CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

BOOL isDownloadSuspended()
{
	CString csSuspend;

	csSuspend.Format( _T( "%s\\%s\\%s"), getDataFolder(), OCS_DOWNLOAD_FOLDER, OCS_DOWNLOAD_SUSPEND);
	return fileExists( csSuspend);
}

BOOL resumeDownload()
{
	CString csSuspend;

	csSuspend.Format( _T( "%s\\%s\\%s"), getDataFolder(), OCS_DOWNLOAD_FOLDER, OCS_DOWNLOAD_SUSPEND);
	return DeleteFile( csSuspend);
}

CFile m_fLock;

BOOL lockDownload()
{
	CString csLock;

	csLock.Format( _T( "%s\\%s\\%s"), getDataFolder(), OCS_DOWNLOAD_FOLDER, OCS_DOWNLOAD_LOCK);
	for (UINT uTry=0; uTry<10; uTry++)
	{
		try
		{
			if (m_fLock.Open( csLock, CFile::modeCreate|CFile::modeNoTruncate|CFile::shareExclusive))
				// Access gained successfully
				return TRUE;
		}
		catch( CException *pEx)
		{
			pEx->Delete();
		}
		// Wait 1 seconds
		Sleep( 1000);
	}
	return FALSE;
}

BOOL unlockDownload()
{
	m_fLock.Close();
	return TRUE;
}


