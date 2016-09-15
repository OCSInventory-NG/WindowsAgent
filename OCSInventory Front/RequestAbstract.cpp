//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// RequestAbstract.cpp: implementation of the CRequestAbstract class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RequestAbstract.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRequestAbstract::CRequestAbstract()
{
	m_pLogger = getOcsLogger();
	m_pDeviceid = getOcsDeviceID();
	m_bFinal = FALSE;
    m_pRawMessage = NULL;

	/****
	 *
	 *	XML beginning generation
	 *
	 ****/
	m_pRequestXmlNode = m_cmXml.AddElem( _T( "REQUEST"));
	m_cmXml.AddChildElem( _T( "DEVICEID"), m_pDeviceid->getDeviceID());
	m_pContentXmlNode = m_cmXml.AddChildElem( _T( "CONTENT")); 
}

CRequestAbstract::~CRequestAbstract()
{
	if (m_pRawMessage != NULL)
		delete m_pRawMessage;
}

/**
 *	Return the compressed sent message length
 */
INT_PTR CRequestAbstract::getRawMessageLength()
{
	if (m_pRawMessage == NULL)
		return 0;
	else
		return m_pRawMessage->GetSize();
}

/**
 *	Removes any binary data from the xml markup
 */
void CRequestAbstract::cleanXml()
{
	CStringA csXml = m_cmXml.GetDoc();
	int		nIndex,
			nCount;

	// Remove "&#x...;" encoded binary characters produced by MS UTF-8 encoder, like "&#x01F;"
	nIndex = csXml.Find( "&#x");
	while (nIndex != -1)
	{
		if ((nCount = csXml.Find( ";", nIndex)) <= 3)
			// No ending ; => only delete first 3 characters "&#x"
			nCount = nIndex + 3;
		else
			// Also delete ;
			nCount++;
		// Delete chars
		csXml.Delete( nIndex, nCount - nIndex);
		// and insert space
		csXml.Insert( nIndex, " ");
		// Find next occurrence
		nIndex = csXml.Find( "&#x", nIndex);
	}
	m_cmXml.SetDoc( csXml);
}

/**
 *	Final operations before a request can be sent
 */
BOOL CRequestAbstract::final()
{
	try
	{
		if(!m_bFinal)
		{
			cleanXml();
/* TODO: Crash on some condition when writing XML content to log 
			m_pLogger->log( LOG_PRIORITY_TRACE, GetUnicodeFromUtf8( getMessage()));
*/
			m_pRawMessage = CZip::deflate( getMessage());
			m_bFinal = TRUE;
			return (m_pRawMessage != NULL);
		}
		else
			return FALSE;
	}
	catch (CException *pEx)
	{
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "REQUEST => Generic deflate error"));
		pEx->Delete();
		return FALSE;
	}
}

/**
 *	Set the query name and optional type
 */
BOOL CRequestAbstract::setQuery( LPCTSTR lpstrQuery, LPCTSTR lpstrType)
{
	m_cmXml.ResetPos( m_pRequestXmlNode); 

	if (!m_cmXml.FindFirstElem( _T("QUERY")))
	{
		if (!m_cmXml.AddChildElem( _T( "QUERY"), lpstrQuery))
			return FALSE;
		if (lpstrType != NULL)
			m_cmXml.AddChildElem( _T( "TYPE"), lpstrType);
	}
	return TRUE;
}

/**
 *	Return a pointer to our global Markup
 */
CMarkup* CRequestAbstract::getXmlPointer()
{
	m_cmXml.ResetPos();
	return &m_cmXml;
};

/**
 *	Return a pointer to our request
 */
CMarkup* CRequestAbstract::getXmlPointerRequest()
{
	m_cmXml.ResetPos( m_pRequestXmlNode);
	return &m_cmXml;
};

/**
 *	Return a pointer to our request content
 */
CMarkup* CRequestAbstract::getXmlPointerContent()
{
	m_cmXml.ResetPos( m_pContentXmlNode);
	return &m_cmXml;
};
