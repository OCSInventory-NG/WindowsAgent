//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ConnexionLocal.cpp: implementation of the CConnexionLocal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConnexionLocal.h"
#include "OcsUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnexionLocal::CConnexionLocal(LPCTSTR lpstrFilename)
{
	ASSERT( lpstrFilename);

	m_dwErrorCode = OCS_RESPONSE_ERROR_SUCCESS;
	m_csErrorString = _T( "No error");
	m_csFileName = lpstrFilename;
}

CConnexionLocal::~CConnexionLocal()
{

}

/* Generate the inventory request to a local file */
CByteArray * CConnexionLocal::sendRequest(CRequestAbstract *pRequest)
{
	ASSERT( pRequest);

	// Finalize the request
	if (!pRequest->final())
	{
		m_dwErrorCode = OCS_RESPONSE_ERROR_ZLIB;
		m_csErrorString = _T( "Zlib compress error");
		return NULL;
	}

	try
	{
		CByteArray	*pResponse = NULL;
		// Open and write file
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "LOCAL => Writing compressed XML file <%s> required by argument /LOCAL"), m_csFileName);
		CFile cOcsFile( m_csFileName, CFile::modeCreate|CFile::modeWrite);
		cOcsFile.Write( pRequest->getRawMessage()->GetData(), pRequest->getRawMessageLength());	
		cOcsFile.Close();
		// Respond to SEND inventory each time
		pResponse = CZip::deflate( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<REPLY>\n<RESPONSE>SEND</RESPONSE>\n</REPLY>\r\n");
		return pResponse;
	}
	catch( CException *pEx)
	{
		m_dwErrorCode = OCS_RESPONSE_ERROR_FILE;
		m_csErrorString = LookupError( pEx);
		pEx->Delete();

	}
	return NULL;
}

BOOL CConnexionLocal::getFile( LPCTSTR lpstrURL, LPCTSTR lpstrFilename)
{
	ASSERT( lpstrURL);
	ASSERT( lpstrFilename);

	// Reset Error Code
	m_dwErrorCode = OCS_RESPONSE_ERROR_SUCCESS;
	m_csErrorString = _T( "Not connected");
	return FALSE;
}

DWORD CConnexionLocal::getErrorStatus()
{
	return m_dwErrorCode;
}

LPCTSTR CConnexionLocal::getErrorString()
{
	return m_csErrorString;
}


BOOL CConnexionLocal::getLabel( LPCTSTR lpstrFilename)
{
	ASSERT( lpstrFilename);

	m_dwErrorCode = OCS_RESPONSE_ERROR_SUCCESS;
	m_csErrorString = _T( "Not connected");
	return FALSE;
}
