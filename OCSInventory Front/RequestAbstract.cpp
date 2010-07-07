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
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
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
	m_cmXml.SetDoc(XML_HEADERS);
	m_cmXml.AddElem( _T( "REQUEST"));
	m_cmXml.IntoElem();
	m_cmXml.AddElemNV( _T( "DEVICEID"), m_pDeviceid->getDeviceID());
	m_cmXml.AddElem( _T( "CONTENT"));
}

CRequestAbstract::~CRequestAbstract()
{
	if (m_pRawMessage != NULL)
		delete m_pRawMessage;
}

/**
 *	Return the compressed sent message length
 */
DWORD CRequestAbstract::getRawMessageLength()
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
	CString bef = m_cmXml.GetDoc();
	
	for (int i=0;i<bef.GetLength();i++)
	{	
		UCHAR cut = (UCHAR)bef.GetAt(i);
		if( (cut<32 || cut>244) && cut!=10 && cut!=13 )
			bef.SetAt(i,'x');		
	}
	m_cmXml.SetDoc(bef);
}

/**
 *	Final operations before a request can be sent
 */
BOOL CRequestAbstract::final()
{
	if(!m_bFinal)
	{
		cleanXml();
		m_pRawMessage = CZip::deflate(m_cmXml.GetDoc());
		m_bFinal = TRUE;
		return (m_pRawMessage != NULL);
	}
	else
		return FALSE;
}

/**
 *	Set the query type
 */
BOOL CRequestAbstract::setQuery(CString csQuery)
{
	BOOL bRet = FALSE;

	m_cmXml.ResetPos(); 
	if (!m_cmXml.FindElem(_T("REQUEST")))
		return bRet;
	if (!m_cmXml.IntoElem())
		return bRet;
	if(!m_cmXml.FindElem( _T("QUERY")))
		bRet = m_cmXml.AddElem( _T( "QUERY"), csQuery);
	return bRet;
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
	m_cmXml.ResetPos();
	if (!m_cmXml.FindElem(_T("REQUEST")))
		return NULL;
	if (!m_cmXml.IntoElem())
		return NULL;
	return &m_cmXml;
};

/**
 *	Return a pointer to our request content
 */
CMarkup* CRequestAbstract::getXmlPointerContent()
{
	m_cmXml.ResetPos();
	if (!m_cmXml.FindElem(_T("REQUEST")))
		return NULL;
	if (!m_cmXml.IntoElem())
		return NULL;
	if (!m_cmXml.FindElem(_T("CONTENT")))
		return NULL;
	if (!m_cmXml.IntoElem())
		return NULL;
	return &m_cmXml;
};

/**
 *	Add some xml tags to the request element
 */
BOOL CRequestAbstract::xmlAddRequestElem( LPCTSTR lpstrTag, LPCTSTR lpstrValue)
{
	m_cmXml.ResetPos(); 
	if (!m_cmXml.FindElem(_T("REQUEST")))
		return FALSE;
	if (!m_cmXml.IntoElem())
		return FALSE;
	return m_cmXml.AddElemNV( lpstrTag, lpstrValue);
}

/**
 *	Add some xml tags to the request content element
 */
BOOL CRequestAbstract::xmlAddContentElem( LPCTSTR lpstrTag, LPCTSTR lpstrValue)
{
	m_cmXml.ResetPos(); 
	if (!m_cmXml.FindElem(_T("CONTENT")))
		return FALSE;
	if (!m_cmXml.IntoElem())
		return FALSE;
	return m_cmXml.AddElemNV( lpstrTag, lpstrValue);
}