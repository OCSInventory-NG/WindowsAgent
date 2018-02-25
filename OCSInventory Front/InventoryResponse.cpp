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
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInventoryResponse::CInventoryResponse(CByteArray *rawResponse) : CResponseAbstract(rawResponse)
{
	// Inherit from CResponseAbstract (parse response as text and put xml in local cmarkup
	CLog *pLogger = getOcsLogger();
	CString csName, csValue;
	TiXmlElement *pXmlReply, *pXmlAccount, *pXmlElement;

	// Parse Account Info updates
	m_cmXml.ResetPos();
	if (pXmlReply = m_cmXml.FindFirstElem( _T( "REPLY")))
	{
		// Search ACCOUNTINFO node under REPLY node
		pXmlAccount = m_cmXml.FindFirstElem( _T( "ACCOUNTINFO"));
		while (pXmlAccount)
		{
			pXmlElement = m_cmXml.FindFirstElem( _T( "KEYNAME"), pXmlAccount);
			csName = m_cmXml.GetData( pXmlElement);
			pXmlElement = m_cmXml.FindFirstElem( _T( "KEYVALUE"), pXmlAccount);
			csValue = m_cmXml.GetData( pXmlElement);
			if (getAgentConfig()->writeAccountInfos( csName, csValue))
				pLogger->log( LOG_PRIORITY_NOTICE, _T( "ADMIN INFO => Couple ( %s <=> %s ) added to configuration file"), csName, csValue);
			else
				pLogger->log( LOG_PRIORITY_ERROR, _T( "ADMIN INFO => Failed to add couple ( %s <=> %s ) to configuration file <%s>"), csName, csValue, LookupError( GetLastError()));
			// Search next ACCOUNTINFO node
			pXmlAccount = m_cmXml.FindNextElem( _T( "ACCOUNTINFO"), pXmlAccount);
		}
	}
	
}

CInventoryResponse::~CInventoryResponse()
{

}
