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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDownloadRequest::CDownloadRequest(void)
{
	CDeviceid *pDeviceid = getOcsDeviceID();

	setQuery( _T( "DOWNLOAD"));
}

CDownloadRequest::~CDownloadRequest(void)
{
}

BOOL CDownloadRequest::setPackageResult( LPCTSTR lpstrPackID, LPCTSTR lpstrCode, LPCTSTR lpstrOutput)
{
	ASSERT( lpstrPackID);
	ASSERT( lpstrCode);

	m_cmXml.ResetPos( m_pRequestXmlNode); 
	if (!m_cmXml.AddAttrib( _T( "ID"), lpstrPackID))
		return FALSE;
	if (!m_cmXml.AddAttrib( _T( "ERR"), lpstrCode))
		return FALSE;
	TRACE( m_cmXml.GetDoc());
	return TRUE;
}

void CDownloadRequest::setSuccess()
{
}
