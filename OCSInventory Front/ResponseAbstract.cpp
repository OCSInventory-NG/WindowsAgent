//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ResponseAbstract.cpp: implementation of the CResponseAbstract class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResponseAbstract.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CResponseAbstract::CResponseAbstract(CByteArray* rawResponse)
{
	/* The CByteArray* is the connexion response 
	   The default is to handle it as text in xml format
	   Specific objects will handle it like they want
	*/
	m_pRawData =  rawResponse;
	if (m_pRawData != NULL)
		m_csContent = CZip::inflate(m_pRawData);
	m_cmXml.SetDoc(m_csContent);
	if(!getErrorStatus())
		setErrorStatus( OCS_RESPONSE_ERROR_SUCCESS);
}

// Empty response object
CResponseAbstract::CResponseAbstract()
{
	m_pRawData = NULL;
}

CResponseAbstract::~CResponseAbstract()
{
	if (m_pRawData != NULL) 
		delete m_pRawData; 
}

CMarkup* CResponseAbstract::getXmlContent()
{
	return &m_cmXml;
}

CString CResponseAbstract::getCStringContent()
{
	return m_csContent;
}

DWORD CResponseAbstract::getErrorStatus()
{
	return m_dwErrorStatus;
}

void CResponseAbstract::setErrorStatus( DWORD dwError)
{
	if (dwError == 200)
		// HTTP success
		m_dwErrorStatus = OCS_RESPONSE_ERROR_SUCCESS;
	else
		// Any error
		m_dwErrorStatus = dwError;
}

CString CResponseAbstract::getErrorString()
{
	return m_csErrorString;
}

void CResponseAbstract::setErrorString( LPCTSTR lpstrErrorString)
{
	m_csErrorString = lpstrErrorString;
}

BOOL CResponseAbstract::isSuccess()
{
	return (m_dwErrorStatus == OCS_RESPONSE_ERROR_SUCCESS);
}