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
//	CDeviceid *pDeviceid = CDeviceid::getInstance();
	CDeviceid *pDeviceid = getOcsDeviceID();

	m_cmXml.SetDoc( XML_HEADERS);
	m_cmXml.AddElem( _T( "REQUEST"));
	m_cmXml.AddAttrib( _T( "QUERY"), _T( "DOWNLOAD"));
	m_cmXml.AddAttrib( _T( "DEVICEID"), pDeviceid->getDeviceID());
	m_cmXml.ResetPos();
}

CDownloadRequest::~CDownloadRequest(void)
{
}

BOOL CDownloadRequest::setPackageResult( LPCTSTR lpstrPackID, LPCTSTR lpstrCode, LPCTSTR lpstrOutput)
{
	ASSERT( lpstrPackID);
	ASSERT( lpstrCode);

	m_cmXml.ResetPos(); 
	if (!m_cmXml.FindElem(_T("REQUEST")))
		return FALSE;
	if (!m_cmXml.AddAttrib( _T( "ID"), lpstrPackID))
		return FALSE;
	if (!m_cmXml.AddAttrib( _T( "ERR"), lpstrCode))
		return FALSE;
	TRACE( m_cmXml.GetDoc());
	/* TODO */
	AfxMessageBox( m_cmXml.GetDoc());
	return TRUE;
}

void CDownloadRequest::setSuccess()
{
}
