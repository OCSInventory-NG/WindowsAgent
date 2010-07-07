//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// VideoAdapter.cpp: implementation of the CVideoAdapter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoAdapter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVideoAdapter::CVideoAdapter()
{
	Clear();
}

CVideoAdapter::~CVideoAdapter()
{
	Clear();
}

void CVideoAdapter::SetName( LPCTSTR lpstrDescription)
{
	m_csName = lpstrDescription;
	StrForSQL( m_csName);
}

void CVideoAdapter::SetChipset( LPCTSTR lpstrChipset)
{
	m_csChipset = lpstrChipset;
	StrForSQL( m_csChipset);
}

void CVideoAdapter::SetMemory( LPCTSTR lpstrMemory)
{
	m_csMemory = lpstrMemory;
	StrForSQL( m_csMemory);
}

void CVideoAdapter::SetMemory( LONG lMemory)
{
	if (lMemory == -1)
		m_csMemory = NOT_AVAILABLE;
	else
		m_csMemory.Format( _T( "%lu"), lMemory/ONE_MEGABYTE);
	StrForSQL( m_csMemory);
}

void CVideoAdapter::SetScreenResolution( LPCTSTR lpstrResolution)
{
	m_csResolution = lpstrResolution;
	StrForSQL( m_csResolution);
}

LPCTSTR CVideoAdapter::GetName()
{
	return m_csName;
}

LPCTSTR CVideoAdapter::GetChipset()
{
	return m_csChipset;
}

LPCTSTR CVideoAdapter::GetMemory()
{
	return m_csMemory;
}

LPCTSTR CVideoAdapter::GetScreenResolution()
{
	return m_csResolution;
}

void CVideoAdapter::Clear()
{
	m_csName.Empty();		
	m_csChipset.Empty();	
	m_csMemory.Empty();		
	m_csResolution.Empty();	
}

int CVideoAdapter::operator==( CVideoAdapter cObject) const
{
	return ((m_csName == cObject.GetName()) &&
		(m_csChipset == cObject.GetChipset()) &&
		(m_csMemory == cObject.GetMemory()) &&
		(m_csResolution == cObject.GetScreenResolution()));
}
