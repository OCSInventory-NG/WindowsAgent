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

	// Get RESPONSE to know if inventory needed
	m_cmXml.ResetPos();
	if (m_cmXml.FindElem( _T( "REPLY")))
	{
		m_cmXml.IntoElem();
		if (m_cmXml.FindElem( _T( "RESPONSE")))
			// If SEND asked, inventory needed
			m_bInventoryRequired = (m_cmXml.GetData().CompareNoCase( _T( "SEND")) == 0);
		// Get PROLOG_FREQ (if negative or 0, set it to default)
		m_cmXml.ResetChildPos();
		if (m_cmXml.FindElem( _T( "PROLOG_FREQ")))
		{
			// 
			m_csPrologFreq = m_cmXml.GetData();
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
	m_cmXml.ResetPos();
	m_cmXml.FindElem( _T( "REPLY"));
	m_cmXml.IntoElem();
	while(m_cmXml.FindElem( _T( "OPTION")))
	{
		m_cmXml.IntoElem();
		m_cmXml.FindElem( _T( "NAME"));
		if (option.CompareNoCase(m_cmXml.GetData()) == 0)
			return TRUE;
		m_cmXml.OutOfElem();			
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
	CString csAttrib = option;
	CString csCheck;
	BOOL bValid = FALSE;

	// Go to response content
	m_cmXml.ResetPos();
	m_cmXml.FindElem( _T( "REPLY"));
	m_cmXml.IntoElem();


	CMapStringToString *pMap = new CMapStringToString[MAX_OPTION_PARAMETERS];

	DWORD dwParameterNumber = 0;
	
	// Parsing options
	while (m_cmXml.FindElem( _T( "OPTION")))
	{
		m_cmXml.IntoElem();
		m_cmXml.FindElem( _T( "NAME"));
		
		// Finding the rigth option
		if( option.CompareNoCase(m_cmXml.GetData()) != 0 )
		{
			m_cmXml.OutOfElem();
			continue;
		}		
		
		// Finally retireving the paramaters for the option
		while(m_cmXml.FindElem( _T( "PARAM")))
		{
			if(dwParameterNumber>=MAX_OPTION_PARAMETERS)
				break;

			// "VAL" is the effective xml tag value
			pMap[dwParameterNumber].SetAt( _T( "VAL"), m_cmXml.GetData());
			
			// Now we retrieve the xml attributes for the option (asked on the stack)
			va_list marker;
			va_start( marker,option  );     /* Initialize variable arguments. */

			if (csAttrib !=  _T( ""))
			{
				do
				{				
					csAttrib = va_arg( marker, LPCTSTR);
					if (csAttrib !=  _T( ""))
						pMap[dwParameterNumber].SetAt( csAttrib, m_cmXml.GetAttrib(csAttrib));
					// If an attribute is NULL, we do not feed the cmap array with it
					pMap[dwParameterNumber].Lookup( csAttrib, csCheck);
					if (csCheck !=  _T( ""))
						bValid = TRUE;
					else
					{
						bValid=FALSE;
						break;
					}
				}
				while( csAttrib != "" );
			}
			va_end( marker );

			if (bValid)
				dwParameterNumber++;
			else
			{
				pMap[dwParameterNumber].RemoveAll();
			}
		}
		m_cmXml.OutOfElem();
	}
	return pMap;
}
