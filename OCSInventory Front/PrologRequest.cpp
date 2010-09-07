//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// PrologRequest.cpp: implementation of the CPrologRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PrologRequest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrologRequest::CPrologRequest()
{
//	CDeviceid *pDeviceid = CDeviceid::getInstance();
	CDeviceid *pDeviceid = getOcsDeviceID();

	m_cmXml.SetDoc(XML_HEADERS);
	m_cmXml.AddElem( _T( "REQUEST"));
	m_cmXml.IntoElem();
	m_cmXml.AddElem( _T( "QUERY"), _T( "PROLOG"));
	m_cmXml.AddElem( _T( "DEVICEID"),pDeviceid->getDeviceID());
	m_cmXml.ResetPos();
}

CPrologRequest::~CPrologRequest()
{

}

void CPrologRequest::setSuccess()
{

}
