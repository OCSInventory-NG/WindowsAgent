//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#include "StdAfx.h"
#include "DownloadRequest.h"

CDownloadRequest::CDownloadRequest(void)
{
	CDeviceid *pDeviceid = getOcsDeviceID();

	m_cmXml.AddElem( _T( "REQUEST"));
	m_cmXml.AddAttrib( _T( "QUERY"), _T( "DOWNLOAD"));
	m_cmXml.AddAttrib( _T( "DEVICEID"), pDeviceid->getDeviceID());
}

CDownloadRequest::~CDownloadRequest(void)
{
}

BOOL CDownloadRequest::setPackageResult( LPCTSTR lpstrPackID, LPCTSTR lpstrCode, LPCTSTR lpstrOutput)
{
	ASSERT( lpstrPackID);
	ASSERT( lpstrCode);
	TiXmlElement *pXmlRequest;

	m_cmXml.ResetPos(); 
	if ((pXmlRequest = m_cmXml.FindFirstElem(_T("REQUEST"))) == NULL)
		return FALSE;
	if (!m_cmXml.AddChildElem( _T( "ID"), lpstrPackID))
		return FALSE;
	if (!m_cmXml.AddChildElem( _T( "ERR"), lpstrCode))
		return FALSE;
	TRACE( m_cmXml.GetDoc());
	return TRUE;
}

void CDownloadRequest::setSuccess()
{
}
