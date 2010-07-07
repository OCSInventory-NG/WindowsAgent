//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CapacityAbstract.cpp: implementation of the CCapacityAbstract class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ocsinventory.h"
#include "CapacityAbstract.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCapacityAbstract::CCapacityAbstract()
{
	m_pLogger			= getOcsLogger();
	m_pProlog			= NULL;
	m_pPrologResp		= NULL;
	m_pInventory		= NULL;
	m_pInventoryResp	= NULL;
	m_pParameters		= NULL;
}

CCapacityAbstract::~CCapacityAbstract()
{
	// Free the CMapStringToString array
	if(m_pParameters!=NULL)
	{
		delete [] m_pParameters;
	}
	m_pParameters=NULL;
}

// Intialization
void CCapacityAbstract::setProlog( CPrologRequest *pProlog)
{
	m_pProlog = pProlog;
}

void CCapacityAbstract::setPrologResp( CPrologResponse *pPrologResp)
{
	m_pPrologResp = pPrologResp;
}

void CCapacityAbstract::setInventory( CInventoryRequest *pInventory)
{
	m_pInventory = pInventory;
}

void CCapacityAbstract::setInventoryResp( CInventoryResponse *pInventoryResponse)
{
	m_pInventoryResp = pInventoryResponse;
}
