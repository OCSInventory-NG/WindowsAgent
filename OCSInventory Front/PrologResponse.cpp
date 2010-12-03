//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// PrologResponse.cpp: implementation of the CPrologResponse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PrologResponse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrologResponse::CPrologResponse(CByteArray *rawResponse) : CResponseAbstract( rawResponse)
{
	m_bInventoryRequired = FALSE;
	m_csPrologFreq.Format( _T( "%lu"), DEFAULT_PROLOG_FREQ);
	TiXmlElement *pXmlReply;
	TiXmlElement *pXmlElement;

	// Get RESPONSE to know if inventory needed
	m_cmXml.ResetPos();
	if (pXmlReply = m_cmXml.FindFirstElem( _T( "REPLY")))
	{
		// Search RESPONSE node under REPLY node
		if (pXmlElement = m_cmXml.FindFirstElem( _T( "RESPONSE"), pXmlReply))
			// If SEND asked, inventory needed
			m_bInventoryRequired = (_tcsicmp( m_cmXml.GetData( pXmlElement), _T( "SEND")) == 0);
		// Get PROLOG_FREQ node under REPLY node
		if (pXmlElement = m_cmXml.FindFirstElem( _T( "PROLOG_FREQ"), pXmlReply))
		{
			// Get prolog frequency value  (if negative or 0, set it to default)
			m_csPrologFreq.Format( _T( "%s"), m_cmXml.GetData( pXmlElement));
			if (_ttol( m_csPrologFreq) <= 0)
			{
				m_csPrologFreq.Format( _T( "%lu"), DEFAULT_PROLOG_FREQ);
			}
		}
	}
}

CPrologResponse::~CPrologResponse()
{
}


BOOL CPrologResponse::isActivatedOption(CString option)
{
	TiXmlElement *pXmlReply, 
				 *pXmlOption,
				 *pXmlElement;
	m_cmXml.ResetPos();
	// Search REPLY node under document node
	pXmlReply = m_cmXml.FindFirstElem( _T( "REPLY"));
	// Search OPTION node under REPLY node
	pXmlOption = m_cmXml.FindFirstElem( _T( "OPTION"), pXmlReply);
	while( pXmlOption)
	{
		pXmlElement = m_cmXml.FindFirstElem( _T( "NAME"), pXmlOption);
		if (option.CompareNoCase(m_cmXml.GetData( pXmlElement)) == 0)
			return TRUE;
		// Search next OPTION node
		pXmlOption = m_cmXml.FindNextElem( _T( "OPTION"), pXmlOption);
	}
	return FALSE;
}

LPCTSTR CPrologResponse::getPrologFreq()
{
	return m_csPrologFreq;
}

BOOL CPrologResponse::isInventoryRequired()
{
	return m_bInventoryRequired;
}

BOOL CPrologResponse::isRegistryRequired()
{
	return isActivatedOption( _T( "REGISTRY"));
}

CMapStringToString* CPrologResponse::getRegistryParameters()
{
	return getOptionAttributes( _T( "REGISTRY"), _T( "REGTREE"), _T( "REGKEY"), _T( "NAME"), _T( ""));
}

BOOL CPrologResponse::isIpdiscoverRequired()
{
	return isActivatedOption( _T( "IPDISCOVER"));
}

CString CPrologResponse::getIpdiscoverLatency()
{
	CMapStringToString* cmParams;
	CString csLatency;

	cmParams = getOptionAttributes( _T( "IPDISCOVER"), _T( "IPDISC_LAT"), _T( ""));
	cmParams->Lookup( _T( "IPDISC_LAT"), csLatency);
	if(cmParams)
		delete cmParams;
	return csLatency;
}

CString	CPrologResponse::getIpdiscoverLan()
{
	CMapStringToString* pParams;
	CString csLan;

	pParams = getOptionAttributes( _T( "IPDISCOVER"), _T( "IPDISC_LAT"), _T( ""));
	pParams->Lookup( _T( "VAL"), csLan);
	if(pParams)
		delete [] pParams;
	return csLan;
}

BOOL CPrologResponse::isDownloadRequired()
{
	return isActivatedOption( _T( "DOWNLOAD"));
}

CMapStringToString* CPrologResponse::getDownloadPackages()
{
	return getOptionAttributes( _T( "DOWNLOAD"), _T( "ID"), _T( "CERT_FILE"), _T( "CERT_PATH"), _T( "PACK_LOC"), _T( "INFO_LOC"), _T( ""));
}

CMapStringToString* CPrologResponse::getDownloadParameters()
{
	return getOptionAttributes( _T( "DOWNLOAD"), _T( "FRAG_LATENCY"), _T( "PERIOD_LATENCY"), _T( "CYCLE_LATENCY"), _T( "PERIOD_LENGTH"), _T( "TIMEOUT"), _T( "ON"), _T( ""));
}

CMapStringToString* CPrologResponse::getOptionAttributes(CString option,...)
{
	CString csOptionName = option;
	CString csAttribute;
	CString csValue;
	BOOL bValid;
	TiXmlElement *pXmlReply, 
				 *pXmlOption,
				 *pXmlElement;

	// Go to response content
	m_cmXml.ResetPos();
	pXmlReply = m_cmXml.FindFirstElem( _T( "REPLY"));

	CMapStringToString *pMap = new CMapStringToString[MAX_OPTION_PARAMETERS];

	DWORD dwParameterNumber = 0;
	
	// Parsing options
	pXmlOption = m_cmXml.FindFirstElem( _T( "OPTION"), pXmlReply);
	while (pXmlOption)
	{
		pXmlElement = m_cmXml.FindFirstElem( _T( "NAME"), pXmlOption);
		
		// Finding the rigth option
		if( csOptionName.CompareNoCase( m_cmXml.GetData( pXmlElement)) != 0 )
		{
			pXmlOption = m_cmXml.FindNextElem( _T( "OPTION"), pXmlOption);
			continue;
		}		
		
		// Finally retrieving the paramaters for the option
		pXmlElement = m_cmXml.FindFirstElem( _T( "PARAM"), pXmlOption);
		while (pXmlElement)
		{
			bValid = FALSE;
			if(dwParameterNumber>=MAX_OPTION_PARAMETERS)
				break;

			// "VAL" is the effective xml tag value
			pMap[dwParameterNumber].SetAt( _T( "VAL"), m_cmXml.GetData( pXmlElement));
			
			// Now we retrieve the xml attributes for the option (asked on the stack)
			va_list marker;
			va_start( marker,option  );     /* Initialize variable arguments. */

			do
			{				
				csAttribute = va_arg( marker, LPCTSTR);
				if (csAttribute !=  _T( ""))
				{
					csValue = m_cmXml.GetAttrib( csAttribute);
					// If an attribute is NULL, we do not feed the cmap array with it
					if (csValue != _T( ""))
					{
						pMap[dwParameterNumber].SetAt( csAttribute, csValue);
						bValid = TRUE;
					}
				}
			}
			while (csAttribute != _T( ""));
			va_end( marker );
			// If all asked attibutes are NULL, we do not store element
			if (bValid)
				dwParameterNumber++;
			else
				pMap[dwParameterNumber].RemoveAll();
			pXmlElement = m_cmXml.FindNextElem( _T( "PARAM"), pXmlElement);
		}
		pXmlOption = m_cmXml.FindNextElem( _T( "OPTION"), pXmlOption);
	}
	return pMap;
}
