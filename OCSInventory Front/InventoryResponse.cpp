//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// InventoryResponse.cpp: implementation of the CInventoryResponse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InventoryResponse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInventoryResponse::CInventoryResponse(CByteArray *rawResponse) : CResponseAbstract(rawResponse)
{
	// Inherit from CResponseAbstract (parse response as text and put xml in local cmarkup
	CLog *pLogger = getOcsLogger();
	CString csName, csValue;

	// Parse Account Infos updates
	m_cmXml.ResetPos();
	if (m_cmXml.FindElem( _T( "REPLY")))
	{
		m_cmXml.IntoElem();
		while (m_cmXml.FindElem( _T( "ACCOUNTINFO")))
		{
			m_cmXml.FindChildElem( _T( "KEYNAME"));
			csName = m_cmXml.GetChildData();
			m_cmXml.ResetChildPos();
			m_cmXml.FindChildElem( _T( "KEYVALUE"));
			csValue = m_cmXml.GetChildData();
			if (getAgentConfig()->writeAccountInfos( csName, csValue))
				pLogger->log( LOG_PRIORITY_NOTICE, _T( "ADMIN INFOS => Couple ( %s <=> %s ) added to configuration file"), csName, csValue);
			else
				pLogger->log( LOG_PRIORITY_ERROR, _T( "ADMIN INFOS => Failed to add couple ( %s <=> %s ) to configuration file <%s>"), csName, csValue, LookupError( GetLastError()));
		}
	}
	
}

CInventoryResponse::~CInventoryResponse()
{

}
